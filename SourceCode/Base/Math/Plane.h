/*
=============================================================================
	File:	Plane.h
	Desc:	3D plane class.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( Plane3D )
=============================================================================
*/

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__
mxSWIPED("idSoftware");
/*
=============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

=============================================================================
*/

mxNAMESPACE_BEGIN

class Vec3D;
class Vec4D;
class Matrix3;

const FLOAT ON_EPSILON				= 0.1f;		// Plane side epsilon.
const FLOAT DEGENERATE_DIST_EPSILON	= 1e-4f;

//
//	EPlaneSide - spatial relation to a plane.
//
enum EPlaneSide
{
	PLANESIDE_FRONT = 0,
	PLANESIDE_BACK,
	PLANESIDE_ON,
	PLANESIDE_CROSS,
};

//
// EPlaneType
//
enum EPlaneType
{
	PLANETYPE_X		= 0,
	PLANETYPE_Y,
	PLANETYPE_Z,
	PLANETYPE_NEGX,
	PLANETYPE_NEGY,
	PLANETYPE_NEGZ,
	PLANETYPE_TRUEAXIAL,	// all types < 6 are true axial planes
	PLANETYPE_ZEROX,
	PLANETYPE_ZEROY,
	PLANETYPE_ZEROZ,
	PLANETYPE_NONAXIAL,
};

//
//	Plane3D - consists of plane normal and distance from the origin.
//
mxALIGN_16( class Plane3D ) {
public:
	union {
		struct {
			FLOAT		a;
			FLOAT		b;
			FLOAT		c;
			FLOAT		d;	// D == ((-1) * distance from the origin)
		};
		mxSimdQuad		quad;
	};

public:
					Plane3D( void );
					Plane3D( FLOAT a, FLOAT b, FLOAT c, FLOAT d );
					Plane3D( const Plane3D& other );
					Plane3D( const Vec4D& vec );
					Plane3D( const Vec3D& normal, const FLOAT dist );
					Plane3D( const Vec3D& normal, const Vec3D& point );

	FLOAT			operator[]( mxUInt index ) const;
	FLOAT &			operator[]( mxUInt index );
	Plane3D			operator-() const;						// flips plane
	Plane3D &		operator=( const Vec3D &v );			// sets normal and sets Plane3D::d to zero
	Plane3D			operator+( const Plane3D &p ) const;	// add plane equations
	Plane3D			operator-( const Plane3D &p ) const;	// subtract plane equations
	Plane3D &		operator*=( const Matrix3 &m );			// Normal() *= m

	bool			Compare( const Plane3D &p ) const;						// exact compare, no epsilon
	bool			Compare( const Plane3D &p, const FLOAT epsilon ) const;	// compare with epsilon
	bool			Compare( const Plane3D &p, const FLOAT normalEps, const FLOAT distEps ) const;	// compare with epsilon
	bool			operator==(	const Plane3D &p ) const;					// exact compare, no epsilon
	bool			operator!=(	const Plane3D &p ) const;					// exact compare, no epsilon

	void			SetZero( void );						// zero plane
	void			SetNormal( const Vec3D &normal );		// sets the normal
	void			Set( const Vec4D& vec );
	const Vec3D &	Normal( void ) const;					// reference to const normal
	Vec3D &			Normal( void );							// reference to normal
	FLOAT			Normalize( bool fixDegenerate = true );	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal( void );			// fix degenerate normal
	bool			FixDegeneracies( FLOAT distEpsilon );	// fix degenerate normal and dist
	FLOAT			Dist( void ) const;						// returns: -d
	void			SetDist( const FLOAT dist );			// sets: d = -dist
	EPlaneType		Type( void ) const;						// returns plane type

	bool			FromPoints( const Vec3D &p1, const Vec3D &p2, const Vec3D &p3, bool fixDegenerate = true );
	bool			FromVecs( const Vec3D &dir1, const Vec3D &dir2, const Vec3D &p, bool fixDegenerate = true );
	void			FitThroughPoint( const Vec3D &p );	// assumes normal is valid
	bool			HeightFit( const Vec3D *points, const mxUInt numPoints );
	Plane3D			Translate( const Vec3D &translation ) const;
	Plane3D &		TranslateSelf( const Vec3D &translation );
	Plane3D			Rotate( const Vec3D &origin, const Matrix3 &axis ) const;
	Plane3D &		RotateSelf( const Vec3D &origin, const Matrix3 &axis );

	FLOAT			Distance( const Vec3D &v ) const;
	EPlaneSide		Side( const Vec3D &v, const FLOAT epsilon = 0.0f ) const;

	bool			LineIntersection( const Vec3D &start, const Vec3D &end ) const;
					// intersection point is start + dir * scale
	bool			RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale ) const;
	bool			PlaneIntersection( const Plane3D &plane, Vec3D &start, Vec3D &dir ) const;

					// assumes lookDir is normalized
	bool			FrontFacing( const Vec3D& lookDir ) const;

	mxUInt			GetDimension( void ) const;

	const Vec4D &	ToVec4( void ) const;
	Vec4D &			ToVec4( void );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( mxUInt precision = 2 ) const;

public:
	static const Plane3D plane_zero;	// ( N = (0,0,0), D = 0 )

	static const Plane3D plane_x;	// ( N = (1,0,0), D = 0 )
	static const Plane3D plane_y;	// ( N = (0,1,0), D = 0 )
	static const Plane3D plane_z;	// ( N = (0,0,1), D = 0 )
};

mxDECLARE_POD_TYPE( Plane3D );

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
	Vec3D &OutIntersection
);

/*================================
			Plane3D
================================*/

FORCEINLINE Plane3D::Plane3D( void ) {
}

FORCEINLINE Plane3D::Plane3D( FLOAT a, FLOAT b, FLOAT c, FLOAT d ) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

FORCEINLINE Plane3D::Plane3D( const Plane3D& other )
	: a( other.a ), b( other.b ), c( other.c ), d( other.d )
{}

FORCEINLINE Plane3D::Plane3D( const Vec4D& vec )
	: a( vec.x ), b( vec.y ), c( vec.z ), d( vec.w )
{}

FORCEINLINE Plane3D::Plane3D( const Vec3D &normal, const FLOAT dist )
{
	Assert( normal.IsNormalized() );
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

FORCEINLINE Plane3D::Plane3D( const Vec3D& normal, const Vec3D& point )
{
	Assert( normal.IsNormalized() );
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -1.0f * ( normal * point );
}

FORCEINLINE FLOAT Plane3D::operator[]( mxUInt index ) const {
	return ( &a )[ index ];
}

FORCEINLINE FLOAT& Plane3D::operator[]( mxUInt index ) {
	return ( &a )[ index ];
}

FORCEINLINE Plane3D Plane3D::operator-() const {
	return Plane3D( -a, -b, -c, -d );
}

FORCEINLINE Plane3D &Plane3D::operator=( const Vec3D &v ) { 
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

FORCEINLINE Plane3D Plane3D::operator+( const Plane3D &p ) const {
	return Plane3D( a + p.a, b + p.b, c + p.c, d + p.d );
}

FORCEINLINE Plane3D Plane3D::operator-( const Plane3D &p ) const {
	return Plane3D( a - p.a, b - p.b, c - p.c, d - p.d );
}

FORCEINLINE Plane3D &Plane3D::operator*=( const Matrix3 &m ) {
	Normal() *= m;
	return *this;
}

FORCEINLINE bool Plane3D::Compare( const Plane3D &p ) const {
	return ( a == p.a && b == p.b && c == p.c && d == p.d );
}

FORCEINLINE bool Plane3D::Compare( const Plane3D &p, const FLOAT epsilon ) const {
	if ( mxFabs( a - p.a ) > epsilon ) {
		return false;
	}
			
	if ( mxFabs( b - p.b ) > epsilon ) {
		return false;
	}

	if ( mxFabs( c - p.c ) > epsilon ) {
		return false;
	}

	if ( mxFabs( d - p.d ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool Plane3D::Compare( const Plane3D &p, const FLOAT normalEps, const FLOAT distEps ) const {
	if ( mxFabs( d - p.d ) > distEps ) {
		return false;
	}
	if ( !Normal().Compare( p.Normal(), normalEps ) ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Plane3D::operator==( const Plane3D &p ) const {
	return Compare( p );
}

FORCEINLINE bool Plane3D::operator!=( const Plane3D &p ) const {
	return !Compare( p );
}

FORCEINLINE void Plane3D::SetZero( void ) {
	a = b = c = d = 0.0f;
}

FORCEINLINE void Plane3D::SetNormal( const Vec3D &normal ) {
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

FORCEINLINE void Plane3D::Set( const Vec4D& vec ) {
	a = vec.x;
	b = vec.y;
	c = vec.z;
	d = vec.w;
}

FORCEINLINE const Vec3D &Plane3D::Normal( void ) const {
	return *reinterpret_cast<const Vec3D *>(&a);
}

FORCEINLINE Vec3D &Plane3D::Normal( void ) {
	return *reinterpret_cast<Vec3D *>(&a);
}

FORCEINLINE FLOAT Plane3D::Normalize( bool fixDegenerate ) {
	FLOAT length = reinterpret_cast<Vec3D *>(&a)->Normalize();

	if ( fixDegenerate ) {
		FixDegenerateNormal();
	}
	return length;
}

FORCEINLINE bool Plane3D::FixDegenerateNormal( void ) {
	return Normal().FixDegenerateNormal();
}

FORCEINLINE bool Plane3D::FixDegeneracies( FLOAT distEpsilon ) {
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if ( fixedNormal ) {
		if ( mxFabs( d - Math::Rint( d ) ) < distEpsilon ) {
			d = Math::Rint( d );
		}
	}
	return fixedNormal;
}

FORCEINLINE FLOAT Plane3D::Dist( void ) const {
	return -d;
}

FORCEINLINE void Plane3D::SetDist( const FLOAT dist ) {
	d = -dist;
}

FORCEINLINE bool Plane3D::FromPoints( const Vec3D &p1, const Vec3D &p2, const Vec3D &p3, bool fixDegenerate ) {
	Normal() = (p1 - p2).Cross( p3 - p2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p2 );
	return true;
}

FORCEINLINE bool Plane3D::FromVecs( const Vec3D &dir1, const Vec3D &dir2, const Vec3D &p, bool fixDegenerate ) {
	Normal() = dir1.Cross( dir2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p );
	return true;
}

FORCEINLINE void Plane3D::FitThroughPoint( const Vec3D &p ) {
	d = -( Normal() * p );
}

FORCEINLINE Plane3D Plane3D::Translate( const Vec3D &translation ) const {
	return Plane3D( a, b, c, d - translation * Normal() );
}

FORCEINLINE Plane3D &Plane3D::TranslateSelf( const Vec3D &translation ) {
	d -= translation * Normal();
	return *this;
}

FORCEINLINE Plane3D Plane3D::Rotate( const Vec3D &origin, const Matrix3 &axis ) const {
	Plane3D p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

FORCEINLINE Plane3D &Plane3D::RotateSelf( const Vec3D &origin, const Matrix3 &axis ) {
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

FORCEINLINE FLOAT Plane3D::Distance( const Vec3D &v ) const {
	//return (a * v.x + b * v.y) + (c * v.z + d);
	return a * v.x + b * v.y + c * v.z + d;
}

FORCEINLINE EPlaneSide Plane3D::Side( const Vec3D &v, const FLOAT epsilon ) const {
	FLOAT dist = Distance( v );
	if ( dist > epsilon ) {
		return PLANESIDE_FRONT;
	}
	else if ( dist < -epsilon ) {
		return PLANESIDE_BACK;
	}
	else {
		return PLANESIDE_ON;
	}
}

FORCEINLINE bool Plane3D::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
	FLOAT d1, d2, fraction;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if ( d1 == d2 ) {
		return false;
	}
	if ( d1 > 0.0f && d2 > 0.0f ) {
		return false;
	}
	if ( d1 < 0.0f && d2 < 0.0f ) {
		return false;
	}
	fraction = ( d1 / ( d1 - d2 ) );
	return ( fraction >= 0.0f && fraction <= 1.0f );
}

FORCEINLINE bool Plane3D::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale ) const {
	FLOAT d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if ( d2 == 0.0f ) {
		return false;
	}
	scale = -( d1 / d2 );
	return true;
}

FORCEINLINE bool Plane3D::FrontFacing( const Vec3D& lookDir ) const
{
	Assert( lookDir.IsNormalized() );
	return Normal().Dot( lookDir ) <= 0.0f;
}

FORCEINLINE mxUInt Plane3D::GetDimension( void ) const {
	return 4;
}

FORCEINLINE const Vec4D &Plane3D::ToVec4( void ) const {
	return *reinterpret_cast<const Vec4D *>(&a);
}

FORCEINLINE Vec4D &Plane3D::ToVec4( void ) {
	return *reinterpret_cast<Vec4D *>(&a);
}

FORCEINLINE const FLOAT *Plane3D::ToFloatPtr( void ) const {
	return reinterpret_cast<const FLOAT *>(&a);
}

FORCEINLINE FLOAT *Plane3D::ToFloatPtr( void ) {
	return reinterpret_cast<FLOAT *>(&a);
}

mxNAMESPACE_END

#endif /* !__MATH_PLANE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
