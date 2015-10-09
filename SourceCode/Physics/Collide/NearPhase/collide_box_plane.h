/*
=============================================================================
	File:	collide_box_plane.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLIDE_BOX_PLANE_H__
#define __PX_COLLIDE_BOX_PLANE_H__

//
//	pxBoxPlaneAgent
//
class pxBoxPlaneAgent : public pxCollisionAgent
{
public:
	pxBoxPlaneAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxBoxPlaneAgent();

	void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	);

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return shapeA == PX_SHAPE_BOX
			&& shapeB == PX_SHAPE_HALFSPACE
			;
	}
};

#endif // !__PX_COLLIDE_BOX_PLANE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
