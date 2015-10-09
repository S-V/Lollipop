/*
=============================================================================
	File:	pxWorld.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Convex.h>
#include <Physics/Collide/Shape/pxShape_HalfSpace.h>
#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Collide/Shape/pxShape_Box.h>
#include <Physics/Collide/Shape/pxShape_StaticBSP.h>

/*
-----------------------------------------------------------------------------
	pxWorldCreationInfo
-----------------------------------------------------------------------------
*/
void pxWorldCreationInfo::setDefaults()
{
	broadphase = nil;
	dispatcher = Physics::GetCollisionDispatcher();

	constraintSolver = nil;

	gravity = pxVec3( 0.0f, -Physik::EARTH_GRAVITY, 0.0f );
}

bool pxWorldCreationInfo::isOk() const
{
	return 1
		&& CHK(broadphase)
		&& CHK(dispatcher)

		&& CHK(constraintSolver)
		;
}

/*
-----------------------------------------------------------------------------
	pxWorld
-----------------------------------------------------------------------------
*/
pxWorld::pxWorld( const pxWorldCreationInfo& worldDesc )
{
	Assert(worldDesc.isOk());

	m_collisionBroadphase = worldDesc.broadphase;
	m_collisionDispatcher = worldDesc.dispatcher;

	m_constraintSolver = worldDesc.constraintSolver;

	m_gravityAcceleration = worldDesc.gravity;

	m_timeAccumulator = 0.0f;
}

pxWorld::~pxWorld()
{
}
//
//pxCollideable::Handle pxWorld::AddCollisionObject( const pxCollideableInfo& cInfo )
//{
//	const pxCollideable::Handle hNewCollideable = m_collideables.Add();
//
//	pxCollideable& rNewCollideable = m_rigidBodies.GetPtrByHandle( hNewCollideable );
//	rNewCollideable._ctor( cInfo );
//
//	m_collisionBroadphase->Add( &rNewCollideable );
//
//	return hNewCollideable;
//}
//
//void pxWorld::FreeCollisionObject( pxCollideable::Handle hCollisionObject )
//{
//	UNDONE;
//}
//
//pxCollideable& pxWorld::GetCollisionObject( pxCollideable::Handle hCollisionObject )
//{
//	UNDONE;
//}
//
void pxWorld::CastRay( const WorldRayCastInput& input, WorldRayCastOutput &output )
{
	Unimplemented;
}

//void pxWorld::LinearCast( const ConvexCastInput& input, ConvexCastOutput &output )
//{
//	m_collisionBroadphase->LinearCast( input, output );
//}

void pxWorld::TraceBox( const TraceBoxInput& input, TraceBoxOutput &output )
{
	m_collisionBroadphase->TraceBox( input, output );
}

pxRigidBody::Handle pxWorld::AddRigidBody( const pxRigidBodyInfo& cInfo )
{
	const pxRigidBody::Handle hNewRigidBody = m_rigidBodies.Add();

	pxRigidBody* pNewRigidBody = m_rigidBodies.GetPtrByHandle( hNewRigidBody );
	pNewRigidBody->_ctor( cInfo );

	m_collisionBroadphase->Add( pNewRigidBody );

	return hNewRigidBody;
}

void pxWorld::FreeRigidBody( pxRigidBody::Handle hRigidBody )
{
	UNDONE;	//see pxSimpleBroadphaseProxy::o - invalid when they are moved
	// and collision agents, etc.

	//pxRigidBody& rRigidBody = m_rigidBodies.GetPtrByHandle( hRigidBody );

	//m_collisionBroadphase->Remove( &rRigidBody );

	//m_rigidBodies.Remove( hRigidBody );
}

pxRigidBody& pxWorld::GetRigidBody( pxRigidBody::Handle hRigidBody )
{
	return *m_rigidBodies.GetPtrByHandle( hRigidBody );
}

UINT pxWorld::NumRigidBodies() const
{
	return m_rigidBodies.Num();
}

void pxWorld::Tick( pxReal deltaTime, pxReal fixedStepSize, pxInt maxSubSteps )
{
	Assert(deltaTime>=0.0f);
	Assert(maxSubSteps > 0);

	// it's recommended to use fixed time step for better accuracy/stability
	bool bUseFixedTimeStep = 1;

	if( bUseFixedTimeStep )
	{
		int numSubSteps = 0;	// number of fixed simulation time steps

		m_timeAccumulator += deltaTime;

		if( m_timeAccumulator >= fixedStepSize )
		{
			numSubSteps = m_timeAccumulator / fixedStepSize;
			m_timeAccumulator -= numSubSteps * fixedStepSize;
		}

		if( numSubSteps > 0 )
		{
			//clamp the number of substeps, to prevent simulation grinding spiralling down to a halt
			numSubSteps = smallest( numSubSteps, maxSubSteps );

			for( int iFixedSubStep = 0; iFixedSubStep < numSubSteps; iFixedSubStep++ )
			{
				this->TickInternal( fixedStepSize );
			}
		}
		else
		{
			// interpolate
			this->IntegrateTransforms( deltaTime );
		}

		//DBGOUT("%d substeps, dt = %.3f\n",numSubSteps,deltaTime);
	}
	else
	{
		// variable time step

		this->TickInternal( deltaTime );
	}
}

void pxWorld::TickInternal( pxReal deltaTime )
{
	PX_PROFILE("pxWorld::TickInternal");

//	const pxReal halfStep = pxReal(0.5) * deltaTime;
//	const pxReal realStep = halfStep;
	const pxReal realStep = deltaTime;

	// apply external forces, integrate velocities, predict unconstrained motion
	{
		const UINT numRigidBodies = m_rigidBodies.Num();
		pxRigidBody* rigidBodies = m_rigidBodies.ToPtr();

		for( UINT iBody = 0; iBody < numRigidBodies; iBody++ )
		{
			pxRigidBody * body = rigidBodies + iBody;

			// apply external forces (such as gravity)
			//if( ! body->IsStatic() )
			{
				pxVec3 gravityForce = m_gravityAcceleration * body->GetMass();
				body->ApplyCentralForce( gravityForce );
			}

			// integrate velocities
			body->IntegrateVelocities( deltaTime );
			body->ClampVelocities( deltaTime );
			body->ApplyDamping( deltaTime );

			// integrate transforms, predict positions
			body->IntegratePositions( deltaTime );
			body->UpdateWorldInertiaTensor();
		}
	}

	// perform collision detection and generate contacts
	{
		this->Collide();
	}

	// solve constraints
	if(1)
	{
		PX_PROFILE("Solve constraints");
		PX_SCOPED_COUNTER(gPhysStats.solveConstraintsMs);

		pxSolverInput	solverInput;
		pxSolverOutput	solverOutput;

		solverInput.deltaTime = realStep;
		solverInput.bodies = m_rigidBodies.ToPtr();
		solverInput.numBodies = m_rigidBodies.Num();
		//	solverInput.constraints = mActiveConstraints.Ptr();
		//	solverInput.numConstraints = mActiveConstraints.Num();
		solverInput.contacts = &m_contactCache;

		m_constraintSolver->Solve( solverInput, solverOutput );
	}

	// advance positions
	{
		this->IntegrateTransforms( realStep );
	}
}

void pxWorld::Collide()
{
	PX_PROFILE("Collision detection");
	PX_SCOPED_COUNTER(gPhysStats.collisionDetectionMs);
	{
		PX_PROFILE("Broadphase collision detection");
		PX_SCOPED_COUNTER(gPhysStats.broadphaseMs);
		m_collisionBroadphase->Collide( *m_collisionDispatcher );
	}
	{
		PX_SCOPED_COUNTER(gPhysStats.narrowphaseMs);
		PX_PROFILE("Nearphase collision detection");
		m_collisionDispatcher->Collide( m_contactCache );
	}
}

void pxWorld::IntegrateTransforms( pxReal deltaTime )
{
	PX_PROFILE("pxWorld::IntegrateTransforms");
	PX_SCOPED_COUNTER(gPhysStats.integrateMs);

	const UINT numRigidBodies = m_rigidBodies.Num();
	pxRigidBody* rigidBodies = m_rigidBodies.ToPtr();

	for( UINT iBody = 0; iBody < numRigidBodies; iBody++ )
	{
		pxRigidBody * body = rigidBodies + iBody;

		// advance positions
		body->IntegratePositions( deltaTime );
		body->UpdateWorldInertiaTensor();
	}
}

void pxWorld::Clear()
{
	m_collisionBroadphase->Clear();
	m_collisionDispatcher->Clear();
	m_contactCache.Clear();

	m_rigidBodies.Clear();
}

void pxWorld::SetGravity( const Vec3D& newGravity )
{
	m_gravityAcceleration.Set( newGravity.ToFloatPtr() );
}

pxVec3& pxWorld::GetGravity()
{
	return m_gravityAcceleration;
}

void pxWorld::Serialize( mxArchive& archive )
{
	archive && m_gravityAcceleration;
	archive && m_timeAccumulator;

	archive && m_rigidBodies;

	if( archive.IsReading() )
	{
		//@todo: serialize broadphase?
		m_collisionBroadphase->Clear();

		const UINT numRigidBodies = m_rigidBodies.Num();
		pxRigidBody* rigidBodies = m_rigidBodies.ToPtr();

		m_collisionBroadphase->Reserve( numRigidBodies );

		for( UINT iBody = 0; iBody < numRigidBodies; iBody++ )
		{
			pxRigidBody * body = rigidBodies + iBody;

			m_collisionBroadphase->Add( body );
		}
	}
}

#if PX_ENABLE_DEBUG_DRAW

void pxWorld::DebugDraw( pxDebugDrawer* renderer )
{
	m_collisionBroadphase->DebugDraw( renderer );

	const UINT numRigidBodies = m_rigidBodies.Num();
	pxRigidBody* rigidBodies = m_rigidBodies.ToPtr();

	for( UINT iBody = 0; iBody < numRigidBodies; iBody++ )
	{
		pxRigidBody * body = rigidBodies + iBody;

		pxAABB	aabb;
		body->GetWorldBounds( aabb );

		renderer->drawBox( aabb.mMin, aabb.mMax, FColor::GREEN.mSimdQuad );
	}
}

#endif // PX_ENABLE_DEBUG_DRAW

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

pxRigidBody::Handle pxUtil_AddStaticPlane( pxWorld* physicsWorld, const Plane3D& plane )
{
	pxShape::Handle staticPlainShape = Physics::AddCollisionShape( new pxShape_HalfSpace( plane ) );

	pxRigidBodyInfo		staticPlainDesc;
	staticPlainDesc.shape = staticPlainShape;

	pxRigidBody::Handle	staticPlaneRB = physicsWorld->AddRigidBody( staticPlainDesc );
	return staticPlaneRB;
}

pxRigidBody::Handle pxUtil_AddSphere( pxWorld* physicsWorld, FLOAT radius, FLOAT density )
{
	pxShape::Handle sphereShape = Physics::AddCollisionShape( new pxShape_Sphere( radius ) );

	pxRigidBodyInfo		sphereRBDesc;
	{
		sphereRBDesc.shape = sphereShape;
		sphereRBDesc.mass = ((4.0f/3.0f)*MX_PI) * (cubef(radius) * density);
	}

	pxRigidBody::Handle	sphereRB = physicsWorld->AddRigidBody( sphereRBDesc );
	return sphereRB;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
