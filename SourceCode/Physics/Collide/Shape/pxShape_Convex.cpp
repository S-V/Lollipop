/*
=============================================================================
	File:	pxShape_Convex.cpp
	Desc:	Collision shape.
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Convex.h>

/*
-----------------------------------------------------------------------------
	pxSupportMappedShape
-----------------------------------------------------------------------------
*/
pxReal pxSupportMappedShape::GetEffectiveRadius( const pxVec3& direction ) const
{
	return this->GetSupportingVertex( direction ).Length();
}

void pxSupportMappedShape::ProjectOnto( const pxVec3& direction, pxReal &outMin, pxReal &outMax ) const
{
	const pxVec3 pos = this->GetSupportingVertex(  direction );
	const pxVec3 neg = this->GetSupportingVertex( -direction );

	outMin = neg.Dot( direction );
	outMax = pos.Dot( direction );
	Assert( outMin < outMax );
}

/*
-----------------------------------------------------------------------------
	pxShape_Convex
-----------------------------------------------------------------------------
*/
pxShape_Convex::pxShape_Convex( pxcShapeType type )
	: pxSupportMappedShape( type )
{
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
