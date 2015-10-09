#pragma once

#include <Physics/Collide/Shape/pxShape_Convex.h>
#include <Physics/Collide/Shape/pxShape_StaticBSP.h>
#include <Physics/Collide/Agent/pxCollisionAgent.h>

/*
-----------------------------------------------------------------------------
	pxConvexVsStaticBspAgent
-----------------------------------------------------------------------------
*/
class pxConvexVsStaticBspAgent
	: public pxOneManifoldCollisionAgent< pxShape_Convex, pxShape_StaticBSP >
{
public:
	typedef pxOneManifoldCollisionAgent Super;

	pxConvexVsStaticBspAgent( const pxCollisionAgentCreationInfo& cInfo );
	~pxConvexVsStaticBspAgent();

	void ProcessCollision(
		pxCollideable* objA, pxCollideable* objB,
		const pxProcessCollisionInput& input,
		pxProcessCollisionOutput & result
	);

	static bool HandlesCollisionShapes( pxcShapeType shapeA, pxcShapeType shapeB )
	{
		return pxShapeIsConvex( shapeA )
			&& pxShapeIsConcave( shapeB )
			;
	}

protected:

};

