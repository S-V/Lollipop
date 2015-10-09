#include "stdafx.h"

#include <Engine/Engine.h>
#include <Engine/Worlds.h>
//#include <Engine/RF/Level.h>

#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include "property_grid.h"

#include "main_window_frame.h"
#include "editor_app.h"
#include "scene_manager.h"
#include "editors/world_editor.h"

/*
-----------------------------------------------------------------------------
	EdSceneManager
-----------------------------------------------------------------------------
*/
EdSceneTreeView::EdSceneTreeView()
	: Super( Universe_Editor::Ptr() )
{
}

void EdSceneTreeView::OnItemSelected( AEditable* pObject )
{
	Super::OnItemSelected( pObject );
}

void EdSceneTreeView::OnItemDoubleClicked( AEditable* pObject )
{
	Super::OnItemDoubleClicked( pObject );

	Util_NewPropertyEditorDockWidget( pObject, EditorApp::GetMainFrame() );
}

/*
-----------------------------------------------------------------------------
	EdSceneManager
-----------------------------------------------------------------------------
*/
EdSceneManager::EdSceneManager()
{
	this->setWindowTitle("Scene Explorer");
	this->setObjectName("Scene Explorer");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
	this->setAcceptDrops(true);
}

EdSceneManager::~EdSceneManager()
{
}

void EdSceneManager::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdSceneManager::PostInit()
{
	//TrySelectFirstWorld();
}

void EdSceneManager::CreateWidgets()
{
	this->QDockWidget::setWidget( &m_treeWidget );
}

void EdSceneManager::CreateActions()
{
	m_viewSceneMgrAct.setText( tr("Scene Manager") );
	m_viewSceneMgrAct.setStatusTip(tr("View the scene manager panel"));
	m_viewSceneMgrAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_S ) );

	m_createNewWorldAct.setText( tr("Create a New World") );
	m_createNewWorldAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_W ) );

	m_loadWorldFromFileAct.setText( tr("Load an existing world from file") );
	m_loadWorldFromFileAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_L ) );


	m_loadRedFactionLevelAct.setText( tr("Load Red Faction Level (.rfl)") );
	m_loadRedFactionLevelAct.setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_R ) );

	m_showNodePropertiesAct.setText("Properties");
}

void EdSceneManager::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;

	menus.viewMenu->addAction( &m_viewSceneMgrAct );

	menus.projectMenu->addAction( &m_createNewWorldAct );
	menus.projectMenu->addAction( &m_loadWorldFromFileAct );
	//menus.projectMenu->addAction( &m_loadRedFactionLevelAct );
}

void EdSceneManager::ConnectSigSlots()
{
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_treeWidget );

	EditorApp::ShowOnActionTriggered( &m_viewSceneMgrAct, this );

	connect( &m_createNewWorldAct, SIGNAL(triggered(bool)), this, SLOT(slot_CreateNewWorld()) );
	connect( &m_loadWorldFromFileAct, SIGNAL(triggered(bool)), this, SLOT(slot_LoadWorldFromFile()) );
	connect( &m_loadRedFactionLevelAct, SIGNAL(triggered(bool)), this, SLOT(slot_LoadRedFactionLevel()) );

	connect( &m_showNodePropertiesAct,SIGNAL(triggered()), this, SLOT(slot_ShowEntityProperties()) );

	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_viewSceneMgrAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_createNewWorldAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_loadWorldFromFileAct );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_loadRedFactionLevelAct );

	mxCONNECT_THIS( EdSystem::Get().Event_UpdateState, EdSceneManager, UpdateAll );
}

void EdSceneManager::UpdateAll( const mxDeltaTime& deltaTime )
{
	//const FLOAT deltaSeconds = deltaTime.fTime;
}

void EdSceneManager::slot_CreateNewWorld()
{
#if 0
	const QString	saveAsFilePath = EditorUtil::Get_AssetPath_SaveFileAs(
		("Save World As..."), ("NewWorld.world"), ("World Files (*.world)")
		);
	if( !saveAsFilePath.isEmpty() )
	{
		TStackRef< World >	newWorld( new World() );
		gEditor.assets->CreateAsset( *newWorld, saveAsFilePath.toAscii().data() );

		gEngine.worlds->Add( newWorld );
	}
#endif

	mxDBG_TRACE_CALL;

	Universe_Editor::Get().CreateNewWorld();
}

void EdSceneManager::slot_LoadWorldFromFile()
{
	UNDONE;
#if 0
	const QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Load World"),
		QCoreApplication::applicationDirPath(),
		tr("Editor World Files (*.world)")
		);
	if( fileName.isEmpty() ) {
		return;
	}

	FileReader	file( fileName.toAscii().data() );
	VRET_IF_NOT( file.IsOpen() );

	ArchiveReader	archive( file );

	World* newWorld = new World();
	newWorld->Serialize( archive );

	gEngine.worlds->Add( newWorld );
#endif
}

void EdSceneManager::slot_LoadRedFactionLevel()
{
#if 0
	QFileDialog dialog(GetMainFrame(),tr("Load Existing Level"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setNameFilter( QString("Red Faction level files (*") + RED_FACTION_LEVEL_EXTENSION.ToChars() + ")" );

	static QString	prevDirectory = QCoreApplication::applicationDirPath();
	dialog.setDirectory(prevDirectory);

	if( dialog.exec() )
	{
		const QStringList selectedFiles( dialog.selectedFiles() );
		if( selectedFiles.count() > 0 )
		{
			const QString filePath = selectedFiles.first();

			FileReader	file( filePath.toAscii().data() );
			if( file.IsOpen() )
			{
				RF_Level::Ref newWorld = new RF_Level();

				newWorld->Load( file );

				UNDONE;
				//Ed_OnNewWorldCreated( newWorld );
			}
		}
	}

	prevDirectory = dialog.directory().path();

#else

	//if( !gEngine.worlds->Num() ) {
	//	MX_DEBUG_BREAK;
	//	return;
	//}
mxDEBUG_BREAK;
#if 0
	World* world = gEngine.worlds->At(0);

	ConstCharPtr pathToMaps("D:/dev/RF_PC/orig_data/multiplayer_maps/");
	ConstCharPtr pathToFile("D:/dev/RF_PC/orig_data/multiplayer_maps/dm02custom.rfl");

	FileReader	file( pathToFile.ToChars() );
	if( file.IsOpen() )
	{
		RF_Level::Ref newLevel = new RF_Level();

		newLevel->Load( file );

		world->AddEntity( newLevel );
	}
#endif
#endif
}

void EdSceneManager::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::RightDockWidgetArea, this );
}

void EdSceneManager::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::RightDockWidgetArea, this );

	if( IsLoading(stream) )
	{
		//slot_TrySelectFirstWorld();
	}
}

void EdSceneManager::ShowContextMenu( AEditable* theObject, const QPoint& globalPos )
{
	m_editedObject = theObject;

	if( m_editedObject != nil )
	{
		QMenu	contextMenu;
		contextMenu.addAction( &m_showNodePropertiesAct );

		contextMenu.exec( globalPos );
	}
}

void EdSceneManager::slot_ShowEntityProperties()
{
	if( m_editedObject != nil )
	{
		ProperyEditorDockWidget* w = new ProperyEditorDockWidget(this);

		w->SetObject( m_editedObject );

		EditorApp::GetMainFrame()->addDockWidget( Qt::RightDockWidgetArea, w );
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
