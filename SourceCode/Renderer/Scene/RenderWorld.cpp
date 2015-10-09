#include "Renderer_PCH.h"
#pragma hdrstop

#include <CodeGen/ResGuids_Editor_AUTO.h>
#include <Renderer/Core/Texture.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Util/BatchRenderer.h>
#include <Renderer/Pipeline/Shadows.h>
#include <Renderer/Pipeline/RenderQueue.h>

#include "RenderWorld.h"

#include <Engine/Worlds.h>

/*
-----------------------------------------------------------------------------
	rxRenderWorld
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(rxRenderWorld);
mxBEGIN_REFLECTION(rxRenderWorld)
	//mxMEMBER_FIELD( m_bounds )
	mxMEMBER_FIELD( m_models )

	mxMEMBER_FIELD( m_dirLights )
	mxMEMBER_FIELD( m_localLights )

	mxMEMBER_FIELD2( m_skyModel, Sky_Model )
mxEND_REFLECTION

rxRenderWorld::rxRenderWorld()
{
}

rxRenderWorld::~rxRenderWorld()
{}

void rxRenderWorld::ReserveNumEntities( UINT count )
{
	Assert( count <= MAX_RENDER_ENTITIES );
	count = smallest( count, MAX_RENDER_ENTITIES );
	m_entities.Reserve( count );
}

void rxRenderWorld::RegisterEntity( rxRenderEntity* newRenderEntity )
{
	Assert( m_entities.Num() <= MAX_RENDER_ENTITIES );

	m_entities.Add( newRenderEntity );
}

void rxRenderWorld::UnregisterEntity( rxRenderEntity* theRenderEntity )
{
	const UINT entityIndex = m_entities.FindIndexOf( theRenderEntity );

	CHK_VRET_IF_NOT( entityIndex != INDEX_NONE );

	m_entities.RemoveAt_Fast( entityIndex );
}

UINT rxRenderWorld::GetNumEntities()
{
	return m_entities.Num();
}

void rxRenderWorld::Clear()
{
	m_entities.Empty();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct SubmitVisiblePrimArgs
{
	rxEntityViewContext*context;
	rxRenderEntity **	entities;
};

static void SubmitVisiblePrimCallback( ActorID actorHandle, void* userData )
{
	const SubmitVisiblePrimArgs& args = *c_cast(const SubmitVisiblePrimArgs*) userData;
	args.entities[ actorHandle ]->rfSubmitBatches( *args.context );
}

void rxRenderWorld::rfBuildDrawList( const rxSceneContext& sceneContext, rxRenderQueue & q )
{
	// Find any potentially visible objects.

	rxEntityViewContext		entityViewContext;
	entityViewContext.q = &q;
	entityViewContext.s = &sceneContext;


	// Add model surfaces.

	//{
	//	const UINT numAabbs = m_bounds.Num();

	//	for( UINT i = 0; i < numAabbs; i++ )
	//	{
	//		const rxAABB & aabb = m_bounds[ i ];

	//		if( sceneContext.frustum.TestAABB( aabb ) )
	//		{
	//			m_visibleModels.Add( &models[i] );
	//		}
	//	}
	//}

	{
		const UINT numModels = m_models.Num();
		rxModel* models = m_models.ToPtr();

		for( UINT i = 0; i < numModels; i++ )
		{
			rxModel & model = models[ i ];

			if( sceneContext.frustum.TestAABB( model.m_worldAABB ) )
			{
				model.SubmitBatches( entityViewContext );
			}
		}
	}

	// Collect graphics primitives from generic entities.

#if USE_SPATIAL_DATABASE

	SubmitVisiblePrimArgs	args;
	args.context = &entityViewContext;
	args.entities = m_entities.ToPtr();

	m_entityTree.ProcessVisibleObjects( sceneContext, &SubmitVisiblePrimCallback, &args );

#else

	rxRenderEntity** pEnts = m_entities.ToPtr();
	const UINT numEnts = m_entities.Num();

	for( UINT iEntity = 0; iEntity < numEnts; iEntity++ )
	{
		rxRenderEntity* ent = pEnts[ iEntity ];

		rxAABB	aabb;
		ent->GetWorldAABB( aabb );

		if( sceneContext.frustum.TestAABB( aabb ) ) {
			ent->rfSubmitBatches( entityViewContext );
		}
	}

#endif



	q.sky = &m_skyModel;




	// Identify all potentially visible lights.

	// Collect global lights.

	const UINT numDirLights = m_dirLights.Num();
	for( UINT iDirLight = 0; iDirLight < numDirLights; iDirLight++ )
	{
		rxParallelLight& rDirLight = m_dirLights[ iDirLight ];
	
		q.globalLights.AddFast_Unsafe( &rDirLight );
	}

	//
	//	Find local lights.
	//

	const UINT numLocalLights = m_localLights.Num();
	for( UINT iLocalLight = 0; iLocalLight < numLocalLights; iLocalLight++ )
	{
		rxLocalLight& rLocalLight = m_localLights[ iLocalLight ];

		FASTBOOL bContainsEyePos = rLocalLight.EnclosesView( sceneContext );
		if( bContainsEyePos ) {
			q.AddLight( &rLocalLight, LightStage_Local_FullScreenQuad, rLocalLight.m_lightType );
		} else {
			q.AddLight( &rLocalLight, LightStage_Local_LightVolumeMeshes, rLocalLight.m_lightType );
		}
	}

	// Collect visible portals.

}

void rxRenderWorld::rfGetDirLightShadowCasters( rxShadowCastingSet & shadowCasters )
{
	const UINT numModels = m_models.Num();
	rxModel* models = m_models.ToPtr();

	for( UINT i = 0; i < numModels; i++ )
	{
		rxModel & model = models[ i ];

		shadowCasters.m_objects.Add( &model );
	}

//	rxRenderEntity** pEnts = m_entities.ToPtr();
//	const UINT numEnts = m_entities.Num();
//
//	for( UINT iEntity = 0; iEntity < numEnts; iEntity++ )
//	{
//		rxRenderEntity* ent = pEnts[ iEntity ];
//
//		//rxAABB	aabb;
//		//ent->GetAABB( aabb );
//
//		//if( sceneContext.frustum.IntersectsAABB(aabb) )
//		//{
//			shadowCasters.m_objects.Add(ent);
//		//}
//	}
}

void rxRenderWorld::rfGetSpotLightShadowCasters(
	const rxViewFrustum& lightFrustum,
	rxShadowCastingSet &shadowCasters
	)
{
	const UINT numModels = m_models.Num();
	rxModel* models = m_models.ToPtr();

	for( UINT i = 0; i < numModels; i++ )
	{
		rxModel & model = models[ i ];

		if( lightFrustum.TestAABB( model.m_worldAABB ) )
		{
			shadowCasters.m_objects.Add( &model );
		}
	}

	//rxRenderEntity** pEnts = m_entities.ToPtr();
	//const UINT numEnts = m_entities.Num();

	//for( UINT iEntity = 0; iEntity < numEnts; iEntity++ )
	//{
	//	rxRenderEntity* ent = pEnts[ iEntity ];

	//	rxAABB	aabb;
	//	ent->GetWorldAABB( aabb );

	//	if( lightFrustum.TestAABB(aabb) )
	//	{
	//		shadowCasters.m_objects.Add(ent);
	//	}
	//}
}

//void rxRenderWorld::ForEachEntity( F_EntityIterator* pIterator, void* pUserData )
//{
//	AssertPtr(pIterator);
//	VRET_IF_NOT(PtrToBool(pIterator));
//
//	rxRenderEntity** pEnts = m_entities.ToPtr();
//	const UINT numEnts = m_entities.Num();
//
//	for( UINT iEntity = 0; iEntity < numEnts; iEntity++ )
//	{
//		rxRenderEntity* ent = pEnts[ iEntity ];
//
//		(*pIterator)( ent, pUserData );
//	}
//}
//
//rxParallelLight* rxRenderWorld::CreateDirectionalLight()
//{
//	if( m_dirLights.IsFull() )
//	{
//		DEVOUT("Cannot have more than %u directional lights.\n",(UINT)m_dirLights.GetCapacity());
//		return nil;
//	}
//
//	const OID hNewLight = m_dirLights.Add();
//
//	UNDONE;
////#if MX_EDITOR
////	ZZ_Create_DirLight_Editor( this, hNewLight );
////#endif // MX_EDITOR
//
//	return m_dirLights.GetPtrByHandle( hNewLight );
//}
//
//rxLocalLight* rxRenderWorld::CreateLocalLight()
//{
//	if( m_localLights.IsFull() )
//	{
//		DEVOUT("Cannot have more than %u dynamic lights.\n",(UINT)m_localLights.GetCapacity());
//		return nil;
//	}
//
//	const OID hNewLight = m_localLights.Add();
//
//	UNDONE;
////#if MX_EDITOR
////	ZZ_Create_LocalLight_Editor( this, hNewLight );
////#endif // MX_EDITOR
//
//	return m_localLights.GetPtrByHandle( hNewLight );
//}

void rxRenderWorld::DebugDraw( const rxViewport* viewport, const rxSceneContext& sceneContext )
{
	//BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();

	//batchRenderer.SetTransform( sceneContext.viewProjectionMatrix );

	//if( g_cvar_debug_draw_spatial_database )
	//{
	//	m_entityTree.DebugDraw( sceneContext );
	//}

	//batchRenderer.Flush();
}

rxModel& rxRenderWorld::CreateModel()
{
	return m_models.Add();
}

rxParallelLight& rxRenderWorld::CreateDirectionalLight()
{
	return m_dirLights.Add();
}

rxLocalLight& rxRenderWorld::CreateLocalLight()
{
	return m_localLights.Add();
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
