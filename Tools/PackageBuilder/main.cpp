#include "stdafx.h"
#include "packagebuilder.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SetupCoreUtil	setupCoreSys;

	PackageBuilder w;
	w.show();
	return a.exec();
}
