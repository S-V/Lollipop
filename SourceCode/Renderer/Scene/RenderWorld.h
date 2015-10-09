/*
=============================================================================
	File:	RenderWorld.h
	Desc:	Graphics scene manager.
=============================================================================
*/
#pragma once

#include <Renderer/Common.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/Scene/Model.h>
#include <Renderer/Scene/SkyModel.h>
#include <Renderer/Scene/SphereTree.h>

// hardcoded limits

enum { MAX_RENDER_ENTITIES = 4096*4 };	// 16 K in each world

/*
-----------------------------------------------------------------------------
	rxRenderWorld
-----------------------------------------------------------------------------
*/
class rxRenderWorld : public SBaseType
{
public:
	// bounds of model instances for frustum culling
	//TList< rxAABB >		m_bounds;

	// graphics models for rendering
	TList< rxModel >	m_models;

	// dynamic light sources
	TList< rxParallelLight >	m_dirLights;
	TList< rxLocalLight >		m_localLights;

	// generic graphics entities
	TList< rxRenderEntity* >	m_entities;	//+noserialize

	// sky
	rxSkyModel		m_skyModel;

public:
	mxDECLARE_CLASS(rxRenderWorld,SBaseType);
	mxDECLARE_REFLECTION;

	rxRenderWorld();
	~rxRenderWorld();

	void ReserveNumEntities( UINT count );
	void RegisterEntity( rxRenderEntity* newRenderEntity );
	void UnregisterEntity( rxRenderEntity* theRenderEntity );

	UINT GetNumEntities();

	rxRenderEntity* GetEntity( UINT index ) { return m_entities[index]; }

	void Clear();

	void rfBuildDrawList( const rxSceneContext& sceneContext, rxRenderQueue & q );

	// compute shadow casters for a directional light
	void rfGetDirLightShadowCasters( rxShadowCastingSet & shadowCasters );

	// compute shadow casters for a spot light
	void rfGetSpotLightShadowCasters(
		const rxViewFrustum& lightFrustum,
		rxShadowCastingSet &shadowCasters
		);

	//virtual void ForEachEntity( F_EntityIterator* pIterator, void* pUserData ) override;

	rxModel& CreateModel();
	rxParallelLight& CreateDirectionalLight();
	rxLocalLight& CreateLocalLight();

	void DebugDraw( const rxViewport* viewport, const rxSceneContext& sceneContext );

	rxSkyModel & GetSky() { return m_skyModel; }

public:	// Editor

#if MX_EDITOR
	friend class Graphics_Models_List;
	friend class Graphics_Model_Editor;
	friend class Local_Light_Editor;
	friend class Scene_Editor_Local_Lights;
	friend class Dir_Light_Editor;
	friend class Scene_Editor_Global_Lights;
#endif // MX_EDITOR

	NO_COPY_CONSTRUCTOR(rxRenderWorld);
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
