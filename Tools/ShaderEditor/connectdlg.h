#ifndef CONNECTDLG_H
#define CONNECTDLG_H

#include <QDialog>
#include "ui_connectdlg.h"

class ConnectDlg : public QDialog
{
	Q_OBJECT

public:
	ConnectDlg(QWidget *parent = 0);
	~ConnectDlg();

public slots:
	void accept();
	void decline();

private:
	Ui::ConnectDlg ui;

public:
	QString		host;
	int			port;
};

#endif // CONNECTDLG_H
