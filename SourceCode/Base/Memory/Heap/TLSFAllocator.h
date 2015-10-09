/*
=============================================================================
	File:	TLSFAllocator.h
	Desc:	Heap memory manager suitable for medium size memory allocations.
	Note:	Based on Two Levels Segregate Fit memory allocator (TLSF);
			all functions have been made to allocate 16-byte aligned memory blocks.
=============================================================================
*/
#pragma once


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

#if 0
interface oInterface
{
	// Returns the new refcount. NOTE: This can be negative, meaning the refcount
	// is invalid. It probably will be a very negative number, not -1 or -2, so
	// if testing for validity, only test for < 0. The refcount should never be
	// 0 either because the count is atomically invalidated so that no quick
	// ref/rel's happen around a 0->1 transition. Generally the user will not need
	// to check this value and should MAKE ALL ATTEMPTS to avoid using this value,
	// as the assert below in intrusive_ptr_add_ref() indicates. However there are
	// rare cases such as the autolist pattern for resource objects that can be
	// streaming in from disk, being rendered by dedicated hardware AND going
	// through the regular mult-threaded user lifetime management where it is
	// possible to query a container for said resource and it could come out that
	// it's been marked for a deferred garbage collection. In that very
	// specialized case it seems that doing an extra check on the single indicator
	// of an object's validity is worth doing.
	virtual int Reference() threadsafe = 0;
	virtual void Release() threadsafe = 0;

	virtual bool QueryInterface(const oGUID& _InterfaceID, threadsafe void** _ppInterface) threadsafe = 0;

	template<typename T> inline bool QueryInterface(const oGUID& _InterfaceID, T** _ppInterface) threadsafe { T* pInterface = 0; *_ppInterface = (QueryInterface(_InterfaceID, (threadsafe void**)&pInterface) ? pInterface : 0); return !!*_ppInterface; }
	//template<typename T> inline bool QueryInterface(const oGUID& _InterfaceID, threadsafe T** _ppInterface) threadsafe { T* pInterface = 0; *_ppInterface = (QueryInterface(_InterfaceID, (void**)&pInterface) ? pInterface : 0); return !!*_ppInterface; }
	template<typename T> inline bool QueryInterface( T** _ppInterface) threadsafe { return QueryInterface( oGetGUID( _ppInterface ), _ppInterface ); }
	//template<typename T> inline bool QueryInterface( threadsafe T** _ppInterface) threadsafe { return QueryInterface( oGetGUID( _ppInterface ), _ppInterface ); }
};

inline void intrusive_ptr_add_ref(threadsafe oInterface* p)
{
#ifdef oENABLE_ASSERTS
	int count =
#endif
		p->Reference();
	oASSERT(count > 0, "ref on invalid object");
}

inline void intrusive_ptr_release(threadsafe oInterface* p) { p->Release(); }

#endif

// _____________________________________________________________________________
// Common DirectX-style patterns

#define oDEFINE_GETINTERFACE_INTERFACE(FnThreadSafety, GetInterfaceFn, TypeThreadSafety, Type, FieldName) void GetInterfaceFn(TypeThreadSafety Type** _ppInterface) FnThreadSafety override { (FieldName)->Reference(); *_ppInterface = (FieldName); }

#define oDEFINE_NOOP_REFCOUNT_INTERFACE() int Reference() threadsafe { return 1; } void Release() threadsafe {}
#define oDEFINE_REFCOUNT_INTERFACE(FieldName) int Reference() threadsafe override { return (FieldName).Reference(); } void Release() threadsafe override { if ((FieldName).Release()) delete this; }
#define oDEFINE_CONST_GETDESC_INTERFACE(FieldName, FnThreadSafety) void GetDesc(DESC* _pDesc) const FnThreadSafety override { *_pDesc = thread_cast<DESC&>(FieldName); }

// Under the MyClass::Create() pattern, usually there's an instantiation of a
// class and assignment to the output pointer. A common pattern of that
// implementation is to have the ctor return a bool success by address. This
// macro encapsulates the common procedure that usually follows of checking
// the pointer itself and success.  It also hooks the memory allocation and
// asserts that the class author is properly setting oSetLastError on failure

#define oCONSTRUCT_NEW(_PointerToInstancePointer, object) *_PointerToInstancePointer = new object
#define oCONSTRUCT_PLACEMENT_NEW(_PointerToInstancePointer, memory, object) *_PointerToInstancePointer = new (memory) object

//#define oCONSTRUCT_BASE_CHECK(_PointerToInstancePointer) if (!*_PointerToInstancePointer) oSetLastError(ENOMEM); else if (!success) { (*_PointerToInstancePointer)->Release(); *_PointerToInstancePointer = 0; }
#define oCONSTRUCT_BASE_CHECK(_PointerToInstancePointer)

#define oCONSTRUCT_BASE(_PointerToInstancePointer, object) oCONSTRUCT_NEW(_PointerToInstancePointer, object); oCONSTRUCT_BASE_CHECK(_PointerToInstancePointer)
#define oCONSTRUCT_PLACEMENT_BASE(_PointerToInstancePointer, memory, object) oCONSTRUCT_PLACEMENT_NEW(_PointerToInstancePointer, memory, object); oCONSTRUCT_BASE_CHECK(_PointerToInstancePointer)

//#ifndef _DEBUG
#define oCONSTRUCT oCONSTRUCT_BASE
#define oCONSTRUCT_PLACEMENT oCONSTRUCT_PLACEMENT_BASE
//#else
//#define oCONSTRUCT(_PointerToInstancePointer, object) \
//	::size_t ErrorCount = oGetLastErrorCount(); \
//	oCONSTRUCT_BASE(_PointerToInstancePointer, object) \
//	oASSERT(success || oGetLastErrorCount() > ErrorCount, "%s failed but didn't call oSetLastError", #object);
//#define oCONSTRUCT_PLACEMENT(_PointerToInstancePointer, memory, object) \
//	::size_t ErrorCount = oGetLastErrorCount(); \
//	oCONSTRUCT_PLACEMENT_BASE(_PointerToInstancePointer, memory, object) \
//	oASSERT(success || oGetLastErrorCount() > ErrorCount, "%s failed but didn't call oSetLastError", #object);
//#endif

// _QI* helper macros are internal, use oDEFINE_TRIVIAL_QUERYINTERFACE*() below
#define _QIPRE() bool QueryInterface(const oGUID& iid, threadsafe void ** _ppInterface) threadsafe override
#define _QIIF(IID) { if (iid == (IID) || iid == oGetGUID<oInterface>())
#define _QIIF2(baseIID, IID) { if (iid == (IID) || iid == (baseIID) || iid == oGetGUID<oInterface>())
#define _QIIF3(baserIID, baseIID, IID) { if (iid == (IID) || iid == (baseIID) || iid == (baserIID) || iid == oGetGUID<oInterface>())
#define _QIIF4(basestIID, baserIID, baseIID, IID) { if (iid == (IID) || iid == (baseIID) || iid == (baserIID) || iid == (basestIID) || iid == oGetGUID<oInterface>())
#define _QIPOST() if (Reference()) *_ppInterface = this; else *_ppInterface = 0; return !!*_ppInterface; }

#define oDEFINE_NOOP_QUERYINTERFACE() _QIPRE() { return false; }
#define oDEFINE_TRIVIAL_QUERYINTERFACE(IID) _QIPRE() _QIIF(IID) _QIPOST()
#define oDEFINE_TRIVIAL_QUERYINTERFACE2(baseIID, IID) _QIPRE() _QIIF2(baseIID, IID) _QIPOST()
#define oDEFINE_TRIVIAL_QUERYINTERFACE3(baserIID, baseIID, IID) _QIPRE() _QIIF3(baserIID, baseIID, IID) _QIPOST()
#define oDEFINE_TRIVIAL_QUERYINTERFACE4(basestIID, baserIID, baseIID, IID) _QIPRE() _QIIF4(basestIID baserIID, baseIID, IID) _QIPOST()


abstract_interface oAllocator //: oInterface
{
	struct DESC
	{
		DESC()
			: pArena(0)
			, ArenaSize(0)
		{}

		void* pArena;
		size_t ArenaSize;
	};

	struct STATS
	{
		size_t NumAllocations;
		size_t BytesAllocated;
		size_t PeakBytesAllocated;
		size_t BytesFree;
	};

	struct BLOCK_DESC
	{
		void* Address;
		size_t Size;
		bool Used;
	};

	virtual void GetDesc(DESC* _pDesc) = 0;
	virtual void GetStats(STATS* _pStats) = 0;
	virtual const char* GetDebugName() const threadsafe = 0;
	virtual const char* GetType() const threadsafe = 0;
	virtual bool IsValid() = 0;

	virtual void* Allocate(size_t _NumBytes, size_t _Alignment = DEFAULT_MEMORY_ALIGNMENT) = 0;
	virtual void* Reallocate(void* _Pointer, size_t _NumBytes) = 0;
	virtual void Deallocate(void* _Pointer) = 0;
	virtual size_t GetBlockSize(void* _Pointer) = 0;

	virtual void Reset() = 0;

	typedef void (*WalkerFn)(const BLOCK_DESC* pDesc, void* _pUser, long _Flags);
	virtual void WalkHeap(WalkerFn _Walker, void* _pUserData, long _Flags = 0) = 0;

	// An alternative for using placement new with memory from this oAllocator.
	// There are several permutations for varying numbers of ctor parameters.
	template<typename T> T* Construct() { void* m = Allocate(sizeof(T)); return new (m) T(); }
	template<typename T, typename U> T* Construct(U u) { void* m = Allocate(sizeof(T)); return new (m) T(u); }
	template<typename T, typename U, typename V> T* Construct(U u, V v) { void* m = Allocate(sizeof(T)); return new (m) T(u, v); }
	template<typename T, typename U, typename V, typename W> T* Construct(U u, V v, W w) { void* m = Allocate(sizeof(T)); return new (m) T(u, v, w); }
	template<typename T, typename U, typename V, typename W, typename X> T* Construct(U u, V v, W w, X x) { void* m = Allocate(sizeof(T)); return new (m) T(u, v, w, x); }
	template<typename T, typename U, typename V, typename W, typename X, typename Y> T* Construct(U u, V v, W w, X x, Y y) { void* m = Allocate(sizeof(T)); return new (m) T(u, v, w, x, y); }
	template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z> T* Construct(U u, V v, W w, X x, Y y, Z z) { void* m = Allocate(sizeof(T)); return new (m) T(u, v, w, x, y, z); }
	template<typename T> void Destroy(T* _Pointer) { _Pointer->~T(); Deallocate((void*)_Pointer); }
};

// oAllocator implementation using the two-level segregated fit
// allocation algorithm. Read more at http://tlsf.baisoku.org/

abstract_interface oAllocatorTLSF : public oAllocator
{
	static bool Create(const char* _DebugName, const DESC* _pDesc, oAllocator** _ppAllocator);
};

struct AllocatorTLSF_Impl : public oAllocatorTLSF
{
	// Always allocate memory for this struct in the arena specified by the user
	void* operator new(size_t _Size) { return 0; }
	void* operator new[](size_t si_Size) { return 0; }
	void operator delete(void* _Pointer) {}
	void operator delete[](void* _Pointer) {}
public:
	void* operator new(size_t _Size, void* _pMemory) { return _pMemory; }
	void operator delete(void* _Pointer, void* _pMemory) {}

	//oDEFINE_REFCOUNT_INTERFACE(RefCount);
	//oDEFINE_TRIVIAL_QUERYINTERFACE(oGetGUID<oAllocatorTLSF>());

	AllocatorTLSF_Impl(const char* _DebugName, const DESC* _pDesc, bool* _pSuccess);
	~AllocatorTLSF_Impl();

	void GetDesc(DESC* _pDesc) override;
	void GetStats(STATS* _pStats) override;
	const char* GetDebugName() const threadsafe override;
	const char* GetType() const threadsafe override;
	bool IsValid() override;
	void* Allocate(size_t _NumBytes, size_t _Alignment = DEFAULT_MEMORY_ALIGNMENT) override;
	void* Reallocate(void* _Pointer, size_t _NumBytes) override;
	void Deallocate(void* _Pointer) override;
	size_t GetBlockSize(void* _Pointer) override;
	void Reset() override;
	void WalkHeap(WalkerFn _Walker, void* _pUserData, long _Flags = 0) override;

	DESC Desc;
	STATS Stats;
	//oRefCount RefCount;
	void* hPool;
	char DebugName[64];
};
