/*
=============================================================================
	File:	pxRigidBody.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

FORCEINLINE F4 InverseOrZero( F4 x )
{
	return (x != 0.0f) ? (1.0f / x) : (0.0f);
}
static void CalcRigidBodyInertiaInvLocal( const pxRigidBodyInfo& cInfo, pxVec3 *invInertiaTensorLocal )
{
	pxVec3	localInertia;
	cInfo.shape.ToPtr()->CalculateInertiaLocal( cInfo.mass, localInertia );

	invInertiaTensorLocal->SetValue(
		InverseOrZero( localInertia.x ),
		InverseOrZero( localInertia.y ),
		InverseOrZero( localInertia.z )
		);
}

/*
-----------------------------------------------------------------------------
	pxRigidBody
-----------------------------------------------------------------------------
*/
pxRigidBody::pxRigidBody()
{
}

pxRigidBody::~pxRigidBody()
{
}

void pxRigidBody::_ctor( const pxRigidBodyInfo& cInfo )
{
	Assert( cInfo.isOk() );

	pxCollideable::_ctor( cInfo );

	mInvInertiaTensorWorld.SetZero();

	mInvInertiaTensorLocal.SetZero();
	CalcRigidBodyInertiaInvLocal( cInfo, &mInvInertiaTensorLocal );

	mInverseMass = 0.0f;
	mMass = 0.0f;
	this->SetMass( cInfo.mass );

	mLinearVelocity.SetZero();
	mLinearDamping = 0.1f;
	mAngularVelocity.SetZero();
	mAngularDamping = 0.5f;

	mTotalForce.SetZero();
	mTotalTorque.SetZero();
}

void pxRigidBody::_dtor()
{
	pxCollideable::_dtor();
}

void pxRigidBody::IntegrateVelocities( pxReal deltaTime )
{
	// newVelocity = oldVelocity + (acceleration * dt) = oldVelocity + ((Force/mass) * dt)
	mLinearVelocity += mTotalForce * mInverseMass * deltaTime;

	// angularVelocity = (invIworld * Torque) * dt;
	mAngularVelocity += mInvInertiaTensorWorld * mTotalTorque * deltaTime;
}

void pxRigidBody::IntegratePositions( pxReal deltaTime )
{
	pxTransform  predictedTransform;
	// Symplectic Euler integrator:
	// integrate position: T' = linearVelocity, T = To + T' * deltaTime
	predictedTransform.SetOrigin( m_transform.GetOrigin() + mLinearVelocity * deltaTime );

	// integrate orientation: Q' = (1/2) * angularVelocity * Q, Q = Qo + Q' * deltaTime
	pxQuat predictedOrn = m_transform.GetRotation();
	predictedOrn += ( mAngularVelocity * predictedOrn ) * ( deltaTime * pxReal(0.5) );
	predictedOrn.Normalize();
	predictedTransform.SetRotation( predictedOrn );

	m_transform = predictedTransform;
}

void pxRigidBody::SetMassProperties( pxReal newMass, const pxMat3x3& newInertiaLocal )
{
	this->SetMass( newMass );
	this->SetInertiaLocal( newInertiaLocal );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
