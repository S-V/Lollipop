/*
=============================================================================
	File:	OOBB.h
	Desc:	Object Oriented Bounding Box.
=============================================================================
*/

#ifndef __BOUNDING_VOLUMES_OBB_H__
#define __BOUNDING_VOLUMES_OBB_H__
mxSWIPED("Id Software");
mxNAMESPACE_BEGIN

//
//	OOBB - Oriented bounding box.
//

class OOBB {
public:
					OOBB( void );
					explicit OOBB( const Vec3D &center, const Vec3D &extents, const Matrix3 &axis );
					explicit OOBB( const Vec3D &point );
					explicit OOBB( const AABB &bounds );
					explicit OOBB( const AABB &bounds, const Vec3D &origin, const Matrix3 &axis );

	OOBB			operator+( const Vec3D &t ) const;				// returns translated box
	OOBB &			operator+=( const Vec3D &t );					// translate the box
	OOBB			operator*( const Matrix3 &r ) const;				// returns rotated box
	OOBB &			operator*=( const Matrix3 &r );					// rotate the box
	OOBB			operator+( const OOBB &a ) const;
	OOBB &			operator+=( const OOBB &a );
	OOBB			operator-( const OOBB &a ) const;
	OOBB &			operator-=( const OOBB &a );

	bool			Compare( const OOBB &a ) const;						// exact compare, no epsilon
	bool			Compare( const OOBB &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const OOBB &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const OOBB &a ) const;						// exact compare, no epsilon

	void			Clear( void );									// inside out box
	void			SetZero( void );								// single point at origin

	const Vec3D &	GetCenter( void ) const;						// returns center of the box
	const Vec3D &	GetExtents( void ) const;						// returns extents of the box
	const Matrix3 &	GetAxis( void ) const;							// returns the axis of the box
	FLOAT			GetVolume( void ) const;						// returns the volume of the box
	bool			IsCleared( void ) const;						// returns true if box are inside out

	bool			AddPoint( const Vec3D &v );					// add the point, returns true if the box expanded
	bool			AddBox( const OOBB &a );						// add the box, returns true if the box expanded
	OOBB			Expand( const FLOAT d ) const;					// return box expanded in all directions with the given value
	OOBB &			ExpandSelf( const FLOAT d );					// expand box in all directions with the given value
	OOBB			Translate( const Vec3D &translation ) const;	// return translated box
	OOBB &			TranslateSelf( const Vec3D &translation );		// translate this box
	OOBB			Rotate( const Matrix3 &rotation ) const;			// return rotated box
	OOBB &			RotateSelf( const Matrix3 &rotation );			// rotate this box

	FLOAT			PlaneDistance( const Plane3D &plane ) const;
	EPlaneSide		PlaneSide( const Plane3D &plane, const FLOAT epsilon = ON_EPSILON ) const;

	bool			ContainsPoint( const Vec3D &p ) const;			// includes touching
	bool			IntersectsBox( const OOBB &a ) const;			// includes touching
	bool			LineIntersection( const Vec3D &start, const Vec3D &end ) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale1, FLOAT &scale2 ) const;

					// tight box for a collection of points
	void			FromPoints( const Vec3D *points, const int numPoints );
					// most tight box for a translation
	void			FromPointTranslation( const Vec3D &point, const Vec3D &translation );
	void			FromBoxTranslation( const OOBB &box, const Vec3D &translation );
					// most tight box for a rotation
	void			FromPointRotation( const Vec3D &point, const Rotation &rotation );
	void			FromBoxRotation( const OOBB &box, const Rotation &rotation );

	enum { NUM_CORNERS = 8 };

	void			ToPoints( Vec3D points[NUM_CORNERS] ) const;
	Sphere			ToSphere( void ) const;

					// calculates the projection of this box onto the given axis
	void			AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const;
	void			AxisProjection( const Matrix3 &ax, AABB &bounds ) const;

					// calculates the silhouette of the box
	int				GetProjectionSilhouetteVerts( const Vec3D &projectionOrigin, Vec3D silVerts[6] ) const;
	int				GetParallelProjectionSilhouetteVerts( const Vec3D &projectionDir, Vec3D silVerts[6] ) const;

public:
	static const OOBB	box_zero;

public:
	Vec3D		center;
	Vec3D		extents;	// half size
	Matrix3		axis;
};
mxDECLARE_POD_TYPE(OOBB);

FORCEINLINE OOBB::OOBB( void ) {
}

FORCEINLINE OOBB::OOBB( const Vec3D &center, const Vec3D &extents, const Matrix3 &axis ) {
	this->center = center;
	this->extents = extents;
	this->axis = axis;
}

FORCEINLINE OOBB::OOBB( const Vec3D &point ) {
	this->center = point;
	this->extents.SetZero();
	this->axis.SetIdentity();
}

FORCEINLINE OOBB::OOBB( const AABB &bounds ) {
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->axis.SetIdentity();
}

FORCEINLINE OOBB::OOBB( const AABB &bounds, const Vec3D &origin, const Matrix3 &axis ) {
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->center = origin + this->center * axis;
	this->axis = axis;
}

FORCEINLINE OOBB OOBB::operator+( const Vec3D &t ) const {
	return OOBB( center + t, extents, axis );
}

FORCEINLINE OOBB &OOBB::operator+=( const Vec3D &t ) {
	center += t;
	return *this;
}

FORCEINLINE OOBB OOBB::operator*( const Matrix3 &r ) const {
	return OOBB( center * r, extents, axis * r );
}

FORCEINLINE OOBB &OOBB::operator*=( const Matrix3 &r ) {
	center *= r;
	axis *= r;
	return *this;
}

FORCEINLINE OOBB OOBB::operator+( const OOBB &a ) const {
	OOBB newBox;
	newBox = *this;
	newBox.AddBox( a );
	return newBox;
}

FORCEINLINE OOBB &OOBB::operator+=( const OOBB &a ) {
	OOBB::AddBox( a );
	return *this;
}

FORCEINLINE OOBB OOBB::operator-( const OOBB &a ) const {
	return OOBB( center, extents - a.extents, axis );
}

FORCEINLINE OOBB &OOBB::operator-=( const OOBB &a ) {
	extents -= a.extents;
	return *this;
}

FORCEINLINE bool OOBB::Compare( const OOBB &a ) const {
	return ( center.Compare( a.center ) && extents.Compare( a.extents ) && axis.Compare( a.axis ) );
}

FORCEINLINE bool OOBB::Compare( const OOBB &a, const FLOAT epsilon ) const {
	return ( center.Compare( a.center, epsilon ) && extents.Compare( a.extents, epsilon ) && axis.Compare( a.axis, epsilon ) );
}

FORCEINLINE bool OOBB::operator==( const OOBB &a ) const {
	return Compare( a );
}

FORCEINLINE bool OOBB::operator!=( const OOBB &a ) const {
	return !Compare( a );
}

FORCEINLINE void OOBB::Clear( void ) {
	center.SetZero();
	extents[0] = extents[1] = extents[2] = -MX_INFINITY;
	axis.SetIdentity();
}

FORCEINLINE void OOBB::SetZero( void ) {
	center.SetZero();
	extents.SetZero();
	axis.SetIdentity();
}

FORCEINLINE const Vec3D &OOBB::GetCenter( void ) const {
	return center;
}

FORCEINLINE const Vec3D &OOBB::GetExtents( void ) const {
	return extents;
}

FORCEINLINE const Matrix3 &OOBB::GetAxis( void ) const {
	return axis;
}

FORCEINLINE FLOAT OOBB::GetVolume( void ) const {
	return ( extents * 2.0f ).LengthSqr();
}

FORCEINLINE bool OOBB::IsCleared( void ) const {
	return extents[0] < 0.0f;
}

FORCEINLINE OOBB OOBB::Expand( const FLOAT d ) const {
	return OOBB( center, extents + Vec3D( d, d, d ), axis );
}

FORCEINLINE OOBB &OOBB::ExpandSelf( const FLOAT d ) {
	extents[0] += d;
	extents[1] += d;
	extents[2] += d;
	return *this;
}

FORCEINLINE OOBB OOBB::Translate( const Vec3D &translation ) const {
	return OOBB( center + translation, extents, axis );
}

FORCEINLINE OOBB &OOBB::TranslateSelf( const Vec3D &translation ) {
	center += translation;
	return *this;
}

FORCEINLINE OOBB OOBB::Rotate( const Matrix3 &rotation ) const {
	return OOBB( center * rotation, extents, axis * rotation );
}

FORCEINLINE OOBB &OOBB::RotateSelf( const Matrix3 &rotation ) {
	center *= rotation;
	axis *= rotation;
	return *this;
}

FORCEINLINE bool OOBB::ContainsPoint( const Vec3D &p ) const {
	Vec3D lp = p - center;
	if ( mxFabs( lp * axis[0] ) > extents[0] ||
			mxFabs( lp * axis[1] ) > extents[1] ||
				mxFabs( lp * axis[2] ) > extents[2] ) {
		return false;
	}
	return true;
}

FORCEINLINE Sphere OOBB::ToSphere( void ) const {
	return Sphere( center, extents.GetLength() );
}

FORCEINLINE void OOBB::AxisProjection( const Vec3D &dir, FLOAT &min, FLOAT &max ) const {
	FLOAT d1 = dir * center;
	FLOAT d2 = mxFabs( extents[0] * ( dir * axis[0] ) ) +
				mxFabs( extents[1] * ( dir * axis[1] ) ) +
				mxFabs( extents[2] * ( dir * axis[2] ) );
	min = d1 - d2;
	max = d1 + d2;
}

FORCEINLINE void OOBB::AxisProjection( const Matrix3 &ax, AABB &bounds ) const {
	for ( int i = 0; i < 3; i++ ) {
		FLOAT d1 = ax[i] * center;
		FLOAT d2 = mxFabs( extents[0] * ( ax[i] * axis[0] ) ) +
					mxFabs( extents[1] * ( ax[i] * axis[1] ) ) +
					mxFabs( extents[2] * ( ax[i] * axis[2] ) );
		bounds[0][i] = d1 - d2;
		bounds[1][i] = d1 + d2;
	}
}

mxNAMESPACE_END

#endif // ! __BOUNDING_VOLUMES_OBB_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
