#pragma once

#include <Core/App/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Editor/HitTesting.h>
#include <Renderer/Editor/EdGraphics.h>
#include "editor_common.h"

class rxViewport;
class EdMimeData;
class EdViewportWidget;

EKeyCode F_MapQtKey( int keyCode );
EMouseButton F_MapQtMouseButton( Qt::MouseButton btn );

// in widget's local space
QPoint GetMouseCursorPosition( QWidget* w );



struct EdViewportContext
{
	TPtr<EdViewport>	pViewport;

	rxView			view;
	Matrix4			viewMatrix;
	Matrix4			projectionMatrix;

	World *			pWorld;
	AHitProxy *		pHitEntity;
	AHitProxy *		pPrevHitEntity;
};

struct EdKeyEvent
{
};
struct EdMouseEvent
{
};
struct EdMouseButtonEvent : EdMouseEvent
{
	QMouseEvent	q;
};
struct EdMouseMoveEvent : EdMouseEvent
{
	QMouseEvent	q;
};
struct EdMouseDragEvent : EdMouseEvent
{
	QMouseEvent	q;
};
struct EdMouseWheelEvent : EdMouseEvent
{
	QWheelEvent	q;
};

/*
-----------------------------------------------------------------------------
	EdViewport
-----------------------------------------------------------------------------
*/
class EdViewport : public ClientViewport, public EdSystemChild
{
	mxDECLARE_ABSTRACT_CLASS( EdViewport, ClientViewport );

public:
	EdViewport();
	~EdViewport();

	// EdViewport interface

	virtual void InitViewport( EdViewportWidget* widget );
	virtual void CloseViewport();

	virtual bool IsSceneViewport() const { return false; }

public:	//-- ClientViewport
	//virtual void Tick( FLOAT deltaSeconds ) = 0;
	//virtual void Draw() = 0;

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;
	virtual void OnClose() override;

	virtual void OnPaint() override;

	//-- InputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

	virtual void OnMouseButton( int x, int y, EMouseButton btn, bool pressed ) override;
	virtual void OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY ) override;
	virtual void OnMouseWheel( int scroll ) override;

public:	// Editor
	virtual bool AcceptsDrop( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) {return true;}
	virtual void OnDropEvent( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) {}

public:
	bool IsValid() const;

	AHitProxy* GetHitProxy( const int iX, const int iY ) const;

	void GetSize( int &width, int &height ) const;

	void ClearViewport( const FColor& color = FColor::BLACK );


	const rxViewport& GetRenderViewport() const
	{
		return m_renderViewport;
	}

public:	//--EdProjectChild
	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

protected:
	rxViewport	m_renderViewport;
	TPtr<EdViewportWidget>	m_widget;
};


/*
-----------------------------------------------------------------------------
	EdSceneViewport
-----------------------------------------------------------------------------
*/
class EdSceneViewport : public EdViewport
{
	mxDECLARE_ABSTRACT_CLASS( EdSceneViewport, EdViewport );

public:
	EdSceneViewport();
	~EdSceneViewport();

	// EdViewport interface

	virtual void InitViewport( EdViewportWidget* widget ) override;
	virtual void CloseViewport() override;
	virtual bool IsSceneViewport() const override { return true; }

public:	//-- ClientViewport

	virtual void Tick( FLOAT deltaSeconds ) override;

	//virtual void Draw() = 0;

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;
	virtual void OnClose() override;

	virtual void OnPaint() override;

	//-- InputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

	virtual void OnMouseButton( int x, int y, EMouseButton btn, bool pressed ) override;
	virtual void OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY ) override;
	virtual void OnMouseWheel( int scroll ) override;

public:	// Editor
	virtual bool AcceptsDrop( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) {return true;}
	virtual void OnDropEvent( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) {}

public:	//--EdProjectChild
	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

public:
	const rxView& GetView() const
	{
		return m_camera.GetView();
	}

private:
	void RecalcMatrices();
	void ClearState();

protected:
	FPSCamera		m_camera;

public:	// precomputes
	float4x4	viewMatrix;
	float4x4	projectionMatrix;
	float4x4  	viewProjectionMatrix;
	float4x4	inverseViewMatrix;
	AHitProxy *	objAtCursor;
	QPoint		dragStartPosition;
};

/*
-----------------------------------------------------------------------------
	EdViewportWidget
	widget for graphics output 
-----------------------------------------------------------------------------
*/
class EdViewportWidget : public QWidget
{
	Q_OBJECT

public:
	typedef QWidget Super;

	EdViewportWidget( QWidget* parent = nil );
	~EdViewportWidget();

	// automatically initializes/closes the viewport
	void SetViewport( EdViewport* newViewport = nil );

	EdViewport* GetViewport();

public:
	virtual QPaintEngine* paintEngine() const;

protected:
	virtual void paintEvent( QPaintEvent* theEvent );

	virtual void keyPressEvent( QKeyEvent* theEvent ); 
	virtual void keyReleaseEvent( QKeyEvent* theEvent ); 
	virtual void mousePressEvent( QMouseEvent* theEvent ); 
	virtual void mouseReleaseEvent( QMouseEvent* theEvent ); 
	virtual void mouseMoveEvent( QMouseEvent* theEvent );
	virtual void wheelEvent( QWheelEvent* theEvent );

	virtual void resizeEvent( QResizeEvent* theEvent );
	virtual bool winEvent( MSG * message, long * result );

	virtual void focusInEvent(QFocusEvent* theEvent);
	virtual void focusOutEvent(QFocusEvent* theEvent);

	virtual void dragEnterEvent(QDragEnterEvent* theEvent);
	virtual void dropEvent(QDropEvent* theEvent);

	virtual void closeEvent( QCloseEvent* theEvent );

private:
	TPtr< EdViewport >	m_viewport;

	// saved position of mouse cursor, relative to this widget
	Vec2D	m_lastCursorPos;
};





FORCEINLINE
bool IsDragging( const ClientViewport& viewport )
{
	return viewport.IsMouseButtonPressed(EMouseButton::LeftMouseButton);
}

inline void PointToNDC( const EdSceneViewport& viewport, const int iX, const int iY, Vec2D &OutP )
{
	ViewportToClip(
		viewport.GetRenderViewport(),
		as_matrix4(viewport.projectionMatrix),
		iX, iY,
		OutP.x, OutP.y );
}

inline Ray3D GetEyeRay( const EdSceneViewport& viewport, const Vec2D& xyNDC )
{
	return Ray3D(
		viewport.GetView().origin,
		as_matrix4(viewport.inverseViewMatrix).TransformNormal(Vec3D( xyNDC, 1.0f )).GetNormalized()
	);
}

// calculates ray for picking
inline Ray3D GetEyeRay( const EdSceneViewport& viewport, const int iX, const int iY )
{
	Vec2D	pickPosNDC;
	PointToNDC( viewport, iX, iY, pickPosNDC );

	return GetEyeRay(
		viewport, pickPosNDC
	);
}

inline Ray3D GetEyeRay( const EdSceneViewport& viewport )
{
	return GetEyeRay( viewport, viewport.GetMouseX(), viewport.GetMouseY() );
}



MX_SWIPED("DXSDK, DXUT, CD3DArcBall");
inline
Vec3D ConvertScreenPointToVector( const EdSceneViewport& viewport, const int iX, const int iY, const float radius = 1.0f )
{
	// window offset, or upper-left corner of window
	FLOAT offsetX = 0.0f;
	FLOAT offsetY = 0.0f;
	FLOAT halfWidth = 0.5f * viewport.GetRenderViewport().GetWidth();
	FLOAT halfHeight = 0.5f * viewport.GetRenderViewport().GetHeight();

	// Scale to screen
	FLOAT x = -( iX - offsetX - halfWidth ) / ( radius * halfWidth );
	FLOAT y = ( iY - offsetY - halfHeight ) / ( radius * halfHeight );

	FLOAT z = 0.0f;
	FLOAT mag = x * x + y * y;

	if( mag > 1.0f )
	{
		FLOAT scale = 1.0f / mxSqrt( mag );
		x *= scale;
		y *= scale;
	}
	else
		z = mxSqrt( 1.0f - mag );

	// Return vector
	return Vec3D( x, y, z );
}



/*
-----------------------------------------------------------------------------
	EdViewportClient
-----------------------------------------------------------------------------
*/
struct EdViewportClient
{
	virtual void OnKeyPressed( const EdSceneViewport& viewport, const EKeyCode key ) {};
	virtual void OnKeyReleased( const EdSceneViewport& viewport, const EKeyCode key ) {};

	virtual void OnMouseButton( const EdSceneViewport& viewport, const int mouseX, const int mouseY, const EMouseButton btn, const bool pressed ) {};
	virtual void OnMouseMove( const EdSceneViewport& viewport, const int mouseDeltaX, const int mouseDeltaY, const int mouseX, const int mouseY ) {};
	virtual void OnMouseWheel( const EdSceneViewport& viewport, const int scroll ) {};

	virtual void OnUpdate( const EdSceneViewport& viewport, FLOAT deltaSeconds ) {};

protected:
	// automatic connection/disconnection
	EdViewportClient();
	virtual ~EdViewportClient();
};


// passed to each EdDrawable
struct EdDrawContext
{
	const EdSceneViewport &	viewport;
	const rxSceneContext &	sceneContext;

	BatchRenderer &	renderer;
	AHitTesting &	hitTesting;

public:
	EdDrawContext(
		const EdSceneViewport& viewport,
		const rxSceneContext& sceneContext,
		BatchRenderer& renderer,
		AHitTesting& hitTesting
		)
		: viewport(viewport)
		, sceneContext(sceneContext)
		, hitTesting(hitTesting)
		, renderer(renderer)
	{
	}
};

/*
-----------------------------------------------------------------------------
	EdDrawable
-----------------------------------------------------------------------------
*/
struct EdDrawable
{
	virtual void DrawBackground( const EdDrawContext& context ) {};
	virtual void Draw( const EdDrawContext& context ) {};
	virtual void DrawForeground( const EdDrawContext& context ) {};

	virtual void DrawBackgroundHitProxy( const EdDrawContext& context )
	{
		MX_UNUSED(context);
	}
	virtual void DrawHitProxy( const EdDrawContext& context )
	{
		MX_UNUSED(context);
	}
	virtual void DrawForegroundHitProxy( const EdDrawContext& context )
	{
		this->DrawForeground( context );
	}

protected:
	// automatic connection/disconnection
	EdDrawable();
	virtual ~EdDrawable();
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
