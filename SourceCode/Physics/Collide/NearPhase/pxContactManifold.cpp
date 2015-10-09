#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/NearPhase/pxContactManifold.h>

/*
-----------------------------------------------------------------------------
	pxContactManifold
-----------------------------------------------------------------------------
*/

UINT SortContactPoints( pxContactPoint points[MAX_CONTACTS] )
{
	return 0;
}

void pxContactManifold::AddPoint( const pxVec3& position, const pxVec3& normal, pxReal	depth )
{
	Assert(numPoints <= MAX_CONTACTS);

#if PX_USE_CONTACT_CACHING

	UINT index = numPoints++;

	if( index == NUMBER_OF(points) )
	{
		index = SortContactPoints(points);
		numPoints = NUMBER_OF(points);
	}

	pxContactPoint & newPt = points[ index ];

	newPt.setPosition( position );
	newPt.setNormalAndDepth( normal, depth );

#else

	StaticAssert(TIsPowerOfTwo<MAX_CONTACTS>::value);
	
	const UINT index = (numPoints++) & (MAX_CONTACTS-1);//avoid checking for overflow

	pxContactPoint & newPt = points[ index ];

	newPt.setPosition( position );
	newPt.setNormalAndDepth( normal, depth );

#endif // PX_USE_CONTACT_CACHING
}

void pxContactManifold::Clear()
{
	numPoints = 0;
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
