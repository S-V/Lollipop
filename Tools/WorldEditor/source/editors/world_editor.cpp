// code for scene editing
#include "stdafx.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>

#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#include <EditorSupport/AssetPipeline/MeshConverter.h>

#include "editor_app.h"
#include "main_window_frame.h"
#include "editors/graphics_model_editor.h"
#include "editors/world_editor.h"
#include "render/viewports.h"



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

	const UINT numEntities = m_world->m_entities.Num();

	this->Reserve( numEntities );

	for( UINT iEntity = 0; iEntity < numEntities; iEntity++ )
	{
		AEntity* pEntity = m_world->m_entities[ iEntity ];

		AEntityEditor* pEditor = AEntityEditor::Create_Editor_For_Entity( this, pEntity );

		Super::Add( pEditor );
	}
}

void World_Entities_List::Add( AEntityEditor* o )
{
	AEntity* pEntity = o->GetEntity();

	m_world->AddEntity( pEntity );

	Super::Add( o );
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

World_Editor::World_Editor( Universe_Editor* theParent, World* theWorld )
	: m_parent( theParent )
	, m_world( theWorld )
	, m_sceneEditor( *this, *theWorld )
	, m_entitiesEditor( this, theWorld )
{
	Assert( m_world->m_editor.IsNull() );
	m_world->m_editor = this;

	theParent->Add( this );
}

World_Editor::~World_Editor()
{
	//mxDBG_TRACE_CALL;
	if( m_world != nil ) {
		m_world->m_editor = nil;
	}
}

void World_Editor::PostLoad()
{
	m_sceneEditor.PostLoad();
	m_entitiesEditor.PostLoad();
}

static
void F_Add_Scene_Object_From_Resource( World_Editor& rWorld,
									  const SAssetInfo& rAssetInfo, const AssetData::Ref& pSrcAssetData )
{
	StaticTriangleMeshData* pStaticMeshData = SafeCast<StaticTriangleMeshData>( pSrcAssetData );
	if( pStaticMeshData != nil )
	{
		Graphics_Model_Editor* pModelEditor = Graphics_Model_Editor::Static_Create_From_Mesh( rWorld, rAssetInfo, pStaticMeshData );
		rWorld.m_sceneEditor.m_models.Add( pModelEditor );
		return;
	}

	mxDBG_UNREACHABLE;
}

bool World_Editor::Accept_Dropped_File( const EdSceneViewport& viewport, const char* filePath, const UINT mouseX, const UINT mouseY )
{
	DBGOUT("World_Editor::AddResourceFromFile: %s\n", filePath);

	DevAssetManager* pAssetDb = Editor::GetAssetDb();

	const SAssetInfo* pAssetInfo = pAssetDb->Get_Asset_Info_By_Path( filePath );
	CHK_VRET_FALSE_IF_NIL(pAssetInfo);


	EdAssetInfo		assetInfo( *pAssetInfo );

	{
		AHitProxy *	pHitObject = viewport.GetHitProxyAt( mouseX, mouseY );
		if( pHitObject != nil )
		{
			DBGOUT("DropTarget = %s\n",pHitObject->edGetName());
			if( pHitObject->edOnItemDropped( &assetInfo ) ) {
				return true;
			}
		}
	}


	AssetData::Ref pSrcAssetData = pAssetDb->LoadAssetData( *pAssetInfo );
	CHK_VRET_FALSE_IF_NIL(pSrcAssetData);

	F_Add_Scene_Object_From_Resource( *this, *pAssetInfo, pSrcAssetData );

	//AEntityEditor* pEntityEditor = AEntityEditor::Create_Entity_From_Resource( &m_entities, *pAssetInfo, pSrcAssetData );
	//if( nil == pEntityEditor )
	//{
	//	mxWarnf("Failed to load asset '%s' - unknown format!\n", pAssetInfo->srcFileName.ToChars());
	//	return false;
	//}

	//m_entities.Add( pEntityEditor );

	return true;
}

void World_Editor::Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	m_sceneEditor.Draw_Editor_Stuff( viewport, sceneContext );
}

void World_Editor::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	m_sceneEditor.Draw_Hit_Proxies( viewport, sceneContext );
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
}

World_Editor* World_Editor::GetPointerByIndex( UINT objectIndex )
{
	return Universe_Editor::Get().SafeGetItemRef( objectIndex );
}
UINT World_Editor::GetIndexByPointer( const World_Editor* thePointer )
{
	return Universe_Editor::Get().edIndexOf( thePointer );
}

/*
-----------------------------------------------------------------------------
	Universe_Editor
-----------------------------------------------------------------------------
*/
mxBEGIN_REFLECTION(Universe_Editor)
	mxMEMBER_FIELD2( m_levelData,	Level_Data )
mxEND_REFLECTION;

Universe_Editor::Universe_Editor()
{
}

Universe_Editor::~Universe_Editor()
{
	this->Clear();
}

World* Universe_Editor::CreateNewWorld()
{
	World *	newWorld = new World();
	m_levelData.m_worlds.Add( newWorld );

	{
		rxParallelLight& newDirLight = newWorld->GetRenderWorld().CreateDirectionalLight();
		(void)newDirLight;

		//rxLocalLight& newDynLight = newWorld->GetRenderWorld().CreateLocalLight();
		//newDynLight.SetOrigin(Vec3D::vec3_zero);
		//newDynLight.SetRadius(100);
	}

	World_Editor* pWorldEditor = new World_Editor( this, newWorld );
	(void)pWorldEditor;

	return newWorld;
}

void Universe_Editor::RemoveWorld( World_Editor* pWorldEditor )
{
	AssertPtr( pWorldEditor );

	World *	theWorld = pWorldEditor->m_world;
	AssertPtr( theWorld );

	this->Remove( pWorldEditor );

	m_levelData.m_worlds.Remove( theWorld );
	delete theWorld;
}

void Universe_Editor::Clear()
{
	//DBG_TRACE_CALL;

	TRefCountedObjectList::Clear();

	m_levelData.m_worlds.DeleteContents();
	m_levelData.m_worlds.Clear();
}

bool Universe_Editor::LoadState( AObjectReader & archive )
{
	archive.LoadObject( m_levelData );

	DBGOUT("Universe_Editor::Load: %u world(s)\n", m_levelData.m_worlds.Num());

	for( UINT iWorld = 0; iWorld < m_levelData.m_worlds.Num(); iWorld++ )
	{
		World *	theWorld = m_levelData.m_worlds[ iWorld ];
		AssertPtr(theWorld);

		mxDBG_CHECK_VTBL( theWorld );

		DBGOUT("Loading world: '%s'\n", theWorld->m_name.ToChars());

		World_Editor* pWorldEditor = new World_Editor( this, theWorld );
		pWorldEditor->PostLoad();
	}

	return false;
}

bool Universe_Editor::SaveState( AObjectWriter & archive )
{
	DBGOUT("Universe_Editor::Save: %u world(s)\n", m_levelData.m_worlds.Num());

	m_levelData.m_formatVersion.v = 1;
	m_levelData.m_timestamp = gCore.currTimeStamp;

	archive.SaveObject( m_levelData );

	return false;
}

UINT Universe_Editor::NumWorlds() const
{
	return m_levelData.m_worlds.Num();
}

World* Universe_Editor::WorldAt( UINT index )
{
	const UINT numWorlds = this->NumWorlds();
	if( index < numWorlds ) {
		return m_levelData.m_worlds[ index ];
	}
	return nil;
}

void Universe_Editor::OnProjectLoaded()
{

}

void Universe_Editor::OnProjectUnloaded()
{
	this->Clear();
}

AEditable* Universe_Editor::edGetParent()
{
	return nil;
}

const char* Universe_Editor::edGetName() const
{
	return "World_Manager";
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
