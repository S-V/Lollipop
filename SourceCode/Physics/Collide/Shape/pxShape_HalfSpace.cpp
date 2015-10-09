/*
=============================================================================
	File:	pxShape_HalfSpace.cpp
	Desc:	Half-space collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_HalfSpace.h>

// from "Real-time collision detection" by Christer Ericson
// Intersect sphere s with movement vector v with plane p.
// If intersecting, return time t of collision and point q
// at which sphere hits plane.
//
int IntersectMovingSpherePlane(const Sphere& sphere, const Vec3D& direction,
							   const Plane3D& plane,
							   float &t, Vec3D &q)
{
	// Compute distance of sphere center to plane.
	//float dist = Dot( p.Normal(), s.Center ) - plane.D.
	const float dist = plane.Distance( sphere.Center );
	if( mxFabs(dist) <= sphere.Radius )
	{
		// The sphere is already overlapping the plane.
		// Set the time of intersection to zero and q to sphere center.
		t = 0.0f;
		q = sphere.Center;
		return 1;
	}
	else
	{
		const float denom = Dot( plane.Normal(), direction );
		if( denom * dist >= 0.0f )
		{
			// No intersection as sphere moving parallel to or away from plane
			return 0;
		}
		else
		{
			// Sphere is moving towards the plane.
			// Use +r in computations if sphere in front of plane, else -r
			const float r = (dist > 0.0f) ? sphere.Radius : -sphere.Radius;
			t = (r - dist) / denom;
			q = sphere.Center + (t * direction) - (r * plane.Normal());
			return 1;
		}
	}
}

// from ODE
static void ComputePlaneAABB( const Plane3D& plane, pxAABB &outAABB )
{
	outAABB.SetInfinity();

	// Planes that have normal vectors aligned along an axis can use a
	// less comprehensive (half space) bounding box.

#if 1
	//const int planeType = plane.Type();
	const Vec3D& N = plane.Normal();

	// NOTE: PX_LARGE_FLOAT is used instead of PX_INFINITY, because numerical errors will occur otherwise
MX_REFACTOR("move to plane3D.cpp");

	const F4 Nx = N[AXIS_X];
	const F4 Ny = N[AXIS_Y];
	const F4 Nz = N[AXIS_Z];

	// if normal aligned with Y-axis
	if( Nx == 0.0f && Nz == 0.0f )
	{
		const F4 minY = (Ny > 0.0f) ? -PX_LARGE_FLOAT : plane.d;
		const F4 maxY = (Ny > 0.0f) ? -plane.d : PX_LARGE_FLOAT;

		outAABB.mMin.y = minY;
		outAABB.mMax.y = maxY;
	}
	else
	// if normal aligned with X-axis
	if( Ny == 0.0f && Nz == 0.0f )
	{
		const F4 minX = (Nx > 0.0f) ? -PX_LARGE_FLOAT : plane.d;
		const F4 maxX = (Nx > 0.0f) ? -plane.d : PX_LARGE_FLOAT;

		outAABB.mMin.x = minX;
		outAABB.mMax.x = maxX;
	}
	else
	// if normal aligned with Z-axis
	if( Nx == 0.0f && Ny == 0.0f )
	{
		const F4 minZ = (Nz > 0.0f) ? -PX_LARGE_FLOAT : plane.d;
		const F4 maxZ = (Nz > 0.0f) ? -plane.d : PX_LARGE_FLOAT;

		outAABB.mMin.z = minZ;
		outAABB.mMax.z = maxZ;
	}
#endif
}

/*================================
		pxShape_HalfSpace
================================*/

pxShape_HalfSpace::pxShape_HalfSpace( pxReal Nx, pxReal Ny, pxReal Nz, pxReal D )
	: pxShape( pxcShapeType::PX_SHAPE_HALFSPACE )
	, mPlane( Nx, Ny, Nz, D )
{
	this->RecomputeBounds();
}

pxShape_HalfSpace::pxShape_HalfSpace( const Plane3D& plane )
	: pxShape( pxcShapeType::PX_SHAPE_HALFSPACE )
	, mPlane( plane.a, plane.b, plane.c, plane.d )
{
	this->RecomputeBounds();
}

pxShape_HalfSpace::~pxShape_HalfSpace() {
}

void pxShape_HalfSpace::GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const
{
	outBounds = mAABB;
}

void pxShape_HalfSpace::TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const
{
	Unimplemented_Checked;
#if 0
	MX_REFACTOR(":");
	// code copied from Quake3 source (CM_TraceThroughBrush())

	// adjust the plane distance appropriately for radius
	Plane3D		p( mPlane );
	p.d -= input.radius;

	output.normal = p.Normal();

	const F4 dStart = p.Distance( input.start );
	const F4 dEnd = p.Distance( input.end );

	bool	getout, startout;

	getout = true;
	startout = true;

	if (dEnd > 0) {
		getout = true;	// endpoint is not in solid
	}
	if (dStart > 0) {
		startout = true;
	}

	#define SURFACE_CLIP_EPSILON	0.125

	// completely in front of plane, no intersection
	//if( dStart > 0 && dEnd > 0 ) {
	if (dStart > 0 && ( dEnd >= SURFACE_CLIP_EPSILON || dEnd >= dStart )  ) {
		output.fraction = 1.0f;
		return;
	}

	// completely behind plane
	if( dStart <= 0 && dEnd <= 0 ) {
		output.fraction = 0.0f;
		return;
	}

	//if( dStart == dEnd ) {
	//	output.fraction = ( dStart > 0 ) ? 1.0f : 0.0f;
	//	return;
	//}

	// crosses plane

	float	enterFrac, leaveFrac;

	enterFrac = -1.0;
	leaveFrac = 1.0;

	if (dStart > dEnd) {	// enter
		float	f = (dStart-SURFACE_CLIP_EPSILON) / (dStart-dEnd);
		if ( f < 0 ) {
			f = 0;
		}
		if (f > enterFrac) {
			enterFrac = f;

		}
	} else {	// leave
		float	f = (dStart+SURFACE_CLIP_EPSILON) / (dStart-dEnd);
		if ( f > 1 ) {
			f = 1;
		}
		if (f < leaveFrac) {
			leaveFrac = f;
		}
	}

	//
	// all planes have been checked, and the trace was not
	// completely outside the brush
	//
	if (!startout) {	// original point was inside brush
		output.startsolid = true;
		if (!getout) {
			output.allsolid = true;
			output.fraction = 0;
			//output.contents = brush->contents;
		}
		return;
	}

	if (enterFrac < leaveFrac) {
		if (enterFrac > -1 && enterFrac < output.fraction) {
			if (enterFrac < 0) {
				enterFrac = 0;
			}
			output.fraction = enterFrac;
			//output.plane = *clipplane;
			//output.surfaceFlags = leadside->surfaceFlags;
			//output.contents = brush->contents;
		}
	}

//DBGOUT("fraction = %f\n",output.fraction);
#endif
}

void pxShape_HalfSpace::RecomputeBounds()
{
	Assert( mPlane.Normal().IsNormalized() );

	// Planes that have normal vectors aligned along an axis can use a
	// less comprehensive (half space) bounding box.

	//mAABB.SetInfinity();
	ComputePlaneAABB( mPlane, mAABB );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
