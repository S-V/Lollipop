/*
=============================================================================
	File:	pxSpherePlaneAgent.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_HalfSpace.h>
#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Collide/NearPhase/pxSpherePlaneAgent.h>


/*
-----------------------------------------------------------------------------
	pxSpherePlaneAgent
-----------------------------------------------------------------------------
*/
pxSpherePlaneAgent::pxSpherePlaneAgent( const pxCollisionAgentCreationInfo& cInfo )
	: Super( cInfo )
{
}

pxSpherePlaneAgent::~pxSpherePlaneAgent()
{
}

void pxSpherePlaneAgent::ProcessCollision(
	pxCollideable* objA, pxCollideable* objB,
	const pxProcessCollisionInput& input,
	pxProcessCollisionOutput & result
	)
{
	pxShape_Sphere* sphereShape;
	pxShape_HalfSpace* planeShape;

	GetShapes( objA, objB, sphereShape, planeShape );

	const pxVec3 sphereOrigin( objA->GetOrigin() );
	const pxReal sphereRadius( sphereShape->GetRadius() );

	const Plane3D plane = planeShape->mPlane;
	const pxVec3 planeNormal = pxVec3::From_Vec3D( plane.Normal() );

	// distance from plane to sphere origin
	const pxReal dist = plane.Distance( sphereOrigin.As_Vec3D() );

	// penetration depth (negative distance from plane to sphere)
	const pxReal depth = sphereRadius - dist;

	// if penetration has occurred
	if( depth > PX_COLLISION_MARGIN )
	{
		// separation distance in the direction of contact normal
		// or penetration depth if positive (>epsilon)
		//DBGOUT("contact depth: %f\n",depth);

		pxContactPoint& point0 = m_manifold->points[0];

		point0.position = sphereOrigin - planeNormal * (sphereRadius - depth);
		point0.setNormalAndDepth( planeNormal, depth );

		m_manifold->numPoints = 1;

		m_manifold->oA = objA;
		m_manifold->oB = objB;

		result.numContacts = 1;

		//dbgout << "contact: depth=" << depth << ", pos=" << point0.position.As_Vec3D() << "\n";
		return;
	}

	m_manifold->numPoints = 0;
}

pxReal pxSpherePlaneAgent::CalculateTOI( const pxToiInput& input, pxToiOutput &output )
{
	return 1.0f;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
