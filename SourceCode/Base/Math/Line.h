/*
=============================================================================
	File:	Line.h
	Desc:	Lines.
=============================================================================
*/

#ifndef __MATH_LINE_H__
#define __MATH_LINE_H__

mxNAMESPACE_BEGIN

//
//	Line3D - limited 3D line (finite line segment).
//
class Line3D {
public:
	Vec3D	start, end;

public:
	Line3D()
	{}
	Line3D( const Vec3D& a, const Vec3D& b )
		: start( a ), end( b )
	{
		Assert( this->IsOk() );
	}

	Vec3D GetMiddle() const {
		return ( start + end ) * 0.5f;
	}

	bool IsOk() const {
		return ( start != end );
	}
};

//
//	Ray3D - infinite 3D line.
//
class Ray3D {
public:
	Vec3D	origin;
	Vec3D	direction;	// must be normalized

public:
	Ray3D()
	{}
	Ray3D( const Vec3D& p, const Vec3D& d )
		: origin( p ), direction( d )
	{
		Assert( this->IsOk() );
	}

	// Returns the closest point on the line to the given point.
	//
	Vec3D ClosestPoint( const Vec3D& point ) const {
		FLOAT t = direction.Dot( point - origin );
		return origin + direction * t;
	}

	// Returns the squared distance between the given point and the line.
	//
	FLOAT DistanceSqr( const Vec3D& point ) const {
		return ( ClosestPoint( point ) - point ).LengthSqr();
	}

	// Returns the distance between the given point and the line.
	//
	FLOAT Distance( const Vec3D& point ) const {
		return ( ClosestPoint( point ) - point ).GetLength();
	}

	bool IsOk() const {
		return ( direction.IsNormalized() );
	}
};



enum ELineStatus
{
	Lines_Parallel,
	Lines_Intersect,
	Lines_Skew
};

//
// returns:
//	OutP1 - closest point on the first ray to the second ray
//	OutP2 - closest point on the second ray to the first ray
//
inline ELineStatus IntersectLines( const Ray3D& a, const Ray3D& b, Vec3D &OutP1, Vec3D &OutP2 )
{
	const Vec3D	p1 = a.origin;
	const Vec3D	p2 = b.origin;
	const Vec3D	d1 = a.direction;
	const Vec3D	d2 = b.direction;

	const FLOAT denominator = Cross(d1,d2).LengthSqr();

	// If the lines are parallel (or coincident), then the cross product of d1 and d2 is the zero vector.

	if( denominator < VECTOR_EPSILON )
	{
		return Lines_Parallel;
	}

	const FLOAT t1 = Cross( (p2-p1), d2 ) * Cross(d1,d2) / denominator;
	const FLOAT t2 = Cross( (p2-p1), d1 ) * Cross(d1,d2) / denominator;

	// If the lines are skew, then I1 and I2 are the points of closest approach.

	const Vec3D I1 = p1 + d1 * t1;
	const Vec3D I2 = p2 + d2 * t2;

	OutP1 = I1;
	OutP2 = I2;

	// To distinguish between skew and intersecting lines,
	// we examine the distance between I1 and I2.

	const FLOAT lsqr = (I1 - I2).LengthSqr();
	return (lsqr > VECTOR_EPSILON) ? Lines_Skew : Lines_Intersect;
}

mxNAMESPACE_END

#endif /* !__MATH_LINE_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
