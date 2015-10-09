#pragma once

#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include "editor_system.h"
#include "widgets/tree_views.h"
#include "forms/addnewitemdialog.h"

/*
-----------------------------------------------------------------------------
	AssetsViewModel
-----------------------------------------------------------------------------
*/
class AssetsViewModel : public QFileSystemModel
{
	Q_OBJECT

public:
	typedef QFileSystemModel Super;

	AssetsViewModel();
	~AssetsViewModel();

	virtual Qt::DropActions supportedDropActions() const override;
	virtual QMimeData* mimeData( const QModelIndexList& indexes ) const override;
	virtual bool dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) override;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};




/*
-----------------------------------------------------------------------------
	AssetsTreeView
-----------------------------------------------------------------------------
*/
class AssetsTreeView : public QTreeView
{
	Q_OBJECT

public:
	typedef QTreeView Super;

	AssetsTreeView( QWidget *parent = nil );
	~AssetsTreeView();

protected:	//==- QTreeView

	virtual void contextMenuEvent( QContextMenuEvent* theEvent ) override;

private slots:
	void slot_Create_Material();
};



/*
-----------------------------------------------------------------------------
	AssetsViewDockWidget
-----------------------------------------------------------------------------
*/
class AssetsViewDockWidget
	: public QDockWidget
	, public EdProjectChild
	, public TGlobal< AssetsViewDockWidget >
{
	Q_OBJECT

public:
	AssetsViewDockWidget(QWidget *parent = nil);
	~AssetsViewDockWidget();

protected:
	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

private slots:
	void slot_OnItemDoubleClicked( const QModelIndex& modelIndex );

private:
	void OnAssetDatabaseChanged( const char* pathToSrcAssets );

private:
	AssetsViewModel		m_model;
	AssetsTreeView		m_treeView;
};

/*
-----------------------------------------------------------------------------
	EdResourceBrowser
-----------------------------------------------------------------------------
*/
class EdResourceBrowserModule
	: public QObject
	, public EdModule
	, public TGlobal< EdResourceBrowserModule >
	, public EdWidgetChild
{
	Q_OBJECT

public:
	EdResourceBrowserModule();
	~EdResourceBrowserModule();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void Shutdown() override;

	virtual void SerializeWidgetLayout( QDataStream & stream ) override;
	virtual void SerializeProjectData( ATextSerializer & archive ) override;

	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

public slots:
	void slot_GenerateFileGuidsCppCode();

private slots:
	void slot_CreateResourceDatabase();
	void slot_RebuildResourceDatabase();
	void slot_OpenResourceDatabase();

private:
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

private:
	TPtr< AEditable >	m_selectedItem;

	EdAction	m_loadResourceDatabaseAct;
	EdAction	m_createResourceDatabaseAct;
	EdAction	m_rebuildResourceDatabaseAct;
	EdAction	m_showResourceBrowserAct;
	EdAction	m_showGenericBrowserAct;

	AssetsViewDockWidget	m_resourcesTreeView;
};
