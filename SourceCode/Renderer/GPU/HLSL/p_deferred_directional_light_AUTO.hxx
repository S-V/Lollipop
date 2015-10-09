// p_deferred_directional_light_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_deferred_lighting.fx' (13)
// Defines:
//	bLight_CastShadows = 0
//	bEnableSoftShadows = 1
//	bVisualizeCascades = 0
cbuffer PerFrame : register(b0)
{
	float4 screenSize_invSize;
	float4 time_invTime_deltaTime_invDeltaTime;
	float4 DoF_params;
	float4 midGray_whiteLum_exposure_bloomScale;
	float4 sssParams;
};
SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState anisotropicSampler : register(s2);
SamplerState pointClampSampler : register(s3);
SamplerState linearClampSampler : register(s4);
SamplerState colorMapSampler : register(s5);
SamplerState detailMapSampler : register(s6);
SamplerState normalMapSampler : register(s7);
SamplerState specularMapSampler : register(s8);
SamplerState attenuationSampler : register(s9);
SamplerState cubeMapSampler : register(s10);
SamplerComparisonState shadowMapSampler : register(s11);
SamplerComparisonState shadowMapPCFSampler : register(s12);
SamplerComparisonState shadowMapPCFBilinearSampler : register(s13);

#include "h_base.h"

// returns render target size (backbuffer dimensions)
inline float2 GetViewportSize() { return screenSize_invSize.xy; }
// returns texel size
inline float2 GetInvViewportSize() { return screenSize_invSize.zw; }

inline float GetGlobalTime() { return time_invTime_deltaTime_invDeltaTime.x; }
inline float GetInvGlobalTime() { return time_invTime_deltaTime_invDeltaTime.y; }
inline float GetDeltaTime() { return time_invTime_deltaTime_invDeltaTime.z; }
inline float GetInvDeltaTime() { return time_invTime_deltaTime_invDeltaTime.w; }

#define g_fFocalPlaneDistance		DoF_params.x
#define g_fNearBlurPlaneDistance	DoF_params.y
#define g_fFarBlurPlaneDistance		DoF_params.z
#define g_fFarBlurLimit				DoF_params.w

#define g_fMidGray				midGray_whiteLum_exposure_bloomScale.x
#define g_fWhiteLum				midGray_whiteLum_exposure_bloomScale.y
#define g_fExposure				midGray_whiteLum_exposure_bloomScale.z
#define g_fBloomScale			midGray_whiteLum_exposure_bloomScale.w

cbuffer PerView : register(b1)
{
	float4x4 viewMatrix;
	float4x4 viewProjectionMatrix;
	float4x4 inverseViewMatrix;
	float4x4 projectionMatrix;
	float4x4 inverseProjectionMatrix;
	float4 tanHalfFoV;
	float4 depthClipPlanes;
	float4 frustumCornerVS_FarTopLeft;
	float4 frustumCornerVS_FarTopRight;
	float4 frustumCornerVS_FarBottomLeft;
	float4 frustumCornerVS_FarBottomRight;
};

#define PER_VIEW_CONSTANTS_DEFINED
#include "h_base.h"

float3 GetCameraRightVector() { return inverseViewMatrix[0].rgb; }
float3 GetCameraUpVector() { return inverseViewMatrix[1].rgb; }
float3 GetLookDirection() { return inverseViewMatrix[2].rgb; }
// returns camera position in world space
float3 GetEyePositionWS() { return inverseViewMatrix[3].rgb; }

float GetNearZ() { return depthClipPlanes.x; }
float GetFarZ() { return depthClipPlanes.y; }
float GetInverseNearZ() { return depthClipPlanes.z; }
float GetInverseFarZ() { return depthClipPlanes.w; }

// distanceToProjectionWindow = 1.0 / tan(0.5 * radians(FoVy));
#define gDistanceToProjectionWindow		tanHalfFoV.z

Texture2D RT_normal_specMul : register(t0);
Texture2D RT_diffuse_specExp : register(t1);
Texture2D RT_linearDepth : register(t2);

#define RX_GBUFFER_READS_ENABLED
#include "h_geometry_buffer.h"

cbuffer Data : register(b2)
{
	float4 lightVectorVS;
	float4 lightDiffuseColor;
	float4 lightSpecularColor;
	float4x4 shadowMatrices[ NUM_SHADOW_CASCADES ];
	float4 cascadeSplits;
};
Texture2D shadowDepthMap : register(t3);

#include "h_screen_shader.h"
#include "h_shade_lib.h"

#if bLight_CastShadows

//--------------------------------------------------------------------------------------
// Samples the shadow map cascades based on the pixel position, using edge-tap
// smoothing PCF for filtering
//--------------------------------------------------------------------------------------
float SampleShadowCascade( in float3 position, in uint cascadeIndex )
{
	float4x4 shadowMatrix = shadowMatrices[ cascadeIndex ];
	float3 shadowPosition = mul( float4(position, 1.0f), shadowMatrix ).xyz;
	float2 shadowTexCoord = shadowPosition.xy;
	float shadowDepth = shadowPosition.z;

	float shadowVisibility;

#if bEnableSoftShadows

	// Edge tap smoothing

	//const int filterSize = 3;
	//Radius = (filterSize - 1.0f) / 2.0f;

	const int Radius = 2;
	const float SHADOW_MAP_DBL_SIZE = SHADOW_MAP_SIZE * 2;
	const int NumSamples = (Radius * 2 + 1) * (Radius * 2 + 1);

	float2 fracs = frac( shadowTexCoord.xy * SHADOW_MAP_DBL_SIZE );
	float leftEdge = 1.0f - fracs.x;
	float rightEdge = fracs.x;
	float topEdge = 1.0f - fracs.y;
	float bottomEdge = fracs.y;

	shadowVisibility = 0.0f;

	[unroll(NumSamples)]
	for( int y = -Radius; y <= Radius; y++ )
	{
		[unroll(NumSamples)]
		for( int x = -Radius; x <= Radius; x++ )
		{
			float2 offset = float2(x, y) * (1.0f / SHADOW_MAP_DBL_SIZE);
			float2 sampleCoord = shadowTexCoord + offset;

			// NOTE: don't use hardware PCF filtering here
			float sample = shadowDepthMap.SampleCmp( shadowMapSampler, sampleCoord, shadowDepth ).x;

			float xWeight = 1;
			float yWeight = 1;

			if( x == -Radius ) {
				xWeight = leftEdge;
			}
			else if(x == Radius) {
				xWeight = rightEdge;
			}

			if(y == -Radius) {
				yWeight = topEdge;
			}
			else if(y == Radius) {
				yWeight = bottomEdge;
			}

			shadowVisibility += sample * xWeight * yWeight;
		}
	}

	shadowVisibility  /= NumSamples;
	shadowVisibility *= 1.55f;

#else

	// cheap hardware bilinear PCF

	shadowVisibility = 0.0f;

	const float comparisonValue = shadowDepth;
	shadowVisibility = shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, shadowTexCoord, comparisonValue ).x;

#endif // bEnableSoftShadows

	return shadowVisibility;
}

//--------------------------------------------------------------------------------------
// Computes the visibility term by performing the shadow test
//--------------------------------------------------------------------------------------
float3 ShadowVisibility( in float3 position, in float depthVS )
{
	float3 shadowVisibility = 1.0f;
	uint cascadeIndex = 0;

	// Figure out which cascade to sample from
	[unroll]
	for( uint i = 0; i < NUM_SHADOW_CASCADES - 1; ++i )
	{
		[flatten]
		if(depthVS > cascadeSplits[i]) {
			cascadeIndex = i + 1;
		}
	}

	shadowVisibility = SampleShadowCascade( position, cascadeIndex );

	// Sample the next cascade, and blend between the two results to
	// smooth the transition
	const float BlendThreshold = 0.1f;
	float nextSplit = cascadeSplits[ cascadeIndex ];
	float splitSize = i == 0 ? nextSplit : nextSplit - cascadeSplits[ cascadeIndex - 1 ];
	float splitDist = (nextSplit - depthVS) / splitSize;

	float nextSplitVisibility = SampleShadowCascade( position, cascadeIndex + 1 );
	float lerpAmount = smoothstep( 0.0f, BlendThreshold, splitDist );
	shadowVisibility = lerp( nextSplitVisibility, shadowVisibility, lerpAmount );

	return shadowVisibility;
}

float3 GetCascadeColor( in float depthVS )
{
	uint cascadeIndex = 0;

	[unroll]
	for( uint i = 0; i < NUM_SHADOW_CASCADES - 1; ++i ) {
		[flatten]
		if( depthVS > cascadeSplits[i] ) {
			cascadeIndex = i + 1;
		}
	}

	const float3 cascadeColors[ NUM_SHADOW_CASCADES ] = {
		float3( 1, 0, 0 ),
		float3( 0, 1, 0 ),
		float3( 0, 0, 1 ),
		float3( 1, 1, 0 ),
	};

	return cascadeColors[ cascadeIndex ];
}

#endif // bLight_CastShadows


struct PSOut
{
	float4	color : SV_Target0;
};

void PS_Main( in VS_ScreenQuadOutput IN, out PSOut OUT )
{
	GSurface	surface;
	ReadSurfaceData( IN.texCoord, surface );

	const float pixelDepth = SamplePixelDepth( IN.texCoord );
	const float3 positionVS  = IN.eyeRayVS * pixelDepth;
	const float3 normalVS  = surface.normal;

	const float3 eyeDirVS = normalize( positionVS );
	const float3 H = normalize( eyeDirVS + lightVectorVS.xyz );

	const float NdotL = dot( normalVS, lightVectorVS.xyz );

	float3 totalColor = surface.albedo * NdotL * lightDiffuseColor.rgb;

	// @todo: specular:
/*
	const float k = pow( saturate( dot(normalVS, H) ), surface.specularPower );

	const float3 specularColor = surface.albedo * k * specularAmount;
*/
	totalColor *= surface.specularAmount;


#if bLight_CastShadows
	float depthVS = UnpackDepth( pixelDepth );
	//const float3 positionWS = mul( float4(positionVS,1), inverseViewMatrix ).xyz;

	#if bVisualizeCascades
		totalColor *= GetCascadeColor( depthVS );
	#else
		float3 shadowVisibility = ShadowVisibility( positionVS, depthVS );
		totalColor *= shadowVisibility;
	#endif // bVisualizeCascades

#endif // bLight_CastShadows

	OUT.color = float4( totalColor, 1 );
}


