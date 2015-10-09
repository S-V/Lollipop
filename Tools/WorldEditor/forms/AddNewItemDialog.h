#ifndef ADDNEWITEMDIALOG_H
#define ADDNEWITEMDIALOG_H

#include <QDialog>
#include "generated/ui_addnewitemdialog.h"

#include "widgets/tree_views.h"

/*
-----------------------------------------------------------------------------
	EdAddNewItemDialog
-----------------------------------------------------------------------------
*/
class EdAddNewItemDialog : public QDialog
{
	Q_OBJECT

public:
	typedef QDialog Super;

	EdAddNewItemDialog( QWidget *parent = 0 );
	~EdAddNewItemDialog();

	void SetRoot( AEditable* root );

public:
	TPtr<AEditable>		pSelectedObject;

	// should be called right after exec()
	void ExecuteCallback( void* pUserData );

public slots:
	virtual void accept() override;

private slots:
	void slot_OnItemDoubleClicked( const QModelIndex& index );
	void slot_OnItemActivated( const QModelIndex& index );

private:
	Ui::AddNewItemDialog ui;
	EdTreeViewModel		m_treeViewModel;
	EdTreeViewModel		m_listViewModel;
};

#endif // ADDNEWITEMDIALOG_H
