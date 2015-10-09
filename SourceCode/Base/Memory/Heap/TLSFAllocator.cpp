/*
=============================================================================
	File:	TLSFAllocator.cpp
	Desc:	Heap memory manager suitable for medium size memory allocations.
	Note:	Based on Two Levels Segregate Fit memory allocator (TLSF);
			all functions have been made to allocate 16-byte aligned memory blocks.

			See:
			http://webkit.sed.hu/blog/20100324/war-allocators-tlsf-action
			http://tlsf.baisoku.org/

			TLSF (two level segregated fit) is a relatively new memory
			allocator designed for embedded systems. It boasts constant
			time O(1) malloc/free response time and a 4-byte block
			overhead. Though it typically is slightly slower than other
			allocators such as dlmalloc, it has no worst-case behavior.

			The original implementation, which comes alongside the white
			paper, is distributed under the GNU GPL/LGPL. The code found
			here is an original implementation, released into the public
			domain, therefore is not subject to any licensing restrictions.

			Features 
				O(1) cost for malloc, free, realloc, memalign
				Extremely low overhead per allocation (4 bytes)
				Low overhead per pool (~3kB)
				Low fragmentation
				Compiles to only a few kB of code and data
				Caveats
				Currently, only supports 32-bit architectures
				Currently, assumes architecture can make 4-byte aligned accesses
				Not designed to be thread safe; the user must provide this
			Known Issues
				Due to the internal block structure size and the implementation
				details of tlsf_memalign, there is worst-case behavior when requesting
				small (<16 byte) blocks aligned to 8-byte boundaries. Overuse of memalign
				will generally increase fragmentation, but this particular case will leave
				lots of unusable "holes" in the heap. The solution would be to internally
				align all blocks to 8 bytes, but this will require significant changes
				to the implementation.

=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include "TLSF/tlsf.h"
#include "TLSFAllocator.h"

mxSWIPED("OOOII");
#define oSAFESTR(str) ((str) ? (str) : "")
#define oSAFESTRN(str) ((str) ? (str) : "(null)")
#define oSTRNEXISTS(str) (str && str[0] != '\0')


#define oSetLastError(...)	mxDEBUG_BREAK


/**************************************************************************
 * The MIT License                                                        *
 * Copyright (c) 2011 Antony Arciuolo & Kevin Myers                       *
 *                                                                        *
 * Permission is hereby granted, free of charge, to any person obtaining  *
 * a copy of this software and associated documentation files (the        *
 * "Software"), to deal in the Software without restriction, including    *
 * without limitation the rights to use, copy, modify, merge, publish,    *
 * distribute, sublicense, and/or sell copies of the Software, and to     *
 * permit persons to whom the Software is furnished to do so, subject to  *
 * the following conditions:                                              *
 *                                                                        *
 * The above copyright notice and this permission notice shall be         *
 * included in all copies or substantial portions of the Software.        *
 *                                                                        *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                  *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE *
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  *
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *
 **************************************************************************/

AllocatorTLSF_Impl::AllocatorTLSF_Impl(const char* _DebugName, const DESC* _pDesc, bool* _pSuccess)
: Desc(*_pDesc)
{
	*_pSuccess = false;
	strcpy_s(DebugName, oSAFESTRN(_DebugName));
	AllocatorTLSF_Impl::Reset();
	if (!AllocatorTLSF_Impl::IsValid())
	{
		oSetLastError(EINVAL, "Failed to construct TLSF allocator");
		return;
	}

	*_pSuccess = true;
}

void TraceAllocated(void* ptr, size_t size, int used, void* user)
{
	if (used)
	{
		char mem[64];
		oFormatMemorySize(mem, size, 2);
		oTRACE("TLSF LEAK %s: 0x%p %s", (const char*)user, ptr, mem);
	}
}

AllocatorTLSF_Impl::~AllocatorTLSF_Impl()
{
	if (Stats.NumAllocations != 0)
		tlsf_walk_heap(hPool, TraceAllocated, DebugName);

	//AssertX(Stats.NumAllocations == 0, "Allocator %s being destroyed with %u allocations still unfreed! This may leave dangling pointers. See trace for addresses.", DebugName, Stats.NumAllocations);

	AssertX(AllocatorTLSF_Impl::IsValid(), "TLSF Heap is corrupt");
	tlsf_destroy(hPool);
}

bool oAllocatorTLSF::Create(const char* _DebugName, const DESC* _pDesc, oAllocator** _ppAllocator)
{
	if (!_pDesc || !_ppAllocator || !_pDesc->pArena || !_pDesc->ArenaSize)
	{
		oSetLastError(EINVAL);
		return false;
	}

	bool success = false;
	oCONSTRUCT_PLACEMENT(_ppAllocator, _pDesc->pArena, AllocatorTLSF_Impl(_DebugName, _pDesc, &success));

	return true;
}

void AllocatorTLSF_Impl::GetDesc(DESC* _pDesc)
{
	*_pDesc = Desc;
}

void AllocatorTLSF_Impl::GetStats(STATS* _pStats)
{
	*_pStats = Stats;
}

const char* AllocatorTLSF_Impl::GetDebugName() const threadsafe
{
	return thread_cast<const char*>(DebugName);
}

const char* AllocatorTLSF_Impl::GetType() const threadsafe
{
	return "TLSF";
}

bool AllocatorTLSF_Impl::IsValid()
{
	return hPool && !tlsf_check_heap(hPool);
}

void* AllocatorTLSF_Impl::Allocate(size_t _NumBytes, size_t _Alignment)
{
	void* p = tlsf_memalign(hPool, _Alignment, __max(_NumBytes, 1));
	if (p)
	{
		size_t blockSize = tlsf_block_size(p);
		Stats.NumAllocations++;
		Stats.BytesAllocated += blockSize;
		Stats.BytesFree -= blockSize;
		Stats.PeakBytesAllocated = __max(Stats.PeakBytesAllocated, Stats.BytesAllocated);
	}

	return p;
}

void* AllocatorTLSF_Impl::Reallocate(void* _Pointer, size_t _NumBytes)
{
	size_t oldBlockSize = _Pointer ? tlsf_block_size(_Pointer) : 0;
	void* p = tlsf_realloc(hPool, _Pointer, _NumBytes);
	if (p)
	{
		size_t blockSizeDiff = tlsf_block_size(p) - oldBlockSize;
		Stats.BytesAllocated += blockSizeDiff;
		Stats.BytesFree -= blockSizeDiff;
		Stats.PeakBytesAllocated = __max(Stats.PeakBytesAllocated, Stats.BytesAllocated);
	}

	return 0;
}

void AllocatorTLSF_Impl::Deallocate(void* _Pointer)
{
	if (_Pointer)
	{
		size_t blockSize = tlsf_block_size(_Pointer);
		tlsf_free(hPool, _Pointer);
		Stats.NumAllocations--;
		Stats.BytesAllocated -= blockSize;
		Stats.BytesFree += blockSize;
	}
}

size_t AllocatorTLSF_Impl::GetBlockSize(void* _Pointer)
{
	return tlsf_block_size(_Pointer);
}

void AllocatorTLSF_Impl::Reset()
{
	memset(&Stats, 0, sizeof(Stats));
	void* pRealArenaStart = oByteAlign(oByteAdd(Desc.pArena, sizeof(*this)), DEFAULT_MEMORY_ALIGNMENT);
	size_t realArenaSize = Desc.ArenaSize - std::distance((char*)Desc.pArena, (char*)pRealArenaStart);
	hPool = tlsf_create(pRealArenaStart, realArenaSize);
	Stats.NumAllocations = 0;
	Stats.BytesAllocated = 0;
	Stats.PeakBytesAllocated = 0;
	Stats.BytesFree = Desc.ArenaSize - tlsf_overhead();
}

struct TLSFContext
{
	oAllocator::WalkerFn Walker;
	void* pUserData;
	long Flags;
};

static void TLSFWalker(void* ptr, size_t size, int used, void* user)
{
	TLSFContext* ctx = (TLSFContext*)user;
	oAllocator::BLOCK_DESC b;
	b.Address = ptr;
	b.Size = size;
	b.Used = !!used;
	ctx->Walker(&b, ctx->pUserData, ctx->Flags);
}

void AllocatorTLSF_Impl::WalkHeap(WalkerFn _Walker, void* _pUserData, long _Flags)
{
	TLSFContext ctx;
	ctx.Walker = _Walker;
	ctx.pUserData = _pUserData;
	ctx.Flags = _Flags;
	tlsf_walk_heap(hPool, TLSFWalker, &ctx);
}

