#pragma once

#include <Renderer/Scene/RenderWorld.h>

#include <Engine/Worlds.h>

#include "editor_common.h"

#include "basic_entity_editor.h"
#include "graphics_world_editor.h"

/*
-----------------------------------------------------------------------------
	World_Entities_List
-----------------------------------------------------------------------------
*/
class World_Entities_List : public TRefCountedObjectList<AEntityEditor>
{
	TValidPtr< World_Editor >	m_parent;
	TValidPtr< World >			m_world;

public:
	typedef TRefCountedObjectList Super;

	World_Entities_List( World_Editor* theParent, World* theWorld );
	~World_Entities_List();

	virtual void PostLoad() override;

	virtual void Add( AEntityEditor* o ) override;
	virtual void Remove( AEntityEditor* o ) override;

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;
};

/*
-----------------------------------------------------------------------------
	World_Editor
-----------------------------------------------------------------------------
*/
struct World_Editor : public AObjectEditor
{
	TValidPtr< Universe_Editor >	m_parent;
	TValidPtr< World >				m_world;

	Graphics_Scene_Editor	m_sceneEditor;
	World_Entities_List		m_entitiesEditor;

public:
	mxDECLARE_CLASS(World_Editor,AObjectEditor);

	// automatically adds itself to the parent
	World_Editor( Universe_Editor* theParent, World* theWorld );
	~World_Editor();

	// should be called after deserialization
	virtual void PostLoad() override;

	bool Accept_Dropped_File( const EdSceneViewport& viewport, const char* filePath, const UINT mouseX, const UINT mouseY );

	void Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );


	mxIMPLEMENT_ED_GROUP_2( m_sceneEditor, m_entitiesEditor );

	virtual AEditable* edGetParent() override;
	virtual void edRemoveSelf() override;
	virtual bool edCanBeRenamed() const { return true; }
	virtual bool edSetName( const char* newName ) override;
	virtual const char* edGetName() const override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

	static World_Editor* GetPointerByIndex( UINT objectIndex );
	static UINT GetIndexByPointer( const World_Editor* thePointer );
};

/*
-----------------------------------------------------------------------------
	Universe_Editor
-----------------------------------------------------------------------------
*/
class Universe_Editor
	: public AEngineClient
	, public TRefCountedObjectList<World_Editor>
	, public EdProjectChild
	, public TGlobal<Universe_Editor>
	, SingleInstance<Universe_Editor>
{
	LevelData		m_levelData;

public:
	mxDECLARE_REFLECTION;

	Universe_Editor();
	~Universe_Editor();

	World* CreateNewWorld();
	//void AddNewWorld( World* newWorld );
	void RemoveWorld( World_Editor* pWorldEditor );

public:	//==- TRefCountedObjectList
	virtual void Clear() override;

public:	//=-- AEngineClient
	virtual bool LoadState( AObjectReader & archive ) override;
	virtual bool SaveState( AObjectWriter & archive ) override;

	virtual UINT NumWorlds() const override;
	virtual World* WorldAt( UINT index ) override;

protected:	//=-- EdProjectChild
	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;

public:	//=== AEditable
	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
