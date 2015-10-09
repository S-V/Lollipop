/*
=============================================================================
	File:	pxShape.cpp
	Desc:	Collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#if PX_DEBUG
const char* pxDbgGetShapeName( pxcShapeType eShapeType )
{
	switch( eShapeType )
	{
	case PX_SHAPE_HALFSPACE :
		return "HalfSpace";

	case PX_SHAPE_SPHERE :
		return "Sphere";

	case PX_SHAPE_STATIC_BSP :
		return "StaticBSP";

	default:
		Unreachable;
		return nil;
	}
}
#endif // PX_DEBUG

void ShapePMTraceInput::UpdateCachedData()
{
	Assert(end != start);
	direction = (end - start).GetNormalized();

	// calculate bounds for the entire trace
	fullTraceBounds.Clear();
	fullTraceBounds.AddPoint( end );
	fullTraceBounds.AddPoint( start );

	//bounds.ExpandSelf( radius );
	Vec3D	halfSize = size.GetHalfSize();
	fullTraceBounds.GetMin() -= halfSize;
	fullTraceBounds.GetMax() += halfSize;
}

ShapePMTraceOutput::ShapePMTraceOutput()
{
	normal.SetZero();
	fraction = 1.0f;
	//startsolid = false;
	//allsolid = false;
}

/*
-----------------------------------------------------------------------------
	pxShape
-----------------------------------------------------------------------------
*/
pxShape::pxShape( pxcShapeType type )
{
	mType = type;
	mUserData = nil;
}

pxShape::~pxShape()
{}

bool pxShape::CastRay( const pxShapeRayCastInput& input, pxShapeRayCastOutput &output ) const
{
	Unimplemented;
	return false;
}

void pxShape::CastRay( const pxShapeRayCastInput& input, pxRayHitCollector &collector ) const
{
	Unimplemented;
}

void pxShape::TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const
{
	// 1.0 = didn't hit anything
	output.fraction = 1.0f;
}

pxReal pxShape::GetEffectiveRadius( const pxVec3& direction ) const
{
	pxAABB bounds;
	GetWorldBounds( pxTransform::GetIdentity(), bounds );
	pxVec3 support = pxAabbSupport( bounds.GetSize(), direction );
	return support.Length();
}

void pxShape::CalculateInertiaLocal( pxReal mass, pxVec3 &inertia ) const
{
	inertia.SetAll( 0.0f );
}

pxShape* pxShape::Static_GetPointerByHandle( pxShape::Handle handle )
{
	return Physics::GetCollisionShape( handle );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
