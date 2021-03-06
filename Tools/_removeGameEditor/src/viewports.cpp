#include <stdafx.h>
#pragma hdrstop

#include <Renderer/Renderer.h>
#include <Renderer/Util/BatchRenderer.h>

#include "QtSupport/DragAndDrop.h"

#include "editor_system.h"
#include "viewports.h"


/*
-----------------------------------------------------------------------------
	EdViewport
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( EdViewport );

EdViewport::EdViewport()
{

}

EdViewport::~EdViewport()
{

}

void EdViewport::InitViewport( EdViewportWidget* widget )
{
	//Assert( winId != INVALID_WINDOW_HANDLE );
	Assert( m_renderViewport.IsNull() );

	{
		rxRenderer & renderer = gRenderer;

		rxViewportConfig	config;
		config.windowHandle = widget->winId();
		config.bIsSceneViewport = this->IsSceneViewport();
		config.bDepthStencil = this->IsSceneViewport();

		renderer.CreateViewport( config, m_renderViewport );
	}

	m_widget = widget;
}

void EdViewport::CloseViewport()
{
	if( !m_renderViewport.IsNull() )
	{
		rxRenderer & renderer = gRenderer;

		renderer.DestroyViewport( m_renderViewport );
	}

	m_widget = nil;
}

void EdViewport::OnProjectLoaded()
{
	//Super::OnProjectLoaded();
}

void EdViewport::OnProjectUnloaded()
{
	//Super::OnProjectUnloaded();
	if( this->IsValid() )
	{
		this->ClearViewport( FColor::BLACK );
	}
}

AHitProxy* EdViewport::GetHitProxy( const int iX, const int iY ) const
{
	Assert( gRenderer.IsMainViewport( m_renderViewport ) );
	AHitProxy* pHitProxy = EdGetHitTesting().GetHitProxy( m_renderViewport, iX, iY );

	if( pHitProxy != nil )
	{
		const EMouseCursor eMouseCursor = IsDragging(*this) ?
			pHitProxy->GetMouseDragCursor()
			:
			pHitProxy->GetMouseCursor()
			;

		switch( eMouseCursor )
		{
		case Mouse_ArrowCursor:	m_widget->setCursor(Qt::ArrowCursor);	break;
		case Mouse_PointingHandCursor:	m_widget->setCursor(Qt::PointingHandCursor);	break;
		case Mouse_OpenHandCursor:	m_widget->setCursor(Qt::OpenHandCursor);	break;
		case Mouse_ClosedHandCursor:	m_widget->setCursor(Qt::ClosedHandCursor);	break;
		case Mouse_CrossCursor:	m_widget->setCursor(Qt::CrossCursor);	break;
		case Mouse_SizeAllCursor:	m_widget->setCursor(Qt::SizeAllCursor);	break;
		default:	m_widget->setCursor(Qt::ArrowCursor);	break;
		}
	}
	else
	{
		m_widget->setCursor(Qt::ArrowCursor);
	}

	return pHitProxy;
}

bool EdViewport::IsValid() const
{
	return !m_renderViewport.IsNull()
		&& m_widget != nil
		;
}

void EdViewport::ClearViewport( const FColor& color )
{
	Assert(this->IsValid());

	rxRenderer & renderer = gRenderer;

	rxFrameContext	frameContext;
	frameContext.clearColor = color;

	renderer.BeginScene( &m_renderViewport, frameContext );
	renderer.EndScene();
}

void EdViewport::OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
{
	AssertX( bFullScreen == false, "Not impl" );

	rxRenderer & renderer = gRenderer;

	renderer.OnViewportResized( m_renderViewport, newWidth, newHeight );
}

void EdViewport::OnClose()
{
	this->CloseViewport();
}

void EdViewport::OnPaint()
{
	//if( IsProjectLoaded() )
	//{
	//	this->Draw();
	//}
}

void EdViewport::OnKeyPressed( EKeyCode key )
{
	Super::OnKeyPressed( key );
}

void EdViewport::OnKeyReleased( EKeyCode key )
{
	Super::OnKeyReleased( key );
}

void EdViewport::OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed )
{
	Super::OnMouseButton(mouseX, mouseY, btn, pressed);
}

void EdViewport::OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY )
{
	Super::OnMouseMove( mouseX, mouseY, mouseDeltaX, mouseDeltaY );
}

void EdViewport::OnMouseWheel( int scroll )
{
	Super::OnMouseWheel(scroll);
}

void EdViewport::GetSize( int &width, int &height ) const
{
	Assert(this->IsValid());
	static bool bUseWinAPI = false;
	//HOT_BOOL(bUseWinAPI);
	if (bUseWinAPI)
	{
		RECT	rect;
		::GetWindowRect( m_widget->winId(), &rect );
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	else
	{
		width = m_renderViewport.GetWidth();
		height = m_renderViewport.GetHeight();
	}
}

/*
-----------------------------------------------------------------------------
	EdSceneViewport
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( EdSceneViewport );

EdSceneViewport::EdSceneViewport()
{
	this->ClearState();
}

EdSceneViewport::~EdSceneViewport()
{

}

void EdSceneViewport::InitViewport( EdViewportWidget* widget )
{
	Super::InitViewport( widget );
}

void EdSceneViewport::CloseViewport()
{
	Super::CloseViewport();

	this->ClearState();
}

void EdSceneViewport::OnProjectLoaded()
{
	Super::OnProjectLoaded();
}

void EdSceneViewport::OnProjectUnloaded()
{
	Super::OnProjectUnloaded();

	this->ClearState();
}

void EdSceneViewport::ClearState()
{
	m_camera.ResetView();
	this->RecalcMatrices();

	objAtCursor = nil;
}

void EdSceneViewport::RecalcMatrices()
{
	this->viewMatrix = m_camera.GetView().CreateViewMatrix();
	this->projectionMatrix = m_camera.GetView().CreateProjectionMatrix();
	this->viewProjectionMatrix = XMMatrixMultiply( viewMatrix, projectionMatrix );
	this->inverseViewMatrix = m_camera.GetView().GetInverseViewMatrix();
}

void EdSceneViewport::Tick( FLOAT deltaSeconds )
{
	m_camera.SetAspectRatio( m_renderViewport.GetAspectRatio() );
	m_camera.Update( deltaSeconds );

	this->RecalcMatrices();

	EdSystem::Get().Event_TickViewport( *this, deltaSeconds );
}

void EdSceneViewport::OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
{
	Super::OnResize(newWidth,newHeight,bFullScreen);
	m_camera.SetAspectRatio( m_renderViewport.GetAspectRatio() );
}

void EdSceneViewport::OnClose()
{
	Super::OnClose();
}

void EdSceneViewport::OnPaint()
{
	Super::OnPaint();
}

void EdSceneViewport::OnKeyPressed( EKeyCode key )
{
	Super::OnKeyPressed( key );

	if( !EditorApp::IsProjectLoaded() ) {
		return;
	}

	m_camera.OnKeyPressed( key );

	EdSystem::Get().Event_KeyPressed( *this, key );
}

void EdSceneViewport::OnKeyReleased( EKeyCode key )
{
	Super::OnKeyReleased( key );

	if( !EditorApp::IsProjectLoaded() ) {
		return;
	}

	m_camera.OnKeyReleased( key );

	EdSystem::Get().Event_KeyReleased( *this, key );
}

void EdSceneViewport::OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed )
{
	Super::OnMouseButton(mouseX, mouseY, btn, pressed);

	if( !EditorApp::IsProjectLoaded() ) {
		return;
	}

	if( btn == EMouseButton::LeftMouseButton && pressed )
	{
		dragStartPosition.setX(mouseX);
		dragStartPosition.setY(mouseY);
	}

	objAtCursor = this->GetHitProxy( mouseX, mouseY );
	EdSystem::Get().Event_MouseButton( *this, mouseX, mouseY, btn, pressed );
}

void EdSceneViewport::OnMouseMove(  int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY )
{
	Super::OnMouseMove( mouseX, mouseY, mouseDeltaX, mouseDeltaY );

	if( !EditorApp::IsProjectLoaded() ) {
		return;
	}

	objAtCursor = this->GetHitProxy( mouseX, mouseY );

	//if(IsMouseButtonPressed(EMouseButton::RightMouseButton))
	//if(IsMouseButtonPressed(EMouseButton::LeftMouseButton))
	if(IsMouseButtonPressed(EMouseButton::MiddleMouseButton))
	{
		m_camera.OnMouseMove( mouseDeltaX, mouseDeltaY );
	}

	EdSystem::Get().Event_MouseMove( *this, mouseX, mouseY, mouseDeltaX, mouseDeltaY );
}

void EdSceneViewport::OnMouseWheel( int scroll )
{
	Super::OnMouseWheel(scroll);

	if( !EditorApp::IsProjectLoaded() ) {
		return;
	}

	int mouseX, mouseY;
	this->GetMouseCursorPos( mouseX, mouseY );
	objAtCursor = this->GetHitProxy( mouseX, mouseY );

	EdSystem::Get().Event_MouseWheel( *this, scroll );
}

/*
-----------------------------------------------------------------------------
	EdViewportWidget
-----------------------------------------------------------------------------
*/
EdViewportWidget::EdViewportWidget( QWidget* parent )
	: Super( parent )
{
	this->setMinimumSize( 1, 1 );

	this->setWindowTitle("Viewport");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	//Accept input focus
	this->setFocusPolicy( Qt::FocusPolicy::StrongFocus );

	this->setAcceptDrops(true);

	this->setAutoFillBackground( false );

	// If mouse tracking is enabled, the widget receives mouse move events even if no buttons are pressed.
	this->setMouseTracking(true);

	QPalette colourPalette = this->palette();
	colourPalette.setColor(QPalette::Active, QPalette::WindowText, Qt::black);
	colourPalette.setColor(QPalette::Active, QPalette::Window, Qt::black);
	this->setPalette(colourPalette);

	m_lastCursorPos.SetZero();
}
//-----------------------------------------------------------------------------
EdViewportWidget::~EdViewportWidget()
{
	if( m_viewport != nil )
	{
		m_viewport->CloseViewport();
	}
}
//-----------------------------------------------------------------------------
void EdViewportWidget::SetViewport( EdViewport* newViewport )
{
	if( m_viewport != nil )
	{
		m_viewport->CloseViewport();
	}

	const bool bRender = ( newViewport != nil );

	// don't let Qt clear the canvas
	this->setAttribute( Qt::WA_OpaquePaintEvent, bRender );
	this->setAttribute( Qt::WA_PaintOnScreen, bRender );
	this->setAttribute( Qt::WA_NoSystemBackground, bRender );

	if( newViewport != nil )
	{
		newViewport->InitViewport( this );
	}

	m_viewport = newViewport;
}
//-----------------------------------------------------------------------------
EdViewport* EdViewportWidget::GetViewport()
{
	return m_viewport;
}
//-----------------------------------------------------------------------------
QPaintEngine* EdViewportWidget::paintEngine() const
{
	if( m_viewport != nil ) {
		return nil;
	}
	return Super::paintEngine();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::paintEvent( QPaintEvent* theEvent )
{
	// you can issue draw calls right here

	if( m_viewport == nil ) {
		Super::paintEvent( theEvent );
	} else {
		m_viewport->OnPaint();
	}
}
//-----------------------------------------------------------------------------
void EdViewportWidget::keyPressEvent( QKeyEvent* theEvent )
{
	if( m_viewport != nil )
	{
		const EKeyCode keyCode = F_MapQtKey( theEvent->key() );

		if( keyCode < NumKeyCodes ) {
			m_viewport->OnKeyPressed( keyCode );
		}
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::keyReleaseEvent( QKeyEvent* theEvent )
{
	if( m_viewport != nil )
	{
		const EKeyCode keyCode = F_MapQtKey( theEvent->key() );

		if( keyCode < NumKeyCodes ) {
			m_viewport->OnKeyReleased( keyCode );
		}
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::mousePressEvent( QMouseEvent* theEvent )
{
	if( m_viewport != nil )
	//if( theEvent->buttons() & Qt::MouseButton::RightButton )
	{
		m_lastCursorPos.x = theEvent->posF().x();
		m_lastCursorPos.y = theEvent->posF().y();

		const EMouseButton btn = F_MapQtMouseButton( theEvent->button() );

		m_viewport->OnMouseButton( theEvent->pos().x(), theEvent->pos().y(), btn, true );
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::mouseReleaseEvent( QMouseEvent* theEvent )
{
	if( m_viewport != nil )
	//if( theEvent->buttons() & Qt::MouseButton::RightButton )
	{
		m_lastCursorPos.x = theEvent->posF().x();
		m_lastCursorPos.y = theEvent->posF().y();

		const EMouseButton btn = F_MapQtMouseButton( theEvent->button() );

		m_viewport->OnMouseButton( theEvent->pos().x(), theEvent->pos().y(), btn, false );
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::mouseMoveEvent( QMouseEvent* theEvent )
{
	if( m_viewport != nil )
	//if( theEvent->buttons() & Qt::MouseButton::RightButton )
	{
		// Get current position of mouse
		Vec2D ptCurMousePos;
		ptCurMousePos.x = theEvent->posF().x();
		ptCurMousePos.y = theEvent->posF().y();

		Vec2D deltaPos = m_lastCursorPos - ptCurMousePos;
		m_lastCursorPos = ptCurMousePos;

		m_viewport->OnMouseMove( theEvent->pos().x(), theEvent->pos().y(), deltaPos.x, deltaPos.y );
	}

	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::wheelEvent( QWheelEvent* theEvent )
{
	if( m_viewport != nil )
	{
		m_lastCursorPos.x = theEvent->pos().x();
		m_lastCursorPos.y = theEvent->pos().y();

		m_viewport->OnMouseWheel( theEvent->delta() );
	}

	theEvent->accept();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::resizeEvent( QResizeEvent* theEvent )
{
	////DBG_TRACE_CALL;
	if( m_viewport != nil )
	{
		const QSize& newSize = theEvent->size();
		m_viewport->OnResize( newSize.width(), newSize.height(), false );
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------
bool EdViewportWidget::winEvent( MSG * message, long * result )
{
#if 0	// glitchy: commented out because it causes strange graphics artifacts
	if( WM_PAINT == message->message )
	{
		if( m_viewport != nil )
		{
			m_viewport->OnPaint();
			return true;
		}
	}
#endif

	// If you return false, this native theEvent is passed back to Qt,
	// which translates the theEvent into a Qt theEvent and sends it to the widget.

	return false;

	//return Super::winEvent( message, result );
}
//------------------------------------------------------------------------------
void EdViewportWidget::focusInEvent(QFocusEvent* theEvent)
{
	Super::focusInEvent( theEvent );
}
//------------------------------------------------------------------------------
void EdViewportWidget::focusOutEvent(QFocusEvent* theEvent)
{
	Super::focusOutEvent( theEvent );
}
//-----------------------------------------------------------------------------
void EdViewportWidget::dragEnterEvent(QDragEnterEvent *theEvent)
{
	if( m_viewport != nil )
	{
		const QMimeData* pMimeData = qobject_cast< const QMimeData* >( theEvent->mimeData() );
		if( pMimeData != nil )
		{
			const QPoint mouseCursor = theEvent->pos();
			if( m_viewport->AcceptsDrop( pMimeData, mouseCursor.x(), mouseCursor.y() ) )
			{
				theEvent->acceptProposedAction();
				return;
			}
		}
	}
	theEvent->ignore();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::dropEvent(QDropEvent *theEvent)
{
	if( m_viewport != nil )
	{
		const QMimeData* pMimeData = qobject_cast< const QMimeData* >( theEvent->mimeData() );
		if( pMimeData != nil )
		{
			const QPoint mouseCursor = theEvent->pos();
			return m_viewport->OnDropEvent( pMimeData, mouseCursor.x(), mouseCursor.y() );
		}
	}
	theEvent->acceptProposedAction();
}
//-----------------------------------------------------------------------------
void EdViewportWidget::closeEvent( QCloseEvent* theEvent )
{
	if( m_viewport != nil )
	{
		m_viewport->OnClose();
	}
	theEvent->accept();
}
//-----------------------------------------------------------------------------



/*
-----------------------------------------------------------------------------
	EdViewportClient
-----------------------------------------------------------------------------
*/
EdViewportClient::EdViewportClient()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_KeyPressed, EdViewportClient, OnKeyPressed );
	MX_CONNECT_THIS( EdSystem::Get().Event_KeyReleased, EdViewportClient, OnKeyReleased );
	MX_CONNECT_THIS( EdSystem::Get().Event_MouseButton, EdViewportClient, OnMouseButton );
	MX_CONNECT_THIS( EdSystem::Get().Event_MouseMove, EdViewportClient, OnMouseMove );
	MX_CONNECT_THIS( EdSystem::Get().Event_MouseWheel, EdViewportClient, OnMouseWheel );

	MX_CONNECT_THIS( EdSystem::Get().Event_TickViewport, EdViewportClient, OnUpdate );
}

EdViewportClient::~EdViewportClient()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_KeyPressed );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_KeyReleased );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_MouseButton );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_MouseMove );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_MouseWheel );

	MX_DISCONNECT_THIS( EdSystem::Get().Event_TickViewport );
}

/*
-----------------------------------------------------------------------------
	EdDrawable
-----------------------------------------------------------------------------
*/
EdDrawable::EdDrawable()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_DrawBackground, EdDrawable, DrawBackground );
	MX_CONNECT_THIS( EdSystem::Get().Event_Draw, EdDrawable, Draw );
	MX_CONNECT_THIS( EdSystem::Get().Event_DrawForeground, EdDrawable, DrawForeground );

	MX_CONNECT_THIS( EdSystem::Get().Event_DrawBackgroundHitProxy, EdDrawable, DrawBackgroundHitProxy );
	MX_CONNECT_THIS( EdSystem::Get().Event_DrawHitProxy, EdDrawable, DrawHitProxy );
	MX_CONNECT_THIS( EdSystem::Get().Event_DrawForegroundHitProxy, EdDrawable, DrawForegroundHitProxy );
}

EdDrawable::~EdDrawable()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_DrawBackground );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_Draw );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_DrawForeground );

	MX_DISCONNECT_THIS( EdSystem::Get().Event_DrawBackgroundHitProxy );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_DrawHitProxy );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_DrawForegroundHitProxy );
}
