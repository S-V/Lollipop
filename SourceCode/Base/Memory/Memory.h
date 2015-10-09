/*
=============================================================================
	File:	Memory.h
	Desc:	Memory management.
	Note:	It's far from finished.
	ToDo:	Memory limits and watchdogs, thread-safety,
			allow users to define their own OutOfMemory handlers, etc.
=============================================================================
*/

#ifndef __MX_MEMORY_H__
#define __MX_MEMORY_H__

//---------------------------------------------------------------------------
//		Defines.
//---------------------------------------------------------------------------


// 1 - Track each memory allocation/deallocation, record memory usage statistics and detect memory leaks and so on.
#define MX_DEBUG_MEMORY			MX_DEBUG

#define MX_ENABLE_DEBUG_HEAP	(1)

// 1 - Redirect the global 'new' and 'delete' to our memory manager.
// this is dangerous, it may be better to define them on per-type basis or use special macros.
#define MX_OVERRIDE_GLOBAL_NEWDELETE	(0)

// 1 - prevent the programmer from using malloc/free.
#define MX_HIDE_MALLOC_AND_FREE		(0)



//---------------------------------------------------------------------------
//		Declarations.
//---------------------------------------------------------------------------

enum EMemType
{
	STACK, // the stack
	SYSTEM, // true underlying OS allocator
	LIBC, // libc's malloc
	STATIC, // heap shared by DLLs/modules, but unique per-process
	EXTERNAL, // a sibling or parent of libc's malloc, not visible to vanilla c api
	INTERNAL, // a user allocator that uses a heap allocated from the system allocator
	MAPPED, // memory that represents memory in another process or on another medium
};

// use several special memory heaps to reduce heap fragmentation and improve performance
//
//	EMemHeap - a category under which the memory used by objects is placed.
//
//	(Also known as memory heap, memory arena/zone, etc.)
//	Each object in the engine should be allocated in a dedicated heap.
//	The main purpose for the different heap
//	types is to decrease memory fragmentation and to improve cache
//	usage by grouping similar data together. Platform ports may define
//	platform-specific heap types, as long as only platform specific
//	code uses those new heap types.
//
enum EMemHeap
{
#define DECLARE_MEMORY_HEAP( enm, data )	enm
	#include "MemoryHeaps.inl"
#undef DECLARE_MEMORY_HEAP

	HeapCount,	// Marker. Don't use.

	//------------------------------

	// Memory heap used (by global 'new' and 'delete') by default.
	DefaultHeap = HeapGeneric
};

const char* mxGetMemoryHeapName( EMemHeap code );



//
//	mxMemoryManagerInfo - holds a short description of a memory manager.
//
struct mxMemoryManagerInfo
{
	char	description[128];

	// true if all methods of the memory manager are thread-safe
	bool	bThreadsafe;

public:
	inline mxMemoryManagerInfo()
	{
		ZERO_OUT(*this);
	}
};

//
//	mxMemoryStatistics - holds memory allocation information.
//
struct mxMemoryStatistics
{
	SizeT	bytesAllocated;	// total size of currently allocated memory, in bytes

	SizeT	totalAllocated;	// size of memory that have been allocated so far, in bytes
	SizeT	totalFreed;		// size of memory that have been deallocated so far, in bytes

	SizeT	peakMemoryUsage;	// in bytes

	UINT	totalNbAllocations;	// incremented whenever allocation occurs
	UINT	totalNbReallocations;// incremented whenever reallocation occurs
	UINT	totalNbDeallocations;// incremented whenever deallocation occurs

public:
	inline mxMemoryStatistics()
	{
		Reset();
	}
	inline void Reset()
	{
		bytesAllocated = 0;

		totalAllocated = 0;
		totalFreed = 0;

		peakMemoryUsage = 0;

		totalNbAllocations = 0;
		totalNbReallocations = 0;
		totalNbDeallocations = 0;
	}
	inline void UpdateOnAllocation( UINT numAllocatedBytes )
	{
		bytesAllocated += numAllocatedBytes;
		totalAllocated += numAllocatedBytes;
		totalNbAllocations++;

		peakMemoryUsage = Max( peakMemoryUsage, bytesAllocated );
	}
	inline void UpdateOnDeallocation( UINT bytesFreed )
	{
		bytesAllocated -= bytesFreed;
		totalFreed += bytesFreed;
		totalNbDeallocations++;
	}

	void Dump( mxOutputDevice& log );
};

// reports memory statistics
void F_GetGlobalMemoryStats( mxMemoryStatistics &outStats );

// writes memory statistics, usage & leak info to the specified file
void F_DumpGlobalMemoryStats( mxOutputDevice& log );

/*
==============================================================
	Notes on memory management system.

	Memory allocations/deallocations are expensive operations
	so use of virtual member functions should be ok.
==============================================================
*/

//
//	mxMemoryAllocator
//
class mxMemoryAllocator
{
public:
	virtual void *	Allocate( SizeT numBytes ) = 0;
//	virtual void *	Reallocate( void* oldMemory, SizeT numBytes ) = 0;

//	virtual void	OptimizeHeap() {};

protected:
	virtual	~mxMemoryAllocator() {}
};

//
//	mxMemoryDeallocator
//
class mxMemoryDeallocator
{
public:
	virtual void	Free( void* pMemory ) = 0;
//	virtual void	Free( void* pMemory, SizeT numBytes ) { Free( pMemory ); }

protected:
	virtual	~mxMemoryDeallocator() {}
};

//
//	mxMemoryManager - is an abstract memory manager.
//
class mxMemoryManager : public mxMemoryAllocator, public mxMemoryDeallocator
{
public:
	virtual void	Initialize() {};
	virtual void	Shutdown() {};	// shutdown and assert on memory leaks

//	virtual void	GetInfo( mxMemoryManagerInfo &outInfo ) = 0;
	virtual void	GetStats( mxMemoryStatistics &outStats ) { ZERO_OUT(outStats); };

	virtual void	Dump() {}; // Log statistics,memory usage,etc
	virtual void	ValidateHeap() {};	// verify heap

	// Returns the size of a memory block allocated by this allocator.
	virtual SizeT	SizeOf( const void* ptr ) const = 0;

protected:
	virtual	~mxMemoryManager() {}
};


//
//	First 'HeapCount' handles point at their corresponding heap managers.
//

HMemory F_RegisterMemoryManager( mxMemoryManager* mgr );
mxMemoryManager* F_GetMemoryManager( HMemory mgr );
void F_UnregisterMemoryManager( HMemory mgr );
UINT F_GetNumRegisteredMemoryManagers();

//
//	Global memory management functions.
//	Most of memory allocations within the engine should be forwarded to these functions.
//
void* F_HeapAlloc( HMemory heap, SizeT numBytes );
void F_HeapFree( HMemory heap, void* pointer );
SizeT F_HeapSizeOfMemoryBlock( HMemory heap, const void* pointer );


//
//	System memory management functions.
//
// TODO: user memory management functions and event listeners
//void F_GlobalSetUserAllocator();

SizeT F_GetMaxAllowedAllocationSize();

void* F_SysAlloc( SizeT numBytes );
void F_SysFree( void* pointer );
SizeT F_SysSizeOfMemoryBlock( const void* pointer );


void F_MemDbg_ValidateHeap();	// debug mode only

//----------------------------------------------

// designed to occupy less space
class HMemMgr
{
	const HMemory	mMemory;	// Handle to some registered memory manager

public:
	HMemMgr( const HMemory memHeap );

	void* Alloc( SizeT numBytes );
	void Free( void* pointer );
	SizeT SizeOf( const void* pointer );
};

//----------------------------------------------

template< typename TYPE >
FORCEINLINE TYPE* New( mxMemoryManager* mgr )
{
	return c_cast(TYPE*) mgr->Allocate( sizeof(TYPE) );
}

template< typename TYPE >
FORCEINLINE TYPE* NewArray( mxMemoryManager* mgr, SizeT numObjects )
{
	return c_cast(TYPE*) mgr->Allocate( sizeof(TYPE) * numObjects );
}

template< typename TYPE >
FORCEINLINE void Delete( mxMemoryManager* mgr, TYPE* objectPtr )
{
	if( objectPtr ) {
		objectPtr->~TYPE();
		mgr->Free( objectPtr );
	}
}
template< typename TYPE >
FORCEINLINE void DeleteAndNil( mxMemoryManager* mgr, TYPE*& objectPtr )
{
	if( objectPtr ) {
		objectPtr->~TYPE();
		mgr->Free( objectPtr );
		objectPtr = nil;
	}
}



//---------------------------------------------------------------------------
//		Miscellaneous helpers.
//---------------------------------------------------------------------------



template< typename T >
struct ObjectDeleter {
	inline void operator () ( T* ptr ) {
		delete ptr;
	}
};
template< typename T >
struct ArrayDeleter {
	inline void operator () ( T* ptr ) {
		delete[] ptr;
	}
};

template< typename T >
struct mxObjectDeleter {
	inline void operator () ( T* ptr ) {
		free_one( ptr );
	}
};
template< typename T >
struct mxArrayDeleter {
	inline void operator () ( T* ptr ) {
		free_array( ptr );
	}
};



//---------------------------------------------------------------------------
//		Helper macros.
//---------------------------------------------------------------------------


#define mxAllocX( memClass, numBytes )		F_HeapAlloc( memClass, (numBytes) )
#define mxFreeX( memClass, pointer )		F_HeapFree( memClass, (pointer) )

#define mxAlloc( numBytes )					mxAllocX( EMemHeap::DefaultHeap, (numBytes) )
#define mxFree( pointer )					mxFreeX( EMemHeap::DefaultHeap, (pointer) )


#define	mxAlloc_Temp( numBytes )	mxAllocX( EMemHeap::HeapTemp, (numBytes) );
#define	mxFree_Temp( pMemory )		mxFreeX( EMemHeap::HeapTemp, (pMemory) );

#define mxMalloc16( numBytes )	mxAlloc( ALIGN16(numBytes) )
#define mxFree16( pMemory )		mxFree( (pMemory) )





#if defined(new_one) || defined(free_one)
#	error 'new_one' and 'free_one' have already been  defined - shouldn't happen!
#endif

#define new_one( x )	new x
#define free_one( x )	delete x

// Array operators

#if defined(new_array) || defined(free_array)
#	error 'new_array' and 'free_array' have already been defined - shouldn't happen!
#endif

#define new_array( x, num )		new x [num]
#define free_array( x )			delete[] x





#if MX_OVERRIDE_GLOBAL_NEWDELETE
	#error Incompatible options: overriden global 'new' and 'delete' and per-class memory heaps.
#endif

//
//	mxDECLARE_CLASS_ALLOCATOR
//
//	'className' can be used to track instances of the class.
//
#define mxDECLARE_CLASS_ALLOCATOR( memClass, className )\
public:\
	typedef className THIS_TYPE;	\
	static EMemHeap GetHeap() { return memClass; }	\
	FORCEINLINE void* operator new      ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete   ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new      ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete   ( void*, void* )		{ }												\
	FORCEINLINE void* operator new[]    ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete[] ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new[]    ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete[] ( void*, void* )		{ }												\

//
//	mxDECLARE_VIRTUAL_CLASS_ALLOCATOR
//
#define mxDECLARE_VIRTUAL_CLASS_ALLOCATOR( memClass, className )\
public:\
	typedef className THIS_TYPE;	\
	FORCEINLINE void* operator new      ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete   ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new      ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete   ( void*, void* )		{ }												\
	FORCEINLINE void* operator new[]    ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete[] ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new[]    ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete[] ( void*, void* )		{ }


//
//	mxDECLARE_NONVIRTUAL_CLASS_ALLOCATOR
//
#define mxDECLARE_NONVIRTUAL_CLASS_ALLOCATOR( memClass, className )\
public:\
	typedef className THIS_TYPE;	\
	FORCEINLINE void* operator new      ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete   ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new      ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete   ( void*, void* )		{ }												\
	FORCEINLINE void* operator new[]    ( size_t sizeInBytes )	{ return mxAllocX( memClass, sizeInBytes ); }	\
	FORCEINLINE void  operator delete[] ( void* ptr )			{ mxFreeX( memClass, ptr ); }					\
	FORCEINLINE void* operator new[]    ( size_t, void* ptr )	{ return ptr; }									\
	FORCEINLINE void  operator delete[] ( void*, void* )		{ }



//---------------------------------------------------------------------------
//		Prevent usage of plain old 'malloc' & 'free' if needed.
//---------------------------------------------------------------------------

#if MX_HIDE_MALLOC_AND_FREE

	#define malloc( size )			mxALWAYS_BREAK
	#define free( mem )				mxALWAYS_BREAK
	#define calloc( num, size )		mxALWAYS_BREAK
	#define realloc( mem, newsize )	mxALWAYS_BREAK

#endif // MX_HIDE_MALLOC_AND_FREE


//---------------------------------------------------------------------------
//		Memory managers.
//---------------------------------------------------------------------------



class mxMemoryTrackingManager : public mxMemoryManager
{
public:
	void GetStats( mxMemoryStatistics &outStats ) override
	{ outStats = mStats; }

protected:
	mxMemoryStatistics	mStats;
};

//
//	mxProxyMemoryManager
//
class mxProxyMemoryManager : public mxMemoryTrackingManager
{
	mxMemoryManager *	clientMgr;

public:
	mxProxyMemoryManager( mxMemoryManager* clientMgr )
		: clientMgr( clientMgr )
	{}

	void* Allocate( SizeT numBytes ) override
	{
		void* pNewMem = clientMgr->Allocate( numBytes );
		SizeT actualNumBytes = clientMgr->SizeOf( pNewMem );
		mStats.UpdateOnAllocation( actualNumBytes );
		return pNewMem;
	}

	void Free( void* pMemory ) override
	{
		if( !pMemory ) {
			return;
		}
		SizeT numBytes = clientMgr->SizeOf( pMemory );
		mStats.UpdateOnDeallocation( numBytes );
		clientMgr->Free( pMemory );
	}

	SizeT SizeOf( const void* ptr ) const override
	{
		return clientMgr->SizeOf( ptr );
	}
};


//
//	mxSystemMemoryManager
//
class mxSystemMemoryManager : public mxMemoryManager
{
public:
	mxSystemMemoryManager()
	{}

	void* Allocate( SizeT numBytes ) override;
	void Free( void* pMemory ) override;
	SizeT SizeOf( const void* ptr ) const override;
};


template< typename TYPE >
class TDefaultAllocator
{
	HMemory		mMemory;	// Handle to the memory manager

public:
	inline explicit TDefaultAllocator( HMemory hMemoryMgr = EMemHeap::DefaultHeap )
		: mMemory( EMemHeap::DefaultHeap )
	{

	}
	inline ~TDefaultAllocator()
	{

	}
	inline void* AllocateMemory( SizeT size )
	{
		return F_GetMemoryManager( mMemory )->Allocate( size);
	}
	inline void ReleaseMemory( void* ptr )
	{
		F_GetMemoryManager( mMemory )->Free( ptr );
	}
};



//------------------------------------------------------------------------
//	Useful macros
//------------------------------------------------------------------------

#ifndef SAFE_DELETE
#define SAFE_DELETE( p )		{ if( p != nil ) { delete (p);     (p) = nil; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( p )	{ if( p != nil ) { delete[] (p);   (p) = nil; } }
#endif

#endif /* ! __MX_MEMORY_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
