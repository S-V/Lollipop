/*
=============================================================================
	File:	pxOBB.h
	Desc:	Oriented bounding box.
=============================================================================
*/

#ifndef __PX_OBB_H__
#define __PX_OBB_H__

//
//	pxOBB
//
class pxOBB {
public:
	pxReal   mOrigin[3];
	pxReal   mAxis[3][2];	// two axes, the third is computed as (axis0 x axis1)
	pxReal   mExtent[3];	// half widths along each dimension

public:
	FORCEINLINE pxOBB()
	{}

	//FORCEINLINE pxOBB( const pxVec3& mOrigin, const pxVec3& halfSize, const pxMat3x3& mAxis )
	//	: mOrigin( mOrigin ), mExtent( halfSize ), mAxis( mAxis )
	//{}
};

#endif // !__PX_OBB_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
