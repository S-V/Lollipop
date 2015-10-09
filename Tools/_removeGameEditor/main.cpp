//@refactor:
//@ 
#include "stdafx.h"
#include <QtGui/QApplication>

#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/Serialization/TextSerializer.h>

#include <Driver/ConsoleWindow.h>

#include <Engine/Engine.h>

#include "app.h"
#include "src/editors/world_editor.h"



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

#if 0
	QPixmap pixmap(":/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	app.processEvents();
#endif

	bool bCustomStyle = 0;
	if(bCustomStyle)
	{
		QStyle* style = QStyleFactory::create( "motif" );
		QApplication::setStyle( style );
	}

	bool bCustomStyleSheet = false;
	if(bCustomStyleSheet)
	{
		QFile file(":/qss/default.qss");
		file.open(QFile::ReadOnly);
		QString styleSheet = QLatin1String(file.readAll());

		qApp->setStyleSheet(styleSheet);
	}



	QDir::setCurrent(app.applicationDirPath());


	SetupBaseUtil	setupBase;

	EdSystem		editorSystem;

	FileLogUtil		fileLog;
	SetupCoreUtil	setupCore;

	InitEditorUtil	setupEditor;

	Global_Level_Editor	levelEditor;

	// Construct the engine object.
	{
		SEngineInitArgs	engineInitParams;
		engineInitParams.levelMgr = &levelEditor;

		gEngine.Initialize( engineInitParams );
	}

	// Construct the editor application.

	EdApp		editorApp;

	CConsole	win32console(_NoInit);
	{
		bool bCreateConsole = false;
		gCore.config->GetBool("bCreateConsoleWindow",bCreateConsole);

		if( bCreateConsole )
		{
			win32console.Create("Win32 Console");
		}
	}


	editorApp.Show();

	return app.exec();
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
