#include "stdafx.h"

// use QDesktopServices
//#include <ShellAPI.h>

#include <Core/IO/IOSystem.h>
#include <Core/IO/NativeFileSystem.h>
#include <Core/Resources.h>

#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include <Renderer/Core/Geometry.h>
#include <Renderer/Materials/Phong.h>
#include <Renderer/Materials/PlainColor.h>

#include <QtSupport/DragAndDrop.h>

#include "editor_common.h"
#include "main_window_frame.h"
#include "app.h"
#include "resource_cache.h"
#include "project_manager.h"
#include "resource_browser.h"

#include "property_grid.h"

/*
-----------------------------------------------------------------------------
	EdResourceViewGlobals
-----------------------------------------------------------------------------
*/
struct EdResourceViewGlobals
{
	//EdAction	m_addNewItemAct;
	//EdAction	m_addExistingItemAct;
	//EdAction	m_addExistingFoldersAct;
	//EdAction	m_showItemPropertiesAct;
	//EdAction	m_createNewFolderAct;
	//EdAction	m_savePackageAct;
	//EdAction	m_removePackageAct;
	//EdAction	m_removeFolderAct;
	//EdAction	m_removeFileAct;
	//EdAction	m_saveAsAct;
	//EdAction	m_saveAllPackagesAct;
	EdAction	m_generateCppCodeFileGuids;

	//EdAction	m_createPrefabBoxAct;
	//EdAction	m_createPrefabQuadAct;
	//EdAction	m_createPrefabSphereAct;

	//QMenu		m_resourceBrowserContextMenu;
	//QMenu		m_packageContextMenu;
	//QMenu		m_folderContextMenu;
	//QMenu		m_fileContextMenu;

	//QMenu		m_createNewItemMenu;

public:
	EdResourceViewGlobals()
	{
		//m_addNewItemAct.setText( "New Item..." );
		//m_addExistingItemAct.setText( "Existing Items..." );
		//m_addExistingFoldersAct.setText( "Existing Folders..." );
		//m_showItemPropertiesAct.setText("Properties");
		//m_createNewFolderAct.setText("New Folder");
		//m_savePackageAct.setText("Save");
		//m_removePackageAct.setText("Delete");
		//m_removeFolderAct.setText("Delete");
		//m_removeFileAct.setText("Delete");
		//m_saveAsAct.setText("Save As...");
		//m_saveAllPackagesAct.setText("Save All Packages");
		m_generateCppCodeFileGuids.setText("Generate file GUIDs (C++)");

		//m_createPrefabBoxAct.setText("Box");
		//m_createPrefabQuadAct.setText("Plane");
		//m_createPrefabSphereAct.setText("Sphere");

		//m_addExistingItemAct.setShortcut( QKeySequence( Qt::SHIFT + Qt::ALT + Qt::Key_A ) );

		//QObject::connect( &m_addNewItemAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_AddNewItem()) );
		//QObject::connect( &m_addExistingItemAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_AddExistingItem()) );
		//QObject::connect( &m_addExistingFoldersAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_AddExistingFolders()) );
		//QObject::connect( &m_removeFileAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_RemoveItem()) );
		//QObject::connect( &m_removeFolderAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_RemoveItem()) );
		//QObject::connect( &m_showItemPropertiesAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_ShowItemProperties()) );
		//QObject::connect( &m_createNewFolderAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_AddNewFolder()) );
		//QObject::connect( &m_savePackageAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_SavePackage()) );
		//QObject::connect( &m_removePackageAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_RemovePackage()) );
		//QObject::connect( &m_saveAllPackagesAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_SaveAllPackages()) );
		QObject::connect( &m_generateCppCodeFileGuids, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_GenerateFileGuidsCppCode()) );

		//QObject::connect( &m_createPrefabBoxAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_CreatePrefab_Box()) );
		//QObject::connect( &m_createPrefabQuadAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_CreatePrefab_Quad()) );
		//QObject::connect( &m_createPrefabSphereAct, SIGNAL(triggered()), EdResourceBrowserModule::Ptr(), SLOT(slot_CreatePrefab_Sphere()) );


		//m_resourceBrowserContextMenu;
		//{
		//	m_resourceBrowserContextMenu.addAction( &m_saveAllPackagesAct );
		//	m_resourceBrowserContextMenu.addSeparator();
		//	m_resourceBrowserContextMenu.addAction( &m_showItemPropertiesAct );
		//}

		//m_packageContextMenu
		//{
		//	{
		//		QMenu* addMenu = m_packageContextMenu.addMenu( "Add" );
		//		//addMenu->addMenu( &m_addNewItemMenu );
		//		addMenu->addAction( &m_addNewItemAct );
		//		addMenu->addAction( &m_addExistingItemAct );
		//		addMenu->addAction( &m_addExistingFoldersAct );
		//		addMenu->addAction( &m_createNewFolderAct );
		//	}
		//	m_packageContextMenu.addMenu( &m_createNewItemMenu );
		//	m_packageContextMenu.addAction( &m_savePackageAct );
		//	m_packageContextMenu.addAction( &m_removePackageAct );
		//	m_packageContextMenu.addAction( &m_generateCppCodeFileGuids );
		//	m_packageContextMenu.addSeparator();
		//	m_packageContextMenu.addAction( &m_showItemPropertiesAct );
		//}

		//m_folderContextMenu
		//{
		//	{
		//		QMenu* addMenu = m_folderContextMenu.addMenu( "Add" );
		//		//addMenu->addMenu( &m_addNewItemMenu );
		//		addMenu->addAction( &m_addNewItemAct );
		//		addMenu->addAction( &m_addExistingItemAct );
		//		addMenu->addAction( &m_addExistingFoldersAct );
		//		addMenu->addAction( &m_createNewFolderAct );
		//	}
		//	m_folderContextMenu.addMenu( &m_createNewItemMenu );
		//	m_folderContextMenu.addAction( &m_removeFolderAct );
		//	m_folderContextMenu.addSeparator();
		//	m_folderContextMenu.addAction( &m_showItemPropertiesAct );
		//}

		//m_fileContextMenu
		//{
		//	m_fileContextMenu.addAction( &m_saveAsAct );
		//	m_fileContextMenu.addAction( &m_removeFileAct );
		//	m_fileContextMenu.addAction( &m_showItemPropertiesAct );
		//}


		//{
		//	m_createNewItemMenu.setTitle( "Create" );
		//}
		//m_createNewItemMenu.addAction( &m_createPrefabBoxAct );
		//m_createNewItemMenu.addAction( &m_createPrefabQuadAct );
		//m_createNewItemMenu.addAction( &m_createPrefabSphereAct );
	}
};

static TBlob16< EdResourceViewGlobals >	gData;




/*
-----------------------------------------------------------------------------
	AssetsViewModel
-----------------------------------------------------------------------------
*/
AssetsViewModel::AssetsViewModel()
{
	this->setSupportedDragActions(Qt::ActionMask);
}

AssetsViewModel::~AssetsViewModel()
{
	//
}

Qt::DropActions AssetsViewModel::supportedDropActions() const
{
	return Qt::ActionMask;
}

QMimeData* AssetsViewModel::mimeData( const QModelIndexList& indexes ) const
{
	return Super::mimeData( indexes );
}

bool AssetsViewModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
	if (action == Qt::IgnoreAction) {
		return true;
	}
	return true;
}

Qt::ItemFlags AssetsViewModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable
		| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled
		//| Qt::ItemIsEditable
		;
}

/*
-----------------------------------------------------------------------------
	AssetsViewDockWidget
-----------------------------------------------------------------------------
*/
AssetsViewDockWidget::AssetsViewDockWidget(QWidget *parent)
	: QDockWidget( parent )
{
	this->setWindowTitle("Asset Browser");
	this->setObjectName("Package Browser");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
	this->setAcceptDrops(true);

	{
		m_treeView.header()->setResizeMode(QHeaderView::ResizeToContents);
		m_treeView.setAcceptDrops(true);
		m_treeView.setDragEnabled(true);
		//m_treeView.setAutoExpandDelay(1000);	// 1 sec
		//m_treeView.setDragDropMode(DragDropMode::DropOnly);
		m_treeView.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		m_treeView.setDropIndicatorShown(true);
	}
	m_treeView.setModel( &m_model );
	this->setWidget( &m_treeView );

	connect(
		&m_treeView, SIGNAL(doubleClicked(QModelIndex)),
		this, SLOT(slot_OnItemDoubleClicked(QModelIndex))
		);

	MX_CONNECT_THIS( EdSystem::Get().Event_AssetDbChanged, AssetsViewDockWidget, OnAssetDatabaseChanged );
}

AssetsViewDockWidget::~AssetsViewDockWidget()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_AssetDbChanged );
}

void AssetsViewDockWidget::OnProjectLoaded()
{
	// nothing
	// tree view should be populated in OnAssetDatabaseChanged()
}

void AssetsViewDockWidget::OnProjectUnloaded()
{
	m_treeView.setModel( nil );
}

void AssetsViewDockWidget::OnAssetDatabaseChanged( const char* pathToSrcAssets )
{
	AssertPtr(pathToSrcAssets);
	VRET_IF_NIL(pathToSrcAssets);

	//@hack
	m_treeView.setModel( nil );

	if( pathToSrcAssets != nil && (mxStrLenAnsi(pathToSrcAssets) > 0) )
	{
		m_treeView.setModel( &m_model );

		{
			// QFileSystemModel - column 0 - File Name and Icon
			//m_treeView.resizeColumnToContents(0);
			MX_TODO("resizeColumnToContents");
			//const UINT QFileSys_Column_Count = 4;
			//for( UINT iColumn = 0; iColumn < QFileSys_Column_Count; iColumn++ )
			//{
			//	m_treeView.resizeColumnToContents( iColumn );
			//}

			// QFileSystemModel - column 1 - File Size
			m_treeView.hideColumn(1);

			// QFileSystemModel - column 2 - File Type
			m_treeView.hideColumn(2);

			// QFileSystemModel - column 3 - Last Time of File Modification
			m_treeView.hideColumn(3);
		}


		const QModelIndex rootIndex = m_model.setRootPath( pathToSrcAssets );
		m_treeView.setRootIndex( rootIndex );
		//m_treeView.expand( rootIndex );
	}
}

void AssetsViewDockWidget::slot_OnItemDoubleClicked( const QModelIndex& modelIndex )
{
	Assert( modelIndex.isValid() );

	// Open the asset with associated application.

	const QFileInfo fileInfo = m_model.fileInfo( modelIndex );
	const QUrl url = QUrl::fromLocalFile( fileInfo.absoluteFilePath() );
	Assert(!url.isEmpty());
	Assert(url.isValid());

	// Open asset in an external editor, texture application or modelling tool depending on what type of asset it is.

	const bool bOk = QDesktopServices::openUrl( url );
	if( bOk ) {
		mxPutf( "Opening '%s'...\n", url.toString().toAscii().data() );
	} else {
		mxWarnf( "Failed to open '%s'\n", url.toString().toAscii().data() );
	}
}

/*
-----------------------------------------------------------------------------
	EdResourceBrowser
-----------------------------------------------------------------------------
*/
EdResourceBrowserModule::EdResourceBrowserModule()
{
	gData.Construct();
}

EdResourceBrowserModule::~EdResourceBrowserModule()
{
	gData.Destruct();
}

void EdResourceBrowserModule::PreInit()
{
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdResourceBrowserModule::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::LeftDockWidgetArea, &m_resourcesTreeView );
}

void EdResourceBrowserModule::Shutdown()
{
}

void EdResourceBrowserModule::CreateActions()
{
	m_loadResourceDatabaseAct.setText( tr("Open Resource Database") );
	m_loadResourceDatabaseAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_O ) );
	m_loadResourceDatabaseAct.setStatusTip(tr("Loads an existing package"));

	m_createResourceDatabaseAct.setText( tr("Create Resource Database") );
	m_createResourceDatabaseAct.setShortcut( QKeySequence( Qt::SHIFT + Qt::ALT + Qt::Key_N ) );
	m_createResourceDatabaseAct.setStatusTip(tr("Creates a new package"));

	m_rebuildResourceDatabaseAct.setText( tr("Rebuild Resource Database") );
	m_rebuildResourceDatabaseAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_F7 ) );
	m_rebuildResourceDatabaseAct.setStatusTip(tr("Rebuids the loaded asset database"));

	m_showResourceBrowserAct.setText( tr("Resources") );
	m_showResourceBrowserAct.setStatusTip(tr("View the resource browser panel"));
	m_showResourceBrowserAct.setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_E ) );

	m_showGenericBrowserAct.setText( tr("Generic Browser") );
}

void EdResourceBrowserModule::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;

	menus.fileMenu->addAction( &m_loadResourceDatabaseAct );
	//menus.fileMenu->addAction( &m_createResourceDatabaseAct );
	menus.fileMenu->addAction( &m_rebuildResourceDatabaseAct );
	menus.fileMenu->addSeparator();

	menus.viewMenu->addAction( &m_showResourceBrowserAct );
	menus.viewMenu->addAction( &m_showGenericBrowserAct );
}

void EdResourceBrowserModule::ConnectSigSlots()
{
	connect( &m_loadResourceDatabaseAct, SIGNAL(triggered()), this, SLOT(slot_OpenResourceDatabase()) );
	connect( &m_createResourceDatabaseAct, SIGNAL(triggered()), this, SLOT(slot_CreateResourceDatabase()) );
	connect( &m_rebuildResourceDatabaseAct, SIGNAL(triggered()), this, SLOT(slot_RebuildResourceDatabase()) );
	connect( &m_showResourceBrowserAct, SIGNAL(triggered()), &m_resourcesTreeView, SLOT(show()) );

	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_loadResourceDatabaseAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_createResourceDatabaseAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_rebuildResourceDatabaseAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_showResourceBrowserAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_showGenericBrowserAct );
}

void EdResourceBrowserModule::SerializeProjectData( ATextSerializer & archive )
{
	//archive.Push_Scope("Resource_Mgr");
	//{
	//	String	myStr = "Crap";
	//	archive.Serialize_String("My_Sr",myStr);
	//}
	//archive.Pop_Scope();
}

void EdResourceBrowserModule::OnProjectLoaded()
{
}

void EdResourceBrowserModule::OnProjectUnloaded()
{
}

void EdResourceBrowserModule::slot_CreateResourceDatabase()
{
	EdGlobalSettings& settings = EdSystem::Get().settings;

	String &	selectedPathToSrcAssets = settings.pathToSrcAssets;
	String &	selectedPathToIntAssets = settings.pathToIntAssets;
	String &	selectedPathToBinAssets = settings.pathToBinAssets;
Unimplemented_Checked;
}

void EdResourceBrowserModule::slot_OpenResourceDatabase()
{
	EdProject* pCurrentProject = EditorApp::GetCurrentProject();
	AssertPtr(pCurrentProject);
	VRET_IF_NIL(pCurrentProject);

	const char* initialDir = EdSystem::Get().settings.pathToAssetDb;

	const String assetDbFileName =
		QtSupport::F_OpenDialog_Select_Asset_DB_File( nil, initialDir );

	if( assetDbFileName.IsEmpty() ) {
		return;
	}

	EdSystem::Get().settings.pathToAssetDb = assetDbFileName;
	EdSystem::Get().settings.pathToAssetDb.StripFilename();

	pCurrentProject->LoadAssetDatabase( assetDbFileName );
}

void EdResourceBrowserModule::slot_RebuildResourceDatabase()
{
	DevAssetManager* assetDb = SafeCast< DevAssetManager >( gCore.resources->GetContentDatabase() );
	AssertPtr( assetDb );
	if( assetDb != nil ) {
		assetDb->Refresh();
	}
}

void EdResourceBrowserModule::SerializeWidgetLayout( QDataStream & stream )
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::LeftDockWidgetArea, &m_resourcesTreeView );
}

void EdResourceBrowserModule::slot_GenerateFileGuidsCppCode()
{
	UNDONE;
	//ResourcePackage* pPackage = m_resourcesTreeView.m_treeView.GetSelectedObject<ResourcePackage>();
	//if( pPackage != nil )
	//{
	//	QFileDialog dialog(GetMainFrame(),tr("Select output folder."));
	//	{
	//		dialog.setViewMode(QFileDialog::Detail);
	//		//dialog.setFileMode(QFileDialog::AnyFile);
	//		dialog.setFileMode(QFileDialog::DirectoryOnly);
	//		dialog.setAcceptMode(QFileDialog::AcceptSave);
	//		//dialog.selectFile(pPackage->GetName());
	//	}

	//	static QString	prevDirectory = QCoreApplication::applicationDirPath();
	//	dialog.setDirectory(prevDirectory);

	//	if( dialog.exec() )
	//	{
	//		const QDir dir = dialog.directory();

	//		//const QStringList selectedFiles( dialog.selectedFiles() );
	//		//Assert(selectedFiles.count() == 1);

	//		//QString cppFileName = selectedFiles.first();

	//		pPackage->edGen_CppCode_FileGuids(
	//			dir.absolutePath().toAscii().data(),
	//			pPackage->GetName()
	//		);
	//	}
	//	prevDirectory = dialog.directory().path();
	//}
}


