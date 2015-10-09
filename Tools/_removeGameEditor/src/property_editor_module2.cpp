#include "stdafx.h"

#include "app.h"
#include "property_editor_module2.h"

/*
-----------------------------------------------------------------------------
	PropertyEditorPlugin2
-----------------------------------------------------------------------------
*/
PropertyEditorPlugin2::PropertyEditorPlugin2()
	: QtSupport::Property_Editor_Dock_Widget( EditorApp::GetMainFrame() )
{
	this->setWindowTitle("Property Editor2");
	this->setObjectName("Property Editor2");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
}

PropertyEditorPlugin2::~PropertyEditorPlugin2()
{
}

void PropertyEditorPlugin2::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void PropertyEditorPlugin2::CreateWidgets()
{
}

void PropertyEditorPlugin2::CreateActions()
{
	m_actViewProperties.setText( tr("Properties Window2") );
}

void PropertyEditorPlugin2::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_actViewProperties );
}

void PropertyEditorPlugin2::ConnectSigSlots()
{
	EditorApp::ShowOnActionTriggered( &m_actViewProperties, this );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_actViewProperties );
}

void PropertyEditorPlugin2::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::RightDockWidgetArea, this );
}

void PropertyEditorPlugin2::Shutdown()
{
	this->Property_Editor_Dock_Widget::SetObject( nil );
}

void PropertyEditorPlugin2::SerializeWidgetLayout( QDataStream & stream )
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::RightDockWidgetArea, this );
}

void PropertyEditorPlugin2::OnSelectionChanged( AEditable* theObject )
{
	this->Property_Editor_Dock_Widget::SetObject( theObject );
}

void PropertyEditorPlugin2::OnObjectDestroyed( AEditable* theObject )
{
	if( this->Property_Editor_Dock_Widget::GetObject() == theObject )
	{
		this->Property_Editor_Dock_Widget::SetObject( nil );
	}
}

void PropertyEditorPlugin2::OnProjectUnloaded()
{
	this->Property_Editor_Dock_Widget::SetObject( nil );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
