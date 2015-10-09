/*
=============================================================================
	File:	TFixedArray.h
	Desc:	One dimensional fixed-size array template (cannot be resized).
	Note:	This array can never grow and reallocate memory
			so it should be safe to store pointers to its contents.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_FIXED_ARRAY_H__
#define __MX_CONTAINTERS_FIXED_ARRAY_H__

mxNAMESPACE_BEGIN

//
//	TFixedSizeArray< TYPE, SIZETYPE >
//
template< typename TYPE, typename SIZETYPE >
class TFixedSizeArray
	: public TArrayBase< TYPE, TFixedSizeArray<TYPE,SIZETYPE> >
{
	TYPE *			mData;	// pointer to the allocated memory
	SIZETYPE		mNum;	// number of elements, this should be inside the range [0..mCapacity)
	SIZETYPE		mCapacity;	// number of allocated entries
	const HMemory	mMemory;	// Handle to the memory manager

public:
	typedef TFixedSizeArray
	<
		TYPE, SIZETYPE
	>
	THIS_TYPE;

	typedef
	SIZETYPE
	SIZETYPE;

	// maximum array size, excluding (-1) which is reserved for invalid index
	enum { MAX_CAPACITY = TPow2< sizeof(SIZETYPE) * BITS_IN_BYTE >::value - 1 };

public:
	// Creates a zero length array.
	FORCEINLINE TFixedSizeArray()
		: mMemory( EMemHeap::DefaultHeap )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Creates a zero length array and sets the memory manager.
	FORCEINLINE explicit TFixedSizeArray( HMemory hMemoryMgr )
		: mMemory( hMemoryMgr )
	{
		mData = nil;
		mNum = 0;
		mCapacity = 0;
	}

	// Use it only if you know what you're doing.
	FORCEINLINE explicit TFixedSizeArray(ENoInit)
		: mMemory( EMemHeap::DefaultHeap )
	{}

	// Deallocates array memory.
	FORCEINLINE ~TFixedSizeArray()
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

	// One-time init.
	// Sets the number of elements.
	FORCEINLINE void SetNum( UINT numElements )
	{
		this->AllocateMemory( numElements );
		mNum = numElements;
	}

	// Convenience function to empty the array. Doesn't release allocated memory.
	// Invokes objects' destructors.
	FORCEINLINE void Empty()
	{
		TDestructN_IfNonPOD( mData, mNum );
		mNum = 0;
	}

	// Releases allocated memory (calling destructors of elements) and empties the array.
	FORCEINLINE void Clear()
	{
		if( mData )
		{
			TDestructN_IfNonPOD( mData, mNum );
			this->ReleaseMemory( mData );
			mData = nil;
		}
		mNum = 0;
		mCapacity = 0;
	}

	FORCEINLINE TYPE * ToPtr() {
		return mData;
	}
	FORCEINLINE const TYPE * ToPtr() const {
		return mData;
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

public:
	// Serialization.

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

		TSerializeArray( archive, o.ToPtr(), num );

		return archive;
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
		TConstructN_IfNonPOD( mData, capacity );

		mCapacity = capacity;
		this->DbgCheckSelf();
	}

private:	PREVENT_COPY(THIS_TYPE);
};

template< typename TYPE >
class TFixedArray : public TFixedSizeArray< TYPE, U4 >
{};

template< typename TYPE >
class TFixedPool : public TFixedArray< TYPE >
{};

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_FIXED_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
