/*
=============================================================================
	File:	pxCollisionPairHash.cpp
	Desc:	This implementation is based on Pierre Terdiman's pair manager.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#if 0
static
PX_INLINE void SortIds( pxULong& oA, pxULong& oB )
{
	if(oA>oB)	TSwap(oA, oB);
}
static
PX_INLINE bool DifferentPair( const pxCollisionPair& p, pxCollideable* oA, pxCollideable* oB )
{
	return !(p.Equals( oA, oB ));
}

const pxUInt INVALID_PAIR_INDEX	= pxUInt(-1);

/*================================
		pxCollisionPairHash
================================*/

pxCollisionPairHash::pxCollisionPairHash( pxUInt maxPairs ) :
	mHashSize		(0),
	mMask			(0),
	mHashTable		(nil),
	mNext			(nil),
	mNbPairs	(0),
	mPairs	(nil)
{
	Assert( maxPairs > 0 && IsPowerOfTwo(maxPairs) );

	mHashSize = maxPairs;
	mMask = mHashSize - 1;

	mHashTable = (pxUInt*)pxNew(mHashSize*sizeof(pxUInt));
	MemSet(mHashTable, INVALID_PAIR_INDEX, mHashSize*sizeof(pxUInt) );

	mNext = (pxUInt*)pxNew(mHashSize * sizeof(pxUInt));
	AssertPtr(mNext);

	mPairs = (pxCollisionPair*)pxNew(mHashSize * sizeof(pxCollisionPair));
	AssertPtr(mPairs);
}

pxCollisionPairHash::~pxCollisionPairHash()
{
	pxFree(mNext);			mNext = nil;
	pxFree(mPairs);			mPairs = nil;
	pxFree(mHashTable);		mHashTable = nil;
}

void pxCollisionPairHash::Clear()
{
// @shvd: we don't release memory
	MemSet(mHashTable, INVALID_PAIR_INDEX, mHashSize*sizeof(pxUInt) );
	mNbPairs	= 0;
}

pxCollisionPair* pxCollisionPairHash::FindPair( pxCollideable* oA, pxCollideable* oB )
{
// @shvd: this can be removed since we always preallocate arrays
//	if(!mHashTable)	return nil;	// Nothing has been allocated yet

	// Order the ids
	SortIds( (pxULong&)oA, (pxULong&)oB );

	// Compute hash value for this pair
	const pxUInt HashValue = pxCollisionPair::GetHash( oA, oB ) & mMask;

	return _FindPair( oA, oB, HashValue );
}

// Internal version saving hash computation
PX_INLINE pxCollisionPair* pxCollisionPairHash::_FindPair( pxCollideable* oA, pxCollideable* oB , pxUInt hashValue )
{
// @shvd: this can be removed since we always preallocate arrays
//	if(!mHashTable)	return nil;	// Nothing has been allocated yet

	// Look for it in the table
	pxUInt Offset = mHashTable[hashValue];
	while(Offset!=INVALID_PAIR_INDEX && DifferentPair(mPairs[Offset], oA, oB))
	{
//		Assert(mPairs[Offset].mID0!=INVALID_PAIR_INDEX);
		Offset = mNext[Offset];		// Better to have a separate array for this
	}
	if(Offset==INVALID_PAIR_INDEX)	return nil;
	Assert(Offset<mNbPairs);
	// Match mPairs[Offset] => the pair is persistent
	return &mPairs[Offset];
}

pxCollisionPair* pxCollisionPairHash::AddUniquePair( pxCollideable* oA, pxCollideable* oB )
{
	// Order the ids
	SortIds( (pxULong&)oA, (pxULong&)oB );

	pxUInt HashValue = pxCollisionPair::GetHash( oA, oB ) & mMask;

	pxCollisionPair* P = _FindPair(oA, oB, HashValue);
	if(P)
		return P;	// Persistent pair

	// This is a new pair
	if(mNbPairs >= mHashSize)
	{
		// Get more entries, resize the table
		mHashSize = NextPowerOfTwo(mNbPairs+1);
		mMask = mHashSize - 1;

		pxFree(mHashTable);
		mHashTable = (pxUInt*)pxNew(mHashSize*sizeof(pxUInt));
		MemSet(mHashTable, INVALID_PAIR_INDEX, mHashSize*sizeof(pxUInt) );

		// Get some bytes for new entries
		pxCollisionPair* NewPairs = (pxCollisionPair*)pxNew(mHashSize * sizeof(pxCollisionPair));
		AssertPtr(NewPairs);

		pxUInt* NewNext = (pxUInt*)pxNew(mHashSize * sizeof(pxUInt));
		AssertPtr(NewNext);

		// Copy old data if needed
		if(mNbPairs) {
			MemCopy(NewPairs, mPairs, mNbPairs*sizeof(pxCollisionPair));
		}
		// ### check it's actually needed... probably only for pairs whose hash value was cut by the and
		// yeah, since pxCollisionPair::GetHash( oA, oB ) is a constant
		// However it might not be needed to recompute them => only less efficient but still ok
		for(pxUInt i=0;i<mNbPairs;i++)
		{
			pxUInt HashValue = mPairs[i].GetHash() & mMask;	// New hash value with new mask
			NewNext[i] = mHashTable[HashValue];
			mHashTable[HashValue] = i;
		}

		// Delete old data
		pxFree(mNext);
		pxFree(mPairs);

		// Assign new pointer
		mPairs = NewPairs;
		mNext = NewNext;

		// Recompute hash value with new hash size
		HashValue = pxCollisionPair::GetHash( oA, oB ) & mMask;
	}

	pxCollisionPair* p = &mPairs[ mNbPairs ];
	new(p) pxCollisionPair( oA, oB );

	mNext[mNbPairs] = mHashTable[HashValue];
	mHashTable[HashValue] = mNbPairs++;
	return p;
}

pxCollisionPair* pxCollisionPairHash::RemovePair( pxCollideable* oA, pxCollideable* oB )
{
	// Order the ids
	SortIds( (pxULong&)oA, (pxULong&)oB );

	const pxUInt HashValue = pxCollisionPair::GetHash( oA, oB ) & mMask;
	pxCollisionPair* P = _FindPair(oA, oB, HashValue);
	if( !P ) {
		return nil;
	}
	Assert(P->Equals(oA,oB));

	const pxUInt PairIndex = GetPairIndex(P);

	// Walk the hash table to fix mNext
	pxUInt Offset = mHashTable[HashValue];
	Assert(Offset!=INVALID_PAIR_INDEX);

	pxUInt Previous=INVALID_PAIR_INDEX;
	while(Offset!=PairIndex)
	{
		Previous = Offset;
		Offset = mNext[Offset];
	}

	// Let us go/jump us
	if(Previous!=INVALID_PAIR_INDEX)
	{
		Assert(mNext[Previous]==PairIndex);
		mNext[Previous] = mNext[PairIndex];
	}
	// else we were the first
	else mHashTable[HashValue] = mNext[PairIndex];
	// we're now free to reuse next[pairIndex] without breaking the list

#if PX_DEBUG
	mNext[PairIndex]=INVALID_PAIR_INDEX;
#endif

	// We now move the last pair into spot of the
	// pair being removed. We need to fix the hash
	// table indices to support the move.

	// Fill holes
	if(1)
	{
		// 1) Remove last pair
		const pxUInt LastPairIndex = mNbPairs-1;
		if(LastPairIndex==PairIndex)
		{
			mNbPairs--;
			return P;
		}

		const pxCollisionPair* Last = &mPairs[LastPairIndex];
		const pxUInt LastHashValue = Last->GetHash() & mMask;

		// Walk the hash table to fix mNext
		pxUInt Offset = mHashTable[LastHashValue];
		Assert(Offset!=INVALID_PAIR_INDEX);

		pxUInt Previous=INVALID_PAIR_INDEX;
		while(Offset!=LastPairIndex)
		{
			Previous = Offset;
			Offset = mNext[Offset];
		}

		// Let us go/jump us
		if(Previous!=INVALID_PAIR_INDEX)
		{
			Assert(mNext[Previous]==LastPairIndex);
			mNext[Previous] = mNext[LastPairIndex];
		}
		// else we were the first
		else mHashTable[LastHashValue] = mNext[LastPairIndex];
		// we're now free to reuse mNext[LastPairIndex] without breaking the list

#if PX_DEBUG
		mNext[LastPairIndex]=INVALID_PAIR_INDEX;
#endif

		// Don't invalidate entry since we're going to shrink the array

		// 2) Re-insert in free slot
		mPairs[PairIndex] = mPairs[LastPairIndex];
#if PX_DEBUG
		Assert(mNext[PairIndex]==INVALID_PAIR_INDEX);
#endif
		mNext[PairIndex] = mHashTable[LastHashValue];
		mHashTable[LastHashValue] = PairIndex;

		mNbPairs--;
	}
	return P;
}
#endif

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
