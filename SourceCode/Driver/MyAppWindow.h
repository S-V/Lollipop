/*
=============================================================================
	File:	MyAppWindow.h
	Desc:	
=============================================================================
*/
#pragma once
#ifndef __MX_MY_APP_WINDOW_H__
#define __MX_MY_APP_WINDOW_H__

#include <Base/Math/Vector/VectorTemplate.h>

#include <Driver/Driver.h>



mxNAMESPACE_BEGIN

enum EAppWindowFlags
{
	// should we capture mouse and keyboard input?
	App_CaptureInput = BIT(0),	//<: default=1

	// Confine mouse movement to window's rectangle.
	App_ClipMouseCursorToWindow = BIT(1),	//<: default=0
};


/*
-----------------------------------------------------------------------------
	MyAppWindow

	grabs mouse input and clips mouse cursor to window rectangle
	until ESC is pressed;
	captures mouse again if LMB is pressed inside the client rect.
-----------------------------------------------------------------------------
*/
class MyAppWindow
	: public Window
	//, SingleInstance<MyAppWindow>
{
	TPtr< AClientViewport >		m_client;

	TBits<EAppWindowFlags,U4>		m_flags;

	// used to restore prev mouse cursor position when releasing mouse capture
	vector2d<int>	m_savedMouseCursorPos;

	// true if the cursor was placed to the window center;
	// this var is needed because on Windows SetCursorPos() causes a WM_MOUSEMOVE event.
	bool	m_bMouseCursorWasReset;

public:
	typedef Window Super;

	MyAppWindow( UINT width, UINT height )
		: Super( L"TestApp", width, height )
	{
		this->Initialize();
	}
	MyAppWindow( HWND existingWindow )
		: Super( existingWindow )
	{
		this->Initialize();
	}
	~MyAppWindow()
	{
	}

	void SetClient( AClientViewport* client )
	{
		//CHK_VRET_IF_NIL( client );
		m_client = client;
	}

	bool CapturesInput() const
	{
		return m_flags & App_CaptureInput;
	}

	void CaptureMouseInput( bool bCapture )
	{
		if( bCapture )
		{
			int mouseX, mouseY;
			this->GetMouseCursorPos( mouseX, mouseY );

			m_savedMouseCursorPos.x = mouseX;
			m_savedMouseCursorPos.y = mouseY;

			m_flags |= App_CaptureInput;
			this->SetMouseCursorVisible(false);
		}
		else
		{
			this->SetMouseCursorPos( m_savedMouseCursorPos.x, m_savedMouseCursorPos.y );

			m_flags &= ~App_CaptureInput;
			this->SetMouseCursorVisible(true);
		}
	}

public://-AClientViewport

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override
	{
		VRET_IF_NOT(m_client.IsValid());
		m_client->OnResize( newWidth, newHeight, bFullScreen );
	}
	virtual void Draw() override
	{
		VRET_IF_NOT(m_client.IsValid());
		m_client->Draw();
	}

public://-Window

	virtual void OnClose() override
	{
		Super::OnClose();
		if( m_client != nil ) {
			m_client->OnClose();
		}
	}

	virtual void onLostFocus() override
	{
		Super::onLostFocus();
		this->CaptureMouseInput(false);
	}
	virtual void onReceivedFocus() override
	{
		Super::onReceivedFocus();
		this->CaptureMouseInput(true);
	}
	virtual void OnKeyPressed( EKeyCode key ) override
	{
		Super::OnKeyPressed( key );

		if( key == EKeyCode::Key_Escape )
		{
			if( this->CapturesInput() )
			{
				this->CaptureMouseInput(false);
			}
			else
			{
				this->close();
				return;
			}
		}

		if( this->CapturesInput() )
		{
			VRET_IF_NOT(m_client.IsValid());
			m_client->OnKeyPressed( key );
		}
	}
	virtual void OnKeyReleased( EKeyCode key ) override
	{
		Super::OnKeyReleased( key );
		if( this->CapturesInput() )
		{
			VRET_IF_NOT(m_client.IsValid());
			m_client->OnKeyReleased( key );
		}
	}
	virtual void OnMouseButton( const SMouseButtonEvent& args ) override
	{
		Super::OnMouseButton( args );

		const bool bCaptureMouseInput = this->CapturesInput();

		if( bCaptureMouseInput )
		{
			VRET_IF_NOT(m_client.IsValid());
			m_client->OnMouseButton( args );
		}
		else
		{
			if( args.button == EMouseButton::LeftMouseButton )
			{
				this->CaptureMouseInput(true);
			}
		}
	}
	virtual void OnMouseMove( const SMouseMoveEvent& args ) override
	{
		const bool bCaptureMouseInput = this->CapturesInput();

		if( !bCaptureMouseInput )
		{
			return;
		}

		// When the mouse cursor moves, we reset its position to the center of the window.

		if ( !m_bMouseCursorWasReset )
		{
			VRET_IF_NOT(m_client.IsValid());
			m_client->OnMouseMove( args );


			if( m_flags & App_ClipMouseCursorToWindow )
			{
				// move the cursor to the center of the window
#if 1
				int	windowWidth, windowHeight;
				this->GetSize( windowWidth, windowHeight );

				this->SetMouseCursorPos( windowWidth / 2, windowHeight / 2 );
#else
				Rect<int>	windowRect;
				this->GetRect( windowRect.left, windowRect.top, windowRect.width, windowRect.height );

				int mouseX, mouseY;
				this->GetMouseCursorPos( mouseX, mouseY );

				if( !windowRect.Contains( mouseX, mouseY ) )
				{
					this->SetMouseCursorPos( windowRect.width / 2, windowRect.height / 2 );
				}
#endif
			}
		}

		m_bMouseCursorWasReset = !m_bMouseCursorWasReset;
	}
	virtual void OnMouseWheel( const SMouseWheelEvent& args ) override
	{
		Super::OnMouseWheel( args );
		VRET_IF_NOT(m_client.IsValid());
		m_client->OnMouseWheel( args );
	}

private:
	void Initialize()
	{
		m_flags = App_CaptureInput | App_ClipMouseCursorToWindow;
		m_bMouseCursorWasReset = false;
		this->CaptureMouseInput(true);
	}
};


mxNAMESPACE_END

#endif // ! __MX_MY_APP_WINDOW_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
