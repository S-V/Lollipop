/*
=============================================================================
	File:	SubsurfaceScattering.cpp
	Desc:	
	ToDo:	remove random memory accesses, bad for cache
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include <Core/Editor/EditableProperties.h>
#include <Core/Util/Tweakable.h>

#include <Renderer/Pipeline/RenderQueue.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/RenderWorld.h>

#include <Renderer/GPU/HLSL/BuildConfig.h>
#include <Renderer/GPU/ShaderPrograms.hxx>

#include <Renderer/Materials/SubsurfaceScattering.h>

#include <Renderer/Pipeline/Backend.h>
#include <Renderer/Pipeline/Shadows.h>

bool g_cvar_enable_subsurface_scattering = true;
bool g_cvar_enable_separable_sss_post_fx = true;

/*
-----------------------------------------------------------------------------
	Material_SSS
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(Material_SSS);

mxBEGIN_REFLECTION(Material_SSS)
	mxMEMBER_FIELD2( diffuseColor,	Diffuse_Color )
	mxMEMBER_FIELD2( specularColor,	Specular_Color )
	mxMEMBER_FIELD2( emissiveColor,	Emissive_Color )

	mxMEMBER_FIELD2( baseMap,		Diffuse_Map )
	mxMEMBER_FIELD2( normalMap,		Normal_Map,		Field_NoDefaultInit )
	mxMEMBER_FIELD2( specularMap,	Specular_Map,	Field_NoDefaultInit )
	mxMEMBER_FIELD2( beckmannMap,	Beckmann_Map,	Field_NoDefaultInit )
mxEND_REFLECTION

static
void Apply_Material_SSS_GBuffer_Pass(
	const rxMaterialRenderContext& context,
	rxMaterial* pMaterial
	)
{
	Material_SSS* pSSSMaterial = pMaterial->UpCast< Material_SSS >();

	ID3D11DeviceContext* pD3DContext = context.pD3D;

	typedef GPU::p_skin_shader_gbuffer_pass ShaderType;

	rxShaderInstanceId	shaderInstanceId = ShaderType::DefaultInstanceId;

	Assert( pSSSMaterial->baseMap.IsValid() );
	{
		ShaderType::baseMap = pSSSMaterial->baseMap.ToPtr()->pSRV;
	}
	if( pSSSMaterial->normalMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bHasNormalMap;
		ShaderType::normalMap = pSSSMaterial->normalMap.ToPtr()->pSRV;
	}
	if( pSSSMaterial->specularMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bHasSpecularMap;
		ShaderType::specularMap = pSSSMaterial->specularMap.ToPtr()->pSRV;
	}

	ShaderType::Set( pD3DContext, shaderInstanceId );
}

static
void Apply_Material_SSS_Forward_Pass(
	const rxMaterialRenderContext& context,
	rxMaterial* pMaterial
	)
{
	Material_SSS* pSSSMaterial = pMaterial->UpCast< Material_SSS >();


	HOT_BOOL(g_cvar_enable_subsurface_scattering);
	//DBG_DO_INTERVAL(DBGOUT("\n---Apply_Material_SSS_Forward_Pass:\n"),1000);

	// find the most influential (most relevant) lights

	rxRenderWorld& scene = context.s->scene;

	TList< rxLocalLight >& lights = scene.m_localLights;

	if( lights.IsEmpty() ) {
		return;
	}

	rxLocalLight *	mainLight = nil;

	for( UINT iLight = 0; iLight < lights.Num(); iLight++ )
	{
		rxLocalLight& light = lights[ iLight ];
		//if( light.m_lightType == Light_Spot ) {
		//	//
		//}
		if( light.m_bUseSSS ) {
			mainLight = &light;
		}
	}

	if( !mainLight ) {
		return;
	}


	const Vec3D lightPos = mainLight->GetOrigin();
	const Vec3D lightDir = mainLight->GetDirection();
	const F4 lightRadius = 10.0f;//minf( mainLight->GetRadius(), 10.0f );
	const F4 lightInvRadius = 1.0f / maxf( lightRadius, 0.001f );
	const F4 lightNearPlane = 0.1f;


	rxModel* model = context.model;

	ID3D11DeviceContext* pD3DContext = context.pD3D;



	// Build shadow map.
	//{
		const float4x4 lightWorldMatrix = BuildBasisVectors( mainLight->m_position, mainLight->m_spotDirection );
		const float4x4 lightViewMatrix = WorldMatrixToViewMatrix( lightWorldMatrix );

		// build regular projection matrix
		const F4 lightFoV = DEG2RAD(90);
		const F4 aspect = 1.0f;
		float4x4 lightProjMatrix;
		as_matrix4( lightProjMatrix ).BuildPerspectiveLH( lightFoV, aspect, lightNearPlane, lightRadius );

		const float4x4 lightViewProjMatrix = lightViewMatrix * lightProjMatrix;

		const UINT shadowMapSquareSize = rxShadowManager::Static_Get_Shadow_Map_Atlas_Size();
		const F4 invShadowMapSize = 1.0f / shadowMapSquareSize;


		const bool bShadowsAreEnabled = true;

		if( bShadowsAreEnabled )
		{
			// build linear projection matrix
			// see: http://www.mvps.org/directx/articles/linear_z/linearz.htm
			//
			Matrix4 linearProjection = as_matrix4(lightProjMatrix);
			const F4 Q = linearProjection[2][2];
			const F4 N = -linearProjection[3][2] / linearProjection[2][2];
			const F4 F = -N * Q / (1 - Q);
			linearProjection[2][2] /= F;
			linearProjection[3][2] /= F;

			F_UnbindShadowMap< GPU::p_skin_shader_forward_pass >( pD3DContext );

			{
				GPU::p_skin_shader_build_shadow_map::Data* pData = GPU::p_skin_shader_build_shadow_map::cb_Data.Map(pD3DContext);
				{
					float4x4 lightLinearProjMatrix;
					as_matrix4(lightLinearProjMatrix) = linearProjection;

					const float4x4 linearLightViewProjMatrix = lightViewMatrix * lightLinearProjMatrix;
					pData->lightWVP = XMMatrixMultiply( model->m_localToWorld, linearLightViewProjMatrix );
				}
				GPU::p_skin_shader_build_shadow_map::cb_Data.Unmap(pD3DContext);
			}

			rxSET_SHADER_SCOPED( p_skin_shader_build_shadow_map, pD3DContext );

			rxScopedShadowMapSetter		bindShadowMap(
				pD3DContext,
				rxShadowManager::Get().m_shadowMap.pDSV,
				shadowMapSquareSize
				);

			rxShadowRenderContext	shadowRenderContext;
			shadowRenderContext.s = context.s;
			shadowRenderContext.pD3D = pD3DContext;

			model->RenderShadowDepth( shadowRenderContext );
		}
	//}


	// Apply subsurface scattering.

	typedef GPU::p_skin_shader_forward_pass ShaderType;

	{
		ShaderType::Data* pData = ShaderType::cb_Data.Map( pD3DContext );
		{
			pData->lightPos_Radius = XMVectorSet( lightPos.x, lightPos.y, lightPos.z, lightRadius );
			pData->lightDir_InvRadius = XMVectorSet( lightDir.x, lightDir.y, lightDir.z, lightInvRadius );
			pData->lightDiffuseColor = mainLight->m_diffuseColor;


			F4 specularFresnel = 0.82f;
			F4 specularIntensity = 1.88f;
			F4 specularRoughness = 0.3f;
			// x - specularFresnel (0.82), y - specularIntensity (1.88f), z - specularRoughness (0.3f)
			pData->lightSpecularParams = XMVectorSet( specularFresnel, specularIntensity, specularRoughness, 0.0f );


			F4 lightAttenuation = 1.0f / 128.0f;
			//F4 lightFalloffWidth = 0.05f;
			F4 lightFalloffWidth = 0.25f;
			F4 lightFalloffStart = mxCos( 0.5f * lightFoV );
			// x - attenuation, y - start angle of spot falloff in radians, z - inverse of spot falloff width
			pData->lightSpotParams = XMVectorSet( lightAttenuation, lightFalloffStart, 1.0f/lightFalloffWidth, 0.0f );


			const float4x4 texScaleBias = Matrix4x4_Build_Texture_Scale_Bias();
			pData->lightViewProjection = lightViewProjMatrix * texScaleBias;


			F4 shadowBias = -0.01f;
			pData->lightShadowParams = XMVectorSet( shadowBias, invShadowMapSize, 0.0f, 0.0f );
		}
		ShaderType::cb_Data.Unmap( pD3DContext );
	}

	rxShaderInstanceId	shaderInstanceId = ShaderType::DefaultInstanceId;

	if( g_cvar_enable_subsurface_scattering )
	{
		shaderInstanceId |= ShaderType::bEnableSSS;
	}

	if( bShadowsAreEnabled )
	{
		shaderInstanceId |= ShaderType::bEnableShadows;
	}

	Assert( pSSSMaterial->baseMap.IsValid() );
	{
		ShaderType::baseMap = pSSSMaterial->baseMap.ToPtr()->pSRV;
	}
	if( pSSSMaterial->normalMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bUseNormalMap;
		ShaderType::normalMap = pSSSMaterial->normalMap.ToPtr()->pSRV;
	}
	if( pSSSMaterial->specularMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bUseSpecularMap;
		ShaderType::specularMap = pSSSMaterial->specularMap.ToPtr()->pSRV;
	}
	if( pSSSMaterial->beckmannMap.IsValid() )
	{
		ShaderType::beckmannTexture = pSSSMaterial->beckmannMap.ToPtr()->pSRV;
	}
	ShaderType::shadowDepthMap = rxShadowManager::Get().m_shadowMap.pSRV;

	ShaderType::Set( pD3DContext, shaderInstanceId );
}

Material_SSS::Material_SSS()
{
	this->rfBindProgram[ RS_Deferred_FillGBuffer ] = &Apply_Material_SSS_GBuffer_Pass;
	this->rfBindProgram[ RS_Forward_SSS ] = &Apply_Material_SSS_Forward_Pass;

	diffuseColor = FColor::WHITE;
	specularColor = FColor::WHITE;
	emissiveColor = FColor::BLACK;
}

Material_SSS::~Material_SSS()
{

}

void Material_SSS::rfSubmitBatches( const rxMaterialViewContext& context )
{
	DEBUG_CODE( context.DbgCheckValid() );

	context.q->AddBatch(
		this,
		RS_Deferred_FillGBuffer,
		DO_Opaque,
		mxFtoI(context.fDistance),mxTODO("dist not used")
		context.entity,
		context.subset
	);

	context.q->AddBatch(
		this,
		RS_Forward_SSS,
		DO_Opaque,
		mxFtoI(context.fDistance),mxTODO("dist not used")
		context.entity,
		context.subset
	);
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
