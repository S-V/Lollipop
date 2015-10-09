/*
=============================================================================
	File:	Angles.h
	Desc:	Euler angles.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idAngles )
=============================================================================
*/

#ifndef __MATH_ANGLES_H__
#define __MATH_ANGLES_H__
mxSWIPED("idSoftware");
/*
=============================================================================

	Euler angles

=============================================================================
*/

mxNAMESPACE_BEGIN

enum EAngleIndices
{
	PITCH	= 0,	// up / down
	YAW		= 1,	// left / right
	ROLL	= 2		// fall over
};

class Vec3D;
class Quat;
class Rotation;
class Matrix3;
class Matrix4;

//
//	Angles - Euler angles ( in degrees ).
//
class Angles {
public:
	FLOAT			pitch;	// In degrees.
	FLOAT			yaw;	// In degrees.
	FLOAT			roll;	// In degrees.

public:
					Angles( void );
					Angles( FLOAT pitch, FLOAT yaw, FLOAT roll );
					explicit Angles( const Vec3D &v );

	void 			Set( FLOAT pitch, FLOAT yaw, FLOAT roll );
	Angles &		Zero( void );

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );
	Angles			operator-() const;			// negate angles, in general not the inverse rotation
	Angles &		operator=( const Angles &a );
	Angles			operator+( const Angles &a ) const;
	Angles &		operator+=( const Angles &a );
	Angles			operator-( const Angles &a ) const;
	Angles &		operator-=( const Angles &a );
	Angles			operator*( const FLOAT a ) const;
	Angles &		operator*=( const FLOAT a );
	Angles			operator/( const FLOAT a ) const;
	Angles &		operator/=( const FLOAT a );

	friend Angles	operator*( const FLOAT a, const Angles &b );

	bool			Compare( const Angles &a ) const;							// exact compare, no epsilon
	bool			Compare( const Angles &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Angles &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Angles &a ) const;						// exact compare, no epsilon

	Angles &		Normalize360( void );	// normalizes 'this'
	Angles &		Normalize180( void );	// normalizes 'this'

	void			Clamp( const Angles &min, const Angles &max );

	INT				GetDimension( void ) const;

	void			ToVectors( Vec3D *forward, Vec3D *right = NULL, Vec3D *up = NULL ) const;
	Vec3D			ToForward( void ) const;
	Quat			ToQuat( void ) const;
	Rotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;
	Matrix4			ToMat4( void ) const;
	Vec3D			ToAngularVelocity( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

public:
	static const Angles euler_zero;
};

FORCEINLINE Angles::Angles( void ) {
}

FORCEINLINE Angles::Angles( FLOAT pitch, FLOAT yaw, FLOAT roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

FORCEINLINE Angles::Angles( const Vec3D &v ) {
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

FORCEINLINE void Angles::Set( FLOAT pitch, FLOAT yaw, FLOAT roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

FORCEINLINE Angles &Angles::Zero( void ) {
	pitch = yaw = roll = 0.0f;
	return *this;
}

FORCEINLINE FLOAT Angles::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

FORCEINLINE FLOAT &Angles::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

FORCEINLINE Angles Angles::operator-() const {
	return Angles( -pitch, -yaw, -roll );
}

FORCEINLINE Angles &Angles::operator=( const Angles &a ) {
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

FORCEINLINE Angles Angles::operator+( const Angles &a ) const {
	return Angles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

FORCEINLINE Angles& Angles::operator+=( const Angles &a ) {
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;

	return *this;
}

FORCEINLINE Angles Angles::operator-( const Angles &a ) const {
	return Angles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

FORCEINLINE Angles& Angles::operator-=( const Angles &a ) {
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;
	return *this;
}

FORCEINLINE Angles Angles::operator*( const FLOAT a ) const {
	return Angles( pitch * a, yaw * a, roll * a );
}

FORCEINLINE Angles& Angles::operator*=( FLOAT a ) {
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

FORCEINLINE Angles Angles::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return Angles( pitch * inva, yaw * inva, roll * inva );
}

FORCEINLINE Angles& Angles::operator/=( FLOAT a ) {
	FLOAT inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

FORCEINLINE Angles operator*( const FLOAT a, const Angles &b ) {
	return Angles( a * b.pitch, a * b.yaw, a * b.roll );
}

FORCEINLINE bool Angles::Compare( const Angles &a ) const {
	return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

FORCEINLINE bool Angles::Compare( const Angles &a, const FLOAT epsilon ) const {
	if ( mxFabs( pitch - a.pitch ) > epsilon ) {
		return false;
	}
			
	if ( mxFabs( yaw - a.yaw ) > epsilon ) {
		return false;
	}

	if ( mxFabs( roll - a.roll ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool Angles::operator==( const Angles &a ) const {
	return Compare( a );
}

FORCEINLINE bool Angles::operator!=( const Angles &a ) const {
	return !Compare( a );
}

FORCEINLINE void Angles::Clamp( const Angles &min, const Angles &max ) {
	if ( pitch < min.pitch ) {
		pitch = min.pitch;
	} else if ( pitch > max.pitch ) {
		pitch = max.pitch;
	}
	if ( yaw < min.yaw ) {
		yaw = min.yaw;
	} else if ( yaw > max.yaw ) {
		yaw = max.yaw;
	}
	if ( roll < min.roll ) {
		roll = min.roll;
	} else if ( roll > max.roll ) {
		roll = max.roll;
	}
}

FORCEINLINE INT Angles::GetDimension( void ) const {
	return 3;
}

FORCEINLINE const FLOAT *Angles::ToFloatPtr( void ) const {
	return &pitch;
}

FORCEINLINE FLOAT *Angles::ToFloatPtr( void ) {
	return &pitch;
}

mxNAMESPACE_END

#endif /* !__MATH_ANGLES_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
