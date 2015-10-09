/*
=============================================================================
	File:	Vec4D.h
	Desc:	4D vector.
			Copyright (C) 2004 Id Software, Inc. ( idVec4 )
=============================================================================
*/

#ifndef __MATH_VEC4D_H__
#define __MATH_VEC4D_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

class Angles;
class Polar3;
class Matrix3;

//
//	Vec4D
//
mxALIGN_16(class) Vec4D {
public:
	union {
		struct {
			FLOAT	x;
			FLOAT	y;
			FLOAT	z;
			FLOAT	w;
		};
		mxSimdQuad	quad;
		struct {
			FLOAT	r;
			FLOAT	g;
			FLOAT	b;
			FLOAT	a;
		};
	};

public:
					Vec4D( void );
					explicit Vec4D( EInitZero );
					explicit Vec4D( EInitIdentity );
					explicit Vec4D( EInitInfinity );
					explicit Vec4D( const FLOAT x, const FLOAT y, const FLOAT z, const FLOAT w );
					explicit Vec4D( const Vec2D& xy, const Vec2D& zw );
					explicit Vec4D( const Vec3D& xyz, const FLOAT w );
					explicit Vec4D( const FLOAT xyzw );

	void 			Set( const FLOAT x, const FLOAT y, const FLOAT z, const FLOAT w );
	void 			Set( const Vec3D& xyz, FLOAT w );
	void			SetZero( void );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	const Vec4D		operator-() const;
	FLOAT			operator*( const Vec4D &a ) const;
	const Vec4D		operator*( const FLOAT a ) const;
	const Vec4D		operator/( const FLOAT a ) const;
	const Vec4D		operator+( const Vec4D &a ) const;
	const Vec4D		operator-( const Vec4D &a ) const;
	Vec4D &			operator+=( const Vec4D &a );
	Vec4D &			operator-=( const Vec4D &a );
	Vec4D &			operator/=( const Vec4D &a );
	Vec4D &			operator/=( const FLOAT a );
	Vec4D &			operator*=( const FLOAT a );

	friend const Vec4D	operator*( const FLOAT a, const Vec4D& b );

	bool			Compare( const Vec4D &a ) const;							// exact compare, no epsilon
	bool			Compare( const Vec4D &a, const FLOAT epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vec4D &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vec4D &a ) const;						// exact compare, no epsilon

	FLOAT			GetLength( void ) const;
	FLOAT			LengthSqr( void ) const;
	FLOAT			Normalize( void );			// returns length
	FLOAT			NormalizeFast( void );		// returns length

	INT				GetDimension( void ) const;

	const Vec2D &	ToVec2( void ) const;
	Vec2D &			ToVec2( void );
	const Vec3D &	ToVec3( void ) const;
	Vec3D &			ToVec3( void );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

	void			Lerp( const Vec4D &v1, const Vec4D &v2, const FLOAT l );

	Vec4D & 		RotateAboutX( FLOAT angle );
	Vec4D & 		RotateAboutY( FLOAT angle );
	Vec4D & 		RotateAboutZ( FLOAT angle );
	Vec4D & 		RotateAboutAxis( FLOAT angle, const Vec3D& axis );

	template< typename TYPE >
	FORCEINLINE Vec4D & operator = ( const TYPE& other )
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

public:
	static const Vec4D vec4_origin;
	static const Vec4D vec4_unit;
};

mxDECLARE_BUILTIN_TYPE( Vec4D,	ETypeKind::Type_Vec4D );

FORCEINLINE Vec4D::Vec4D( void ) {
}

FORCEINLINE Vec4D::Vec4D( EInitZero )
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

FORCEINLINE Vec4D::Vec4D( EInitIdentity )
{
	this->x = 1.0f;
	this->y = 1.0f;
	this->z = 1.0f;
	this->w = 1.0f;
}

FORCEINLINE Vec4D::Vec4D( EInitInfinity )
{
	this->x = BIG_NUMBER;
	this->y = BIG_NUMBER;
	this->z = BIG_NUMBER;
	this->w = BIG_NUMBER;
}

FORCEINLINE Vec4D::Vec4D( const FLOAT x, const FLOAT y, const FLOAT z, const FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE Vec4D::Vec4D( const Vec2D& xy, const Vec2D& zw ) {
	this->x = xy.x;
	this->y = xy.y;
	this->z = zw.x;
	this->w = zw.y;
}

FORCEINLINE Vec4D::Vec4D( const Vec3D& xyz, const FLOAT w ) {
	this->x = xyz.x;
	this->y = xyz.y;
	this->z = xyz.z;
	this->w = w;
}

FORCEINLINE Vec4D::Vec4D( const FLOAT xyzw ) {
	this->x = xyzw;
	this->y = xyzw;
	this->z = xyzw;
	this->w = xyzw;
}

FORCEINLINE void Vec4D::Set( const FLOAT x, const FLOAT y, const FLOAT z, const FLOAT w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

FORCEINLINE void Vec4D::Set( const Vec3D& xyz, FLOAT w )
{
	this->x = xyz.x;
	this->y = xyz.y;
	this->z = xyz.z;
	this->w = w;
}

FORCEINLINE void Vec4D::SetZero( void ) {
	x = y = z = w = 0.0f;
}

FORCEINLINE FLOAT Vec4D::operator[]( INT index ) const {
	return ( &x )[ index ];
}

FORCEINLINE FLOAT& Vec4D::operator[]( INT index ) {
	return ( &x )[ index ];
}

FORCEINLINE const Vec4D Vec4D::operator-() const {
	return Vec4D( -x, -y, -z, -w );
}

FORCEINLINE const Vec4D Vec4D::operator-( const Vec4D &a ) const {
	return Vec4D( x - a.x, y - a.y, z - a.z, w - a.w );
}

FORCEINLINE FLOAT Vec4D::operator*( const Vec4D &a ) const {
	return x * a.x + y * a.y + z * a.z + w * a.w;
}

FORCEINLINE const Vec4D Vec4D::operator*( const FLOAT a ) const {
	return Vec4D( x * a, y * a, z * a, w * a );
}

FORCEINLINE const Vec4D Vec4D::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return Vec4D( x * inva, y * inva, z * inva, w * inva );
}

FORCEINLINE const Vec4D operator*( const FLOAT a, const Vec4D& b ) {
	return Vec4D( b.x * a, b.y * a, b.z * a, b.w * a );
}

FORCEINLINE const Vec4D Vec4D::operator+( const Vec4D &a ) const {
	return Vec4D( x + a.x, y + a.y, z + a.z, w + a.w );
}

FORCEINLINE Vec4D &Vec4D::operator+=( const Vec4D &a ) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

FORCEINLINE Vec4D &Vec4D::operator/=( const Vec4D &a ) {
	x /= a.x;
	y /= a.y;
	z /= a.z;
	w /= a.w;

	return *this;
}

FORCEINLINE Vec4D &Vec4D::operator/=( const FLOAT a ) {
	FLOAT inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	w *= inva;

	return *this;
}

FORCEINLINE Vec4D &Vec4D::operator-=( const Vec4D &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

FORCEINLINE Vec4D &Vec4D::operator*=( const FLOAT a ) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

FORCEINLINE bool Vec4D::Compare( const Vec4D &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && w == a.w );
}

FORCEINLINE bool Vec4D::Compare( const Vec4D &a, const FLOAT epsilon ) const {
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

FORCEINLINE bool Vec4D::operator==( const Vec4D &a ) const {
	return Compare( a );
}

FORCEINLINE bool Vec4D::operator!=( const Vec4D &a ) const {
	return !Compare( a );
}

FORCEINLINE FLOAT Vec4D::GetLength( void ) const {
	return ( FLOAT )mxSqrt( x * x + y * y + z * z + w * w );
}

FORCEINLINE FLOAT Vec4D::LengthSqr( void ) const {
	return ( x * x + y * y + z * z + w * w );
}

FORCEINLINE FLOAT Vec4D::Normalize( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z + w * w;
	invLength = mxInvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE FLOAT Vec4D::NormalizeFast( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z + w * w;
	invLength = mxInvSqrtEst( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE INT Vec4D::GetDimension( void ) const {
	return 4;
}

FORCEINLINE const Vec2D &Vec4D::ToVec2( void ) const {
	return *reinterpret_cast<const Vec2D *>(this);
}

FORCEINLINE Vec2D &Vec4D::ToVec2( void ) {
	return *reinterpret_cast<Vec2D *>(this);
}

FORCEINLINE const Vec3D &Vec4D::ToVec3( void ) const {
	return *reinterpret_cast<const Vec3D *>(this);
}

FORCEINLINE Vec3D &Vec4D::ToVec3( void ) {
	return *reinterpret_cast<Vec3D *>(this);
}

FORCEINLINE const FLOAT *Vec4D::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Vec4D::ToFloatPtr( void ) {
	return &x;
}

mxNAMESPACE_END

#endif /* !__MATH_VEC4D_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
