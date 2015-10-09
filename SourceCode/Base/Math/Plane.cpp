/*
=============================================================================
	File:	Plane.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*
===================================
		Plane3D constants
===================================
*/
const Plane3D Plane3D::plane_zero( 0.0f, 0.0f, 0.0f, 0.0f );

const Plane3D Plane3D::plane_x( 1.0f, 0.0f, 0.0f, 0.0f );
const Plane3D Plane3D::plane_y( 0.0f, 1.0f, 0.0f, 0.0f );
const Plane3D Plane3D::plane_z( 0.0f, 0.0f, 1.0f, 0.0f );

/*
==================================
	PlanesIntersection

	Calculates the point of intersection of the given 3 planes,
	returns true if the given planes have a common point,
	returns false if 2 or 3 planes are parallel.
==================================
*/
bool PlanesIntersection(
	const Plane3D& a, const Plane3D& b, const Plane3D& c,
	Vec3D &OutIntersection )
{
	// The intersection of three planes is either a point, a line,
	// or there is no intersection (any two of the planes are parallel).

	// Let P(x,y,z) be the point of intersection, then:
	//
	// P * N1 = -d1
	// P * N2 = -d2
	// P * N3 = -d3
	// i.e.
	// a1 * x + b1 * y + c1 * z + d1 = 0
	// a2 * x + b2 * y + c2 * z + d2 = 0
	// a3 * x + b3 * y + c3 * z + d3 = 0
	
	const Vec3D N1( a.Normal() );
	const Vec3D N2( b.Normal() );
	const Vec3D N3( c.Normal() );

	Assert( N1.IsNormalized() );
	Assert( N2.IsNormalized() );
	Assert( N3.IsNormalized() );

	// determinant - triple product
	const FLOAT denominator = N1 * ( N2.Cross( N3 ) );

	//if( denominator == 0.0f ) {
	if ( mxFabs(denominator) < 1e-6f ) {
		return false;
	}

	const Vec3D numerator = (-a.d) * (N2.Cross(N3))
							+ (-b.d) * (N3.Cross(N1))
							+ (-c.d) * (N1.Cross(N2));

	OutIntersection = numerator / denominator;
	return true;
}

/*
================
Plane3D::Type
================
*/
EPlaneType Plane3D::Type( void ) const
{
	const Vec3D& N = this->Normal();

	if ( N[0] == 0.0f ) {
		if ( N[1] == 0.0f ) {
			return N[2] > 0.0f ? EPlaneType::PLANETYPE_Z : EPlaneType::PLANETYPE_NEGZ;
		}
		else if ( N[2] == 0.0f ) {
			return N[1] > 0.0f ? EPlaneType::PLANETYPE_Y : EPlaneType::PLANETYPE_NEGY;
		}
		else {
			return EPlaneType::PLANETYPE_ZEROX;
		}
	}
	else if ( N[1] == 0.0f ) {
		if ( N[2] == 0.0f ) {
			return N[0] > 0.0f ? EPlaneType::PLANETYPE_X : EPlaneType::PLANETYPE_NEGX;
		}
		else {
			return EPlaneType::PLANETYPE_ZEROY;
		}
	}
	else if ( N[2] == 0.0f ) {
		return EPlaneType::PLANETYPE_ZEROZ;
	}
	else {
		return EPlaneType::PLANETYPE_NONAXIAL;
	}
}

/*
================
Plane3D::HeightFit
================
*/
bool Plane3D::HeightFit( const Vec3D *points, const mxUInt numPoints ) {
	mxUInt i;
	FLOAT sumXX = 0.0f, sumXY = 0.0f, sumXZ = 0.0f;
	FLOAT sumYY = 0.0f, sumYZ = 0.0f;
	Vec3D sum, average, dir;

	if ( numPoints == 1 ) {
		a = 0.0f;
		b = 0.0f;
		c = 1.0f;
		d = -points[0].z;
		return true;
	}
	if ( numPoints == 2 ) {
		dir = points[1] - points[0];
		Normal() = dir.Cross( Vec3D( 0, 0, 1 ) ).Cross( dir );
		Normalize();
		d = -( Normal() * points[0] );
		return true;
	}

	sum.SetZero();
	for ( i = 0; i < numPoints; i++) {
		sum += points[i];
	}
	average = sum / numPoints;

	for ( i = 0; i < numPoints; i++ ) {
		dir = points[i] - average;
		sumXX += dir.x * dir.x;
		sumXY += dir.x * dir.y;
		sumXZ += dir.x * dir.z;
		sumYY += dir.y * dir.y;
		sumYZ += dir.y * dir.z;
	}

	Matrix2 m( sumXX, sumXY, sumXY, sumYY );
	if ( !m.InverseSelf() ) {
		return false;
	}

	a = - sumXZ * m[0][0] - sumYZ * m[0][1];
	b = - sumXZ * m[1][0] - sumYZ * m[1][1];
	c = 1.0f;
	Normalize();
	d = -( a * average.x + b * average.y + c * average.z );
	return true;
}

/*
================
Plane3D::PlaneIntersection
================
*/
bool Plane3D::PlaneIntersection( const Plane3D &plane, Vec3D &start, Vec3D &dir ) const {
	DOUBLE n00, n01, n11, det, invDet, f0, f1;

	n00 = Normal().LengthSqr();
	n01 = Normal() * plane.Normal();
	n11 = plane.Normal().LengthSqr();
	det = n00 * n11 - n01 * n01;

	if ( mxFabs(det) < 1e-6f ) {
		return false;
	}

	invDet = 1.0f / det;
	f0 = ( n01 * plane.d - n11 * d ) * invDet;
	f1 = ( n01 * d - n00 * plane.d ) * invDet;

	dir = Normal().Cross( plane.Normal() );
	start = f0 * Normal() + f1 * plane.Normal();
	return true;
}

/*
=============
Plane3D::ToChars
=============
*/
const char *Plane3D::ToChars( mxUInt precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
