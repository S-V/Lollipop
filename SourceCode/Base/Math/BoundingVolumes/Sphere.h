/*
=============================================================================
	File:	Sphere.h
	Desc:	Bounding sphere.
=============================================================================
*/

#ifndef __BOUNDING_VOLUMES_SPHERE_H__
#define __BOUNDING_VOLUMES_SPHERE_H__
mxSWIPED("Id Software");
mxNAMESPACE_BEGIN

//
//	Sphere
//
mxALIGN_16(struct Sphere)
{
	Vec3D			Center;//mCenter
	FLOAT			Radius;//mRadius

public:
					Sphere( void );
					explicit Sphere( EInitZero );
					explicit Sphere( EInitInfinity );
					explicit Sphere( const Vec3D &point );
					explicit Sphere( const Vec3D &point, const FLOAT r );

	FLOAT			operator[]( const UINT index ) const;
	FLOAT &			operator[]( const UINT index );
	Sphere			operator+( const Vec3D &t ) const;				// returns translated sphere
	Sphere &		operator+=( const Vec3D &t );					// translate the sphere
	Sphere			operator+( const Sphere &s ) const;
	Sphere &		operator+=( const Sphere &s );

	bool			Compare( const Sphere &a ) const;						// exact compare, no epsilon
	bool			Compare( const Sphere &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const Sphere &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const Sphere &a ) const;					// exact compare, no epsilon

	void			Clear( void );									// inside out sphere
	void			SetZero( void );								// single point at origin
	void			SetInfinity();
	void			SetOrigin( const Vec3D &o );					// set origin of sphere
	void			SetRadius( const FLOAT r );						// set sphere radius
	void			Set( FLOAT x, FLOAT y, FLOAT z, FLOAT radius );

	const Vec3D &	GetOrigin( void ) const;						// returns origin of sphere
	FLOAT			GetRadius( void ) const;						// returns sphere radius
	bool			IsCleared( void ) const;						// returns true if sphere is inside out

	bool			AddPoint( const Vec3D &p );						// add the point, returns true if the sphere expanded
	bool			AddSphere( const Sphere &s );					// add the sphere, returns true if the sphere expanded
	Sphere			Expand( const FLOAT d ) const;					// return bounds expanded in all directions with the given value
	Sphere &		ExpandSelf( const FLOAT d );					// expand bounds in all directions with the given value
	Sphere			Translate( const Vec3D &translation ) const;
	Sphere &		TranslateSelf( const Vec3D &translation );

	Sphere			Transform( const Vec3D& translation, const Quat& rotation, const FLOAT scale ) const;
	Sphere &		TransformSelf( const Vec3D& translation, const Quat& rotation, const FLOAT scale );

	FLOAT			PlaneDistance( const Plane3D &plane ) const;
	EPlaneSide		PlaneSide( const Plane3D &plane, const FLOAT epsilon = ON_EPSILON ) const;

	bool			ContainsPoint( const Vec3D& p ) const;			// includes touching
	bool			ContainsSphere( const Sphere& s ) const;	// includes touching
	bool			IntersectsSphere( const Sphere& s ) const;	// includes touching
	bool			LineIntersection( const Vec3D& start, const Vec3D& end ) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const Vec3D& start, const Vec3D& dir, FLOAT &scale1, FLOAT &scale2 ) const;

					// construct from the given three points
	void			FromPoints( const Vec3D& a, const Vec3D& b, const Vec3D& c );
					// Tight sphere for a point set.
	void			FromPoints( const Vec3D *points, const UINT numPoints );
					// Most tight sphere for a translation.
	void			FromPointTranslation( const Vec3D &point, const Vec3D &translation );
	void			FromSphereTranslation( const Sphere &sphere, const Vec3D &start, const Vec3D &translation );
					// Most tight sphere for a rotation.
	void			FromPointRotation( const Vec3D &point, const Rotation &rotation );
	void			FromSphereRotation( const Sphere &sphere, const Vec3D &start, const Rotation &rotation );

	void			AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const;

	bool			IsValid() const;

public:
	static const Sphere	ZERO_SPHERE;
	static const Sphere	UNIT_SPHERE;
};
mxDECLARE_POD_TYPE(Sphere);

FORCEINLINE Sphere::Sphere( void ) {
}

FORCEINLINE Sphere::Sphere( const Vec3D &point ) {
	Center = point;
	Radius = 0.0f;
}

FORCEINLINE Sphere::Sphere( const Vec3D &point, const FLOAT r ) {
	Center = point;
	Radius = r;
}

FORCEINLINE Sphere::Sphere( EInitZero ) {
	Center.SetZero();
	Radius = 0.0f;
}

FORCEINLINE Sphere::Sphere( EInitInfinity ) {
	Center.SetZero();
	Radius = BIG_NUMBER;
}

FORCEINLINE FLOAT Sphere::operator[]( const UINT index ) const {
	return ((FLOAT *) &Center)[index];
}

FORCEINLINE FLOAT &Sphere::operator[]( const UINT index ) {
	return ((FLOAT *) &Center)[index];
}

FORCEINLINE Sphere Sphere::operator+( const Vec3D &t ) const {
	return Sphere( Center + t, Radius );
}

FORCEINLINE Sphere &Sphere::operator+=( const Vec3D &t ) {
	Center += t;
	return *this;
}

FORCEINLINE bool Sphere::Compare( const Sphere &a ) const {
	return ( Center.Compare( a.Center ) && Radius == a.Radius );
}

FORCEINLINE bool Sphere::Compare( const Sphere &a, const FLOAT epsilon ) const {
	return ( Center.Compare( a.Center, epsilon ) && mxFabs( Radius - a.Radius ) <= epsilon );
}

FORCEINLINE bool Sphere::operator==( const Sphere &a ) const {
	return Compare( a );
}

FORCEINLINE bool Sphere::operator!=( const Sphere &a ) const {
	return !Compare( a );
}

FORCEINLINE void Sphere::Clear( void ) {
	Center.SetZero();
	Radius = -1.0f;
}

FORCEINLINE void Sphere::SetZero( void ) {
	Center.SetZero();
	Radius = 0.0f;
}

FORCEINLINE void Sphere::SetInfinity() {
	Center.SetZero();
	Radius = BIG_NUMBER;
}

FORCEINLINE void Sphere::SetOrigin( const Vec3D &o ) {
	Center = o;
}

FORCEINLINE void Sphere::SetRadius( const FLOAT r ) {
	Radius = r;
}

FORCEINLINE void Sphere::Set( FLOAT x, FLOAT y, FLOAT z, FLOAT radius ) {
	Center.x = x;
	Center.y = y;
	Center.z = z;
	Radius = radius;
}

FORCEINLINE const Vec3D &Sphere::GetOrigin( void ) const {
	return Center;
}

FORCEINLINE FLOAT Sphere::GetRadius( void ) const {
	return Radius;
}

FORCEINLINE bool Sphere::IsCleared( void ) const {
	return ( Radius < 0.0f );
}

FORCEINLINE bool Sphere::AddPoint( const Vec3D &p ) {
	if ( Radius < 0.0f ) {
		Center = p;
		Radius = 0.0f;
		return true;
	}
	else {
		FLOAT r = ( p - Center ).LengthSqr();
		if ( r > Radius * Radius ) {
			r = mxSqrt( r );
			Center += ( p - Center ) * 0.5f * (1.0f - Radius / r );
			Radius += 0.5f * ( r - Radius );
			return true;
		}
		return false;
	}
}

FORCEINLINE bool Sphere::AddSphere( const Sphere &s ) {
	if ( Radius < 0.0f ) {
		Center = s.Center;
		Radius = s.Radius;
		return true;
	}
	else {
		FLOAT r = ( s.Center - Center ).LengthSqr();
		if ( r > ( Radius + s.Radius ) * ( Radius + s.Radius ) ) {
			r = mxSqrt( r );
			Center += ( s.Center - Center ) * 0.5f * (1.0f - Radius / ( r + s.Radius ) );
			Radius += 0.5f * ( ( r + s.Radius ) - Radius );
			return true;
		}
		return false;
	}
}

FORCEINLINE Sphere Sphere::Expand( const FLOAT d ) const {
	return Sphere( Center, Radius + d );
}

FORCEINLINE Sphere &Sphere::ExpandSelf( const FLOAT d ) {
	Radius += d;
	return *this;
}

FORCEINLINE Sphere Sphere::Translate( const Vec3D &translation ) const {
	return Sphere( Center + translation, Radius );
}

FORCEINLINE Sphere &Sphere::TranslateSelf( const Vec3D &translation ) {
	Center += translation;
	return *this;
}

FORCEINLINE Sphere Sphere::Transform( const Vec3D& translation, const Quat& rotation, const FLOAT scale ) const
{
	return Sphere(
		( Center * scale ) * rotation + translation,
		Radius * scale
	);
}

FORCEINLINE Sphere &Sphere::TransformSelf( const Vec3D& translation, const Quat& rotation, const FLOAT scale )
{
	*this = this->Transform( translation, rotation, scale );
}

FORCEINLINE bool Sphere::ContainsPoint( const Vec3D &p ) const {
	if ( ( p - Center ).LengthSqr() > Radius * Radius ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Sphere::ContainsSphere( const Sphere& s ) const
{
	// find distance between centers
	const F4 dist = ( s.Center - Center ).GetLength();

	return (dist + s.Radius) < this->Radius;
}

FORCEINLINE bool Sphere::IntersectsSphere( const Sphere &s ) const {
	const FLOAT r = this->Radius + s.Radius;
	if ( ( s.Center - Center ).LengthSqr() > r * r ) {
		return false;
	}
	return true;
}

FORCEINLINE void Sphere::FromPointTranslation( const Vec3D &point, const Vec3D &translation ) {
	Center = point + 0.5f * translation;
	Radius = mxSqrt( 0.5f * translation.LengthSqr() );
}

FORCEINLINE void Sphere::FromSphereTranslation( const Sphere &sphere, const Vec3D &start, const Vec3D &translation ) {
	Center = start + sphere.Center + 0.5f * translation;
	Radius = mxSqrt( 0.5f * translation.LengthSqr() ) + sphere.Radius;
}

FORCEINLINE void Sphere::FromPointRotation( const Vec3D &point, const Rotation &rotation ) {
	Vec3D end = rotation * point;
	Center = ( point + end ) * 0.5f;
	Radius = mxSqrt( 0.5f * ( end - point ).LengthSqr() );
}

FORCEINLINE void Sphere::FromSphereRotation( const Sphere &sphere, const Vec3D &start, const Rotation &rotation ) {
	Vec3D end = rotation * sphere.Center;
	Center = start + ( sphere.Center + end ) * 0.5f;
	Radius = mxSqrt( 0.5f * ( end - sphere.Center ).LengthSqr() ) + sphere.Radius;
}

FORCEINLINE void Sphere::AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const {
	FLOAT d;
	d = dir * Center;
	min = d - Radius;
	max = d + Radius;
}

FORCEINLINE bool Sphere::IsValid() const {
	return (! FLOAT_IS_INVALID( Radius )) && ( Center.IsValid() );
}

mxNAMESPACE_END

#endif // ! __BOUNDING_VOLUMES_SPHERE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
