/*
=============================================================================
	File:	pxShape_Convex.h
	Desc:	Convex collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_CONVEX_H__
#define __PX_COLLISION_SHAPE_CONVEX_H__

#include <Physics/Collide/Shape/pxShape.h>

/*
-----------------------------------------------------------------------------
	pxSupportMappedShape
-----------------------------------------------------------------------------
*/
class pxSupportMappedShape : public pxShape
{
public:

	// Returns the vertex furthest along the given input direction in the shape's local space.
	// NOTE: 'direction' must be be normalized.
	//
	virtual pxVec3 GetSupportingVertex( const pxVec3& direction ) const = 0;

	//--pxShape
	virtual pxReal GetEffectiveRadius( const pxVec3& direction ) const override;

	virtual void ProjectOnto( const pxVec3& direction, pxReal &outMin, pxReal &outMax ) const;

protected:
	inline pxSupportMappedShape( pxcShapeType type )
		: pxShape( type )
	{}
};

/*
-----------------------------------------------------------------------------
	pxShape_Convex
-----------------------------------------------------------------------------
*/
class pxShape_Convex : public pxSupportMappedShape
{
public:

protected:
	pxShape_Convex( pxcShapeType type );
};

#endif // !__PX_COLLISION_SHAPE_CONVEX_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
