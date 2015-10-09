#include "StdAfx.h"
#include "create_asset_db_dialog.h"

//---------------------------------------------------------------------------

static
void F_Select_Existing_Directory( QWidget* parent, QLineEdit* lineEdit )
{
	//@fixme: it takes too long to show a dialog window
	mxPLATFORM_PROBLEM("Bug in Qt 4? Nasty thread deadlocks?");
	if(0)
	{
		QFileDialog::Options options = /*QFileDialog::DontResolveSymlinks |*/ QFileDialog::ShowDirsOnly;

		QString directory = QFileDialog::getExistingDirectory(parent,
			QString("QFileDialog::getExistingDirectory()"),
			lineEdit->text(),
			options);

		if (!directory.isEmpty()) {
			lineEdit->setText(directory);
		}
	}
	else
	{
		QFileDialog dialog( parent, "Select Existing Folder" );
		{
			dialog.setViewMode(QFileDialog::Detail);
			dialog.setFileMode(QFileDialog::Directory);
			dialog.setAcceptMode(QFileDialog::AcceptOpen);
		}
		dialog.selectFile(lineEdit->text());

		if( dialog.exec() )
		{
			const QString directory = dialog.directory().path();
			if (!directory.isEmpty()) {
				lineEdit->setText(directory);
			}
		}
	}
}

//---------------------------------------------------------------------------

static
bool F_Check_LineEdit_Dir_Is_Ok( QWidget* parent, QLineEdit* lineEdit, const QString& errorMsg )
{
	const QString currText = lineEdit->text();

	if( currText.isEmpty() ) {
		QMessageBox::critical( parent, "Input error", errorMsg );
		lineEdit->setFocus();
		return false;
	}

	if( !QDir(currText).exists() ) {
		QMessageBox::critical( parent, "Input error", QString("Directory '%1' doesn't exist").arg(currText) );
		lineEdit->setFocus();
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------

Create_Asset_Db_Dialog::Create_Asset_Db_Dialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setWindowTitle("Create a new Asset Database");


	connect(
		ui.pushButton_OK, SIGNAL(clicked()),
		this, SLOT(accept())
		);

	connect(
		ui.pushButton_Cancel, SIGNAL(clicked()),
		this, SLOT(reject())
		);


	connect(
		ui.pushButton_SelectPathToSrcAssets, SIGNAL(clicked()),
		this, SLOT(slot_SelectPathToSrcAssets())
		);

	connect(
		ui.pushButton_SelectPathToIntAssets, SIGNAL(clicked()),
		this, SLOT(slot_SelectPathToIntAssets())
		);

	connect(
		ui.pushButton_SelectPathToBinAssets, SIGNAL(clicked()),
		this, SLOT(slot_SelectPathToBinAssets())
		);


	ui.lineEdit_pathToSrcAssets->setText( AppSettings::Get().pathToSrcAssets.ToChars() );
	ui.lineEdit_pathToIntAssets->setText( AppSettings::Get().pathToIntAssets.ToChars() );
	ui.lineEdit_pathToBinAssets->setText( AppSettings::Get().pathToBinAssets.ToChars() );
}

Create_Asset_Db_Dialog::~Create_Asset_Db_Dialog()
{
	AppSettings::Get().pathToSrcAssets = ui.lineEdit_pathToSrcAssets->text().toAscii().data();
	AppSettings::Get().pathToIntAssets = ui.lineEdit_pathToIntAssets->text().toAscii().data();
	AppSettings::Get().pathToBinAssets = ui.lineEdit_pathToBinAssets->text().toAscii().data();
}

void Create_Asset_Db_Dialog::accept()
{
	if( F_Check_LineEdit_Dir_Is_Ok( this, ui.lineEdit_pathToSrcAssets, "Specify path to source assets" )
		&& F_Check_LineEdit_Dir_Is_Ok( this, ui.lineEdit_pathToIntAssets, "Specify path to intermediate assets" )
		&& F_Check_LineEdit_Dir_Is_Ok( this, ui.lineEdit_pathToBinAssets, "Specify path to compiled assets" )
		)
	{
		return Super::accept();
	}
}

void Create_Asset_Db_Dialog::slot_SelectPathToSrcAssets()
{
	F_Select_Existing_Directory( this, ui.lineEdit_pathToSrcAssets );
}

void Create_Asset_Db_Dialog::slot_SelectPathToIntAssets()
{
	F_Select_Existing_Directory( this, ui.lineEdit_pathToIntAssets );
}

void Create_Asset_Db_Dialog::slot_SelectPathToBinAssets()
{
	F_Select_Existing_Directory( this, ui.lineEdit_pathToBinAssets );
}

