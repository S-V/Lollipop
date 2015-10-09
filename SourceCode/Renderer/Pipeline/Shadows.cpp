/*
=============================================================================
	File:	Shadows.cpp
	Desc:	Shadow rendering.
	ToDo:	.
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Core/Util/Tweakable.h>

//#include <Renderer/Debugger.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/RenderWorld.h>
#include <Renderer/Pipeline/DeferredLighting.h>
#include <Renderer/Pipeline/Shadows.h>
#include <Renderer/GPU/ShaderPrograms.hxx>


/*
--------------------------------------------------------------
	rxShadowCastingSet
--------------------------------------------------------------
*/
rxShadowCastingSet::rxShadowCastingSet()
	: m_objects(EMemHeap::HeapSceneData)
{
	m_objects.Reserve(64);
}

void rxShadowCastingSet::UpdatePerObjectConstants( const rxModel& model, mat4_carg lightViewProjection )
{
	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	GPU::p_build_hw_shadow_map::Data* pData = GPU::p_build_hw_shadow_map::cb_Data.Map(pD3DContext);
	{
		pData->lightWVP = XMMatrixMultiply( model.m_localToWorld, lightViewProjection );
	}
	GPU::p_build_hw_shadow_map::cb_Data.Unmap(pD3DContext);
}

void rxShadowCastingSet::Render( const rxShadowRenderContext& shadowRenderContext, mat4_carg lightViewProjection )
{
	shadowRenderContext.pD3D->IASetInputLayout( GPU::Vertex_P3f::layout );

	mxOPTIMIZE("render them front to back");
	const UINT numObjects = m_objects.Num();
	rxModel** objectsArray = m_objects.ToPtr();

	for( UINT iShadowCaster = 0; iShadowCaster < numObjects; iShadowCaster++ )
	{
		rxModel* pObject = objectsArray[ iShadowCaster ];

		this->UpdatePerObjectConstants( *pObject, lightViewProjection );

		pObject->RenderShadowDepth( shadowRenderContext );
	}
}

void D3D_GetShadowMapFormats(const DXGI_FORMAT inTexture2DFormat,
							 DXGI_FORMAT inDSVFormat,
							 DXGI_FORMAT inSRVFormat)
{
	switch ( inTexture2DFormat ) 
	{
	case DXGI_FORMAT_R32_TYPELESS : 
		inSRVFormat = DXGI_FORMAT_R32_FLOAT;
		inDSVFormat = DXGI_FORMAT_D32_FLOAT;
		break;

	case DXGI_FORMAT_R24G8_TYPELESS : 
		inSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		inDSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;

	case DXGI_FORMAT_R16_TYPELESS : 
		inSRVFormat = DXGI_FORMAT_R16_UNORM;
		inDSVFormat = DXGI_FORMAT_D16_UNORM;
		break;

	case DXGI_FORMAT_R8_TYPELESS : 
		inSRVFormat = DXGI_FORMAT_R8_UNORM;
		inDSVFormat = DXGI_FORMAT_R8_UNORM;
		break;

	default:
		Unreachable;
	}
}

//--------------------------------------------------------------------------------------
// This function takes the camera's projection matrix and returns the 8
// points that make up a view frustum.
// The frustum is scaled to fit within the Begin and End interval parameters.
//--------------------------------------------------------------------------------------
void CreateFrustumPointsFromCascadeInterval( F4 fCascadeIntervalBegin, 
											F4 fCascadeIntervalEnd, 
											const float4x4 &vProjection,
											float4* pvCornerPointsInViewSpace ) 
{
	XNA::Frustum vViewFrust;
	ComputeFrustumFromProjection( &vViewFrust, &vProjection );
	vViewFrust.Near = fCascadeIntervalBegin;
	vViewFrust.Far = fCascadeIntervalEnd;

	static const XMVECTORU32 vGrabY = {0x00000000,0xFFFFFFFF,0x00000000,0x00000000};
	static const XMVECTORU32 vGrabX = {0xFFFFFFFF,0x00000000,0x00000000,0x00000000};

	XMVECTORF32 vRightTop = {vViewFrust.RightSlope,vViewFrust.TopSlope,1.0f,1.0f};
	XMVECTORF32 vLeftBottom = {vViewFrust.LeftSlope,vViewFrust.BottomSlope,1.0f,1.0f};
	XMVECTORF32 vNear = {vViewFrust.Near,vViewFrust.Near,vViewFrust.Near,1.0f};
	XMVECTORF32 vFar = {vViewFrust.Far,vViewFrust.Far,vViewFrust.Far,1.0f};
	float4 vRightTopNear = XMVectorMultiply( vRightTop, vNear );
	float4 vRightTopFar = XMVectorMultiply( vRightTop, vFar );
	float4 vLeftBottomNear = XMVectorMultiply( vLeftBottom, vNear );
	float4 vLeftBottomFar = XMVectorMultiply( vLeftBottom, vFar );

	pvCornerPointsInViewSpace[0] = vRightTopNear;
	pvCornerPointsInViewSpace[1] = XMVectorSelect( vRightTopNear, vLeftBottomNear, vGrabX );
	pvCornerPointsInViewSpace[2] = vLeftBottomNear;
	pvCornerPointsInViewSpace[3] = XMVectorSelect( vRightTopNear, vLeftBottomNear,vGrabY );

	pvCornerPointsInViewSpace[4] = vRightTopFar;
	pvCornerPointsInViewSpace[5] = XMVectorSelect( vRightTopFar, vLeftBottomFar, vGrabX );
	pvCornerPointsInViewSpace[6] = vLeftBottomFar;
	pvCornerPointsInViewSpace[7] = XMVectorSelect( vRightTopFar ,vLeftBottomFar, vGrabY );
}

//--------------------------------------------------------------------------------------
// This function converts the "center, extents" version of an AABB into 8 points.
//--------------------------------------------------------------------------------------
void CreateAABBPoints( float4* vAABBPoints, FXMVECTOR vCenter, FXMVECTOR vExtents )
{
	//This map enables us to use a for loop and do vector math.
	static const XMVECTORF32 vExtentsMap[] = 
	{ 
		{1.0f, 1.0f, -1.0f, 1.0f}, 
		{-1.0f, 1.0f, -1.0f, 1.0f}, 
		{1.0f, -1.0f, -1.0f, 1.0f}, 
		{-1.0f, -1.0f, -1.0f, 1.0f}, 
		{1.0f, 1.0f, 1.0f, 1.0f}, 
		{-1.0f, 1.0f, 1.0f, 1.0f}, 
		{1.0f, -1.0f, 1.0f, 1.0f}, 
		{-1.0f, -1.0f, 1.0f, 1.0f} 
	};

	for( INT index = 0; index < 8; ++index ) 
	{
		vAABBPoints[index] = XMVectorMultiplyAdd(vExtentsMap[index], vExtents, vCenter ); 
	}
}



#if 0
// Helper for doing Parallel-Split Shadow Maps / Cascaded Shadow Maps.
struct PSSM_Util
{
	enum { NUM_CASCADES = 1, NUM_SPLITS = NUM_CASCADES };	// number of view frustum splits

	// splitDistances[0] = near clip distance, splitDistances[NUM_CASCADES] = far clip distance
	float splitDistances[ NUM_SPLITS + 1 ];

	float4x4	lightViewMatrices[ NUM_CASCADES ];
	float4x4	lightProjectionMatrices[ NUM_CASCADES ];
	float4x4	lightViewProjectionMatrices[ NUM_CASCADES ];

public:
	PSSM_Util(
		const rxRenderView& eyeView,
		const Vec3D& lightDirection,
		FLOAT maxShadowDistance = 100.0f	// max. distance at which shadows start to fade out
		)
	{
		{StaticAssert(NUM_CASCADES >= 1);}

		//FLOAT fCameraNearFarRange = eyeView.farZ - eyeView.nearZ;

		// Calculate the split distances for the view frustum splits.

		// lambda scales between between logarithmic and uniform split scheme
		const float splitDistanceLambda = 0.5f;

		// practical split scheme:
		// See:http://http.developer.nvidia.com/GPUGems3/gpugems3_ch10.html
		// CLi and CUi are the split positions in the logarithmic split scheme and the uniform split scheme.
		// CLi = n*(f/n)^(i/numsplits)
		// CUi = n + (f-n) * (i/numsplits)
		// Ci  = lerp(CLi, CUi, lambda)

		float zNear = eyeView.nearZ;
		float zFar  = maxShadowDistance;
		IndexT i;
		for (i = 0; i < NUM_CASCADES; i++)
		{
			float idm = i * (1.0f / cast(float)NUM_CASCADES);
			float distLog = zNear * mxPow(zFar / zNear, idm);
			float distUniform = zNear + (zFar - zNear) * idm;
			this->splitDistances[i] = Lerp(distLog, distUniform, splitDistanceLambda);
			//DBGOUT("%.3f\n",this->splitDistances[i]);
		}

		// clamp border values
		this->splitDistances[0] = zNear;
		this->splitDistances[NUM_CASCADES] = zFar;


		// We loop over the cascades to calculate the orthographic projection for each cascade.
		for( UINT iCascade = 0; iCascade < NUM_CASCADES; iCascade++ )
		{
			// Get world-space positions of the 8 corners of the current cascade's view frustum.
			Vec3D	frustumCornersWS[8];

			FLOAT	nearZ = splitDistances[iCascade];
			FLOAT	farZ = splitDistances[iCascade+1];

			// first calculate frustum corners in view-space
			GetViewSpaceFrustumCorners( eyeView.fovY, gfxFrontEnd.viewportAspectRatio, nearZ, farZ, frustumCornersWS );

			// transform view-space frustum corners them into world space
			for( UINT iPoint = 0; iPoint < 8; iPoint++ )
			{
				frustumCornersWS[ iPoint ] = as_matrix4(eyeView.invViewMatrix).TransformVector( frustumCornersWS[ iPoint ] );
			}

			// Find the centroid of the main camera's view frustum in world space.

			Vec3D	frustumCenterWS(0.0f);
			for( UINT iPoint = 0; iPoint < 8; iPoint++ )
			{
				frustumCenterWS += frustumCornersWS[ iPoint ];
			}
			frustumCenterWS *= (1.0f/8.0f);



			// Create a view matrix for the light, looking at the center of the view frustum.

			float4 lightTarget = vec_load1(frustumCenterWS);
			float4 lightVecWS = vec_load0(-lightDirection);	// light vector in world space
			float4 lightOrigin = vec_add( lightTarget, vec_mul( lightVecWS, 10.0f ) );	// backed up in the direction of the light

			float4x4	lightViewMatrix;
			BuildLightViewMatrix( as_vec3(lightOrigin), lightDirection, as_matrix4(lightViewMatrix) );

			// To calculate the projection, the eight points
			// that make up the view frustum are transformed into light space.
			// Next, the minimum and maximum values in X and Y are found.
			// These values make up the bounds for an orthographic projection.

			// Calculate frustum corners into the light space.
			Vec3D	frustumCornersLS[8];
			for( UINT iPoint = 0; iPoint < 8; iPoint++ )
			{
				frustumCornersLS[ iPoint ] = as_matrix4(lightViewMatrix).TransformVector( frustumCornersWS[ iPoint ] );
			}


			AABB	frustumBoundsLS;
			frustumBoundsLS.FromPoints( frustumCornersLS, 8 );

			Vec3D mins = frustumBoundsLS.GetMin();
			Vec3D maxs = frustumBoundsLS.GetMax();

			mins.z = 0.1f;

			float4x4	lightProjectionMatrix = XMMatrixOrthographicOffCenterLH(
				mins.x, maxs.x,
				mins.y, maxs.y,
				mins.z, maxs.z
			);

			lightViewMatrices[ iCascade ] = lightViewMatrix;
			lightProjectionMatrices[ iCascade ] = lightProjectionMatrix;
			lightViewProjectionMatrices[ iCascade ] = XMMatrixMultiply( lightViewMatrix, lightProjectionMatrix );
		}
	}
};
#endif




/*
--------------------------------------------------------------
	rxShadowManager
--------------------------------------------------------------
*/
rxShadowManager::rxShadowManager()
{

}

static const F4 ShadowDist = 1.0f;
static const F4 Light_Backup_Distance = 140.0f;mxUNDONE//<= compute light backup dist dynamically
static const F4 Light_Camera_Near_Clip = 1.0f;

static const F4 CascadeSplits[4] = { 0.125f, 0.25f, 0.5f, 1.0f };
//static const F4 CascadeSplits[4] = { 0.05f, 0.2f, 0.5f, 1.0f };

static const F4 Bias = 0.005f;

// size of (square) shadow map for each cascade
// 4 textures are packed into atlas as follows:
// 01
// 32
//
static const UINT nCascadeBufferSize = SHADOW_MAP_SIZE;
static const F4 fCascadeBufferSize = (F4)SHADOW_MAP_SIZE;

// actual size of shadow texture
static const UINT SHADOW_MAP_ATLAS_SIZE = SHADOW_MAP_SIZE * 2;

//@todo: atlas can hold 4 shadow maps for spot lights -> use in forward rendering?

// static variables
bool	rxShadowManager::g_cvar_spot_light_soft_shadows = true;
bool	rxShadowManager::g_cvar_dir_light_soft_shadows = true;
bool	rxShadowManager::g_cvar_dir_light_visualize_cascades = false;

UINT rxShadowManager::Static_Get_Shadow_Map_Atlas_Size()
{
	return SHADOW_MAP_ATLAS_SIZE;
}

void rxShadowManager::Initialize()
{
	mxSTATIC_ASSERT( NUM_SHADOW_CASCADES == 4 );

	mxOPTIMIZE("experiment with different shadow depth texture formats");
	const bool b32bitShadowDepthMap = true;

	// Create the shadow map as a texture atlas.

	DXGI_FORMAT	depthStencilFormat;
	if( b32bitShadowDepthMap )
	{
		depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
	}
	else
	{
		depthStencilFormat = DXGI_FORMAT_D16_UNORM;
		//DXGI_FORMAT_D24_UNORM_S8_UINT
	}

	m_shadowMap.Create( SHADOW_MAP_ATLAS_SIZE, SHADOW_MAP_ATLAS_SIZE, depthStencilFormat, true, 1, 0 );


#if MX_DEVELOPER

	DEVOUT("Shadow map format: %s\n", DXGIFormat_ToString(depthStencilFormat));

	UINT	shadowBufferSize = 0;
	shadowBufferSize += m_shadowMap.CalcSizeInBytes();
	DEVOUT("Shadow map size(%ux%u): %u KiB\n",
		(UINT)SHADOW_MAP_ATLAS_SIZE, (UINT)SHADOW_MAP_ATLAS_SIZE, shadowBufferSize/mxKIBIBYTE );

#endif //MX_DEVELOPER


#if MX_EDITOR
	gRenderer.editor->GetTextureInspector()->AddDebugItem( m_shadowMap.pTexture, "ShadowMap" );
#endif // MX_EDITOR

	HOT_BOOL(g_cvar_spot_light_soft_shadows);
	HOT_BOOL(g_cvar_dir_light_soft_shadows);
	HOT_BOOL(g_cvar_dir_light_visualize_cascades);
}

mxSWIPED("Matt Pettineo (MJP)")
// Renders meshes using cascaded shadow mapping. The technique is a basic stable CSM implementation,
// based on the article "Stable rendering of cascaded shadow maps" by Michael Valient, from ShaderX6
//
ID3D11ShaderResourceView* rxShadowManager::Process_Directional_Light(
	const rxRenderContext& context,
	const rxParallelLight& light
	)
{
#if RX_D3D_USE_PERF_HUD
	PIXEvent	renderShadowMapEvent(L"Shadow Map Rendering [Directional Light]");
#endif // RX_D3D_USE_PERF_HUD

	Assert( as_vec3(light.m_lightDirWS).IsNormalized() );

	const rxSceneContext& sceneContext = *context.s;

	ID3D11DeviceContext* pD3DContext = context.pD3D;

	// Get the current render targets and viewports
	dxSaveRenderTargetsAndViewports			rememberRenderTargetsAndDepthStencil( pD3DContext );

	// Get the current render states
	dxSaveRasterizerState		rememberRasterizerState( pD3DContext );
	dxSaveBlendState			rememberBlendState( pD3DContext );
	dxSaveDepthStencilState		rememberDepthStencilState( pD3DContext );

	// Set render states
	GPU::Build_Shadow_Map.Set( pD3DContext );

	// Set the shadow map as the depth target
	ID3D11DepthStencilView* pShadowMapDSV = m_shadowMap.pDSV;

	pD3DContext->ClearDepthStencilView( pShadowMapDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0 );

	ID3D11RenderTargetView* nullRenderTarget[1] = { nil };
	pD3DContext->OMSetRenderTargets( 0, nullRenderTarget, pShadowMapDSV );


	mxSTATIC_ASSERT( NUM_SHADOW_CASCADES == 4 );

	// Cascade offsets (shadow texture coordinates)
	const XMFLOAT2 viewportOffsets[4] =
	{
		XMFLOAT2(0.0f, 0.0f),	// top left
		XMFLOAT2(0.5f, 0.0f),	// top right
		XMFLOAT2(0.5f, 0.5f),	// bottom right
		XMFLOAT2(0.0f, 0.5f)	// bottom left
	};

	// Render the meshes to each cascade
	for( UINT iCascadeIndex = 0; iCascadeIndex < NUM_SHADOW_CASCADES; ++iCascadeIndex )
	{
#if RX_D3D_USE_PERF_HUD
		PIXEvent	genShadowCascadeMapEvent(
			iCascadeIndex == 0 ? L"Cascade 0" :
			iCascadeIndex == 1 ? L"Cascade 1" :
			iCascadeIndex == 2 ? L"Cascade 2" :
			iCascadeIndex == 3 ? L"Cascade 3" :
			L"Rendering cascade N"
			);
#endif // RX_D3D_USE_PERF_HUD



		// Set viewport
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX	= viewportOffsets[ iCascadeIndex ].x * fCascadeBufferSize * 2.0f;
		viewport.TopLeftY	= viewportOffsets[ iCascadeIndex ].y * fCascadeBufferSize * 2.0f;
		viewport.Width		= fCascadeBufferSize;
		viewport.Height		= fCascadeBufferSize;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		pD3DContext->RSSetViewports( 1, &viewport );

		// Get the 8 points of the view frustum in world space

		// first get the view frustum corners in projection space
		float4 frustumCornersWS[8] =
		{
			// near plane
			XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),
			XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),
			XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f),
			XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),

			// far plane
			XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f),
			XMVectorSet( 1.0f,  1.0f, 1.0f, 1.0f),
			XMVectorSet( 1.0f, -1.0f, 1.0f, 1.0f),
			XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
		};

		const F4 prevSplitDist = (iCascadeIndex == 0)
			?
			0.0f
			:
			CascadeSplits[ iCascadeIndex - 1 ] * ShadowDist
			;

		const F4 splitDist = CascadeSplits[ iCascadeIndex ] * ShadowDist;

		const float4x4 invViewProj = sceneContext.invViewProjectionMatrix;
		for(UINT i = 0; i < 8; ++i)
		{
			frustumCornersWS[i] = XMVector3TransformCoord( frustumCornersWS[i], invViewProj );
		}

		// Scale by the shadow view distance
		for(UINT i = 0; i < 4; ++i)
		{
			float4 cornerRay = XMVectorSubtract(frustumCornersWS[i + 4], frustumCornersWS[i]);
			float4 nearCornerRay = XMVectorScale(cornerRay, prevSplitDist);
			float4 farCornerRay = XMVectorScale(cornerRay, splitDist);
			frustumCornersWS[i + 4] = XMVectorAdd(frustumCornersWS[i], farCornerRay);
			frustumCornersWS[i] = XMVectorAdd(frustumCornersWS[i], nearCornerRay);
		}

		// Calculate the centroid of the view frustum
		float4 sphereCenterPosition = XMVectorZero();
		for(UINT i = 0; i < 8; ++i)
		{
			sphereCenterPosition = XMVectorAdd( sphereCenterPosition, frustumCornersWS[i] );
		}
		sphereCenterPosition = XMVectorScale( sphereCenterPosition, 1.0f / 8.0f );

		// Calculate the radius of a bounding sphere
		float4 sphereRadiusVec = XMVectorZero();
		for(UINT i = 0; i < 8; ++i)
		{
			float4 dist = XMVector3Length(XMVectorSubtract( frustumCornersWS[i], sphereCenterPosition) );
			sphereRadiusVec = XMVectorMax( sphereRadiusVec, dist );
		}

		sphereRadiusVec = XMVectorRound( sphereRadiusVec );
		const F4 sphereRadius = XMVectorGetX( sphereRadiusVec );
		const F4 backupDist = sphereRadius + Light_Camera_Near_Clip + Light_Backup_Distance;

		// Get position of the shadow camera
		float4 shadowCameraPosition = sphereCenterPosition;
		float4 backupDirVec = XMVectorNegate( light.m_lightDirWS );
		backupDirVec = XMVectorScale( backupDirVec, backupDist );
		shadowCameraPosition = XMVectorAdd( shadowCameraPosition, backupDirVec );

		const float4 shadowCameraLookDirection = XMVector3Normalize( XMVectorSubtract( sphereCenterPosition, shadowCameraPosition ) );
		const float4 upDirVec = BuildBasisVectors( shadowCameraPosition, shadowCameraLookDirection ).r[1];

		// Come up with a new orthographic camera for the shadow caster
		OrthographicCamera shadowCamera(
			shadowCameraPosition, sphereCenterPosition, upDirVec,
			-sphereRadius, -sphereRadius,
			+sphereRadius, +sphereRadius,
			Light_Camera_Near_Clip, backupDist + sphereRadius
		);

		// Create the rounding matrix, by projecting the world-space origin and determining
		// the fractional offset in texel space
		float4x4 shadowViewProjMatrix = shadowCamera.m_viewProjection;

		float4 shadowOrigin = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
		shadowOrigin = XMVector4Transform( shadowOrigin, shadowViewProjMatrix );
		shadowOrigin = XMVectorScale( shadowOrigin, fCascadeBufferSize / 2.0f );

		float4 roundedOrigin = XMVectorRound( shadowOrigin );
		float4 roundOffset = XMVectorSubtract( roundedOrigin, shadowOrigin );
		roundOffset = XMVectorScale( roundOffset, 2.0f / fCascadeBufferSize );
		roundOffset = XMVectorSetZ( roundOffset, 0.0f);
		roundOffset = XMVectorSetW( roundOffset, 0.0f);

		float4x4 shadowProjMatrix = shadowCamera.m_projection;
		shadowProjMatrix.r[3] = XMVectorAdd( shadowProjMatrix.r[3], roundOffset );
		shadowCamera.UpdateProjectionMatrix( shadowProjMatrix );
		shadowViewProjMatrix = shadowCamera.m_viewProjection;


		// Draw the objects with depth only, using the new shadow camera
		{
			rxShadowRenderContext	shadowRenderContext;
			shadowRenderContext.s = &sceneContext;
			shadowRenderContext.pD3D = pD3DContext;
mxTODO("use dir light frustum");
			m_shadowCasters.m_objects.Empty();
			sceneContext.scene.rfGetDirLightShadowCasters( m_shadowCasters );

			rxSET_SHADER_SCOPED( p_build_hw_shadow_map, pD3DContext );

			m_shadowCasters.Render( shadowRenderContext, shadowCamera.m_viewProjection );
		}



		// Apply the scale/offset/bias matrix, which transforms from [-1,1]
		// post-projection space to [0,1] UV space
		const float4x4 texScaleBias = Matrix4x4_Build_Texture_Scale_Bias( Bias );

		shadowViewProjMatrix = XMMatrixMultiply( shadowViewProjMatrix, texScaleBias );

		// Apply the cascade offset/scale matrix, which applies the offset and scale needed to
		// convert the UV coordinate into the proper coordinate for the cascade being sampled in
		// the atlas.
		XMFLOAT4 offset( viewportOffsets[iCascadeIndex].x, viewportOffsets[iCascadeIndex].y, 0.0f, 1.0 );
		float4x4 cascadeOffsetMatrix = XMMatrixScaling( 0.5f, 0.5f, 1.0f );
		cascadeOffsetMatrix.r[3] = XMLoadFloat4(&offset);
		shadowViewProjMatrix = XMMatrixMultiply( shadowViewProjMatrix, cascadeOffsetMatrix );

		// multiply by inverse view matrix, because we do our lighting calculations in view space
		shadowViewProjMatrix = XMMatrixMultiply( sceneContext.invViewMatrix, shadowViewProjMatrix );

		// Store the shadow matrix
		m_shadowMatrices[iCascadeIndex] = shadowViewProjMatrix;

		// Store the split distance in terms of view space depth
		const F4 clipDist = sceneContext.farZ - sceneContext.nearZ;
		m_cascadeSplits[iCascadeIndex] = sceneContext.nearZ + splitDist * clipDist;
	}

	return m_shadowMap.pSRV;
}

ID3D11ShaderResourceView* rxShadowManager::Prepare_ShadowMap_for_SpotLight(
	const rxRenderContext& context,
	const rxLocalLight& light,
	const rxViewFrustum& lightFrustum,
	mat4_carg lightViewProjection
	)
{
#if RX_D3D_USE_PERF_HUD
	PIXEvent	renderShadowMapEvent(L"Shadow Map Rendering [Spot Light]");
#endif // RX_D3D_USE_PERF_HUD

	const rxSceneContext* pSceneContext = context.s;
	rxRenderWorld& renderWorld = pSceneContext->scene;

	ID3D11DeviceContext* pD3DContext = context.pD3D;

	rxScopedShadowMapSetter	bindShadowMap( pD3DContext, m_shadowMap.pDSV, SHADOW_MAP_ATLAS_SIZE );

	// Set render states
	GPU::Build_Shadow_Map.Set( pD3DContext );

	// Draw the objects with depth only, using the shadow camera
	{
		rxShadowRenderContext	shadowRenderContext;
		shadowRenderContext.s = pSceneContext;
		shadowRenderContext.pD3D = pD3DContext;

		m_shadowCasters.m_objects.Empty();
		renderWorld.rfGetSpotLightShadowCasters( lightFrustum, m_shadowCasters );

		rxSET_SHADER_SCOPED( p_build_hw_shadow_map, pD3DContext );

		m_shadowCasters.Render( shadowRenderContext, lightViewProjection );
	}

	return m_shadowMap.pSRV;
}


rxScopedShadowMapSetter::rxScopedShadowMapSetter( ID3D11DeviceContext* pD3DContext, ID3D11DepthStencilView* pShadowMapDSV, UINT shadowMapSquareSize )
	: rememberRenderTargetsAndDepthStencil( pD3DContext )
	, rememberRasterizerState( pD3DContext )
	, rememberBlendState( pD3DContext )
	, rememberDepthStencilState( pD3DContext )
{
	// Set render states.
	GPU::Build_Shadow_Map.Set( pD3DContext );

	// Clear the shadow map.
	pD3DContext->ClearDepthStencilView( pShadowMapDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0 );

	// Set the shadow map as the depth target.
	ID3D11RenderTargetView* nullRenderTarget = nil;
	pD3DContext->OMSetRenderTargets( 1, &nullRenderTarget, pShadowMapDSV );

	// Set viewport
	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = shadowMapSquareSize;
		viewport.Height = shadowMapSquareSize;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
	}
	pD3DContext->RSSetViewports( 1, &viewport );
}

rxScopedShadowMapSetter::~rxScopedShadowMapSetter()
{

}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
