#include "stdafx.h"
#include "packagebuilder.h"

#include <Core/Resource/Package.h>
#include <Core/Resource/Package.h>

/*
-----------------------------------------------------------------------------
	PackageBuilder
-----------------------------------------------------------------------------
*/
PackageBuilder::PackageBuilder(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	connect(
		ui.pushButton_BuildArchive,
		SIGNAL(clicked()),
		this,
		SLOT(BuildArchive())
	);
}

PackageBuilder::~PackageBuilder()
{

}

void PackageBuilder::BuildArchive()
{
	//const QString inputDir = ui.lineEdit_InputFolder->text();
	//const QString outputDir = ui.lineEdit_OutputFolder->text();

	PCHARS inputDir =
		"D:/dev/RF_PC/data/maps1/"
		//"D:/dev/RF_PC/doc"
		//"R:/_/Bin/Cache/"
		;
	PCHARS outputDir =
		"R:/_/Bin/"
		;

	mxPackage	newPackage;
	newPackage.CreateNew( inputDir, outputDir );
}



bool EditorHooks::Show_YesNo_Warning( const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	const int ret = QMessageBox::warning(nil, "MyApp",
		buffer,
		QMessageBox::Yes | QMessageBox::No
	);
	if( ret == QMessageBox::Yes )
	{
		return true;
	}
	else
	{
		return false;
	}
}
