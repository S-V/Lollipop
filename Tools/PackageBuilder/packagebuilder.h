#ifndef PACKAGEBUILDER_H
#define PACKAGEBUILDER_H

#include <Core/Resource/Package.h>

#include <QtGui/QMainWindow>
#include "ui_packagebuilder.h"

class PackageBuilder : public QMainWindow
{
	Q_OBJECT

public:
	PackageBuilder(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PackageBuilder();

private slots:
	void BuildArchive();

private:
	Ui::PackageBuilderClass ui;
};

#endif // PACKAGEBUILDER_H
