#include "stdafx.h"

#include <Renderer/Debugger.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/Util/ScreenQuad.h>
#include <Engine/Engine.h>

#include "main_window_frame.h"
#include "app.h"

#include "debugger.h"

/*
-----------------------------------------------------------------------------
	EdViewport
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( EdViewport_TextureView );

EdViewport_TextureView::EdViewport_TextureView()
{

}

EdViewport_TextureView::~EdViewport_TextureView()
{

}

void EdViewport_TextureView::InitViewport( EdViewportWidget* widget )
{
	Super::InitViewport( widget );

	MX_CONNECT_THIS( EdSystem::Get().Event_RenderViewports, EdViewport_TextureView, Draw );
}

void EdViewport_TextureView::CloseViewport()
{
	Super::CloseViewport();

	m_currentTexture.Clear();
}

void EdViewport_TextureView::SetCurrentTexture( PCSTR pTexture )
{
	m_currentTexture.SetString( pTexture );
}

void EdViewport_TextureView::OnPaint()
{
	if( EditorApp::IsProjectLoaded() )
	{
		this->Draw();
	}
}

//void EdViewport_TextureView::OnObjectDestroyed( Editable* theObject )
//{
//	ATexturePreview* pTexture = gRenderer.debugger->GetTextureInspector()->GetTextureByName(m_currentTexture.ToChars());
//	if( m_currentTexture == theObject )
//	{
//		m_currentTexture == nil;
//	}
//}

void EdViewport_TextureView::Draw()
{
	if( m_renderViewport.IsNull() )
	{
		return;
	}

	rxRenderer & renderer = gRenderer;

	rxFrameContext	frameContext;
	frameContext.clearColor = FColor::BLACK;

	renderer.BeginScene( &m_renderViewport, frameContext );

	ATexturePreview* pTexture = gRenderer.debugger->GetTextureInspector()->GetTextureByName( m_currentTexture.ToChars() );

	if( pTexture != nil )
	{
		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

		const GrViewport & vp = m_renderViewport;

		pD3DContext->OMSetRenderTargets( 1, &vp.mainRT.pRTV.Ptr, nil );

		{
			UINT	rtW, rtH;
			pTexture->GetSize( rtW, rtH );

			m_widget->resize( rtW, rtH );
		}


#if 0
		FLOAT	vpW, vpH;
		vpW = vp.GetWidth();
		vpH = vp.GetHeight();

		// calculate viewport size so that the texture fits into the view
		const FLOAT ratioW = vpW / rtW;
		const FLOAT ratioH = vpH / rtH;
		const FLOAT minRatio = minf( ratioW, ratioH );


		D3D11_VIEWPORT	newD3DViewport;
		newD3DViewport.TopLeftX = 0;
		newD3DViewport.TopLeftY = 0;
		newD3DViewport.Width = rtW * minRatio;
		newD3DViewport.Height = rtH * minRatio;
		newD3DViewport.MinDepth = 0;
		newD3DViewport.MaxDepth = 1;

		pD3DContext->RSSetViewports( 1, &newD3DViewport );
#else
		pD3DContext->RSSetViewports( 1, &m_renderViewport.d );
#endif

		ID3D11ShaderResourceView* pSRV = pTexture->GetSRV();

		GPU::SolidNoCullNoZTestNoClipNoBlend.Set( pD3DContext );

		DrawFullScreenTriangle( pD3DContext, pSRV );

		// draw a white border around the texture
		//{
		//	const FLOAT	topLeftX = 0;
		//	const FLOAT	topLeftY = 0;
		//	const FLOAT	bottomRightX = newD3DViewport.Width;
		//	const FLOAT	bottomRightY = newD3DViewport.Height;

		//	pD3DContext->RSSetViewports( 1, &m_renderViewport.vp.d );

		//	Transform_ViewportToClipSpace();
		//}

	}
	renderer.EndScene();
}

void EdViewport_TextureView::OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
{
	// Don't resize
}

void EdViewport_TextureView::OnKeyPressed( EKeyCode key )
{
	//TList<R8G8B8A8>	pixels(EMemHeap::HeapTempStack);
	//UINT maxSizeX, maxSizeY;

	//gRenderer.debugger->ReadPixels( pixels, maxSizeX, maxSizeY );
}



EdTextureInspectorTreeView::EdTextureInspectorTreeView( QWidget *parent )
	: Super( parent )
{
	this->setWindowTitle("Texture Inspector");
	this->setObjectName("EdTextureInspectorTreeView");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
}

EdTextureInspectorTreeView::~EdTextureInspectorTreeView()
{
}

void EdTextureInspectorTreeView::OnItemClicked( AEditable* pObject )
{
	this->OnItemSelected( pObject );
}

void EdTextureInspectorTreeView::OnItemDoubleClicked( AEditable* pObject )
{
	this->OnItemSelected( pObject );
}

void EdTextureInspectorTreeView::OnItemSelected( AEditable* pObject )
{
	EdWidget_TextureInspector::Get().OnItemDoubleClicked( pObject );
}

EdTextureInspectorGraphicsView::EdTextureInspectorGraphicsView( QWidget *parent )
	: Super( parent )
{
	this->setBackgroundRole(QPalette::Dark);
	this->setWidget(&m_viewportWidget);

	m_viewportWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_viewportWidget.resize(512,512);
}

EdTextureInspectorGraphicsView::~EdTextureInspectorGraphicsView()
{

}

void EdTextureInspectorGraphicsView::Initialize()
{
	m_viewportWidget.SetViewport(&m_viewportTextureView);
}

void EdTextureInspectorGraphicsView::Shutdown()
{
	m_viewportWidget.SetViewport(nil);
}

void EdTextureInspectorGraphicsView::SetCurrentTexture( ATexturePreview* pTexture )
{
	UINT	rtW, rtH;
	pTexture->GetSize( rtW, rtH );

	m_viewportWidget.resize( rtW, rtH );

	m_viewportTextureView.SetCurrentTexture(pTexture->GetName());
}

/*
-----------------------------------------------------------------------------
	EdWidget_TextureInspector
-----------------------------------------------------------------------------
*/
EdWidget_TextureInspector::EdWidget_TextureInspector( QWidget *parent )
	: Super( parent )
{
	this->setWindowTitle("Texture Inspector");
	this->setObjectName("EdWidget_TextureInspector");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	this->addWidget(&m_treeView);
	this->addWidget(&m_graphicsView);

	MX_CONNECT_THIS( gRenderer.BeforeMainViewportResized, EdWidget_TextureInspector, OnBeforeMainViewportResized );
	MX_CONNECT_THIS( gRenderer.AfterMainViewportResized, EdWidget_TextureInspector, OnAfterMainViewportResized );
}

EdWidget_TextureInspector::~EdWidget_TextureInspector()
{
	MX_DISCONNECT_THIS( gRenderer.BeforeMainViewportResized );
	MX_DISCONNECT_THIS( gRenderer.AfterMainViewportResized );
}

void EdWidget_TextureInspector::Initialize()
{
	m_graphicsView.Initialize();
}

void EdWidget_TextureInspector::Shutdown()
{
	m_graphicsView.Shutdown();
}

void EdWidget_TextureInspector::OnProjectLoaded()
{
	m_model.SetRootObject( gRenderer.debugger->GetTextureInspector() );
	m_treeView.setModel(&m_model);
}

void EdWidget_TextureInspector::OnProjectUnloaded()
{
}

//void EdWidget_TextureInspector::OnItemClicked( Editable* pObject )
//{
//	this->OnItemDoubleClicked( pObject );
//}

void EdWidget_TextureInspector::OnItemDoubleClicked( AEditable* pObject )
{
	ATexturePreview* pTexturePreview = SafeCast<ATexturePreview>( pObject );
	if( pTexturePreview != nil )
	{
		m_graphicsView.SetCurrentTexture(pTexturePreview);

		this->setWindowTitle(pTexturePreview->edToChars(0));
	}
}

void EdWidget_TextureInspector::OnBeforeMainViewportResized( UINT newWidth, UINT newHeight )
{
	m_model.SetRootObject( nil );
	m_treeView.setModel( nil );
}

void EdWidget_TextureInspector::OnAfterMainViewportResized( UINT newWidth, UINT newHeight )
{
	m_model.SetRootObject( gRenderer.debugger->GetTextureInspector() );
	m_treeView.setModel( &m_model );
}



EdWidget_EngineTreeView::EdWidget_EngineTreeView( QWidget *parent )
	: Super( parent )
{
}

EdWidget_EngineTreeView::~EdWidget_EngineTreeView()
{
}

void EdWidget_EngineTreeView::OnItemSelected( AEditable* pObject )
{
	EdDebugger::Get().m_engineProperties.SetObject( pObject );
}

EdDockWidget_EngineView::EdDockWidget_EngineView( QWidget *parent )
	: Super( parent )
{
	this->setWindowTitle("Engine");
	this->setObjectName("EdDockWidget_EngineView");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );

	Super::setWidget(&m_engineTreeView);

	m_engineTreeView.header()->hide();


	//m_engineViewModel.SetRootObject(&gE);
	m_engineTreeView.setModel(&m_engineViewModel);
}

EdDockWidget_EngineView::~EdDockWidget_EngineView()
{

}

/*
-----------------------------------------------------------------------------
	EdDebugger
-----------------------------------------------------------------------------
*/
EdDebugger::EdDebugger()
	: QMainWindow(EditorApp::GetMainFrame())
{
	this->setWindowTitle(this->GetModuleName());
	this->setObjectName(this->GetModuleName());
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
	this->hide();
}

EdDebugger::~EdDebugger()
{
}

void EdDebugger::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdDebugger::SetDefaultValues()
{
	//QMainWindow* mainWindow = GetMainFrame();

	this->resize(800,600);
}

void EdDebugger::PostInit()
{
	m_textureInspector.Initialize();
}

void EdDebugger::Shutdown()
{
	m_textureInspector.Shutdown();
}

void EdDebugger::CreateWidgets()
{
	this->addDockWidget(Qt::LeftDockWidgetArea,&m_engineViewDockWidget);

	this->setCentralWidget(&m_engineProperties);
}

void EdDebugger::CreateActions()
{
	m_showDebuggerAct.setText( tr("Engine Subsystems") );
	m_showTextureInspectorAct.setText( tr("Texture Inspector") );
	m_showMemoryMapAct.setText( tr("Heap Inspector") );
}

void EdDebugger::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.debugMenu->addAction( &m_showDebuggerAct );
	menus.debugMenu->addAction( &m_showTextureInspectorAct );
	menus.debugMenu->addAction( &m_showMemoryMapAct );
}

void EdDebugger::ConnectSigSlots()
{
	EditorApp::ShowOnActionTriggered( &m_showDebuggerAct, this );
	EditorApp::ShowOnActionTriggered( &m_showTextureInspectorAct, &m_textureInspector );
	MX_CONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded, EdDebugger, OnProjectUnloaded );
}

void EdDebugger::OnProjectUnloaded()
{
	gRenderer.debugger->Clear();
}

void EdDebugger::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	SerializeMainWindow_GeometryAndState(stream,this);

	SerializeWidgetGeometry(stream,&m_textureInspector);
}
