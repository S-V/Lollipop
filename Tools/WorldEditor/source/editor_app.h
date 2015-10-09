#pragma once

#include <Base/Util/LogUtil.h>

#include <Core/Util/Timer.h>
#include <QtSupport/qt_common.h>

#include "editor_common.h"
#include "editor_system.h"
#include "main_window_frame.h"


/*
-----------------------------------------------------------------------------
	EdActions
-----------------------------------------------------------------------------
*/
struct EdActions
{
	EdAction	exitAppAct;

	EdAction	editPreferencesAct;
	EdAction	importStyleSheetAct;

public:
	void Setup(QWidget* parent);
};

/*
-----------------------------------------------------------------------------
	EdMenus
-----------------------------------------------------------------------------
*/
struct EdMenus
{
	TPtr< QMenu >	fileMenu;
	TPtr< QMenu >	editMenu;
	TPtr< QMenu >	viewMenu;
	TPtr< QMenu >	projectMenu;
	TPtr< QMenu >	buildMenu;
	TPtr< QMenu >	debugMenu;
	TPtr< QMenu >	toolsMenu;
	TPtr< QMenu >	settingsMenu;
	TPtr< QMenu >	pluginsMenu;
	TPtr< QMenu >	windowsMenu;
	TPtr< QMenu >	helpMenu;

	TPtr< QMenuBar >	menuBar;

public:
	void Setup(QMenuBar* theMenuBar);
};



/*
-----------------------------------------------------------------------------
	EdWidgets
-----------------------------------------------------------------------------
*/
struct EdWidgets
{

public:
	EdWidgets();
	~EdWidgets();
};

/*
-----------------------------------------------------------------------------
	EdApp
-----------------------------------------------------------------------------
*/
class EdApp
	: public QObject
	, public TGlobal< EdApp >
{
	Q_OBJECT

public:	// global variables

	EdMainWindowFrame	mainFrame;
	QMdiArea			mdiArea;
	EdActions			actions;
	EdMenus				menus;

public:
	EdApp();	// Initializes everything.
	~EdApp();	// Closes the editor.

	void Show();

signals:
	// used to notify Qt widgets about project status change
	void projectLoadedFlag( bool bLoaded );

	// internal; used with Qt timer to advance editor_app state
	void signal_FrameStarted();

public slots:
	bool RequestExit();

private slots:
	void UpdateAll();
	void slot_ImportStyleSheet();

private:
	void ReadPersistentData();
	void SavePersistentData();

	void SerializeWidgetLayout( QDataStream & stream );

	void CreateMainFrame();
	void CreateMdiArea();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

	// emit Qt signals to notify widgets about project status
	void slot_OnProjectLoaded();
	void slot_OnProjectUnloaded();

	void UpdateMainWindowTitle();

protected:
	virtual void timerEvent( QTimerEvent* theEvent );


private:
	// used for game loop/autosave/etc
	GameTimer	m_gameTimer;

	// qt timer for real-time updates
	int		m_timerID;
};
