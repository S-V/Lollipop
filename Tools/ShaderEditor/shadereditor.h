#ifndef SHADEREDITOR_H
#define SHADEREDITOR_H

#include <QtGui/QMainWindow>
#include "ui_shadereditor.h"

#include "gpuresourcesview.h"

//#include <QtNetwork/QtNetwork>

#include <Network/Protocol.h>
using namespace Net;


#include <Graphics/Graphics_Protocol.h>

class CodeEditor;

/*
--------------------------------------------------------------
	ShaderEditor
--------------------------------------------------------------
*/
class ShaderEditor
	: public QMainWindow
	, public Net::Client_TCP
	, public TSingleton< ShaderEditor >
{
	Q_OBJECT

public:
	struct ConnectionSettings 
	{
		QString	host;
		int	port;
	};
	ShaderEditor( const ConnectionSettings& settings );
	~ShaderEditor();

public:

	void LoadShader(const QString& name);

public:
	// Load/Save saved settings (e.g. UI layout, camera config) from file.
	void saveSettings( const QString& fileName );
	void loadSettings( const QString& fileName );

signals:

	void frameStarted();

private slots:
	void Tick();

protected:	// override Net::Client_TCP
	
	// the packet is removed from the queue
	virtual void on_received_packet( const RawPacket& p );

	// called when the client has disconnected from the server
	virtual void on_disconnected();

protected:	// override QWidget

	// used for signaling time to start rendering
	virtual void timerEvent( QTimerEvent* evt );

	virtual void keyPressEvent( QKeyEvent* evt );

private:
	void createDockWindows();
	void connectToServer(const ConnectionSettings& settings);
	void initStuffForTesting();

private:
	Ui::ShaderEditorClass ui;

	QMdiSubWindow* codeEditorWindow;

public_internal:

	NetCommandHelper	cmdExec;
};

#endif // SHADEREDITOR_H
