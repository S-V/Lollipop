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

	void Add_Entity( AEntity* newEntity );

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
	TValidPtr< Global_Level_Editor >	m_parent;
	TValidPtr< World >		m_world;

	Graphics_Scene_Editor	m_graphicsScene;

	World_Entities_List		m_entities;

public:
	mxDECLARE_CLASS(World_Editor,AObjectEditor);

	World_Editor( Global_Level_Editor* theParent, World* theWorld );
	~World_Editor();

	// should be called after deserialization
	virtual void PostLoad() override;

	void Add_Resource_From_File( const char* filePath );

	mxIMPLEMENT_ED_GROUP_2( m_graphicsScene, m_entities );

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
	Global_Level_Editor
-----------------------------------------------------------------------------
*/
class Global_Level_Editor
	: public AWorldManager
	, public TRefCountedObjectList<World_Editor>
	, public EdProjectChild
	, public TGlobal<Global_Level_Editor>
	, SingleInstance<Global_Level_Editor>
{
	LevelData		m_levelData;

public:
	Global_Level_Editor();
	~Global_Level_Editor();

	World* CreateNewWorld();
	//void AddNewWorld( World* newWorld );
	void RemoveWorld( World_Editor* pWorldEditor );

public:	//==- TRefCountedObjectList
	virtual void Clear() override;

public:	//=-- AWorldManager
	virtual bool Load( AObjectReader & archive ) override;
	virtual bool Save( AObjectWriter & archive ) override;

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
