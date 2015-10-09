/*
=============================================================================
	File:	pxShape_Sphere.cpp
	Desc:	Collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Sphere.h>

/*================================
		pxShape_Sphere
================================*/

pxShape_Sphere::pxShape_Sphere( pxReal radius )
	: pxShape_Convex( pxcShapeType::PX_SHAPE_SPHERE )
	, mRadius( radius )
{
}

pxShape_Sphere::~pxShape_Sphere() {
}

void pxShape_Sphere::GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const
{
	const pxVec3 center = xform.GetOrigin();
	const pxVec3 extent( mRadius, mRadius, mRadius );
	outBounds.mMin = center - extent;
	outBounds.mMax = center + extent;
}

void pxShape_Sphere::CalculateInertiaLocal( pxReal mass, pxVec3 &inertia ) const
{
	const pxReal x = (0.4f * mass) * squaref( mRadius );
	inertia.SetAll( x );
}

pxVec3 pxShape_Sphere::GetSupportingVertex( const pxVec3& direction ) const
{
	Assert(direction.IsNormalized());
	return direction * mRadius;
}

pxReal pxShape_Sphere::GetEffectiveRadius( const pxVec3& direction ) const
{
	Assert(direction.IsNormalized());
	return mRadius;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
