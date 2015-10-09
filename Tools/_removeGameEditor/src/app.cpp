#include "stdafx.h"

#include <Core/Serialization.h>

#include <EditorSupport/Serialization/TextSerializer.h>

//#include <Renderer/Renderer.h>
#include <Renderer/Common.h>
//#include <Engine/Worlds.h>
#include <Engine/Engine.h>
#include <Engine/Worlds.h>

#include "app.h"
#include "main_window_frame.h"
#include "editor_system.h"
#include "resource_cache.h"
#include "project_manager.h"

#define ED_DEFAULT_MAIN_WINDOW_SIZE	1024,768

/*
-----------------------------------------------------------------------------
	EdActions
-----------------------------------------------------------------------------
*/
void EdActions::Setup(QWidget* parent)
{
	exitAppAct.setText( TR("Exit") );
	exitAppAct.setShortcut(QKeySequence( Qt::ALT + Qt::Key_X ));

	editPreferencesAct.setText( TR("Preferences") );
	importStyleSheetAct.setText( TR("Import Qt Style Sheet") );
}

/*
-----------------------------------------------------------------------------
	EdMenus
-----------------------------------------------------------------------------
*/
void EdMenus::Setup(QMenuBar* theMenuBar)
{
	this->menuBar = theMenuBar;
	this->fileMenu = this->menuBar->addMenu(TR("&File"));
	this->editMenu = this->menuBar->addMenu(TR("Edit"));
	this->viewMenu = this->menuBar->addMenu(TR("View"));
	this->projectMenu = this->menuBar->addMenu(TR("Project"));
	this->buildMenu = this->menuBar->addMenu(TR("Build"));
	this->debugMenu = this->menuBar->addMenu(TR("Debug"));
	this->toolsMenu = this->menuBar->addMenu(TR("Tools"));
	this->settingsMenu = this->menuBar->addMenu(TR("Settings"));
	this->pluginsMenu = this->menuBar->addMenu(TR("Plugins"));
	this->windowsMenu = this->menuBar->addMenu(TR("Windows"));
	this->helpMenu = this->menuBar->addMenu(TR("Help"));

	EditorApp::ValidOnlyWhenProjectIsLoaded(this->editMenu);
	//EditorUtil::ValidOnlyWhenProjectIsLoaded(this->viewMenu);
	EditorApp::ValidOnlyWhenProjectIsLoaded(this->projectMenu);
	EditorApp::ValidOnlyWhenProjectIsLoaded(this->buildMenu);
	EditorApp::ValidOnlyWhenProjectIsLoaded(this->debugMenu);
	EditorApp::ValidOnlyWhenProjectIsLoaded(this->toolsMenu);
}

/*
-----------------------------------------------------------------------------
	EdApp
-----------------------------------------------------------------------------
*/
EdApp::EdApp()
{
	this->CreateMainFrame();
	this->CreateMdiArea();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();

	// create modules
	EdSystem::Get().InstallDefaultPlugins();

	//gEditorSettings.SetDefaults();

	// Initialize modules.
	{
		// create widgets, menus, etc.
		EdSystem::Get().PreInit();

		// read app settings from file
		// or load default settings
		this->ReadPersistentData();

		EdSystem::Get().PostInit();
	}

	{
		// 'Exit' should be the last action
		menus.fileMenu->addAction( &actions.exitAppAct );

		//menus.settingsMenu->addSeparator();
		menus.settingsMenu->addAction( &actions.editPreferencesAct );
		menus.settingsMenu->addAction( &actions.importStyleSheetAct );

		//menus.pluginsMenu->addAction( &actions.viewPlugisAct );
	}


	//StartTimer
	{
		// If interval is 0, then the timer event occurs once every time
		// there are no more window system events to process.
		//
		const int timerInterval = 0;	// in milliseconds

		m_timerID = this->startTimer(timerInterval);

		ensure(connect(
			this, SIGNAL(signal_FrameStarted()),
			this, SLOT(UpdateAll())
		));
	}


	mainFrame.statusBar()->showMessage(TR("Ready"));

	EdSystem::Get().Event_ProjectUnloaded();

	EdSystem::Get().Event_AppInitialized();
}

EdApp::~EdApp()
{
	gEngine.Shutdown();
}

void EdApp::Show()
{
	//QTBUG
	mainFrame.setVisible(true);

	if( MX_DEVELOPER )
	{
		mainFrame.show();	
	}
	else
	{
		mainFrame.showMaximized();
	}	
}

bool EdApp::RequestExit()
{
	//DBG_TRACE_CALL;
	{
		EdSystem::Get().ShutdownModules();

		this->SavePersistentData();

		EdSystem::Get().DestroyModules();

		//recursive call if isExiting is false
		mainFrame.close();

		this->killTimer( m_timerID );
	}
	return true;
}

static
void F_LoadStyleSheet(const QString &filePath)
{
	//QFile file(":/qss/" + sheetName.toLower() + ".qss");
	QFile file(filePath);

	file.open(QFile::ReadOnly);
	VRET_IF_NOT(file.isOpen());

	const QString styleSheet = QLatin1String(file.readAll());

	DEVOUT("Loading style sheet from file: '%s'\n", filePath.toAscii().data());

	qApp->setStyleSheet(styleSheet);

	EdSystem::Get().settings.pathToQtStyleSheet = filePath.toAscii().data();
}

static
void F_FixWidgetLayoutAfterLoading()
{
	QMainWindow* mainWindowFrame = EditorApp::GetMainFrame();

	//HACK: sometimes after loading, its size is zero
	// it's a bug in Qt with save/restore geometry, no fix yet (March 2012)

	const QSize windowSize = mainWindowFrame->size();

	enum { MIN_WINDOW_SIZE = 256 };

	if( windowSize.width() < MIN_WINDOW_SIZE || windowSize.height() < MIN_WINDOW_SIZE )
	{
		// this line doesn't work (:
		//mainWindowFrame->resize( ED_DEFAULT_MAIN_WINDOW_SIZE );
	
		// reset to default values

		mxWarnf("F_FixWidgetLayoutAfterLoading(): restoreGeometry() BUG strikes again.\n");

		EdSystem::Get().SetDefaultValues();
	}
}

void EdApp::ReadPersistentData()
{
	// Read layout
	{
		QFile		file(ED_PATH_TO_APP_LAYOUT);
		file.open(QIODevice::ReadOnly);
		if( file.isOpen() )
		{
			QDataStream	stream( &file );

			this->SerializeWidgetLayout(stream);
		}
		else
		{
			mxWarnf("Failed to read layout from file '%s'.\n",ED_PATH_TO_APP_LAYOUT);

			// load default settings
			EdSystem::Get().SetDefaultValues();
		}
	}

	F_FixWidgetLayoutAfterLoading();

	// Read settings
	{
		FileReader	file( ED_PATH_TO_APP_SETTINGS, FileRead_NoOpenError );
		if( file.IsOpen() )
		{
			TextReader	archive( file );

			EdSystem::Get().settings.Serialize( archive );

			if( !EdSystem::Get().settings.pathToQtStyleSheet.IsEmpty() )
			{
				F_LoadStyleSheet( EdSystem::Get().settings.pathToQtStyleSheet.ToChars() );
			}

			EdSystem::Get().Event_SerializeAppData( archive );
		}
	}
}

void EdApp::SavePersistentData()
{
	// Save layout
	{
		QFile		file(ED_PATH_TO_APP_LAYOUT);
		file.open(QIODevice::WriteOnly);
		if( !file.isOpen() ) {
			mxWarnf("Failed to save layout to file '%s'.\n",ED_PATH_TO_APP_LAYOUT);
			return;
		}

		QDataStream	stream( &file );
		this->SerializeWidgetLayout(stream);
	}

	// Save current settings
	{
		FileWriter	file( ED_PATH_TO_APP_SETTINGS );
		if( file.IsOpen() )
		{
			TextWriter	archive;

			EdSystem::Get().settings.Serialize( archive );

			EdSystem::Get().Event_SerializeAppData( archive );

			archive.WriteAllToStream( file );
		}
	}
}

void EdApp::SerializeWidgetLayout( QDataStream & stream )
{
	EdSystem::Get().Event_SerializeLayout( stream );

	// serialize main window's state
	// (and its children)

	SerializeMainWindow_GeometryAndState( stream, &mainFrame );
}

void EdApp::CreateMainFrame()
{
	mainFrame.resize(ED_DEFAULT_MAIN_WINDOW_SIZE);
}

void EdApp::CreateMdiArea()
{
	//mdiArea.setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	//mdiArea.setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	//mainFrame.setCentralWidget( &mdiArea );
	//connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
	//	this, SLOT(OnSubWindowActivated()));
}

void EdApp::CreateActions()
{
	actions.Setup(&mainFrame);
}

void EdApp::CreateMenus()
{
	menus.Setup(mainFrame.menuBar());
}

void EdApp::ConnectSigSlots()
{
	connect(
		&actions.exitAppAct, SIGNAL(triggered()),
		this, SLOT(RequestExit())
		);

	connect(
		&actions.importStyleSheetAct, SIGNAL(triggered()),
		this, SLOT(slot_ImportStyleSheet())
		);

	MX_CONNECT_THIS( EdSystem::Get().Event_ProjectLoaded, EdApp, slot_OnProjectLoaded );
	MX_CONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded, EdApp, slot_OnProjectUnloaded );
}

void EdApp::slot_OnProjectLoaded()
{
	emit projectLoadedFlag( true );
}

void EdApp::slot_OnProjectUnloaded()
{
	emit projectLoadedFlag( false );
}

void EdApp::UpdateMainWindowTitle()
{
	// Refresh main window's title.

	EdProject* currentProject = EditorApp::GetCurrentProject();

	if( currentProject != nil )
	{
		EditorApp::GetMainFrame()->setWindowTitle(
			ED_APPLICATION_NAME + QString(" - ")
			+ currentProject->GetName() );
	}
	else
	{
		EditorApp::GetMainFrame()->setWindowTitle( ED_APPLICATION_NAME );
	}
}

void EdApp::slot_ImportStyleSheet()
{
	QFileDialog dialog( EditorApp::GetMainFrame() );
	{
		dialog.setViewMode(QFileDialog::Detail);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setAcceptMode(QFileDialog::AcceptOpen);
	}
	dialog.setFilter("Qt Style Sheets (*.qss)");
	//dialog.setDirectory( initialDir );

	if( dialog.exec() )
	{
		const String selectedFileName = QtSupport::F_Get_Selected_File_Name( dialog );
		EdSystem::Get().settings.pathToQtStyleSheet = selectedFileName;

		F_LoadStyleSheet(selectedFileName.ToChars());
	}
}

void EdApp::timerEvent( QTimerEvent* theEvent )
{
	if( theEvent->timerId() == m_timerID )
	{
		emit signal_FrameStarted();
	}

	theEvent->accept();
}

void EdApp::UpdateAll()
{
	MX_PROFILE_FUNCTION;

	const bool bProjectIsLoaded = (EditorApp::GetCurrentProject() != nil);

	if( bProjectIsLoaded )
	{
		const F4 deltaSeconds = m_gameTimer.TickFrame();
		const INT deltaMilliSeconds = deltaSeconds * 1e3f;

		gfxBEStats.lastFrameRenderTime = UINT(deltaSeconds * 1e6f);


		mxDeltaTime	deltaTime;
		deltaTime.fTime = deltaSeconds;
		deltaTime.iTime = deltaMilliSeconds;


		gEngine.worlds->Tick(deltaSeconds);

		EdSystem::Get().Event_UpdateState( deltaTime );

		// Draw all.

		gfxBEStats.Reset();
		gfxStats.Reset();

		EdSystem::Get().Event_RenderViewports();
	}

#if MX_DEBUG
	mxSleepMilliseconds(1);
#endif

	MX_PROFILE_INCREMENT_FRAME_COUNTER;
}


EdAction::EdAction( const QString& text, QObject* parent )
	: QAction( text, parent )
{
	this->userData = nil;

	this->connect( this, SIGNAL(triggered()), this, SLOT(slot_OnTriggered()));
}

void EdAction::slot_OnTriggered()
{
	if( this->OnTriggered )
	{
		this->OnTriggered( this->userData );
	}
}

//---------------------------------------------------------------------------

namespace EditorApp
{

	QMainWindow* GetMainFrame()
	{
		return &EdApp::Get().mainFrame;
	}

	QStatusBar* GetStatusBar()
	{
		return GetMainFrame()->statusBar();
	}

	void ValidOnlyWhenProjectIsLoaded( QWidget* w )
	{
		QObject::connect( EdApp::Ptr(), SIGNAL(projectLoadedFlag(bool)), w, SLOT(setEnabled(bool)));
	}

	void ValidOnlyWhenProjectIsLoaded( QAction* a )
	{
		QObject::connect( EdApp::Ptr(), SIGNAL(projectLoadedFlag(bool)), a, SLOT(setEnabled(bool)));
	}

	void ShowOnActionTriggered( QAction* a, QWidget* w )
	{
		QObject::connect( a, SIGNAL(triggered(bool)), w, SLOT(show()) );
	}
	void VisibleWhenActionIsChecked( QAction* a, QWidget* w )
	{
		a->setCheckable(true);
		QObject::connect( a, SIGNAL(toggled(bool)), w, SLOT(setVisible(bool)) );
	}
	void SetActionCheckedIfVisible( QAction* a, QWidget* w )
	{
		a->setCheckable(true);
		a->setChecked( w->isVisible() );
	}
	//---------------------------------------------------------------------------
	//void ExpandTreeViewOnDataChanged( QTreeView* v, QAbstractItemModel* m )
	//{
	//	QObject::connect( m, SIGNAL(dataChanged(QModelIndex,QModelIndex)), v, SLOT(expand(QModelIndex)) );
	//}

}//namespace EditorUtil



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
