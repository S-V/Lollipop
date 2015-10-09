/*
=============================================================================
	File:	pxSpherePlaneAgent.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLIDE_SPHERE_PLANE_H__
#define __PX_COLLIDE_SPHERE_PLANE_H__

#include <Physics/Collide/Agent/pxCollisionAgent.h>
#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Collide/Shape/pxShape_HalfSpace.h>

//
//	pxSpherePlaneAgent
//
class pxSpherePlaneAgent
	: public pxOneManifoldCollisionAgent< pxShape_Sphere, pxShape_HalfSpace >
{
public:
	typedef pxOneManifoldCollisionAgent Super;

	pxSpherePlaneAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxSpherePlaneAgent();

	virtual void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	) override;

	virtual pxReal CalculateTOI( const pxToiInput& input, pxToiOutput &output ) override;

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return shapeA == PX_SHAPE_SPHERE
			&& shapeB == PX_SHAPE_HALFSPACE
			;
	}
};

#endif // !__PX_COLLIDE_SPHERE_PLANE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
