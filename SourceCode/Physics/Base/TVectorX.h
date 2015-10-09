/*
=============================================================================
	File:	TVectorX.h
	Desc:	.
=============================================================================
*/

#ifndef __PX_VectorX_H__
#define __PX_VectorX_H__

//
//	TVectorX< T >
//
//	NOTE: data should be allocated elsewhere
//
template< typename T >
class TVectorX {
public:
	FORCEINLINE TVectorX()
	{
		mData = nil;
		mSize = 0;
	}

	FORCEINLINE TVectorX( T* pData, pxUInt num )
	{
		AssertPtr(pData);
		Assert(num > 0);
		mData = pData;
		mSize = num;
	}

	// Returns the size (the number of elements in the array).
	FORCEINLINE pxUInt Num() const
	{
		return mSize;
	}

	FORCEINLINE void SetValue( const T& newValue )
	{
		for( pxUInt i=0; i < Num(); i++ ) {
			mData[ i ] = newValue;
		}
	}
	FORCEINLINE void SetZero()
	{
		MemSet( mData, 0, GetDataSize() );
	}


	// Read/write access to the i'th element.
	FORCEINLINE T& operator [] ( pxUInt i )
	{
		Assert( i >= 0 && i < Num() );
		return mData[ i ];
	}

	// Read only access to the i'th element.
	FORCEINLINE const T& operator [] ( pxUInt i ) const
	{
		Assert( i >= 0 && i < Num() );
		return mData[ i ];
	}



	FORCEINLINE T * ToPtr()
	{
		return mData;
	}
	FORCEINLINE T * ToPtr( pxUInt index )
	{
		Assert( index >= 0 && index < Num() );
		return mData + index;
	}
	FORCEINLINE const T * ToPtr( pxUInt index ) const
	{
		Assert( index >= 0 && index < Num() );
		return mData + index;
	}
	FORCEINLINE const T * ToPtr() const
	{
		return mData;
	}


	// Returns the total size of stored elements, in bytes.
	FORCEINLINE SizeT GetDataSize() const
	{
		return Num() * sizeof(T);
	}

private:
	typedef TVectorX<T> THIS_TYPE;
	PREVENT_COPY(THIS_TYPE);

private:
	T *		mData;
	pxUInt	mSize;
};

typedef TVectorX< pxReal >	pxVecX;

#endif // !__PX_VectorX_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
