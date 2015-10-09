/*
=============================================================================
	File:	TPool.h
	Desc:	Object pool for fixed-size allocations.
=============================================================================
*/

#ifndef MX_TEMPLATED_OBJECT_POOL_H__
#define MX_TEMPLATED_OBJECT_POOL_H__

#include <Base/Templates/Containers/Array/TFixedArray.h>

mxNAMESPACE_BEGIN

template< typename TYPE >
class TPool
{
public:
	typedef TPool
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
	enum { DEFAULT_GRANULARITY = 16 };

	FORCEINLINE explicit TPool( UINT granularity = DEFAULT_GRANULARITY )
		: mMemory( EMemHeap::DefaultHeap )
	{
		Assert(granularity > 0);
		mGranularity = granularity;
	}

	FORCEINLINE explicit TPool( UINT granularity, HMemory hMemoryMgr )
		: mMemory( hMemoryMgr )
	{
		Assert(granularity > 0);
		mGranularity = granularity;
	}

	// Deallocates array memory.
	FORCEINLINE ~TPool()
	{
		Clear();
	}

	// default constructor is called
	FORCEINLINE TYPE & Add()
	{
		if( !mHead ) {
			mHead = NewMemArea();
		}
		if( !mHead->IsFull() ) {
			mHead = NewMemArea();
		}
		TYPE& newItem = mHead->AllocateNewItem();
		return newItem;
	}

	// Releases allocated memory (calling destructors of elements) and empties the array.
	FORCEINLINE void Clear()
	{
		// Destroy memory heaps.

		Entry* entry = mHead;
		while( entry )
		{
			Entry* nextEntry = entry->next;

			entry->Destroy();

			entry = nextEntry;
		}

		mHead = nil;
	}

	FORCEINLINE void SetGranularity( UINT newGranularity )
	{
		Assert( newGranularity > 0 );
		mGranularity = newGranularity;
	}

	FORCEINLINE UINT GetGranularity() const
	{
		return mGranularity;
	}

private:
	struct Entry
	{
		TYPE *	items;	// pooled items are located right after this structure
		UINT	numTaken;	// number of taken slots
		UINT	capacity;
		Entry *	next;

	public:
		bool IsFull() const
		{
			return numTaken == capacity;
		}
		TYPE& AllocateNewItem()
		{
			Assert(numTaken < capacity);
			return items[ numTaken++ ];
		}
		void Destroy()
		{
			TDestructN_IfNonPOD( items, numTaken );
			items = nil;
			numTaken = 0;
			capacity = 0;
			next = nil;
		}
	};

	Entry* NewMemArea()
	{
		const UINT newItemsSize = mGranularity;
		const SizeT newEntrySize = sizeof(Entry) + newItemsSize * sizeof(TYPE);

		Entry* newEntry = (Entry*) AllocateMemory( newEntrySize );

		// pooled items are located right after this structure
		TYPE* newItems = (TYPE*) (newEntry + sizeof Entry);

		// call default constructors
		if( !TYPE_TRAIT::IsPlainOldDataType )
		{
			TConstructN< TYPE >( newItems, newItemsSize );
		}

		newEntry->next = mHead;
		mHead = newEntry;

		newEntry->capacity = newItemsSize;
		newEntry->numTaken = 0;

		newEntry->items = newItems;

		return newEntry;
	}


	FORCEINLINE void* AllocateMemory( SizeT numBytes )
	{
		return F_GetMemoryManager( mMemory )->Allocate( numBytes );
	}
	FORCEINLINE void ReleaseMemory( TYPE* ptr )
	{
		F_GetMemoryManager( mMemory )->Free( ptr );
	}

private:	PREVENT_COPY(THIS_TYPE);
private:
	// memory management
	TPtr< Entry >	mHead;
	UINT	mGranularity;	// size (in objects) of new allocated entries
	const HMemory	mMemory;	// Handle to the memory manager
};

mxNAMESPACE_END

#endif // MX_TEMPLATED_OBJECT_POOL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
