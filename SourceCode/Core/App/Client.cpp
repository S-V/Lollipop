/*
=============================================================================
	File:	Client.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include "Client.h"

mxNAMESPACE_BEGIN

const char* EMouseEvent_To_Chars( EMouseEvent type )
{
	switch( type )
	{
	case Cursor_Moved :				return "Cursor_Moved";
	case Wheel_Moved :				return "Wheel_Moved";
	case LeftButton_PressedDown :	return "LeftButton_PressedDown";
	case LeftButton_LeftUp :		return "LeftButton_LeftUp";
	case RightButton_PressedDown :	return "RightButton_PressedDown";
	case RightButton_LeftUp :		return "RightButton_LeftUp";
	case MiddleButton_PressedDown :	return "MiddleButton_PressedDown";
	case MiddleButton_LeftUp :		return "MiddleButton_LeftUp";
	default:	Unreachable;		return nil;
	}
}


/*================================
		InputClient_State
================================*/

InputClient_State::InputClient_State()
{
}

InputClient_State::~InputClient_State()
{

}

BOOL InputClient_State::IsKeyPressed( EKeyCode key ) const
{
	return m_keyboard.keys[ key ].IsPressed();
}

BOOL InputClient_State::IsKeyHeld( EKeyCode key ) const
{
	return m_keyboard.keys[ key ].IsHeld();
}

BOOL InputClient_State::IsKeyToggled( EKeyCode key ) const
{
	return m_keyboard.keys[ key ].IsToggled();
}

BOOL InputClient_State::IsCtrlPressed() const
{
	return IsKeyPressed(EKeyCode::Key_Control)
		//|| IsKeyPressed(EKeyCode::Key_LControl)
		//|| IsKeyPressed(EKeyCode::Key_RControl)
		;
}
BOOL InputClient_State::IsShiftPressed() const
{
	return IsKeyPressed(EKeyCode::Key_Shift)
		//|| IsKeyPressed(EKeyCode::Key_LShift)
		//|| IsKeyPressed(EKeyCode::Key_RShift)
		;
}
BOOL InputClient_State::IsAltPressed() const
{
	return IsKeyPressed(EKeyCode::Key_Menu)
		//|| IsKeyPressed(EKeyCode::Key_LMenu)
		//|| IsKeyPressed(EKeyCode::Key_RMenu)
		;
}

BOOL InputClient_State::IsMouseButtonPressed( EMouseButton btn ) const
{
	return m_mouse.buttons[ btn ].IsPressed();
}

void InputClient_State::GetMouseCursorPos( int &x, int &y ) const
{
	x = m_mouse.mouseX;
	y = m_mouse.mouseY;
}

int InputClient_State::GetMouseX() const
{
	return m_mouse.mouseX;
}

int InputClient_State::GetMouseY() const
{
	return m_mouse.mouseY;
}

void InputClient_State::OnKeyPressed( EKeyCode key )
{
	m_keyboard.keys[ key ].Press();
	m_keyboard.keys[ key ].Toggle();
}

void InputClient_State::OnKeyReleased( EKeyCode key )
{
	m_keyboard.keys[ key ].Release();
}

void InputClient_State::OnMouseButton( const SMouseButtonEvent& args )
{
	if( args.pressed ) {
		m_mouse.buttons[ args.button ].Press();
	} else {
		m_mouse.buttons[ args.button ].Release();
	}
}

void InputClient_State::OnMouseMove( const SMouseMoveEvent& args )
{
	m_mouse.mouseX = args.mouseX;
	m_mouse.mouseY = args.mouseY;
}

void InputClient_State::OnMouseWheel( const SMouseWheelEvent& args )
{
	m_mouse.mouseWheel += args.scroll;
}


mxDEFINE_ABSTRACT_CLASS( AClientViewport );

mxNAMESPACE_END
