//@refactor:
//
#include "stdafx.h"
#include <QtGui/QApplication>

#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/Serialization/TextSerializer.h>

#include <Driver/ConsoleWindow.h>

#include <Engine/Engine.h>

#include "editor_app.h"
#include "editors/world_editor.h"

static
int MyAssertCallback( const char* expr, const char* file, const char* func, unsigned int line, bool* ignore )
{
	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"Assertion failed:\n\n '%s'\n\n in file %s,\nfunction '%s',\nline %d\n",
		expr, file, func, line );

	QMessageBox		messageBox;
	messageBox.setIcon(QMessageBox::Icon::Critical);
	messageBox.setWindowTitle("Assertion failure");
	messageBox.setText(buffer);
	messageBox.setInformativeText("\nDo you wish to debug?\nYes - 'Debug Break', No - 'Exit', Cancel - 'Don't bother me again!\n");

	//msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	//msgBox.setDefaultButton(QMessageBox::Save);


	QPushButton * debugButton = messageBox.addButton( "Debug", QMessageBox::ButtonRole::YesRole );
	QPushButton * exitButton = messageBox.addButton( "Exit", QMessageBox::ButtonRole::DestructiveRole );
	QPushButton * ignoreButton = messageBox.addButton( "Ignore", QMessageBox::ButtonRole::RejectRole );

	messageBox.setDefaultButton( debugButton );

	const int ret = messageBox.exec();
	(void)ret;

	if( messageBox.clickedButton() == debugButton ) {
		DebugBreak();
	}
	if( messageBox.clickedButton() == exitButton ) {
		mxFatal( "Assertion failed!\n" );
	}
	if( messageBox.clickedButton() == ignoreButton ) {
		if( ignore != nil ) {
			*ignore = true;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	QApplication editor_app(argc, argv);

#if 0
	QPixmap pixmap(":/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	editor_app.processEvents();
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



	QDir::setCurrent(editor_app.applicationDirPath());


	SetUserAssertCallback( &MyAssertCallback );

	SetupBaseUtil	_firstInitBase;
	FileLogUtil		_firstInitFileLog;
	SetupCoreUtil	_firstInitCore;
	InitEditorUtil	_firstInitEditor;

	EdSystem		editorSystem;

	Universe_Editor levelEditor;


	// Construct the engine object.
	{
		SEngineInitArgs	engineInitParams;
		engineInitParams.client = &levelEditor;

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

	return editor_app.exec();
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
