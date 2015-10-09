#include "stdafx.h"
#include "shadergenui.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ShaderGenUI w;
	w.show();
	return a.exec();
}
