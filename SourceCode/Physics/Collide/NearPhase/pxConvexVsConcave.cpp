#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/NearPhase/pxConvexVsConcave.h>

/*
-----------------------------------------------------------------------------
	pxOneManifoldCollisionAgent
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
	pxConvexVsStaticBspAgent
-----------------------------------------------------------------------------
*/
pxConvexVsStaticBspAgent::pxConvexVsStaticBspAgent( const pxCollisionAgentCreationInfo& cInfo )
	: Super( cInfo )
{

}

pxConvexVsStaticBspAgent::~pxConvexVsStaticBspAgent()
{

}

void pxConvexVsStaticBspAgent::ProcessCollision(
	pxCollideable* objA, pxCollideable* objB,
	const pxProcessCollisionInput& input,
	pxProcessCollisionOutput & result
)
{
	const pxTransform& convexObjTransform = objA->GetTransform();
	const pxTransform& staticBspTransform = objB->GetTransform();

	Assert(staticBspTransform.IsIdentity());

	//const pxTransform convexToBspTransform = convexObjTransform.inverse();

	pxShape_Convex* convexShape;
	pxShape_StaticBSP* bspShape;

	GetShapes( objA, objB, convexShape, bspShape );
Unimplemented_Checked;
	//const bool bIntersects = bspShape->GetTree().TestOverlapConvex(
	//	convexShape, convexObjTransform, *m_manifold );

	//if(!bIntersects)
(void)convexObjTransform;
	MX_UNDONE;
	//pxShape_Sphere* sphereShape = convexShape->UpCast<pxShape_Sphere>();

	//bspShape->GetTree().OverlapSphere(
	//	convexShape, convexObjTransform, *m_manifold
	//	);
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
