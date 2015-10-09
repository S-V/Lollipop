#include "stdafx.h"

#include <Core/Util/Tweakable.h>

#include "main_window_frame.h"
#include "app.h"

#include "tweakable_vars_editor.h"

/*
-----------------------------------------------------------------------------
	TweakableVarsEditor
-----------------------------------------------------------------------------
*/
TweakableVarsEditor::TweakableVarsEditor()
{
	this->setWindowTitle("Tweakable variables");
	this->setObjectName("Tweakable variables");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );


	//QObject* parent = GetMainFrame();
	m_viewTweakableVarsBar.setText( TR("Tweakable variables") );

	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_viewTweakableVarsBar );

	connect(&m_viewTweakableVarsBar, SIGNAL(triggered()), this, SLOT(show()));


	this->SetObject(gCore.tweaks);

	MX_CONNECT_THIS( EdSystem::Get().Event_ObjectModified, TweakableVarsEditor, OnObjectModified );
}

TweakableVarsEditor::~TweakableVarsEditor()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_ObjectModified );
}

void TweakableVarsEditor::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();
	mainWindow->addDockWidget( Qt::RightDockWidgetArea, this );
	this->hide();
}

void TweakableVarsEditor::PostInit()
{
	
}

void TweakableVarsEditor::Shutdown()
{
	this->SetObject( nil );
}

void TweakableVarsEditor::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::RightDockWidgetArea, this );
}

void TweakableVarsEditor::OnProjectUnloaded()
{
	// nothing
}

void TweakableVarsEditor::OnObjectModified( AEditable* theObject )
{
	if( theObject == gCore.tweaks )
	{
		//PropertyEditorModel* pModel = this->GetModel();
		//if( pModel != nil )
		//{
		//	pModel->Refresh();
		//}

		//HACK: Force refreshing.
		this->SetObject(nil);
		this->SetObject(gCore.tweaks);
	}
}
