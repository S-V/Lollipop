#ifndef CREATE_ASSET_DB_DIALOG_H
#define CREATE_ASSET_DB_DIALOG_H

#include <QDialog>
#include "ui_create_asset_db_dialog.h"

#include "content_manager.h"

class Create_Asset_Db_Dialog : public QDialog
	, DependsOnGlobal< AppSettings >
{
	Q_OBJECT

public:
	typedef QDialog Super;

	Create_Asset_Db_Dialog(QWidget *parent = 0);
	~Create_Asset_Db_Dialog();

	const String GetPathToSrcAssets() const
	{
		return ui.lineEdit_pathToSrcAssets->text().toAscii().data();
	}
	const String GetPathToIntAssets() const
	{
		return ui.lineEdit_pathToIntAssets->text().toAscii().data();
	}
	const String GetPathToBinAssets() const
	{
		return ui.lineEdit_pathToBinAssets->text().toAscii().data();
	}

public slots:
    virtual void accept() override;

private slots:
	void slot_SelectPathToSrcAssets();
	void slot_SelectPathToIntAssets();
	void slot_SelectPathToBinAssets();

private:
	Ui::Create_Asset_Db_Dialog ui;
};

#endif // CREATE_ASSET_DB_DIALOG_H
