/*
=============================================================================
	File:	pxCollisionPairHash.h
	Desc:	The code is based on Pierre Terdiman's pair manager.
=============================================================================
*/

#ifndef __PX_COLLISION_PAIR_HASH_H__
#define __PX_COLLISION_PAIR_HASH_H__

#if 0
//
//	pxCollisionPairHash
//
class pxCollisionPairHash
{
public:
	pxCollisionPairHash( pxUInt maxPairs = 1024 );
	~pxCollisionPairHash();

	// removes all entries and releases memory
	void Clear();

	pxCollisionPair* AddUniquePair( pxCollideable* oA, pxCollideable* oB );

	// attempts to remove the specified pair; returns nil if such pair wasn't found
	pxCollisionPair* RemovePair( pxCollideable* oA, pxCollideable* oB );

	// returns nil if such pair wasn't found
	pxCollisionPair* FindPair( pxCollideable* oA, pxCollideable* oB );

	PX_INLINE pxUInt GetPairIndex( const pxCollisionPair* pair ) const
	{
		return ((pxUInt)((SizeT(pair) - SizeT(mPairs))) / sizeof(pxCollisionPair));
	}

	PX_INLINE pxUInt GetNumPairs() const { return mNbPairs; }

	PX_INLINE pxCollisionPair* GetPairs() { return mPairs; }
	PX_INLINE const pxCollisionPair* GetPairs() const { return mPairs; }

private:
	PX_INLINE pxCollisionPair* _FindPair( pxCollideable* oA, pxCollideable* oB, pxUInt hashValue );

private:
	pxUInt				mHashSize;		// should be a power of two
	pxUInt				mMask;			// mMask = mHashSize - 1
	pxUInt				mNbPairs;		// number of pairs
	pxUInt*				mHashTable;		// array of offsets into mPairs
	pxUInt*				mNext;			// array of links to next items
	pxCollisionPair*	mPairs;			// pairs are kept here
};

#endif

#endif // !__PX_COLLISION_PAIR_HASH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
