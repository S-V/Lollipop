/*
=============================================================================
	File:	Worlds.cpp
	Desc:	scene management
=============================================================================
*/

#include <Engine_PCH.h>
#pragma hdrstop
#include <Engine.h>

#include <Core/Editor/EditableProperties.h>
#include <Core/Entity/System.h>

#include <Renderer/Core/Geometry.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/Scene/RenderWorld.h>

#include <Engine/Engine.h>
#include <Engine/Entity.h>
#include <Engine/Worlds.h>

mxNAMESPACE_BEGIN

bool	g_cvar_tick_physics_world = true;

/*
-----------------------------------------------------------------------------
	EdLevelInformation
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( EdLevelInformation )
mxBEGIN_REFLECTION( EdLevelInformation )
	mxMEMBER_FIELD2( name, Level_Name )
	mxMEMBER_FIELD2( author, Level_Author )
	mxMEMBER_FIELD2( description, Level_Description )
	mxMEMBER_FIELD2( version, Level_Version )
mxEND_REFLECTION

/*
-----------------------------------------------------------------------------
	SWorldEditInfo
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( SWorldEditInfo )
mxBEGIN_REFLECTION( SWorldEditInfo )
	mxMEMBER_FIELD2( camera, Camera_Params )
mxEND_REFLECTION

/*
-----------------------------------------------------------------------------
	World
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(World);

mxBEGIN_REFLECTION(World)
	mxMEMBER_FIELD2( m_renderWorld, Graphics_Scene )
	mxMEMBER_FIELD2( m_entities, Entities_List )
	mxMEMBER_FIELD2( m_editorInfo, Editor_Params )
	mxMEMBER_FIELD2( m_name, Name_In_Editor )
mxEND_REFLECTION

World::World()
{
	m_name = "World";
}

World::~World()
{
	//mxDBG_TRACE_CALL;
	this->Clear();
}

//!=- MACRO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define DO_FOR_EACH_ENTITY( WHAT )\
	{\
		AEntity** ptrEntities = m_entities.ToPtr();\
		const UINT numEntities = m_entities.Num();\
		for( UINT iEntity = 0; iEntity < numEntities; iEntity++ )\
		{\
			AEntity* pEntity = ptrEntities[ iEntity ];\
			WHAT;\
		}\
	}
//!--------------------------------------------------------------------------

void World::Clear()
{
	m_entities.DeleteContents();
	m_entities.Clear();

	m_renderWorld.Clear();
}

void World::AddEntity( AEntity* newEntity )
{
	AssertPtr(newEntity);
	//Assert(!newEntity->m_containingWorld);
	Assert(!m_entities.Contains(newEntity));

	SEntitySpawnContext		entitySpawnContext;
	entitySpawnContext.world = this;

	newEntity->Initialize(entitySpawnContext);

	m_entities.Add(newEntity);

	this->Register_Entity_Components( newEntity );
}

void World::DeleteEntity( AEntity* theEntity )
{
	AssertPtr(theEntity);
	//Assert(theEntity->m_containingWorld==this);
	Assert(m_entities.Num() > 0);

	this->Unregister_Entity_Components( theEntity );

	m_entities.Remove( theEntity );

	theEntity->Shutdown();

	delete theEntity;
}

void World::Register_Entity_Components( AEntity* newEntity )
{
	rxRenderEntity* pRenderEntity = newEntity->GetVisuals();
	if(PtrToBool( pRenderEntity ))
	{
		m_renderWorld.RegisterEntity(pRenderEntity);
	}
}

void World::Unregister_Entity_Components( AEntity* theEntity )
{
	rxRenderEntity* pRenderEntity = theEntity->GetVisuals();
	if(PtrToBool( pRenderEntity ))
	{
		m_renderWorld.UnregisterEntity(pRenderEntity);
	}
}

void World::RunPhysics( FLOAT deltaSeconds )
{
	//if( g_cvar_tick_physics_world )
	//{
	//	const F4 timeMultiplier = 1.0f;
	//	const F4 deltaTime = deltaSeconds * timeMultiplier;

	//	const F4 fixedStepSize = 1.0f/60.0f;//PX_DEFAULT_FIXED_STEP_SIZE;
	//	const U4 numSubsteps = 100;

	//	this->GetPhysicsWorld()->Tick( deltaTime, fixedStepSize, numSubsteps );
	//}
}

void World::UpdateEntities( FLOAT deltaSeconds )
{
	//AEntity** entitiesArray = m_entities.ToPtr();
	//const UINT numEntities = m_entities.Num();

	//SEntityUpdateContext	updateContext;

	//for( UINT iEntity = 0; iEntity < numEntities; iEntity++ )
	//{
	//	AEntity* pEntity = entitiesArray[ iEntity ];

	//	updateContext.deltaSeconds = deltaSeconds;

	//	pEntity->Tick( updateContext );
	//}
}

void World::Optimize()
{
	// TODO: sort entity list by object id
	// to 'group' identical objects together
	// (function calls, instruction cache)
}

void World::PostLoad()
{
	Super::PostLoad();

	DO_FOR_EACH_ENTITY( pEntity->PostLoad() );

	m_renderWorld.ReserveNumEntities( m_entities.Num() );

	DO_FOR_EACH_ENTITY( this->Register_Entity_Components( pEntity ) );
}

/*
-----------------------------------------------------------------------------
	LevelData
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( LevelData )
mxBEGIN_REFLECTION( LevelData )
	mxMEMBER_FIELD2( m_worlds, Worlds_List )

	mxMEMBER_FIELD2( m_engineVersion, Engine_Version )
	mxMEMBER_FIELD2( m_formatVersion, File_Version )
	mxMEMBER_FIELD2( m_timestamp, TimeStamp )

	mxMEMBER_FIELD2( m_levelInfo, Level_Info )
mxEND_REFLECTION

LevelData::LevelData()
{
	mxGetCurrentEngineVersion( m_engineVersion );
	m_formatVersion.v = 0;
	m_timestamp = gCore.currTimeStamp;
}

void LevelData::PostLoad()
{
	const UINT numWorlds = m_worlds.Num();
	for( UINT iWorld = 0; iWorld < numWorlds; iWorld++ )
	{
		World *	pWorld = m_worlds[ iWorld ];
		pWorld->PostLoad();
	}
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
