/*
=============================================================================
	File:	Win32_Memory.cpp
	Desc:
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

mxSWIPED("Nebula 3");
//------------------------------------------------------------------------------
/**
    Helper function for Heap16 functions: aligns pointer to 16 byte and 
    writes padding mask to byte before returned pointer.
*/
__forceinline unsigned char*
__HeapAlignPointerAndWritePadding16(unsigned char* ptr)
{
    unsigned char paddingMask = DWORD(ptr) & 15;
    ptr = (unsigned char*)(DWORD(ptr + 16) & ~15);
    ptr[-1] = paddingMask;
    return ptr;
}

//------------------------------------------------------------------------------
/**
    Helper function for Heap16 functions: "un-aligns" pointer through
    the padding mask stored in the byte before the pointer.
*/
__forceinline unsigned char*
__HeapUnalignPointer16(unsigned char* ptr)
{
    return (unsigned char*)(DWORD(ptr - 16) | ptr[-1]);
}

//------------------------------------------------------------------------------
/**
    HeapAlloc replacement which always returns 16-byte aligned addresses.

    NOTE: only works for 32 bit pointers!
*/
__forceinline LPVOID 
__HeapAlloc16(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    #if __XBOX360__
    return ::HeapAlloc(hHeap, dwFlags, dwBytes);
    #else
    unsigned char* ptr = (unsigned char*) ::HeapAlloc(hHeap, dwFlags, dwBytes + 16);
    ptr = __HeapAlignPointerAndWritePadding16(ptr);
    return (LPVOID) ptr;
    #endif
}

//------------------------------------------------------------------------------
/**
    HeapReAlloc replacement for 16-byte alignment.

    NOTE: only works for 32 bit pointers!
*/
__forceinline LPVOID
__HeapReAlloc16(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
    #if __XBOX360__
    return HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
    #else
    // restore unaligned pointer
    unsigned char* ptr = (unsigned char*) lpMem;
    unsigned char* rawPtr = __HeapUnalignPointer16(ptr); 

    // perform re-alloc, NOTE: if re-allocation can't happen in-place,
    // we need to handle the allocation ourselves, in order not to destroy 
    // the original data because of different alignment!!!
    ptr = (unsigned char*) ::HeapReAlloc(hHeap, (dwFlags | HEAP_REALLOC_IN_PLACE_ONLY), rawPtr, dwBytes + 16);
    if (0 == ptr)
    {                   
        DWORD rawSize = ::HeapSize(hHeap, dwFlags, rawPtr);
        mxASSERT(dwBytes + 16 >= rawSize);
        // re-allocate manually because padding may be different!
        ptr = (unsigned char*) ::HeapAlloc(hHeap, dwFlags, dwBytes + 16);
        ptr = __HeapAlignPointerAndWritePadding16(ptr);
        ::CopyMemory(ptr, lpMem, rawSize - 16);    
        // release old mem block
        ::HeapFree(hHeap, dwFlags, rawPtr);
    }
    else
    {
        // was re-allocated in place
        ptr = __HeapAlignPointerAndWritePadding16(ptr);
    }
    return (LPVOID) ptr;
    #endif
}

//------------------------------------------------------------------------------
/**
    HeapFree replacement which always returns 16-byte aligned addresses.

    NOTE: only works for 32 bit pointers!
*/
__forceinline BOOL
__HeapFree16(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    #if __XBOX360__
    return ::HeapFree(hHeap, dwFlags, lpMem);
    #else
    unsigned char* ptr = (unsigned char*) lpMem;
    ptr = __HeapUnalignPointer16(ptr);
    return ::HeapFree(hHeap, dwFlags, ptr);
    #endif
}

//------------------------------------------------------------------------------
/**
    HeapSize replacement function.
*/
__forceinline SIZE_T
__HeapSize16(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
    #if __XBOX360__
    return ::HeapSize(hHeap, dwFlags, lpMem);
    #else
    unsigned char* ptr = (unsigned char*) lpMem;
    ptr = __HeapUnalignPointer16(ptr);
    return ::HeapSize(hHeap, dwFlags, ptr);
    #endif
}    

//
//	mxMemoryHeap_Win32
//
class mxMemoryHeap_Win32
{
	HANDLE heap;

public:

	mxMemoryHeap_Win32()
	{
		this->heap = NULL;
	}

	bool Setup( SizeT initialSize = 0, SizeT maxSize = 0 )
	{
		DWORD options = 0;

		// The flag HEAP_NO_SERIALIZE is used to disable the use of synchronization on this new heap, since only a single thread will access it.

		// Creates a private heap object that can be used by the calling process.
		// The function reserves space in the virtual address space of the process
		// and allocates physical storage for a specified initial portion of this block.

		this->heap = ::HeapCreate(
			options,
			initialSize,	// The initial size of the heap, in bytes. This value determines the initial amount of memory that is committed for the heap.
			maxSize			// The maximum size of the heap, in bytes. If dwMaximumSize is 0, the heap can grow in size. The heap's size is limited only by the available memory.
		);
		mxASSERT_NZ( this->heap );
		if( NULL == this->heap )
		{
			mxMsgBoxf(
				("Error"),
				("Failed to create a new heap with LastError %d.\n"),
				GetLastError()
			);
			return false;
		}

		//
		// Enable the low-fragmenation heap (LFH). Starting with Windows Vista, 
		// the LFH is enabled by default but this call does not cause an error.
		//
		
		// To enable the LFH for the specified heap,
		// set the variable pointed to by the HeapInformation parameter to 2.
		// After the LFH is enabled for a heap, it cannot be disabled.

		ULONG heapFragValue = 2;	// HEAP_LFH

		// The LFH cannot be enabled for heaps created with HEAP_NO_SERIALIZE
		// or for heaps created with a fixed size.

		::HeapSetInformation(
			this->heap,
			HeapCompatibilityInformation,
			&heapFragValue,
			sizeof(heapFragValue)
		);

		return true;
	}

	void Destroy()
	{
		BOOL success = ::HeapDestroy( this->heap );
		mxASSERT_NZ( success );
	}

	void* Alloc( size_t size )
	{
		void* ptr = __HeapAlloc16(this->heap, HEAP_GENERATE_EXCEPTIONS, size);
		return ptr;
	}
	void* Realloc(void* ptr, size_t size)
	{
		void* newPtr = __HeapReAlloc16(this->heap, HEAP_GENERATE_EXCEPTIONS, ptr, size);
		return newPtr;
	}
	void Free(void* ptr)
	{
		mxASSERT_NZ( ptr );
		BOOL success = __HeapFree16(this->heap, 0, ptr);
		mxASSERT_NZ( success );
	}

	/**
	Validate the heap. This walks over the heap's memory block and checks
	the control structures. If somethings wrong the function will
	stop the program, otherwise the functions returns true.
	*/
	bool ValidateHeap() const
	{
		BOOL result = ::HeapValidate(
			this->heap,
			0,		// The heap access options.
			NULL	// A pointer to a memory block within the specified heap.
		);
		// If the specified heap or memory block is valid, the return value is nonzero.
		return (FALSE != result);
	}

	mxSWIPED("Nebula 3");
	/**
		This is a helper method which walks a Windows heap and writes a
		log entry per allocated memory block to DebugOut.
	*/
	void DumpMemoryLeaks( const char* heapName, HANDLE hHeap )
	{
		PROCESS_HEAP_ENTRY walkEntry = { 0 };
		::HeapLock( hHeap );
		bool heapMsgShown = false;
		while( ::HeapWalk(hHeap, &walkEntry) )
		{
			if (walkEntry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
			{
				char strBuf[256];
				if( !heapMsgShown )
				{
					_snprintf( strBuf, sizeof(strBuf), "!!! HEAP MEMORY LEAKS !!! (Heap: %s)\n", heapName );
					::OutputDebugStringA( strBuf );
					heapMsgShown = true;
				}
				_snprintf( strBuf, sizeof(strBuf), "addr(0x%0lx) size(%d)\n", (size_t) walkEntry.lpData, walkEntry.cbData );
				::OutputDebugStringA( strBuf );
			}
		}
		::HeapUnlock(hHeap);
	}
};


int memiszero(BYTE const *p, unsigned long n) {
	unsigned int z = 0;
	BYTE const *t;

	IF_UNLIKELY(n < 8) {
		goto bytes_loop;
	}
	t = (BYTE const *) (((unsigned long) p + n) & ~7UL);
	n = ((unsigned long) p + n) & 7;
	switch ((unsigned long) p & 7) {
	case 1:
		z |= *(U1 const *) p++;
	case 2:
		z |= *(U1 const *) p++;
	case 3:
		z |= *(U1 const *) p++;
	case 4:
		z |= *(U1 const *) p++;
	case 5:
		z |= *(U1 const *) p++;
	case 6:
		z |= *(U1 const *) p++;
	case 7:
		z |= *(U1 const *) p++;
		if (z)
			return 0;
	}
	for (; p < t; p += 8)
		if (*(U1 const *) p)
			return 0;
bytes_loop:
	switch (n) {
	case 7:
		z |= *(U1 const *) p++;
	case 6:
		z |= *(U1 const *) p++;
	case 5:
		z |= *(U1 const *) p++;
	case 4:
		z |= *(U1 const *) p++;
	case 3:
		z |= *(U1 const *) p++;
	case 2:
		z |= *(U1 const *) p++;
	case 1:
		z |= *(U1 const *) p++;
	}
	return !z;
}

int memiszero_32(BYTE const *p, unsigned long n) {
	unsigned int z = 0;
	BYTE const *t;

	IF_UNLIKELY(n < 4) {
		goto bytes_loop;
	}
	t = (BYTE const *) (((unsigned long) p + n) & ~3UL);
	n = ((unsigned long) p + n) & 3;
	switch ((unsigned long) p & 3) {
	case 1:
		z |= *(U1 const *) p++;
	case 2:
		z |= *(U1 const *) p++;
	case 3:
		z |= *(U1 const *) p++;
		if (z)
			return 0;
	}
	for (; p < t; p += 4)
		if (*(U4 const *) p)
			return 0;
bytes_loop:
	switch (n) {
	case 3:
		z |= *(U1 const *) p++;
	case 2:
		z |= *(U1 const *) p++;
	case 1:
		z |= *(U1 const *) p++;
	}
	return !z;
}

int memiszero_loop(BYTE const *p, unsigned long n) {
	BYTE const *t;

	t = p + n;
	for (; p < t; p++)
		if (*(U1 const *) p)
			return 0;
	return 1;
}



namespace MemAlignUtil
{
	SizeT GetAlignedMemSize( SizeT nBytes )
	{
		enum { alignment = 16 };	// 16 bytes

		// we need to store a pointer to original (unaligned) memory block
		// so that we can free() it later
		// we can store it in the allocated memory block

		// Allocate a bigger buffer for alignment purpose,
		// stores the original allocated address just before the aligned buffer for a later call to free().

		const SizeT expandSizeBytes = nBytes + (sizeof(void*) + (alignment-1));

		return expandSizeBytes;
	}

	void* GetAlignedPtr( void* allocatedMem )
	{
		enum { alignment = 16 };	// 16 bytes

		BYTE* rawAddress = c_cast(BYTE*) allocatedMem;
		BYTE* alignedAddress = c_cast(BYTE*) (SizeT(rawAddress + alignment + sizeof(void*)) & ~(alignment-1));

		(c_cast(void**)alignedAddress)[-1] = rawAddress;	// store pointer to original (allocated) memory block

		return alignedAddress;	// return aligned pointer
	}

	void* GetUnalignedPtr( void* alignedPtr )
	{
		// We need a way to go from our aligned pointer to the original pointer.

		BYTE* alignedAddress = c_cast(BYTE*) alignedPtr;

		// To accomplish this, we store a pointer to the memory returned by malloc
		// immediately preceding the start of our aligned memory block.

		BYTE* rawAddress = c_cast(BYTE*) (c_cast(void**)alignedAddress)[-1];

		return rawAddress;
	}
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
