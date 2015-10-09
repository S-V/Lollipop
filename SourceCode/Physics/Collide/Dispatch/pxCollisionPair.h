/*
=============================================================================
	File:	pxCollisionPair.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLISION_PAIR_H__
#define __PX_COLLISION_PAIR_H__

PX_INLINE static
pxUInt pxCalcCollisionPairHash( pxCollideable* oA, pxCollideable* oB )
{
	// the lowest four bits are usually zeros because of 16-byte alignment
	const pxULong addrA = pxULong( oA ) >> 4;
	const pxULong addrB = pxULong( oB ) >> 4;
	return Hash32Bits_0( pxUInt(addrA) | (pxUInt(addrB)<<16) );
}

//
//	pxCollisionPair
//
struct pxCollisionPair
{
	pxCollisionAgent *	agent;
	pxCollideable *	RESTRICT_PTR( oA );
	pxCollideable *	RESTRICT_PTR( oB );
	//@todo: 16-bit indices will suffice
	//pxBroadphasePair	d;

public:
	PX_INLINE pxCollisionPair()
	{
		//agent = nil;
		//d.id = 0;
	}

	FORCEINLINE void Clear()
	{
		this->agent = nil;
		this->oA = nil;
		this->oB = nil;
	}
	FORCEINLINE void Initialize( pxCollideable* oA, pxCollideable* oB )
	{
		this->agent = nil;
		this->oA = oA;
		this->oB = oB;
		//d.id0 = oA;
		//d.id1 = oB;
	}

	//FORCEINLINE bool Equals( UINT oA, UINT oB ) const
	//{
	//	return (this->d.id0 == oA) && (this->d.id1 == oB);
	//}
	FORCEINLINE bool Equals( pxCollideable* oA, pxCollideable* oB ) const
	{
		return (this->oA == oA) && (this->oB == oB);
	}

	FORCEINLINE pxU4 GetHash() const
	{
		//return Hash32Bits_0( d.id );
		return pxCalcCollisionPairHash( this->oA, this->oB );
	}

public:
	PX_DECLARE_POD_ALLOCATOR( pxCollisionPair, PX_MEMORY_COLLISION_NARROWPHASE );
};

typedef TList< pxCollisionPair >	pxCollisionPairList;


//struct PairCachePolicy
//{
//	void AfterNewPairAdded( pxCollisionPair& newPair )
//	{}
//	void BeforePairRemoved( pxCollisionPair& oldPair )
//	{}
//};
template
<
	class DERIVED
>
class pxSimplePairCache
{
	pxCollisionPairList	m_pairs;

public:
	pxSimplePairCache()
	{
	}
	~pxSimplePairCache()
	{
	}

	FORCEINLINE DERIVED* AsDerived()
	{
		return static_cast< DERIVED* >( this );
	}

	FORCEINLINE UINT NumPairs() const
	{
		return m_pairs.Num();
	}
	FORCEINLINE pxCollisionPair* GetPairs()
	{
		return m_pairs.ToPtr();
	}

	void Clear()
	{
		m_pairs.EmptyOrClear();
	}

	pxCollisionPair* FindPair( pxCollideable* oA, pxCollideable* oB )
	{
		PX_STATS(gPhysStats.searchedPairs++);

		TOrderPointers( oA, oB );

		const UINT numPairs = m_pairs.Num();
		pxCollisionPair* pairsArray = m_pairs.ToPtr();

		for( UINT iPair = 0; iPair < numPairs; iPair++ )
		{
			pxCollisionPair* existingPair = pairsArray + iPair;
			if( existingPair->Equals( oA, oB ) )
			{
				return existingPair;
			}
		}

		return nil;
	}

	void AddPair( pxCollideable* oA, pxCollideable* oB )
	{
		TOrderPointers( oA, oB );

		if( !this->FindPair( oA, oB ) )
		{
			pxCollisionPair& newPair = m_pairs.Add();
			newPair.Initialize( oA, oB );

			this->AsDerived()->AfterNewPairAdded( &newPair );

			PX_STATS(gPhysStats.addedPairs++);
		}
	}

	void RemovePair( pxCollideable* oA, pxCollideable* oB )
	{
		TOrderPointers( oA, oB );

		pxCollisionPair* existingPair = this->FindPair( oA, oB );

		if(PtrToBool( existingPair ))
		{
			const UINT pairIndex = existingPair - m_pairs.ToPtr();

			this->AsDerived()->BeforePairRemoved( existingPair );

			m_pairs.RemoveAt_Fast( pairIndex );

			PX_STATS(gPhysStats.removedPairs++);
		}

		//Unreachable;
	}
};

#endif // !__PX_COLLISION_PAIR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
