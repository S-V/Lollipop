#include "StdAfx.h"

#include <Renderer/Materials/Phong.h>
#include <Renderer/Materials/PlainColor.h>

#include <QtSupport/qt_common.h>

#include "editor_common.h"
#include "addnewitemdialog.h"

/*
-----------------------------------------------------------------------------
	EdAddNewItemDialog
-----------------------------------------------------------------------------
*/
EdAddNewItemDialog::EdAddNewItemDialog( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi(this);

	connect(ui.addButton,SIGNAL(pressed()),this,SLOT(accept()));
	connect(ui.cancelButton_2,SIGNAL(pressed()),this,SLOT(reject()));

	connect(ui.treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slot_OnItemDoubleClicked(QModelIndex)));
	connect(ui.listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slot_OnItemDoubleClicked(QModelIndex)));

	connect(ui.treeView,SIGNAL(activated(QModelIndex)),this,SLOT(slot_OnItemActivated(QModelIndex)));
	connect(ui.treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(slot_OnItemActivated(QModelIndex)));
	connect(ui.treeView,SIGNAL(pressed(QModelIndex)),this,SLOT(slot_OnItemActivated(QModelIndex)));
	connect(ui.treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slot_OnItemActivated(QModelIndex)));


	ui.treeView->setModel( &m_treeViewModel );
	ui.listView->setModel( &m_listViewModel );
}

EdAddNewItemDialog::~EdAddNewItemDialog()
{

}

void EdAddNewItemDialog::SetRoot( AEditable* root )
{
	m_treeViewModel.SetRootObject( root );
	m_listViewModel.SetRootObject( root );
}

void EdAddNewItemDialog::accept()
{
	QModelIndex selectedIndex = ui.listView->currentIndex();
	if( selectedIndex.isValid() )
	{
		this->slot_OnItemDoubleClicked( selectedIndex );
	}
}

void EdAddNewItemDialog::slot_OnItemDoubleClicked( const QModelIndex& index )
{
	Assert( index.isValid() );

	this->pSelectedObject = static_cast<AEditable*>( index.internalPointer() );

	// if this is a leaf node...
	if( this->pSelectedObject->uiIsLeafNode() )
	{
		Super::accept();
	}
	else
	{
		// show its chilren in the list view
		m_listViewModel.SetRootObject( this->pSelectedObject );
	}
}

void EdAddNewItemDialog::slot_OnItemActivated( const QModelIndex& index )
{
	Assert( index.isValid() );

	this->pSelectedObject = static_cast<AEditable*>( index.internalPointer() );

	m_listViewModel.SetRootObject( this->pSelectedObject );
}

void EdAddNewItemDialog::ExecuteCallback( void* pUserData )
{
	AssertPtr(pSelectedObject);

	EdGroup* pGroup = SafeCast<EdGroup>(pSelectedObject);
	AssertPtr(pGroup);
	if( pGroup != nil )
	{
		if( pGroup->callback )
		{
			(*pGroup->callback)( pGroup, pUserData );
		}
		else
		{
			mxWarnf("No callback associated with '%s'\n",pGroup->name);
		}
	}
}
