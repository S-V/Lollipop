/*
=============================================================================
	File:	collide_box_plane.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>
#if 0
PX_OPTIMIZE("use SAT")
pxUInt FCollide_Box_HalfSpace( pxCollideable* objA, pxCollideable* objB, pxCollisionResult &result )
{
	//PX_ASSERT_PTR2(objA,objB);
	//Assert( objA->GetShape()->GetType() == pxcShapeType::PX_SHAPE_BOX );
	//Assert( objB->GetShape()->GetType() == pxcShapeType::PX_SHAPE_HALFSPACE );

	pxShape_Box * box = (pxShape_Box*)objA->GetShape();
	pxShape_HalfSpace * plane = (pxShape_HalfSpace*)objB->GetShape();

	const pxVec3 boxCenter = objA->GetOrigin();
	const pxVec3 halfExtent = box->mHalfSize;

	const pxVec3 x_axis = objA->GetTransform().GetBasis().GetRow(0);
	const pxVec3 y_axis = objA->GetTransform().GetBasis().GetRow(1);
	const pxVec3 z_axis = objA->GetTransform().GetBasis().GetRow(2);

	const pxPlane	pln( plane->mPlane );

	// get world-space corners of the box and test them agains the plane

	Matrix3 axis(
		(const Vec3D&)x_axis,
		(const Vec3D&)y_axis,
		(const Vec3D&)z_axis
	);
	axis.TransposeSelf();

	OOBB oobb( (const Vec3D&)boxCenter, (const Vec3D&)halfExtent, axis );

	Vec3D boxCorners[8];
	oobb.ToPoints( boxCorners );


	int numContacts = 0;
	TStaticArray<pxContact,8> contacts;

	for( int i=0; i<8; i++ )
	{
		pxVec3 p;
		p.Set(boxCorners[i].ToFloatPtr());
		const pxReal d = pln.GetDistance( p );
		if( d < PX_COLLISION_MARGIN )
		{
			contacts[ numContacts ].position.Set( boxCorners[i].ToFloatPtr() );
			contacts[ numContacts ].depth = d;
			++numContacts;
		}
	}

	if( !numContacts ) {
		return 0;
	}

	class Cmp {
	public:
		int operator () ( const pxContact& cA, const pxContact& cB ) {
			return cA.depth < cB.depth;
		}
	} cmp;
	// sort by penetration distance
	contacts.Sort_Insertion(cmp,0,numContacts-1);

	numContacts = Min<pxUInt>( numContacts, MAX_CONTACTS );

	pxContactPointMaterial	contactMaterial;
	contactMaterial.Set( objA->GetSurfaceInfo(), objB->GetSurfaceInfo() );

	pxContactManifold & manifold = result.manifold;

	for( int i = 0; i<numContacts; i++ )
	{
		manifold.points[i].position = contacts[i].position;
		manifold.points[i].normal = pln.GetNormal();
		manifold.points[i].depth = -contacts[i].depth;

		manifold.points[i].material = contactMaterial;
	}

	manifold.numPoints = numContacts;

	result.oA = objA;
	result.oB = objB;

	return numContacts;
}
#endif
NO_EMPTY_FILE
//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
