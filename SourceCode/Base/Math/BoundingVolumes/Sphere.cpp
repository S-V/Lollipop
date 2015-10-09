/*
=============================================================================
	File:	Sphere.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*================================
			Sphere
================================*/

const Sphere Sphere::ZERO_SPHERE( Vec3D::vec3_zero, 0.0f );
const Sphere Sphere::UNIT_SPHERE( Vec3D::vec3_zero, 1.0f );

/*
================
Sphere::PlaneDistance
================
*/
FLOAT Sphere::PlaneDistance( const Plane3D &plane ) const {
	FLOAT d;

	d = plane.Distance( Center );
	if ( d > Radius ) {
		return d - Radius;
	}
	if ( d < -Radius ) {
		return d + Radius;
	}
	return 0.0f;
}

/*
================
Sphere::PlaneSide
================
*/
EPlaneSide Sphere::PlaneSide( const Plane3D &plane, const FLOAT epsilon ) const {
	FLOAT d;

	d = plane.Distance( Center );
	if ( d > Radius + epsilon ) {
		return EPlaneSide::PLANESIDE_FRONT;
	}
	if ( d < -Radius - epsilon ) {
		return EPlaneSide::PLANESIDE_BACK;
	}
	return EPlaneSide::PLANESIDE_CROSS;
}

/*
============
Sphere::LineIntersection

  Returns true if the line intersects the sphere between the start and end point.
============
*/
bool Sphere::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
	Vec3D r, s, e;
	FLOAT a;

	s = start - Center;
	e = end - Center;
	r = e - s;
	a = -s * r;
	if ( a <= 0 ) {
		return ( s * s < Radius * Radius );
	}
	else if ( a >= r * r ) {
		return ( e * e < Radius * Radius );
	}
	else {
		r = s + ( a / ( r * r ) ) * r;
		return ( r * r < Radius * Radius );
	}
}

/*
============
Sphere::RayIntersection

  Returns true if the ray intersects the sphere.
  The ray can intersect the sphere in both directions from the start point.
  If start is inside the sphere then scale1 < 0 and scale2 > 0.
============
*/
bool Sphere::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale1, FLOAT &scale2 ) const {
	DOUBLE a, b, c, d, sqrtd;
	Vec3D p;

	p = start - Center;
	a = dir * dir;
	b = dir * p;
	c = p * p - Radius * Radius;
	d = b * b - c * a;

	if ( d < 0.0f ) {
		return false;	// A negative discriminant corresponds to ray missing sphere.
	}

	sqrtd = mxSqrt( d );
	a = 1.0f / a;

	scale1 = ( -b + sqrtd ) * a;
	scale2 = ( -b - sqrtd ) * a;

	return true;
}

/*
============
Sphere::FromPoints
============
*/
void Sphere::FromPoints( const Vec3D& a, const Vec3D& b, const Vec3D& c )
{
	Unimplemented;
}

/*
============
Sphere::FromPoints

  Tight sphere for a point set.
============
* /
void Sphere::FromPoints( const Vec3D *points, const UINT numPoints ) {
	UINT i;
	FLOAT radiusSqr, dist;
	Vec3D mins, maxs;

	mxCompute_MinMax( mins, maxs, points, numPoints );

	mOrigin = ( mins + maxs ) * 0.5f;

	radiusSqr = 0.0f;
	for ( i = 0; i < numPoints; i++ ) {
		dist = ( points[i] - mOrigin ).LengthSqr();
		if ( dist > radiusSqr ) {
			radiusSqr = dist;
		}
	}
	mRadius = mxSqrt( radiusSqr );
}
*/
mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
