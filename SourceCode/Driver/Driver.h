/*
=============================================================================
	File:	Driver.h
	Desc:	Public application framework header file.
=============================================================================
*/

#ifndef __MX_DRIVER_PUBLIC_H__
#define __MX_DRIVER_PUBLIC_H__

#include <Core/App/Client.h>

/*
=============================================================================
	
	PLATFORM DRIVER

=============================================================================
*/
mxNAMESPACE_BEGIN

#if MX_PLATFORM == Platform_Win32
	typedef HWND mxWindowHandle;
#else
#	error Unknown platform!
#endif

enum EWindowFlags
{
	Window_Resizable = BIT(0)
};

/*
-----------------------------------------------------------------------------
	WindowDescription
-----------------------------------------------------------------------------
*/
class WindowDescription
{
public:

	mxChar *	title;	// optional window caption

	UINT	width;
	UINT	height;

	TBits< EWindowFlags >	flags;

public:
	WindowDescription();
};


/*
-----------------------------------------------------------------------------
	Window
-----------------------------------------------------------------------------
*/
class Window : public AClientViewport
{
public:
	Window( const mxChar* title, UINT width, UINT height );
	Window( const WindowDescription& desc );
	explicit Window( HWND existingWindow );
	~Window();

public:

	bool isOpen() const;

	void close();

	void setTitle( const mxChar* newCaption );

	void bringToFront();


	void setTopLeft( int x, int y );

public:	//--AClientViewport

	virtual void OnClose() override;

	// AInputClient interface
	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

	virtual void OnMouseButton( const SMouseButtonEvent& args ) override;
	virtual void OnMouseMove( const SMouseMoveEvent& args ) override;
	virtual void OnMouseWheel( const SMouseWheelEvent& args ) override;

public:
	virtual void GetSize( int &width, int &height ) const;
	virtual void GetRect( int &topLeftX, int &topLeftY, int &width, int &height ) const;
	virtual void GetMouseCursorPos( int &x, int &y ) const;

	// Set absolute mouse cursor position within the window.
	void SetMouseCursorPos( int x, int y );

	void SetMouseCursorVisible( bool bVisible );

	HWND getWindowHandle() const { return this->hWnd; }

public_internal:

	// lowercase names to avoid name clashes with Windows
	virtual bool isActive() const;
	virtual void setActive( bool bActiveWindow );

	virtual void onLostFocus();
	virtual void onReceivedFocus();

private:
	friend LRESULT WindowProc( Window* wnd, UINT message, WPARAM wParam, LPARAM lParam );

	MouseState & GetMouseState() { return m_mouse; }

public_internal:
	mxWindowHandle	hWnd;

	bool	bHasFocus;
};

extern void Util_CloseWindow( Window* w );
extern void Util_PaintWindow( Window* w );

/*
-----------------------------------------------------------------------------
	WindowsDriver
-----------------------------------------------------------------------------
*/

// called every frame in sim loop
//
typedef void F_FrameCallback();

// called after sim loop exits
typedef int F_CloseCallback( void );

struct Callbacks
{
	F_FrameCallback *	onFrame;
	F_CloseCallback *	onExit;

	Callbacks()
	{
		ZERO_OUT(*this);
	}
};

class WindowsDriver
{
public:
	
	explicit WindowsDriver(ENoInit);

	WindowsDriver();


	// requests exit
	void Close();

	// runs a single frame, processes windows messages
	// returns false if exit requested
	bool ProcessWinMessages();

	// enters the message loop
	int Run( const Callbacks& callbacks );

public:
	~WindowsDriver();

public_internal:
	bool		bIsRunning;	// entered the main loop
	bool		bIsActive;	// main window has focus
};

mxNAMESPACE_END


#include "ConsoleWindow.h"

#endif // ! __MX_DRIVER_PUBLIC_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
