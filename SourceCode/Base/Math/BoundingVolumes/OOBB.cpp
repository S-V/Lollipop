/*
=============================================================================
	File:	OOBB.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

/*================================
			OOBB
================================*/

const OOBB OOBB::box_zero( Vec3D::vec3_zero, Vec3D::vec3_zero, Matrix3::mat3_identity );

/*
            4---{4}---5
 +         /|        /|
 Z      {7} {8}   {5} |
 -     /    |    /    {9}
      7--{6}----6     |
      |     |   |     |
    {11}    0---|-{0}-1
      |    /    |    /       -
      | {3}  {10} {1}       Y
      |/        |/         +
      3---{2}---2

	    - X +

  plane bits:
  0 = min x
  1 = max x
  2 = min y
  3 = max y
  4 = min z
  5 = max z

*/

local_ int boxVertPlanes[8] = {
	( (1<<0) | (1<<2) | (1<<4) ),
	( (1<<1) | (1<<2) | (1<<4) ),
	( (1<<1) | (1<<3) | (1<<4) ),
	( (1<<0) | (1<<3) | (1<<4) ),
	( (1<<0) | (1<<2) | (1<<5) ),
	( (1<<1) | (1<<2) | (1<<5) ),
	( (1<<1) | (1<<3) | (1<<5) ),
	( (1<<0) | (1<<3) | (1<<5) )
};

local_ int boxVertEdges[8][3] = {
	// bottom
	{ 3, 0, 8 },
	{ 0, 1, 9 },
	{ 1, 2, 10 },
	{ 2, 3, 11 },
	// top
	{ 7, 4, 8 },
	{ 4, 5, 9 },
	{ 5, 6, 10 },
	{ 6, 7, 11 }
};

local_ int boxEdgePlanes[12][2] = {
	// bottom
	{ 4, 2 },
	{ 4, 1 },
	{ 4, 3 },
	{ 4, 0 },
	// top
	{ 5, 2 },
	{ 5, 1 },
	{ 5, 3 },
	{ 5, 0 },
	// sides
	{ 0, 2 },
	{ 2, 1 },
	{ 1, 3 },
	{ 3, 0 }
};

local_ int boxEdgeVerts[12][2] = {
	// bottom
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	// top
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	// sides
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};

local_ int boxPlaneBitsSilVerts[64][7] = {
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000000 = 0
	{ 4, 7, 4, 0, 3, 0, 0 }, // 000001 = 1
	{ 4, 5, 6, 2, 1, 0, 0 }, // 000010 = 2
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000011 = 3
	{ 4, 4, 5, 1, 0, 0, 0 }, // 000100 = 4
	{ 6, 3, 7, 4, 5, 1, 0 }, // 000101 = 5
	{ 6, 4, 5, 6, 2, 1, 0 }, // 000110 = 6
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000111 = 7
	{ 4, 6, 7, 3, 2, 0, 0 }, // 001000 = 8
	{ 6, 6, 7, 4, 0, 3, 2 }, // 001001 = 9
	{ 6, 5, 6, 7, 3, 2, 1 }, // 001010 = 10
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001011 = 11
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001100 = 12
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001101 = 13
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001110 = 14
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001111 = 15
	{ 4, 0, 1, 2, 3, 0, 0 }, // 010000 = 16
	{ 6, 0, 1, 2, 3, 7, 4 }, // 010001 = 17
	{ 6, 3, 2, 6, 5, 1, 0 }, // 010010 = 18
	{ 0, 0, 0, 0, 0, 0, 0 }, // 010011 = 19
	{ 6, 1, 2, 3, 0, 4, 5 }, // 010100 = 20
	{ 6, 1, 2, 3, 7, 4, 5 }, // 010101 = 21
	{ 6, 2, 3, 0, 4, 5, 6 }, // 010110 = 22
	{ 0, 0, 0, 0, 0, 0, 0 }, // 010111 = 23
	{ 6, 0, 1, 2, 6, 7, 3 }, // 011000 = 24
	{ 6, 0, 1, 2, 6, 7, 4 }, // 011001 = 25
	{ 6, 0, 1, 5, 6, 7, 3 }, // 011010 = 26
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011011 = 27
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011100 = 28
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011101 = 29
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011110 = 30
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011111 = 31
	{ 4, 7, 6, 5, 4, 0, 0 }, // 100000 = 32
	{ 6, 7, 6, 5, 4, 0, 3 }, // 100001 = 33
	{ 6, 5, 4, 7, 6, 2, 1 }, // 100010 = 34
	{ 0, 0, 0, 0, 0, 0, 0 }, // 100011 = 35
	{ 6, 4, 7, 6, 5, 1, 0 }, // 100100 = 36
	{ 6, 3, 7, 6, 5, 1, 0 }, // 100101 = 37
	{ 6, 4, 7, 6, 2, 1, 0 }, // 100110 = 38
	{ 0, 0, 0, 0, 0, 0, 0 }, // 100111 = 39
	{ 6, 6, 5, 4, 7, 3, 2 }, // 101000 = 40
	{ 6, 6, 5, 4, 0, 3, 2 }, // 101001 = 41
	{ 6, 5, 4, 7, 3, 2, 1 }, // 101010 = 42
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101011 = 43
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101100 = 44
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101101 = 45
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101110 = 46
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101111 = 47
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110000 = 48
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110001 = 49
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110010 = 50
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110011 = 51
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110100 = 52
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110101 = 53
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110110 = 54
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110111 = 55
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111000 = 56
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111001 = 57
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111010 = 58
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111011 = 59
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111100 = 60
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111101 = 61
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111110 = 62
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111111 = 63
};


/*
============
OOBB::AddPoint
============
*/
bool OOBB::AddPoint( const Vec3D &v ) {
	Matrix3 axis2;
	AABB bounds1, bounds2;

	if ( extents[0] < 0.0f ) {
		extents.SetZero();
		center = v;
		axis.SetIdentity();
		return true;
	}

	bounds1[0][0] = bounds1[1][0] = center * axis[0];
	bounds1[0][1] = bounds1[1][1] = center * axis[1];
	bounds1[0][2] = bounds1[1][2] = center * axis[2];
	bounds1[0] -= extents;
	bounds1[1] += extents;
	if ( !bounds1.AddPoint( Vec3D( v * axis[0], v * axis[1], v * axis[2] ) ) ) {
		// point is contained in the box
		return false;
	}

	axis2[0] = v - center;
	axis2[0].Normalize();
	axis2[1] = axis[ Min3Index( axis2[0] * axis[0], axis2[0] * axis[1], axis2[0] * axis[2] ) ];
	axis2[1] = axis2[1] - ( axis2[1] * axis2[0] ) * axis2[0];
	axis2[1].Normalize();
	axis2[2].Cross( axis2[0], axis2[1] );

	AxisProjection( axis2, bounds2 );
	bounds2.AddPoint( Vec3D( v * axis2[0], v * axis2[1], v * axis2[2] ) );

	// create new box based on the smallest bounds
	if ( bounds1.GetVolume() < bounds2.GetVolume() ) {
		center = ( bounds1[0] + bounds1[1] ) * 0.5f;
		extents = bounds1[1] - center;
		center *= axis;
	}
	else {
		center = ( bounds2[0] + bounds2[1] ) * 0.5f;
		extents = bounds2[1] - center;
		center *= axis2;
		axis = axis2;
	}
	return true;
}

/*
============
OOBB::AddBox
============
*/
bool OOBB::AddBox( const OOBB &a ) {
	int i, besti;
	FLOAT v, bestv;
	Vec3D dir;
	Matrix3 ax[4];
	AABB bounds[4], b;

	if ( a.extents[0] < 0.0f ) {
		return false;
	}

	if ( extents[0] < 0.0f ) {
		center = a.center;
		extents = a.extents;
		axis = a.axis;
		return true;
	}

	// test axis of this box
	ax[0] = axis;
	bounds[0][0][0] = bounds[0][1][0] = center * ax[0][0];
	bounds[0][0][1] = bounds[0][1][1] = center * ax[0][1];
	bounds[0][0][2] = bounds[0][1][2] = center * ax[0][2];
	bounds[0][0] -= extents;
	bounds[0][1] += extents;
	a.AxisProjection( ax[0], b );
	if ( !bounds[0].AddBounds( b ) ) {
		// the other box is contained in this box
		return false;
	}

	// test axis of other box
	ax[1] = a.axis;
	bounds[1][0][0] = bounds[1][1][0] = a.center * ax[1][0];
	bounds[1][0][1] = bounds[1][1][1] = a.center * ax[1][1];
	bounds[1][0][2] = bounds[1][1][2] = a.center * ax[1][2];
	bounds[1][0] -= a.extents;
	bounds[1][1] += a.extents;
	AxisProjection( ax[1], b );
	if ( !bounds[1].AddBounds( b ) ) {
		// this box is contained in the other box
		center = a.center;
		extents = a.extents;
		axis = a.axis;
		return true;
	}

	// test axes aligned with the vector between the box centers and one of the box axis
	dir = a.center - center;
	dir.Normalize();
	for ( i = 2; i < 4; i++ ) {
		ax[i][0] = dir;
		ax[i][1] = ax[i-2][ Min3Index( dir * ax[i-2][0], dir * ax[i-2][1], dir * ax[i-2][2] ) ];
		ax[i][1] = ax[i][1] - ( ax[i][1] * dir ) * dir;
		ax[i][1].Normalize();
		ax[i][2].Cross( dir, ax[i][1] );

		AxisProjection( ax[i], bounds[i] );
		a.AxisProjection( ax[i], b );
		bounds[i].AddBounds( b );
	}

	// get the bounds with the smallest volume
	bestv = MX_INFINITY;
	besti = 0;
	for ( i = 0; i < 4; i++ ) {
		v = bounds[i].GetVolume();
		if ( v < bestv ) {
			bestv = v;
			besti = i;
		}
	}

	// create a box from the smallest bounds axis pair
	center = ( bounds[besti][0] + bounds[besti][1] ) * 0.5f;
	extents = bounds[besti][1] - center;
	center *= ax[besti];
	axis = ax[besti];

	return false;
}

/*
================
OOBB::PlaneDistance
================
*/
FLOAT OOBB::PlaneDistance( const Plane3D &plane ) const {
	FLOAT d1, d2;

	d1 = plane.Distance( center );
	d2 = mxFabs( extents[0] * plane.Normal()[0] ) +
			mxFabs( extents[1] * plane.Normal()[1] ) +
				mxFabs( extents[2] * plane.Normal()[2] );

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
OOBB::PlaneSide
================
*/
EPlaneSide OOBB::PlaneSide( const Plane3D &plane, const FLOAT epsilon ) const {
	FLOAT d1, d2;

	d1 = plane.Distance( center );
	d2 = mxFabs( extents[0] * plane.Normal()[0] ) +
			mxFabs( extents[1] * plane.Normal()[1] ) +
				mxFabs( extents[2] * plane.Normal()[2] );

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
OOBB::IntersectsBox
============
*/
bool OOBB::IntersectsBox( const OOBB &a ) const {
    Vec3D dir;			// vector between centers
    FLOAT c[3][3];		// matrix c = axis.Transpose() * a.axis
    FLOAT ac[3][3];		// absolute values of c
    FLOAT axisdir[3];	// axis[i] * dir
    FLOAT d, e0, e1;	// distance between centers and projected extents

	dir = a.center - center;
    
    // axis C0 + t * A0
    c[0][0] = axis[0] * a.axis[0];
    c[0][1] = axis[0] * a.axis[1];
    c[0][2] = axis[0] * a.axis[2];
    axisdir[0] = axis[0] * dir;
    ac[0][0] = mxFabs( c[0][0] );
    ac[0][1] = mxFabs( c[0][1] );
    ac[0][2] = mxFabs( c[0][2] );

    d = mxFabs( axisdir[0] );
	e0 = extents[0];
    e1 = a.extents[0] * ac[0][0] + a.extents[1] * ac[0][1] + a.extents[2] * ac[0][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1
    c[1][0] = axis[1] * a.axis[0];
    c[1][1] = axis[1] * a.axis[1];
    c[1][2] = axis[1] * a.axis[2];
    axisdir[1] = axis[1] * dir;
    ac[1][0] = mxFabs( c[1][0] );
    ac[1][1] = mxFabs( c[1][1] );
    ac[1][2] = mxFabs( c[1][2] );

    d = mxFabs( axisdir[1] );
	e0 = extents[1];
    e1 = a.extents[0] * ac[1][0] + a.extents[1] * ac[1][1] + a.extents[2] * ac[1][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2
    c[2][0] = axis[2] * a.axis[0];
    c[2][1] = axis[2] * a.axis[1];
    c[2][2] = axis[2] * a.axis[2];
    axisdir[2] = axis[2] * dir;
    ac[2][0] = mxFabs( c[2][0] );
    ac[2][1] = mxFabs( c[2][1] );
    ac[2][2] = mxFabs( c[2][2] );

    d = mxFabs( axisdir[2] );
	e0 = extents[2];
    e1 = a.extents[0] * ac[2][0] + a.extents[1] * ac[2][1] + a.extents[2] * ac[2][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B0
    d = mxFabs( a.axis[0] * dir );
    e0 = extents[0] * ac[0][0] + extents[1] * ac[1][0] + extents[2] * ac[2][0];
	e1 = a.extents[0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B1
    d = mxFabs( a.axis[1] * dir );
    e0 = extents[0] * ac[0][1] + extents[1] * ac[1][1] + extents[2] * ac[2][1];
	e1 = a.extents[1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B2
    d = mxFabs( a.axis[2] * dir );
    e0 = extents[0] * ac[0][2] + extents[1] * ac[1][2] + extents[2] * ac[2][2];
	e1 = a.extents[2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB0
    d = mxFabs( axisdir[2] * c[1][0] - axisdir[1] * c[2][0] );
    e0 = extents[1] * ac[2][0] + extents[2] * ac[1][0];
    e1 = a.extents[1] * ac[0][2] + a.extents[2] * ac[0][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB1
    d = mxFabs( axisdir[2] * c[1][1] - axisdir[1] * c[2][1] );
    e0 = extents[1] * ac[2][1] + extents[2] * ac[1][1];
    e1 = a.extents[0] * ac[0][2] + a.extents[2] * ac[0][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB2
    d = mxFabs( axisdir[2] * c[1][2] - axisdir[1] * c[2][2] );
    e0 = extents[1] * ac[2][2] + extents[2] * ac[1][2];
    e1 = a.extents[0] * ac[0][1] + a.extents[1] * ac[0][0];
    if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB0
    d = mxFabs( axisdir[0] * c[2][0] - axisdir[2] * c[0][0] );
    e0 = extents[0] * ac[2][0] + extents[2] * ac[0][0];
    e1 = a.extents[1] * ac[1][2] + a.extents[2] * ac[1][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB1
    d = mxFabs( axisdir[0] * c[2][1] - axisdir[2] * c[0][1] );
    e0 = extents[0] * ac[2][1] + extents[2] * ac[0][1];
    e1 = a.extents[0] * ac[1][2] + a.extents[2] * ac[1][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB2
    d = mxFabs( axisdir[0] * c[2][2] - axisdir[2] * c[0][2] );
    e0 = extents[0] * ac[2][2] + extents[2] * ac[0][2];
    e1 = a.extents[0] * ac[1][1] + a.extents[1] * ac[1][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB0
    d = mxFabs( axisdir[1] * c[0][0] - axisdir[0] * c[1][0] );
    e0 = extents[0] * ac[1][0] + extents[1] * ac[0][0];
    e1 = a.extents[1] * ac[2][2] + a.extents[2] * ac[2][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB1
    d = mxFabs( axisdir[1] * c[0][1] - axisdir[0] * c[1][1] );
    e0 = extents[0] * ac[1][1] + extents[1] * ac[0][1];
    e1 = a.extents[0] * ac[2][2] + a.extents[2] * ac[2][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB2
    d = mxFabs( axisdir[1] * c[0][2] - axisdir[0] * c[1][2] );
    e0 = extents[0] * ac[1][2] + extents[1] * ac[0][2];
    e1 = a.extents[0] * ac[2][1] + a.extents[1] * ac[2][0];
	if ( d > e0 + e1 ) {
        return false;
	}
    return true;
}

/*
============
OOBB::LineIntersection

  Returns true if the line intersects the box between the start and end point.
============
*/
bool OOBB::LineIntersection( const Vec3D &start, const Vec3D &end ) const {
    FLOAT ld[3];
    Vec3D lineDir = 0.5f * ( end - start );
    Vec3D lineCenter = start + lineDir;
    Vec3D dir = lineCenter - center;

    ld[0] = mxFabs( lineDir * axis[0] );
	if ( mxFabs( dir * axis[0] ) > extents[0] + ld[0] ) {
        return false;
	}

    ld[1] = mxFabs( lineDir * axis[1] );
	if ( mxFabs( dir * axis[1] ) > extents[1] + ld[1] ) {
        return false;
	}

    ld[2] = mxFabs( lineDir * axis[2] );
	if ( mxFabs( dir * axis[2] ) > extents[2] + ld[2] ) {
        return false;
	}

    Vec3D cross = lineDir.Cross( dir );

	if ( mxFabs( cross * axis[0] ) > extents[1] * ld[2] + extents[2] * ld[1] ) {
        return false;
	}

	if ( mxFabs( cross * axis[1] ) > extents[0] * ld[2] + extents[2] * ld[0] ) {
        return false;
	}

	if ( mxFabs( cross * axis[2] ) > extents[0] * ld[1] + extents[1] * ld[0] ) {
        return false;
	}

    return true;
}

/*
============
BoxPlaneClip
============
*/
static bool BoxPlaneClip( const FLOAT denom, const FLOAT numer, FLOAT &scale0, FLOAT &scale1 ) {
	if ( denom > 0.0f ) {
		if ( numer > denom * scale1 ) {
			return false;
		}
		if ( numer > denom * scale0 ) {
			scale0 = numer / denom;
		}
		return true;
	}
	else if ( denom < 0.0f ) {
		if ( numer > denom * scale0 ) {
			return false;
		}
		if ( numer > denom * scale1 ) {
			scale1 = numer / denom;
		}
		return true;
	}
	else {
		return ( numer <= 0.0f );
	}
}

/*
============
OOBB::RayIntersection

  Returns true if the ray intersects the box.
  The ray can intersect the box in both directions from the start point.
  If start is inside the box then scale1 < 0 and scale2 > 0.
============
*/
bool OOBB::RayIntersection( const Vec3D &start, const Vec3D &dir, FLOAT &scale1, FLOAT &scale2 ) const {
	Vec3D localStart, localDir;

	localStart = ( start - center ) * axis.Transpose();
	localDir = dir * axis.Transpose();

	scale1 = -MX_INFINITY;
	scale2 = MX_INFINITY;
    return	BoxPlaneClip(  localDir.x, -localStart.x - extents[0], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.x,  localStart.x - extents[0], scale1, scale2 ) &&
			BoxPlaneClip(  localDir.y, -localStart.y - extents[1], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.y,  localStart.y - extents[1], scale1, scale2 ) &&
			BoxPlaneClip(  localDir.z, -localStart.z - extents[2], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.z,  localStart.z - extents[2], scale1, scale2 );
}

/*
============
OOBB::FromPoints

  Tight box for a collection of points.
============
* /
void OOBB::FromPoints( const Vec3D *points, const int numPoints ) {
	int i;
	FLOAT invNumPoints, sumXX, sumXY, sumXZ, sumYY, sumYZ, sumZZ;
	Vec3D dir;
	AABB bounds;
	idMatX eigenVectors;
	idVecX eigenValues;

	// compute mean of points
	center = points[0];
	for ( i = 1; i < numPoints; i++ ) {
		center += points[i];
	}
	invNumPoints = 1.0f / numPoints;
	center *= invNumPoints;

	// compute covariances of points
	sumXX = 0.0f; sumXY = 0.0f; sumXZ = 0.0f;
	sumYY = 0.0f; sumYZ = 0.0f; sumZZ = 0.0f;
	for ( i = 0; i < numPoints; i++ ) {
		dir = points[i] - center;
		sumXX += dir.x * dir.x;
		sumXY += dir.x * dir.y;
		sumXZ += dir.x * dir.z;
		sumYY += dir.y * dir.y;
		sumYZ += dir.y * dir.z;
		sumZZ += dir.z * dir.z;
	}
	sumXX *= invNumPoints;
	sumXY *= invNumPoints;
	sumXZ *= invNumPoints;
	sumYY *= invNumPoints;
	sumYZ *= invNumPoints;
	sumZZ *= invNumPoints;

	// compute eigenvectors for covariance matrix
	eigenValues.SetData( 3, VECX_ALLOCA( 3 ) );
	eigenVectors.SetData( 3, 3, MATX_ALLOCA( 3 * 3 ) );

	eigenVectors[0][0] = sumXX;
	eigenVectors[0][1] = sumXY;
	eigenVectors[0][2] = sumXZ;
	eigenVectors[1][0] = sumXY;
	eigenVectors[1][1] = sumYY;
	eigenVectors[1][2] = sumYZ;
	eigenVectors[2][0] = sumXZ;
	eigenVectors[2][1] = sumYZ;
	eigenVectors[2][2] = sumZZ;
	eigenVectors.Eigen_SolveSymmetric( eigenValues );
	eigenVectors.Eigen_SortIncreasing( eigenValues );

	axis[0][0] = eigenVectors[0][0];
	axis[0][1] = eigenVectors[0][1];
	axis[0][2] = eigenVectors[0][2];
	axis[1][0] = eigenVectors[1][0];
	axis[1][1] = eigenVectors[1][1];
	axis[1][2] = eigenVectors[1][2];
	axis[2][0] = eigenVectors[2][0];
	axis[2][1] = eigenVectors[2][1];
	axis[2][2] = eigenVectors[2][2];

	extents[0] = eigenValues[0];
	extents[1] = eigenValues[0];
	extents[2] = eigenValues[0];

	// refine by calculating the bounds of the points projected onto the axis and adjusting the center and extents
	bounds.Clear();
    for ( i = 0; i < numPoints; i++ ) {
		bounds.AddPoint( Vec3D( points[i] * axis[0], points[i] * axis[1], points[i] * axis[2] ) );
    }
	center = ( bounds[0] + bounds[1] ) * 0.5f;
	extents = bounds[1] - center;
	center *= axis;
}
*/
/*
============
OOBB::FromPointTranslation

  Most tight box for the translational movement of the given point.
============
*/
void OOBB::FromPointTranslation( const Vec3D &point, const Vec3D &translation ) {
	// FIXME: implement
}

/*
============
OOBB::FromBoxTranslation

  Most tight box for the translational movement of the given box.
============
*/
void OOBB::FromBoxTranslation( const OOBB &box, const Vec3D &translation ) {
	// FIXME: implement
}

/*
============
OOBB::FromPointRotation

  Most tight bounds for the rotational movement of the given point.
============
*/
void OOBB::FromPointRotation( const Vec3D &point, const Rotation &rotation ) {
	// FIXME: implement
}

/*
============
OOBB::FromBoxRotation

  Most tight box for the rotational movement of the given box.
============
*/
void OOBB::FromBoxRotation( const OOBB &box, const Rotation &rotation ) {
	// FIXME: implement
}

/*
============
OOBB::ToPoints
============
*/
void OOBB::ToPoints( Vec3D points[8] ) const {
	Matrix3 ax;
	Vec3D temp[4];

	ax[0] = extents[0] * axis[0];
	ax[1] = extents[1] * axis[1];
	ax[2] = extents[2] * axis[2];
	temp[0] = center - ax[0];
	temp[1] = center + ax[0];
	temp[2] = ax[1] - ax[2];
	temp[3] = ax[1] + ax[2];
	points[0] = temp[0] - temp[3];
	points[1] = temp[1] - temp[3];
	points[2] = temp[1] + temp[2];
	points[3] = temp[0] + temp[2];
	points[4] = temp[0] - temp[2];
	points[5] = temp[1] - temp[2];
	points[6] = temp[1] + temp[3];
	points[7] = temp[0] + temp[3];
}

/*
============
OOBB::GetProjectionSilhouetteVerts
============
*/
int OOBB::GetProjectionSilhouetteVerts( const Vec3D &projectionOrigin, Vec3D silVerts[6] ) const {
	FLOAT f;
	int i, planeBits, *index;
	Vec3D points[8], dir1, dir2;

	ToPoints( points );

	dir1 = points[0] - projectionOrigin;
	dir2 = points[6] - projectionOrigin;
	f = dir1 * axis[0];
	planeBits = FLOATSIGNBITNOTSET( f );
	f = dir2 * axis[0];
	planeBits |= FLOATSIGNBITSET( f ) << 1;
	f = dir1 * axis[1];
	planeBits |= FLOATSIGNBITNOTSET( f ) << 2;
	f = dir2 * axis[1];
	planeBits |= FLOATSIGNBITSET( f ) << 3;
	f = dir1 * axis[2];
	planeBits |= FLOATSIGNBITNOTSET( f ) << 4;
	f = dir2 * axis[2];
	planeBits |= FLOATSIGNBITSET( f ) << 5;

	index = boxPlaneBitsSilVerts[planeBits];
	for ( i = 0; i < index[0]; i++ ) {
		silVerts[i] = points[index[i+1]];
	}

	return index[0];
}

/*
============
OOBB::GetParallelProjectionSilhouetteVerts
============
*/
int OOBB::GetParallelProjectionSilhouetteVerts( const Vec3D &projectionDir, Vec3D silVerts[6] ) const {
	FLOAT f;
	int i, planeBits, *index;
	Vec3D points[8];

	ToPoints( points );

	planeBits = 0;
	f = projectionDir * axis[0];
	if ( FLOATNOTZERO( f ) ) {
		planeBits = 1 << FLOATSIGNBITSET( f );
	}
	f = projectionDir * axis[1];
	if ( FLOATNOTZERO( f ) ) {
		planeBits |= 4 << FLOATSIGNBITSET( f );
	}
	f = projectionDir * axis[2];
	if ( FLOATNOTZERO( f ) ) {
		planeBits |= 16 << FLOATSIGNBITSET( f );
	}

	index = boxPlaneBitsSilVerts[planeBits];
	for ( i = 0; i < index[0]; i++ ) {
		silVerts[i] = points[index[i+1]];
	}

	return index[0];
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
