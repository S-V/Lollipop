// code for scene editing
#include "stdafx.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>

#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#include <EditorSupport/AssetPipeline/MeshConverter.h>

#include "main_window_frame.h"
#include "app.h"

#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	World_Entities_List
-----------------------------------------------------------------------------
*/
World_Entities_List::World_Entities_List( World_Editor* theParent, World* theWorld )
	: m_parent( theParent )
	, m_world( theWorld )
{
}

World_Entities_List::~World_Entities_List()
{
}

void World_Entities_List::PostLoad()
{
	// create editor objects for world entities

	const UINT numEnts = m_world->m_entities.Num();
	for( UINT iEntity = 0; iEntity < numEnts; iEntity++ )
	{
		AEntity* pEntity = m_world->m_entities[ iEntity ];

		Unimplemented;
		//F_Create_Editor_For_Entity( this, pEntity );
	}
}

void World_Entities_List::Add_Entity( AEntity* newEntity )
{
	CHK_VRET_IF_NIL( newEntity );

	Unimplemented;
	//F_Create_Editor_For_Entity( this, newEntity );

	m_world->AddEntity( newEntity );
}

void World_Entities_List::Remove( AEntityEditor* o )
{
	AEntity* pEntity = o->GetEntity();

	Super::Remove( o );

	m_world->DeleteEntity( pEntity );
}

AEditable* World_Entities_List::edGetParent()
{
	return m_parent;
}

const char* World_Entities_List::edGetName() const
{
	return "Entities";
}

void World_Entities_List::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	//Super::edShowContextMenu(args);
}

/*
-----------------------------------------------------------------------------
	World_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(World_Editor);

World_Editor::World_Editor( Global_Level_Editor* theParent, World* theWorld )
	: m_parent( theParent )
	, m_world( theWorld )
	, m_graphicsScene( this, theWorld )
	, m_entities( this, theWorld )
{
	Assert( m_world->m_editor.IsNull() );
	m_world->m_editor = this;

	theParent->Add( this );
}

World_Editor::~World_Editor()
{
	DBG_TRACE_CALL;
	if( m_world != nil ) {
		m_world->m_editor = nil;
	}
}

void World_Editor::PostLoad()
{
	m_entities.PostLoad();
}


void World_Editor::Add_Resource_From_File( const char* filePath )
{
	DBGOUT("World_Editor::AddResourceFromFile: %s\n", filePath);


	DevAssetManager* assetDb = Editor::GetAssetDb();


	const AssetInfo* pAssetInfo = assetDb->Get_Asset_Info_By_Path( filePath );
	CHK_VRET_IF_NIL(pAssetInfo);


	Editor::AssetData::Ref pSrcAssetData = assetDb->LoadAssetData( *pAssetInfo );
	VRET_IF_NIL(pSrcAssetData);

	//if( pAssetInfo->assetType == EResourceType::Asset_StaticMesh )
	//{
	//	//
	//}



	//VRET_IF_NOT(F_Try_To_Load_Static_Model( assetData ));
	Editor::StaticTriangleMeshData* meshData = SafeCast<Editor::StaticTriangleMeshData>( pSrcAssetData );
	if( meshData != nil )
	{
		UNDONE;
		//StaticModelEntity* newStaticModel = new StaticModelEntity();

		//F_Load_Static_Model( *meshData, newStaticModel );

		//m_entities.Add_Entity( newStaticModel );
	}



	mxWarnf("Failed to load asset '%s' - unknown format!\n",
		pAssetInfo->srcFileName.ToChars());
}

AEditable* World_Editor::edGetParent()
{
	return m_parent;
}

void World_Editor::edRemoveSelf()
{
	m_parent->RemoveWorld( this );
}

bool World_Editor::edSetName( const char* newName )
{
	AssertPtr(newName);
	VRET_X_IF_NIL(newName,false);

	m_world->m_name = newName;

	return true;
}

const char* World_Editor::edGetName() const
{
	return m_world->m_name.ToChars();
}

void World_Editor::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	QMenu	menu;
	{
		//
	}
	menu.exec( QPoint( args.posX, args.posY ) );
}

World_Editor* World_Editor::GetPointerByIndex( UINT objectIndex )
{
	return Global_Level_Editor::Get().SafeGetItemRef( objectIndex );
}
UINT World_Editor::GetIndexByPointer( const World_Editor* thePointer )
{
	return Global_Level_Editor::Get().edIndexOf( thePointer );
}

/*
-----------------------------------------------------------------------------
	Global_Level_Editor
-----------------------------------------------------------------------------
*/
Global_Level_Editor::Global_Level_Editor()
{
}

Global_Level_Editor::~Global_Level_Editor()
{
	this->Clear();
}

World* Global_Level_Editor::CreateNewWorld()
{
	World *	newWorld = new World();
	m_levelData.m_worlds.Add( newWorld );

	World_Editor* pWorldEditor = new World_Editor( this, newWorld );
	(void)pWorldEditor;

	return newWorld;
}

void Global_Level_Editor::RemoveWorld( World_Editor* pWorldEditor )
{
	AssertPtr( pWorldEditor );

	World *	theWorld = pWorldEditor->m_world;
	AssertPtr( theWorld );

	this->Remove( pWorldEditor );

	m_levelData.m_worlds.Remove( theWorld );
	delete theWorld;
}

void Global_Level_Editor::Clear()
{
	//DBG_TRACE_CALL;

	TRefCountedObjectList::Clear();

	m_levelData.m_worlds.DeleteContents();
	m_levelData.m_worlds.Clear();
}

bool Global_Level_Editor::Load( AObjectReader & archive )
{
	archive.LoadObject( m_levelData );

	DBGOUT("Global_Level_Editor::Load: %u world(s)\n", m_levelData.m_worlds.Num());

	for( UINT iWorld = 0; iWorld < m_levelData.m_worlds.Num(); iWorld++ )
	{
		World *	theWorld = m_levelData.m_worlds[ iWorld ];
		AssertPtr(theWorld);

		MX_DBG_CHECK_VTBL( theWorld );

		DBGOUT("Loading world: '%s'\n", theWorld->m_name.ToChars());

		World_Editor* pWorldEditor = new World_Editor( this, theWorld );
		pWorldEditor->PostLoad();
	}

	return false;
}

bool Global_Level_Editor::Save( AObjectWriter & archive )
{
	DBGOUT("Global_Level_Editor::Save: %u world(s)\n", m_levelData.m_worlds.Num());

	m_levelData.m_formatVersion.v = 1;
	m_levelData.m_timestamp = gCore.currTimeStamp;

	archive.SaveObject( m_levelData );

	return false;
}

UINT Global_Level_Editor::NumWorlds() const
{
	return m_levelData.m_worlds.Num();
}

World* Global_Level_Editor::WorldAt( UINT index )
{
	const UINT numWorlds = this->NumWorlds();
	if( index < numWorlds ) {
		return m_levelData.m_worlds[ index ];
	}
	return nil;
}

void Global_Level_Editor::OnProjectLoaded()
{

}

void Global_Level_Editor::OnProjectUnloaded()
{
	this->Clear();
}

AEditable* Global_Level_Editor::edGetParent()
{
	return nil;
}

const char* Global_Level_Editor::edGetName() const
{
	return "World_Manager";
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
