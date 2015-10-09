#include "stdafx.h"
#include "shadereditor.h"

#include <Graphics/Graphics_DX11.h>

#include "gpuresourcesview.h"
#include "connectdlg.h"
#include "outputwindow.h"
#include "codeeditor.h"
#include "consolepanel.h"
#include "hlslsyntaxhighlighter.h"
#include "utils.h"

const QString EDITOR_SETTINGS_FILE ("layout.dat");
const QString EDITOR_SAVED_DATA_FILE ("settings.dat");

struct PrivateGlobals
{
	TRefPtr< HLSLUtil >		hlslUtil;

	CodeEditor			codeEditor;
	GpuResourcesView	gpuResourcesView;

	PrivateGlobals()
	{
		hlslUtil = new HLSLUtil();
	}
};

TBlob< PrivateGlobals >		globals;

ShaderEditor::ShaderEditor( const ConnectionSettings& settings )
	: QMainWindow( nil, (Qt::WFlags)0)
{
	this->ui.setupUi(this);

	this->ui.mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->ui.mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	this->setAcceptDrops(true);

	new(&globals)PrivateGlobals();

	this->createDockWindows();

	this->initStuffForTesting();

	this->loadSettings( EDITOR_SETTINGS_FILE );

	this->connectToServer(settings);

	// Set up rendering occurs each millisecond.

	ensure(connect(
		this, SIGNAL(frameStarted()),
		this, SLOT(Tick())
		));

	this->startTimer(1);

	
}

ShaderEditor::~ShaderEditor()
{
	this->saveSettings( EDITOR_SETTINGS_FILE );

	Client_TCP::Close();

	globals.Destruct();
}

void ShaderEditor::createDockWindows()
{
	this->addDockWidget( Qt::DockWidgetArea::LeftDockWidgetArea, &GpuResourcesView::Get() );
	GpuResourcesView::Get().showMaximized();

	//ConsolePanel_DockWidget* console = new ConsolePanel_DockWidget(this);
	//this->addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, console );
	//console->showMaximized();

	OutputWindow* outputWnd = new OutputWindow(this);
	this->addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, outputWnd );
	outputWnd->showMaximized();


	CodeEditor* doc = CodeEditor::GetInstance();
	this->codeEditorWindow = ui.mdiArea->addSubWindow(doc,Qt::SubWindow);
	this->codeEditorWindow->setAttribute(Qt::WA_DeleteOnClose,false);
	this->codeEditorWindow->showMaximized();
	this->codeEditorWindow->hide();
}

void ShaderEditor::saveSettings( const QString& fileName )
{
	QFile file( fileName );
	if( !file.open( QIODevice::OpenModeFlag::WriteOnly ) ) {
		QMessageBox::warning ( this, "Warning",
			QString("Failed to open file '%1' for saving current settings.")
			.arg(fileName)
		);
		return;
	}

	file.write( this->saveGeometry() );
	file.write( this->saveState() );

	//------------------------------------------------------
	//mxArchive::Ref archive = IOServer::Get().OpenWriter(EDITOR_SAVED_DATA_FILE.toAscii().data());
	//if( archive != nil )
	//{
	//	//		this->persistentData.Serialize(*archive);
	//	this->mainViewport->camera->Serialize(*archive);
	//	this->developerPanel->Serialize(*archive);
	//}
}

void ShaderEditor::loadSettings( const QString& fileName )
{
	// Load saved settings from file.

	QFile	file( fileName );

	if( !file.open( QIODevice::OpenModeFlag::ReadOnly ) )
	{
		this->ui.statusBar->showMessage(
			tr("Failed to load editor settings from file: '%1'.")
			.arg(fileName)
		);
		return;
	}

	QByteArray geometryData( file.readAll() );
	QByteArray stateData( file.readAll() );

	this->restoreGeometry( geometryData );
	this->restoreState( stateData );

	this->ui.statusBar->showMessage(
		tr("Loaded editor settings from file: '%1'.")
		.arg(EDITOR_SETTINGS_FILE)
	);

	// so that widgets with restored geometry are shown
	this->show();
}

void ShaderEditor::timerEvent( QTimerEvent* evt )
{
	emit frameStarted();

	evt->accept();
}

void ShaderEditor::keyPressEvent( QKeyEvent* evt )
{
	__super::keyPressEvent(evt);
}

void ShaderEditor::connectToServer(const ConnectionSettings& settings)
{
	Timeout	timeout;
	timeout.seconds = 1;
	timeout.milliseconds = 0;

	this->cmdExec.SetTimeout( timeout );


	const bool bOk = Connect( settings.host.toAscii().data(), settings.port );
	if( !bOk )
	{
		mxMsgBoxf(TEXT("Warning"),TEXT("Failed to connect to the host"));
		return;
	}

	GpuResourcesView::Get().OnConnectedTo( GetSocket() );
}

void ShaderEditor::on_received_packet( const RawPacket& p )
{
	__super::on_received_packet(p);
}

void ShaderEditor::on_disconnected()
{
	__super::on_disconnected();
}

void ShaderEditor::initStuffForTesting()
{
	//DocumentEditor* newDocument = new DocumentEditor(this);
	//ui.mdiArea->addSubWindow(newDocument);

	//new HLSLSyntaxHighlighter(newDocument->document());

	//newDocument->showMaximized();

	//LoadShader("test.fx");
}

void ShaderEditor::Tick()
{
	static FLOAT	lastFrameTime = 0.0f;

	FLOAT currTime = mxGetTimeInSeconds();
	FLOAT deltaTime = currTime - lastFrameTime;


	if( Client_TCP::IsConnected() )
	{
		Client_TCP::Tick();
	}


	lastFrameTime = currTime;
}

void UpdateWindowTitle( QMainWindow* window, const QString& document )
{
	QString	title("Shader Browser");
	title.append(" - ");
	title.append(document);

	window->setWindowTitle(title);
}

void ShaderEditor::LoadShader(const QString& name)
{
	CodeEditor* doc = CodeEditor::GetInstance();

	const bool bOk = doc->loadDocument(name);

	if( bOk )
	{
		//doc->showMaximized();
		doc->show();

		//this->codeEditorWindow

		UpdateWindowTitle(this,name);
	}
}
