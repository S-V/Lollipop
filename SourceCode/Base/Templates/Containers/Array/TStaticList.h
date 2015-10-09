/*
=============================================================================
	File:	TStaticList.
	Desc:	Static list template.
			A non-growing, memset-able list using no memory allocation.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __T_STATIC_LIST_H__
#define __T_STATIC_LIST_H__

mxNAMESPACE_BEGIN

/*
--------------------------------------------------------------
	TStaticList< typename TYPE, const UINT SIZE >
	- 32-bit non-growable array
--------------------------------------------------------------
*/
template< typename TYPE, const UINT SIZE >
class TStaticList
	: public TArrayBase< TYPE, TStaticList<TYPE,SIZE> >
{
	UINT	mNum;
	TYPE	mData[ SIZE ];

public:

	typedef TStaticList<TYPE,SIZE> THIS_TYPE;

	typedef
	TYPE
	ITEM_TYPE;

public:
	FORCEINLINE TStaticList()
	{
		mNum = 0;
	}
	FORCEINLINE explicit TStaticList(EInitZero)
	{
		mNum = 0;
		ZERO_OUT(mData);
	}
	FORCEINLINE ~TStaticList()
	{
	}

	// Returns the number of elements currently contained in the list.
	FORCEINLINE UINT Num() const
	{
		return mNum;
	}

	FORCEINLINE void SetNum( UINT newNum )
	{
		Assert(newNum <= Max());
		mNum = newNum;
	}

	// Returns the maximum number of elements in the list.
	FORCEINLINE UINT Max() const
	{
		return SIZE;
	}
	FORCEINLINE UINT GetCapacity() const
	{
		return SIZE;
	}

	FORCEINLINE void Empty()
	{
		TDestructN_IfNonPOD( mData, mNum );
		mNum = 0;
	}
	FORCEINLINE void Clear()
	{
		TDestructN_IfNonPOD( mData, mNum );
		mNum = 0;
	}

	FORCEINLINE bool IsFull() const
	{
		return mNum == SIZE;
	}

	// Returns a pointer to the beginning of the array.  Useful for iterating through the list in loops.
	FORCEINLINE TYPE * ToPtr()
	{
		return mData;
	}
	FORCEINLINE const TYPE * ToPtr() const
	{
		return mData;
	}

	// Implicit type conversion - intentionally disabled:
	//FORCEINLINE operator TYPE* ()
	//{ return mData; }
	//FORCEINLINE operator const TYPE* () const
	//{ return mData; }

	FORCEINLINE void Add( const TYPE& newItem )
	{
		Assert( mNum < SIZE );
		mData[ mNum++ ] = newItem;
	}
	FORCEINLINE TYPE & Add()
	{
		Assert( mNum < SIZE );
		return mData[ mNum++ ];
	}

	void AddBytes( const void* src, SizeT numBytes )
	{
		SizeT oldNum = mNum;
		SizeT newNum = oldNum + numBytes;
		SetNum( newNum );

		MemCopy( mData + oldNum, src, numBytes );
	}

	// Slow!
	bool AddUnique( const TYPE& item )
	{
		for( UINT i = 0; i < mNum; i++ )
		{
			if( mData[i] == item ) {
				return false;
			}
		}
		Add( item );
		return true;
	}

	// Slow!
	FORCEINLINE bool Remove( const TYPE& item )
	{
		return Array_Util::RemoveElement( *this, item );
	}

	// Slow!
	INLINE void RemoveAt( UINT index )
	{
		Array_Util::RemoveAt( mData, mNum, index, 1 );
	}

	// Doesn't preserve the relative order of elements.
	FORCEINLINE void RemoveAt_Fast( UINT index )
	{
		Array_Util::RemoveAt_Fast( mData, mNum, index );
	}

	// deletes the last element
	FORCEINLINE void PopBack()
	{
		Array_Util::PopBack( mData, mNum );
	}


	// Use it only if you know what you're doing.
	// This only works if array capacity is a power of two.
	FORCEINLINE TYPE & AddFast_Unsafe()
	{
		mxSTATIC_ASSERT( TIsPowerOfTwo< SIZE >::value );
		Assert( mNum < SIZE );
		const UINT newIndex = (mNum++) & (SIZE-1);//avoid checking for overflow
		return mData[ newIndex ];
	}
	// only valid for lists with power-of-two capacities
	FORCEINLINE void AddFast_Unsafe( const TYPE& newItem )
	{
		mxSTATIC_ASSERT( TIsPowerOfTwo< SIZE >::value );
		Assert( mNum < SIZE );
		const UINT newIndex = (mNum++) & (SIZE-1);//avoid checking for overflow
		mData[ newIndex ] = newItem;
	}


	// Returns the total amount of occupied memory in bytes.
	FORCEINLINE UINT GetMemoryUsed() const
	{
		return sizeof(*this);
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const THIS_TYPE &o )
	{
		file << o.mNum;
		file.SerializeArray( o.mData, o.mNum );
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, THIS_TYPE &o )
	{
		file >> o.mNum;
		//Assert( data.mNum < data.Max() );
		file.SerializeArray( o.mData, o.mNum );
		return file;
	}
	friend mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		archive && o.mNum;
		TSerializeArray( archive, o.mData, o.mNum );
		return archive;
	}

public:
	// Text formatted data writer.
	class OStream : public AStreamWriter
	{
		THIS_TYPE &	mArray;

	public:
		OStream( THIS_TYPE & dest )
			: mArray( dest )
		{}

		virtual SizeT Write( const void* pBuffer, SizeT numBytes ) override
		{
			const UINT oldSize = mArray.Num();
			const UINT newSize = oldSize + numBytes;

			mArray.SetNum( newSize );

			MemCopy( mArray.ToPtr() + oldSize, pBuffer, numBytes );

			return newSize - oldSize;
		}
	};

	OStream GetOStream()
	{
		return OStream( *this );
	}

public_internal:

	/// For serialization, we want to initialize the vtables
	/// in classes post data load, and NOT call the default constructor
	/// for the arrays (as the data has already been set).
	FORCEINLINE explicit TStaticList( _FinishedLoadingFlag )
	{
	}

private:
	//NO_ASSIGNMENT(THIS_TYPE);
	NO_COPY_CONSTRUCTOR(THIS_TYPE);
	NO_COMPARES(THIS_TYPE);
};

//
//	mxStaticList_InitZeroed
//
template< typename TYPE, const UINT SIZE >
class TStaticList_InitZeroed : public TStaticList< TYPE, SIZE >
{
public:
	TStaticList_InitZeroed()
		: TStaticList(_InitZero)
	{}
};

mxNAMESPACE_END

#endif /* !__T_STATIC_LIST_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
