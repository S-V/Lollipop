#include "StdAfx.h"
#include "connectdlg.h"

ConnectDlg::ConnectDlg(QWidget *parent)
	: QDialog(parent)
	, host("localhost")
	, port(Net::DEFAULT_PORT)
{
	ui.setupUi(this);

	ui.spinBox_Port->setValue(Net::DEFAULT_PORT);

	connect(
		ui.pushButton_OK, SIGNAL(clicked()),
		this, SLOT(accept())
	);
	connect(
		ui.pushButton_Cancel, SIGNAL(clicked()),
		this, SLOT(decline())
	);
}

ConnectDlg::~ConnectDlg()
{

}
void ConnectDlg::accept()
{
	this->host = ui.lineEdit_Host->text();
	this->port = ui.spinBox_Port->value();

	this->close();
}
void ConnectDlg::decline()
{
	this->close();
}
