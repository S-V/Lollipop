/*
=============================================================================
	File:	pxRigidBody.h
	Desc:	
	ToDo:	SoA layout, no rigid body class, body = integer id
=============================================================================
*/

#ifndef __PX_RIGID_BODY_H__
#define __PX_RIGID_BODY_H__

#include <Physics/Collide/Agent/pxCollideable.h>

#define PX_USE_APPROX_DAMPING	1

// linear speed threshold
const pxReal PX_RB_MAX_LINEAR_VELOCITY = REAL(200.);

// angular speed threshold - limit the angular velocity to this magnitude.
// High rotational velocities can lead to problems because certain linear approximations of the rotational motion fail to hold.
const pxReal PX_RB_MAX_ANGULAR_VELOCITY = MX_HALF_PI * REAL(0.5);

const pxReal PX_RB_MAX_FORCE = REAL(1e5);

//
// pxClampBodyVelocities
//
inline void pxClampBodyVelocities(
	pxVec3 & linearVelocity, pxVec3 & angularVelocity,
	pxReal deltaTime )
{
	// clamp linear velocity
	if( linearVelocity.LengthSqr() > Square(PX_RB_MAX_LINEAR_VELOCITY) )
	{
		linearVelocity *= mxReciprocal( PX_RB_MAX_LINEAR_VELOCITY );
	}
	// clamp angular velocity. collision calculations will fail on high angular velocities.
	const pxReal angVelLen = angularVelocity.Length();
	if( angVelLen * deltaTime > PX_RB_MAX_ANGULAR_VELOCITY )
	{
		angularVelocity *= (PX_RB_MAX_ANGULAR_VELOCITY / deltaTime) / angVelLen;
	}
}

// damping parameters
class pxDampingParameters
{
public:
//	pxReal	linearThreshold;   // linear (squared) average speed threshold
//	pxReal	angularThreshold;  // angular (squared) average speed threshold
	pxReal	linearDamping;  // translational friction, multiply the linear velocity by (1 - scale)
	pxReal	angularDamping; // rotational friction, multiply the angular velocity by (1 - scale)

public:
	pxDampingParameters()
	{
		setDefaults();
	}
	void setDefaults()
	{
		linearDamping = REAL(0.5);
		angularDamping = REAL(0.5);
	//	linearThreshold = Square(PX_RB_MAX_LINEAR_VELOCITY);
	//	angularThreshold = Square(PX_RB_MAX_ANGULAR_VELOCITY);
	}
};

//
// pxApplyDampingToBodyVelocities
//
inline void pxApplyDampingToBodyVelocities(
	pxVec3 & linearVelocity, pxVec3 & angularVelocity,
	const pxDampingParameters& damping,
	pxReal deltaTime )
{
#if 1
	linearVelocity *= REAL(1.) - damping.linearDamping * deltaTime;
	angularVelocity *= REAL(1.) - damping.angularDamping * deltaTime;
#elif 0
	if( linearVelocity.LengthSqr() > damping.linearThreshold )
	{
		linearVelocity *= REAL(1.) - damping.linearDamping * deltaTime;
	}
	if( angularVelocity.LengthSqr() > damping.angularThreshold )
	{
		angularVelocity *= REAL(1.) - damping.angularDamping * deltaTime;
	}
#else
	linearVelocity *= mxPow( pxReal(1.) - damping.linearDamping, deltaTime );
	angularVelocity *= mxPow( pxReal(1.) - damping.angularDamping, deltaTime );
#endif
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



//
//	pxRigidBodyInfo
//
struct pxRigidBodyInfo : pxCollideableInfo
{
	pxReal		mass;

public:
	pxRigidBodyInfo()
	{
		setDefaults();
	}
	void setDefaults()
	{
		pxCollideableInfo::setDefaults();

		mass = 0.0f;
	}
	bool isOk() const
	{
		return pxCollideableInfo::isOk()
			;
	}
};

/*
-----------------------------------------------------------------------------
	pxRigidBody

	Represents a single rigid body.
	Contains information needed to represent a rigid body,
	including collision data,
	position and velocity (both linear and angular),
	mass and inertia matrix.

	this should a POD type (and as small as possible)

	@todo: it should not inherit from pxCollideable
-----------------------------------------------------------------------------
*/
MX_ALIGN_64(struct) pxRigidBody : pxCollideable
{
	pxMat3x3	mInvInertiaTensorWorld;	//«48 inverse inertia matrix in world space
	pxVec3		mInvInertiaTensorLocal;	//«16 inverse of the (diagonal) moment of inertia in body's local space

	pxVec3		mLinearVelocity;	//«16 center of mass velocity (linear units per time unit)
	pxVec3		mAngularVelocity;	//«16 angular velocity (radians per time unit around each axis)

	pxVec3		mTotalForce;	//«16 force accumulator (cumulative force)
	pxVec3		mTotalTorque;	//«16 torque accumulator (cumulative torque)

	pxReal		mInverseMass;	//«4 inverse of the total mass
	pxReal		mMass;			//«4 total mass of the rigid body

	pxReal		mLinearDamping;	//«4 translational friction
	pxReal		mAngularDamping;//«4 rotational friction

	//pxCollideable::Handle	mCollisionData;

	//@todo: remove this:
	pxU4		m_solverIndex;	//«4 offset for indexing into sparse matrices (set by constraint solver)

public:
	typedef OID Handle;
	enum { NullHandle = -1 };

	void ApplyForce( const pxVec3& force, const pxVec3& relativePoint );
	void ApplyCentralForce( const pxVec3& force );
	void ApplyTorque( const pxVec3& torque );

	const pxVec3& GetLinearVelocity() const;
	void SetLinearVelocity( const pxVec3& newLinearVelocity );
	
	const pxVec3& GetAngularVelocity() const;
	void SetAngularVelocity( const pxVec3& newAngularVelocity );

	pxVec3 GetVelocityInLocalPoint( const pxVec3& relativePosition ) const;

	// Gets the velocity of point p on the rigid body in world space.
	void GetPointVelocity( const pxVec3& point, pxVec3& vecOut ) const;

	void ApplyCentralImpulse( const pxVec3& impulse );
	void ApplyImpulse( const pxVec3& impulse, const pxVec3& relativePosition ) ;
	void ApplyTorqueImpulse( const pxVec3& torque );


	void SetMassProperties( pxReal newMass, const pxMat3x3& newInertiaLocal );

	// Gets the inverse inertia tensor in world space.
	const pxMat3x3& GetInvInertiaWorld() const;

	// Sets the inertia tensor of the rigid body.
	void SetInertiaLocal( const pxMat3x3& newInertia );

	// Sets the inertia tensor of the rigid body by supplying its inverse.
	void SetInertiaInvLocal( const pxMat3x3& newInvInertia );

	void SetMass( pxReal newMass );
	pxReal GetMass() const;
	pxReal GetInvMass() const;


	bool IsStatic() const;
	bool IsMovable() const;

public_internal:

	PX_DECLARE_POD_ALLOCATOR( pxRigidBody, PX_MEMORY_DYNAMICS );

	pxRigidBody();
	~pxRigidBody();

	void _ctor( const pxRigidBodyInfo& cInfo );
	void _dtor();

	// Gets the force accumulator.
	const pxVec3& GetTotalForce() const;
	
	// Gets the torque accumulator.
	const pxVec3& GetTotalTorque() const;

	// Clears force accumulators.
	void ClearForces();



	void ClampVelocities( pxReal deltaTime );
	void ApplyDamping( pxReal deltaTime );

	void IntegrateVelocities( pxReal deltaTime );
	void IntegratePositions( pxReal deltaTime );


	// Recalculates world-space inertia tensor based on current local-to-world transform.
	void UpdateWorldInertiaTensor();


	pxReal ComputeImpulseDenominator( const pxVec3& pos, const pxVec3& normal ) const;
	pxReal ComputeAngularImpulseDenominator( const pxVec3& axis ) const;
};

MX_DECLARE_POD_TYPE( pxRigidBody );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PX_INLINE
void pxRigidBody::ApplyForce( const pxVec3& force, const pxVec3& relativePoint )
{
	this->ApplyCentralForce( force );
	this->ApplyTorque( relativePoint.Cross( force ) );
}

PX_INLINE
void pxRigidBody::ApplyCentralForce( const pxVec3& force ) {
	mTotalForce += force;
}

PX_INLINE
void pxRigidBody::ApplyTorque( const pxVec3& torque ) {
	mTotalTorque += torque;
}

PX_INLINE
void pxRigidBody::ClearForces()
{
	mTotalForce.SetZero();
	mTotalTorque.SetZero();
}

PX_INLINE
const pxVec3& pxRigidBody::GetLinearVelocity() const {
	return mLinearVelocity;
}

PX_INLINE
void pxRigidBody::SetLinearVelocity( const pxVec3& newLinearVelocity ) {
	mLinearVelocity = newLinearVelocity;
}

PX_INLINE
const pxVec3& pxRigidBody::GetAngularVelocity() const {
	return mAngularVelocity;
}

PX_INLINE
void pxRigidBody::SetAngularVelocity( const pxVec3& newAngularVelocity ) {
	mAngularVelocity = newAngularVelocity;
}

PX_INLINE
pxVec3 pxRigidBody::GetVelocityInLocalPoint( const pxVec3& relativePosition ) const
{
	return mLinearVelocity + mAngularVelocity.Cross( relativePosition );
}

PX_INLINE
void pxRigidBody::ApplyCentralImpulse( const pxVec3& impulse )
{
	mLinearVelocity += impulse * mInverseMass;
}

PX_INLINE
void pxRigidBody::ApplyImpulse( const pxVec3& impulse, const pxVec3& relativePosition ) 
{
	Assert(this->IsMovable());
	this->ApplyCentralImpulse( impulse );
	this->ApplyTorqueImpulse( relativePosition.Cross( impulse ) );
}

PX_INLINE
void pxRigidBody::ApplyTorqueImpulse( const pxVec3& torque )
{
	mAngularVelocity += mInvInertiaTensorWorld * torque;
}

PX_INLINE
void pxRigidBody::ClampVelocities( pxReal deltaTime )
{
	pxClampBodyVelocities( mLinearVelocity, mAngularVelocity, deltaTime );
}

PX_INLINE
void pxRigidBody::ApplyDamping( pxReal deltaTime )
{
#if PX_USE_APPROX_DAMPING
	MX_SWIPED("comment from Box2D");
	// Apply damping.
	// ODE: dv/dt + c * v = 0
	// Solution: v(t) = v0 * exp(-c * t)
	// Time step: v(t + dt) = v0 * exp(-c * (t + dt)) = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
	// v2 = exp(-c * dt) * v1
	// Taylor expansion:
	// v2 = (1.0f - c * dt) * v1
	mLinearVelocity *= clampf( REAL(1.) - deltaTime * mLinearDamping, REAL(0.0), REAL(1.0) );
	mAngularVelocity *= clampf( REAL(1.) - deltaTime * mAngularDamping, REAL(0.0), REAL(1.0) );
#else
	mLinearVelocity *= mxPow( pxReal(1.) - mLinearDamping, deltaTime );
	mAngularVelocity *= mxPow( pxReal(1.) - mAngularDamping, deltaTime );
#endif
}

PX_INLINE
void pxRigidBody::SetMass( pxReal newMass )
{
	mMass = newMass;
	if( newMass == 0.0f ) {
		mInverseMass = 0.0f;
	} else {
		mInverseMass = 1.0f / newMass;
	}
}

PX_INLINE
pxReal pxRigidBody::GetMass() const {
	return mMass;
}

PX_INLINE
pxReal pxRigidBody::GetInvMass() const {
	return mInverseMass;
}

PX_INLINE
bool pxRigidBody::IsStatic() const {
	//return F32_EQUAL_0( mInverseMass );
	return mInverseMass == 0.0f;
}

PX_INLINE
bool pxRigidBody::IsMovable() const {
	return !this->IsStatic();
}

PX_INLINE
void pxRigidBody::UpdateWorldInertiaTensor()
{
	// Iworld = R * Ilocal * R^T
	mInvInertiaTensorWorld = m_transform.GetBasis()
								.GetScaled( mInvInertiaTensorLocal )
									* m_transform.GetBasis().GetTranspose();
}

PX_INLINE
const pxMat3x3& pxRigidBody::GetInvInertiaWorld() const {
	return mInvInertiaTensorWorld;
}

PX_INLINE
void pxRigidBody::SetInertiaLocal( const pxMat3x3& newInertia )
{
	Assert(newInertia.IsDiagonal());
	pxMat3x3	invInertiaTensorLocal = newInertia.GetInverse();
	this->SetInertiaInvLocal( invInertiaTensorLocal );
}

PX_INLINE
void pxRigidBody::SetInertiaInvLocal( const pxMat3x3& newInvInertia )
{
	Assert(newInvInertia.IsDiagonal());
	//mInvInertiaTensorLocal = invInertiaTensorLocal;
	mInvInertiaTensorLocal = newInvInertia.GetDiagonal();
}

PX_INLINE
const pxVec3& pxRigidBody::GetTotalForce() const
{
	return mTotalForce;
}

PX_INLINE
const pxVec3& pxRigidBody::GetTotalTorque() const
{
	return mTotalTorque;
}

PX_INLINE
pxReal pxRigidBody::ComputeImpulseDenominator( const pxVec3& pos, const pxVec3& normal ) const
{
	const pxVec3 r0 = pos - this->GetCenterOfMassPosition();
	const pxVec3 c0 = r0.Cross( normal );
	const pxVec3 vec = ( c0 * this->GetInvInertiaWorld() ).Cross( r0 );

	return mInverseMass + normal.Dot( vec );
}

PX_INLINE
pxReal pxRigidBody::ComputeAngularImpulseDenominator( const pxVec3& axis ) const
{
	const pxVec3 vec = axis * this->GetInvInertiaWorld();
	return axis.Dot( vec );
}

// only rigid bodies are supported/implemented
FORCEINLINE pxRigidBody* AsRigidBody( pxCollideable* o )
{
	return c_cast(pxRigidBody*) o;
}

#endif // !__PX_RIGID_BODY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
