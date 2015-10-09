#pragma once

#include <Renderer/Renderer.h>
#include <Renderer/Debugger.h>
#include "property_grid.h"

#include "viewports.h"
#include "editor_system.h"

/*
-----------------------------------------------------------------------------
	EdViewport_TextureView
-----------------------------------------------------------------------------
*/
class EdViewport_TextureView : public EdViewport
{
	mxDECLARE_CLASS( EdViewport_TextureView, EdViewport );

public:
	typedef EdViewport Super;

	EdViewport_TextureView();
	~EdViewport_TextureView();

	virtual void InitViewport( EdViewportWidget* widget ) override;
	virtual void CloseViewport() override;

	void SetCurrentTexture( PCSTR pTexture );

	virtual void OnPaint() override;

	//virtual void OnObjectDestroyed( Editable* theObject ) override;

public:	// ClientViewport interface

	virtual void Draw() override;

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;

public:	// InputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;

private:
	StackString		m_currentTexture;
};

class EdTextureInspectorTreeView : public EdTreeView
{
public:
	typedef EdTreeView Super;

	EdTextureInspectorTreeView( QWidget *parent = nil );
	~EdTextureInspectorTreeView();

protected:	//-- tree_views
	void OnItemClicked( AEditable* pObject ) override;
	void OnItemDoubleClicked( AEditable* pObject ) override;
	void OnItemSelected( AEditable* pObject ) override;
};

/*
-----------------------------------------------------------------------------
	EdTextureInspectorGraphicsView
-----------------------------------------------------------------------------
*/
class EdTextureInspectorGraphicsView
	: public QScrollArea
{
public:
	typedef QScrollArea Super;

	EdTextureInspectorGraphicsView( QWidget *parent = nil );
	~EdTextureInspectorGraphicsView();

	void Initialize();
	void Shutdown();

	void SetCurrentTexture( ATexturePreview* pTexture );

private:
	EdViewportWidget	m_viewportWidget;
	EdViewport_TextureView	m_viewportTextureView;
};

/*
-----------------------------------------------------------------------------
	EdWidget_TextureInspector
-----------------------------------------------------------------------------
*/
class EdWidget_TextureInspector
	: public QSplitter, public EdProjectChild
	, public TGlobal<EdWidget_TextureInspector>
{
public:
	typedef QSplitter Super;

	EdWidget_TextureInspector( QWidget *parent = nil );
	~EdWidget_TextureInspector();

	void Initialize();
	void Shutdown();

	void OnProjectLoaded() override;
	void OnProjectUnloaded() override;

	//void OnItemClicked( Editable* pObject );
	void OnItemDoubleClicked( AEditable* pObject );

private:
	// this will crash otherwise
	void OnBeforeMainViewportResized( UINT newWidth, UINT newHeight );
	void OnAfterMainViewportResized( UINT newWidth, UINT newHeight );

private:
	EdTreeViewModel		m_model;
	EdTextureInspectorTreeView	m_treeView;
	EdTextureInspectorGraphicsView	m_graphicsView;
};



class EdWidget_EngineTreeView : public EdTreeView
{
public:
	typedef EdTreeView Super;

	EdWidget_EngineTreeView( QWidget *parent = nil );
	~EdWidget_EngineTreeView();

protected:	//-- tree_views
	virtual void OnItemSelected( AEditable* pObject );
};

class EdDockWidget_EngineView : public QDockWidget
{
	Q_OBJECT

public:
	typedef QDockWidget Super;

	EdDockWidget_EngineView( QWidget* parent = nil );
	~EdDockWidget_EngineView();

private:
	EdTreeViewModel		m_engineViewModel;
	EdWidget_EngineTreeView	m_engineTreeView;
};

class EdDebugger
	: public QMainWindow
	, public EdModule
	, public TGlobal< EdDebugger >
	, public EdWidgetChild
{
	Q_OBJECT

public:
	EdDebugger();
	~EdDebugger();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;
	virtual void Shutdown() override;

	virtual void OnProjectUnloaded() override;

	virtual const char* GetModuleName() const
	{
		return "Debugger";
	}

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

public:
	EdDockWidget_EngineView		m_engineViewDockWidget;
	ProperyEditorDockWidget		m_engineProperties;

	EdWidget_TextureInspector	m_textureInspector;

	EdAction	m_showDebuggerAct;
	EdAction	m_showTextureInspectorAct;
	EdAction	m_showMemoryMapAct;
};

