/*
=============================================================================
	File:	scene_renderer.h
	Desc:	Scene renderer used by the editor.
=============================================================================
*/
#pragma once

#include <Base/Util/FPSTracker.h>
#include <Renderer/Renderer.h>
#include <Engine/Worlds.h>

#include "viewports.h"

#include "editor_system.h"
#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	PrivateGlobals
-----------------------------------------------------------------------------
*/
struct PrivateGlobals : public TGlobal<PrivateGlobals>
{
	EdAction	m_editViewportPropertiesAct;

	QMenu		m_viewportContextMenu;

public:
	PrivateGlobals()
	{
		m_editViewportPropertiesAct.setText("Properties");

		//m_viewportContextMenu
		{
			m_viewportContextMenu.setTitle( "Viewport" );
			m_viewportContextMenu.addSeparator();
			m_viewportContextMenu.addAction( &m_editViewportPropertiesAct );
		}

#if 0
		//m_folderContextMenu
		{
			{
				QMenu* addMenu = m_folderContextMenu.addMenu( "Add" );
				addMenu->addMenu( &m_addNewItemMenu );
				addMenu->addAction( &m_addExistingItemAct );
				addMenu->addAction( &m_createNewFolderAct );
			}
			m_folderContextMenu.addAction( &m_removeFolderAct );
			m_folderContextMenu.addSeparator();
			m_folderContextMenu.addAction( &m_showItemPropertiesAct );
		}
#endif

	}
};




/*
-----------------------------------------------------------------------------
	EdMainViewport
-----------------------------------------------------------------------------
*/
class EdMainViewport : public EdSceneViewport
{
	mxDECLARE_CLASS( EdMainViewport, EdSceneViewport );

public:
	EdMainViewport();
	~EdMainViewport();

	virtual void InitViewport( EdViewportWidget* widget ) override;
	virtual void CloseViewport() override;

	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;
	virtual void SerializeProjectData( ATextSerializer & archive ) override;

	virtual void OnSelectionChanged( AEditable* theObject ) override;
	virtual void OnObjectDestroyed( AEditable* theObject ) override;

public:	// EdViewport interface
	virtual bool AcceptsDrop( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) override;
	virtual void OnDropEvent( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) override;

public:	// ClientViewport interface

	virtual void Tick( FLOAT deltaSeconds ) override;
	virtual void Draw() override;

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;

	virtual void OnClose() override;

	//-- InputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

public:	// Editor
	virtual const char* edToChars( UINT column ) const override;
	virtual void edCreateProperties( EdPropertyCollector & outProperties, bool bClear = true ) override;

private:
	void OnEngineLoaded();

private:
	TIndexPtr< World_Editor >	m_worldEditor;

	FPSTracker<>	m_fpsCounter;
};

class EdMyViewportWidget : public EdViewportWidget
{
public:
	Q_OBJECT

public:
	typedef EdViewportWidget Super;

	EdMyViewportWidget( QWidget* parent = nil );
	~EdMyViewportWidget();

public slots:
	void slot_EditViewportProperties();

protected:
	virtual void contextMenuEvent( QContextMenuEvent* theEvent ) override;
};




/*
-----------------------------------------------------------------------------
	EdSceneRenderer
-----------------------------------------------------------------------------
*/
class EdSceneRenderer
	: public QObject
	, public EdModule
	, public TGlobal< EdSceneRenderer >
	, public EdWidgetChild
{
	Q_OBJECT

public:
	EdSceneRenderer();
	~EdSceneRenderer();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

	EdMyViewportWidget* GetMainViewportWidget()
	{
		return &m_mainViewportWidget;
	}

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

	void CreateViewports();
	void CloseViewports();

	void Tick( const mxDeltaTime& deltaTime );

	void DrawAll();

private:
	PrivateGlobals		m_globals;

	EdMainViewport		m_mainViewport;
	EdMyViewportWidget	m_mainViewportWidget;

	EdAction	m_showMainViewportAct;
};
