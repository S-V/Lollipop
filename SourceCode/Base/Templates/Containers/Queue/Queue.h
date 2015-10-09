/*
=============================================================================
	File:	Queue.h
	Desc:	A FIFO circular queue.
	ToDo:	Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_CONTAINTERS_FIFO_QUEUE_H__
#define __MX_CONTAINTERS_FIFO_QUEUE_H__

mxNAMESPACE_BEGIN

//
//	TQueue< TYPE >
//
template< typename TYPE >
class TQueue
	: public TArrayBase< TYPE, TQueue<TYPE> >
{
	// queue memory management
	TList< TYPE >	mData;

public:
	typedef TQueue
	<
		TYPE
	>
	THIS_TYPE;

	typedef
	TYPE
	ITEM_TYPE;

	typedef TypeTrait
	<
		TYPE
	>
	TYPE_TRAIT;

public:
	// Creates a zero length queue.
	FORCEINLINE TQueue()
	{}

	// Creates a zero length queue and sets the memory manager.
	FORCEINLINE explicit TQueue( HMemory hMemoryMgr )
		: mData( hMemoryMgr )
	{}

	// Use it only if you know what you're doing.
	FORCEINLINE explicit TQueue(ENoInit)
		: mData( EMemHeap::DefaultHeap )
	{}

	// Deallocates queue memory.
	FORCEINLINE ~TQueue()
	{
		this->Clear();
	}

	// Returns the total capacity of the queue storage.
	FORCEINLINE UINT GetCapacity() const
	{
		return mData.GetCapacity();
	}

	// Convenience function to get the number of elements in this queue.
	FORCEINLINE UINT Num() const
	{
		return mData.Num();
	}

	FORCEINLINE void Reserve( UINT numElements )
	{
		mData.Reserve( numElements );
	}

	// Convenience function to empty the queue. Doesn't release allocated memory.
	// Invokes objects' destructors.
	FORCEINLINE void Empty()
	{
		mData.Empty();
	}

	// Releases allocated memory (calling destructors of elements) and empties the queue.
	FORCEINLINE void Clear()
	{
		mData.Clear();
	}

	// Accesses the element at the front of the queue but does not remove it.
	FORCEINLINE TYPE& Peek()
	{
		return mData.GetFirst();
	}
	FORCEINLINE const TYPE& Peek() const
	{
		return mData.GetFirst();
	}

	// Places a new element to the back of the queue and expand storage if necessary.
	FORCEINLINE void Enqueue( const TYPE& newOne )
	{
		mData.Add( newOne );
	}

	// Fills in the data with the element at the front of the queue
	// and removes the element from the front of the queue.
	FORCEINLINE void Deque( TYPE &element )
	{
		element = mData.GetFirst();
		mData.RemoveAt( 0 );
	}
	FORCEINLINE void Deque()
	{
		mData.RemoveAt( 0 );
	}

	FORCEINLINE void Add( const TYPE& newOne )
	{
		Enqueue( newOne );
	}
	FORCEINLINE TYPE& Add()
	{
		return mData.Add();
	}

	FORCEINLINE TYPE* ToPtr() {
		return mData.ToPtr();
	}
	FORCEINLINE const TYPE* ToPtr() const {
		return mData.ToPtr();
	}


public_internal:
	void SetNum_Unsafe( UINT newNum )
	{
		mData.SetNum_Unsafe( newNum );
	}

private:
	NO_ASSIGNMENT(THIS_TYPE);
	NO_COPY_CONSTRUCTOR(THIS_TYPE);
	NO_COMPARES(THIS_TYPE);
};

mxNAMESPACE_END

#endif // ! __MX_CONTAINTERS_QUEUE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
