#include "stdafx.h"

//#include <Engine/rxRenderEntity.h>

#include "main_window_frame.h"
#include "editor_app.h"

#include "property_editor.h"

/*
-----------------------------------------------------------------------------
	PropertyEditorPlugin
-----------------------------------------------------------------------------
*/
PropertyEditorPlugin::PropertyEditorPlugin()
	: ProperyEditorDockWidget(EditorApp::GetMainFrame())
{
	this->setWindowTitle("Property Editor");
	this->setObjectName("Property Editor");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );
}

PropertyEditorPlugin::~PropertyEditorPlugin()
{
}

void PropertyEditorPlugin::PreInit()
{
	this->CreateWidgets();
	this->CreateActions();
	this->CreateMenus();
	this->ConnectSigSlots();
}

void PropertyEditorPlugin::CreateWidgets()
{
}

void PropertyEditorPlugin::CreateActions()
{
	m_actViewProperties.setText( tr("Properties Window") );
}

void PropertyEditorPlugin::CreateMenus()
{
	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_actViewProperties );
}

void PropertyEditorPlugin::ConnectSigSlots()
{
	EditorApp::ShowOnActionTriggered( &m_actViewProperties, this );
	EditorApp::ValidOnlyWhenProjectIsLoaded( &m_actViewProperties );

	mxCONNECT_THIS( EdSystem::Get().Event_SelectionChanged, ProperyEditorDockWidget, SetObject );
}

void PropertyEditorPlugin::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::RightDockWidgetArea, this );
}

void PropertyEditorPlugin::Shutdown()
{
	////DBG_TRACE_CALL;
	this->SetObject( nil );
}

void PropertyEditorPlugin::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::RightDockWidgetArea, this );
}

void PropertyEditorPlugin::OnProjectUnloaded()
{
	this->SetObject( nil );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
