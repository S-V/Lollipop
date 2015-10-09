/*
=============================================================================
	File:	pxWorld.h
	Desc:	Physics world.
=============================================================================
*/

#ifndef __PX_PHYSICS_WORLD_H__
#define __PX_PHYSICS_WORLD_H__

#include <Core/Memory.h>


// size of fixed time step - ~60 Hz
#define PX_DEFAULT_FIXED_STEP_SIZE (1.0f/60.0f)

enum { PX_MAX_COLLIDEABLES = 512*2 };
// temporary workaround until we have rigid body dynamics
enum { PX_MAX_RIGID_BODIES = /*512**/2 };


// NOTE: pxCollideable structs are not moved in memory (they're usually accessed randomly);
// but pointers will invalidated if the array reallocates memory so use handles/indices.
typedef
	TList< pxCollideable >
CollideableList;

// NOTE: pxRigidBody structs can be reordered in memory (they're usually accessed linearly)
typedef
	TIndirectHandleManager< pxRigidBody, PX_MAX_RIGID_BODIES >
RigidBodyList;


//
// pxSolverInput
//
class pxSolverInput {
public:
	// Time step information.
	pxReal			deltaTime;

	pxRigidBody *	bodies;
	pxUInt			numBodies;
//	pxConstraint *	constraints;
//	pxUInt			numConstraints;
	pxContactCache*	contacts;

	pxF4	gravity;	// for faking friction

public:
	pxSolverInput()
	{
		setDefaults();
	}
	void setDefaults()
	{
		deltaTime = 0.0f;

		bodies = nil;
		numBodies = 0;

		contacts = nil;

		gravity = 10.0f;
	}
	bool isOk() const
	{
		return 1
			&& (deltaTime > 0.0f)

		//	&& CHK((bodies && numBodies)||(!bodies && !numBodies))
			&& CHK(contacts)
			;
	}
};


/*
-----------------------------------------------------------------------------
	pxWorldCreationInfo
-----------------------------------------------------------------------------
*/
struct pxWorldCreationInfo
{
	pxBroadphase *			broadphase;
	pxCollisionDispatcher *	dispatcher;

	pxConstraintSolver *	constraintSolver;

	pxVec3	gravity;	// gravity acceleration

public:
	pxWorldCreationInfo()
	{
		setDefaults();
	}
	void setDefaults();
	bool isOk() const;
};


/*
-----------------------------------------------------------------------------
	pxWorld

	- is a simulation environment.
-----------------------------------------------------------------------------
*/
class pxWorld
{
	PX_DECLARE_CLASS_ALLOCATOR( pxWorld, PX_MEMORY_DYNAMICS );

public:
			pxWorld( const pxWorldCreationInfo& worldDesc );
	virtual	~pxWorld();

	// Collision detection

	//pxCollideable::Handle AddCollisionObject( const pxCollideableInfo& cInfo );
	//void FreeCollisionObject( pxCollideable::Handle hCollisionObject );
	//pxCollideable& GetCollisionObject( pxCollideable::Handle hCollisionObject );

	void CastRay( const WorldRayCastInput& input, WorldRayCastOutput &output );


	void LinearCast( const ConvexCastInput& input, ConvexCastOutput &output );

	void TraceBox( const TraceBoxInput& input, TraceBoxOutput &output );

	// Rigid body simulation

	pxRigidBody::Handle AddRigidBody( const pxRigidBodyInfo& cInfo );
	void FreeRigidBody( pxRigidBody::Handle hRigidBody );
	pxRigidBody& GetRigidBody( pxRigidBody::Handle hRigidBody );
	UINT NumRigidBodies() const;

	// removes all bodies from this world
	void Clear();

	void Tick(
		pxReal deltaTime,
		pxReal fixedStepSize = PX_DEFAULT_FIXED_STEP_SIZE,
		pxInt maxSubSteps = 1
	);

	void SetGravity( const Vec3D& newGravity );
	pxVec3& GetGravity();

	pxConstraintSolver* GetSolver() {return m_constraintSolver;}

	void DebugDraw( pxDebugDrawer* renderer );

public_internal:
	void Serialize( mxArchive& archive );

private:
	void TickInternal( pxReal deltaTime );

	void Collide();

	// advances simulation forward by the given amount of time
	void IntegrateTransforms( pxReal deltaTime );

private:
	TPtr< pxBroadphase >			m_collisionBroadphase;
	TPtr< pxCollisionDispatcher >	m_collisionDispatcher;
	pxContactCache					m_contactCache;

	// collision objects
	//CollideableList		m_collideables;

	TPtr< pxConstraintSolver >		m_constraintSolver;

	pxVec3	m_gravityAcceleration;
	pxReal	m_timeAccumulator;	// delta time accumulated across previous frames

	// non-sleeping bodies
	RigidBodyList	m_rigidBodies;

private:	PREVENT_COPY(pxWorld);
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

pxRigidBody::Handle pxUtil_AddStaticPlane( pxWorld* physicsWorld, const Plane3D& plane );
pxRigidBody::Handle pxUtil_AddSphere( pxWorld* physicsWorld, FLOAT radius, FLOAT density = 1.0f );

#endif // !__PX_PHYSICS_WORLD_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
