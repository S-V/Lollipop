/*
=============================================================================
	File:	Rotation.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*
============
Rotation::ToAngles
============
*/
Angles Rotation::ToAngles( void ) const {
	return ToMat3().ToAngles();
}

/*
============
Rotation::ToQuat
============
*/
Quat Rotation::ToQuat( void ) const {
	FLOAT a, s, c;

	a = angle * ( MX_DEG2RAD * 0.5f );
	mxSinCos( a, s, c );
	return Quat( vec.x * s, vec.y * s, vec.z * s, c );
}

/*
============
Rotation::toMat3
============
*/
const Matrix3 &Rotation::ToMat3( void ) const {
	FLOAT wx, wy, wz;
	FLOAT xx, yy, yz;
	FLOAT xy, xz, zz;
	FLOAT x2, y2, z2;
	FLOAT a, c, s, x, y, z;

	if ( axisValid ) {
		return axis;
	}

	a = angle * ( MX_DEG2RAD * 0.5f );
	mxSinCos( a, s, c );

	x = vec[0] * s;
	y = vec[1] * s;
	z = vec[2] * s;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = c * x2;
	wy = c * y2;
	wz = c * z2;

	axis[ 0 ][ 0 ] = 1.0f - ( yy + zz );
	axis[ 0 ][ 1 ] = xy - wz;
	axis[ 0 ][ 2 ] = xz + wy;

	axis[ 1 ][ 0 ] = xy + wz;
	axis[ 1 ][ 1 ] = 1.0f - ( xx + zz );
	axis[ 1 ][ 2 ] = yz - wx;

	axis[ 2 ][ 0 ] = xz - wy;
	axis[ 2 ][ 1 ] = yz + wx;
	axis[ 2 ][ 2 ] = 1.0f - ( xx + yy );

	axisValid = true;

	return axis;
}

/*
============
Rotation::ToMat4
============
*/
Matrix4 Rotation::ToMat4( void ) const {
	return ToMat3().ToMat4();
}

/*
============
Rotation::ToAngularVelocity
============
*/
Vec3D Rotation::ToAngularVelocity( void ) const {
	return vec * DEG2RAD( angle );
}

/*
============
Rotation::Normalize180
============
*/
void Rotation::Normalize180( void ) {
	angle -= floor( angle / 360.0f ) * 360.0f;
	if ( angle > 180.0f ) {
		angle -= 360.0f;
	}
	else if ( angle < -180.0f ) {
		angle += 360.0f;
	}
}

/*
============
Rotation::Normalize360
============
*/
void Rotation::Normalize360( void ) {
	angle -= floor( angle / 360.0f ) * 360.0f;
	if ( angle > 360.0f ) {
		angle -= 360.0f;
	}
	else if ( angle < 0.0f ) {
		angle += 360.0f;
	}
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
