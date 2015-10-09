//@todo: refactor
//
#include "stdafx.h"

#include "main_window_frame.h"
#include "editor_app.h"
#include "output_log.h"
#include "project_manager.h"
#include "resource_browser.h"
#include "scene_manager.h"
#include "property_editor.h"
#include "property_editor_module2.h"
#include "tweakable_vars_editor.h"
#include "shader_editor.h"
#include "texture_inspector.h"
#include "engine_stats.h"

#include "gizmo/manipulator.h"
#include "widgets/object_inspector.h"
#include "render/gfx_engine.h"

/*
-----------------------------------------------------------------------------
	EdModule
-----------------------------------------------------------------------------
*/
EdModule::EdModule()
{
}

EdModule::~EdModule()
{

}

/*
-----------------------------------------------------------------------------
	EdSystem
-----------------------------------------------------------------------------
*/
EdSystem::EdSystem()
{
	//Assert( !CoreSubsystemIsOpen() );
	Assert( gCore.editor == nil || gCore.editor == AEditorInterface::GetDummyInstance() );
	gCore.editor = this;
}

EdSystem::~EdSystem()
{
	//mxDBG_TRACE_CALL;

	this->DestroyModules();

	Assert( gCore.editor == this );
	gCore.editor = AEditorInterface::GetDummyInstance();
}


//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define DO_FOR_EACH_MODULE( WHAT )\
	for( UINT iModule = 0; iModule < m_modules.Num(); iModule++ )\
	{\
		EdModule* pModule = m_modules[ iModule ];\
		pModule->WHAT;\
	}
//!--------------------------------------------------------------------------

void EdSystem::PreInit()
{
	DO_FOR_EACH_MODULE(PreInit());
}

void EdSystem::SetDefaultValues()
{
	DO_FOR_EACH_MODULE(SetDefaultValues());
}

void EdSystem::PostInit()
{
	DO_FOR_EACH_MODULE(PostInit());
}

void EdSystem::ShutdownModules()
{
	DO_FOR_EACH_MODULE(Shutdown());
}

void EdSystem::InstallDefaultPlugins()
{
	this->RegisterPlugin(new EdOutputLog());
	this->RegisterPlugin(new TweakableVarsEditor());


	//this->RegisterPlugin(new EdObjectInspector());


	this->RegisterPlugin(new PropertyEditorPlugin());

	mxUNDONE
	//this->RegisterPlugin(new PropertyEditorPlugin2());

	this->RegisterPlugin(new EdModule_ShaderEditor());


	this->RegisterPlugin(new EdProjectManager());
	this->RegisterPlugin(new EdResourceBrowserModule());


	this->RegisterPlugin(new EdSceneManager());
	this->RegisterPlugin(new EdRenderer());

	this->RegisterPlugin(new EdManipulator());

	this->RegisterPlugin(new EdDebugger());

	this->RegisterPlugin(new EdStatsView());
}

void EdSystem::DestroyModules()
{
	for( INT i = m_modules.Num() - 1; i >= 0; i-- )
	{
		m_modules[i] = nil;
	}
	m_modules.Empty();
}

void EdSystem::RegisterPlugin( EdModule* newModule )
{
	AssertPtr(newModule);
	m_modules.AddUnique(newModule);
}

void EdSystem::slot_OnProjectUnloaded()
{
	m_selectedObject = nil;
}

// should be called immediately after a new object has been created
void EdSystem::Notify_ObjectCreated( AEditable* newObject )
{
	AssertPtr( newObject );

	this->Event_ObjectCreated( newObject );
}

// NOTE: the passed argument can be null if deselected
void EdSystem::Notify_SelectionChanged( AEditable* theObject )
{
	if( m_selectedObject == theObject ) {
		return;
	}

	m_selectedObject = theObject;

	this->Event_SelectionChanged( m_selectedObject );

	if( m_selectedObject != nil ) {
		DBGOUT("Selected object: '%s'\n",m_selectedObject->edGetName());
	} else {
		DBGOUT("Deselected object\n");
	}
}

// returns the currently selected object;
// may return null
AEditable* EdSystem::GetSelectedObject()
{
	return m_selectedObject;
}

void EdSystem::Notify_ObjectModified( AEditable* theObject )
{
	AssertPtr( theObject );

	this->Event_ObjectModified( theObject );
}

// should be called before the object is destroyed
void EdSystem::Notify_ObjectBeingDestroyed( AEditable* theObject )
{
	AssertPtr( theObject );

	//this->PatchReferencesTo( theObject );
	//this->UnregisterObject( theObject );

	if( m_selectedObject == theObject ) {
		m_selectedObject = nil;
	}
	this->Event_ObjectBeingDestroyed( theObject );
}

// Begins a row insertion operation.
// You must call this function before inserting data into the model's underlying data store.
//
void EdSystem::Notify_BeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows )
{
	AssertPtr( parent );
	this->Event_BeginInsertRows( parent, iFirstRow, iNumRows );
}

// Ends a row insertion operation.
// You must call this function after inserting data from the model's underlying data store.
//
void EdSystem::Notify_EndInsertRows( AEditable* parent )
{
	AssertPtr( parent );
	this->Event_EndInsertRows( parent );
}

// Begins a row removal operation.
// You must call this function before removing data from the model's underlying data store.
//
void EdSystem::Notify_BeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows )
{
	AssertPtr( parent );
	this->Event_BeginRemoveRows( parent, iFirstRow, iNumRows );
}

// Ends a row removal operation.
// You must call this function after removing data from the model's underlying data store.
//
void EdSystem::Notify_EndRemoveRows( AEditable* parent )
{
	AssertPtr( parent );
	this->Event_EndRemoveRows( parent );
}

// Begins a model reset operation.
// When a model is reset it means that any previous data reported from the model is now invalid and has to be queried for again.
// This also means that the current item and any selected items will become invalid.
// You must call this function before resetting any internal data structures in your model or proxy model.
//
void EdSystem::Notify_BeginResetModel( AEditable* parent )
{
	AssertPtr( parent );
	this->Event_BeginResetModel( parent );
}

// Completes a model reset operation.
// You must call this function after resetting any internal data structure in your model or proxy model.
//
void EdSystem::Notify_EndResetModel( AEditable* parent )
{
	AssertPtr( parent );
	this->Event_EndResetModel( parent );
}

bool EdSystem::Show_YesNo_Warning( const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	const int ret = QMessageBox::warning(nil, TR(ED_APPLICATION_NAME),
		buffer,
		QMessageBox::Yes | QMessageBox::No
	);

	return ret == QMessageBox::Yes;
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
