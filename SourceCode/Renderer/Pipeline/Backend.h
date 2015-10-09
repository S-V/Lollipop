/*
=============================================================================
	File:	Backend.h
	Desc:	Rendering pipeline interface.
=============================================================================
*/

#pragma once

/*
=======================================================================
	
	Graphics pipeline

=======================================================================
*/

#include <Core/Util/Timer.h>

#include <Renderer/Pipeline/RenderQueue.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Core/Material.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/Globals.h>
#include <Renderer/Util/RenderMesh.h>


class rxShadowManager;
class LightShapes;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void Pipeline_Init( UINT viewportWidth, UINT viewportHeight );
void Pipeline_Close();

// releases render targets
void Pipeline_ReleaseBuffers();

// reallocates render targets
void Pipeline_ResizeBuffers( UINT maxViewportWidth, UINT maxViewportHeight );

void Pipeline_RenderScene( const rxViewport* viewport, const rxSceneContext& sceneContext );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

extern bool	g_cvar_enable_directional_light_shadows;
extern bool	g_cvar_enable_point_light_shadows;
extern bool	g_cvar_enable_spot_light_shadows;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


/*
-----------------------------------------------------------------------------
	SRenderStageContext
-----------------------------------------------------------------------------
*/
struct SRenderStageContext : rxRenderContext
{
	// offsets for indexing into sorted array of batches
	UINT	batchOffsets[RS_MAX];
};

/*
-----------------------------------------------------------------------------
	SLightingStageContext
-----------------------------------------------------------------------------
*/
struct SLightingStageContext : SRenderStageContext
{
	rxShadowManager *	shadowMgr;
	LightShapes *		lightShapes;
};





//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static inline
void F_CopyCommonLightData(
	GPU::Shared_LocalLightData::PerLocalLight* pDest,
	const rxLocalLight* pLight,
	const rxSceneContext* pContext
	)
{
	pDest->lightDiffuseColor = pLight->m_diffuseColor;

	const float4 lightPosWS = pLight->m_position;
	pDest->lightPosition = XMVector3Transform( lightPosWS, pContext->viewMatrix );

	pDest->lightRadiusInvRadius = pLight->m_radiusInvRadius;

	pDest->lightSpecularColor = pLight->m_specularColor;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static inline
void F_CopySpotLightData(
	GPU::Shared_LocalLightData::PerLocalLight* pDest,
	const rxLocalLight* pLight,
	const rxSceneContext* pContext
	)
{
	//Assert( pLight->m_lightType == Light_Spot );
	const float4 lightDirWS = pLight->m_spotDirection;
	//pData->spotLightDirection = XMMatrixMultiply( lightDirWS, s->viewMatrix );
	pDest->spotLightDirection = XMVector3TransformNormal( lightDirWS, pContext->viewMatrix );

	pDest->spotLightAngles = pLight->m_spotAngles;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class SHADER_PROGRAM >
rxShaderInstanceId F_UpdateSpotLightData(
	const SLightingStageContext& rContext,
	const rxLocalLight* pLight
	)
{
	const rxSceneContext* pSceneContext = rContext.s;
	//ID3D11DeviceContext* pD3DContext = rContext.pD3D;

	rxShaderInstanceId	shaderInstanceId = SHADER_PROGRAM::DefaultInstanceId;

	const float4x4	lightWorldMatrix = CalcSpotLightWorldMatrix(pLight);
	const float4x4	lightShapeScale = CalcSpotLightScale(pLight);

	//@todo: these are only needed if the lights projects an image or casts shadows:

	const float4x4	lightViewMatrix = WorldMatrixToViewMatrix(lightWorldMatrix);
	const float4x4	lightProjectionMatrix = CalcSpotLightProjectionMatrix(pLight);
	const float4x4	lightViewProjectionMatrix = XMMatrixMultiply( lightViewMatrix, lightProjectionMatrix );

	if( g_cvar_enable_spot_light_shadows && pLight->DoesCastShadows() )
	{
		shaderInstanceId |= SHADER_PROGRAM::bSpotLight_CastShadows;

		rxViewFrustum	lightFrustum;
		lightFrustum.Build( &lightWorldMatrix, &lightProjectionMatrix );

		SHADER_PROGRAM::shadowDepthMap = rContext.shadowMgr->Prepare_ShadowMap_for_SpotLight(
			rContext, *pLight, lightFrustum, lightViewProjectionMatrix
			);
	}
	else
	{
		SHADER_PROGRAM::shadowDepthMap = nil;
	}

	GPU::Shared_LocalLightData::PerLocalLight* pData = GPU::Shared_LocalLightData::cb_PerLocalLight.Map( rContext.pD3D );
	{
		F_CopyCommonLightData( pData, pLight, pSceneContext );
		F_CopySpotLightData( pData, pLight, pSceneContext );

		pData->lightShapeTransform = XMMatrixMultiply( lightShapeScale, lightWorldMatrix );
		pData->lightShapeTransform = XMMatrixMultiply( pData->lightShapeTransform, pSceneContext->viewProjectionMatrix );

		pData->eyeToLightProjection = XMMatrixMultiply( pSceneContext->invViewMatrix, lightViewProjectionMatrix );

		pData->lightProjectorIntensity = pLight->m_projectorIntensity;
		pData->lightShadowDepthBias = pLight->m_shadowDepthBias;

		if( pLight->m_projector.IsValid() )
		{
			shaderInstanceId |= SHADER_PROGRAM::bSpotLight_ProjectsTexture;

			SHADER_PROGRAM::projector = pLight->m_projector.ToPtr()->pSRV;
		}
		else
		{
			SHADER_PROGRAM::projector = nil;
		}
	}
	GPU::Shared_LocalLightData::cb_PerLocalLight.Unmap( rContext.pD3D );

	return shaderInstanceId;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class SHADER_PROGRAM >
static inline
void F_UnbindShadowMap( ID3D11DeviceContext* pD3DContext )
{
	ID3D11ShaderResourceView *	nullSRV[1] = { nil };
	pD3DContext->PSSetShaderResources( SHADER_PROGRAM::SR_Slot_shadowDepthMap, 1, nullSRV );
}

void Draw_Sorted_Batches(ERenderStage stage, const rxRenderContext& context,
						 const rxSurface* batches, UINT numBatches);

void Draw_Sorted_Batches(ERenderStage stage,
						 const SRenderStageContext& context,
						 const rxRenderQueue& renderQueue);


// Stencil values for subsurface scattering passes.

enum {
	STENCIL_DEFAULT = 0,
	STENCIL_SSS_ID = 100
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
