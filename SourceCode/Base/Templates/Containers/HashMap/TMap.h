/*
=============================================================================
	File:	TMap.h
	Desc:	A very simple (closed) hash map - maps (unique) keys to values.
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
			Stop reinventing the wheel.
=============================================================================
*/

#ifndef __MX_TEMPLATE_MAP_H__
#define __MX_TEMPLATE_MAP_H__

mxNAMESPACE_BEGIN

// out-of-line methods to reduce memory bloat
// (also, brings convenience - all memory functions in one place)
namespace HashMapUtil
{
	void* AllocateMemory( UINT bytes );
	void ReleaseMemory( void* ptr );

	// hashMod = tableSize - 1
	FORCEINLINE UINT GetHashTableSize( const void* buckets, UINT hashMod )
	{
	#if MX_USE_BIT_TRICKS
		return if_c_a_else_0( (INT)buckets, hashMod + 1 );
	#else
		return buckets ? (hashMod + 1) : 0;
	#endif
	}

	enum { DEFAULT_HASH_TABLE_SIZE = 16 };

	FORCEINLINE UINT CalcHashTableSize(
		UINT numItems, UINT defaultTableSize = DEFAULT_HASH_TABLE_SIZE )
	{
		return (numItems > 0) ?
			NextPowerOfTwo(defaultTableSize + numItems*2) : defaultTableSize;
	}
}

//
//	TMap< KEY, VALUE > -  An unordered data structure mapping keys to values.
//
//	NOTE: KEY should have member function 'UINT GetHashCode( const KEY& key )'
//	and 'bool operator == (const KEY& other) const'.
//
//	Periodically check that DbgGetLoad() is low (> 0.1) and DbgGetSpread() is high.
//	When DbgGetLoad() gets near 1.0 your hash function is badly designed
//	and maps too many inputs to the same output.
//
template<
	typename KEY,
	typename VALUE,
	class HASH_FUNC = THashTrait< KEY >,
	class EQUALS_FUNC = TEqualsTrait< KEY >,
	typename SIZETYPE = INT16	// NOTE: must be signed! (16-bit indices by default)
>
class TMap {
public:
	typedef TMap
	<
		KEY,
		VALUE,
		HASH_FUNC,
		EQUALS_FUNC,
		SIZETYPE
	> THIS_TYPE;

	enum { DEFAULT_HASH_TABLE_SIZE = HashMapUtil::DEFAULT_HASH_TABLE_SIZE };

	explicit TMap( ENoInit )
	{
		mTable = nil;
		mTableMask = 0;
	}

	explicit TMap( UINT tableSize = DEFAULT_HASH_TABLE_SIZE )
	{
		mTable = nil;
		mTableMask = 0;
		this->Setup( tableSize );
	}

	explicit TMap( UINT tableSize, HMemory heap = EMemHeap::DefaultHeap )
		: mPairs( heap )
	{
		mTable = nil;
		mTableMask = 0;
		this->Setup( tableSize );
	}

	~TMap()
	{
		this->Clear();
	}

	void Setup( UINT tableSize, UINT initialElementCount = 0 )
	{
		Assert(tableSize > 1 && IsPowerOfTwo(tableSize));
		Assert(nil == mTable && mPairs.IsEmpty());

		const UINT numBytes = tableSize * sizeof(mTable[0]);

		mTable = (SIZETYPE*) HashMapUtil::AllocateMemory( numBytes );
		MemSet( mTable, INDEX_NONE, numBytes );

		mTableMask = tableSize - 1;

		if( initialElementCount )
		{
			mPairs.Reserve( initialElementCount );
		}
	}

	//Initialize
	void Reserve( UINT elementCount )
	{
		Assert( elementCount > 0 );
		if( elementCount > 0 )
		{
			const UINT tableSize = HashMapUtil::CalcHashTableSize( elementCount );
			this->Setup( tableSize, elementCount );
		}
	}

	// Removes all elements from the table.
	void Empty()
	{
		const UINT tableSize = this->GetTableSize();
		if( tableSize )
		{
			const UINT numBytes = tableSize * sizeof(mTable[0]);
			MemSet( mTable, INDEX_NONE, numBytes );
		}
		mPairs.DestroyAndEmpty();
	}

	// Removes all elements from the table and releases allocated memory.

	void Clear()
	{
		if( mTable ) {
			HashMapUtil::ReleaseMemory( mTable );
			mTable = nil;
		}
		mTableMask = 0;
		mPairs.Clear();
	}

	// assuming that VALUEs are pointers, deletes them and empties the table
	void DeleteValues()
	{
		const UINT tableSize = GetTableSize();

		const UINT numBytes = tableSize * sizeof(mTable[0]);
		MemSet( mTable, INDEX_NONE, numBytes );

		for( UINT i = 0; i < mPairs.Num(); i++ )
		{
			delete mPairs[i].value;
		}
		mPairs.Empty();
	}

	// Returns a pointer to the element if it exists, or nil if it does not.

	VALUE* Find( const KEY& key )
	{
		AssertPtr(mTable);
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		for( INT i = mTable[hash]; i != INDEX_NONE; i = mPairs[i].next )
		{
			if(EQUALS_FUNC::Equals( mPairs[i].key, key )) {
				return &(mPairs[i].value);
			}
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
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		for( INT i = mTable[hash]; i != INDEX_NONE; i = mPairs[i].next )
		{
			if(EQUALS_FUNC::Equals( mPairs[i].key, key )) {
				return mPairs[i].value;
			}
		}
		return (VALUE) nil;
	}

	const VALUE FindRef( const KEY& key ) const
	{
		return const_cast< THIS_TYPE* >( this )->FindRef( key );
	}

	bool Contains( const KEY& key ) const
	{
		AssertPtr(mTable);
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		for( INT i = mTable[hash]; i != INDEX_NONE; i = mPairs[i].next )
		{
			if(EQUALS_FUNC::Equals( mPairs[i].key, key )) {
				return true;
			}
		}
		return false;
	}

	// Returns key by value. Returns nil pointer if it's not in the table. Slow!

	KEY* FindKeyByValue( const VALUE& value )
	{
		for( UINT i = 0; i < mPairs.Num(); i++ )
		{
			if( mPairs[i].value == value ) {
				return &(mPairs[i].key);
			}
		}
		return nil;
	}
	UINT FindKeyIndex( const KEY& key ) const
	{
		for( UINT i = 0; i < mPairs.Num(); i++ )
		{
			if( mPairs[i].key == key ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Inserts a (key,value) pair into the table.

	VALUE& Set( const KEY& key, const VALUE& value )
	{
		return SetX( key, value ).value;
	}

	// NOTE: expensive operation!
	// Returns the number of removed items.

	UINT Remove( const KEY& key )
	{
		Assert( mPairs.NonEmpty() );

#if 0	// #undef'ed to use swap-with-last-item trick when we remove one element

		// Removes all pairs matching the specified key, returns the number of removed items.
		// This may cause a potentially slow rehashing.

		// remove all pairs with the given key
		UINT numRemovedItems = 0;
		for( INT i = mPairs.Num()-1; i >= 0; i-- )
		{
			if( mPairs[i].key == key ) {
				mPairs.RemoveAt_Fast( i );
				numRemovedItems++;
			}
		}
		if( numRemovedItems )
		{
			// Relax the table.
			this->Relax();
		}
		return numRemovedItems;

#else

		AssertPtr(mTable);
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;

		Pair* pair = this->InternalFindPair( key, hash );
		if( !PtrToBool( pair ) ) {
			return 0;
		}

		// index of pair being removed in the mPairs array
		const INT thisPairIndex = mPairs.GetItemIndex( pair );

		// Remove the pair from the hash table.

		this->InternalUnlinkPair( thisPairIndex, hash );

		// We now move the last pair into spot of the pair being removed
		// (to avoid unused holes in the array of pairs).
		// We need to fix the hash table indices to support the move.
		{
			const INT lastPairIndex = mPairs.Num() - 1;

			// If the removed pair is the last pair, we are done.
			if( thisPairIndex == lastPairIndex )
			{
				mPairs.PopBack();
				return 1;
			}

			// Remove the last pair from the hash table.
			const INT lastPairHash = HASH_FUNC::GetHashCode( mPairs[ lastPairIndex ].key ) & mTableMask;

			this->InternalUnlinkPair( lastPairIndex, lastPairHash );

			// Copy the last pair into the removed pair's spot.
			mPairs[ thisPairIndex ] = mPairs[ lastPairIndex ];

			// Insert the last pair into the hash table.
			mPairs[ thisPairIndex ].next = mTable[ lastPairHash ];
			mTable[ lastPairHash ] = thisPairIndex;

			// Remove and destroy the last pair.
			mPairs.PopBack();
		}

		return 1;
#endif
	}

	// Returns the number of buckets.
	FORCEINLINE UINT GetTableSize() const
	{
		return HashMapUtil::GetHashTableSize( mTable, mTableMask );
	}

	// Returns the number of key-value pairs stored in the table.
	FORCEINLINE UINT NumEntries() const
	{
		return mPairs.Num();
	}

	FORCEINLINE bool IsEmpty() const
	{
		return !this->NumEntries();
	}

	FORCEINLINE void Resize( UINT newTableSize )
	{
		Assert(IsPowerOfTwo( newTableSize ));
		this->Rehash( newTableSize );
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const THIS_TYPE& o )
	{
		file << o.mPairs;
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, THIS_TYPE& o )
	{
		file >> o.mPairs;
		o.Rehash();
		return file;
	}

	friend mxArchive& operator && ( mxArchive& archive, THIS_TYPE& o )
	{
		archive && o.mPairs;
		if( archive.IsReading() )
		{
			o.Rehash();
		}
		return archive;
	}

public_internal:

	struct Pair
	{
		KEY			key;
		VALUE		value;
		SIZETYPE	next;

	public:
		FORCEINLINE Pair()
			: next(INDEX_NONE)
		{}
		FORCEINLINE Pair( const KEY& k, const VALUE& v )
			: key( k ), value( v )
			, next(INDEX_NONE)
		{}
		FORCEINLINE Pair( const KEY& k, const VALUE& v, INT nextPairIndex )
			: key( k ), value( v )
			, next( nextPairIndex )
		{}
		friend AStreamWriter& operator << ( AStreamWriter& file, const Pair& o )
		{
			file << o.key << o.value;
			return file;
		}
		friend AStreamReader& operator >> ( AStreamReader& file, Pair& o )
		{
			file >> o.key >> o.value;
			return file;
		}
		friend mxArchive& operator && ( mxArchive& archive, Pair& o )
		{
			return archive && o.key && o.value;
		}
	};

	typedef TLinearBuffer< Pair, SIZETYPE >	PairsArray;

	Pair* FindPair( const KEY& key )
	{
		AssertPtr(mTable);
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;
		return this->InternalFindPair( key, hash );
	}

	Pair& SetX( const KEY& key, const VALUE& value )
	{
		AssertPtr(mTable);

		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;

		for( INT i = mTable[hash]; i != INDEX_NONE; i = mPairs[i].next )
		{
			if(EQUALS_FUNC::Equals( mPairs[i].key, key )) {
				mPairs[i].value = value;
				return mPairs[i];
			}
		}

		// Create a new entry.

		const UINT newEntryIndex = mPairs.Num();

		Pair & newPair = mPairs.Add();

		new (&newPair) Pair( key, value, mTable[ hash ] );

		mTable[ hash ] = newEntryIndex;

		return newPair;
	}

	Pair& Add( const KEY& key )
	{
		AssertPtr(mTable);
		const INT hash = HASH_FUNC::GetHashCode( key ) & mTableMask;

		// Create a new entry.

		const UINT newEntryIndex = mPairs.Num();

		Pair & newPair = mPairs.Add();

		newPair.key = key;
		newPair.next = mTable[ hash ];

		mTable[ hash ] = newEntryIndex;

		return newPair;
	}

	FORCEINLINE PairsArray & GetPairs()
	{
		return mPairs;
	}
	FORCEINLINE const PairsArray & GetPairs() const
	{
		return mPairs;
	}

	void Relax()
	{
		// calculate new table size
		UINT tableSize = mTableMask + 1;
		while( tableSize > mPairs.Num()*2 + DEFAULT_HASH_TABLE_SIZE )
		{
			tableSize /= 2;
		}
		tableSize = CeilPowerOfTwo( tableSize );
		this->Rehash( tableSize );
	}
	void Rehash()
	{
		const UINT oldNum = mPairs.Num();
		UINT tableSize = oldNum ? (oldNum * 2) : DEFAULT_HASH_TABLE_SIZE;
		tableSize = CeilPowerOfTwo( tableSize );
		this->Rehash( tableSize );
	}
	void GrowIfNeeded()
	{
		const UINT numPairs = mPairs.Num();
		const UINT tableSize = this->GetTableSize();
		if( tableSize * 2 + DEFAULT_HASH_TABLE_SIZE < numPairs )
		{
			const UINT newTableSize = CeilPowerOfTwo( numPairs+1 );
			this->Rehash( newTableSize );
		}
	}

public:	// Iterators, algorithms, ...

	friend class Iterator;
	class Iterator {
	public:
		INLINE Iterator( TMap& map )
			: mMap( map )
			, mPairs( map.mPairs )
			, mIndex( 0 )
		{}
		
		// Pre-increment.
		FORCEINLINE void operator ++ ()
		{
			++mIndex;
		}
		// returns 'true' if this iterator is valid (there are other elements after it)
		FORCEINLINE operator bool () const
		{
			return mIndex < mPairs.Num();
		}
		FORCEINLINE KEY & Key() const
		{
			return mPairs[ mIndex ].key;
		}
		FORCEINLINE VALUE & Value() const
		{
			return mPairs[ mIndex ].value;
		}

	private:
		TMap &			mMap;
		PairsArray &	mPairs;
		SIZETYPE		mIndex;
	};

	friend class ConstIterator;
	class ConstIterator {
	public:
		INLINE ConstIterator( const TMap& map )
			: mMap( map )
			, mPairs( map.mPairs )
			, mIndex( 0 )
		{}

		// Pre-increment.
		FORCEINLINE void operator ++ ()
		{
			++mIndex;
		}
		// returns 'true' if this iterator is valid (there are other elements after it)
		FORCEINLINE operator bool () const
		{
			return mIndex < mPairs.Num();
		}
		FORCEINLINE const KEY& Key() const
		{
			return mPairs[ mIndex ].key;
		}
		FORCEINLINE const VALUE& Value() const
		{
			return mPairs[ mIndex ].value;
		}

	private:
		const TMap &		mMap;
		const PairsArray &	mPairs;
		SIZETYPE			mIndex;
	};

public:	// Testing & Debugging.

	UINT DbgGetDeepestBucketSize() const
	{
		const UINT numBuckets = this->GetTableSize();

		UINT deepest = 0;

		for(UINT iBucket = 0; iBucket < numBuckets; iBucket++)
		{
			UINT numItemsInBucket = 0;

			for(INT iPair = mTable[ iBucket ];
				iPair != INDEX_NONE;
				iPair = mPairs[ iPair ].next)
			{
				numItemsInBucket++;
			}
			deepest = Max( deepest, numItemsInBucket );
		}
		return deepest;
	}

	// returns the hash table's load factor - a number in range [0..1]; the less number means the better

	FLOAT DbgGetLoad() const
	{
		return (FLOAT)this->DbgGetDeepestBucketSize() / this->NumEntries();
	}

	// returns a number in range [0..100]; the greater number means the better

	UINT DbgGetSpread() const
	{
		const UINT numEntries = this->NumEntries();
		const UINT tableSize = this->GetTableSize();

		// if this table is empty
		if( !numEntries ) {
			return 100;
		}

		UINT average = numEntries / tableSize;

		UINT error = 0;

		for(UINT iBucket = 0; iBucket < tableSize; iBucket++)
		{
			UINT numItemsInBucket = 0;

			for(INT iPair = mTable[ iBucket ];
				iPair != INDEX_NONE;
				iPair = mPairs[ iPair ].next)
			{
				numItemsInBucket++;
			}

			UINT e = Abs( numItemsInBucket - average );
			if( e > 1 ) {
				error += e - 1;
			}
		}
		return 100 - (error * 100 / numEntries);
	}

private:

	// Resizing takes O(n) time to complete, where n is a number of entries in the table.
	void Rehash( UINT newTableSize )
	{
		Assert(newTableSize > 1 && IsPowerOfTwo(newTableSize));

		const UINT numBytes = newTableSize * sizeof(mTable[0]);
		SIZETYPE* newTable = (SIZETYPE*) HashMapUtil::AllocateMemory( numBytes );
		MemSet( newTable, INDEX_NONE, numBytes );

		mTableMask = newTableSize - 1;

		for( UINT i = 0; i < mPairs.Num(); i++ )
		{
			Pair & pair = mPairs[ i ];
			const UINT hash = HASH_FUNC::GetHashCode( pair.key ) & mTableMask;
			pair.next = newTable[ hash ];
			newTable[ hash ] = i;
		}
		if( mTable ) {
			HashMapUtil::ReleaseMemory( mTable );
		}
		mTable = newTable;
	}

	inline Pair* InternalFindPair( const KEY& key, const INT hash )
	{
		AssertPtr(mTable);
		for( INT i = mTable[hash]; i != INDEX_NONE; i = mPairs[i].next )
		{
			if(EQUALS_FUNC::Equals( mPairs[i].key, key )) {
				return &(mPairs[i]);
			}
		}
		return nil;
	}

	// Removes the pair from the hash table.
	inline void InternalUnlinkPair( const INT pairIndex, const INT hash )
	{
		// find the index of pair that references the pair being deleted
		// i.e. find 'prevIndex' that 'mPairs[ prevIndex ].next == pairIndexInArray'
		//
		INT prevPairIndex = INDEX_NONE;

		// this could be removed if we had 'Pair::prev' along with 'Pair::next'
		{
			INT currPairIndex = mTable[ hash ];
			Assert( currPairIndex != INDEX_NONE );

			while( currPairIndex != pairIndex )
			{
				prevPairIndex = currPairIndex;
				currPairIndex = mPairs[ currPairIndex ].next;
			}
		}

		if( prevPairIndex != INDEX_NONE )
		{
			Assert( mPairs[ prevPairIndex ].next == pairIndex );
			// remove this pair from the linked list
			mPairs[ prevPairIndex ].next = mPairs[ pairIndex ].next;
		}
		else
		{
			// if this pair was the first one
			mTable[ hash ] = mPairs[ pairIndex ].next;
		}
	}

private:	PREVENT_COPY(THIS_TYPE);
protected:
	SIZETYPE *		mTable;
	PairsArray		mPairs;
	SIZETYPE		mTableMask;	// tableSize - 1, tableSize must be a power of two
};

mxNAMESPACE_END

#endif // !__MX_TEMPLATE_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
