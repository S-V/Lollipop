/*
=============================================================================
	File:	pxShape_HalfSpace.h
	Desc:	Half-space collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_HalfSpace_H__
#define __PX_COLLISION_SHAPE_HalfSpace_H__

//
//	pxShape_HalfSpace - is an infinite non-moving (static) collision plane.
//
struct pxShape_HalfSpace : public pxShape
{
	pxAABB		mAABB;
	Plane3D		mPlane;

public:
	pxShape_HalfSpace( pxReal Nx, pxReal Ny, pxReal Nz, pxReal D );
	pxShape_HalfSpace( const Plane3D& plane );
	~pxShape_HalfSpace();

	virtual void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const override;

	virtual void TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const override;

private:
	void RecomputeBounds();

public_internal:
	pxShape_HalfSpace() {}
};

#endif // !__PX_COLLISION_SHAPE_HalfSpace_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
