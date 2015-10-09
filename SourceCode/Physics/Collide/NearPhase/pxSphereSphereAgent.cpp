/*
=============================================================================
	File:	pxSphereSphereAgent.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Collide/NearPhase/pxSphereSphereAgent.h>

pxSphereSphereAgent::pxSphereSphereAgent( const pxCollisionAgentCreationInfo& cInfo )
	: pxCollisionAgent( cInfo )
{
	manifold = Physics::GetCollisionDispatcher()->CreateContactManifold();
}
pxSphereSphereAgent::~pxSphereSphereAgent()
{
	Physics::GetCollisionDispatcher()->ReleaseContactManifold( manifold );
}

void pxSphereSphereAgent::ProcessCollision(
	pxCollideable* objA, pxCollideable* objB,
	const pxProcessCollisionInput& input,
	pxProcessCollisionOutput & result
)
{
	pxShape_Sphere * shapeA = (pxShape_Sphere*)objA->GetShape();
	pxShape_Sphere * shapeB = (pxShape_Sphere*)objB->GetShape();

	const pxReal margin = PX_COLLISION_MARGIN;

	// sphere centers in world space
	const pxVec3 posA = objA->GetOrigin();
	const pxVec3 posB = objB->GetOrigin();
	const pxReal radiusA = shapeA->GetRadius() + margin;
	const pxReal radiusB = shapeB->GetRadius() + margin;

	const pxVec3 axis = posA - posB;
	const pxReal distanceSq = axis.LengthSqr();

	//iff len positive, don't generate a new contact
	if( distanceSq > squaref(radiusA + radiusB) )
	{
		return;
	}

	const pxReal len = mxSqrt(distanceSq);

	//distance (negative means penetration)
	const pxReal distance = len - (shapeA->GetRadius() + shapeB->GetRadius());

	// contact normalOnSurfaceB
	pxVec3 normalOnSurfaceB(0.0f,1.0f,0.0f);
	if( len > PX_EPSILON ) {
		normalOnSurfaceB = axis / len;
	}

	//point on B (in world space)
	const pxVec3 pB = posB + radiusB * normalOnSurfaceB;


	manifold->points[0].position = pB;

	manifold->points[0].normalAndDepth.mVec128 = normalOnSurfaceB.mVec128;
	manifold->points[0].normalAndDepth.w = distance;

	manifold->numPoints = 1;

	manifold->oA = objA;
	manifold->oB = objB;
}

#if 0
MX_SWIPED("Bullet");
pxUInt FCollide_Sphere_Sphere( pxCollideable* objA, pxCollideable* objB, pxCollisionResult &result )
{
	//PX_ASSERT_PTR2(objA,objB);
	//Assert( objA->GetShape()->GetType() == pxcShapeType::PX_SHAPE_SPHERE );
	//Assert( objB->GetShape()->GetType() == pxcShapeType::PX_SHAPE_SPHERE );

	pxShape_Sphere * sphereA = (pxShape_Sphere*)objA->GetShape();
	pxShape_Sphere * sphereB = (pxShape_Sphere*)objB->GetShape();

	const pxReal margin = SMALL_NUMBER;//PX_COLLISION_MARGIN;

	// sphere centers in world space
	const pxVec3 posA = objA->GetOrigin();
	const pxVec3 posB = objB->GetOrigin();
	const pxReal radiusA = sphereA->GetRadius() + margin;
	const pxReal radiusB = sphereB->GetRadius() + margin;

	const pxVec3 axis = posA - posB;
	const pxReal distanceSq = axis.LengthSqr();

	//iff len positive, don't generate a new contact
	if( distanceSq > squaref(radiusA + radiusB) )
	{
		return 0;
	}

	const pxReal len = mxSqrt(distanceSq);

	//distance (negative means penetration)
	const pxReal distance = len - (sphereA->mRadius + sphereB->mRadius);

	// contact normalOnSurfaceB
	pxVec3 normalOnSurfaceB(0.0f,1.0f,0.0f);
	if( len > PX_EPSILON ) {
		normalOnSurfaceB = axis / len;
	}

	//point on B (in world space)
	const pxVec3 pB = posB + radiusB * normalOnSurfaceB;

	pxContactManifold & manifold = result.manifold;

	manifold.points[0].position = pB;
	manifold.points[0].normal = normalOnSurfaceB;
	manifold.points[0].depth = distance;

	manifold.points[0].material.Set( objA->GetSurfaceInfo(), objB->GetSurfaceInfo() );

	manifold.numPoints = 1;
		
	result.oA = objA;
	result.oB = objB;

	return 1;
}
#endif
//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
