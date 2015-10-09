#pragma once

#include <Core/App/Client.h>

class Player;

//
//	EPlayerMovementType
//
enum EPlayerMovementType
{
	PM_NORMAL,				// normal physics
	PM_DEAD,				// no acceleration or turning, but free falling
	PM_SPECTATOR,			// flying without gravity but with collision detection
	PM_FREEZE,				// stuck in place without control
	PM_NOCLIP				// flying without collision detection nor gravity
};


typedef enum {
	WATERLEVEL_NONE,
	WATERLEVEL_FEET,
	WATERLEVEL_WAIST,
	WATERLEVEL_HEAD
} waterLevel_t;


//
//	EPlayerMotionState
//
enum EPlayerMotionState
{
	Player_On_Ground,
	Player_In_Air,
};

//
//	SCharacterSettings
//
struct SCharacterSettings
{
	F4		mass;		// Specifies how much the character pushes down on objects it stands on.
	F4		strength;	// Specifies how much the character is able to push other objects around.
};


/*
-----------------------------------------------------------------------------
	GamePlayerController

	Simulates the motion of a player through the environment.Input from the
	player is used to allow a certain degree of control over the motion.
-----------------------------------------------------------------------------
*/
struct GamePlayerController : public InputClient
{
	GamePlayerController( Player* player );

	void ResetView();

	void UpdateView( F4 deltaSeconds );

	void SetView( const rxView& camera );

	void SetAspectRatio( FLOAT newAspectRatio );

	const rxView& GetView() const;

public:	// movement controls

	void MoveForward();
	void MoveBackward();
	void StrafeLeft();
	void StrafeRight();
	void JumpUp();
	void Crouch();

	bool CanJump() const;

public:	// functions for handling user input

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

	virtual void OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed ) override;
	virtual void OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY ) override;
	virtual void OnMouseWheel( int scroll ) override;

private:

	enum EMovementAction
	{
		MA_Unknown,
		//ResetToDefaults,
		MA_MoveForward,
		MA_MoveBackward,
		MA_MoveUp,
		MA_MoveDown,
		MA_StrafeLeft,
		MA_StrafeRight,
		MA_MAX
	};

	static EMovementAction MapKeyToAction( EKeyCode key );

private:
	TPtr<Player>	m_player;

	rxView	m_view;	// view parameters

	TStaticArray<U4,MA_MAX>	m_pressedKeys;	// state of input

	FLOAT	m_rotationScaler;      // Scaler for rotation
	FLOAT	m_movementScaler;          // Scaler for movement

	Vec3D	m_linearVelocity;	// linear velocity of camera in local space

	Vec2D	m_rotationVelocity;	// rotation velocity of camera
	FLOAT	m_yawAngle;	// in radians
	FLOAT	m_pitchAngle;	// in radians

	// for damping camera movement
	Vec3D	m_velocityDrag;        // velocity drag force
	FLOAT	m_totalDragTimeToZero; // Time it takes for velocity to go from full to 0, in seconds
	FLOAT	m_dragTimer;           // Countdown timer to apply drag, in seconds

	Vec2D	m_mouseDelta;			// Mouse relative delta
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
