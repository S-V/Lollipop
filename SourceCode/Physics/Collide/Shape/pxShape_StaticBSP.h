/*
=============================================================================
	File:	pxShape_StaticBSP.h
	Desc:	BSP collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_BSP_H__
#define __PX_COLLISION_SHAPE_BSP_H__

#include <Core/VectorMath.h>
#include <Physics/Collide/Shape/pxShape.h>
#include <Physics/Support/BSP.h>

/*
-----------------------------------------------------------------------------
	pxShape_StaticBSP
-----------------------------------------------------------------------------
*/
class pxShape_StaticBSP : public pxShape
{
	pxAABB		m_bounds;	// in local space
	BSP_Tree	m_bspTree;

public:
	pxShape_StaticBSP();
	~pxShape_StaticBSP();

	void Build( pxTriangleMeshInterface* triangleMesh );

	virtual void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const override;

	virtual void TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const override;

	FORCEINLINE const BSP_Tree& GetTree() const
	{
		return m_bspTree;
	}
};

#endif // !__PX_COLLISION_SHAPE_BSP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
