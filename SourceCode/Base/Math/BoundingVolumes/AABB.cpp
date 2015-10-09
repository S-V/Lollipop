/*
=============================================================================
	File:	AABB.cpp
	Desc:	Axis aligned bounding box.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*================================
			AABB
================================*/

const AABB AABB::zero_aabb		( Vec3D::vec3_zero, Vec3D::vec3_zero );
const AABB AABB::infinite_aabb	( Vec3D::vec3_minus_inf, Vec3D::vec3_plus_inf );
const AABB AABB::invalid_aabb	( Vec3D::vec3_plus_inf, Vec3D::vec3_minus_inf );

/*
============
AABB::GetRadius
============
*/
FLOAT AABB::GetRadius( void ) const {
	int		i;
	FLOAT	total, b0, b1;

	total = 0.0f;
	for ( i = 0; i < 3; i++ ) {
		b0 = (FLOAT)mxFabs( mPoints[0][i] );
		b1 = (FLOAT)mxFabs( mPoints[1][i] );
		if ( b0 > b1 ) {
			total += b0 * b0;
		} else {
			total += b1 * b1;
		}
	}
	return mxSqrt( total );
}

/*
============
AABB::GetRadius
============
*/
FLOAT AABB::GetRadius( const Vec3D &center ) const {
	int		i;
	FLOAT	total, b0, b1;

	total = 0.0f;
	for ( i = 0; i < 3; i++ ) {
		b0 = (FLOAT)mxFabs( center[i] - mPoints[0][i] );
		b1 = (FLOAT)mxFabs( mPoints[1][i] - center[i] );
		if ( b0 > b1 ) {
			total += b0 * b0;
		} else {
			total += b1 * b1;
		}
	}
	return mxSqrt( total );
}

/*
================
AABB::PlaneDistance
================
*/
FLOAT AABB::PlaneDistance( const Plane3D &plane ) const
{
	Vec3D center;
	FLOAT d1, d2;

	center = ( mPoints[0] + mPoints[1] ) * 0.5f;

	d1 = plane.Distance( center );
	d2 = mxFabs( ( mPoints[1][0] - center[0] ) * plane.Normal()[0] ) +
			mxFabs( ( mPoints[1][1] - center[1] ) * plane.Normal()[1] ) +
				mxFabs( ( mPoints[1][2] - center[2] ) * plane.Normal()[2] );

	if ( d1 - d2 > 0.0f ) {
		return d1 - d2;
	}
	if ( d1 + d2 < 0.0f ) {
		return d1 + d2;
	}
	return 0.0f;
}

/*
================
AABB::ShortestDistanceSquared
================
*/
FLOAT AABB::ShortestDistanceSquared( const Vec3D &point ) const
{
	int		i;
	FLOAT	delta, distance;

	if( this->ContainsPoint( point ) ) {

		return( 0.0f );
	}

	distance = 0.0f;
	for( i = 0; i < 3; i++ ) {

		if( point[i] < mPoints[0][i] ) {

			delta = mPoints[0][i] - point[i];
			distance += delta * delta;

		} else if ( point[i] > mPoints[1][i] ) {

			delta = point[i] - mPoints[1][i];
			distance += delta * delta;
		}
	}

	return distance;
}

/*
================
AABB::ShortestDistance
================
*/
FLOAT AABB::ShortestDistance( const Vec3D &point ) const {
	return mxSqrt( this->ShortestDistanceSquared( point ) );
}

Vec3D AABB::FindEdgePoint( const Vec3D& dir ) const {
	return this->FindEdgePoint( GetCenter(), dir );
}

Vec3D AABB::FindEdgePoint( const Vec3D& start, const Vec3D& dir ) const {
	Vec3D center( GetCenter() );
	FLOAT radius = GetRadius( center );
	Vec3D point( start + dir * radius );
	FLOAT scale = 0.0f;

    this->RayIntersection( point, -dir, scale );
	
	Vec3D p = point + -dir * scale;
	return p;
}

Vec3D AABB::FindVectorToEdge( const Vec3D& dir ) const {
	return Vec3D( FindVectorToEdge(GetCenter(), dir) );
}

Vec3D AABB::FindVectorToEdge( const Vec3D& start, const Vec3D& dir ) const {
	return Vec3D( FindEdgePoint(start, dir) - start );
}

/*
================
AABB::PlaneSide
================
*/
EPlaneSide AABB::PlaneSide( const Plane3D &plane, const FLOAT epsilon ) const
{
	Vec3D center;
	FLOAT d1, d2;

	center = ( mPoints[0] + mPoints[1] ) * 0.5f;

	d1 = plane.Distance( center );
	d2 = mxFabs( ( mPoints[1][0] - center[0] ) * plane.Normal()[0] ) +
			mxFabs( ( mPoints[1][1] - center[1] ) * plane.Normal()[1] ) +
				mxFabs( ( mPoints[1][2] - center[2] ) * plane.Normal()[2] );

	if ( d1 - d2 > epsilon ) {
		return EPlaneSide::PLANESIDE_FRONT;
	}
	if ( d1 + d2 < -epsilon ) {
		return EPlaneSide::PLANESIDE_BACK;
	}
	return EPlaneSide::PLANESIDE_CROSS;
}

/*
============
AABB::LineIntersection

  Returns true if the line intersects the bounds between the start and end point.
============
*/
bool AABB::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
    FLOAT ld[3];
	Vec3D center = ( mPoints[0] + mPoints[1] ) * 0.5f;
	Vec3D extents = mPoints[1] - center;
    Vec3D lineDir = 0.5f * ( end - start );
    Vec3D lineCenter = start + lineDir;
    Vec3D dir = lineCenter - center;

    ld[0] = mxFabs( lineDir[0] );
	if ( mxFabs( dir[0] ) > extents[0] + ld[0] ) {
        return false;
	}

    ld[1] = mxFabs( lineDir[1] );
	if ( mxFabs( dir[1] ) > extents[1] + ld[1] ) {
        return false;
	}

    ld[2] = mxFabs( lineDir[2] );
	if ( mxFabs( dir[2] ) > extents[2] + ld[2] ) {
        return false;
	}

    Vec3D cross = lineDir.Cross( dir );

	if ( mxFabs( cross[0] ) > extents[1] * ld[2] + extents[2] * ld[1] ) {
        return false;
	}

	if ( mxFabs( cross[1] ) > extents[0] * ld[2] + extents[2] * ld[0] ) {
        return false;
	}

	if ( mxFabs( cross[2] ) > extents[0] * ld[1] + extents[1] * ld[0] ) {
        return false;
	}

    return true;
}

/*
============
AABB::RayIntersection

  Returns true if the ray intersects the bounds.
  The ray can intersect the bounds in both directions from the start point.
  If start is inside the bounds it is considered an intersection with scale = 0
============
*/
bool AABB::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale ) const {
	int i, ax0, ax1, ax2, side, inside;
	float f;
	Vec3D hit;

	ax0 = -1;
	inside = 0;
	for ( i = 0; i < 3; i++ ) {
		if ( start[i] < mPoints[0][i] ) {
			side = 0;
		}
		else if ( start[i] > mPoints[1][i] ) {
			side = 1;
		}
		else {
			inside++;
			continue;
		}
		if ( dir[i] == 0.0f ) {
			continue;
		}
		f = ( start[i] - mPoints[side][i] );
		if ( ax0 < 0 || mxFabs( f ) > mxFabs( scale * dir[i] ) ) {
			scale = - ( f / dir[i] );
			ax0 = i;
		}
	}

	if ( ax0 < 0 ) {
		scale = 0.0f;
		// return true if the start point is inside the bounds
		return ( inside == 3 );
	}

	ax1 = (ax0+1)%3;
	ax2 = (ax0+2)%3;
	hit[ax1] = start[ax1] + scale * dir[ax1];
	hit[ax2] = start[ax2] + scale * dir[ax2];

	return ( hit[ax1] >= mPoints[0][ax1] && hit[ax1] <= mPoints[1][ax1] &&
				hit[ax2] >= mPoints[0][ax2] && hit[ax2] <= mPoints[1][ax2] );
}

/*
============
AABB::FromTransformedBounds
============
*/
void AABB::FromTransformedBounds( const AABB &bounds, const Vec3D &origin, const Matrix3 &axis ) {
	int i;
	Vec3D center, extents, rotatedExtents;

	center = (bounds[0] + bounds[1]) * 0.5f;
	extents = bounds[1] - center;

	for ( i = 0; i < 3; i++ ) {
		rotatedExtents[i] = mxFabs( extents[0] * axis[0][i] ) +
							mxFabs( extents[1] * axis[1][i] ) +
							mxFabs( extents[2] * axis[2][i] );
	}

	center = origin + center * axis;
	mPoints[0] = center - rotatedExtents;
	mPoints[1] = center + rotatedExtents;
}

/*
============
AABB::FromPoints

  Most tight bounds for a point set.
============
*/

void AABB::FromPoints( const Vec3D *points, const UINT numPoints ) {
#if 0
	SIMDProcessor->MinMax( mPoints[0], mPoints[1], points, numPoints );
#else
	this->Clear();
	for( UINT iPoint = 0; iPoint < numPoints; iPoint++ )
	{
		this->AddPoint( points[ iPoint ] );
	}
#endif
}

/*
============
AABB::FromPointTranslation

  Most tight bounds for the translational movement of the given point.
============
*/
void AABB::FromPointTranslation( const Vec3D &point, const Vec3D &translation ) {
	int i;

	for ( i = 0; i < 3; i++ ) {
		if ( translation[i] < 0.0f ) {
			mPoints[0][i] = point[i] + translation[i];
			mPoints[1][i] = point[i];
		}
		else {
			mPoints[0][i] = point[i];
			mPoints[1][i] = point[i] + translation[i];
		}
	}
}

/*
============
AABB::FromBoundsTranslation

  Most tight bounds for the translational movement of the given bounds.
============
*/
void AABB::FromBoundsTranslation( const AABB &bounds, const Vec3D &origin, const Matrix3 &axis, const Vec3D &translation ) {
	int i;

	if ( axis.IsRotated() ) {
		this->FromTransformedBounds( bounds, origin, axis );
	}
	else {
		mPoints[0] = bounds[0] + origin;
		mPoints[1] = bounds[1] + origin;
	}
	for ( i = 0; i < 3; i++ ) {
		if ( translation[i] < 0.0f ) {
			mPoints[0][i] += translation[i];
		}
		else {
			mPoints[1][i] += translation[i];
		}
	}
}

/*
================
BoundsForPointRotation

  only for rotations < 180 degrees
================
*/
static AABB BoundsForPointRotation( const Vec3D &start, const Rotation &rotation )
{
	int i;
	FLOAT radiusSqr;
	Vec3D v1, v2;
	Vec3D origin, axis, end;
	AABB bounds;

	end = start * rotation;
	axis = rotation.GetVec();
	origin = rotation.GetOrigin() + axis * ( axis * ( start - rotation.GetOrigin() ) );
	radiusSqr = ( start - origin ).LengthSqr();
	v1 = ( start - origin ).Cross( axis );
	v2 = ( end - origin ).Cross( axis );

	for ( i = 0; i < 3; i++ ) {
		// if the derivative changes sign along this axis during the rotation from start to end
		if ( ( v1[i] > 0.0f && v2[i] < 0.0f ) || ( v1[i] < 0.0f && v2[i] > 0.0f ) ) {
			if ( ( 0.5f * (start[i] + end[i]) - origin[i] ) > 0.0f ) {
				bounds[0][i] = Min( start[i], end[i] );
				bounds[1][i] = origin[i];
				if( axis[i] * axis[i] < 1.0f ) {
					bounds[1][i] += mxSqrt( radiusSqr * ( 1.0f - axis[i] * axis[i] ) );
				}
			}
			else {
				bounds[0][i] = origin[i];
				if( axis[i] * axis[i] < 1.0f ) {
					bounds[0][i] -= mxSqrt( radiusSqr * ( 1.0f - axis[i] * axis[i] ) );
				}
				bounds[1][i] = Max( start[i], end[i] );
			}
		}
		else if ( start[i] > end[i] ) {
			bounds[0][i] = end[i];
			bounds[1][i] = start[i];
		}
		else {
			bounds[0][i] = start[i];
			bounds[1][i] = end[i];
		}
	}

	return bounds;
}

/*
============
AABB::FromPointRotation

  Most tight bounds for the rotational movement of the given point.
============
*/
void AABB::FromPointRotation( const Vec3D &point, const Rotation &rotation ) {
	FLOAT radius;

	if ( mxFabs( rotation.GetAngle() ) < 180.0f ) {
		(*this) = BoundsForPointRotation( point, rotation );
	}
	else {

		radius = ( point - rotation.GetOrigin() ).GetLength();

		// FIXME: these bounds are usually way larger
		mPoints[0].Set( -radius, -radius, -radius );
		mPoints[1].Set( radius, radius, radius );
	}
}

/*
============
AABB::FromBoundsRotation

  Most tight bounds for the rotational movement of the given bounds.
============
*/
void AABB::FromBoundsRotation( const AABB &bounds, const Vec3D &origin, const Matrix3 &axis, const Rotation &rotation ) {
	UINT i;
	FLOAT radius;
	Vec3D point;
	AABB rBounds;

	if ( mxFabs( rotation.GetAngle() ) < 180.0f ) {

		(*this) = BoundsForPointRotation( bounds[0] * axis + origin, rotation );
		for ( i = 1; i < 8; i++ ) {
			point[0] = bounds[(i^(i>>1))&1][0];
			point[1] = bounds[(i>>1)&1][1];
			point[2] = bounds[(i>>2)&1][2];
			(*this) += BoundsForPointRotation( point * axis + origin, rotation );
		}
	}
	else {

		point = (bounds[1] - bounds[0]) * 0.5f;
		radius = (bounds[1] - point).GetLength() + (point - rotation.GetOrigin()).GetLength();

		// FIXME: these bounds are usually way larger
		mPoints[0].Set( -radius, -radius, -radius );
		mPoints[1].Set( radius, radius, radius );
	}
}

/*
============
AABB::GetCorners
============
*/
void AABB::GetCorners( Vec3D points[8] ) const
{
/*

   Y
   |  /Z
   | /
   |/_____X
 (0,0,0)


        7___________6
       /|           /
      / |          /|
     /  |         / |
    3------------2  |
    |   4________|__5
    |   /        |  /
    |  /         | /
    | /          |/
    0/___________1


	0 = ---
	1 = +--
	2 = ++-
	3 = -+-
	4 = --+
	5 = +-+
	6 = +++
	7 = -++

*/

	for( UINT i = 0; i < 8; i++ )
	{
		points[i][0] = mPoints[(i^(i>>1))&1][0];
		points[i][1] = mPoints[(i>>1)&1][1];
		points[i][2] = mPoints[(i>>2)&1][2];
	}
}

/*
============
AABB::GetEdges
============
*/
const UINT* AABB::GetEdges()
{
	static const UINT indices[ 12*2 ] = 
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		7, 6,
		6, 5,
		5, 4,
		4, 7,
		
		1, 5,
		6, 2,
		3, 7,
		4, 0
	};
	return indices;
}

/*
============
AABB::GetVertexNormals
============
*/
const Vec3D* AABB::GetUnitCubeVertexNormals()
{
	mxPERM("scale normals properly, using minPoint and maxPoint")
	static const Vec3D vertexNormals[ 8 ] = 
	{
		Vec3D( -MX_INV_SQRT_3,	-MX_INV_SQRT_3,	-MX_INV_SQRT_3	),
		Vec3D( MX_INV_SQRT_3,	-MX_INV_SQRT_3,	-MX_INV_SQRT_3	),
		Vec3D( MX_INV_SQRT_3,	MX_INV_SQRT_3,	-MX_INV_SQRT_3	),
		Vec3D( -MX_INV_SQRT_3,	MX_INV_SQRT_3,	-MX_INV_SQRT_3	),
		
		Vec3D( -MX_INV_SQRT_3,	-MX_INV_SQRT_3,	MX_INV_SQRT_3	),
		Vec3D( MX_INV_SQRT_3,	-MX_INV_SQRT_3,	MX_INV_SQRT_3	),
		Vec3D( MX_INV_SQRT_3,	MX_INV_SQRT_3,	MX_INV_SQRT_3	),
		Vec3D( -MX_INV_SQRT_3,	MX_INV_SQRT_3,	MX_INV_SQRT_3	)
	};
	return vertexNormals;
}

/*
============
AABB::GetPlanes
============
*/
void AABB::GetPlanes( Plane3D planes[NUM_SIDES] ) const
{
	Vec3D  corners[ NUM_CORNERS ];
	GetCorners( corners );

	planes[ SIDE_ABOVE ]	.FromPoints( corners[3], corners[2], corners[6] );
	planes[ SIDE_BENEATH ]	.FromPoints( corners[1], corners[0], corners[4] );

	planes[ SIDE_IN_FRONT ]	.FromPoints( corners[5], corners[4], corners[7] );
	planes[ SIDE_BEHIND ]	.FromPoints( corners[0], corners[1], corners[2] );

	planes[ SIDE_LEFT ]		.FromPoints( corners[0], corners[3], corners[7] );
	planes[ SIDE_RIGHT ]	.FromPoints( corners[1], corners[5], corners[6] );

	for ( UINT iSide = 0; iSide < NUM_SIDES; iSide++ )
	{
		planes[ iSide ].Normalize();
	}

/*
        7___________6
       /|           /
      / |          /|
     /  |         / |
    3------------2  |
    |   4________|__5
    |   /        |  /
    |  /         | /
    | /          |/
    0/___________1
*/
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
