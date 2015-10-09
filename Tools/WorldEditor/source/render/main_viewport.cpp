#include "stdafx.h"

#include "editor_system.h"
#include "render/main_viewport.h"
#include "render/draw_viewport.h"
#include "editors/world_editor.h"

/*
-----------------------------------------------------------------------------
	EdMainViewport
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( EdMainViewport );

EdMainViewport::EdMainViewport()
{
	mxCONNECT_THIS( EdSystem::Get().Event_EngineLoaded, EdMainViewport, OnEngineLoaded );
}

EdMainViewport::~EdMainViewport()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_EngineLoaded );
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
	World_Editor* pWorldEditor = theObject->edFindParentNode< World_Editor >();
	if( pWorldEditor != nil )
	{
		mxDBG_CHECK_VTBL( pWorldEditor );
		mxDBG_CHECK_VTBL( (void*)pWorldEditor->m_world );

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
				m_worldEditor->Accept_Dropped_File( *this, localFile.toAscii().data(), mouseX, mouseY );
			}
		}
	}
	if( mimeData->hasText() )
	{
		const QString text = mimeData->text();

		DBGOUT("EdMainViewport::OnDropEvent: text = %s\n", text.toAscii().data());
	}
}

void EdMainViewport::TickViewport( FLOAT deltaSeconds )
{
	Super::TickViewport( deltaSeconds );

	if( m_worldEditor.IsValid() )
	{
		m_worldEditor->m_world->m_editorInfo.camera = m_camera;
	}
}

void EdMainViewport::Draw()
{
	if( m_renderViewport.IsNull() )
	{
		return;
	}

	rxRenderer & renderer = gRenderer;

	rxFrameContext	frameContext;
	frameContext.clearColor = FColor::BLACK;

	renderer.BeginScene( &m_renderViewport, frameContext );

	if( m_worldEditor.IsValid() )
	{
		World* pWorld = m_worldEditor->m_world;

		Rendering::Draw_Viewport( *this, *pWorld );
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
		if( pObject != nil )
		{
			pObject->edRemoveSelf();
		}
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

void EdMainViewport::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties( properties, bClear );
	m_camera.edCreateProperties( properties );
}

/*
-----------------------------------------------------------------------------
	EdMainViewportWidget
-----------------------------------------------------------------------------
*/
EdMainViewportWidget::EdMainViewportWidget( QWidget* parent )
	: Super( parent )
{
	this->setObjectName("MainViewportWidget");
}

EdMainViewportWidget::~EdMainViewportWidget()
{
}

void EdMainViewportWidget::contextMenuEvent( QContextMenuEvent* theEvent )
{
	EdViewport* pViewport = this->GetViewport();

	if( pViewport != nil )
	{
		QMenu	menu;

		EdShowContextMenuArgs	contextMenuArgs;
		contextMenuArgs.parent = &menu;
		//args.pObject = pObject;
		//contextMenuArgs.x = mouseX;
		//contextMenuArgs.y = mouseY;

		const UINT mouseX = theEvent->pos().x();
		const UINT mouseY = theEvent->pos().y();

		AHitProxy* pHitProxy = GetHitTesting().GetHitProxy( pViewport->GetRenderViewport(), mouseX, mouseY );

		if( pHitProxy != nil )
		{
			AEditable* pEditable = pHitProxy->GetSelectionProxy();
			if( pEditable != nil )
			{
				pEditable->edShowContextMenu( contextMenuArgs );
			}
		}

		if( this->GetViewport() != nil )
		{
			qtCONNECT(
				menu.addAction("Viewport properties"), SIGNAL(triggered()),
				this, SLOT(slot_Edit_Properties())
				);
		}

		menu.exec( theEvent->globalPos() );
	}
}

void EdMainViewportWidget::slot_Edit_Properties()
{
	Util_NewPropertyEditorDockWidget( this->GetViewport(), this );
}
