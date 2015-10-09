/*
=============================================================================
	File:	pxShape_StaticBSP.cpp
	Desc:	BSP collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Support/BSP.h>
#include <Physics/Collide/Shape/pxShape_StaticBSP.h>

/*================================
		pxShape_StaticBSP
================================*/

pxShape_StaticBSP::pxShape_StaticBSP()
	: pxShape( pxcShapeType::PX_SHAPE_STATIC_BSP )
{
}

pxShape_StaticBSP::~pxShape_StaticBSP() {
}

void pxShape_StaticBSP::Build( pxTriangleMeshInterface* triangleMesh )
{
	pxTriangleIndexCallback_ComputeAABB		computeBounds;
	triangleMesh->ProcessAllTriangles( &computeBounds );

	m_bounds.Set(
		pxVec3::From_Vec3D( computeBounds.aabb.GetMin() ),
		pxVec3::From_Vec3D( computeBounds.aabb.GetMax() )
		);

	m_bspTree.Build( triangleMesh );
}

void pxShape_StaticBSP::GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const
{
	pxTransformAabb(
		m_bounds.mMin, m_bounds.mMax,
		PX_COLLISION_TOLERANCE,
		xform,
		outBounds.mMin, outBounds.mMax );
}

void pxShape_StaticBSP::TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const
{
	m_bspTree.TraceAABB( input.size, input.start, input.end,
		output.fraction, output.normal );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
