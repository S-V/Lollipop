/*
=============================================================================
	File:	PoolAllocator.h
	Desc:	Fast pool allocator for fixed-size allocations.
=============================================================================
*/

#ifndef __MX_MEMORY_POOL_ALLOCATOR_H__
#define __MX_MEMORY_POOL_ALLOCATOR_H__


mxNAMESPACE_BEGIN

mxSWIPED("Bullet physics");

///The btPoolAllocator class allows to efficiently allocate a large pool of objects, instead of dynamically allocating them separately.
class mxPoolAllocator
{
	UINT			m_elemSize;
	UINT			m_maxElements;
	UINT			m_freeCount;
	void *			m_firstFree;
	BYTE *			m_pool;
	const HMemory	m_memHeap;

public:
	mxPoolAllocator( UINT elemSize, UINT maxElements, HMemory memHeap = EMemHeap::DefaultHeap )
		: m_elemSize( elemSize )
		, m_maxElements( maxElements )
		, m_memHeap( memHeap )
	{
		Assert( elemSize > sizeof U4 );
		Assert( maxElements > 1 );

		const UINT sizeOfMemPool = m_elemSize * m_maxElements;

		m_pool = (BYTE*) mxAllocX( m_memHeap, sizeOfMemPool );

		BYTE* p = m_pool;
		m_firstFree = p;
		m_freeCount = m_maxElements;
		UINT count = m_maxElements;
		while( --count ) {
			*(void**)p = (p + m_elemSize);
			p += m_elemSize;
		}
		*(void**)p = nil;
	}

	~mxPoolAllocator()
	{
		mxFreeX( m_memHeap, m_pool );
	}

	FORCEINLINE UINT GetItemSize() const
	{
		return m_elemSize;
	}
	FORCEINLINE UINT GetFreeCount() const
	{
		return m_freeCount;
	}
	FORCEINLINE UINT GetUsedCount() const
	{
		return m_maxElements - m_freeCount;
	}
	FORCEINLINE UINT GetMaxCount() const
	{
		return m_maxElements;
	}

	INLINE void* Allocate( UINT size )
	{
		// release mode fix
		(void)size;
		Assert( size != 0 );
		Assert( size <= m_elemSize );
		Assert( m_freeCount > 0 );
		void* result = m_firstFree;
		m_firstFree = *(void**)m_firstFree;
		--m_freeCount;
		return result;
	}

	INLINE bool HasAddress( const void* ptr )
	{
		return mxIsValidHeapPointer(ptr) &&
			(((BYTE*)ptr >= m_pool && (BYTE*)ptr < m_pool + m_maxElements * m_elemSize))
			;
	}

	INLINE void Deallocate( void* ptr )
	{
		Assert(this->HasAddress(ptr));
		if(PtrToBool( ptr ))
		{
			//Assert((BYTE*)ptr >= m_pool && (BYTE*)ptr < m_pool + m_maxElements * m_elemSize);

			*(void**)ptr = m_firstFree;
			m_firstFree = ptr;
			++m_freeCount;
		}
	}

	FORCEINLINE BYTE* GetPoolAddress()
	{
		return m_pool;
	}
	FORCEINLINE const BYTE* GetPoolAddress() const
	{
		return m_pool;
	}
};

mxNAMESPACE_END

#endif // __MX_MEMORY_POOL_ALLOCATOR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
