#include "stdafx.h"

#include <gui/AntTweakBar_114/include/AntTweakBar.h>
//#pragma comment( lib, "AntTweakBar.lib" )

#include <Core/IO/IOSystem.h>
#include <Core/IO/NativeFileSystem.h>
#include <Core/Util/Tweakable.h>

#include <EditorSupport/Serialization/ASerializer.h>

#include <Graphics/DX11/DX11Private.h>
#include <Graphics/DX11/DX11Helpers.h>

#include <Renderer/Renderer.h>
#include <Renderer/Common.h>
#include <Renderer/Core/Font.h>
#include <Renderer/Scene/Model.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Util/BatchRenderer.h>
#include <Renderer/Util/ScreenQuad.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/Editor/EdGraphics.h>
#include <Renderer/Editor/HitTesting.h>

#include <Engine/Worlds.h>

#include <QtSupport/DragAndDrop.h>

#include "main_window_frame.h"
#include "app.h"

#include "scene_renderer.h"
#include "property_editor.h"
#include "resource_browser.h"

#define ED_DEFAULT_VIEWPORT_SIZE	800,600

template< UINT HISTORY_BUF_SIZE >
static
void F_RenderDebugInfo( BatchRenderer & batchRenderer, const rxView& view, FPSTracker<HISTORY_BUF_SIZE>& fpsCounter )
{
	DbgFont_BeginRendering();

	const FLOAT deltaSeconds = (DOUBLE)gfxBEStats.lastFrameRenderTime * 1e-6f;

MX_HACK("don't show large FPS");
	const FLOAT fps = fpsCounter.CalculateFPS(deltaSeconds) / 5.0f;

	Dbg_DrawBackEndStats( fps, 10, 10 );
	DrawCameraStats( view, 1, 100 );

	DbgFont_EndRendering();
}

// Renders a little X/Y/Z axis orientation icon in bottom left corner.
static
void F_DrawAxisIcon( BatchRenderer & batchRenderer, const GrViewport& viewport, const rxView& view )
{
	const FLOAT fAxisSize = 0.1f;
	const FLOAT fAxisOffCenter = 1.0f - fAxisSize;

	//bottom left corner
	Vec3D	offset( -fAxisOffCenter, -fAxisOffCenter, 0.0f );

	//Matrix4 viewRot = as_matrix4(view.CreateViewMatrix());
	Matrix4 viewRot = MatrixFromAxes( view.right, view.up, view.look );
	//viewRot.SetTranslation(-offset);
	viewRot.InverseSelf();
MX_FIXME("icon is aligned incorrectly");
	//bottom left corner
	viewRot.SetTranslation(offset);
UNDONE;
#if 0
	const float4x4 oldXform = batchRenderer.GetTransform();
	batchRenderer.SetTransform( viewRot );
	batchRenderer.SetState(GPU::Debug_NoCull);
	D3DContext->RSSetState(GPU::RS_SolidNoCullNoClip);
	D3DContext->OMSetDepthStencilState(GPU::DS_NoZTestWriteNoStencil,0);
	batchRenderer.DrawAxes( fAxisSize );
	batchRenderer.SetTransform(as_matrix4(oldXform));
#endif
}

static
void F_RenderHitProxies( BatchRenderer & batchRenderer, const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	HitTesting & hitTesting = EdGetHitTesting();
	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();
	Matrix4 viewProj = as_matrix4(sceneContext.viewProjectionMatrix);

	rxRenderWorld& renderWorld = sceneContext.scene;


	hitTesting.DeleteHitProxies();

	hitTesting.Begin( viewport.GetRenderViewport() );
	batchRenderer.SetHitTesting( &hitTesting );

	// World

	GPU::Default.Set( pD3DContext );
	{
		MX_UNDONE;
		//renderWorld.edDrawHitProxies( sceneContext, hitTesting );
	}

	// Editor elements

	EdDrawContext	drawContext( viewport, sceneContext, batchRenderer, hitTesting );
	{
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);

		EdSystem::Get().Event_DrawBackgroundHitProxy( drawContext );
	}
	{
		GPU::HitProxy_ZTestLessEqualCullBack.Set( pD3DContext );
		batchRenderer.SetState(GPU::HitProxy_ZTestLessEqualCullBack);

		EdSystem::Get().Event_DrawHitProxy( drawContext );
	}
	{
		GPU::SolidNoCullNoZTestNoClipNoBlend.Set( pD3DContext );
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);

		EdSystem::Get().Event_DrawForegroundHitProxy( drawContext );
	}

	batchRenderer.SetHitTesting( nil );
	hitTesting.End();
}

static void DrawEditorGrid( BatchRenderer & batchRenderer, const Vec3D& eyePos )
{
	// Draw editor grid and coordinate axes.
	batchRenderer.DrawInfiniteGrid( eyePos, 0.0f, FColor::GRAY );
	batchRenderer.DrawAxes();
}

static
void F_DrawEditorStuff( BatchRenderer & batchRenderer, const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	//HitTesting & hitTesting = EdGetHitTesting();
	//ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	batchRenderer.SetTransform(sceneContext.viewProjectionMatrix);

	rxRenderWorld& renderWorld = sceneContext.scene;


	EdDrawContext	drawContext( viewport, sceneContext, batchRenderer, AHitTesting::NullObject );

	{
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);
		//world->edDrawBackground(sceneContext,batchRenderer);
		EdSystem::Get().Event_DrawBackground( drawContext );
	}
	{
		batchRenderer.SetState(GPU::Default);
		DrawEditorGrid( batchRenderer, sceneContext.GetOrigin() );
		MX_UNDONE;
		//renderWorld.edDrawEditorStuff(sceneContext,batchRenderer);
		EdSystem::Get().Event_Draw( drawContext );
	}
	{
		batchRenderer.SetState(GPU::SolidNoCullNoZTestNoClipNoBlend);
		//world->edDrawForeground(sceneContext,batchRenderer);
		EdSystem::Get().Event_DrawForeground( drawContext );
	}


	//F_DrawAxisIcon( viewport.vp, view );
}

static
void F_RenderSingleViewport( EdMainViewport& viewport, World* world, FPSTracker<>& fpsCounter )
{
	rxRenderWorld& renderWorld = world->GetRenderWorld();

	const rxView& view = viewport.GetView();

	rxSceneContext	sceneContext( view, renderWorld );


	// render the main scene

	rxRenderer & renderer = gRenderer;

	renderer.DrawScene( sceneContext );


	const rxViewport& vp = viewport.GetRenderViewport();

	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();

	// render hit proxies into special off-screen render target

	F_RenderHitProxies( batchRenderer, viewport, sceneContext );


	// render editor stuff and debug info

	// set main render target and depth-stencil
	pD3DContext->OMSetRenderTargets( 1, &vp.mainRT.pRTV.Ptr, vp.mainDS.pDSV );
	pD3DContext->RSSetViewports( 1, &vp.d );


	F_DrawEditorStuff( batchRenderer, viewport, sceneContext );

	batchRenderer.Flush();



	F_RenderDebugInfo(batchRenderer, view,fpsCounter);
}

/*
-----------------------------------------------------------------------------
	EdViewport
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( EdMainViewport );

EdMainViewport::EdMainViewport()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_EngineLoaded, EdMainViewport, OnEngineLoaded );
}

EdMainViewport::~EdMainViewport()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_EngineLoaded );
}

void EdMainViewport::InitViewport( EdViewportWidget* widget )
{
	Super::InitViewport( widget );
}

void EdMainViewport::CloseViewport()
{
	Super::CloseViewport();

	m_worldEditor = nil;
	m_fpsCounter.Reset();
}

void EdMainViewport::OnProjectLoaded()
{
	Super::OnProjectLoaded();
}

void EdMainViewport::OnProjectUnloaded()
{
	Super::OnProjectUnloaded();

	m_worldEditor = nil;
	m_fpsCounter.Reset();
}

void EdMainViewport::SerializeProjectData( ATextSerializer & archive )
{
	Super::SerializeProjectData( archive );

	TextScope	scope( archive, "Main_Viewport");

	UINT & worldIndex = m_worldEditor.GetHandleRef();
	archive.Serialize_Uint32( "Curr_World_Index", worldIndex );

	if( archive.IsStoring() && m_worldEditor.IsValid() )
	{
		m_worldEditor->m_world->m_editorInfo.camera = m_camera;
	}
	if( archive.IsLoading() )
	{
		//
	}
}

void EdMainViewport::OnEngineLoaded()
{
	m_worldEditor.UpdateHandle();

	if( m_worldEditor.IsValid() )
	{
		m_camera = m_worldEditor->m_world->m_editorInfo.camera;
	}
}

void EdMainViewport::OnSelectionChanged( AEditable* theObject )
{
	Super::OnSelectionChanged(theObject);

	World_Editor* pWorldEditor = theObject->edFindParentNode< World_Editor >();
	if( pWorldEditor != nil )
	{
		MX_DBG_CHECK_VTBL( pWorldEditor );
		MX_DBG_CHECK_VTBL( (void*)pWorldEditor->m_world );

		if( m_worldEditor.IsValid() )
		{
			m_worldEditor->m_world->m_editorInfo.camera = m_camera;
		}

		m_worldEditor = pWorldEditor;

		m_camera = m_worldEditor->m_world->m_editorInfo.camera;
	}
}

void EdMainViewport::OnObjectDestroyed( AEditable* theObject )
{
	if( m_worldEditor == theObject ) {
		m_worldEditor = nil;
	}
}

bool EdMainViewport::AcceptsDrop( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY )
{
	if( m_worldEditor.IsValid() )
	{
		return m_worldEditor->edAcceptsDrop( mimeData );
	}

	EditorApp::ShowMsg_OnWrongUserAction("Select a world before adding any resources to it",ED_STATUS_BAR_MSG_TIMEOUT);

	return false;
}

void EdMainViewport::OnDropEvent( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY )
{
	if( !m_worldEditor.IsValid() )
	{
		return;
	}

	if( mimeData->hasUrls() )
	{
		const QList<QUrl> urlsList = mimeData->urls();

		foreach( QUrl url, urlsList )
		{
			DBGOUT("EdMainViewport::OnDropEvent: URL = %s\n", url.toString().toAscii().data());

			const QString localFile = url.toLocalFile();
			if( !localFile.isEmpty() )
			{
				DBGOUT("EdMainViewport::OnDropEvent: local file = %s\n", localFile.toAscii().data());
				m_worldEditor->Add_Resource_From_File( localFile.toAscii().data() );
			}
		}
	}
	if( mimeData->hasText() )
	{
		const QString text = mimeData->text();

		DBGOUT("EdMainViewport::OnDropEvent: text = %s\n", text.toAscii().data());
	}

	//if( mimeData->objects.IsEmpty() ) {
	//	return;
	//}

	//AHitProxy* pHitProxy = this->GetHitProxy( mouseX, mouseY );

	//if( pHitProxy != nil )
	//{
	//	HModelBatch* pHModelBatch = SafeCast<HModelBatch>(pHitProxy);
	//	if( pHModelBatch != nil)
	//	{
	//		if( mimeData->objects.Num() > 1 ) {
	//			EditorUtil::ShowMsg_OnWrongUserAction("Cannot apply multiple materials to a single model batch!");
	//			return;
	//		}

	//		UNDONE;
	//		//rxMaterial* pMaterial = SafeCast<rxMaterial>(mimeData->objects.GetFirst());
	//		//if( pMaterial != nil )
	//		//{
	//		//	pHModelBatch->pBatch->material = pMaterial;
	//		//	EdSystem::Get().OnSelectionChanged(pHModelBatch->pEntity->rfGetEditor());
	//		//	return;
	//		//}
	//	}
	//}

	//if( m_worldEditor.IsValid() )
	//{
	//	UNDONE;
	//	//m_worldEditor->edBulkAdd( mimeData->objects.ToPtr(), mimeData->objects.Num() );

	//	if( mimeData->objects.Num() == 1 )
	//	{
	//		EdSystem::Get().Notify_SelectionChanged( mimeData->objects.GetFirst() );
	//	}
	//}
}

void EdMainViewport::Tick( FLOAT deltaSeconds )
{
	Super::Tick( deltaSeconds );

	if( m_worldEditor.IsValid() )
	{
		m_worldEditor->m_world->m_editorInfo.camera = m_camera;
	}
}

void EdMainViewport::Draw()
{
	rxRenderer & renderer = gRenderer;

	rxFrameContext	frameContext;
	frameContext.clearColor = FColor::BLACK;

	renderer.BeginScene( &m_renderViewport, frameContext );

	if( m_worldEditor.IsValid() )
	{
		World* pWorld = m_worldEditor->m_world;

		MX_DBG_CHECK_VTBL( pWorld );

		F_RenderSingleViewport( *this, pWorld, m_fpsCounter );
	}

	renderer.EndScene();
}

void EdMainViewport::OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
{
	Super::OnResize(newWidth,newHeight,bFullScreen);
}

void EdMainViewport::OnClose()
{
	Super::OnClose();
}

void EdMainViewport::OnKeyPressed( EKeyCode key )
{
	Super::OnKeyPressed( key );

	if( key == EKeyCode::Key_Delete )
	{
		AEditable* pObject = gCore.editor->GetSelectedObject();
		UNDONE;
		//EdRemoveNode( pObject );
	}
}

void EdMainViewport::OnKeyReleased( EKeyCode key )
{
	Super::OnKeyReleased( key );
}

const char* EdMainViewport::edToChars( UINT column ) const
{
	return "Main Viewport";
}

void EdMainViewport::edCreateProperties( EdPropertyCollector & outProperties, bool bClear )
{
	Super::edCreateProperties( outProperties, bClear );
	m_camera.edCreateProperties( outProperties );
}

/*
-----------------------------------------------------------------------------
	EdMyViewportWidget
-----------------------------------------------------------------------------
*/
EdMyViewportWidget::EdMyViewportWidget( QWidget* parent )
	: Super( parent )
{
	this->setObjectName("EdMyViewportWidget");
	connect( &PrivateGlobals::Get().m_editViewportPropertiesAct, SIGNAL(triggered()), this, SLOT(slot_EditViewportProperties()) );
}

EdMyViewportWidget::~EdMyViewportWidget()
{
}

void EdMyViewportWidget::slot_EditViewportProperties()
{
	EdViewport* pViewport = this->GetViewport();
	if( pViewport != nil )
	{
		//FPSCamera* pCamera = pViewport->GetCamera();
		//if( pCamera != nil )
		//{
			ProperyEditorDockWidget* pProperyEditor = new ProperyEditorDockWidget(this);
			pProperyEditor->SetObject(pViewport);

			EditorApp::GetMainFrame()->addDockWidget(Qt::RightDockWidgetArea,pProperyEditor);
			pProperyEditor->show();
		//}
	}
}

MX_REMOVE_OLD_CODE
//static void ZZ_ShowPropertiesOfSelectedObject( void* pUserData )
//{
//	rxSpatialObject* pSpatialObject = c_cast(rxSpatialObject*) pUserData;
//	if( pSpatialObject != nil )
//	{
//		Util_NewPropertyEditorDockWidget( pSpatialObject->rfGetEditor(), EdSceneRenderer::Get().GetMainViewportWidget() );
//	}
//}

void EdMyViewportWidget::contextMenuEvent( QContextMenuEvent* theEvent )
{
	EdViewport* pViewport = this->GetViewport();

	if( pViewport != nil )
	{
		const UINT mouseX = theEvent->pos().x();
		const UINT mouseY = theEvent->pos().y();

		AHitProxy* pHitProxy = EdGetHitTesting().GetHitProxy( pViewport->GetRenderViewport(), mouseX, mouseY );
		if( pHitProxy != nil )
		{
			//HModelBatch* pHModelBatch = SafeCast<HModelBatch>(pHitProxy);
			//if(pHModelBatch)
			//{
			//	return;
			//}
			//rxSpatialObject* pSpatialObject = pHitProxy->GetParentEntity();
			//if( pSpatialObject != nil )
			//{
			//	EdAction	showItemPropsAct("Show Properties");
			//	showItemPropsAct.userData = pSpatialObject;
			//	showItemPropsAct.OnTriggered = MX_BIND_FREE_FUNCTION( ZZ_ShowPropertiesOfSelectedObject );

			//	QMenu	menu;
			//	{
			//		menu.addAction( &showItemPropsAct );
			//	}

			//	menu.exec( theEvent->globalPos() );
			//	return;
			//}
			Unimplemented_Checked;
		}

		PrivateGlobals::Get().m_viewportContextMenu.exec( theEvent->globalPos() );
	}
}

/*
-----------------------------------------------------------------------------
	EdSceneRenderer
-----------------------------------------------------------------------------
*/
EdSceneRenderer::EdSceneRenderer()
{
}

EdSceneRenderer::~EdSceneRenderer()
{
}

void EdSceneRenderer::CreateWidgets()
{
	QMainWindow* mainFrame = EditorApp::GetMainFrame();

	mainFrame->setCentralWidget( &m_mainViewportWidget );

	m_mainViewportWidget.showMaximized();
}

void EdSceneRenderer::CreateActions()
{
	m_showMainViewportAct.setText( TR("Main Viewport") );
	m_showMainViewportAct.setStatusTip( TR("View the main viewport") );
	m_showMainViewportAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_L ) );
}

void EdSceneRenderer::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_showMainViewportAct );
}

void EdSceneRenderer::ConnectSigSlots()
{
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_showMainViewportAct );
	EditorApp::ShowOnActionTriggered( &m_showMainViewportAct, &m_mainViewportWidget );

	MX_CONNECT_THIS( EdSystem::Get().Event_UpdateState, EdSceneRenderer, Tick );
	MX_CONNECT_THIS( EdSystem::Get().Event_RenderViewports, EdSceneRenderer, DrawAll );
}

void EdSceneRenderer::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdSceneRenderer::SetDefaultValues()
{
	m_mainViewportWidget.resize( ED_DEFAULT_VIEWPORT_SIZE );
}

void EdSceneRenderer::PostInit()
{
	this->CreateViewports();
}

void EdSceneRenderer::Shutdown()
{
	this->CloseViewports();
}

void EdSceneRenderer::CreateViewports()
{
	m_mainViewportWidget.SetViewport( &m_mainViewport );
}

void EdSceneRenderer::CloseViewports()
{
	m_mainViewportWidget.SetViewport( nil );
}

void EdSceneRenderer::Tick( const mxDeltaTime& deltaTime )
{
	const FLOAT deltaSeconds = deltaTime.fTime;

	// update the main viewport
	
	m_mainViewport.Tick( deltaSeconds );
}

void EdSceneRenderer::DrawAll()
{
	// render the main viewport

	if( m_mainViewportWidget.isVisible() )
	{
		m_mainViewport.Draw();
	}
}

void EdSceneRenderer::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	//QMainWindow* mainWindow = GetMainFrame();

	SerializeWidgetGeometry( stream, &m_mainViewportWidget );
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
