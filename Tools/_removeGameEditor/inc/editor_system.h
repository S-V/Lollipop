// modules/plugins system
//
#pragma once

#include <Core/Object.h>

#include "editor_common.h"
#include "property_grid.h"

class rxSceneContext;
class EdDrawContext;
class ClientViewport;
class EdSceneViewport;

/*
-----------------------------------------------------------------------------
	EdModule
-----------------------------------------------------------------------------
*/
class EdModule
	: public ReferenceCountedX
	, public EdSystemChild
{
public:
	MX_DECLARE_CLASS_ALLOCATOR(HeapGeneric,EdModule);

	typedef TRefPtr< EdModule >	Ref;

	EdModule();
	~EdModule();

	// initialize the GUI stuff:
	// create widgets, menus,
	// connect signals & slots, etc.
	//
	virtual void PreInit() {}

	// initializes the module with default values
	// if the file with app settings couldn't be loaded.
	//
	virtual void SetDefaultValues() {}

	// called after deserialization (or SetDefaultValues())
	virtual void PostInit() {}

	virtual void Shutdown() {}

	// components that listen for viewport events should return non-null
	//virtual EdViewportClient* IsViewportClient() { return nil; }

	virtual const char* GetModuleName() const
	{
		return "Module";
	}
};

/*
-----------------------------------------------------------------------------
	EdSystem

	conveniently puts many global editor variables into one place;
	also serves as the main communication hub for editor subsystems
-----------------------------------------------------------------------------
*/
class EdSystem
	: public AEditorInterface
	, public TGlobal< EdSystem >
	, SingleInstance< EdSystem >
{
public:	// Global variables

	EdGlobalSettings	settings;	// managed externally, by the app

	QFileIconProvider	defaultFileIconProvider;
	EdPropertySystem	propertySystem;

public:	// Common callbacks

	// called after all modules had been initialized
	TEvent0<>		Event_AppInitialized;

	TEvent0<>	Event_ProjectLoaded;
	TEvent0<>	Event_ProjectUnloaded;

public:
	// Misc events

	TEvent1
	<
		const char*	// path to source assets - can be nil
	>
	Event_AssetDbChanged;


public:	// Serialization

	// called when loading/saving app settings
	TEvent1< ATextSerializer& >		Event_SerializeAppData;
	TEvent1< QDataStream& >			Event_SerializeLayout;

	// serialize project-related data
	TEvent1< ATextSerializer& >		Event_SerializeProjectData;

	// called when the level has completed loading
	TEvent0<>						Event_EngineLoaded;

public:	// Timing

	// real-time updates
	TEvent1< const mxDeltaTime& >	Event_UpdateState;

	TEvent2< const EdSceneViewport&, FLOAT /*deltaSeconds*/ >	Event_TickViewport;

public:	// Object editing

	TEvent1< AEditable* >	Event_ObjectCreated;

	// Object selection

	// the passed argument can be null if deselected
	TEvent1< AEditable* >	Event_SelectionChanged;


	// This signal is emitted when items are changed in the model.
	// The changed items are those from topLeft to bottomRight inclusive.
	// If just one item is changed topLeft == bottomRight.
	//
	//TEvent2< Editable* /*topLeft*/, Editable* /*bottomRight*/ >		OnObjectModified;
	TEvent1< AEditable* >	Event_ObjectModified;


	TEvent1< AEditable* >	Event_ObjectBeingDestroyed;


public:	// Insertion/Removal operations on trees.


	// Begins a row insertion operation.
	// You must call this function before inserting data into the model's underlying data store.
	TEvent3
	<
		AEditable* /*parent*/, UINT /*iFirstRow*/, UINT /*iNumRows*/
	>
	Event_BeginInsertRows;

	// Ends a row insertion operation.
	// You must call this function after inserting data from the model's underlying data store.
	TEvent1
	<
		AEditable* /*parent*/
	>
	Event_EndInsertRows;



	// Begins a row removal operation.
	// You must call this function before removing data from the model's underlying data store.
	TEvent3
	<
		AEditable* /*parent*/, UINT /*iFirstRow*/, UINT /*iNumRows*/
	>
	Event_BeginRemoveRows;

	// Ends a row removal operation.
	// You must call this function after removing data from the model's underlying data store.
	TEvent1
	<
		AEditable* /*parent*/
	>
	Event_EndRemoveRows;


	// Begins a model reset operation.
	// When a model is reset it means that any previous data reported from the model is now invalid and has to be queried for again.
	// This also means that the current item and any selected items will become invalid.
	// You must call this function before resetting any internal data structures in your model or proxy model.
	TEvent1
	<
		AEditable* /*parent*/
	>
	Event_BeginResetModel;

	// Completes a model reset operation.
	// You must call this function after resetting any internal data structure in your model or proxy model.
	TEvent1
	<
		AEditable* /*parent*/
	>
	Event_EndResetModel;



public:	// Viewports and rendering


	TEvent1< const EdDrawContext& >	Event_DrawBackground;
	TEvent1< const EdDrawContext& >	Event_Draw;
	TEvent1< const EdDrawContext& >	Event_DrawForeground;

	TEvent1< const EdDrawContext& >	Event_DrawBackgroundHitProxy;
	TEvent1< const EdDrawContext& >	Event_DrawHitProxy;
	TEvent1< const EdDrawContext& >	Event_DrawForegroundHitProxy;


	//TEvent1< EdViewport* >	OnViewportActivated;

	// params are guaranteed to be valid
	//TEvent2< const rxSceneContext&, const ClientViewport& >	OnViewportRender;
 

	// called by the main app to render all viewports
	TEvent0<>		Event_RenderViewports;


public:
	// EdViewportClient interface
	// NOTE: input is captured in the currently active viewport

	TEvent2< const EdSceneViewport& /*viewport*/,
		const EKeyCode /*key*/
	>
	Event_KeyPressed;



	TEvent2< const EdSceneViewport& /*viewport*/,
		const EKeyCode /*key*/
	>
	Event_KeyReleased;



	TEvent5< const EdSceneViewport& /*viewport*/,
		const int /*x*/, const int /*y*/, const EMouseButton /*btn*/, const bool /*pressed*/
	>
	Event_MouseButton;



	TEvent5< const EdSceneViewport& /*viewport*/,
		const int /*mouseDeltaX*/, const int /*mouseDeltaY*/, const int /*mouseX*/, const int /*mouseY*/
	> 
	Event_MouseMove;



	TEvent2< const EdSceneViewport& /*viewport*/,
		const int /*scroll*/
	> Event_MouseWheel;



public:	//=-- AEditorInterface

	// should be called immediately after a new object has been created
	virtual void Notify_ObjectCreated( AEditable* newObject ) override;


	// NOTE: the passed argument can be null if deselected
	virtual void Notify_SelectionChanged( AEditable* theObject ) override;

	// returns the currently selected object;
	// may return null
	virtual AEditable* GetSelectedObject() override;


	virtual void Notify_ObjectModified( AEditable* theObject ) override;

	// should be called before the object is destroyed
	virtual void Notify_ObjectBeingDestroyed( AEditable* theObject ) override;





	// Begins a row insertion operation.
	// You must call this function before inserting data into the model's underlying data store.
	//
	virtual void Notify_BeginInsertRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) override;


	// Ends a row insertion operation.
	// You must call this function after inserting data from the model's underlying data store.
	//
	virtual void Notify_EndInsertRows( AEditable* parent ) override;


	// Begins a row removal operation.
	// You must call this function before removing data from the model's underlying data store.
	//
	virtual void Notify_BeginRemoveRows( AEditable* parent, UINT iFirstRow, UINT iNumRows ) override;

	// Ends a row removal operation.
	// You must call this function after removing data from the model's underlying data store.
	//
	virtual void Notify_EndRemoveRows( AEditable* parent ) override;

	// Begins a model reset operation.
	// When a model is reset it means that any previous data reported from the model is now invalid and has to be queried for again.
	// This also means that the current item and any selected items will become invalid.
	// You must call this function before resetting any internal data structures in your model or proxy model.
	//
	virtual void Notify_BeginResetModel( AEditable* parent ) override;


	// Completes a model reset operation.
	// You must call this function after resetting any internal data structure in your model or proxy model.
	//
	virtual void Notify_EndResetModel( AEditable* parent ) override;


	virtual bool Show_YesNo_Warning( const char* fmt, ... ) override;




public_internal:
	EdSystem();
	~EdSystem();

	void InstallDefaultPlugins();
	void DestroyModules();

	void RegisterPlugin( EdModule* newModule );

public_internal:

	void PreInit();
	void SetDefaultValues();
	void PostInit();

	void ShutdownModules();

private:
	void slot_OnProjectUnloaded();

private:
	// all registered plugins
	TList< EdModule::Ref >	m_modules;

	// cached pointer to currently selected object
	TPtr< AEditable >		m_selectedObject;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
