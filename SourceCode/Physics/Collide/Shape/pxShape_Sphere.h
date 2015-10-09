/*
=============================================================================
	File:	pxShape_Sphere.h
	Desc:	Collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_Sphere_H__
#define __PX_COLLISION_SHAPE_Sphere_H__

#include <Physics/Base/pxSphere.h>
#include <Physics/Collide/Shape/pxShape_Convex.h>

//
//	pxShape_Sphere
//
class pxShape_Sphere : public pxShape_Convex
{
	pxReal	mRadius;

public:
	pxShape_Sphere( pxReal radius = 1.0f );
	~pxShape_Sphere();

	FORCEINLINE pxReal GetRadius() const { return mRadius; }

public:	//--pxShape
	virtual void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const override;
	virtual void CalculateInertiaLocal( pxReal mass, pxVec3 &inertia ) const override;

public:	//--pxShape_Convex
	virtual pxVec3 GetSupportingVertex( const pxVec3& direction ) const override;
	virtual pxReal GetEffectiveRadius( const pxVec3& direction ) const override;
};

#endif // !__PX_COLLISION_SHAPE_Sphere_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
