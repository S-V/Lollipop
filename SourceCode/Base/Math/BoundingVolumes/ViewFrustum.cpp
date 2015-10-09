/*
=============================================================================
	File:	ViewFrustum.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*================================
		ViewFrustum
================================*/

ViewFrustum::ViewFrustum()
{}

ViewFrustum::ViewFrustum( const Matrix4& mat )
{
	this->ExtractFrustumPlanes( mat );
}

//
//	ViewFrustum::PointInFrustum
//
//	NOTE: This test assumes frustum planes face inward.
//
FASTBOOL ViewFrustum::PointInFrustum( const Vec3D& point ) const
{
#if 0 
	for( UINT iPlane = 0; iPlane < VF_NUM_PLANES; ++iPlane )
	{
		if( planes[ iPlane ].Distance( point ) < 0.0f )
		{
			return FALSE;
		}
	}
    return TRUE;
#else
	// assume the point is inside by default
	FASTBOOL	mask = 1;

	for( UINT iPlane = 0; (iPlane < VF_NUM_PLANES) && mask; ++iPlane )
	{
		mask &= ( planes[ iPlane ].Distance( point ) >= 0.0f );
	}
    return mask;
#endif
}

//
//	ViewFrustum::IntersectSphere
//
//	NOTE: This test assumes frustum planes face inward.
//
FASTBOOL ViewFrustum::IntersectSphere( const Sphere& sphere ) const
{
	//DX_CYCLE_COUNTER( gStats.cull_sphere_cycles );

#if 0 
	for( UINT iPlane = 0; iPlane < VF_NUM_PLANES; ++iPlane )
	{
		if( planes[ iPlane ].Distance( sphere.GetOrigin() ) < -sphere.GetRadius() )
		{
			return FALSE;
		}
	}
    return TRUE;
#else
	// assume the sphere is inside by default
	FASTBOOL	mask = 1;

	for( UINT iPlane = 0; (iPlane < VF_NUM_PLANES) && mask; ++iPlane )
	{
		// test whether the sphere is on the positive half space of each frustum plane.  
		// If it is on the negative half space of one plane we can reject it.
		mask &= ( planes[ iPlane ].Distance( sphere.GetOrigin() ) + sphere.GetRadius() > 0.0f );
	}
    return mask;
#endif
}

//
//	ViewFrustum::IntersectsAABB
//
//	NOTE: This test assumes frustum planes face inward.
//
FASTBOOL ViewFrustum::IntersectsAABB( const AABB& aabb ) const
{
	//DX_CYCLE_COUNTER( gStats.cull_aabb_cycles );

	mxSTATIC_ASSERT( (int)ESpatialRelation::Outside == 0 );
#if 1
		return Classify(aabb);// != ESpatialRelation::Outside;
#else
	{
		// Test each corner of aabb against each frustum plane;
		// if all corners are outside the frustum plane then the box doesn't intersect the frustum.
		// BUG: it doesn't work if a (small) frustum is inside a (large) box.

		Vec3D min( aabb.GetMin() );
		Vec3D max( aabb.GetMax() );
		for( UINT iPlane = 0; iPlane < VF_NUM_PLANES; ++iPlane )
		{
			if( planes[ iPlane ].Distance(Vec3D( min[0],min[1],min[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( min[0],min[1],max[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( min[0],max[1],min[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( min[0],max[1],max[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( max[0],min[1],min[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( max[0],min[1],max[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( max[0],max[1],min[2] )) > 0.f ) continue;
			if( planes[ iPlane ].Distance(Vec3D( max[0],max[1],max[2] )) > 0.f ) continue;
			return FALSE;
		}
		return TRUE;
	}
#endif
}

//
//	ViewFrustum::Classify
//
//	See: http://www.flipcode.com/articles/article_frustumculling.shtml
//
//	NOTE: This test assumes frustum planes face inward.
//
int ViewFrustum::Classify( const AABB& aabb ) const
{
	FASTBOOL bIntersect = 0;

	for( UINT iPlane = 0; iPlane < VF_NUM_PLANES; ++iPlane )
	{
		const UINT nV = signs[ iPlane ];
		
		// pVertex is diagonally opposed to nVertex
		const Vec3D nVertex( ( nV & 1 ) ? aabb.mPoints[0].x : aabb.mPoints[1].x, ( nV & 2 ) ? aabb.mPoints[0].y : aabb.mPoints[1].y, ( nV & 4 ) ? aabb.mPoints[0].z : aabb.mPoints[1].z );
		const Vec3D pVertex( ( nV & 1 ) ? aabb.mPoints[1].x : aabb.mPoints[0].x, ( nV & 2 ) ? aabb.mPoints[1].y : aabb.mPoints[0].y, ( nV & 4 ) ? aabb.mPoints[1].z : aabb.mPoints[0].z );

		if ( planes[ iPlane ].Distance( nVertex ) < 0.0f ) {
			return ESpatialRelation::Outside;
		}
		if ( planes[ iPlane ].Distance( pVertex ) < 0.0f ) {
			bIntersect = 1;
		}
	}

	//return ( bIntersect ? ESpatialRelation::Intersects : ESpatialRelation::Inside );
	return ESpatialRelation( bIntersect + ESpatialRelation::Intersects );

	{mxSTATIC_ASSERT( (int)ESpatialRelation::Outside == 0 );}
	{mxSTATIC_ASSERT( (int)ESpatialRelation::Intersects == 1 );}
	{mxSTATIC_ASSERT( (int)ESpatialRelation::Inside == 2 );}
}

//
//	ViewFrustum::ExtractFrustumPlanes - extracts from a matrix the planes that make up the frustum.
//
// 
// See: http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf

// Summary of method:

/*	If we take a vector v and a projection matrix M, the transformed point v' is = vM.  
A transformed point in clip space will be inside the frustum (AABB) if -w' < x' < w', -w' < y' < w' and 0 < z' < w'

We can deduce that it is on the positive side of the left plane's half space if x' > -w'
Since w' = v.col4 and x' = v.col1, we can rewrite this as v.col1 > -v.col4
thus v.col1 + v.col4 > 0   ... and ... v.(col1 + col4) > 0

This is just another way of writing the following: x( m14 + m11 ) + y( m24 + m21 ) + z( m34 + m31 ) + w( m44 + m41 )
For the untransformed point w = 1, so we can simplify to x( m14 + m11 ) + y( m24 + m21 ) + z( m34 + m31 ) + m44 + m41
Which is the same as the standard plane equation ax + by + cz + d = 0
Where a = m14 + m11, b = m24 + m21, c = m34 + m31 and d = m44 + m41

Running this algorithm on a projection matrix yields the clip planes in view space
Running it on a view projection matrix yields the clip planes in world space
.... for world view projection it yields the clip planes in object/local space

dxWorld space is usually ideal, so a view projection matrix is usually passed in.
*/

void ViewFrustum::ExtractFrustumPlanes( const Matrix4& mat )
{
	Vec4D  col0( mat[0][0], mat[1][0], mat[2][0], mat[3][0] );
	Vec4D  col1( mat[0][1], mat[1][1], mat[2][1], mat[3][1] );
	Vec4D  col2( mat[0][2], mat[1][2], mat[2][2], mat[3][2] );
	Vec4D  col3( mat[0][3], mat[1][3], mat[2][3], mat[3][3] );

	// Planes' normals face inward.

	planes[ VF_LEFT_PLANE ]		.Set( col3 + col0 );	// -w' < x'		-> -(v.col4) < v.col -> 0 < (v.col4) + (v.col1) -> 0 < v.(col1 + col4)
	planes[ VF_RIGHT_PLANE ]	.Set( col3 - col0 );	// x' < w'		-> 0 < v.(col4 - col1)

	planes[ VF_BOTTOM_PLANE ]	.Set( col3 + col1 );	// y' < w'		-> 0 < v.(col4 - col2)
	planes[ VF_TOP_PLANE	]	.Set( col3 - col1 );	// -w' < y'		-> 0 < v.(col4 + col2)

	planes[ VF_NEAR_PLANE ]		.Set( col2 );			// 0 < z'		-> 0 < v.col3
	planes[ VF_FAR_PLANE ]		.Set( col3 - col2 );	// z' < w'		-> 0 < v.(col4 - col3)

	for( UINT iPlane = 0; iPlane < VF_NUM_PLANES; iPlane++ )
	{
		FLOAT invLength = 1.0f / planes[ iPlane ].Normal().GetLength();
		planes[ iPlane ].Normal() *= invLength;
		planes[ iPlane ].d *= invLength;

		signs[ iPlane ] = (( planes[ iPlane ].a < 0.f ) ? 1 : 0 )
						| ((planes[ iPlane ].b < 0.f ) ? 2 : 0 )
						| ((planes[ iPlane ].c < 0.f ) ? 4 : 0 );
	}
}

bool ViewFrustum::GetFarLeftDown( Vec3D & point ) const
{
	return PlanesIntersection(
		planes[ VF_FAR_PLANE ],
		planes[ VF_LEFT_PLANE ],
		planes[ VF_BOTTOM_PLANE ],
		point
	);
}

bool ViewFrustum::GetFarLeftUp( Vec3D & point ) const
{
	return PlanesIntersection(
		planes[ VF_FAR_PLANE ],
		planes[ VF_LEFT_PLANE ],
		planes[ VF_TOP_PLANE ],
		point
	);
}

bool ViewFrustum::GetFarRightUp( Vec3D & point ) const
{
	return PlanesIntersection(
		planes[ VF_FAR_PLANE ],
		planes[ VF_RIGHT_PLANE ],
		planes[ VF_TOP_PLANE ],
		point
	);
}

bool ViewFrustum::GetFarRightDown( Vec3D & point ) const
{
	return PlanesIntersection(
		planes[ VF_FAR_PLANE ],
		planes[ VF_RIGHT_PLANE ],
		planes[ VF_BOTTOM_PLANE ],
		point
	);
}

//
//	ViewFrustum::CalculateCornerPoints - Computes positions of the 8 vertices of the frustum.
//
void ViewFrustum::CalculateCornerPoints( Vec3D corners[8] ) const
{
	for( UINT iPoint = 0; iPoint < 8; iPoint++ )  
	{
		const Plane3D & p0 = (iPoint & 1) ? planes[VF_NEAR_PLANE]	: planes[VF_FAR_PLANE];		// alternate between near and far every iteration
		const Plane3D & p1 = (iPoint & 2) ? planes[VF_BOTTOM_PLANE]	: planes[VF_TOP_PLANE];		// alternate between bottom and top every iteration
		const Plane3D & p2 = (iPoint & 4) ? planes[VF_LEFT_PLANE]	: planes[VF_RIGHT_PLANE];	// alternate between left and right every iteration

		// For each three planes tested, there will be one common intersection point.  
		// These points make up the 8 vertices of the view frustum.
		const mxBool bOk = PlanesIntersection( p0, p1, p2, corners[iPoint] );
		Assert(bOk);
		(void)bOk;
	}
}

//
//	ViewFrustum::CullPoints - Returns 1 if all points are outside the frustum.
//
FASTBOOL ViewFrustum::CullPoints( const Vec3D* points, UINT numPoints ) const
{
	mxOPTIMIZE(:);
	for( UINT iPlane = 0; iPlane < VF_CLIP_PLANES; iPlane++ )
	{
		FASTBOOL bAllOut = 1;

		for( UINT iPoint = 0; iPoint < numPoints; iPoint++ )  
		{
			if( planes[ iPlane ].Distance( points[iPoint] ) > 0.0f )
			{
				bAllOut = 0;
				break;
			}
		}

		if( bAllOut ) {
			return TRUE;
		}
	}
	return FALSE;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
