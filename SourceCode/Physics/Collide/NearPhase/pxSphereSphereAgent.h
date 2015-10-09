/*
=============================================================================
	File:	pxSphereSphereAgent.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLIDE_SPHERE_SPHERE_H__
#define __PX_COLLIDE_SPHERE_SPHERE_H__

//
//	pxSphereSphereAgent
//
class pxSphereSphereAgent : public pxCollisionAgent
{
public:
	pxSphereSphereAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxSphereSphereAgent();

	void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	);

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return shapeA == PX_SHAPE_SPHERE
			&& shapeB == PX_SHAPE_SPHERE
			;
	}

protected:
	pxContactManifold *	manifold;
};

#endif // !__PX_COLLIDE_SPHERE_SPHERE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
