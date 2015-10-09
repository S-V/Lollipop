/*
=============================================================================
	File:	pxCollisionResult.h
	Desc:	Contact processing.
=============================================================================
*/

#ifndef __PX_COLLISION_RESULT_H__
#define __PX_COLLISION_RESULT_H__

#if 0
//
//	pxCollisionResult - collision/contact info.
//
MX_ALIGN_16(struct) pxCollisionResult
{
	pxContactManifold *	manifold;	// proximity points
	pxUInt				numManifolds;

	pxCollideable *	oA;	// body A, it is always dynamic (movable)
	pxCollideable *	oB;	// body B, it can be either static or dynamic

public:
	pxCollisionResult()
	{}

	PX_INLINE void FlipNormals()
	{
		for(pxUInt iPoint = 0; iPoint < manifold.numPoints; iPoint++)
		{
			manifold.points[ iPoint ].normal *= pxReal(-1.0f);
		}
	}
	PX_INLINE void SwapBodies()
	{
		TSwap( oA, oB );
	}

	PX_INLINE pxUInt NumPoints() const {
		return manifold.numPoints;
	}

	PX_INLINE bool isOk() const
	{
		return CHK(oA)
			&& CHK(oB)
			&& CHK(oA!=oB)
			&& CHK(NumPoints()>0)
			&& CHK(NumPoints()<=MAX_CONTACTS)
			;
	}
};

typedef TList< pxCollisionResult >	pxCollisionInfoList;
#endif

#endif // !__PX_COLLISION_RESULT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
