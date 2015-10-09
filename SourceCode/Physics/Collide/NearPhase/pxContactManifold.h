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

MX_ALIGN_16(struct) pxManifoldPoint : pxContactPoint
{
};

/*
-----------------------------------------------------------------------------
	pxContactManifold - is a set of contact points for a single colliding pair.
	It keeps contact information until two rigid bodies are separated.
-----------------------------------------------------------------------------
*/
MX_ALIGN_16(struct) pxContactManifold
{
	//pxContactPointId	pointIds[ MAX_CONTACTS ];	// only valid if 'numPoints' > 0
	pxContactPoint		points[ MAX_CONTACTS ];	// only valid if 'numPoints' > 0
	pxUInt				numPoints;	// can be zero
	pxCollideable *		oA;
	pxCollideable *		oB;

public_internal:
	pxU4	internalIndex;	// array index, don't touch!

public:
	FORCEINLINE pxContactManifold(ENoInit)
	{}

	FORCEINLINE pxContactManifold(EInitZero)
	{
		ZERO_OUT(*this);
	}

	void AddPoint( const pxVec3& position, const pxVec3& normal, pxReal	depth );

	void Clear();

private:
	StaticAssert(TIsPowerOfTwo<MAX_CONTACTS>::value);
};

#endif // !__PX_CONTACT_MANIFOLD_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
