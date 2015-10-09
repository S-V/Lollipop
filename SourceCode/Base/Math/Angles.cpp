/*
=============================================================================
	File:	Angles.cpp
	Desc:	
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

const Angles Angles::euler_zero( 0.0f, 0.0f, 0.0f );

/*
=================
Angles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
Angles& Angles::Normalize360( void ) {
	INT i;

	for ( i = 0; i < 3; i++ ) {
		if ( ( (*this)[i] >= 360.0f ) || ( (*this)[i] < 0.0f ) ) {
			(*this)[i] -= floor( (*this)[i] / 360.0f ) * 360.0f;

			if ( (*this)[i] >= 360.0f ) {
				(*this)[i] -= 360.0f;
			}
			if ( (*this)[i] < 0.0f ) {
				(*this)[i] += 360.0f;
			}
		}
	}

	return *this;
}

/*
=================
Angles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
Angles& Angles::Normalize180( void ) {
	Normalize360();

	if ( pitch > 180.0f ) {
		pitch -= 360.0f;
	}
	
	if ( yaw > 180.0f ) {
		yaw -= 360.0f;
	}

	if ( roll > 180.0f ) {
		roll -= 360.0f;
	}
	return *this;
}

/*
=================
Angles::ToVectors
=================
*/
void Angles::ToVectors( Vec3D *forward, Vec3D *right, Vec3D *up ) const {
	FLOAT sr, sp, sy, cr, cp, cy;
	
	mxSinCos( DEG2RAD( yaw ), sy, cy );
	mxSinCos( DEG2RAD( pitch ), sp, cp );
	mxSinCos( DEG2RAD( roll ), sr, cr );

	if ( forward ) {
		forward->Set( cp * cy, cp * sy, -sp );
	}

	if ( right ) {
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
	}

	if ( up ) {
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	}
}

/*
=================
Angles::ToForward
=================
*/
Vec3D Angles::ToForward( void ) const {
	FLOAT sp, sy, cp, cy;

	mxSinCos( DEG2RAD( yaw ), sy, cy );
	mxSinCos( DEG2RAD( pitch ), sp, cp );

	return Vec3D( cp * cy, cp * sy, -sp );
}

/*
=================
Angles::ToQuat
=================
*/
Quat Angles::ToQuat( void ) const {
	FLOAT sx, cx, sy, cy, sz, cz;
	FLOAT sxcy, cxcy, sxsy, cxsy;

	mxSinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	mxSinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	mxSinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	return Quat( cxsy*sz - sxcy*cz, -cxsy*cz - sxcy*sz, sxsy*cz - cxcy*sz, cxcy*cz + sxsy*sz );
}

/*
=================
Angles::ToRotation
=================
*/
Rotation Angles::ToRotation( void ) const {
	Vec3D vec;
	FLOAT angle, w;
	FLOAT sx, cx, sy, cy, sz, cz;
	FLOAT sxcy, cxcy, sxsy, cxsy;

	if ( pitch == 0.0f ) {
		if ( yaw == 0.0f ) {
			return Rotation( Vec3D::vec3_zero, Vec3D( -1.0f, 0.0f, 0.0f ), roll );
		}
		if ( roll == 0.0f ) {
			return Rotation( Vec3D::vec3_zero, Vec3D( 0.0f, 0.0f, -1.0f ), yaw );
		}
	} else if ( yaw == 0.0f && roll == 0.0f ) {
		return Rotation( Vec3D::vec3_zero, Vec3D( 0.0f, -1.0f, 0.0f ), pitch );
	}

	mxSinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	mxSinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	mxSinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	vec.x =  cxsy * sz - sxcy * cz;
	vec.y = -cxsy * cz - sxcy * sz;
	vec.z =  sxsy * cz - cxcy * sz;
	w =		 cxcy * cz + sxsy * sz;
	angle = Math::ACos( w );
	if ( angle == 0.0f ) {
		vec.Set( 0.0f, 0.0f, 1.0f );
	} else {
		//vec *= (1.0f / sin( angle ));
		vec.Normalize();
		vec.FixDegenerateNormal();
		angle *= 2.0f * MX_RAD2DEG;
	}
	return Rotation( Vec3D::vec3_zero, vec, angle );
}

/*
=================
Angles::ToMat3
=================
*/
Matrix3 Angles::ToMat3( void ) const {
	Matrix3 mat;
	FLOAT sr, sp, sy, cr, cp, cy;

	mxSinCos( DEG2RAD( yaw ), sy, cy );
	mxSinCos( DEG2RAD( pitch ), sp, cp );
	mxSinCos( DEG2RAD( roll ), sr, cr );

	mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

	return mat;
}

/*
=================
Angles::ToMat4
=================
*/
Matrix4 Angles::ToMat4( void ) const {
	return ToMat3().ToMat4();
}

/*
=================
Angles::ToAngularVelocity
=================
*/
Vec3D Angles::ToAngularVelocity( void ) const {
	Rotation rotation = Angles::ToRotation();
	return rotation.GetVec() * DEG2RAD( rotation.GetAngle() );
}

/*
=============
Angles::ToChars
=============
*/
const char *Angles::ToChars( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
