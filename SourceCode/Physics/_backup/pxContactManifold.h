/*
=============================================================================
	File:	pxContactManifold.h
	Desc:	Contact processing.
=============================================================================
*/

#ifndef __PX_CONTACT_MANIFOLD_H__
#define __PX_CONTACT_MANIFOLD_H__

// max. number of contact points in a contact manifold
enum pxcContactLimits { MAX_CONTACTS = 4 };

//
//	pxContactManifold - is a set of contact points for a single colliding pair.
//
MX_ALIGN_16(struct) pxContactManifold
{
	pxContact	points[ MAX_CONTACTS ];	// only valid if 'numPoints' > 0
	pxUInt		numPoints;	// can be zero

	pxContactManifold * prev;
	pxContactManifold * next;

public:
	PX_INLINE pxContactManifold(ENoInit)
	{}

	PX_INLINE pxContactManifold(EInitZero)
	{
		ZERO_OUT(*this);
	}

	void AddPoint( const pxVec3& position, const pxVec3& normal, pxReal	depth )
	{
		StaticAssert(TIsPowerOfTwo<MAX_CONTACTS>::value);
		Assert(numPoints < MAX_CONTACTS);
		const UINT index = (numPoints++) & (MAX_CONTACTS-1);//avoid checking for overflow
		pxContact & newPt = (points[ index ]);
		newPt.position = position;
		newPt.normal = normal;
		newPt.depth = depth;
	}

	PX_INLINE pxUInt NumPoints() const {
		return numPoints;
	}

	PX_INLINE pxContact& GetPoint( pxUInt index ) {
		Assert( index >= 0 && index < NumPoints() );
		return points[ index ];
	}
	PX_INLINE const pxContact& GetPoint( pxUInt index ) const {
		Assert( index >= 0 && index < NumPoints() );
		return points[ index ];
	}

private:
	static void checks() {
		StaticAssert(TIsPowerOfTwo<MAX_CONTACTS>::value);
	}
};

#endif // !__PX_CONTACT_MANIFOLD_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
