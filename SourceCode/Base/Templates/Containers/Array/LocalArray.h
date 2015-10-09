/*
=============================================================================
	File:	LocalArray.h
	Desc:	One dimensional fixed-size array template (cannot be resized).
	Note:	This array can never grow and reallocate memory
			so it should be safe to store pointers to its contents.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_LOCAL_ARRAY_H__
#define __MX_CONTAINTERS_LOCAL_ARRAY_H__

mxNAMESPACE_BEGIN

#error Unfinished pile of crap.

#if 0
mxSWIPED("comments from Havok")
/// A array class which uses hkMemory stack based allocations.
/// Stack allocation patterns can be much faster than heap allocations.
/// See the hkMemory/Frame based allocation user guide section.
/// When an hkLocalArray grows beyond its original specified capacity, it
/// falls back on heap allocations so it is important to specify a good
/// initial capacity.
/// hkLocalArray should almost always be a local variable in a
/// method and almost never a member of an object.

//
//	TLocalArray< TYPE, SIZETYPE >
//
template< typename TYPE, typename SIZETYPE >
class TLocalArray
{
	TYPE *			mData;	// pointer to the allocated memory
	SIZETYPE		mNum;	// number of elements, this should be inside the range [0..mCapacity)
	SIZETYPE		mCapacity;	// number of allocated entries
	const HMemory	mMemory;	// Handle to the memory manager

public:
	typedef TLocalArray
	<
		TYPE, SIZETYPE
	>
	THIS_TYPE;

	// maximum array size, excluding (-1) which is reserved for invalid index
	enum { MAX_CAPACITY = TPow2< sizeof(SIZETYPE) * BITS_IN_BYTE >::value - 1 };

	// Creates a zero length array.
	FORCEINLINE TLocalArray()
		: mMemory( EMemHeap::DefaultHeap )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Creates a zero length array and sets the memory manager.
	FORCEINLINE explicit TLocalArray( HMemory hMemoryMgr )
		: mMemory( hMemoryMgr )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Use it only if you know what you're doing.
	FORCEINLINE explicit TLocalArray(ENoInit)
		: mMemory( EMemHeap::DefaultHeap )
	{}

	// Deallocates array memory.
	FORCEINLINE ~TLocalArray()
	{
		Clear();
	}

#if 0
	// One-time init
	void Reserve( UINT maxNumber )
	{
		AllocateMemory(maxNumber);
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

	// Checks if the size is zero.
	FORCEINLINE bool IsEmpty() const
	{
		return !mNum;
	}

	// Convenience function to empty the array. Doesn't release allocated memory.
	// Invokes objects' destructors.
	FORCEINLINE void Empty()
	{
		Unimplemented;
		if( !TYPE_TRAIT::IsPlainOldDataType )
		{
			TDestructN< TYPE >( mData, mNum );
		}
		mNum = 0;
	}

	// Releases allocated memory (calling destructors of elements) and empties the array.
	FORCEINLINE void Clear()
	{
		if( mData )
		{
			if( !TYPE_TRAIT::IsPlainOldDataType )
			{
				TDestructN< TYPE >( mData, mNum );
			}
			ReleaseMemory( mData );
			mData = nil;
		}
		mNum = 0;
		mCapacity = 0;
	}

	// Returns true if the index is within the boundaries of this array.
	FORCEINLINE bool IsValidIndex( UINT index ) const
	{
		return (index >= 0) && (index < mNum);
	}

	FORCEINLINE TYPE & GetLast()
	{
		Assert(mNum>0);
		return mData[ mNum-1 ];
	}
	FORCEINLINE const TYPE & GetLast() const
	{
		Assert(mNum>0);
		return mData[ mNum-1 ];
	}

	// Returns the index of the first occurrence of the given element, or INDEX_NONE if not found.
	// Slow! (uses linear search)
	INLINE int FindIndexOf( const TYPE& element ) const
	{
		for( UINT i = 0; i < mNum; i++ ) {
			if( element == mData[ i ] ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	INLINE int FindPtrIndex( const void* pointer ) const
	{
		for( UINT i = 0; i < mNum; i++ ) {
			if( pointer == &mData[ i ] ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Returns index of the last occurrence of the given element, or INDEX_NONE if not found.
	INLINE int LastIndexOf( const TYPE& element ) const
	{
		for(UINT i = mNum-1; i >= 0; i++) {
			if( element == mData[ i ] ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Searches for the given element and returns a pointer to it, or NULL if not found.
	INLINE TYPE* Find( const TYPE& element ) const
	{
		for( UINT i = 0; i < mNum; i++ ) {
			if( element == mData[ i ] ) {
				return &(mData[ i ]);
			}
		}
		return nil;
	}
	
	// Read/write access to the i'th element.
	FORCEINLINE TYPE& operator [] ( UINT i )
	{
		Assert( IsValidIndex( i ) );
		return mData[ i ];
	}
	// Read only access to the i'th element.
	FORCEINLINE const TYPE& operator [] ( UINT i ) const
	{
		Assert( IsValidIndex( i ) );
		return mData[ i ];
	}

	// Returns null if the index is out of array bounds.
	FORCEINLINE TYPE * SafeGetItemAt( UINT index ) {
		if( IsValidIndex( index ) ) {
			return mData + index;
		}
		return nil;
	}

	FORCEINLINE TYPE * ToPtr() {
		return mData;
	}
	FORCEINLINE const TYPE * ToPtr() const {
		return mData;
	}
	
	FORCEINLINE TYPE & At( UINT index )
	{
		Assert( IsValidIndex( index ) );
		return mData[ index ];
	}
	FORCEINLINE const TYPE & At( UINT index ) const
	{
		Assert( IsValidIndex( index ) );
		return mData[ index ];
	}

	FORCEINLINE TYPE * GetItemPtr( UINT index )
	{
		Assert( IsValidIndex( index ) );
		return mData + index;
	}
	FORCEINLINE const TYPE * GetItemPtr( UINT index ) const
	{
		Assert( IsValidIndex( index ) );
		return mData + index;
	}

	// Adds an element to the end.
	FORCEINLINE TYPE& Add( const TYPE& newOne )
	{
		Assert( mNum < GetCapacity() );
		mData[ mNum ] = newOne;
		return mData[ mNum++ ];
	}

	// Increments the size by 1 and returns a reference to the first element created.
	FORCEINLINE TYPE & Add()
	{
		Assert( mNum < GetCapacity() );
		return mData[ mNum++ ];
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

	void AddZeroed( UINT numElements )
	{
		Assert( mNum + numElements < GetCapacity() );
		MemZero( (BYTE*)mData + mNum*sizeof(TYPE), numElements*sizeof(TYPE) );
	}

	// Use it only if you know what you're doing.
	// This only works if mCapacity is a power of two.
	FORCEINLINE TYPE & AddFast_Unsafe()
	{
		Assert(IsPowerOfTwoU( mCapacity ));
		const UINT newIndex = (mNum++) & (mCapacity-1);//avoid checking for overflow
		return mData[ newIndex ];
	}
	FORCEINLINE void AddFast_Unsafe( const TYPE& newOne )
	{
		Assert(IsPowerOfTwoU( mCapacity ));
		const UINT newIndex = (mNum++) & (mCapacity-1);//avoid checking for overflow
		mData[ newIndex ] = newOne;
	}

	// Slow!
	FORCEINLINE bool Remove( const TYPE& item )
	{
		int index = FindIndexOf( item );
		if( INDEX_NONE == index ) {
			return false;
		}
		RemoveAt( index );
		return true;
	}

	// Slow!
	INLINE void RemoveAt( UINT index )
	{
		Assert( IsValidIndex( index ) );
		--mNum;
		for( UINT i = index; i < mNum; i++ ) {
			mData[i] = mData[i+1];
		}
	}

	// Doesn't preserve the relative order of elements.
	FORCEINLINE void RemoveAt_Fast( UINT index )
	{
		Assert( IsValidIndex( index ) );
		--mNum;
		mData[ index ] = mData[ mNum ];
	}


	// Sets the new number of elements. Resizes the array if necessary.
	FORCEINLINE void SetNum( UINT numElements )
	{
		AllocateMemory( numElements );
		mNum = numElements;
	}

	// Returns the total size of stored elements, in bytes.
	FORCEINLINE SizeT GetDataSize() const
	{
		return mNum * sizeof(TYPE);
	}

	// Returns the amount of reserved memory in bytes (memory allocated for storing the elements).
	FORCEINLINE SizeT GetAllocatedMemory() const
	{
		return mCapacity * sizeof(TYPE);
	}

	// Returns the total amount of occupied memory in bytes.
	FORCEINLINE SizeT GetMemoryUsed() const
	{
		return GetAllocatedMemory() + sizeof(*this);
	}


	// Serialization.

	friend mxArchive& operator << ( mxArchive& file, const THIS_TYPE& o )
	{
		file << o.mNum;
		for( UINT i = 0; i < o.mNum; i++ )
		{
			file << o.mData[i];
		}
		return file;
	}
	friend mxArchive& operator >> ( mxArchive& file, THIS_TYPE& o )
	{
		UINT number;
		file >> number;
		o.SetNum( number );
		for( UINT i = 0; i < number; i++ )
		{
			file >> o.mData[i];
		}
		return file;
	}


	//TODO: sorting, binary search, algorithms & iterators


	// Algorithms.


	template< class FUNCTOR >
	void Do_ForEach( FUNCTOR& functor, UINT startIndex, UINT endIndex )
	{
		for( UINT i = startIndex; i < endIndex; i++ ) {
			functor( (this*)[ i ] );
		}
	}
	template< class FUNCTOR >
	void Do_ForAll( FUNCTOR& functor )
	{
		for( UINT i = 0; i < mNum; i++ ) {
			functor( mData[ i ] );
		}
	}

	template< class FUNCTOR >
	TYPE* Do_FindFirst( FUNCTOR& functor )
	{
		for( UINT i = 0; i < mNum; i++ ) {
			if( functor( mData[i] ) ) {
				return &mData[i];
			}
		}
		return nil;
	}
	template< class FUNCTOR >
	INT Do_FindFirstIndex( FUNCTOR& functor )
	{
		for( UINT i = 0; i < mNum; i++ ) {
			if( functor( mData[i] ) ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Testing, Checking & Debugging.

	bool DbgCheckSelf()
	{
		return 1
		//	&& CHK( IsPowerOfTwoU( mCapacity ) )
			&& CHK( mCapacity <= MAX_CAPACITY )
			&& CHK( mNum <= mCapacity )//this can happen after AddFast_Unsafe()
			&& CHK( mData )
			&& CHK( sizeof(SIZETYPE) <= sizeof(UINT) )//need to impl size_t for 64-bit systems
			;
	}

private:
	FORCEINLINE void ReleaseMemory( TYPE* ptr )
	{
		F_GetMemoryManager( mMemory )->Free( ptr );
	}

	void AllocateMemory( UINT capacity )
	{
		AssertX( !mData && !mCapacity, "This function can only be called once.");

		// Allocate a new memory buffer
		mData = (TYPE*) F_GetMemoryManager( mMemory )->Allocate( capacity * sizeof(TYPE) );

		// call default constructors
		if( !TYPE_TRAIT::IsPlainOldDataType )
		{
			TConstructN< TYPE >( mData, capacity );
		}

		mCapacity = capacity;
		DbgCheckSelf();
	}
#endif


private:	PREVENT_COPY(THIS_TYPE);
};
#endif

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_LOCAL_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
