/*
=============================================================================
	File:	TDynaMap.h
	Desc:	A very simple (closed) dynamic hash map.
	Note:	Programmer uses hash table size that is power of 2
			because address calculation can be performed very quickly.
			The integer hash function can be used to post condition the output
			of a marginal quality hash function before the final address calculation is done. 
			Using the inlined version of the integer hash function is faster
			than doing a remaindering operation with a prime number!
			An integer remainder operation may take up to 18 cycles or longer to complete,
			depending on machine architecture.
	ToDo:	Try tables with a non-power-of-two sizes (using prime numbers);
			some say that by AND-ing with (tableSize-1) you effectively limit
			the range of possible values of hash keys.
=============================================================================
*/

#ifndef __MX_TEMPLATE_DYNAMIC_MAP_H__
#define __MX_TEMPLATE_DYNAMIC_MAP_H__

#include <Base/Templates/Containers/HashMap/TMap.h>

mxNAMESPACE_BEGIN

//
//	TDynaMap< KEY, VALUE > -  An unordered data structure mapping keys to values.
//
//	NOTE: KEY should have member function 'UINT GetHashCode( const KEY& key )'
//	and 'bool operator == (const KEY& other) const'.
//
//	Periodically check that DbgGetLoad() is low (> 0.1). When it gets near
//	1.0 your hash function is badly designed and maps too many inputs to
//	the same output.
//
template<
	typename KEY,
	typename VALUE,
	class HASH_FUNC = THashTrait< KEY >,
	class EQUALS_FUNC = TEqualsTrait< KEY >
>
class TDynaMap {
public:
	typedef TDynaMap
	<
		KEY,
		VALUE,
		HASH_FUNC,
		EQUALS_FUNC
	> THIS_TYPE;

	enum { DEFAULT_HASH_TABLE_SIZE = 128 };

	inline explicit TDynaMap( ENoInit )
		: mMemory( EMemHeap::DefaultHeap )
	{
		mTable = nil;
		mTableMask = 0;
		mNumEntries = 0;
	}

	explicit TDynaMap( UINT tableSize = DEFAULT_HASH_TABLE_SIZE )
		: mMemory( EMemHeap::DefaultHeap )
	{
		mTable = nil;
		mTableMask = 0;
		mNumEntries = 0;
		this->Setup( tableSize );
	}

	inline ~TDynaMap()
	{
		this->Clear();
	}

	void Setup( UINT tableSize )
	{
		Assert(tableSize > 1 && IsPowerOfTwoU(tableSize));
		Assert(nil == mTable && !mNumEntries);
		UINT numBytes = tableSize * sizeof(mTable[0]);
		mTable = (Node**)this->Allocate( numBytes );
		MemSet( mTable, nil, numBytes );
		mTableMask = tableSize - 1;
	}

	// Removes all elements from the table.
	void Empty()
	{
		UINT tableSize = this->GetTableSize();
		if( tableSize )
		{
			this->DeleteNodes();
			UINT numBytes = tableSize * sizeof(mTable[0]);
			MemSet( mTable, nil, numBytes );
		}
		mNumEntries = 0;
	}

	// Removes all elements from the table and releases allocated memory.

	void Clear()
	{
		this->DeleteNodes();
		if( mTable ) {			
			this->Free( mTable );
			mTable = nil;
		}
		mTableMask = 0;
		mNumEntries = 0;
	}

	// Returns a pointer to the element if it exists, or nil if it does not.

	VALUE* Find( const KEY& key )
	{
		AssertPtr(mTable);
		UINT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;

		Node * node = mTable[ hash ];
		while( node )
		{
			if( EQUALS_FUNC::Equals( node->key, key ) ) {
				return &(node->value);
			}
			node = node->next;
		}
		return nil;
	}

	// Returns a pointer to the element if it exists, or nil if it does not.

	const VALUE* Find( const KEY& key ) const
	{
		return const_cast< THIS_TYPE* >( this )->Find( key );
	}

	// Returns a reference to the element if it exists, or a default-constructed value if it does not.

	VALUE FindRef( const KEY& key )
	{
		AssertPtr(mTable);
		UINT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		Node * node = mTable[ hash ];
		while( node )
		{
			if( EQUALS_FUNC::Equals( node->key, key ) ) {
				return (node->value);
			}
			node = node->next;
		}
		return (VALUE) nil;
	}

	// Returns key by value. Returns nil pointer if it's not in the table. Slow!

	KEY* FindKey( const VALUE& value )
	{
		UINT numBuckets = GetTableSize();
		for(UINT iBucket = 0; iBucket < numBuckets; iBucket++)
		{
			Node * node = mTable[ iBucket ];
			while( node )
			{
				if( node->value == value ) {
					return &(node->key);
				}
				node = node->next;
			}
		}
		return nil;
	}

	// Inserts a (key,value) pair into the table.

	VALUE& Set( const KEY& key, const VALUE& value )
	{
		AssertPtr(mTable);
		UINT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		Node * node = mTable[ hash ];
		while( node )
		{
			if( EQUALS_FUNC::Equals( node->key, key ) ) {
				node->value = value;
				return node->value;
			}
			node = node->next;
		}

		++mNumEntries;

		Node * newNode = this->AllocNode( key, value, mTable[ hash ] );
		mTable[ hash ] = newNode;
		return newNode->value;
	}

	// Removes all pairs matching the specified key, returns true if any items have been removed.
	// This may cause a potentially slow rehashing.

	bool Remove( const KEY& key )
	{
		UINT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		Node **head = &mTable[ hash ];
		Node * prev = nil;
		Node * node = *head;
		while( node )
		{
			if( EQUALS_FUNC::Equals( node->key, key ) )
			{
				// remove (unlink) and delete the matching node
				if( prev ) {
					// this node had a predecessor
					prev->next = node->next;
				} else {
					// this node was a head pointer and had no predecessor
					*head = node->next;
				}
				this->DestroyNode( node );
				--mNumEntries;
				return true;
			}
			prev = node;
			node = node->next;
		}
		return false;
	}

	// Returns the number of buckets.
	FORCEINLINE UINT GetTableSize() const
	{
		return HashMapUtil::GetHashTableSize( mTable, mTableMask );
	}

	// Returns the number of keys.
	FORCEINLINE UINT NumEntries() const
	{
		return mNumEntries;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return !this->NumEntries();
	}

public_internal:
	
	struct Node
	{
		KEY		key;
		VALUE	value;
		Node *	next;

	public:
		FORCEINLINE Node()
			: next( nil )
		{}
		FORCEINLINE Node( const KEY& k, const VALUE& v, Node* next )
			: key( k ), value( v )
			, next( next )
		{}
	};

public:	// Iterators, algorithms, ...

	friend class Iterator;
	class Iterator {
	public:
		INLINE Iterator( TDynaMap& map )
			: mMap( map )
			, mNode( map.mTable[0] )
			, mBucket( 0 )
		{
			Assert(!map.IsEmpty());
			FindNextNode();
		}

		// Pre-increment.
		FORCEINLINE void operator ++ ()
		{
			mNode = mNode->next;
			FindNextNode();
		}
		// returns 'true' if this iterator is valid (there are other elements after it)
		FORCEINLINE operator bool () const
		{
			return (mBucket < mMap.GetTableSize() );
		}
		FORCEINLINE KEY& Key() const
		{
			return mNode->key;
		}
		FORCEINLINE VALUE& Value() const
		{
			return mNode->value;
		}

	private:
		// searches for the next valid bucket
		void FindNextNode()
		{
			while( !mNode  )
			{
				mBucket++;
				if( mBucket > mMap.GetTableSize() ) {
					break;
				}
				mNode = mMap.mTable[ mBucket ];
			}
		}

	private:
		TDynaMap &		mMap;
		Node*			mNode;	// current node in the bucket
		UINT			mBucket;	// current bucket index
	};

public:	// Testing & Debugging.

	UINT DbgGetDeepestBucketSize() const
	{
		UINT deepest = 0;
		UINT numBuckets = GetTableSize();
		for(UINT iBucket = 0; iBucket < numBuckets; iBucket++)
		{
			UINT numItemsInBucket = 0;

			Node * node = mTable[ iBucket ];
			while( node )
			{
				++numItemsInBucket;
				node = node->next;
			}

			deepest = Max( deepest, numItemsInBucket );
		}
		return deepest;
	}

	// returns a number in range [0..1]; the less number means the better

	FLOAT DbgGetLoad() const
	{
		return (FLOAT)DbgGetDeepestBucketSize() / NumEntries();
	}

	// returns a number in range [0..100]; the greater number means the better

	UINT DbgGetSpread() const
	{
		const UINT numEntries = this->NumEntries();
		const UINT tableSize = this->GetTableSize();

		// if no items in hash
		if( !numEntries ) {
			return 100;
		}

		INT average = numEntries / tableSize;

		UINT error = 0;
		
		for(UINT iBucket = 0; iBucket < tableSize; iBucket++)
		{
			INT numItemsInBucket = 0;

			Node * node = mTable[ iBucket ];
			while( node )
			{
				++numItemsInBucket;
				node = node->next;
			}

			UINT e = Abs( numItemsInBucket - average );
			if( e > 1 ) {
				error += e - 1;
			}
		}
		return 100 - (error * 100 / numEntries);
	}

private:

	void DeleteNodes()
	{
		UINT tableSize = GetTableSize();

		for( UINT iBucket = 0; iBucket < tableSize; iBucket++ )
		{
			Node * node = mTable[ iBucket ];
			while( node ) {
				Node * tmp = node;
				node = node->next;
				DestroyNode( tmp );
			}
			mTable[ iBucket ] = nil;
		}

		mNumEntries = 0;
	}

	FORCEINLINE Node* AllocNode(  const KEY& k, const VALUE& v, Node* next )
	{
		void* mem = Allocate( sizeof(Node) );
		return new(mem) Node( k, v, next );
	}
	FORCEINLINE void DestroyNode( Node* ptr )
	{
		ptr->~Node();
		Free( ptr );
	}
	FORCEINLINE void* Allocate( SizeT bytes )
	{
		return F_GetMemoryManager( mMemory )->Allocate( bytes );
	}
	FORCEINLINE void Free( void* ptr )
	{
		F_GetMemoryManager( mMemory )->Free( ptr );
	}

private:	PREVENT_COPY(THIS_TYPE);

	Node **			mTable;		// array of 'capacity' size
	UINT			mTableMask;	// capacity - 1, capacity must be a power of two
	UINT			mNumEntries;	// number of stored items
	const HMemory	mMemory;	// Handle to the memory manager
};

mxNAMESPACE_END

#endif // !__MX_TEMPLATE_DYNAMIC_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
