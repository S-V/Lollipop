/*
=============================================================================
	File:	Array.h
	Desc:	Dynamic (variable sized) templated array.
	The array is always stored in a contiguous chunk.
	The array can be resized.
	A size increase will cause more memory to be allocated,
	and may result in relocation of the array memory.
	A size decrease has no effect on the memory allocation.

	ToDo:	Stop reinventing the wheel.
	Idea:	store size, capacity and memory manager index
			(and other flags) in a single integer (bit mask);
			store size and capacity at the beginning of allocated memory;
			can store additional info in upper 4 bits
			of the (16-byte aligned) pointer to allocated memory.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_ARRAY_H__
#define __MX_CONTAINTERS_ARRAY_H__

#include <Base/Object/ArrayDescriptor.h>

class mxType;
class mxClass;


mxNAMESPACE_BEGIN

namespace Array_Util
{
	// figure out the size for allocating a new buffer
	UINT CalculateNewCapacity( UINT numElements );

}//namespace Array_Util


/*
-----------------------------------------------------------------------------
	TLinearBuffer< TYPE, SIZETYPE >

	- is a resizable array which doubles in size by default.
	Does not allocate memory until the first item is added.
-----------------------------------------------------------------------------
*/
template
<
	typename TYPE,	// type of stored elements
	typename SIZETYPE,	// integer type for storing size and number of allocated elements
	class ALLOCATOR = TDefaultAllocator<TYPE>	// memory manager
>
class TLinearBuffer
	: public TArrayBase< TYPE, TLinearBuffer<TYPE,SIZETYPE,ALLOCATOR> >
{
	TYPE *		mData;	// pointer to the allocated memory
	SIZETYPE	mNum;	// number of elements, this should be inside the range [0..mCapacity)
	SIZETYPE	mCapacity;	// number of allocated entries
	ALLOCATOR	mMemory;	// memory manager

public:
	typedef TLinearBuffer
	<
		TYPE, SIZETYPE, ALLOCATOR
	>
	THIS_TYPE;

	typedef
	TYPE
	ITEM_TYPE;

	typedef
	SIZETYPE
	SIZETYPE;

	// maximum array size, excluding (-1) which is reserved for invalid index
	enum { MAX_CAPACITY = TPow2< sizeof(SIZETYPE) * BITS_IN_BYTE >::value - 1 };

public:

	// Creates a zero length array.
	FORCEINLINE TLinearBuffer()
		: mMemory( EMemHeap::DefaultHeap )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	FORCEINLINE explicit TLinearBuffer( const THIS_TYPE& other )
		: mMemory( other.mMemory )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;

		this->Copy( other );
	}

	// Creates a zero length array and sets the memory manager.
	FORCEINLINE explicit TLinearBuffer( HMemory hMemoryMgr )
		: mMemory( hMemoryMgr )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Creates a zero length array and sets the memory manager.
	template< typename ARG >
	FORCEINLINE TLinearBuffer( EInitCustom, ARG param )
		: mMemory( param )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Use it only if you know what you're doing.
	FORCEINLINE explicit TLinearBuffer(ENoInit)
		: mMemory( EMemHeap::DefaultHeap )
	{}

	// Deallocates array memory.
	FORCEINLINE ~TLinearBuffer()
	{
		this->Clear();
	}

	// Returns the current capacity of this array.
	FORCEINLINE UINT GetCapacity() const
	{
		return mCapacity;
	}

	// Convenience function to get the number of elements in this array.
	// Returns the size (the number of elements in the array).
	FORCEINLINE UINT Num() const
	{
		return mNum;
	}

	FORCEINLINE TYPE * ToPtr() {
		return mData;
	}
	FORCEINLINE const TYPE* ToPtr() const {
		return mData;
	}

	// Convenience function to empty the array. Doesn't release allocated memory.
	// Doesn't call objects' destructors.
	FORCEINLINE void Empty()
	{
		mNum = 0;
	}

	// Convenience function to empty the array. Doesn't release allocated memory.
	// Invokes objects' destructors.
	FORCEINLINE void DestroyAndEmpty()
	{
		TDestructN_IfNonPOD( mData, mNum );
		mNum = 0;
	}

	// Releases allocated memory (calling destructors of elements) and empties the array.
	void Clear()
	{
		if(PtrToBool( mData ))
		{
			TDestructN_IfNonPOD( mData, mNum );
			this->ReleaseMemory( mData );
			mData = nil;
		}
		mNum = 0;
		mCapacity = 0;
	}

	// Resizes the array to exactly the number of elements it contains or frees up memory if empty.
	void Shrink()
	{
		// Condense the array.
		if( mNum > 0 )
		{
			this->Resize( mNum );
		}
		else
		{
			this->Clear();
		}
	}

	// See: http://www.codercorner.com/blog/?p=494
	//
	void EmptyOrClear()
	{
		const UINT capacity = this->GetCapacity();
		const UINT num = this->Num();

		if( num > capacity/2 )
		{
			this->Empty();
		}
		else
		{
			this->Clear();
		}
	}

	// Adds an element to the end.
	FORCEINLINE TYPE & Add( const TYPE& newOne )
	{
		this->Reserve( mNum + 1 );
		mData[ mNum ] = newOne;
		return mData[ mNum++ ];
	}

	// Increments the size by 1 and returns a reference to the first element created.
	FORCEINLINE TYPE & Add()
	{
		this->Reserve( mNum + 1 );
		return mData[ mNum++ ];
	}

	FORCEINLINE void Add( const TYPE* items, UINT numItems )
	{
		const UINT oldNum = mNum;
		const UINT newNum = oldNum + numItems;
		this->SetNum( newNum );
		TCopyArray( mData + oldNum, items, numItems );
	}

	// Slow!
	bool AddUnique( const TYPE& item )
	{
		const UINT num = mNum;
		for( UINT i = 0; i < num; i++ )
		{
			if( mData[i] == item ) {
				return false;
			}
		}
		this->Add( item );
		return true;
	}

	void AddZeroed( UINT numElements )
	{
		const UINT newNum = mNum + numElements;
		this->Reserve( newNum );
		MemZero( (BYTE*)mData + mNum*sizeof(TYPE), numElements*sizeof(TYPE) );
		mNum = newNum;
	}


	// Use it only if you know what you're doing.
	// This only works if 'capacity' is a power of two.
	FORCEINLINE TYPE& AddFast_Unsafe()
	{
		Assert(IsPowerOfTwo( mCapacity ));
		const UINT newIndex = (mNum++) & (mCapacity-1);//avoid checking for overflow
		return mData[ newIndex ];
	}
	FORCEINLINE void AddFast_Unsafe( const TYPE& newOne )
	{
		Assert(IsPowerOfTwo( mCapacity ));
		const UINT newIndex = (mNum++) & (mCapacity-1);//avoid checking for overflow
		mData[ newIndex ] = newOne;
	}



	// Slow!
	FORCEINLINE bool Remove( const TYPE& item )
	{
		return Array_Util::RemoveElement( *this, item );
	}

	// Slow!
	INLINE void RemoveAt( UINT index, UINT count = 1 )
	{
		Array_Util::RemoveAt( mData, mNum, index, count );
	}

	// this method is faster (uses the 'swap trick')
	// Doesn't preserve the relative order of elements.
	FORCEINLINE void RemoveAt_Fast( UINT index )
	{
		Array_Util::RemoveAt_Fast( mData, mNum, index );
	}

	// Removes all occurrences of value in the array
	// and returns the number of entries removed.
	//
	UINT RemoveAll( const TYPE& theValue )
	{
		UINT numRemoved = 0;
		for( UINT i = 0; i < mNum; ++i )
		{
			if( mData[i] == theValue ) {
				this->RemoveAt( i );
				numRemoved++;
			}
		}
		return numRemoved;
	}

	// deletes the last element
	FORCEINLINE void PopBack()
	{
		Array_Util::PopBack( mData, mNum );
	}

	// Slow!
	// inserts a new element at the given index and keeps the relative order of elements.
	FORCEINLINE TYPE & InsertAt( UINT index )
	{
		Assert( this->IsValidIndex( index ) );
		const UINT oldNum = mNum;
		const UINT newNum = oldNum + 1;
		TYPE* data = mData;
		this->Reserve( newNum );
		for ( UINT i = oldNum; i > index; --i )
		{
			data[i] = data[i-1];
		}
		mNum = newNum;
		return data[ index ];
	}

	// Ensures no reallocation occurs until at least size 'numElements'.
	INLINE void Reserve( UINT numElements )
	{
	//	Assert( numElements > 0 );//<- this helped me to catch errors
		Assert( numElements <= MAX_CAPACITY );
		// resize if necessary
		if( numElements > mCapacity )
		{
			const UINT newCapacity = Array_Util::CalculateNewCapacity( numElements );
			this->Resize( newCapacity );
		}
	}

	// Ensures that there's a space for at least the given number of elements.
	FORCEINLINE void ReserveMore( UINT numElements )
	{
		const UINT oldCapacity = mCapacity;
		const UINT newCapacity = oldCapacity + numElements;
		this->Reserve( newCapacity );
	}

	// Sets the new number of elements. Resizes the array if necessary.
	FORCEINLINE void SetNum( UINT numElements )
	{
		// Resize to the exact size specified irregardless of granularity.
		if( numElements > mCapacity )
		{
			const UINT newCapacity = numElements;
			this->Resize( newCapacity );
		}
		mNum = numElements;
	}

	// Sets the new number of elements directly.
	// Use it only if you know what you're doing.
	FORCEINLINE void SetNum_Unsafe( UINT numElements )
	{
		mNum = numElements;
	}
	FORCEINLINE void DecNum_Unsafe()
	{
		--mNum;
	}
	FORCEINLINE void IncNum_Unsafe()
	{
		++mNum;
	}

	// Returns the amount of reserved memory in bytes (memory allocated for storing the elements).
	FORCEINLINE SizeT GetAllocatedMemory() const
	{
		return mCapacity * sizeof(TYPE);
	}

	// Returns the total amount of occupied memory in bytes.
	FORCEINLINE SizeT GetMemoryUsed() const
	{
		return this->GetAllocatedMemory() + sizeof(*this);
	}

	inline friend void F_UpdateMemoryStats( MemStatsCollector& stats, const THIS_TYPE& o )
	{
		stats.staticMem += sizeof o;
		stats.dynamicMem += o.GetAllocatedMemory();
	}


public:	// Reflection.

	typedef
	THIS_TYPE
	ARRAY_TYPE;

	class ArrayDescriptor : public mxArrayType
	{
	public:
		inline ArrayDescriptor( const char* typeName )
			: mxArrayType( typeName, STypeDescription::For_Type<ARRAY_TYPE>(), T_DeduceTypeInfo<ITEM_TYPE>() )
		{}

		//==- mxArrayType

		virtual UINT Generic_Get_Count( const void* pArrayObject ) const override
		{
			const ARRAY_TYPE* theArray = static_cast< const ARRAY_TYPE* >( pArrayObject );
			return theArray->Num();
		}
		virtual void Generic_Set_Count( void* pArrayObject, UINT newNum ) const override
		{
			ARRAY_TYPE* theArray = static_cast< ARRAY_TYPE* >( pArrayObject );
			return theArray->SetNum( newNum );
		}
		virtual void* Generic_Get_Data( void* pArrayObject ) const override
		{
			ARRAY_TYPE* theArray = static_cast< ARRAY_TYPE* >( pArrayObject );
			return theArray->ToPtr();
		}
		virtual const void* Generic_Get_Data( const void* pArrayObject ) const override
		{
			const ARRAY_TYPE* theArray = static_cast< const ARRAY_TYPE* >( pArrayObject );
			return theArray->ToPtr();
		}
	};

public:	// Binary Serialization.

	// used to relocate pointers during in-place loading
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		s.ProcessPointer( mData, mNum, p );
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const THIS_TYPE& o )
	{
		file << o.mNum;

		file.SerializeArray( o.mData, o.mNum );

		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, THIS_TYPE& o )
	{
		SIZETYPE number;
		file >> number;
		o.SetNum( number );

		file.SerializeArray( o.mData, o.mNum );

		return file;
	}
	friend mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		SIZETYPE num = o.Num();
		archive && num;

		if( archive.IsReading() )
		{
			o.SetNum( num );
		}

		TSerializeArray( archive, o.mData, num );

		return archive;
	}


public:
	//TODO: make special Shrink(), ReserveAndGrowByHalf(newCapacity) and ReserveAndGrowByNumber(newCapacity,granularity) ?

	//TODO: sorting, binary search, algorithms & iterators

	// Deep copy. Slow!
	THIS_TYPE & operator = ( const THIS_TYPE& other )
	{
		return this->Copy( other );
	}

	THIS_TYPE& Copy( const THIS_TYPE& other )
	{
		//@todo: copy memory allocator?
		this->SetNum( other.mNum );
		//if( mNum )
		{
			TCopyArray( mData, other.mData, mNum );
		}
		return *this;
	}

	void AddBytes( const void* src, SizeT numBytes )
	{
		mxSTATIC_ASSERT( sizeof TYPE == sizeof BYTE );
		const SizeT oldNum = mNum;
		const SizeT newNum = oldNum + numBytes;
		this->SetNum( newNum );
		MemCopy( (BYTE*)mData + oldNum, src, numBytes );
	}

	template< typename U >
	void CopyFromArray( const U* src, UINT num )
	{
		this->SetNum( num );
		for( UINT i = 0; i < num; i++ )
		{
			mData[ i ] = src[ i ];
		}
	}

	void Append( const THIS_TYPE& other )
	{
		const SizeT oldNum = mNum;
		const SizeT newNum = oldNum + other.Num();

		this->SetNum( newNum );

		//if( newNum )
		{
			TCopyArray( mData + oldNum, other.mData, other.Num() );
		}
	}

	// works only with types that can be copied via assignment
	// returns the number of removed elements
	//
	template< class FUNCTOR >
	UINT Do_RemoveIf( FUNCTOR& functor )
	{
		const UINT oldNum = mNum;
		UINT newNum = 0;
		for( UINT i = 0; i < oldNum; i++ )
		{
			// if no need to remove this element
			if( !functor( mData[i] ) )
			{
				// then copy it
				mData[ newNum++ ] = mData[ i ];
			}
			// otherwise, skip it
		}
		mNum = newNum;
		return (oldNum - newNum);
	}

public:
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

			MemCopy( (BYTE*)mArray.ToPtr() + oldSize, pBuffer, numBytes );

			return newSize - oldSize;
		}
	};
	OStream GetOStream()
	{
		return OStream( *this );
	}

public:
	// Testing, Checking & Debugging.

	bool DbgCheckSelf() const
	{
		//CHK_VRET_FALSE_IF_NOT( IsPowerOfTwo( mCapacity ) );
		CHK_VRET_FALSE_IF_NOT( mCapacity <= MAX_CAPACITY );
		CHK_VRET_FALSE_IF_NOT( mNum <= mCapacity );//this can happen after AddFast_Unsafe()
		CHK_VRET_FALSE_IF_NOT( mData );
		CHK_VRET_FALSE_IF_NOT( sizeof(SIZETYPE) <= sizeof(UINT) );//need to impl size_t for 64-bit systems
		return true;
	}

private:
	FORCEINLINE void ReleaseMemory( void* ptr )
	{
		mMemory.ReleaseMemory( ptr );
	}

	void Resize( UINT newCapacity )
	{
		Assert( newCapacity > 0 );

		// Allocate a new memory buffer
		TYPE * newArray = c_cast(TYPE*) mMemory.AllocateMemory( newCapacity * sizeof(TYPE) );

		TYPE * const oldArray = mData;
		const UINT oldNum = mNum;

#if 0
		// call default constructors
		TConstructN_IfNonPOD( newArray, newCapacity );

		if(PtrToBool( oldArray ))
		{
			if( oldNum )
			{
				// copy the old list into our new one
				TCopyArray( newArray, oldArray, oldNum );
				// destroy previous contents
				TDestructN_IfNonPOD( oldArray, oldNum );
			}
			// deallocate old memory buffer
			this->ReleaseMemory( oldArray );
		}
#else
		if( PtrToBool( oldArray ) && oldNum )
		{
			// copy-construct the new elements
			TCopyConstructArray( newArray, oldArray, oldNum );
			// destroy the old contents
			TDestructN_IfNonPOD( oldArray, oldNum );
			// deallocate old memory buffer
			this->ReleaseMemory( oldArray );
		}

		// call default constructors for the rest
		const UINT numNewItems = newCapacity - oldNum;
		TConstructN_IfNonPOD( newArray + oldNum, numNewItems );
#endif

		mData = newArray;
		mCapacity = newCapacity;

		this->DbgCheckSelf();
	}

public_internal:

	/// For serialization, we want to initialize the vtables
	/// in classes post data load, and NOT call the default constructor
	/// for the arrays (as the data has already been set).
	FORCEINLINE explicit TLinearBuffer( _FinishedLoadingFlag )
	{
	}

private:
	//NO_ASSIGNMENT(THIS_TYPE);
	//NO_COPY_CONSTRUCTOR(THIS_TYPE);
	NO_COMPARES(THIS_TYPE);
};



//
//	TBuffer< TYPE > - is a 16-bit array which doubles in size by default.
//
//	NOTE: its capacity can only be a power of two.
//	NOTE: memory alignment is EFFICIENT_ALIGNMENT (usually 16-byte aligned).
//
// It occupies 12 bytes (using Visual C++ 9.0 on 32-bit Windows)
// 0 - mData (size=4)
// 4 - mNum (size=2)
// 6 - mCapacity (size=2)
// 8 - mMemory (size=2)
// 2 - alignment (size=2)
//
template< typename TYPE >
class TSmallList : public TLinearBuffer< TYPE, U2 >
{
public:
	TSmallList()
		: TLinearBuffer()
	{}

	explicit TSmallList( HMemory hMemoryMgr )
		: TLinearBuffer( hMemoryMgr )
	{}

	explicit TSmallList( _FinishedLoadingFlag )
		: TLinearBuffer( _FinishedLoadingFlag )
	{}
};

//
//	TBigBuffer< TYPE > - Array-based implementation of a list.
//
//	It is a 32-bit array which doubles in size by default.
//
//	NOTE: its capacity can only be a power of two.
//	NOTE: memory alignment is EFFICIENT_ALIGNMENT (usually 16-byte aligned).
//
// It occupies 16 bytes (using Visual C++ 9.0 on 32-bit Windows)
// 0 - mData (size=4)
// 4 - mNum (size=4)
// 8 - mCapacity (size=4)
// 12 - mMemory (size=2)
// 2 - alignment (size=2)
//
template< typename TYPE >
class TList : public TLinearBuffer< TYPE, U4 >
{
public:
	TList()
		: TLinearBuffer()
	{}

	explicit TList( HMemory hMemoryMgr )
		: TLinearBuffer( hMemoryMgr )
	{}

	explicit TList( _FinishedLoadingFlag x )
		: TLinearBuffer( x )
	{}
};



template< typename TYPE >
class TStack : public TLinearBuffer< TYPE, U4 >
{
public:
	TStack()
		: TLinearBuffer()
	{}

	explicit TStack( HMemory hMemoryMgr )
		: TLinearBuffer( hMemoryMgr )
	{}

	explicit TStack( _FinishedLoadingFlag x )
		: TLinearBuffer( x )
	{}
};




//---------------------------------------------------------------------------
// Reflection.
//
template< typename TYPE >
struct TypeDeducer< TList< TYPE > >
{
	static inline const mxType& GetType()
	{
		static TList< TYPE >::ArrayDescriptor staticTypeInfo("TList");
		return staticTypeInfo;
	}
	static inline ETypeKind GetTypeKind()
	{
		return ETypeKind::Type_Array;
	}
};



//---------------------------------------------------------------------------

namespace Array_Util
{
	template< class KLASS >	// where KLASS is reference counted via Grab()/Drop()
	inline void DropReferences( TList<KLASS*> & a )
	{
		struct {
			inline void operator () ( KLASS* o )
			{
				o->Drop();
			}
		} dropThem;

		a.Do_ForAll( dropThem );
	}

}//namespace Array_Util

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
