/*
=============================================================================
	File:	TMatrixX.h
	Desc:	Arbitrary sized dense real matrix.
=============================================================================
*/

#ifndef __PX_TMatrixX_H__
#define __PX_TMatrixX_H__

//
//	TMatrixX< T > - arbitrary sized dense matrix.
//
//	NOTE: it is a row-major matrix (M rows, N columns).
//
//	NOTE: data should be allocated elsewhere and 16-byte aligned.
//
template< typename T >
class TMatrixX
{
public:
	pxULong MaxRows() const { return lol::TPow2< BYTES_TO_BITS(sizeof(mNumRows)) >::value; }
	pxULong MaxColumns() const { return lol::TPow2< BYTES_TO_BITS(sizeof(mNumColumns)) >::value; }

	FORCEINLINE TMatrixX()
	{
		mData = nil;
		mNumRows = 0;
		mNumColumns = 0;
	}

	FORCEINLINE
	TMatrixX( T* pData, pxUInt numRows, pxUInt numColumns )
	{
		AssertPtr(pData);
		mData = pData;
		mNumRows = numRows;
		mNumColumns = numColumns;
	}


	// Returns the number of rows.
	pxUInt GetNumRows() const
	{ return mNumRows; }

	// Returns the number of columns.
	pxUInt GetNumColumns() const
	{ return mNumColumns; }

	// Returns the size (the number of elements in the array).
	FORCEINLINE pxUInt Num() const
	{
		return (mNumRows * mNumColumns);
	}



	FORCEINLINE bool IsSquare() const
	{ return ( mNumRows == mNumColumns ); }

	FORCEINLINE void SetZero()
	{
		MemSet( mData, 0, GetDataSize() );
	}



	FORCEINLINE
	T & operator () ( pxUInt iRow, pxUInt iColumn ) {
		Assert( iRow < mNumRows && iColumn < mNumColumns );
		return *(mData + iRow * mNumColumns + iColumn);
	}
	FORCEINLINE
	const T & operator () ( pxUInt iRow, pxUInt iColumn ) const {
		Assert( iRow < mNumRows && iColumn < mNumColumns );
		return *(mData + iRow * mNumColumns + iColumn);
	}

	FORCEINLINE T * ToPtr()
	{
		return mData;
	}
	FORCEINLINE T * ToPtr( pxUInt iRow )
	{
		return mData + iRow * mNumColumns;
	}
	FORCEINLINE T * ToPtr( pxUInt iRow, pxUInt iColumn )
	{
		return mData + iRow * mNumColumns + iColumn;
	}

	FORCEINLINE const T * ToPtr() const { return mData; }



	// Returns the total size of stored elements, in bytes.
	FORCEINLINE SizeT GetDataSize() const
	{
		return Num() * sizeof(T);
	}


private:
	typedef TMatrixX<T> THIS_TYPE;
	PREVENT_COPY(THIS_TYPE);

private:
	T *		mData;
	pxU2	mNumRows;
	pxU2	mNumColumns;
};

typedef TMatrixX< pxReal >	pxMatX;

#endif // !__PX_TMatrixX_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
