#include "stdafx.h"
#include "shadereditor.h"
#include <QtGui/QApplication>

#include "connectdlg.h"

struct SetupEngineUtil 
{
	SetupEngineUtil()
	{
		mxInitializeBase();
		SetupCoreSubsystem();
		Net::mxInitializeNetwork();
	}
	~SetupEngineUtil()
	{
		Net::mxShutdownNetwork();
		ShutdownCoreSubsystem();
		mxShutdownBase();
	}
};


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#if defined(Q_WS_WIN)
	//a.setStyle(new QPlastiqueStyle());
	a.setStyle(new QCleanlooksStyle());
#endif

	SetupEngineUtil	setupEngine;
	FileLogUtil		fileLog;

	int retCode = -1;

	{
		ConnectDlg	dlg;
		dlg.exec();

		ShaderEditor::ConnectionSettings	cInfo;
		cInfo.host = dlg.host;
		cInfo.port = dlg.port;

		ShaderEditor w( cInfo );
		w.show();

		retCode = a.exec();
	}

	return retCode;
}
