/*
=============================================================================
	File:	collide_sphere_box.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLIDE_SPHERE_BOX_H__
#define __PX_COLLIDE_SPHERE_BOX_H__

//
//	pxSphereBoxAgent
//
class pxSphereBoxAgent : public pxCollisionAgent
{
public:
	pxSphereBoxAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxSphereBoxAgent();

	void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	);

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return shapeA == PX_SHAPE_SPHERE
			&& shapeB == PX_SHAPE_BOX
			;
	}
};

#endif // !__PX_COLLIDE_SPHERE_BOX_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
