/*
=============================================================================
	File:	Shadows.h
	Desc:	Shadow rendering.
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/Common.h>
#include <Renderer/Core/Font.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/Scene/RenderWorld.h>
#include <Renderer/Util/ScreenQuad.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>

#include <Renderer/Materials/SubsurfaceScattering.h>

#include <Renderer/Pipeline/Backend.h>
#include <Renderer/Pipeline/DeferredLighting.h>
#include <Renderer/Pipeline/Shadows.h>
#include <Renderer/Pipeline/PostProcess.h>
#include <Renderer/Pipeline/PostProcess/SeparableSSS.h>

#include <Renderer/Util/RenderMesh.h>

// Debugging.
#if 0
	#define RR_LOG(...)	DBGOUT( __VA_ARGS__ )
#else
	#define RR_LOG(...)
#endif

//---------------------------------------------------------------------------

static void F_BuildRenderQueue( const rxSceneContext& sceneContext, rxRenderQueue &renderQueue )
{
	mxPROFILE_SCOPE("Cull");

	rxRenderWorld& scene = sceneContext.scene;

	renderQueue.Empty();

	// Find visible objects and portals.

	// Generate render queue.

	scene.rfBuildDrawList( sceneContext, renderQueue );
}

//---------------------------------------------------------------------------

static void F_SortRenderQueue( rxRenderQueue & renderQueue )
{
	mxPROFILE_SCOPE("Sort");

	// Sort objects by material, entity index and eye-space depth.
	renderQueue.Sort();
}

//---------------------------------------------------------------------------

void Draw_Sorted_Batches(ERenderStage stage, const rxRenderContext& context,
						 const rxSurface* batches, UINT numBatches)
{
	rxMaterialRenderContext	materialContext;
	CopyStruct( materialContext, context );


	rxMaterial *	oldMaterial = nil;
	rxModel *		oldEntity = nil;
	rxMesh *		oldMesh = nil;

	UINT	iBatch = 0;

	while( iBatch < numBatches )
	{
		const rxSurface & batch = batches[ iBatch++ ];

		rxMaterial *	material = batch.k.material;
		rxModel *		entity = batch.entity;
		const UINT		subset = batch.subset;

		// update per-object data
		if( oldEntity != entity )
		{
			oldEntity = entity;

			materialContext.model = entity;

			rxMesh * mesh = entity->m_mesh.ToPtr();

			GPU::UpdatePerObjectConstants( entity->m_localToWorld );

			if( oldMesh != mesh )
			{
				oldMesh = mesh;

				rxGPU_MARKER(Bind_Mesh);
				F_Bind_Mesh( *mesh, context.pD3D );
			}

			RX_STATS(gfxStats.numEntities++);
		}

		materialContext.batch = subset;

		// change per-material data if needed
		if( oldMaterial != material )
		{
			oldMaterial = material;

			rxGPU_MARKER(Bind_Material);
			material->rfBind( stage, materialContext  );

			RX_STATS(gfxStats.numMaterialChanges++);
		}

//L_DrawBatch:
		const rxModelBatch& drawCall = entity->m_batches[ subset ];
		context.pD3D->DrawIndexed( drawCall.indexCount, drawCall.startIndex, drawCall.baseVertex );
	}//while

	RX_STATS(gfxStats.numBatches += numBatches);
}

void Draw_Sorted_Batches(ERenderStage stage,
						 const SRenderStageContext& context,
						 const rxRenderQueue& renderQueue)
{
	const UINT numBatches = renderQueue.numBatches[ stage ];
	if( !numBatches ) {
		return;
	}
	const UINT offset = context.batchOffsets[ stage ];
	const rxSurface* batches = renderQueue.GetBatches() + offset;

	Draw_Sorted_Batches( stage, context, batches, numBatches );
}


/*
--------------------------------------------------------------
	GeometryStage
--------------------------------------------------------------
*/
class GeometryStage
{
	GPU::GBuffer		m_buffers;

private:
	void ReleaseRenderTargets( UINT newWidth, UINT newHeight )
	{
		m_buffers.ReleaseRenderTargets();
	}
	void AcquireRenderTargets( UINT newWidth, UINT newHeight )
	{
		m_buffers.Initialize();
	}

public:
	GeometryStage()
	{
		mxCONNECT_THIS( gRenderer.BeforeMainViewportResized, GeometryStage, ReleaseRenderTargets );
		mxCONNECT_THIS( gRenderer.AfterMainViewportResized, GeometryStage, AcquireRenderTargets );

#if MX_DEVELOPER

		UINT	geometryBufferWidth, geometryBufferHeight;
		GPU::RT_Normal_SpecIntensity.CalcWidthHeight( geometryBufferWidth, geometryBufferHeight );

		UINT	geometryBufferSize = 0;

		geometryBufferSize += GPU::RT_Normal_SpecIntensity.CalcSizeInBytes();
		geometryBufferSize += GPU::RT_Diffuse_SpecPower.CalcSizeInBytes();
		geometryBufferSize += GPU::RT_LinearDepth.CalcSizeInBytes();

		DEVOUT("G-Buffer size(%ux%u): %u MiB\n",
			geometryBufferWidth, geometryBufferHeight, geometryBufferSize/mxMEBIBYTE );

#endif //MX_DEVELOPER

	}
	~GeometryStage()
	{
	}
	void Begin(const SRenderStageContext& context)
	{
		ID3D11DeviceContext* pD3DContext = context.pD3D;

		// Bind render targets and main depth-stencil surface.

		m_buffers.ClearRenderTargets( pD3DContext );
		m_buffers.Bind( pD3DContext, context.v->mainDS.pDSV );

		pD3DContext->RSSetViewports( 1, &context.v->d );

		GPU::Default.stencilRef = STENCIL_DEFAULT;
		GPU::Default.Set( pD3DContext );
	}
	void End(const SRenderStageContext& context)
	{
		// G-buffer has been filled with data.

		// Set shader resources that the following render stages can use.
		{
			GPU::Shared_GBuffer::RT_normal_specMul = GPU::RT_Normal_SpecIntensity.pSRV;
			GPU::Shared_GBuffer::RT_diffuse_specExp = GPU::RT_Diffuse_SpecPower.pSRV;
			GPU::Shared_GBuffer::RT_linearDepth = GPU::RT_LinearDepth.pSRV;
		}
		GPU::Shared_GBuffer::Set( context.pD3D );
	}
};



// this template will be specialized for each lighting stage/light type combination
template
<
	ELightStage LIGHT_STAGE,
	ELightType LIGHT_TYPE
>
static
void Apply_Deferred_Lighting_Template(const SLightingStageContext& rContext,
									  const rxRenderQueue& rQueue,
									  const rxLightEntry* lights,
									  const UINT numLights)
{
	NOT_IMPLEMENTED;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< ELightStage LIGHT_STAGE >
static
void Deferred_Lighting_Stage_Template(const SLightingStageContext& rContext,
									  const rxRenderQueue& rQueue,
									  const UINT lightOffsets[NumLightTypes])
{
	const rxLightEntry* lights = rQueue.localLights.ToPtr();

#define APPLY_LIGHTS_OF_TYPE( LIGHT_TYPE )\
	{\
		Apply_Deferred_Lighting_Template< LIGHT_STAGE, LIGHT_TYPE >(\
			rContext, rQueue,\
			lights + lightOffsets[ LIGHT_TYPE ],\
			rQueue.numLocalLights[ LIGHT_STAGE ][ LIGHT_TYPE ]\
		);\
	}

	APPLY_LIGHTS_OF_TYPE( ELightType::Light_Point );
	APPLY_LIGHTS_OF_TYPE( ELightType::Light_Spot );

#undef APPLY_LIGHTS_OF_TYPE

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template<>
static
void Apply_Deferred_Lighting_Template
	<
		ELightStage::LightStage_Local_FullScreenQuad,
		ELightType::Light_Point
	>
	(
		const SLightingStageContext& rContext,
		const rxRenderQueue& rQueue,
		const rxLightEntry* lights,
		const UINT numLights
	)
{
	rxGPU_MARKER(FullScreen_PointLights);

	const rxSceneContext* pSceneContext = rContext.s;

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		const rxLightEntry & rLightEntry = lights[ iLight ];

		rxLocalLight* pLight = rLightEntry.CastToLocalLight(Light_Point);

		typedef GPU::p_deferred_fullscreen_point_light ShaderProgram;

		rxShaderInstanceId	shaderInstanceId = ShaderProgram::DefaultInstanceId;

		GPU::Shared_LocalLightData::PerLocalLight* pData = GPU::Shared_LocalLightData::cb_PerLocalLight.Map( rContext.pD3D );
		{
			F_CopyCommonLightData( pData, pLight, pSceneContext );
		}
		GPU::Shared_LocalLightData::cb_PerLocalLight.Unmap( rContext.pD3D );

		ShaderProgram::Set( rContext.pD3D, shaderInstanceId );

		GPU::Deferred_Light_FullScreen_Additive.Set( rContext.pD3D );

		RenderFullScreenQuadOnly( rContext.pD3D );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template<>
static
void Apply_Deferred_Lighting_Template
	<
		ELightStage::LightStage_Local_LightVolumeMeshes,
		ELightType::Light_Point
	>
	(
		const SLightingStageContext& rContext,
		const rxRenderQueue& rQueue,
		const rxLightEntry* lights,
		const UINT numLights
	)
{
	rxGPU_MARKER(Local_PointLights);

	const rxSceneContext* pSceneContext = rContext.s;

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		const rxLightEntry & rLightEntry = lights[ iLight ];

		rxLocalLight* pLight = rLightEntry.CastToLocalLight(Light_Point);

		typedef GPU::p_deferred_local_point_light ShaderProgram;

		rxShaderInstanceId	shaderInstanceId = ShaderProgram::DefaultInstanceId;

		GPU::Shared_LocalLightData::PerLocalLight* pData = GPU::Shared_LocalLightData::cb_PerLocalLight.Map( rContext.pD3D );
		{
			F_CopyCommonLightData( pData, pLight, pSceneContext );

			const float radius = pLight->GetRadius();
			const float4 scaling = XMVectorSet(radius,radius,radius,0.0f);
			const float4x4	lightWorldMatrix = XMMatrixAffineTransformation(
				scaling, g_XMZero, g_XMIdentityR3, pLight->m_position
				);

			pData->lightShapeTransform = XMMatrixMultiply( lightWorldMatrix, pSceneContext->viewProjectionMatrix );
		}
		GPU::Shared_LocalLightData::cb_PerLocalLight.Unmap( rContext.pD3D );

		ShaderProgram::Set( rContext.pD3D, shaderInstanceId );

		GPU::Deferred_Light_ConvexMesh_Additive.Set( rContext.pD3D );

		rContext.lightShapes->unitPointLightShape.SetAndDraw( rContext.pD3D );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template<>
static
void Apply_Deferred_Lighting_Template
	<
		ELightStage::LightStage_Local_FullScreenQuad,
		ELightType::Light_Spot
	>
	(
		const SLightingStageContext& rContext,
		const rxRenderQueue& rQueue,
		const rxLightEntry* lights,
		const UINT numLights
	)
{
	rxGPU_MARKER(FullScreen_SpotLights);

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		const rxLightEntry & rLightEntry = lights[ iLight ];

		rxLocalLight* pLight = rLightEntry.CastToLocalLight(Light_Spot);


		typedef GPU::p_deferred_fullscreen_spot_light ShaderProgram;

		const rxShaderInstanceId shaderInstanceId = F_UpdateSpotLightData< ShaderProgram >( rContext, pLight );

		ShaderProgram::Set( rContext.pD3D, shaderInstanceId );

		GPU::Deferred_Light_FullScreen_Additive.Set( rContext.pD3D );

		RenderFullScreenQuadOnly( rContext.pD3D );

		F_UnbindShadowMap< ShaderProgram >( rContext.pD3D );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template<>
static
void Apply_Deferred_Lighting_Template
	<
		ELightStage::LightStage_Local_LightVolumeMeshes,
		ELightType::Light_Spot
	>
	(
		const SLightingStageContext& rContext,
		const rxRenderQueue& rQueue,
		const rxLightEntry* lights,
		const UINT numLights
	)
{
	rxGPU_MARKER(Local_SpotLights);

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		const rxLightEntry & rLightEntry = lights[ iLight ];

		rxLocalLight* pLight = rLightEntry.CastToLocalLight(Light_Spot);


		typedef GPU::p_deferred_local_spot_light ShaderProgram;

		const rxShaderInstanceId shaderInstanceId = F_UpdateSpotLightData< ShaderProgram >( rContext, pLight );

		GPU::Deferred_Light_ConvexMesh_Additive.Set( rContext.pD3D );

		ShaderProgram::Set( rContext.pD3D, shaderInstanceId );

		rContext.lightShapes->unitSpotLightShape.SetAndDraw( rContext.pD3D );

		F_UnbindShadowMap< ShaderProgram >( rContext.pD3D );
	}
}

static
void Render_Directional_Lights(
	const SLightingStageContext& rContext,
	const rxRenderQueue& rQueue
	)
{
	const rxSceneContext* pSceneContext = rContext.s;

	// Apply directional (global, full-screen) lights.

	const UINT numDirectionalLights = rQueue.globalLights.Num();

	for( UINT iDirLight = 0; iDirLight < numDirectionalLights; iDirLight++ )
	{
		const rxParallelLight * pLight = rQueue.globalLights[ iDirLight ];

		float4	lightDirWS = pLight->m_lightDirWS;
		float4	lightVecWS = XMVectorNegate( lightDirWS );
		float4	lightVecVS = XMVector3TransformNormal( lightVecWS, pSceneContext->viewMatrix );
		lightVecVS = XMVector3Normalize(lightVecVS);

		typedef GPU::p_deferred_directional_light ProgramType;

		rxShaderInstanceId	shaderInstanceId = ProgramType::DefaultInstanceId;

		if( g_cvar_enable_directional_light_shadows && pLight->DoesCastShadows() )
		{
			shaderInstanceId |= ProgramType::bLight_CastShadows;

			ProgramType::shadowDepthMap = rContext.shadowMgr->Process_Directional_Light( rContext, *pLight );
		}
		if( rxShadowManager::g_cvar_dir_light_soft_shadows )
		{
			shaderInstanceId |= ProgramType::bEnableSoftShadows;
		}
		if( rxShadowManager::g_cvar_dir_light_visualize_cascades )
		{
			shaderInstanceId |= ProgramType::bVisualizeCascades;
		}

		ProgramType::Data* pData = ProgramType::cb_Data.Map( rContext.pD3D );
		{
			pData->lightVectorVS = lightVecVS;
			pData->lightDiffuseColor = pLight->m_diffuseColor;
			pData->lightSpecularColor = pLight->m_specularColor;

			for( UINT iCascade = 0; iCascade < NUM_SHADOW_CASCADES; iCascade++ )
			{
				pData->shadowMatrices[ iCascade ] = rContext.shadowMgr->m_shadowMatrices[ iCascade ];
				as_vec4(pData->cascadeSplits)[ iCascade ] = rContext.shadowMgr->m_cascadeSplits[ iCascade ];
			}
		}
		ProgramType::cb_Data.Unmap( rContext.pD3D );
		ProgramType::Set( rContext.pD3D, shaderInstanceId );

		GPU::Deferred_Light_FullScreen_Additive.Set( rContext.pD3D );

		RenderFullScreenQuadOnly( rContext.pD3D );

		F_UnbindShadowMap< ProgramType >( rContext.pD3D );
	}
}

/*
--------------------------------------------------------------
	rxDeferredLighting

	Calculates lighting
	( diffuse light-RGB, specular light & intensity only).
	Interleaved rendering with shadow map.
	Renders emissive objects.
--------------------------------------------------------------
*/
class rxDeferredLighting
{
	LightShapes			m_lightShapes;
	rxShadowManager		m_shadowsMgr;

public:
	rxDeferredLighting()
	{
		m_shadowsMgr.Initialize();
		m_lightShapes.CreateMeshes();
	}
	~rxDeferredLighting()
	{

	}
	void Begin(const rxRenderContext& context)
	{
		GPU::Shared_LocalLightData::Set( context.pD3D );
	}
	void End(const rxRenderContext& context)
	{

	}

	void Render( const SRenderStageContext& context, const rxRenderQueue& renderQueue )
	{
		mxPROFILE_FUNCTION;

		SLightingStageContext	lightingStageContext;
		{
			CopyStruct( lightingStageContext, context );

			lightingStageContext.lightShapes = &m_lightShapes;
			lightingStageContext.shadowMgr = &m_shadowsMgr;
		}

		UINT	lightOffsets[NumLightStages][NumLightTypes];
		Build_Offset_Table_2D< NumLightStages, NumLightTypes >( renderQueue.numLocalLights, lightOffsets );


		// Loop for each light stage.

		Render_Directional_Lights( lightingStageContext, renderQueue );

		Deferred_Lighting_Stage_Template< LightStage_Local_FullScreenQuad >( lightingStageContext, renderQueue, lightOffsets[LightStage_Local_FullScreenQuad] );
		Deferred_Lighting_Stage_Template< LightStage_Local_LightVolumeMeshes >( lightingStageContext, renderQueue, lightOffsets[LightStage_Local_LightVolumeMeshes] );
	}
};

/*
-----------------------------------------------------------------------------
	SceneRenderer
-----------------------------------------------------------------------------
*/
class SceneRenderer
{
	GeometryStage		m_geometryStage;
	rxDeferredLighting	m_lightStage;

	rxPostProcessor		m_postProcessor;
	SeparableSSS		m_separableSSS;

	GameTimer		m_timer;

	// put large structures at the end
	rxRenderQueue	m_renderQueue;

public:
	SceneRenderer()
	{

	}
	~SceneRenderer()
	{
		m_postProcessor.Shutdown();
	}

	void Initialize( UINT viewportWidth, UINT viewportHeight )
	{
		m_postProcessor.Initialize( viewportWidth, viewportHeight );
	}

	void RenderScene( const rxViewport* viewport, const rxSceneContext& sceneContext )
	{
#if RX_D3D_USE_PERF_HUD
		PIXEvent	renderSceneEvent(L"Render Scene");
#endif // RX_D3D_USE_PERF_HUD

		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

		const FLOAT	currentTimeInSeconds = m_timer.TickFrame();
		const FLOAT	deltaTimeInSeconds = m_timer.DeltaSeconds();

		Assert( sceneContext.aspectRatio == viewport->GetAspectRatio() );


		GPU::UpdatePerFrameConstants( *viewport, currentTimeInSeconds, deltaTimeInSeconds );
		GPU::Shared_Globals::Set( pD3DContext );
		RX_STATS(gfxStats.frameCount++);


		GPU::UpdatePerViewConstants( sceneContext );
		GPU::Shared_View::Set( pD3DContext );
		RX_STATS(gfxStats.viewCount++);


		// Generate render queue.
		{
			mxPROFILE_SCOPE("Prepare render queue");

			// Find visible objects and portals.
			F_BuildRenderQueue( sceneContext, m_renderQueue );

			// Sort batches by material, geometry, eye-space depth, etc.
			// Sort lights.
			F_SortRenderQueue( m_renderQueue );
		}





		SRenderStageContext		renderContext;

		renderContext.v = viewport;
		renderContext.s = &sceneContext;

	#if RX_USE_PROXY_DEVICE_CONTEXT
		D3DDeviceContext	proxyDeviceContext( pD3DContext );
		renderContext.pD3D = &proxyDeviceContext;
	#else
		renderContext.pD3D = pD3DContext;
	#endif //RX_USE_PROXY_DEVICE_CONTEXT

		renderContext.time = currentTimeInSeconds;

		Build_Offset_Table_1D( m_renderQueue.numBatches, renderContext.batchOffsets );




		// First render everything except portals.

		// Draw the current cell (containing the eye) and, possibly, solid portal occluders
		// to cull geometry behind portals.

		// Draw everything in the current cell except portals and collect visible portals...



		GPU::Shared_Object::Set( pD3DContext );


		// Fill geometry buffers.
		{
			rxGPU_MARKER(Fill_Geometry_Buffer);
			mxPROFILE_SCOPE("Fill buffers");

			// Rasterize G-buffers for opaque surfaces.

			m_geometryStage.Begin( renderContext );

			Draw_Sorted_Batches( RS_Deferred_FillGBuffer, renderContext, m_renderQueue );

			// restore the main render target and the depth-stencil surface.
			// NOTE: it must be done before binding G-buffer for reading
			viewport->Set( pD3DContext );

			// bind light transport accumulator (it can be a special HDR target or the main framebuffer)
			m_postProcessor.BindLightAccumulationBuffer( renderContext );

			m_geometryStage.End( renderContext );
		}


		// Apply deferred lighting.

		const UINT numGlobalLights = m_renderQueue.globalLights.Num();
		const UINT numLocalLights = Calculate_Sum_2D( m_renderQueue.numLocalLights );

		RX_STATS(gfxStats.numGlobalLights += numGlobalLights);
		RX_STATS(gfxStats.numLocalLights += numLocalLights);

		if( numGlobalLights + numLocalLights )
		{
			rxGPU_MARKER(Deferred_Lighting);
			mxPROFILE_SCOPE("Deferred lighting");

			m_lightStage.Begin( renderContext );
			m_lightStage.Render( renderContext, m_renderQueue );
			m_lightStage.End( renderContext );
		}

		// Unlit
		//if(0)
		//{
		//	mxPROFILE_SCOPE("Unlit");

			//GPU::Default.Set( pD3DContext );

		//	TRenderLoop
		//	<
		//		BIT(DO_Opaque)

		//	>::DrawSortedBatches( m_unlitOpaque, renderContext, m_renderQueue );
		//}



		// Subsurface scattering simulation.

		if( m_renderQueue.numBatches[ RS_Forward_SSS ] )
		{
			// Forward pass.
			{
				pD3DContext->OMSetDepthStencilState( GPU::DS_NormalZTestWrite_StencilWrite, STENCIL_SSS_ID );
				pD3DContext->OMSetBlendState( GPU::Default.blend, GPU::Default.blendFactorRGBA, GPU::Default.sampleMask );
				pD3DContext->RSSetState( GPU::Default.rasterizer );

				Draw_Sorted_Batches( RS_Forward_SSS, renderContext, m_renderQueue );
			}

			// Subsurface Scattering Pass (Post-processing).

			HOT_BOOL(g_cvar_enable_separable_sss_post_fx);

			// GPU::RT_HDR_SceneColor is only filled if 'g_cvar_enable_post_processing_effects' is set to true.
			if( g_cvar_enable_separable_sss_post_fx && g_cvar_enable_post_processing_effects )
			{
				pD3DContext->OMSetDepthStencilState( GPU::DS_SeparableSSS, STENCIL_SSS_ID );

				m_separableSSS.Go( GPU::RT_HDR_SceneColor, viewport->mainDS );
			}

			// restore default depth-stencil state
			pD3DContext->OMSetDepthStencilState( GPU::DS_NormalZTestWriteNoStencil, STENCIL_DEFAULT );
		}


		// Draw the sky if needed.

		//@fixme: early-Z won't work after the lighting stage so sky pixels will be culled inefficiently

		rxSkyModel* pSkyModel = m_renderQueue.sky.Ptr;
		if( pSkyModel != nil )
		{
			rxGPU_MARKER(Draw_Sky);
			mxPROFILE_SCOPE("Draw skies");

			mxUNDONE;
			//pSkyModel->Render( renderContext );
		}


		{
			rxGPU_MARKER(Post_Processing);
			m_postProcessor.WriteFinalColorToBackBuffer( renderContext );
		}
	}

private:

};

static TBlob16<SceneRenderer>	TheSceneRenderer;

bool	g_cvar_enable_directional_light_shadows = true;
bool	g_cvar_enable_point_light_shadows = true;
bool	g_cvar_enable_spot_light_shadows = true;

void Pipeline_Init( UINT viewportWidth, UINT viewportHeight )
{
	HOT_BOOL(g_cvar_enable_directional_light_shadows);
	HOT_BOOL(g_cvar_enable_point_light_shadows);
	HOT_BOOL(g_cvar_enable_spot_light_shadows);

	//mxPut( "Initializing the rendering pipeline.\n" );
	TheSceneRenderer.Construct();
	TheSceneRenderer.Get().Initialize( viewportWidth, viewportHeight );
}

void Pipeline_Close()
{
	//mxPut( "Shutting down the rendering pipeline.\n" );
	TheSceneRenderer.Destruct();
}

// releases render targets
void Pipeline_ReleaseBuffers()
{
	rxGPU_MARKER(Pipeline_ReleaseBuffers);

	DBGOUT( "Deallocating render targets.\n" );
	GPU::ReleaseRenderTargets();
}

// reallocates render targets
void Pipeline_ResizeBuffers( UINT maxViewportWidth, UINT maxViewportHeight )
{
	rxGPU_MARKER(Pipeline_ResizeBuffers);

	DBGOUT( "Resizing render targets to %ux%u.\n", maxViewportWidth, maxViewportHeight );
	GPU::ReallocRenderTargets( maxViewportWidth, maxViewportHeight );
}

void Pipeline_RenderScene( const rxViewport* viewport, const rxSceneContext& sceneContext )
{
	const UINT startTime = mxGetTimeInMicroseconds();

	gfxBEStats.Reset();

	// Draw the scene.

	TheSceneRenderer.Get().RenderScene( viewport, sceneContext );

	// Draw debug info.
	{
		rxGPU_MARKER(Draw_Debug_Info);

		sceneContext.scene.DebugDraw( viewport, sceneContext );
	}


	gfxBEStats.lastFrameRenderTime = mxGetTimeInMicroseconds() - startTime;
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
