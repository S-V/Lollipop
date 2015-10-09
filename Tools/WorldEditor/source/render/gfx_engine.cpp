// @fixme: cannot (de)serialize viewport geometry properly
//
#include "stdafx.h"

#include "editor_app.h"
#include "render/gfx_engine.h"
#include "render/texture_inspector.h"
#include "render/hit_testing.h"
#include "render/viewports.h"
#include "render/main_viewport.h"

/*
-----------------------------------------------------------------------------
	EdRenderer
-----------------------------------------------------------------------------
*/
namespace
{
	struct EdGfxData
	{
		EdTextureInspector		m_textureInspector;
		HitTesting				m_hitTesting;

		EdMainViewport			m_mainViewport;
		EdMainViewportWidget 	m_mainViewportWidget;
		TPtr< QDockWidget >		m_mainViewportDockWidget;

		QAction		m_actShowMainViewport;

	public:
		EdGfxData()
			: m_actShowMainViewport("Main Viewport",nil)
		{
		}
	};
	static TPtr<EdGfxData>	gData;

}//namespace

EdRenderer::EdRenderer()
{
	gData.ConstructInPlace();

	Assert(gRenderer.editor == nil);
	gRenderer.editor = this;
}

EdRenderer::~EdRenderer()
{
	Assert(gRenderer.editor == this);
	gRenderer.editor = nil;

	gData.Destruct();
}


static
void F_UpdateViewports()
{

}

static
void F_RenderViewports()
{

}

enum {
	DEFAULT_VIEWPORT_WIDTH = 800, DEFAULT_VIEWPORT_HEIGHT = 600,
	MIN_VIEWPORT_WIDTH = 256, MIN_VIEWPORT_HEIGHT = 256
};

static
void F_FixViewportSize( QWidget* viewportWidget )
{
	const QSize windowSize = viewportWidget->size();

	const UINT newWidth = largest( windowSize.width(), MIN_VIEWPORT_WIDTH );
	const UINT newHeight = largest( windowSize.height(), MIN_VIEWPORT_HEIGHT );

	viewportWidget->resize( newWidth, newHeight );
}

static
void F_CreateWidgets()
{
	QMainWindow* mainFrame = EditorApp::GetMainFrame();

	gData->m_mainViewportDockWidget = new QDockWidget( "Main Viewport", mainFrame );
	gData->m_mainViewportDockWidget->setWidget( &gData->m_mainViewportWidget );
	//mainFrame->setCentralWidget( gData->m_mainViewportDockWidget );
	//gData->m_mainViewportDockWidget->showMaximized();
	gData->m_mainViewportDockWidget->setFloating(false);
}

static
void F_CreateActions()
{
	gData->m_actShowMainViewport.setText( TR("Main Viewport") );
	gData->m_actShowMainViewport.setStatusTip( TR("View the main viewport") );
	gData->m_actShowMainViewport.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_L ) );
}

static
void F_CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &gData->m_actShowMainViewport );
}

static
void F_ConnectSigSlots()
{
	//EditorApp::ValidOnlyWhenProjectIsLoaded( &gData->m_actShowMainViewport );
	EditorApp::ShowOnActionTriggered( &gData->m_actShowMainViewport, gData->m_mainViewportDockWidget );
}

void EdRenderer::PreInit()
{
	F_CreateWidgets();
	F_CreateActions();
	F_CreateMenus();
	F_ConnectSigSlots();
	mxCONNECT_THIS( EdSystem::Get().Event_UpdateState, EdRenderer, UpdateViewports );
	mxCONNECT_THIS( EdSystem::Get().Event_RenderViewports, EdRenderer, RenderViewports );
}

void EdRenderer::SetDefaultValues()
{
	QMainWindow* mainFrame = EditorApp::GetMainFrame();
	mainFrame->setCentralWidget( gData->m_mainViewportDockWidget );
	gData->m_mainViewportDockWidget->showMaximized();
}

void EdRenderer::PostInit()
{
	//this->CreateViewports();
	F_FixViewportSize( &gData->m_mainViewportWidget );
	gData->m_mainViewportWidget.SetViewport( &gData->m_mainViewport );
}

void EdRenderer::Shutdown()
{
	//this->CloseViewports();
	gData->m_mainViewportWidget.SetViewport( nil );
}

void EdRenderer::SerializeAddData( ATextSerializer & serializer )
{

}

void EdRenderer::SerializeWidgetLayout( QDataStream & stream )
{
	//F_FixViewportSize( gData->m_mainViewportDockWidget );
	//SerializeWidgetGeometry( stream, gData->m_mainViewportDockWidget );
	//F_FixViewportSize( gData->m_mainViewportDockWidget );

	QMainWindow* mainFrame = EditorApp::GetMainFrame();
	mainFrame->setCentralWidget( gData->m_mainViewportDockWidget );
	gData->m_mainViewportDockWidget->showMaximized();	
}

void EdRenderer::UpdateViewports( const mxDeltaTime& deltaTime )
{
	const FLOAT deltaSeconds = deltaTime.fTime;

	// update the main viewport

	gData->m_mainViewport.TickViewport( deltaSeconds );
}

void EdRenderer::RenderViewports()
{
	// render the main viewport

	if( gData->m_mainViewportWidget.isVisible() )
	{
		gData->m_mainViewport.Draw();
	}
}

void EdRenderer::InitHitTesting( UINT viewportWidth, UINT viewportHeight )
{
	gData->m_hitTesting.Initialize( viewportWidth, viewportHeight );
}

void EdRenderer::CloseHitTesting()
{
	gData->m_hitTesting.Shutdown();
}

ATextureInspector* EdRenderer::GetTextureInspector()
{
	return &gData->m_textureInspector;
}

HitTesting& EdRenderer::GetHitTesting()
{
	return gData->m_hitTesting;
}

void EdRenderer::OnProjectUnloaded()
{
	GetHitTesting().Clear();
}

mxCOMPILER_PROBLEM("had to put impl here because of 'unresolved external symbol' error")
/*
-----------------------------------------------------------------------------
	EdTexturePreview
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( EdTexturePreview );

EdTexturePreview::EdTexturePreview( ID3D11Texture2D* theTexture, PCSTR name )
{
	theTexture->GetDesc(&m_texDesc);

	StackString		formatStr;
	formatStr.SetString(DXGIFormat_ToString(m_texDesc.Format));

	static const ConstCharPtr	dxgiFormatPrefix("DXGI_FORMAT_");
	formatStr.StripLeadingCaseless(dxgiFormatPrefix.ToChars(),dxgiFormatPrefix.Length());


	// @todo: all mip levels
	//const float size = (1.0f / 1024.0f) * m_texDesc.Width * m_texDesc.Height * DXGIFormat_GetElementSize(m_texDesc.Format);

	m_descStr.Format("%s: %dx%d, %s, %u mip(s)",
		name,
		m_texDesc.Width,m_texDesc.Height,
		formatStr.ToChars(),//DXGIFormat_ToString(m_texDesc.Format),
		m_texDesc.MipLevels
		);

	m_pTexture = theTexture;


	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	if( m_texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL )
	{
		if( m_texDesc.Format == DXGI_FORMAT_D32_FLOAT ) {
			srDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if( m_texDesc.Format == DXGI_FORMAT_R32_TYPELESS ) {
			srDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if( m_texDesc.Format == DXGI_FORMAT_R16_TYPELESS ) {
			srDesc.Format = DXGI_FORMAT_R16_FLOAT;
		}
		else {
			Unimplemented;
		}
	}
	else {
		srDesc.Format = m_texDesc.Format;
	}
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	dxchk( D3DDevice->CreateShaderResourceView( m_pTexture, &srDesc, &m_pSRV.Ptr ) );
}

EdTexturePreview::~EdTexturePreview()
{
	if( DEBUG_THE_DEBUGGER ) {
		DEVOUT("~EdTexturePreview: %s\n",m_descStr.ToChars());
	}
}

AEditable* EdTexturePreview::edGetParent()
{
	return &gData->m_textureInspector;
}

/*
-----------------------------------------------------------------------------
	EdTextureInspector
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(EdTextureInspector);

EdTextureInspector::EdTextureInspector()
{
}

EdTextureInspector::~EdTextureInspector()
{
}

void EdTextureInspector::AddDebugItem( ID3D11Texture2D* pTexture, PCSTR name )
{
#if DEBUG_THE_DEBUGGER
	const UINT numTextures = this->Num();
	for( UINT iTexture = 0; iTexture < numTextures; iTexture++ )
	{
		EdTexturePreview* pInfo = UpCast< EdTexturePreview >( this->At( iTexture ) );

		if( pInfo->m_pTexture.Ptr == pTexture )
		{
			mxDEBUG_BREAK;
			return;
		}
	}
#endif // DEBUG_THE_DEBUGGER

	this->Add( new EdTexturePreview( pTexture, name ) );

	this->uiRefreshView();
}

void EdTextureInspector::RemoveDebugItem( ID3D11Texture2D* pTexture )
{
	const UINT numTextures = this->Num();
	for( UINT iTexture = 0; iTexture < numTextures; iTexture++ )
	{
		EdTexturePreview* pInfo = UpCast< EdTexturePreview >( this->At( iTexture ) );

		if( pInfo->m_pTexture.Ptr == pTexture )
		{
#if DEBUG_THE_DEBUGGER
			Assert( pInfo->edBelongsToTree(this) );
			this->DbgDump();
			DBGOUT(" == Removing texture[%u]: %s\n", iTexture,pInfo->GetName());
#endif // DEBUG_THE_DEBUGGER
			this->Remove( pInfo );

			//this->uiRefreshView();
			return;
		}
	}
}

ATexturePreview* EdTextureInspector::GetTextureByName( PCSTR name )
{
	const UINT numTextures = this->Num();
	for( UINT iTexture = 0; iTexture < numTextures; iTexture++ )
	{
		EdTexturePreview* pTexturePreview = UpCast< EdTexturePreview >( this->At( iTexture ) );

		if( mxStrEquAnsi( pTexturePreview->GetName(), name ) )
		{
			return pTexturePreview;
		}
	}
	return nil;
}

AEditable* EdTextureInspector::edGetParent()
{
	return nil;
}

UINT EdTextureInspector::edNumRows() const
{
	const UINT numKids = Super::edNumRows();
	if( DEBUG_THE_DEBUGGER ) {
		DEVOUT("EdTextureInspector: %u kids\n",numKids);
	}
	return numKids;
}

const char* EdTextureInspector::edToChars( UINT column ) const
{
	return "EdTextureInspector";
}

void EdTextureInspector::DbgDump()
{
	const UINT numTextures = this->Num();

	DBGOUT("=== %u textures:\n", numTextures);

	for( UINT iTexture = 0; iTexture < numTextures; iTexture++ )
	{
		const EdTexturePreview* pInfo = UpCast< EdTexturePreview >( this->At( iTexture ) );
		DBGOUT("texture[%u] is %s\n", iTexture, pInfo->GetName());
	}

	DBGOUT("===============\n");
}


HitTesting& GetHitTesting()
{
	return EdRenderer::Get().GetHitTesting();
}

