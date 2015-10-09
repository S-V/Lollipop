//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//===========================================================================//

#ifndef MEMPOOL_H
#define MEMPOOL_H

#ifdef _WIN32
#pragma once
#endif

mxSWIPED("Valve Source Engine L4D SDK");

#if 0

#if (defined(_DEBUG) || defined(USE_MEM_DEBUG))
#define MEM_ALLOC_CREDIT_(tag)	CMemAllocAttributeAlloction memAllocAttributeAlloction( tag, __LINE__ )
#define MemAlloc_PushAllocDbgInfo( pszFile, line ) g_pMemAlloc->PushAllocDbgInfo( pszFile, line )
#define MemAlloc_PopAllocDbgInfo() g_pMemAlloc->PopAllocDbgInfo()
#define MemAlloc_RegisterAllocation( pFileName, nLine, nLogicalSize, nActualSize, nTime ) g_pMemAlloc->RegisterAllocation( pFileName, nLine, nLogicalSize, nActualSize, nTime )
#define MemAlloc_RegisterDeallocation( pFileName, nLine, nLogicalSize, nActualSize, nTime ) g_pMemAlloc->RegisterDeallocation( pFileName, nLine, nLogicalSize, nActualSize, nTime )
#else
#define MEM_ALLOC_CREDIT_(tag)	((void)0)
#define MemAlloc_PushAllocDbgInfo( pszFile, line ) ((void)0)
#define MemAlloc_PopAllocDbgInfo() ((void)0)
#define MemAlloc_RegisterAllocation( pFileName, nLine, nLogicalSize, nActualSize, nTime ) ((void)0)
#define MemAlloc_RegisterDeallocation( pFileName, nLine, nLogicalSize, nActualSize, nTime ) ((void)0)
#endif



#define MEM_ALLOC_CREDIT()	MEM_ALLOC_CREDIT_(__FILE__)

//-----------------------------------------------------------------------------

#if defined(_WIN32) && ( defined(_DEBUG) || defined(USE_MEM_DEBUG) )

	#pragma warning(disable:4290)
	#pragma warning(push)
	#include <typeinfo.h>

	// MEM_DEBUG_CLASSNAME is opt-in.
	// Note: typeid().name() is not threadsafe, so if the project needs to access it in multiple threads
	// simultaneously, it'll need a mutex.
	#if defined(_CPPRTTI) && defined(MEM_DEBUG_CLASSNAME)
		#define MEM_ALLOC_CREDIT_CLASS()	MEM_ALLOC_CREDIT_( typeid(*this).name() )
		#define MEM_ALLOC_CLASSNAME(type) (typeid((type*)(0)).name())
	#else
		#define MEM_ALLOC_CREDIT_CLASS()	MEM_ALLOC_CREDIT_( __FILE__ )
		#define MEM_ALLOC_CLASSNAME(type) (__FILE__)
	#endif

	// MEM_ALLOC_CREDIT_FUNCTION is used when no this pointer is available ( inside 'new' overloads, for example )
	#ifdef _MSC_VER
		#define MEM_ALLOC_CREDIT_FUNCTION()		MEM_ALLOC_CREDIT_( __FUNCTION__ )
	#else
		#define MEM_ALLOC_CREDIT_FUNCTION() (__FILE__)
	#endif

	#pragma warning(pop)
#else
	#define MEM_ALLOC_CREDIT_CLASS()
	#define MEM_ALLOC_CLASSNAME(type) NULL
	#define MEM_ALLOC_CREDIT_FUNCTION() 
#endif

#else

#endif







//-----------------------------------------------------------------------------
// Purpose: Optimized pool memory allocator
//-----------------------------------------------------------------------------

typedef void (*MemoryPoolReportFunc_t)( char const* pMsg, ... );

class CMemoryPool
{
public:
	// Ways the memory pool can grow when it needs to make a new blob.
	enum MemoryPoolGrowType_t
	{
		GROW_NONE=0,		// Don't allow new blobs.
		GROW_FAST=1,		// New blob size is numElements * (i+1)  (ie: the blocks it allocates
		// get larger and larger each time it allocates one).
		GROW_SLOW=2			// New blob size is numElements.
	};

	CMemoryPool( int blockSize, int numElements, int growMode = GROW_FAST, const char *pszAllocOwner = NULL, int nAlignment = 0 );
	~CMemoryPool();

	void*		Alloc();	// Allocate the element size you specified in the constructor.
	void*		Alloc( size_t amount );
	void*		AllocZero();	// Allocate the element size you specified in the constructor, zero the memory before construction
	void*		AllocZero( size_t amount );
	void		Free(void *pMem);

	// Frees everything
	void		Clear();

	// Error reporting... 
	static void SetErrorReportFunc( MemoryPoolReportFunc_t func );

	// returns number of allocated blocks
	int Count() { return m_BlocksAllocated; }
	int PeakCount() { return m_PeakAlloc; }

protected:
	class CBlob
	{
	public:
		CBlob	*m_pPrev, *m_pNext;
		int		m_NumBytes;		// Number of bytes in this blob.
		char	m_Data[1];
	};

	// Resets the pool
	void		Init();
	void		AddNewBlob();
	void		ReportLeaks();

	int			m_BlockSize;
	int			m_BlocksPerBlob;

	int			m_GrowMode;	// GROW_ enum.

	// FIXME: Change m_ppMemBlob into a growable array?
	CBlob			m_BlobHead;
	void			*m_pHeadOfFreeList;
	int				m_BlocksAllocated;
	int				m_PeakAlloc;
	unsigned short	m_nAlignment;
	unsigned short	m_NumBlobs;
	const char *	m_pszAllocOwner;

	static MemoryPoolReportFunc_t g_ReportFunc;
};


//-----------------------------------------------------------------------------
// Wrapper macro to make an allocator that returns particular typed allocations
// and construction and destruction of objects.
//-----------------------------------------------------------------------------
template< class T >
class CClassMemoryPool : public CMemoryPool
{
public:
	CClassMemoryPool(int numElements, int growMode = GROW_FAST, int nAlignment = 0 )
		: CMemoryPool( sizeof(T), numElements, growMode, MEM_ALLOC_CLASSNAME(T), nAlignment )
	{}

	T*		Alloc();
	T*		AllocZero();
	void	Free( T *pMem );

	void	Clear();
};


//-----------------------------------------------------------------------------
// Specialized pool for aligned data management (e.g., Xbox cubemaps)
//-----------------------------------------------------------------------------
template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD = 4 >
class CAlignedMemPool
{
	enum
	{
		BLOCK_SIZE = max( ALIGN_VALUE( ITEM_SIZE, ALIGNMENT ), 8 ),
	};

public:
	CAlignedMemPool();

	void *Alloc();
	void Free( void *p );

	static int __cdecl CompareChunk( void * const *ppLeft, void * const *ppRight );
	void Compact();

	int NumTotal()			{ return m_Chunks.Num() * ( CHUNK_SIZE / BLOCK_SIZE ); }
	int NumAllocated()		{ return NumTotal() - m_nFree; }
	int NumFree()			{ return m_nFree; }

	int BytesTotal()		{ return NumTotal() * BLOCK_SIZE; }
	int BytesAllocated()	{ return NumAllocated() * BLOCK_SIZE; }
	int BytesFree()			{ return NumFree() * BLOCK_SIZE; }

	int ItemSize()			{ return ITEM_SIZE; }
	int BlockSize()			{ return BLOCK_SIZE; }
	int ChunkSize()			{ return CHUNK_SIZE; }

private:
	struct FreeBlock_t
	{
		FreeBlock_t *pNext;
		byte		reserved[ BLOCK_SIZE - sizeof( FreeBlock_t *) ];
	};

	TList<void *>		m_Chunks;		// Chunks are tracked outside blocks (unlike CMemoryPool) to simplify alignment issues
	FreeBlock_t *		m_pFirstFree;
	int					m_nFree;
	CAllocator			m_Allocator;
	float				m_TimeLastCompact;
};

//-----------------------------------------------------------------------------
// Pool variant using standard allocation
//-----------------------------------------------------------------------------
#if 0
template <typename T, int nInitialCount = 0, bool bDefCreateNewIfEmpty = true >
class CObjectPool
{
public:
	CObjectPool()
	{
		int i = nInitialCount;
		while ( i-- > 0 )
		{
			m_AvailableObjects.PushItem( new T );
		}
	}

	~CObjectPool()
	{
		Purge();
	}

	int NumAvailable()
	{
		return m_AvailableObjects.Num();
	}

	void Purge()
	{
		T *p;
		while ( m_AvailableObjects.PopItem( &p ) )
		{
			delete p;
		}
	}

	T *GetObject( bool bCreateNewIfEmpty = bDefCreateNewIfEmpty )
	{
		T *p;
		if ( !m_AvailableObjects.PopItem( &p )  )
		{
			p = ( bCreateNewIfEmpty ) ? new T : NULL;
		}
		return p;
	}

	void PutObject( T *p )
	{
		m_AvailableObjects.PushItem( p );
	}

private:
	CTSList<T *> m_AvailableObjects;
};
#endif

//-----------------------------------------------------------------------------


template< class T >
inline T* CClassMemoryPool<T>::Alloc()
{
	T *pRet;

	{
		MEM_ALLOC_CREDIT_(MEM_ALLOC_CLASSNAME(T));
		pRet = (T*)CMemoryPool::Alloc();
	}

	if ( pRet )
	{
		Construct( pRet );
	}
	return pRet;
}

template< class T >
inline T* CClassMemoryPool<T>::AllocZero()
{
	T *pRet;

	{
		MEM_ALLOC_CREDIT_(MEM_ALLOC_CLASSNAME(T));
		pRet = (T*)CMemoryPool::AllocZero();
	}

	if ( pRet )
	{
		Construct( pRet );
	}
	return pRet;
}

template< class T >
inline void CClassMemoryPool<T>::Free(T *pMem)
{
	if ( pMem )
	{
		Destruct( pMem );
	}

	CMemoryPool::Free( pMem );
}

template< class T >
inline void CClassMemoryPool<T>::Clear()
{
	CUtlRBTree<void *> freeBlocks;
	SetDefLessFunc( freeBlocks );

	void *pCurFree = m_pHeadOfFreeList;
	while ( pCurFree != NULL )
	{
		freeBlocks.Insert( pCurFree );
		pCurFree = *((void**)pCurFree);
	}

	for( CBlob *pCur=m_BlobHead.m_pNext; pCur != &m_BlobHead; pCur=pCur->m_pNext )
	{
		T *p = (T *)pCur->m_Data;
		T *pLimit = (T *)(pCur->m_Data + pCur->m_NumBytes);
		while ( p < pLimit )
		{
			if ( freeBlocks.Find( p ) == freeBlocks.InvalidIndex() )
			{
				Destruct( p );
			}
			p++;
		}
	}

	CMemoryPool::Clear();
}


//-----------------------------------------------------------------------------
// Macros that make it simple to make a class use a fixed-size allocator
// Put DECLARE_FIXEDSIZE_ALLOCATOR in the private section of a class,
// Put DEFINE_FIXEDSIZE_ALLOCATOR in the CPP file
//-----------------------------------------------------------------------------
#define DECLARE_FIXEDSIZE_ALLOCATOR( _class )									\
	public:																		\
	inline void* operator new( size_t size ) { MEM_ALLOC_CREDIT_(#_class " pool"); return s_Allocator.Alloc(size); }   \
	inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine ) { MEM_ALLOC_CREDIT_(#_class " pool"); return s_Allocator.Alloc(size); }   \
	inline void  operator delete( void* p ) { s_Allocator.Free(p); }		\
	inline void  operator delete( void* p, int nBlockUse, const char *pFileName, int nLine ) { s_Allocator.Free(p); }   \
	private:																		\
	static   CMemoryPool   s_Allocator

#define DEFINE_FIXEDSIZE_ALLOCATOR( _class, _initsize, _grow )					\
	CMemoryPool   _class::s_Allocator(sizeof(_class), _initsize, _grow, #_class " pool")

#define DEFINE_FIXEDSIZE_ALLOCATOR_ALIGNED( _class, _initsize, _grow, _alignment )		\
	CMemoryPool   _class::s_Allocator(sizeof(_class), _initsize, _grow, #_class " pool", _alignment )

#define DECLARE_FIXEDSIZE_ALLOCATOR_MT( _class )									\
	public:																		\
	inline void* operator new( size_t size ) { MEM_ALLOC_CREDIT_(#_class " pool"); return s_Allocator.Alloc(size); }   \
	inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine ) { MEM_ALLOC_CREDIT_(#_class " pool"); return s_Allocator.Alloc(size); }   \
	inline void  operator delete( void* p ) { s_Allocator.Free(p); }		\
	inline void  operator delete( void* p, int nBlockUse, const char *pFileName, int nLine ) { s_Allocator.Free(p); }   \
	private:																		\
	static   CMemoryPoolMT   s_Allocator

#define DEFINE_FIXEDSIZE_ALLOCATOR_MT( _class, _initsize, _grow )					\
	CMemoryPoolMT   _class::s_Allocator(sizeof(_class), _initsize, _grow, #_class " pool")

//-----------------------------------------------------------------------------
// Macros that make it simple to make a class use a fixed-size allocator
// This version allows us to use a memory pool which is externally defined...
// Put DECLARE_FIXEDSIZE_ALLOCATOR_EXTERNAL in the private section of a class,
// Put DEFINE_FIXEDSIZE_ALLOCATOR_EXTERNAL in the CPP file
//-----------------------------------------------------------------------------

#define DECLARE_FIXEDSIZE_ALLOCATOR_EXTERNAL( _class )							\
   public:																		\
   inline void* operator new( size_t size )  { MEM_ALLOC_CREDIT_(#_class " pool"); return s_pAllocator->Alloc(size); }   \
   inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine )  { MEM_ALLOC_CREDIT_(#_class " pool"); return s_pAllocator->Alloc(size); }   \
   inline void  operator delete( void* p )   { s_pAllocator->Free(p); }		\
   private:																		\
   static   CMemoryPool*   s_pAllocator

#define DEFINE_FIXEDSIZE_ALLOCATOR_EXTERNAL( _class, _allocator )				\
	CMemoryPool*   _class::s_pAllocator = _allocator


template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD >
inline CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, COMPACT_THRESHOLD>::CAlignedMemPool()
: m_pFirstFree( 0 ),
m_nFree( 0 ),
m_TimeLastCompact( 0 )
{
	mxCOMPILE_TIME_ASSERT( sizeof( FreeBlock_t ) >= BLOCK_SIZE );
	mxCOMPILE_TIME_ASSERT( ALIGN_VALUE( sizeof( FreeBlock_t ), ALIGNMENT ) == sizeof( FreeBlock_t ) );
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD >
inline void *CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, COMPACT_THRESHOLD>::Alloc()
{
	if ( !m_pFirstFree )
	{
		FreeBlock_t *pNew = (FreeBlock_t *)m_Allocator.Alloc( CHUNK_SIZE );
		Assert( (unsigned)pNew % ALIGNMENT == 0 );
		m_Chunks.AddToTail( pNew );
		m_nFree = CHUNK_SIZE / BLOCK_SIZE;
		m_pFirstFree = pNew;
		for ( int i = 0; i < m_nFree - 1; i++ )
		{
			pNew->pNext = pNew + 1;
			pNew++;
		}
		pNew->pNext = NULL;
	}

	void *p = m_pFirstFree;
	m_pFirstFree = m_pFirstFree->pNext;
	m_nFree--;

	return p;
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD >
inline void CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, COMPACT_THRESHOLD>::Free( void *p )
{
	// Insertion sort to encourage allocation clusters in chunks
	FreeBlock_t *pFree = ((FreeBlock_t *)p);
	FreeBlock_t *pCur = m_pFirstFree;
	FreeBlock_t *pPrev = NULL;

	while ( pCur && pFree > pCur )
	{
		pPrev = pCur;
		pCur = pCur->pNext;
	}

	pFree->pNext = pCur;

	if ( pPrev )
	{
		pPrev->pNext = pFree;
	}
	else
	{
		m_pFirstFree = pFree;
	}
	m_nFree++;

	if ( m_nFree >= ( CHUNK_SIZE / BLOCK_SIZE ) * COMPACT_THRESHOLD )
	{
		float time = Plat_FloatTime();
		float compactTime = ( m_nFree >= ( CHUNK_SIZE / BLOCK_SIZE ) * COMPACT_THRESHOLD * 4 ) ? 15.0 : 30.0;
		if ( m_TimeLastCompact > time || m_TimeLastCompact + compactTime < Plat_FloatTime() )
		{
			Compact();
			m_TimeLastCompact = time;
		}
	}
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD >
inline int __cdecl CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, COMPACT_THRESHOLD>::CompareChunk( void * const *ppLeft, void * const *ppRight )
{
	return ((unsigned)*ppLeft) - ((unsigned)*ppRight);
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, int COMPACT_THRESHOLD >
inline void CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, COMPACT_THRESHOLD>::Compact()
{
	FreeBlock_t *pCur = m_pFirstFree;
	FreeBlock_t *pPrev = NULL;

	m_Chunks.Sort( CompareChunk );

#ifdef VALIDATE_ALIGNED_MEM_POOL
	{
		FreeBlock_t *p = m_pFirstFree;
		while ( p )
		{
			if ( p->pNext && p > p->pNext )
			{
				__asm { int 3 }
			}
			p = p->pNext;
		}

		for ( int i = 0; i < m_Chunks.Num(); i++ )
		{
			if ( i + 1 < m_Chunks.Num() )
			{
				if ( m_Chunks[i] > m_Chunks[i + 1] )
				{
					__asm { int 3 }
				}
			}
		}
	}
#endif

	int i;

	for ( i = 0; i < m_Chunks.Num(); i++ )
	{
		int nBlocksPerChunk = CHUNK_SIZE / BLOCK_SIZE;
		FreeBlock_t *pChunkLimit = ((FreeBlock_t *)m_Chunks[i]) + nBlocksPerChunk;
		int nFromChunk = 0;
		if ( pCur == m_Chunks[i] )
		{
			FreeBlock_t *pFirst = pCur;
			while ( pCur && pCur >= m_Chunks[i] && pCur < pChunkLimit )
			{
				pCur = pCur->pNext;
				nFromChunk++;
			}
			pCur = pFirst;

		}

		while ( pCur && pCur >= m_Chunks[i] && pCur < pChunkLimit )
		{
			if ( nFromChunk != nBlocksPerChunk )
			{
				if ( pPrev )
				{
					pPrev->pNext = pCur;
				}
				else
				{
					m_pFirstFree = pCur;
				}
				pPrev = pCur;
			}
			else if ( pPrev )
			{
				pPrev->pNext = NULL;
			}
			else
			{
				m_pFirstFree = NULL;
			}

			pCur = pCur->pNext;
		}

		if ( nFromChunk == nBlocksPerChunk )
		{
			m_Allocator.Free( m_Chunks[i] );
			m_nFree -= nBlocksPerChunk;
			m_Chunks[i] = 0;
		}
	}

	for ( i = m_Chunks.Num() - 1; i >= 0 ; i-- )
	{
		if ( !m_Chunks[i] )
		{
			m_Chunks.FastRemove( i );
		}
	}
}

#endif // MEMPOOL_H
