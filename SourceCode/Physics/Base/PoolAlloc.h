/*
=============================================================================
	File:	PoolAlloc.h
	Desc:	Fast pool allocator. See:
		http://altdevblogaday.org/2011/02/12/alternatives-to-malloc-and-new/
=============================================================================
*/

#ifndef __MX_POOL_ALLOCATOR_H__
#define __MX_POOL_ALLOCATOR_H__
MX_SWIPED("Bullet");

//
//	mxPoolAlloc
//
class mxPoolAlloc {
public:
	FORCEINLINE mxPoolAlloc()
	{
		mFirstFree = nil;
		mFreeCount = 0;
		mData = nil;
		mElementSize = 0;
		mMaxElements = 0;
	}

	FORCEINLINE mxPoolAlloc( UINT elementSize, UINT maxElements )
	{
		mFirstFree = nil;
		mFreeCount = 0;
		mData = nil;
		mElementSize = 0;
		mMaxElements = 0;
		Setup( elementSize, maxElements );
	}

	FORCEINLINE ~mxPoolAlloc()
	{
		pxFree( mData );
	}

	void Setup( UINT elementSize, UINT maxElements )
	{
		Assert(elementSize > 1 && maxElements > 1);
		Assert(!mData && !mFirstFree && !mFreeCount && !mMaxElements && !mElementSize);
		if( !mData )
		{
			mElementSize = elementSize;
			mMaxElements = maxElements;
			mData = (BYTE*) pxNew(static_cast<SizeT>( mElementSize * mMaxElements ));

			BYTE* p = mData;
			mFirstFree = p;
			mFreeCount = mMaxElements;

			UINT count = mMaxElements;
			while( --count ) {
				*(void**)p = (p + mElementSize);
				p += mElementSize;
			}
			*(void**)p = 0;
		}
		Assert(this->isOk());
	}

	void Close()
	{
		if( mData ) {
			pxFree( mData );
			mData = nil;
		}
		mFirstFree = nil;
		mFreeCount = 0;
		mMaxElements = 0;
	}

	FORCEINLINE int getFreeCount() const
	{
		return mFreeCount;
	}

	FORCEINLINE int getUsedCount() const
	{
		return mMaxElements - mFreeCount;
	}

	FORCEINLINE void* GetNew(/* SizeT size */)
	{
		Assert(mFreeCount > 0);
		void* result = mFirstFree;
		mFirstFree = *(BYTE**)mFirstFree;
		--mFreeCount;
		return result;
	}

	FORCEINLINE void Free( void* ptr )
	{
		Assert(
			ptr
			&&
			(BYTE*)ptr >= (BYTE*)getPoolAddress()
			&&
			(BYTE*)ptr < ((BYTE*)getPoolAddress() + mMaxElements * mElementSize)
		);
		*(void**)ptr = mFirstFree;
		mFirstFree = ptr;
		++mFreeCount;
	}

	FORCEINLINE SizeT getElementSize() const
	{
		return mElementSize;
	}

	FORCEINLINE void* getPoolAddress()
	{
		return mData;
	}

	FORCEINLINE const void* getPoolAddress() const
	{
		return mData;
	}

	bool isValidPtr( const void* ptr ) const
	{
		if( ptr )
		{
			if( ((BYTE*)ptr >= (BYTE*)mData
				&& (BYTE*)ptr < (BYTE*)mData + mMaxElements * mElementSize) )
			{
				return true;
			}
		}
		return false;
	}

	bool isOk() const {
		return (mElementSize > 0)
			&& (mMaxElements > 0)
			&& (mFreeCount > 0)
			&& (mxIsValidHeapPointer(mFirstFree))
			&& (mxIsValidHeapPointer(mData))
			;
	}

	static SizeT MAX_CAPACITY() {
		return (1 << BYTES_TO_BITS(FIELD_SIZE(mxPoolAlloc,mMaxElements)));
	}

private:	PREVENT_COPY(mxPoolAlloc);

private:
	void *	mFirstFree;
	BYTE *	mData;
	UINT	mFreeCount;
	UINT16	mElementSize;
	UINT16	mMaxElements;
};

#endif // !__MX_POOL_ALLOCATOR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
