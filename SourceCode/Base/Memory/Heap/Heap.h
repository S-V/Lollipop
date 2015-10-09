/*
=============================================================================
	File:	Heap.h
	Desc:	
=============================================================================
*/
#pragma once

mxSWIPED("OOOII, file: oHeap.h");

namespace oHeap
{
	enum TYPE
	{
		STACK, // the stack
		SYSTEM, // true underlying OS allocator
		LIBC, // libc's malloc
		STATIC, // heap shared by DLLs/modules, but unique per-process
		EXTERNAL, // a sibling or parent of libc's malloc, not visible to vanilla c api
		INTERNAL, // a user allocator that uses a heap allocated from the system allocator
		MAPPED, // memory that represents memory in another process or on another medium
	};

	struct BLOCK_DESC
	{
		void* Address;
		unsigned long long Size;
		unsigned long long Overhead;
		bool Used;
	};

	struct DESC
	{
		char Name[64];
		char Description[64];
		TYPE Type;
	};

	struct STATISTICS
	{
		unsigned long long BlocksFree;
		unsigned long long BlocksUsed;
		unsigned long long Blocks;
		unsigned long long BytesFree;
		unsigned long long BytesUsed;
		unsigned long long Bytes;
		unsigned long long Overhead;
		unsigned long long BlocksSmall;
		unsigned long long BlocksMedium;
		unsigned long long BlocksLarge;
		bool Valid;
	};

	struct GLOBAL_HEAP_DESC
	{
		unsigned long long TotalMemoryUsed;
		unsigned long long AvailablePhysical;
		unsigned long long TotalPhysical;
		unsigned long long AvailableVirtualProcess;
		unsigned long long TotalVirtualProcess;
		unsigned long long AvailablePaged;
		unsigned long long TotalPaged;
	};

	typedef void (*WalkerFn)(const BLOCK_DESC* pDesc, void* user, long flags);
	typedef void (*DescriberFn)(const DESC* pDesc, void* user, long flags);
	typedef void (*SummarizerFn)(const STATISTICS* pStats, void* user, long flags);

	static const unsigned long long SMALL_BLOCK_SIZE = 480;
	static const unsigned long long LARGE_BLOCK_SIZE = 2 * 1024 * 1024;
	// a medium-sized block is anything in between small and large

	inline bool IsSmall(unsigned long long _Size) { return _Size <= SMALL_BLOCK_SIZE; }
	inline bool IsMedium(unsigned long long _Size) { return _Size > SMALL_BLOCK_SIZE && _Size <= LARGE_BLOCK_SIZE; }
	inline bool IsLarge(unsigned long long _Size) { return _Size > LARGE_BLOCK_SIZE; }

	// Sums the specified BLOCK_DESC into the specified STATS
	void Add(STATISTICS* _pStats, const BLOCK_DESC* _pDesc);

	bool WalkExternals(WalkerFn _Walker, DescriberFn _Describer, SummarizerFn _Summarizer, void* _pUserData, long _Flags = 0);
	void SetSmallBlockHeapThreshold(unsigned long long _sbhMaxSize);

	void GetGlobalDesc(GLOBAL_HEAP_DESC* _pDesc);

	unsigned int GetSystemAllocationGranularity();
} // namespace oHeap
