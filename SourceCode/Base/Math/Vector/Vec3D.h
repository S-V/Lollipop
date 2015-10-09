/*
=============================================================================
	File:	Vec3D.h
	Desc:	3D vector.
			Copyright (C) 2004 Id Software, Inc. ( idVec3 )

	Note: some returned values have been declared as 'const', see:
	http://tfpsly.free.fr/english/3d/code.html#Vector and matrix classes, math operators
=============================================================================
*/

#ifndef __MATH_VEC3D_H__
#define __MATH_VEC3D_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

class Angles;
class Polar3;
class Matrix3;

//
//	Vec3D
//

struct Vec3D
{
	FLOAT		x;
	FLOAT		y;
	FLOAT		z;

public:
					Vec3D( void );	// empty ctor leaves data uninitialized
					explicit Vec3D( const FLOAT f );	// initializes all three components to the given value
					explicit Vec3D( const Vec2D& xy, const FLOAT z );
					explicit Vec3D( const FLOAT x, const FLOAT y, const FLOAT z );

	void 			Set( const FLOAT x, const FLOAT y, const FLOAT z );
	void			SetAll( const FLOAT value );
	void			SetZero( void );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	const Vec3D		operator-() const;
	Vec3D &			operator=( const Vec3D &a );		// required because of a msvc 6 & 7 bug
	FLOAT			operator*( const Vec3D &a ) const;	// Dot product.
	const Vec3D		operator^( const Vec3D &a ) const;	// Cross product.
	const Vec3D		operator*( const FLOAT a ) const;
	const Vec3D		operator/( const FLOAT a ) const;
	const Vec3D		operator+( const Vec3D &a ) const;
	const Vec3D		operator-( const Vec3D &a ) const;
	Vec3D &			operator+=( const Vec3D &a );
	Vec3D &			operator-=( const Vec3D &a );
	Vec3D &			operator/=( const Vec3D &a );
	Vec3D &			operator/=( const FLOAT a );
	Vec3D &			operator*=( const FLOAT a );

	friend const Vec3D	operator*( const FLOAT a, const Vec3D b );

	bool			Compare( const Vec3D &a ) const;						// exact compare, no epsilon
	bool			Compare( const Vec3D &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Vec3D &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vec3D &a ) const;						// exact compare, no epsilon

	bool			FixDegenerateNormal( void );	// fix degenerate axial cases
	bool			FixDenormals( void );			// change tiny numbers to zero
	bool			IsNormalized( const FLOAT epsilon = VECTOR_EPSILON ) const;
	bool			IsZero() const;
	bool			Any() const;	// returns 'true' if any components of the vector are non-zero; otherwise, returns 'false'
	bool			All() const;	// returns 'true' if if all components of the vector are non-zero; otherwise, returns 'false'

	// Return a vector with the absolute values of each element.
	const Vec3D		Absolute() const;

	FLOAT			Dot( const Vec3D &a ) const;
	const Vec3D		Cross( const Vec3D &a ) const;
	Vec3D &			Cross( const Vec3D &a, const Vec3D &b );
	FLOAT			GetLength( void ) const;
	FLOAT			LengthSqr( void ) const;
	FLOAT			LengthFast( void ) const;
	const Vec3D		GetNormalized() const;
	FLOAT			Normalize( void );				// returns length
	FLOAT			NormalizeSafe( void );
	void			NormalizeFast( void );			// unsafe and approximate!
	Vec3D &			Truncate( FLOAT length );		// cap length
	void			Clamp( const Vec3D &min, const Vec3D &max );
	void			Snap( void );					// snap to closest integer value
	void			SnapInt( void );				// snap towards integer (floor)

	INT				GetDimension( void ) const;

	FLOAT			ToYaw( void ) const;
	FLOAT			ToPitch( void ) const;
	const Angles	ToAngles( void ) const;
	const Polar3	ToPolar( void ) const;
	const Matrix3	ToMat3( void ) const;		// vector should be normalized
	const Matrix3	ToMat3( INT axis ) const;		// vector should be normalized
	const Vec2D &	ToVec2( void ) const;
	Vec2D &			ToVec2( void );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

	void			NormalVectors( Vec3D &left, Vec3D &down ) const;	// vector should be normalized
	void			OrthogonalBasis( Vec3D &left, Vec3D &up ) const;

					// Slide off of the impacting surface
	void			ProjectOntoPlane( const Vec3D &normal, const FLOAT overBounce = 1.0f );
	bool			ProjectAlongPlane( const Vec3D &normal, const FLOAT epsilon, const FLOAT overBounce = 1.0f );

	void			ProjectSelfOntoSphere( const FLOAT radius );

	void			Lerp( const Vec3D &v1, const Vec3D &v2, const FLOAT l );
	void			SLerp( const Vec3D &v1, const Vec3D &v2, const FLOAT l );

	Vec3D & 		RotateAboutX( FLOAT angle );
	Vec3D & 		RotateAboutY( FLOAT angle );
	Vec3D & 		RotateAboutZ( FLOAT angle );
	Vec3D & 		RotateAboutAxis( FLOAT angle, const Vec3D& axis );

	bool			IsValid() const;

	template< typename TYPE >
	FORCEINLINE Vec3D & operator = ( const TYPE& other )
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

public:
	static const Vec3D	GetRandom( INT seed = 0 );
	static const Vec3D	GetRandomNormalized( INT seed = 0 );

	static inline const Vec3D StaticGetAxis( const EAxisType axis )
	{
		return (axis == AXIS_X) ? Vec3D::vec3_unit_x
			: (axis == AXIS_Y) ? Vec3D::vec3_unit_y
			: /*(axis == AXIS_Z) ?*/ Vec3D::vec3_unit_z
			;
	}

public:
	static const Vec3D   vec3_zero;	// ( 0, 0, 0 ).
	static const Vec3D   vec3_unit;	// ( 1, 1, 1 ).

	static const Vec3D   vec3_unit_x;		// unit vector along +x axis
	static const Vec3D   vec3_unit_y;		// unit vector along +y axis
	static const Vec3D   vec3_unit_z;		// unit vector along +z axis
	static const Vec3D   vec3_neg_unit_x;	// unit vector along -x axis
	static const Vec3D   vec3_neg_unit_y;	// unit vector along -y axis
	static const Vec3D   vec3_neg_unit_z;	// unit vector along -z axis

	static const Vec3D   vec3_plus_inf;
	static const Vec3D   vec3_minus_inf;
};

mxDECLARE_BUILTIN_TYPE( Vec3D,	ETypeKind::Type_Vec3D );


inline mxTextStream & operator << ( mxTextStream & rStream, const Vec3D& v )
{
	rStream.Printf("Vec3D( %.3f, %.3f, %.3f )", v.x, v.y, v.z );
	return rStream;
}

FORCEINLINE Vec3D::Vec3D( void ) {
}

FORCEINLINE Vec3D::Vec3D( const FLOAT f ) {
	this->x = f;
	this->y = f;
	this->z = f;
}

FORCEINLINE Vec3D::Vec3D( const Vec2D& xy, const FLOAT z ) {
	this->x = xy.x;
	this->y = xy.y;
	this->z = z;
}

FORCEINLINE Vec3D::Vec3D( const FLOAT x, const FLOAT y, const FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE FLOAT Vec3D::operator[]( const INT index ) const {
	return ( &x )[ index ];
}

FORCEINLINE FLOAT &Vec3D::operator[]( const INT index ) {
	return ( &x )[ index ];
}

FORCEINLINE void Vec3D::Set( const FLOAT x, const FLOAT y, const FLOAT z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

FORCEINLINE void Vec3D::SetAll( const FLOAT value ) {
	this->x = value;
	this->y = value;
	this->z = value;
}

FORCEINLINE void Vec3D::SetZero( void ) {
	x = y = z = 0.0f;
}

FORCEINLINE const Vec3D Vec3D::operator-() const {
	return Vec3D( -x, -y, -z );
}

FORCEINLINE Vec3D &Vec3D::operator=( const Vec3D &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

FORCEINLINE const Vec3D Vec3D::operator-( const Vec3D &a ) const {
	return Vec3D( x - a.x, y - a.y, z - a.z );
}

FORCEINLINE FLOAT Vec3D::operator*( const Vec3D &a ) const {
	return Dot( a );
}

FORCEINLINE const Vec3D Vec3D::Absolute() const {
	return Vec3D( mxFabs(x), mxFabs(y), mxFabs(z) );
}

FORCEINLINE FLOAT Vec3D::Dot( const Vec3D &a ) const {
	return x * a.x + y * a.y + z * a.z;
}

FORCEINLINE const Vec3D Vec3D::operator*( const FLOAT a ) const {
	return Vec3D( x * a, y * a, z * a );
}

FORCEINLINE const Vec3D Vec3D::operator^( const Vec3D &a ) const {
	return Vec3D(
		y * a.z - z * a.y,
		z * a.x - x * a.z,
		x * a.y - y * a.x );
}

FORCEINLINE const Vec3D Vec3D::operator/( const FLOAT a ) const {
	FLOAT inva = 1.0f / a;
	return Vec3D( x * inva, y * inva, z * inva );
}

FORCEINLINE const Vec3D operator*( const FLOAT a, const Vec3D b ) {
	return Vec3D( b.x * a, b.y * a, b.z * a );
}

FORCEINLINE const Vec3D Vec3D::operator+( const Vec3D &a ) const {
	return Vec3D( x + a.x, y + a.y, z + a.z );
}

FORCEINLINE Vec3D &Vec3D::operator+=( const Vec3D &a ) {
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator/=( const Vec3D &a ) {
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator/=( const FLOAT a ) {
	FLOAT inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator-=( const Vec3D &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}

FORCEINLINE Vec3D &Vec3D::operator*=( const FLOAT a ) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

FORCEINLINE bool Vec3D::Compare( const Vec3D &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

FORCEINLINE bool Vec3D::Compare( const Vec3D &a, const FLOAT epsilon ) const {
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

FORCEINLINE bool Vec3D::operator==( const Vec3D &a ) const {
	return Compare( a );
}

FORCEINLINE bool Vec3D::operator!=( const Vec3D &a ) const {
	return !Compare( a );
}

// fast vector normalize routine that does not check to make sure
// that length != 0, nor does it return length, uses rsqrt approximation
//
FORCEINLINE void Vec3D::NormalizeFast( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = mxInvSqrtEst( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
}

FORCEINLINE bool Vec3D::FixDegenerateNormal( void ) {
	if ( x == 0.0f ) {
		if ( y == 0.0f ) {
			if ( z > 0.0f ) {
				if ( z != 1.0f ) {
					z = 1.0f;
					return true;
				}
			} else {
				if ( z != -1.0f ) {
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if ( z == 0.0f ) {
			if ( y > 0.0f ) {
				if ( y != 1.0f ) {
					y = 1.0f;
					return true;
				}
			} else {
				if ( y != -1.0f ) {
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	} else if ( y == 0.0f ) {
		if ( z == 0.0f ) {
			if ( x > 0.0f ) {
				if ( x != 1.0f ) {
					x = 1.0f;
					return true;
				}
			} else {
				if ( x != -1.0f ) {
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if ( mxFabs( x ) == 1.0f ) {
		if ( y != 0.0f || z != 0.0f ) {
			y = z = 0.0f;
			return true;
		}
		return false;
	} else if ( mxFabs( y ) == 1.0f ) {
		if ( x != 0.0f || z != 0.0f ) {
			x = z = 0.0f;
			return true;
		}
		return false;
	} else if ( mxFabs( z ) == 1.0f ) {
		if ( x != 0.0f || y != 0.0f ) {
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

FORCEINLINE bool Vec3D::FixDenormals( void ) {
	bool denormal = false;
	if ( mxFabs( x ) < 1e-30f ) {
		x = 0.0f;
		denormal = true;
	}
	if ( mxFabs( y ) < 1e-30f ) {
		y = 0.0f;
		denormal = true;
	}
	if ( mxFabs( z ) < 1e-30f ) {
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

FORCEINLINE bool Vec3D::IsNormalized( const FLOAT epsilon /* = VECTOR_EPSILON */ ) const {
	return mxFabs( LengthSqr() - 1.0f ) < epsilon;
}

// NOTE: (vec3_unit_x * -vec3_unit_z == vec3_unit_y).
FORCEINLINE const Vec3D Vec3D::Cross( const Vec3D &a ) const {
	return Vec3D(
		y * a.z - z * a.y,
		z * a.x - x * a.z,
		x * a.y - y * a.x );
}
// NOTE: (vec3_unit_x * -vec3_unit_z == vec3_unit_y).
FORCEINLINE Vec3D & Vec3D::Cross( const Vec3D &a, const Vec3D &b ) {
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

FORCEINLINE FLOAT Vec3D::GetLength( void ) const {
	return ( FLOAT )mxSqrt( x * x + y * y + z * z );
}

FORCEINLINE FLOAT Vec3D::LengthSqr( void ) const {
	return ( x * x + y * y + z * z );
}

FORCEINLINE FLOAT Vec3D::LengthFast( void ) const {
	FLOAT sqrLength;

	sqrLength = x * x + y * y + z * z;
	return sqrLength * mxInvSqrt( sqrLength );
}

FORCEINLINE const Vec3D Vec3D::GetNormalized() const {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = mxInvSqrt( sqrLength );

	return Vec3D(
		x * invLength,
		y * invLength,
		z * invLength
	);
}

FORCEINLINE FLOAT Vec3D::Normalize( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = mxInvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE FLOAT Vec3D::NormalizeSafe( void ) {
	FLOAT sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	if( sqrLength < SMALL_NUMBER ) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		return 0.0f;
	}
	invLength = mxInvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

FORCEINLINE Vec3D & Vec3D::Truncate( FLOAT length ) {
	FLOAT length2;
	FLOAT ilength;

	if ( !length ) {
		SetZero();
	}
	else {
		length2 = LengthSqr();
		if ( length2 > length * length ) {
			ilength = length * mxInvSqrt( length2 );
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}

	return *this;
}

FORCEINLINE void Vec3D::Clamp( const Vec3D &min, const Vec3D &max ) {
	if ( x < min.x ) {
		x = min.x;
	} else if ( x > max.x ) {
		x = max.x;
	}
	if ( y < min.y ) {
		y = min.y;
	} else if ( y > max.y ) {
		y = max.y;
	}
	if ( z < min.z ) {
		z = min.z;
	} else if ( z > max.z ) {
		z = max.z;
	}
}

FORCEINLINE void Vec3D::Snap( void ) {
	x = Math::Floor( x + 0.5f );
	y = Math::Floor( y + 0.5f );
	z = Math::Floor( z + 0.5f );
}

FORCEINLINE void Vec3D::SnapInt( void ) {
	x = FLOAT( INT( x ) );
	y = FLOAT( INT( y ) );
	z = FLOAT( INT( z ) );
}

FORCEINLINE INT Vec3D::GetDimension( void ) const {
	return 3;
}

FORCEINLINE const Vec2D &Vec3D::ToVec2( void ) const {
	return *reinterpret_cast<const Vec2D *>(this);
}

FORCEINLINE Vec2D &Vec3D::ToVec2( void ) {
	return *reinterpret_cast<Vec2D *>(this);
}

FORCEINLINE const FLOAT *Vec3D::ToFloatPtr( void ) const {
	return &x;
}

FORCEINLINE FLOAT *Vec3D::ToFloatPtr( void ) {
	return &x;
}

FORCEINLINE void Vec3D::NormalVectors( Vec3D &left, Vec3D &down ) const {
	FLOAT d;

	d = x * x + y * y;
	if ( !d ) {
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	} else {
		d = mxInvSqrt( d );
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross( *this );
}

FORCEINLINE void Vec3D::OrthogonalBasis( Vec3D &left, Vec3D &up ) const {
	FLOAT l, s;

	if ( mxFabs( z ) > 0.7f ) {
		l = y * y + z * z;
		s = mxInvSqrt( l );
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else {
		l = x * x + y * y;
		s = mxInvSqrt( l );
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

// Slide off of the impacting surface
//
FORCEINLINE void Vec3D::ProjectOntoPlane( const Vec3D &normal, const FLOAT overBounce ) {
	FLOAT backoff;
	
	backoff = *this * normal;
	
	if ( overBounce != 1.0 ) {
		if ( backoff < 0 ) {
			backoff *= overBounce;
		} else {
			backoff /= overBounce;
		}
	}

	*this -= backoff * normal;
}

FORCEINLINE bool Vec3D::ProjectAlongPlane( const Vec3D &normal, const FLOAT epsilon, const FLOAT overBounce ) {
	Vec3D cross;
	FLOAT len;

	cross = this->Cross( normal ).Cross( (*this) );
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if ( mxFabs( len ) < epsilon ) {
		return false;
	}
	cross *= overBounce * ( normal * (*this) ) / len;
	(*this) -= cross;
	return true;
}

FORCEINLINE bool Vec3D::IsValid() const
{
	return IsValidFloat( x )
		&& IsValidFloat( y )
		&& IsValidFloat( z );
}

FORCEINLINE bool Vec3D::IsZero() const
{
	return ( x == 0.0f ) && ( y == 0.0f ) && ( z == 0.0f );
}

FORCEINLINE bool Vec3D::Any() const
{
	return ( x != 0.0f ) || ( y != 0.0f ) || ( z != 0.0f );
}

FORCEINLINE bool Vec3D::All() const
{
	return ( x != 0.0f ) && ( y != 0.0f ) && ( z != 0.0f );
}

FORCEINLINE const Vec3D Vec3D::GetRandom( INT seed ) {
	mxRandom  random( seed );
	Vec3D v( random.CRandomFloat(), random.CRandomFloat(), random.CRandomFloat() );
	return v;
}

FORCEINLINE const Vec3D Vec3D::GetRandomNormalized( INT seed ) {
	return GetRandom( seed ).GetNormalized();
}

//===============================================================
//
//	Polar3
//
//===============================================================

class Polar3 {
public:	
	FLOAT			radius, theta, phi;

					Polar3( void );
					explicit Polar3( const FLOAT radius, const FLOAT theta, const FLOAT phi );

	void 			Set( const FLOAT radius, const FLOAT theta, const FLOAT phi );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	Polar3			operator-() const;
	Polar3 &		operator=( const Polar3 &a );

	Vec3D			ToVec3( void ) const;
};

FORCEINLINE Polar3::Polar3( void ) {
}

FORCEINLINE Polar3::Polar3( const FLOAT radius, const FLOAT theta, const FLOAT phi ) {
	Assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}
	
FORCEINLINE void Polar3::Set( const FLOAT radius, const FLOAT theta, const FLOAT phi ) {
	Assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

FORCEINLINE FLOAT Polar3::operator[]( const INT index ) const {
	return ( &radius )[ index ];
}

FORCEINLINE FLOAT &Polar3::operator[]( const INT index ) {
	return ( &radius )[ index ];
}

FORCEINLINE Polar3 Polar3::operator-() const {
	return Polar3( radius, -theta, -phi );
}

FORCEINLINE Polar3 &Polar3::operator=( const Polar3 &a ) { 
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

FORCEINLINE Vec3D Polar3::ToVec3( void ) const {
	FLOAT sp, cp, st, ct;
	mxSinCos( phi, sp, cp );
	mxSinCos( theta, st, ct );
 	return Vec3D( cp * radius * ct, cp * radius * st, radius * sp );
}



FORCEINLINE FLOAT Dot( const Vec3D &a, const Vec3D &b ) {
	return a.Dot(b);
}

FORCEINLINE Vec3D Cross( const Vec3D &a, const Vec3D &b ) {
	return a.Cross(b);
}


FORCEINLINE FLOAT AngleBetween( const Vec3D& vA, const Vec3D& vB )
{
	float	fDot = Dot( vA, vB );
	return mxACos( fDot / (vA.GetLength() * vB.GetLength()) );
}

mxNAMESPACE_END

#endif /* !__MATH_VEC3D_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
