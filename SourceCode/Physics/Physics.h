/*
=============================================================================
	File:	Physics.h
	Desc:	Physics system public header file.
=============================================================================
*/

#ifndef __PX_PHYSICS_H__
#define __PX_PHYSICS_H__

//Physics/Base
#include <Physics/Base/pxBuildConfig.h>
#include <Physics/Base/pxTypes.h>
#include <Physics/Base/pxMath.h>
#include <Physics/Base/pxQuadWord.h>
#include <Physics/Base/pxVec3.h>
#include <Physics/Base/pxVec4.h>
#include <Physics/Base/pxPlane.h>
#include <Physics/Base/TVectorX.h>
#include <Physics/Base/pxQuat.h>
#include <Physics/Base/pxMat3x3.h>
#include <Physics/Base/pxMat4x3.h>
#include <Physics/Base/TMatrixX.h>
#include <Physics/Base/pxTransform.h>
#include <Physics/Base/pxSphere.h>
#include <Physics/Base/pxAABB.h>
#include <Physics/Base/pxOBB.h>
#include <Physics/Base/pxMass.h>
#include <Physics/Base/pxMaterial.h>
#include <Physics/Base/pxDebugDrawer.h>
#include <Physics/Base/Experimental.h>
#include <Physics/Base/pxObjects.h>
#include <Physics/Base/PoolAlloc.h>

#include <Physics/Support/pxUtilities.h>

//Physics/Collide
#include <Physics/Collide/pxCollision.h>

//Physics/Collide/Shape
#include <Physics/Collide/Shape/pxShape.h>
//#include <Physics/Collide/Shape/pxShape_Convex.h>
//#include <Physics/Collide/Shape/pxShape_HalfSpace.h>
//#include <Physics/Collide/Shape/pxShape_Sphere.h>
//#include <Physics/Collide/Shape/pxShape_Box.h>
//#include <Physics/Collide/Shape/pxShape_StaticBSP.h>

//Physics/Collide/Agent
#include <Physics/Collide/Agent/pxCollideable.h>


//Physics/Collide/Agent
#include <Physics/Collide/Agent/pxCollisionAgent.h>

//Physics/Collide/BroadPhase
#include <Physics/Collide/BroadPhase/pxBroadphase.h>
#include <Physics/Collide/BroadPhase/pxBroadphasePair.h>
#include <Physics/Collide/BroadPhase/pxBroadphase_Simple.h>

//Physics/Collide/Query
#include <Physics/Collide/Query/pxShapeRayCastInput.h>
#include <Physics/Collide/Query/pxShapeRayCastOutput.h>

//Physics/Collide/NearPhase
#include <Physics/Collide/NearPhase/pxContactPoint.h>
#include <Physics/Collide/NearPhase/pxContactManifold.h>
#include <Physics/Collide/NearPhase/pxContactCache.h>

//Physics/Collide/Dispatch
#include <Physics/Collide/Dispatch/pxCollisionPair.h>
#include <Physics/Collide/Dispatch/pxCollisionPairHash.h>
#include <Physics/Collide/Dispatch/pxCollisionDispatcher.h>

//Physics/Collide/NearPhase
//#include <Physics/Collide/NearPhase/pxSpherePlaneAgent.h>
//#include <Physics/Collide/NearPhase/pxSphereSphereAgent.h>
#include <Physics/Collide/NearPhase/collide_box_plane.h>
#include <Physics/Collide/NearPhase/collide_sphere_box.h>

//Physics/Collide/
#include <Physics/Collide/pxCollisionWorld.h>

//Physics/Solve/
#include <Physics/Solve/pxConstraint.h>
#include <Physics/Solve/pxJoint.h>
#include <Physics/Solve/pxConstraintSolver.h>
#include <Physics/Solve/pxConstraintSolver_PGS.h>

//Physics/Simulate/
#include <Physics/Simulate/pxRigidBody.h>
#include <Physics/Simulate/pxWorld.h>
#include <Physics/Simulate/pxPhysicsSystem.h>


namespace Physics
{
	void Initialize();
	void Shutdown();

	void Serialize( mxArchive& archive );

	pxCollisionDispatcher* GetCollisionDispatcher();

	pxShape::Handle AddCollisionShape( pxShape* shape );
	pxShape* GetCollisionShape( pxShape::Handle shape );
	void RemoveCollisionShape( pxShape::Handle shape );

	pxMaterial::Handle NewMaterial( const pxMaterialDesc& desc );
	pxMaterial& GetMaterial( pxMaterial::Handle materialId );
	void RemoveMaterial( pxMaterial::Handle materialId );

}//namespace Physics

#endif // !__PX_PHYSICS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
