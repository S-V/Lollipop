/*
=============================================================================
	File:	Client.h
	Desc:	Client viewport interface.
	ToDo:	refactor (split Input/Sound/Graphics)
=============================================================================
*/

#ifndef __MX_CORE_CLIENT_H__
#define __MX_CORE_CLIENT_H__

#include <Core/Object.h>

mxNAMESPACE_BEGIN


/*
=============================================================================

	INPUT SYSTEM

=============================================================================
*/
mxSWIPED("parts swiped from CryEngine SDK");


enum EInputDevice
{
	InputDevice_Unknown = 0,
	InputDevice_Keyboard,
	InputDevice_Mouse,
	InputDevice_Joystick,
	InputDevice_Gamepad,
};
enum EInputEvent
{
	IE_Pressed = 0,
	IE_Released,
	IE_Repeat,
	IE_DoubleClick,
	IE_Axis,
};
enum EInputState
{
	eIS_Unknown		= 0,
	eIS_Pressed		= (1 << 0),
	eIS_Released	= (1 << 1),
	eIS_Down		= (1 << 2),
	eIS_Changed		= (1 << 3),
	eIS_UI			= (1 << 4),
};

// Key and button states for engine window input.
enum EButtonState : U1
{
	Button_NotPressed = 0,
	Button_kPressed,
	Button_kHeld
};

/*
-----------------------------------------------------------------------------
	SButtonState
-----------------------------------------------------------------------------
*/
class SButtonState
{
	TEnum<EButtonState,U1>	state;
	U1	isToggled;

public:
	FORCEINLINE SButtonState()
	{
		this->state = EButtonState::Button_NotPressed;
		this->isToggled = false;
	}

	FORCEINLINE EButtonState State() const
	{
		return this->state;
	}

	FORCEINLINE BOOL IsPressed() const
	{
		//return this->State() != EButtonState::Button_NotPressed;
		return this->State() == EButtonState::Button_kPressed;
	}
	FORCEINLINE BOOL IsHeld() const
	{
		return this->State() == EButtonState::Button_kHeld;
	}
	FORCEINLINE BOOL IsToggled() const
	{
		return this->isToggled;
	}

	FORCEINLINE void Press()
	{
		this->state = (Button_NotPressed == this->State())
			? Button_kPressed : Button_kHeld;
	}
	FORCEINLINE void Toggle()
	{
		this->isToggled = !this->isToggled;
	}

	FORCEINLINE void Release()
	{
		this->state = Button_NotPressed;
	}
};

/*
=============================================================================

	Keyboard

=============================================================================
*/
mxPERM("currently, engine key codes correspond to Windows virtual key codes");
//
//	EKeyCode - enumeration of logical key codes.
//
enum EKeyCode
{
	Key_Unknown		= 0,	//<= Unknown key code, don't use

	Mouse_LButton	= 0x01,  // Left mouse button
	Mouse_RButton	= 0x02,  // Right mouse button
	Mouse_MButton	= 0x04,  // Middle mouse button
	Mouse_XButton1	= 0x05,  // Windows 2000/XP: X1 mouse button
	Mouse_XButton2	= 0x06,  // Windows 2000/XP: X2 mouse button
	Key_Back		= 0x08,  // BACKSPACE key	
	Key_Tab			= 0x09,  // TAB key  
	Key_Clear		= 0x0C,  // CLEAR key	
	Key_Return		= 0x0D,  // ENTER key	
	Key_Shift		= 0x10,  // SHIFT key	
	Key_Control		= 0x11,  // CTRL key  
	Key_Menu		= 0x12,  // ALT key  
	Key_Pause		= 0x13,  // PAUSE key	
	Key_Capital		= 0x14,  // CAPS LOCK key	
	Key_Escape		= 0x1B,  // ESC key  
	Key_Convert		= 0x1C,  // IME convert 
	Key_Nonconvert	= 0x1D,  // IME nonconvert	
	Key_Accept		= 0x1E,  // IME accept	
	Key_ModeChange	= 0x1F,  // IME mode change request 
	Key_Space		= 0x20,  // SPACEBAR (whitespace)
	Key_Prior		= 0x21,  // PAGE UP key  
	Key_Next		= 0x22,  // PAGE DOWN key	
	Key_End			= 0x23,  // END key  
	Key_Home		= 0x24,  // HOME key  
	Key_Left		= 0x25,  // LEFT ARROW	key	 
	Key_Up			= 0x26,  // UP	ARROW key  
	Key_Right		= 0x27,  // RIGHT ARROW key  
	Key_Down		= 0x28,  // DOWN ARROW	key	 
	Key_Select		= 0x29,  // SELECT	key	 
	Key_Print		= 0x2A,  // PRINT key
	Key_Execute		= 0x2B,  // EXECUTE key  
	Key_Snapshot	= 0x2C,  // PRINT SCREEN key  
	Key_Insert		= 0x2D,  // INS key  
	Key_Delete		= 0x2E,  // DEL key  
	Key_Help		= 0x2F,  // HELP key  
	Key_0			= 0x30,  // 0 key	
	Key_1			= 0x31,  // 1 key	
	Key_2			= 0x32,  // 2 key	
	Key_3			= 0x33,  // 3 key	
	Key_4			= 0x34,  // 4 key	
	Key_5			= 0x35,  // 5 key	
	Key_6			= 0x36,  // 6 key	
	Key_7			= 0x37,  // 7 key	
	Key_8			= 0x38,  // 8 key	
	Key_9			= 0x39,  // 9 key	
	Key_A			= 0x41,  // A key	
	Key_B			= 0x42,  // B key	
	Key_C			= 0x43,  // C key	
	Key_D			= 0x44,  // D key	
	Key_E			= 0x45,  // E key	
	Key_F			= 0x46,  // F key	
	Key_G			= 0x47,  // G key	
	Key_H			= 0x48,  // H key	
	Key_I			= 0x49,  // I key	
	Key_J			= 0x4A,  // J key	
	Key_K			= 0x4B,  // K key	
	Key_L			= 0x4C,  // L key	
	Key_M			= 0x4D,  // M key	
	Key_N			= 0x4E,  // N key	
	Key_O			= 0x4F,  // O key	
	Key_P			= 0x50,  // P key	
	Key_Q			= 0x51,  // Q key	
	Key_R			= 0x52,  // R key	
	Key_S			= 0x53,  // S key	
	Key_T			= 0x54,  // T key	
	Key_U			= 0x55,  // U key	
	Key_V			= 0x56,  // V key	
	Key_W			= 0x57,  // W key	
	Key_X			= 0x58,  // X key	
	Key_Y			= 0x59,  // Y key	
	Key_Z			= 0x5A,  // Z key	
	Key_LWin		= 0x5B,  // Left Windows key (Microsoft® Natural® keyboard)  
	Key_RWin		= 0x5C,  // Right Windows key (Natural	keyboard)  
	Key_Apps		= 0x5D,  // Applications key (Natural keyboard)	 
	Key_Sleep		= 0x5F,  // Computer Sleep	key	
	Key_Numpad0		= 0x60,  // Numeric keypad 0 key  
	Key_Numpad1		= 0x61,  // Numeric keypad 1 key  
	Key_Numpad2		= 0x62,  // Numeric keypad 2 key  
	Key_Numpad3		= 0x63,  // Numeric keypad 3 key  
	Key_Numpad4		= 0x64,  // Numeric keypad 4 key  
	Key_Numpad5		= 0x65,  // Numeric keypad 5 key  
	Key_Numpad6		= 0x66,  // Numeric keypad 6 key  
	Key_Numpad7		= 0x67,  // Numeric keypad 7 key  
	Key_Numpad8		= 0x68,  // Numeric keypad 8 key  
	Key_Numpad9		= 0x69,  // Numeric keypad 9 key  
	Key_Multiply	= 0x6A,  // Multiply key  
	Key_Add			= 0x6B,  // Add key  
	Key_Separator	= 0x6C,  // Separator key
	Key_Subtract	= 0x6D,  // Subtract key  
	Key_Decimal		= 0x6E,  // Decimal key  
	Key_Divide		= 0x6F,  // Divide key	 
	Key_F1			= 0x70,  // F1	key	 
	Key_F2			= 0x71,  // F2	key	 
	Key_F3			= 0x72,  // F3	key	 
	Key_F4			= 0x73,  // F4	key	 
	Key_F5			= 0x74,  // F5	key	 
	Key_F6			= 0x75,  // F6	key	 
	Key_F7			= 0x76,  // F7	key	 
	Key_F8			= 0x77,  // F8	key	 
	Key_F9			= 0x78,  // F9	key	 
	Key_F10			= 0x79,  // F10 key  
	Key_F11			= 0x7A,  // F11 key  
	Key_F12			= 0x7B,  // F12 key  
	Key_F13			= 0x7C,  // F13 key  
	Key_F14			= 0x7D,  // F14 key  
	Key_F15			= 0x7E,  // F15 key  
	Key_F16			= 0x7F,  // F16 key  
	Key_F17			= 0x80,  // F17 key  
	Key_F18			= 0x81,  // F18 key  
	Key_F19			= 0x82,  // F19 key  
	Key_F20			= 0x83,  // F20 key  
	Key_F21			= 0x84,  // F21 key  
	Key_F22			= 0x85,  // F22 key  
	Key_F23			= 0x86,  // F23 key  
	Key_F24			= 0x87,  // F24 key  
	Key_Numlock		= 0x90,  // NUM LOCK key  
	Key_Scroll		= 0x91,  // SCROLL	LOCK key  
	Key_LShift		= 0xA0,  // Left SHIFT	key	
	Key_RShift		= 0xA1,  // Right SHIFT key 
	Key_LControl	= 0xA2,  // Left CONTROL key 
	Key_RControl	= 0xA3,  // Right CONTROL key 
	Key_LMenu		= 0xA4,  // Left MENU key 
	Key_RMenu		= 0xA5,  // Right MENU	key	
	Key_Comma		= 0xBC,  // Comma key	(,)
	Key_Plus		= 0xBB,  // Plus key	(+)
	Key_Minus		= 0xBD,  // Minus key	(-)
	Key_Period		= 0xBE,  // Period key	(.)
	Key_Tilde		= 0xC0,  // Tilde key (~)
	Key_Attn		= 0xF6,  // Attn key 
	Key_CrSel		= 0xF7,  // CrSel key 
	Key_ExSel		= 0xF8,  // ExSel key 
	Key_ErEof		= 0xF9,  // Erase EOF key 
	Key_Play		= 0xFA,  // Play key 
	Key_Zoom		= 0xFB,  // Zoom key 
	Key_PA1			= 0xFD,  // PA1 key 
	Key_OemClear	= 0xFE,  // Clear key	

	NumKeyCodes		= 0x100  // Maximum number of key codes
};

const char* mxKeyToStr( EKeyCode keyCode );

/*
-----------------------------------------------------------------------------
	EKeyModifier
-----------------------------------------------------------------------------
*/
enum EKeyModifier
{
	KeyModifier_None			= 0,
	KeyModifier_LCtrl			= BIT(0),
	KeyModifier_LShift			= BIT(1),
	KeyModifier_LAlt			= BIT(2),
	KeyModifier_LWin			= BIT(3),
	KeyModifier_RCtrl			= BIT(4),
	KeyModifier_RShift			= BIT(5),
	KeyModifier_RAlt			= BIT(6),
	KeyModifier_RWin			= BIT(7),
	KeyModifier_NumLock			= BIT(8),
	KeyModifier_CapsLock		= BIT(9),
	KeyModifier_ScrollLock		= BIT(10),

	KeyModifier_Ctrl			= (KeyModifier_LCtrl | KeyModifier_RCtrl),	// True if ctrl was also pressed.
	KeyModifier_Shift			= (KeyModifier_LShift | KeyModifier_RShift),	// True if shift was also pressed.
	KeyModifier_Alt				= (KeyModifier_LAlt | KeyModifier_RAlt),	// True if alt was also pressed.
	KeyModifier_Win				= (KeyModifier_LWin | KeyModifier_RWin),
	KeyModifier_Modifiers		= (KeyModifier_Ctrl | KeyModifier_Shift | KeyModifier_Alt | KeyModifier_Win),
	KeyModifier_LockKeys		= (KeyModifier_CapsLock | KeyModifier_NumLock | KeyModifier_ScrollLock)
};

typedef TBits< EKeyModifier, U4 >	KeyModifiers;


#pragma pack (push,4)
struct mxKeyboardEvent
{
	EKeyCode		key;		// Key which has been pressed or released.
	EButtonState	state;
	KeyModifiers	modifiers;
};
#pragma pack (pop)

/*
=============================================================================

	Mouse

=============================================================================
*/
enum EMouseButton
{
	LeftMouseButton,
	MiddleMouseButton,
	RightMouseButton,

	NumMouseButtons
};

enum EMouseEvent
{
	Cursor_Moved = 0,
	Wheel_Moved,

	LeftButton_PressedDown,
	LeftButton_LeftUp,

	RightButton_PressedDown,
	RightButton_LeftUp,

	MiddleButton_PressedDown,
	MiddleButton_LeftUp,

	MouseEventCount
};

const char* EMouseEvent_To_Chars( EMouseEvent type );

#pragma pack (push,4)
struct mxMouseEvent
{
	EMouseEvent	Type;

	INT		x, y;	// Updated cursor coordinates.

	// Only valid on Cursor_Moved event.
	// Delta = CurrentCursorPosition - PreviousCursorPosition.
	INT		deltaX, deltaY;

	// Only valid on Wheel_Moved event. Usually in the range [-1.0 .. 1.0].
	// A positive value indicates that the wheel was rotated forward, away from the user; a negative value indicates that the wheel was rotated backward, toward the user.
	FLOAT	wheelDelta;
};
#pragma pack (pop)

struct SInputEvent
{
};
struct SKeyboardEvent : SInputEvent
{
};
struct SJoystickEvent : SInputEvent
{
};
struct SMouseEvent : SInputEvent
{
	// the absolute position of the mouse cursor
	int mouseX;
	int mouseY;
};
struct SMouseButtonEvent : SMouseEvent
{
	EMouseButton button;
	bool pressed;
};
struct SMouseMoveEvent : SMouseEvent
{
	int mouseDeltaX;
	int mouseDeltaY;
};
struct SMouseWheelEvent : SMouseEvent
{
	int scroll;
};

/*
-----------------------------------------------------------------------------
	AInputClient responds to input events
-----------------------------------------------------------------------------
*/
class AInputClient
{
public:
	virtual ~AInputClient() {}

	// user input event handling

	virtual void OnKeyPressed( EKeyCode key ) = 0;
	virtual void OnKeyReleased( EKeyCode key ) = 0;

	virtual void OnMouseButton( const SMouseButtonEvent& args ) = 0;
	virtual void OnMouseMove( const SMouseMoveEvent& args ) = 0;
	virtual void OnMouseWheel( const SMouseWheelEvent& args ) = 0;
};

/*
-----------------------------------------------------------------------------
	InputClient_State
-----------------------------------------------------------------------------
*/

struct KeyboardState
{
	// keyboard key states
	TStaticArray< SButtonState, NumKeyCodes >		keys;
};

struct MouseState
{
	// mouse button states
	TStaticArray< SButtonState, NumMouseButtons >		buttons;

	// current mouse cursor and wheel position
	int		mouseX, mouseY;
	int		mouseWheel;

public:
	MouseState()
	{
		mouseX = 0; mouseY = 0;
		mouseWheel = 0;
	}
};

// stores info about which keys are pressed, current mouse cursor position, etc.
//
class InputClient_State : public AInputClient
{
public:
	InputClient_State();
	~InputClient_State();

	BOOL IsKeyPressed( EKeyCode key ) const;
	BOOL IsKeyHeld( EKeyCode key ) const;
	BOOL IsKeyToggled( EKeyCode key ) const;

	BOOL IsCtrlPressed() const;
	BOOL IsShiftPressed() const;
	BOOL IsAltPressed() const;

	BOOL IsMouseButtonPressed( EMouseButton btn ) const;

	void GetMouseCursorPos( int &x, int &y ) const;

	int GetMouseX() const;
	int GetMouseY() const;

public:
	// AInputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;
	
	virtual void OnMouseButton( const SMouseButtonEvent& args ) override;
	virtual void OnMouseMove( const SMouseMoveEvent& args ) override;
	virtual void OnMouseWheel( const SMouseWheelEvent& args ) override;

protected:
	KeyboardState	m_keyboard;
	MouseState		m_mouse;
};


/*
-----------------------------------------------------------------------------
	InputSystem

	@todo: unfinished
-----------------------------------------------------------------------------
*/
class InputSystem : SingleInstance< InputSystem >
{
public:
	InputSystem();
	~InputSystem();

	void AddClient(AInputClient*);

private:
	
};


/*
-----------------------------------------------------------------------------
	game/editor/whatever client viewports classes can inherit
	from this abstract interface
-----------------------------------------------------------------------------
*/
class AClientViewport : public AEditableRefCounted, public InputClient_State
{
	mxDECLARE_ABSTRACT_CLASS( AClientViewport, AEditableRefCounted );

public:

	// Renders the scene.
	virtual void Draw() = 0;

	// this function gets called when the parent window's size changes
	// and this viewport should be resized
	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
	{}

	// the containing viewport widget is about to be closed;
	// shutdown everything (but keep the object around).
	virtual void OnClose()
	{}

	// the containing viewport widget requested redraw
	// you can issue draw calls here
	virtual void OnPaint()
	{
		this->Draw();
	}

public:
	bool IsDraggingMouse() const
	{
		return this->IsMouseButtonPressed(EMouseButton::LeftMouseButton);
	}

protected:
	AClientViewport()
	{}
	virtual ~AClientViewport()
	{}
};

mxNAMESPACE_END

#endif // !__MX_CORE_CLIENT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
