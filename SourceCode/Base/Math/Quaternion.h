/*
=============================================================================
	File:	Quaternion.h
	Desc:	Quaternion class.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idQuat )
=============================================================================
*/

#ifndef __MATH_QUATERNION_H__
#define __MATH_QUATERNION_H__
mxSWIPED("idSoftware");
/*
=============================================================================

	Quaternion

=============================================================================
*/

mxNAMESPACE_BEGIN

class Vec3D;
class Angles;
class Rotation;
class Matrix3;
class Matrix4;
class CQuat;

//
//	Quat
//
mxALIGN_16(class) Quat {
public:
	union {
		struct {
			FLOAT	x;
			FLOAT	y;
			FLOAT	z;
			FLOAT	w;
		};
		mxSimdQuad	quad;
	};

public:
					Quat( void );
					Quat( FLOAT x, FLOAT y, FLOAT z, FLOAT w );
					Quat( const Vec3D& axis, FLOAT angle );
					explicit Quat( EInitIdentity );

	void 			Set( FLOAT x, FLOAT y, FLOAT z, FLOAT w );
	void			SetZero();
	void			SetIdentity();

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );
	Quat			operator-() const;
	Quat &			operator=( const Quat &a );
	Quat			operator+( const Quat &a ) const;
	Quat &			operator+=( const Quat &a );
	Quat			operator-( const Quat &a ) const;
	Quat &			operator-=( const Quat &a );
	Quat			operator*( const Quat &a ) const;
	Vec3D			operator*( const Vec3D &a ) const;
	Quat			operator*( FLOAT a ) const;
	Quat &			operator*=( const Quat &a );
	Quat &			operator*=( FLOAT a );

	friend Quat		operator*( const FLOAT a, const Quat &b );
	friend Vec3D	operator*( const Vec3D &a, const Quat &b );

	bool			Compare( const Quat &a ) const;						// exact compare, no epsilon
	bool			Compare( const Quat &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Quat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const Quat &a ) const;					// exact compare, no epsilon

	Quat			Inverse( void ) const;
	FLOAT			GetLength( void ) const;
	FLOAT			LengthSq( void ) const;
	Quat &			Normalize( void );
	void			NormalizeFast( void );	// unsafe

	FLOAT			CalcW( void ) const;
	INT				GetDimension( void ) const;

	Angles			ToAngles( void ) const;
	Rotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;	// NOTE: this quaternion must be normalized.
	Matrix4			ToMat4( void ) const;	// NOTE: this quaternion must be normalized.
	CQuat			ToCQuat( void ) const;
	Vec3D			ToAngularVelocity( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

					// Spherical interpolation for two quaternions.
	Quat &			Slerp( const Quat &from, const Quat &to, FLOAT t );

					// Constructs a direction vector from this quaternion.
	Vec3D			ToDirection() const;

	// Testing & Debugging.

	bool	IsNormalized( const FLOAT tolerance = 0.0f ) const;

public:
	static const Quat	quat_zero;		// Addition identity quaternion.
	static const Quat	quat_identity;	// Multiplication identity quaternion.

mxTODO(" add quaternions, representing common rotations (e.g. 'static Quat::quat_rotXY90')");
};

FORCEINLINE Quat::Quat( void ) {
}

FORCEINLINE Quat::Quat( FLOAT x, FLOAT y, FLOAT z, FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE Quat::Quat( const Vec3D& axis, FLOAT angle )
{
	Assert( axis.IsNormalized() );
	FLOAT halfAngle = angle * 0.5f;
	FLOAT sine, cosine;
	mxSinCos( halfAngle, sine, cosine );

	this->x = sine * axis.x;
	this->y = sine * axis.y;
	this->z = sine * axis.z;
	this->w = cosine;
}

FORCEINLINE Quat::Quat( EInitIdentity )
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 1.0f;
}

FORCEINLINE FLOAT Quat::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& Quat::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

FORCEINLINE Quat Quat::operator-() const {
	return Quat( -x, -y, -z, -w );
}

FORCEINLINE Quat &Quat::operator=( const Quat &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator+( const Quat &a ) const {
	return Quat( x + a.x, y + a.y, z + a.z, w + a.w );
}

FORCEINLINE Quat& Quat::operator+=( const Quat &a ) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator-( const Quat &a ) const {
	return Quat( x - a.x, y - a.y, z - a.z, w - a.w );
}

FORCEINLINE Quat& Quat::operator-=( const Quat &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

FORCEINLINE Quat Quat::operator*( const Quat &a ) const {
	return Quat(	w*a.x + x*a.w + y*a.z - z*a.y,
					w*a.y + y*a.w + z*a.x - x*a.z,
					w*a.z + z*a.w + x*a.y - y*a.x,
					w*a.w - x*a.x - y*a.y - z*a.z );
}

FORCEINLINE Vec3D Quat::operator*( const Vec3D &a ) const {
#if 0
	mxOPTIMIZE( is it faster to do the conversion to a 3x3 matrix and multiply the vector by this 3x3 matrix? )
	return ( ToMat3() * a );
#else
	// result = this->Inverse() * Quat( a.x, a.y, a.z, 0.0f ) * (*this)
	FLOAT xxzz = x*x - z*z;
	FLOAT wwyy = w*w - y*y;

	FLOAT xw2 = x*w*2.0f;
	FLOAT xy2 = x*y*2.0f;
	FLOAT xz2 = x*z*2.0f;
	FLOAT yw2 = y*w*2.0f;
	FLOAT yz2 = y*z*2.0f;
	FLOAT zw2 = z*w*2.0f;

	return Vec3D(
		(xxzz + wwyy)*a.x		+ (xy2 + zw2)*a.y		+ (xz2 - yw2)*a.z,
		(xy2 - zw2)*a.x			+ (y*y+w*w-x*x-z*z)*a.y	+ (yz2 + xw2)*a.z,
		(xz2 + yw2)*a.x			+ (yz2 - xw2)*a.y		+ (wwyy - xxzz)*a.z
	);
#endif
}

FORCEINLINE Quat Quat::operator*( FLOAT a ) const {
	return Quat( x * a, y * a, z * a, w * a );
}

FORCEINLINE Quat operator*( const FLOAT a, const Quat &b ) {
	return b * a;
}

FORCEINLINE Vec3D operator*( const Vec3D &a, const Quat &b ) {
	return b * a;
}

FORCEINLINE Quat& Quat::operator*=( const Quat &a ) {
	*this = *this * a;

	return *this;
}

FORCEINLINE Quat& Quat::operator*=( FLOAT a ) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

FORCEINLINE bool Quat::Compare( const Quat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && ( w == a.w ) );
}

FORCEINLINE bool Quat::Compare( const Quat &a, const FLOAT epsilon ) const {
	if ( mxFabs( x - a.x ) > epsilon ) {
		return false;
	}
	if ( mxFabs( y - a.y ) > epsilon ) {
		return false;
	}
	if ( mxFabs( z - a.z ) > epsilon ) {
		return false;
	}
	if ( mxFabs( w - a.w ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Quat::operator==( const Quat &a ) const {
	return Compare( a );
}

FORCEINLINE bool Quat::operator!=( const Quat &a ) const {
	return !Compare( a );
}

FORCEINLINE void Quat::Set( FLOAT x, FLOAT y, FLOAT z, FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE void Quat::SetZero() {
	*this = Quat::quat_zero;
}

FORCEINLINE void Quat::SetIdentity() {
	*this = quat_identity;
}

FORCEINLINE Quat Quat::Inverse( void ) const {
	return Quat( -x, -y, -z, w );
}

FORCEINLINE FLOAT Quat::LengthSq( void ) const {
	return ( x * x + y * y + z * z + w * w );
}

FORCEINLINE FLOAT Quat::GetLength( void ) const {
	FLOAT len;

	len = x * x + y * y + z * z + w * w;
	return mxSqrt( len );
}

FORCEINLINE Quat& Quat::Normalize( void )
{
	FLOAT len;
	FLOAT ilength;

	len = this->GetLength();
	if ( len ) {
		ilength = 1.0f / len;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
	return *this;
}

FORCEINLINE void Quat::NormalizeFast( void )
{
	FLOAT invLength = mxInvSqrtEst( LengthSq() );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
}

FORCEINLINE FLOAT Quat::CalcW( void ) const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return mxSqrt( mxFabs( 1.0f - ( x * x + y * y + z * z ) ) );
}

FORCEINLINE INT Quat::GetDimension( void ) const {
	return 4;
}

FORCEINLINE const FLOAT *Quat::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Quat::ToFloatPtr( void ) {
	return &x;
}

FORCEINLINE Vec3D Quat::ToDirection() const
{
	FLOAT xz = x * z;
	FLOAT yw = y * w;
	FLOAT yz = y * z;
	FLOAT xw = x * w;
	FLOAT xx = x * x;
	FLOAT yy = y * y;

	return Vec3D(
		2.0f * ( xz - yw ),
		2.0f * ( yz + xw ),
		1.0f - 2.0f * ( xx + yy )
	);
}

//===========================================================================

// Multiply two quaternions.
//
FORCEINLINE Quat MulQuatQuat( const Quat& q1, const Quat& q2 )
{
	return Quat(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z );
}

// Multiply a quaternion with a vector.
//
FORCEINLINE Quat MulQuatVec3( const Quat& q, const Vec3D& w )
{
	return Quat(
		q.w * w.x + q.y * w.z - q.z * w.y,
		q.w * w.y + q.z * w.x - q.x * w.z,
		q.w * w.z + q.x * w.y - q.y * w.x,
		-q.x * w.x - q.y * w.y - q.z * w.z );
}

// Multiply a vector with a quaternion.
//
FORCEINLINE Quat MulVec3Quat( const Vec3D& w, const Quat& q )
{
	return Quat(
		w.x * q.w + w.y * q.z - w.z * q.y,
		w.y * q.w + w.z * q.x - w.x * q.z,
		w.z * q.w + w.x * q.y - w.y * q.x,
		-w.x * q.x - w.y * q.y - w.z * q.z );
}

/*
=============================================================================

	Compressed quaternion

=============================================================================
*/

class CQuat {
public:
	FLOAT			x;
	FLOAT			y;
	FLOAT			z;

public:
					CQuat( void );
					CQuat( FLOAT x, FLOAT y, FLOAT z );

	void 			Set( FLOAT x, FLOAT y, FLOAT z );

	FLOAT			operator[]( INT index ) const;
	FLOAT &			operator[]( INT index );

	bool			Compare( const CQuat &a ) const;						// exact compare, no epsilon
	bool			Compare( const CQuat &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const CQuat &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const CQuat &a ) const;					// exact compare, no epsilon

	INT				GetDimension( void ) const;

	Angles			ToAngles( void ) const;
	Rotation		ToRotation( void ) const;
	Matrix3			ToMat3( void ) const;
	Matrix4			ToMat4( void ) const;
	Quat			ToQuat( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToString( INT precision = 2 ) const;
};

FORCEINLINE CQuat::CQuat( void ) {
}

FORCEINLINE CQuat::CQuat( FLOAT x, FLOAT y, FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE void CQuat::Set( FLOAT x, FLOAT y, FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE FLOAT CQuat::operator[]( INT index ) const {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& CQuat::operator[]( INT index ) {
	Assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

FORCEINLINE bool CQuat::Compare( const CQuat &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

FORCEINLINE bool CQuat::Compare( const CQuat &a, const FLOAT epsilon ) const {
	if ( mxFabs( x - a.x ) > epsilon ) {
		return false;
	}
	if ( mxFabs( y - a.y ) > epsilon ) {
		return false;
	}
	if ( mxFabs( z - a.z ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool CQuat::operator==( const CQuat &a ) const {
	return Compare( a );
}

FORCEINLINE bool CQuat::operator!=( const CQuat &a ) const {
	return !Compare( a );
}

FORCEINLINE INT CQuat::GetDimension( void ) const {
	return 3;
}

FORCEINLINE Quat CQuat::ToQuat( void ) const
{
	// Take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1.
	return Quat( x, y, z, mxSqrt( mxFabs( 1.0f - ( x * x + y * y + z * z ) ) ) );
}

FORCEINLINE const FLOAT *CQuat::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *CQuat::ToFloatPtr( void ) {
	return &x;
}


mxSWIPED("DXSDK, DXUT, CD3DArcBall");
FORCEINLINE
Quat QuatFromBallPoints( const Vec3D& vFrom, const Vec3D& vTo )
{
	float	fDot = Dot( vFrom, vTo );
	Vec3D	vPart = Cross( vFrom, vTo ).GetNormalized();

	return Quat( vPart.x, vPart.y, vPart.z, fDot );
}

mxNAMESPACE_END

#endif /* !__MATH_QUATERNION_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
