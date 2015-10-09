#include "Game_PCH.h"
#pragma hdrstop

#include <Engine/Worlds.h>

#include <Game/Player/GamePlayer.h>
#include <Game/Player/PlayerPhysics.h>

static const F4 DefaultMovementVelocity = 5.0f;

static const F4 DefaultRotationVelocity = 0.01f;

const F4 air_friction = 1.0f;	// between [0..1]
const F4 water_friction = 1.0f;	// between [0..1]
const F4 ground_friction = 1.0f;	// between [0..1]

const float OVERCLIP = 1.00005f;

const AABB player_stand_bounds(
							   Vec3D(-1,-1,-2),
							   Vec3D(+1,+1,+3)
							   );

const AABB player_crouch_bounds(
							   Vec3D(-1,-1,-2)*0.3f,
							   Vec3D(+1,+1,+1.5f)*0.3f
							   );


static F4 GetPitchDelta( F4 fPitchDelta )
{
	const bool m_bInvertPitch = false;

	// Invert pitch if requested

	return m_bInvertPitch ? -fPitchDelta : fPitchDelta;
}

/*
-----------------------------------------------------------------------------
	GamePlayerController
-----------------------------------------------------------------------------
*/
GamePlayerController::GamePlayerController( Player* player )
{
	m_player = player;
	Assert(m_player != nil);

	this->ResetView();
}

void GamePlayerController::ResetView()
{
	ZERO_OUT( m_pressedKeys );

	m_rotationScaler		= DefaultRotationVelocity;
	m_movementScaler		= DefaultMovementVelocity;
	m_linearVelocity		.SetZero();
	m_velocityDrag			.SetZero();
	m_totalDragTimeToZero	= 0.15f;//0.25f;
	m_dragTimer				= 0.0f;
	m_rotationVelocity		.SetZero();
	m_yawAngle				= 0.0f;
	m_pitchAngle			= 0.0f;
	m_mouseDelta			.SetZero();

	m_view.nearZ = 0.1f;
	m_view.farZ = 1e3f;
}

void GamePlayerController::UpdateView( F4 deltaSeconds )
{
	// Update the camera rotation

	m_rotationVelocity = m_mouseDelta * m_rotationScaler;

	//m_mouseDelta.SetZero();
	m_mouseDelta *= 0.5f;


	// Update the pitch & yaw angle based on mouse movement
	const F4 fYawDelta = m_rotationVelocity.x;
	const F4 fPitchDelta = GetPitchDelta( m_rotationVelocity.y );

	m_pitchAngle += fPitchDelta;
	m_yawAngle += fYawDelta;

	// Limit pitch to straight up or straight down
	m_pitchAngle = clampf( m_pitchAngle, -MX_HALF_PI, +MX_HALF_PI );



	// Make a rotation matrix based on the camera's yaw & pitch.
	const float4x4	cameraRotation = XMMatrixRotationRollPitchYaw( m_pitchAngle, m_yawAngle, 0.0f );
	const float4	vWorldRight = XMVector3TransformNormal( g_XMIdentityR0, cameraRotation );
	const float4	vWorldUp = XMVector3TransformNormal( g_XMIdentityR1, cameraRotation );
	const float4	vWorldAhead = XMVector3TransformNormal( g_XMIdentityR2, cameraRotation );

	m_view.right = as_vec4( vWorldRight ).ToVec3().GetNormalized();
	m_view.up = as_vec4( vWorldUp ).ToVec3().GetNormalized();
	m_view.look = as_vec4( vWorldAhead ).ToVec3().GetNormalized();




	// Update the camera position

	Vec3D vKeyboardDirection(0.0f);// Direction vector of keyboard input

	if( m_pressedKeys[MA_MoveForward] )
	{
		vKeyboardDirection.z += 1.0f;
	}
	if( m_pressedKeys[MA_MoveBackward] )
	{
		vKeyboardDirection.z -= 1.0f;
	}

	if( m_pressedKeys[MA_StrafeLeft] )
	{
		vKeyboardDirection.x -= 1.0f;
	}
	if( m_pressedKeys[MA_StrafeRight] )
	{
		vKeyboardDirection.x += 1.0f;
	}

	if( m_pressedKeys[MA_MoveUp] )
	{
		vKeyboardDirection.y += 1.0f;
	}
	if( m_pressedKeys[MA_MoveDown] )
	{
		vKeyboardDirection.y -= 1.0f;
	}


	Vec3D vAccel = vKeyboardDirection;
	const FLOAT mag = vAccel.LengthSqr();
	if( mag > VECTOR_EPSILON )
	{
		vAccel *= mxInvSqrt(mag);
	}
	else
	{
		vAccel.SetZero();
	}



	// Scale the acceleration vector
	vAccel *= m_movementScaler;

	bool bMovementDrag = 1;        // If true, then camera movement will slow to a stop otherwise movement is instant
	if( bMovementDrag )
	{
		// Is there any acceleration this frame?
		const FLOAT lengthSq = vAccel.LengthSqr();
		if( lengthSq > 0.0f )
		{
			// If so, then this means the user has pressed a movement key
			// so change the velocity immediately to acceleration 
			// upon keyboard input.  This isn't normal physics
			// but it will give a quick response to keyboard input
			m_linearVelocity = vAccel;
			m_dragTimer = m_totalDragTimeToZero;
			m_velocityDrag = vAccel / m_dragTimer;
		}
		else
		{
			// If no key being pressed, then slowly decrease velocity to 0
			if( m_dragTimer > 0 )
			{
				// Drag until timer is <= 0
				m_linearVelocity -= m_velocityDrag * deltaSeconds;
				m_dragTimer -= deltaSeconds;
			}
			else
			{
				// Zero velocity
				m_linearVelocity = Vec3D( 0, 0, 0 );
			}
		}
	}
	else
	{
		// No drag, so immediately change the velocity
		m_linearVelocity = vAccel;
	}


	if( g_cvar_clip_player_movement )
	{
		pxWorld* physicsWorld = m_player->m_containingWorld->GetPhysicsWorld();

#if 1
		// calculate position we are trying to move to

		const Vec3D deltaPositionLocal = m_linearVelocity * deltaSeconds;
		const Vec3D deltaPositionWorld = as_matrix4( cameraRotation ).TransformNormal( deltaPositionLocal );
		const Vec3D predictedPosition = m_view.origin + deltaPositionWorld;

		// see if we can make it there

		TraceBoxInput		castInput;
		castInput.start = m_view.origin;
		castInput.end = predictedPosition;
		castInput.size = player_stand_bounds;
		//castInput.radius = 1.0f;

		TraceBoxOutput	castOutput;
		physicsWorld->TraceBox( castInput, castOutput );

		// modify velocity to satisfy contact constraints

		Vec3D	velocityWorld = as_matrix4( cameraRotation ).TransformNormal( m_linearVelocity );

		// clip the movement delta and velocity, slide along the contact plane

		velocityWorld.ProjectOntoPlane( castOutput.hitNormal, OVERCLIP );

		m_linearVelocity = as_matrix4( cameraRotation ).InverseTransformNormal( velocityWorld );

		m_view.origin += velocityWorld * deltaSeconds;
		//m_view.origin = castOutput.hitPosition;

#else
		UNDONE;
		Vec3D	velocityWorld = as_matrix4( cameraRotation ).TransformNormal( m_linearVelocity );

		Vec3D	origin = m_view.origin;

		PlayerPhysics_SlideMove( origin, velocityWorld, physicsWorld, deltaSeconds );

		m_linearVelocity = as_matrix4( cameraRotation ).InverseTransformNormal( velocityWorld );

		//m_view.origin = origin + velocityWorld * deltaSeconds;
		//m_view.origin += velocityWorld * deltaSeconds;
		m_view.origin = origin;
#endif
	}
	else
	{
		const Vec3D vPosDelta = m_linearVelocity * deltaSeconds;
		const Vec3D deltaPositionWorld = as_matrix4(cameraRotation).TransformVector(vPosDelta);
		const Vec3D predictedPosition = m_view.origin + deltaPositionWorld;

		m_view.origin = predictedPosition;
	}
}

void GamePlayerController::SetView( const rxView& camera )
{
	m_view = camera;
}

void GamePlayerController::SetAspectRatio( FLOAT newAspectRatio )
{
	m_view.aspectRatio = newAspectRatio;
}

const rxView& GamePlayerController::GetView() const
{
	return m_view;
}

void GamePlayerController::MoveForward()
{
	UNDONE;
	//m_accumVelocity += 1.0f;
}

void GamePlayerController::MoveBackward()
{
	UNDONE;
	//m_accumVelocity -= 1.0f;
}

void GamePlayerController::StrafeLeft()
{
	UNDONE;
	//m_accumStrafeVelocity -= 1.0f;
}

void GamePlayerController::StrafeRight()
{
	UNDONE;
	//m_accumStrafeVelocity += 1.0f;
}

void GamePlayerController::JumpUp()
{
	UNDONE;
	//m_accumJumpVelocity += 1.0f;
}

void GamePlayerController::Crouch()
{
	UNDONE;
	//m_accumJumpVelocity -= 1.0f;
}

bool GamePlayerController::CanJump() const
{
	UNDONE;
	//return HasGroundContacts()
	//	&& (GWorld->GetTime() - m_uLastTimeJumped > 1250);
	return true;
}

void GamePlayerController::OnKeyPressed( EKeyCode key )
{
	const EMovementAction mappedKey = MapKeyToAction( key );
	if( mappedKey != MA_Unknown )
	{
		m_pressedKeys[ mappedKey ] = 1;
	}

	if( key == EKeyCode::Key_R )
	{
		m_view.origin.SetAll(1);
	}
}

void GamePlayerController::OnKeyReleased( EKeyCode key )
{
	const EMovementAction mappedKey = MapKeyToAction( key );
	if( mappedKey != MA_Unknown )
	{
		m_pressedKeys[ mappedKey ] = 0;
	}
}

void GamePlayerController::OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed )
{
	if( pressed )
	{
		if( btn == EMouseButton::LeftMouseButton )
		{
			Unimplemented_Checked;
		}
	}
}

void GamePlayerController::OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY )
{
	const bool bSmoothMouseMovement = 0;

	const float mouseSensitivityX = 1.f;
	const float mouseSensitivityY = 1.f;

	const float fMouseDeltaX = mouseDeltaX * mouseSensitivityX;
	const float fMouseDeltaY = mouseDeltaY * mouseSensitivityY;

	if( bSmoothMouseMovement )
	{
		if(0)
		{
			const float blend = 0.5f;
			m_mouseDelta.x = Lerp( m_mouseDelta.x, fMouseDeltaX, blend );
			m_mouseDelta.y = Lerp( m_mouseDelta.y, fMouseDeltaY, blend );
		}
		else
		{
			// Smooth the relative mouse data over a few frames so it isn't 
			// jerky when moving slowly at low frame rates.

			// Number of frames to smooth mouse data over
			const float numFramesToSmoothMouseData = 2;

			float fPercentOfNew = 1.0f / numFramesToSmoothMouseData;
			float fPercentOfOld = 1.0f - fPercentOfNew;
			m_mouseDelta.x = m_mouseDelta.x * fPercentOfOld + fMouseDeltaX * fPercentOfNew;
			m_mouseDelta.y = m_mouseDelta.y * fPercentOfOld + fMouseDeltaY * fPercentOfNew;
		}
	}
	else
	{
		m_mouseDelta.x = fMouseDeltaX;
		m_mouseDelta.y = fMouseDeltaY;
	}
}

void GamePlayerController::OnMouseWheel( int scroll )
{
}

GamePlayerController::EMovementAction GamePlayerController::MapKeyToAction( EKeyCode key )
{
	switch( key )
	{
	case EKeyCode::Key_W :	return EMovementAction::MA_MoveForward;
	case EKeyCode::Key_S :	return EMovementAction::MA_MoveBackward;
	case EKeyCode::Key_A :	return EMovementAction::MA_StrafeLeft;
	case EKeyCode::Key_D :	return EMovementAction::MA_StrafeRight;
	case EKeyCode::Key_C :	return EMovementAction::MA_MoveDown;
	case EKeyCode::Key_Space :	return EMovementAction::MA_MoveUp;
	//case EKeyCode::Key_R :	return EMovementAction::MA_ResetToDefaults;
	}
	return EMovementAction::MA_Unknown;
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
