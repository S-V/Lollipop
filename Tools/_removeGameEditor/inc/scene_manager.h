/*
=============================================================================
	File:	scene_manager.h
	Desc:	Displays scene hierarchy.
=============================================================================
*/
#pragma once

//#include <Engine/rxRenderEntity.h>
#include <Engine/Worlds.h>

#include <QtSupport/common.h>

#include "tree_views.h"
#include "editor_system.h"
#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	EdSceneTreeView
-----------------------------------------------------------------------------
*/

class EdSceneTreeView
	: public TEditableTreeWidget<>
	, DependsOnGlobal< Global_Level_Editor >
{
public:
	typedef TEditableTreeWidget Super;

	EdSceneTreeView();

protected:
	virtual void OnItemSelected( AEditable* pObject ) override;
	virtual void OnItemDoubleClicked( AEditable* pObject ) override;
};

/*
-----------------------------------------------------------------------------
	EdSceneManager
-----------------------------------------------------------------------------
*/
class EdSceneManager
	: public QDockWidget
	, public EdModule
	, public EdWidgetChild
	, public TGlobal< EdSceneManager >
{
	Q_OBJECT

public:
	EdSceneManager();
	~EdSceneManager();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

public:
	void ShowContextMenu( AEditable* theObject, const QPoint& globalPos );

private slots:
	void slot_CreateNewWorld();
	void slot_LoadWorldFromFile();
	void slot_LoadRedFactionLevel();
	void slot_ShowEntityProperties();

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

	void UpdateAll( const mxDeltaTime& deltaTime );

private:
	EdSceneTreeView	m_treeWidget;

	EdAction	m_viewSceneMgrAct;

	EdAction	m_createNewWorldAct;
	EdAction	m_loadWorldFromFileAct;
	EdAction	m_loadRedFactionLevelAct;

	EdAction	m_showNodePropertiesAct;

	// for showing context menus
	TPtr< AEditable >	m_editedObject;

};
