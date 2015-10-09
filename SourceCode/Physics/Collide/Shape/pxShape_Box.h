/*
=============================================================================
	File:	pxShape_Box.h
	Desc:	Collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_BOX_H__
#define __PX_COLLISION_SHAPE_BOX_H__

//
//	pxShape_Box
//
MX_ALIGN_16(class) pxShape_Box : public pxShape {
public_internal:
	pxVec3	mHalfSize;

public:
	pxShape_Box( const pxVec3& halfExtents );
	~pxShape_Box();

	void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const;

	FORCEINLINE const pxVec3& GetHalfSize() const { return mHalfSize; }
	FORCEINLINE pxVec3 Num() const { return pxReal(2.0) * mHalfSize; }
};

#endif // !__PX_COLLISION_SHAPE_BOX_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
