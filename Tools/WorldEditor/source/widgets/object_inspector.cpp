#include "stdafx.h"

#include "editor_app.h"
#include "widgets/object_inspector.h"

///*
//-----------------------------------------------------------------------------
//	EdWidget_ObjectInspector
//-----------------------------------------------------------------------------
//*/
//EdWidget_ObjectInspector::EdWidget_ObjectInspector( QWidget* parent )
//	: Super( parent )
//{
//	this->setSelectionMode(QTreeView::SingleSelection);
//	this->setSelectionBehavior(QTreeView::SelectRows);
//
//	this->setAutoScroll(true);
//	this->setRootIsDecorated( false );
//	this->setAlternatingRowColors(true);
//	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//
//	this->setEditTriggers( QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked );
//	//this->setEditTriggers( QAbstractItemView::AllEditTriggers );
//
//	this->header()->setResizeMode(QHeaderView::ResizeToContents);
//
//
//	this->setAcceptDrops(true);
//	this->setDragEnabled(true);
//	//this->setAutoExpandDelay(1000);	// 1 sec
//	//this->setDragDropMode(DragDropMode::DropOnly);
//	this->setDragDropMode(DragDropMode::DragDrop);
//	this->setDropIndicatorShown(true);
//
//
//	//this->setUniformRowHeights( true );
//
//
//	this->setModel( &m_model );
//	this->setItemDelegate( &m_delegate );
//}
//
//EdWidget_ObjectInspector::~EdWidget_ObjectInspector()
//{
//}

/*
-----------------------------------------------------------------------------
	EdObjectInspector
-----------------------------------------------------------------------------
*/
EdObjectInspector::EdObjectInspector()
	: Super( EditorApp::GetMainFrame() )
{
	this->setWindowTitle("Object Inspector");
	this->setObjectName("Object Inspector");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
}

EdObjectInspector::~EdObjectInspector()
{
}

void EdObjectInspector::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void EdObjectInspector::CreateWidgets()
{
}

void EdObjectInspector::CreateActions()
{
	m_actViewObjectInspector.setText( tr("Object Inspector") );
}

void EdObjectInspector::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_actViewObjectInspector );
}

void EdObjectInspector::ConnectSigSlots()
{
	EditorApp::ShowOnActionTriggered( &m_actViewObjectInspector, this );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_actViewObjectInspector );
}

void EdObjectInspector::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::RightDockWidgetArea, this );
}

void EdObjectInspector::Shutdown()
{
	//this->Property_Editor_Dock_Widget::SetObject( nil );
}

void EdObjectInspector::SerializeWidgetLayout( QDataStream & stream )
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::RightDockWidgetArea, this );
}

void EdObjectInspector::OnSelectionChanged( AEditable* theObject )
{
	//this->Property_Editor_Dock_Widget::SetObject( theObject );
}

void EdObjectInspector::OnObjectDestroyed( AEditable* theObject )
{
	//if( this->Property_Editor_Dock_Widget::GetObject() == theObject )
	//{
	//	this->Property_Editor_Dock_Widget::SetObject( nil );
	//}
}

void EdObjectInspector::OnProjectUnloaded()
{
	//this->Property_Editor_Dock_Widget::SetObject( nil );
}

