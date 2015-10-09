/*
=============================================================================
	File:	WndMsgProc.h
	Desc:	Window callback function (handles window events)
=============================================================================
*/

#include <windowsx.h>

inline
LRESULT WindowProc( Window* wnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	AssertPtr(wnd);
	if( nil == wnd ) {
		return -1;
	}

	HWND	hWnd = wnd->hWnd;

	switch ( message )
	{

	//------------------------------------------------------------------------
	//	Keyboard events.
	//------------------------------------------------------------------------

#pragma region Keyboard Events

	// Is a key being pressed?
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			EKeyCode key = ( EKeyCode ) wParam;
			wnd->OnKeyPressed( key );
		}
		return 0;

	// Is a key being released?
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			EKeyCode key = ( EKeyCode ) wParam;
			wnd->OnKeyReleased( key );
		}
		return 0;

#pragma endregion

	//------------------------------------------------------------------------
	//	Mouse events
	//------------------------------------------------------------------------

#pragma region Mouse Events

	// Were any of the mouse-buttons pressed?
	case WM_LBUTTONDOWN:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = LeftMouseButton;
			args.pressed = true;

			wnd->OnMouseButton( args );
		}
		return 0;

	case WM_RBUTTONDOWN:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = RightMouseButton;
			args.pressed = true;

			wnd->OnMouseButton( args );
		}
		return 0;

	case WM_MBUTTONDOWN:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = MiddleMouseButton;
			args.pressed = true;

			wnd->OnMouseButton( args );
		}
		return 0;

	// Were any of the mouse-buttons released?
	case WM_LBUTTONUP:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = LeftMouseButton;
			args.pressed = false;

			wnd->OnMouseButton( args );
		}
		return 0;

	case WM_RBUTTONUP:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = RightMouseButton;
			args.pressed = false;

			wnd->OnMouseButton( args );
		}
		return 0;

	case WM_MBUTTONUP:
		{
			SMouseButtonEvent	args;
			args.mouseX = GET_X_LPARAM(lParam);
			args.mouseY = GET_Y_LPARAM(lParam);
			args.button = MiddleMouseButton;
			args.pressed = false;

			wnd->OnMouseButton( args );
		}
		return 0;

	// Did the mouse move?
	case WM_MOUSEMOVE:
		{
			// Get the absolute position of the mouse cursor
			const int	x = GET_X_LPARAM(lParam);
			const int	y = GET_Y_LPARAM(lParam);

			MouseState& mouse = wnd->GetMouseState();

			const int prevX = mouse.mouseX;
			const int prevY = mouse.mouseY;
	
			// Calculate deltas
			const int dX = x - prevX;
			const int dY = y - prevY;


			SMouseMoveEvent		args;
			{
				args.mouseX = x;
				args.mouseY = y;
				args.mouseDeltaX = dX;
				args.mouseDeltaY = dY;
			}
			wnd->OnMouseMove( args );

			mouse.mouseX = x;
			mouse.mouseY = y;
		}
		return 0;

	// Mouse wheel action?
	case WM_MOUSEWHEEL:
		{
			int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			if( wheelDelta != 0 )
			{
				MouseState& mouse = wnd->GetMouseState();

				mouse.mouseWheel += wheelDelta;

				SMouseWheelEvent	args;
				args.scroll = mouse.mouseWheel;

				wnd->OnMouseWheel( args );
			}
		}
		return 0;

	// WM_CAPTURECHANGED - Sent to the window that is losing the mouse capture.
	case WM_CAPTURECHANGED:
		{
			wnd->setActive(false);
		}
		return 0;

#if 0
	// Retrieve high-definition mouse movement data.
	// See:
	// http://msdn.microsoft.com/en-us/library/ee418864(v=VS.85).aspx
	// http://msdn.microsoft.com/en-us/library/ms645543.aspx
	//
	case WM_INPUT:
		{
			UINT dwSize;

			GetRawInputData(
				(HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
				sizeof(RAWINPUTHEADER)
				);

			LPBYTE lpb = mxStackAlloc( dwSize );
			if( !lpb )
			{
				return 0;
			}

			int readSize = GetRawInputData(
				(HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)
				);

			if( readSize != dwSize )
				puts( "ERROR:  GetRawInputData didn't return correct size!" ); 

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				if( raw->data.keyboard.VKey == VK_SPACE )
				{
					puts( "You are pressing space" );
				}
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				int dx = raw->data.mouse.lLastX;
				int dy = raw->data.mouse.lLastY;

				printf( "%d %d\n", dx, dy );
			} 

			delete[] lpb;
			return 0;
		}
#endif

#pragma endregion

	//------------------------------------------------------------------------
	//	Miscellaneous events.
	//------------------------------------------------------------------------

	case WM_CREATE:
		// the window has just been created...
		break;

	// Did we receive a close message?
	case WM_CLOSE :
		wnd->OnClose();
		Util_CloseWindow( wnd );
		return 0;

	case WM_DESTROY:
		::PostQuitMessage( 0 );
		return 0;

	// Resize the window?
	case WM_SIZE:
		{
			/* wParam values:
				SIZE_MAXHIDE
				Message is sent to all pop-up windows when some other window is maximized.
				
				SIZE_MAXIMIZED
				The window has been maximized.
				
				SIZE_MAXSHOW
				Message is sent to all pop-up windows when some other window has been restored to its former size.
				
				SIZE_MINIMIZED
				The window has been minimized.
				
				SIZE_RESTORED
				The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.

				lParam values:
				The low-order word of lParam specifies the new width of the client area. 
				The high-order word of lParam specifies the new height of the client area. 

			*/
			// deactivate if the window was minimized
			if ( wParam == SIZE_MINIMIZED ) {
				wnd->setActive( false );
			} else {
				wnd->setActive( true );
			}

			int width = LOWORD( lParam );
			int height = HIWORD( lParam );

			const bool bFullscreen = false;
			wnd->OnResize( width, height, bFullscreen );
		}
		break;

	// Move the window?
	case WM_MOVE:
		break;

	// WM_WINDOWPOSCHANGED - Sent to a window whose size,
	// position, or place in the Z order has changed
	// as a result of a call to the SetWindowPos function
	// or another window-management function.
	//
	case WM_WINDOWPOSCHANGED:
		{
			// lParam - A pointer to a WINDOWPOS structure
			// that contains information about the window's new size and position.
			LPWINDOWPOS winPosInfo = (LPWINDOWPOS) lParam;
			
			if( (winPosInfo->flags & SWP_NOSIZE) == 0 )
			{
				RECT rect;
				GetClientRect( hWnd, &rect );

				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;

				const bool bFullscreen = false;

				if( width && height ) {
					wnd->OnResize( width, height, bFullscreen );
				}
			}
		}
		break;

	case WM_ENTERMENULOOP:
	case WM_ENTERSIZEMOVE:
		wnd->setActive( false );
		break;

	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		wnd->setActive( true );
		break;

	case WM_ACTIVATEAPP:
		wnd->setActive( wParam );
		break;

	// Window activate message? (iconification?)
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.
	//
	case WM_ACTIVATE:
		if( LOWORD( wParam ) == WA_INACTIVE ) {
			wnd->setActive( false );
		} else {
			wnd->setActive( true );
		}
		break;

	case WM_SETFOCUS:
		wnd->onReceivedFocus();
		break;

	case WM_KILLFOCUS:
		wnd->onLostFocus();
		break;

	case WM_SETCURSOR :
		//if( bHideMouseCursor ) {
		//	::SetCursor( nil );	// Turn off mouse cursor if needed.
		//} else {
		//	::SetCursor( gDriver.mHCursor );
		//}
		break;

	// A window receives this message when the user chooses a command
	// from the Window menu (formerly known as the system or control menu)
	// or when the user chooses the maximize button, minimize button, restore button, or close button.
	case WM_SYSCOMMAND :
		// Prevent screensaver or monitor powersave mode from starting.
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER)
		{
			return 0;
		}
		break;

	// Was the window contents damaged?
	case WM_PAINT :
		{
			//window needs to be repainted
			PAINTSTRUCT ps;
			BeginPaint( hWnd, &ps );
			Util_PaintWindow( wnd );
			EndPaint( hWnd, &ps );
			//bool bActive = !IsRectEmpty( &ps.rcPaint );
		}
		return 0;

	case WM_ERASEBKGND:
		break;

	case WM_INPUTLANGCHANGE:
		{
			// get the new codepage used for keyboard input
			//G.KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
			//G.KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(G.KEYBOARD_INPUT_HKL) );
		}
		break;

	// The WM_DISPLAYCHANGE message is sent to all windows when the display resolution has changed.
	case WM_DISPLAYCHANGE:
		{
			// the message is sent to all top-level main windows,
			// and posted to all other main windows, and never send/posted to any child windows.
			// WM_DISPLAYCHANGE does not appear to reach child windows.
		}
		break;

	case WM_POWER :
		// The system, typically a battery-powered PC, is about to enter a suspended mode.
		wnd->setActive( false );
		break;

	case WM_POWERBROADCAST :
		// A power management event has occurred.
		break;

	}//End switch

	return ::DefWindowProc( hWnd, message, wParam, lParam );
}
