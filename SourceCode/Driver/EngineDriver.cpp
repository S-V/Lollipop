/*
=============================================================================
	File:	WindowsDriver.cpp
	Desc:	
=============================================================================
*/
#include <Driver_PCH.h>
#pragma hdrstop
#include <Driver.h>

mxNAMESPACE_BEGIN

//--------------------------------
//		Globals
//--------------------------------

static struct Globals 
{
	const TCHAR* WINDOW_CLASS_NAME;

	HINSTANCE	hInstance;	// handle to the application instance
	HANDLE		hCurrThread;	// handle to the current thread

	HKL		KEYBOARD_INPUT_HKL;
	UINT	KEYBOARD_INPUT_CODEPAGE;

	TList< Window* >	windows;

public:
	Globals()
		: WINDOW_CLASS_NAME( TEXT("Lollipop3D") )
	{
		hInstance = ::GetModuleHandle( nil );
		hCurrThread = ::GetCurrentThread();
		KEYBOARD_INPUT_HKL = 0;
		KEYBOARD_INPUT_CODEPAGE = 1252;
	}
	~Globals()
	{
		Assert(windows.GetAllocatedMemory() == 0);
		Assert(windows.IsEmpty());
	}

} G;

//-------------------------------------------------------------------------------------------------------------//

void Util_CloseWindow( Window* w )
{
	AssertPtr(w);

	HWND hWnd = w->hWnd;
	if( hWnd != NULL )
	{
		// Fade out
		::AnimateWindow(
			hWnd,
			200,	// The time it takes to play the animation, in milliseconds.
			AW_HIDE | AW_BLEND
		);

		::DestroyWindow( hWnd );


		w->hWnd = NULL;
	}

	G.windows.Remove( w );

	// if the last window was closed...
	if( G.windows.IsEmpty() )
	{
		G.windows.Clear();
	}
}
void Util_CloseAllWindows()
{
	for( UINT iWindow = 0; iWindow < G.windows.Num(); iWindow++ )
	{
		Window * w = G.windows[ iWindow ];
		Util_CloseWindow( w );
	}
	G.windows.Clear();
}
void Util_PaintWindow( Window* w )
{
	//...
}



//-------------------------------------------------------------------------------------------------------------//

#include "WndMsgProc.inl"

static
LRESULT WINAPI Global_WindowsMsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	for( UINT iWindow = 0; iWindow < G.windows.Num(); iWindow++ )
	{
		Window * w = G.windows[ iWindow ];
		if( w->hWnd == hWnd ) {
			return ::WindowProc( w, message, wParam, lParam );
		}
	}
	return ::DefWindowProc( hWnd, message, wParam, lParam );
}

//-------------------------------------------------------------------------------------------------------------//

HWND Util_CreateNewWindow(
	const mxChar* title,
	UINT width, UINT height,
	bool bResizable = false
	)
{
	// Register Class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= Global_WindowsMsgProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= G.hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= G.WINDOW_CLASS_NAME;
	wcex.hIconSm		= 0;

    // Load user-provided icon if available
	wcex.hIcon = ::LoadIcon( G.hInstance, TEXT("Lollipop.ico") );
	if( !wcex.hIcon )
	{
		// Load default icon
		//wcex.hIcon = LoadIcon( NULL, IDI_WINLOGO );
		wcex.hIcon = ::LoadIcon( G.hInstance, IDI_APPLICATION );
	}

	RegisterClassEx(&wcex);

	// calculate client size

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = width;
	clientSize.bottom = height;

	DWORD windowStyle = WS_POPUP;

	const bool bFullscreen = false;
	if (!bFullscreen) {
		windowStyle |= (WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}

	if( bResizable )
	{
		windowStyle |= WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	::AdjustWindowRect(&clientSize, windowStyle, FALSE);

	const INT32 realWidth = clientSize.right - clientSize.left;
	const INT32 realHeight = clientSize.bottom - clientSize.top;

	INT32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	INT32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if ( windowLeft < 0 )
		windowLeft = 0;
	if ( windowTop < 0 )
		windowTop = 0;	// make sure window menus are in screen on creation

	if (bFullscreen)
	{
		windowLeft = 0;
		windowTop = 0;
	}

	// Create a window.

	HWND hWnd = ::CreateWindow(
		G.WINDOW_CLASS_NAME,	// Class name
		title,			// Window title
		windowStyle,	// Defined window style
		windowLeft,
		windowTop,
		realWidth,		// Decorated window width
		realHeight,		// Decorated window height
		NULL,			// No parent window
		NULL,			// No menu
		G.hInstance,
		NULL			// Nothing to WM_CREATE
	);

	if( !hWnd )
	{
		mxErr("Failed to create a window");
		return NULL;
	}

	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);

	// fix ugly ATI driver bugs. Thanks to ariaci
	::MoveWindow(hWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	// set this as active window
	::SetActiveWindow(hWnd);

	Util_SetForegroundWindow(hWnd);

	return hWnd;
}

HWND Util_CreateNewWindowX( const WindowDescription& desc )
{
	bool bResizable = desc.flags & EWindowFlags::Window_Resizable;
	return Util_CreateNewWindow( desc.title, desc.width, desc.height, bResizable );
}

/*================================
		WindowsDriver
================================*/

WindowsDriver::WindowsDriver()
{
	DBG_ENSURE_ONLY_ONE_CALL;

	bIsRunning = true;
	bIsActive = true;

	Util_LimitThreadAffinityToCurrentProcessor();
}
WindowsDriver::WindowsDriver(ENoInit)
{
	DBG_ENSURE_ONLY_ONE_CALL;

	bIsRunning = true;
	bIsActive = true;

	Util_LimitThreadAffinityToCurrentProcessor();
}
WindowsDriver::~WindowsDriver()
{
	Util_CloseAllWindows();
}
//-------------------------------------------------------------------------------------------------------------//
void WindowsDriver::Close()
{
	MSG msg;
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	PostQuitMessage(0);
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	Util_CloseAllWindows();
	bIsRunning = false;
	bIsActive = false;
}
//-------------------------------------------------------------------------------------------------------------//
bool WindowsDriver::ProcessWinMessages()
{
	MSG msg;

	// Use PeekMessage() so we can use idle time to render the scene.

	if( ::PeekMessage( &msg, nil, 0, 0, PM_REMOVE ) )
	{
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	// Application logic:

	// If only one window is opened...
	if( G.windows.Num() == 1 )
	{
		bIsActive = G.windows[0]->isActive();
	}
	// If all windows are closed then stop the engine.
	else
	if( !G.windows.Num() )
	{
		bIsRunning = false;
	}

	return bIsRunning;
}
//-------------------------------------------------------------------------------------------------------------//
int WindowsDriver::Run( const Callbacks& callbacks )
{
	MSG msg;
	ZERO_OUT(msg);

	while( true )
	{
		// PeekMessage is NOT a BLOCKING FUNCTION.
		// That is, PeekMessage() returns immediately
		// with either a TRUE (there was a message
		// for our window)
		// or a FALSE (there was no message for our window).

		// If there was a message for our window, then
		// we want to translate and dispatch that message.

		// Use PeekMessage() so we can use idle time to render the scene.

		if( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT )
			{
				break;
			}
		}
		else
		{
			::TranslateMessage( &msg );   // translates
			::DispatchMessage( &msg );    // this line RESULTS IN
			// a call to WndProc(), passing the message and
			// the HWND.

			// Note that in this program, all we're really using
			// the messaging system for is for
			// processing the QUIT message that occurs
			// when the user clicks the X on the window
			// to close it.
		}

		// Application logic:

		callbacks.onFrame();
	}

	return callbacks.onExit();
}


static void WinUtil_ShowMouseCursor()
{
	while( ::ShowCursor(TRUE) < 0 )
		;
}
static void WinUtil_HideMouseCursor()
{
	while( ::ShowCursor(FALSE) >= 0 )
		;
}
static void WinUtil_SetCaptureMouseInput( HWND hWnd, bool bGrabMouse )
{
	if( bGrabMouse )
	{
		RECT rect;
		::GetClientRect( hWnd,&rect );
		::ClientToScreen( hWnd, (LPPOINT)&rect.left );
		::ClientToScreen( hWnd, (LPPOINT)&rect.right );
		::ClipCursor( &rect );
	}
	else {
		::ClipCursor( NULL );
	}
}

/*================================
			Window
================================*/

Window::Window( const mxChar* title, UINT width, UINT height )
{
	hWnd = nil;
	bHasFocus = true;
	
	hWnd = Util_CreateNewWindow( title, width, height );

	if( hWnd ) {
		G.windows.Add( this );
	}
}

Window::Window( const WindowDescription& desc )
{
	hWnd = nil;
	bHasFocus = true;

	hWnd = Util_CreateNewWindowX( desc );

	if( hWnd ) {
		G.windows.Add( this );
	}
}

Window::Window( HWND existingWindow )
{
	AssertPtr(existingWindow);
	hWnd = existingWindow;
	bHasFocus = true;

	if( hWnd != NULL ) {
		G.windows.Add( this );
	}
}

Window::~Window()
{
	Util_CloseWindow( this );
}

bool Window::isOpen() const
{
	return hWnd != NULL;
}

void Window::close()
{
	Util_CloseWindow(this);
	hWnd = nil;
}

void Window::setTitle( const mxChar* newCaption )
{
	if( hWnd ) {
		::SetWindowText( hWnd, newCaption );
	}
}

void Window::bringToFront()
{
	Util_SetForegroundWindow( hWnd );
}

void Window::setTopLeft( int x, int y )
{
	int	width, height;
	this->GetSize(width, height);
	::SetWindowPos( hWnd, HWND_TOP, x, y, width, height, 0 );
}

void Window::GetSize( int &width, int &height ) const
{
	RECT	rect;
	::GetWindowRect( hWnd,&rect );
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void Window::GetRect( int &topLeftX, int &topLeftY, int &width, int &height ) const
{
	RECT	rect;
	::GetWindowRect(hWnd,&rect);
	topLeftX = rect.left;
	topLeftY = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void Window::GetMouseCursorPos( int &x, int &y ) const
{
	POINT pt;
	::GetCursorPos( &pt );
	::ScreenToClient( this->hWnd, &pt );
	x = pt.x;
	y = pt.y;
}

void Window::SetMouseCursorPos( int x, int y )
{
	POINT point;
	point.x = x;
	point.y = y;
	::ClientToScreen( this->hWnd, &point );
	::SetCursorPos( point.x, point.y );
}

void Window::SetMouseCursorVisible( bool bVisible )
{
	if( bVisible )
	{
		WinUtil_ShowMouseCursor();
	}
	else
	{
		WinUtil_HideMouseCursor();
	}
}

void Window::setActive( bool bActiveWindow )
{
	bHasFocus = bActiveWindow;
}

bool Window::isActive() const
{
	//return bHasFocus;
	return ::GetActiveWindow() == ( this->hWnd );
}

void Window::onLostFocus()
{
	bHasFocus = false;
}

void Window::onReceivedFocus()
{
	bHasFocus = true;
}

void Window::OnKeyPressed( EKeyCode key )
{
	//if( key == EKeyCode::Key_Escape )
	//{
	//	close();
	//	return;
	//}
	InputClient_State::OnKeyPressed( key );
}

void Window::OnKeyReleased( EKeyCode key )
{
	InputClient_State::OnKeyReleased( key );
}

void Window::OnMouseButton( const SMouseButtonEvent& args )
{
	InputClient_State::OnMouseButton( args );
}

void Window::OnMouseMove( const SMouseMoveEvent& args )
{
	InputClient_State::OnMouseMove( args );
}

void Window::OnMouseWheel( const SMouseWheelEvent& args )
{
	InputClient_State::OnMouseWheel( args );
}

void Window::OnClose()
{
	bHasFocus = false;
}

/*================================
		WindowDescription
================================*/

WindowDescription::WindowDescription()
{
	ZERO_OUT(*this);

	width = 800;
	height = 600;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
