/*
=============================================================================
	File:	pxShape_Box.cpp
	Desc:	Collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Box.h>

/*================================
		pxShape_Box
================================*/

pxShape_Box::pxShape_Box( const pxVec3& halfExtents )
	: pxShape( pxcShapeType::PX_SHAPE_BOX )
{
	mHalfSize = halfExtents;
}

pxShape_Box::~pxShape_Box() {
}

void pxShape_Box::GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const
{
	const pxVec3& center = xform.GetOrigin();

	const pxVec3 x_row = xform.GetBasis().GetRow(0).absolute();
	const pxVec3 y_row = xform.GetBasis().GetRow(1).absolute();
	const pxVec3 z_row = xform.GetBasis().GetRow(2).absolute();

	const pxReal xrange = x_row.Dot( mHalfSize );
	const pxReal yrange = y_row.Dot( mHalfSize );
	const pxReal zrange = z_row.Dot( mHalfSize );

	const pxVec3 extent( xrange, yrange, zrange );

	outBounds.mMin = center - extent;
	outBounds.mMax = center + extent;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
