/*
=============================================================================
	File:	pxSphere.h
	Desc:	Bounding sphere.
=============================================================================
*/

#ifndef __PX_SPHERE_H__
#define __PX_SPHERE_H__

#include <Physics/Base/pxVec3.h>

//
//	pxSphere
//
MX_ALIGN_16(class) pxSphere {
public:
	pxReal   mOrigin[3];
	pxReal   mRadius;

public:
	FORCEINLINE pxSphere()
	{}

	FORCEINLINE pxSphere( const pxVec3& center, pxReal radius )
	{
		this->Set( center, radius );
	}

	FORCEINLINE pxVec3 GetOrigin() const {
		return pxVec3(mOrigin[0],mOrigin[1],mOrigin[2]);
	}
	FORCEINLINE pxReal GetRadius() const {
		return mRadius;
	}

	FORCEINLINE void Set( const pxVec3& center, pxReal radius )
	{
		mOrigin[0] = center.x;
		mOrigin[1] = center.y;
		mOrigin[2] = center.z;
		mRadius = radius;
	}

	FORCEINLINE bool Intersects( const pxSphere& other ) const {
		const pxVec3 posA = this->GetOrigin();
		const pxVec3 posB = other.GetOrigin();
		const pxReal radiusA = this->GetRadius();
		const pxReal radiusB = other.GetRadius();
		return (posA - posB).LengthSqr() < squaref(radiusA + radiusB);
	}
	FORCEINLINE bool Intersects( const pxSphere& other, pxReal tolerance ) const {
		const pxVec3 posA = this->GetOrigin();
		const pxVec3 posB = other.GetOrigin();
		const pxReal radiusA = this->GetRadius();
		const pxReal radiusB = other.GetRadius();
		return (posA - posB).LengthSqr() < (squaref(radiusA + radiusB) + tolerance);
	}
};

#endif // !__PX_SPHERE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
