// p_skin_shader_forward_pass_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_skin_shaders.fx' (173)
// Defines:
//	bEnableSSS = 0
//	bUseNormalMap = 0
//	bUseSpecularMap = 0
//	bEnableShadows = 0
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

cbuffer PerObject : register(b2)
{
	float4x4 worldMatrix;
	float4x4 worldViewMatrix;
	float4x4 worldViewProjectionMatrix;
};

#define PER_OBJECT_CONSTANTS_DEFINED
#include "h_transform.h"

cbuffer Data : register(b3)
{
	float4 lightPos_Radius;
	float4 lightDir_InvRadius;
	float4 lightDiffuseColor;
	float4 lightSpecularParams;
	float4 lightSpotParams;
	float4x4 lightViewProjection;
	float4 lightShadowParams;
};
Texture2D baseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D shadowDepthMap : register(t3);
Texture2D beckmannTexture : register(t4);

#define lightPosition	lightPos_Radius.xyz
#define lightDirection	lightDir_InvRadius.xyz
#define lightRadius		lightPos_Radius.w
#define lightInvRadius	lightDir_InvRadius.w
#define shadowMapBias		lightShadowParams.x
#define invShadowMapSize	lightShadowParams.y

#define lightSpecularFresnel	lightSpecularParams.x
#define lightSpecularIntensity	lightSpecularParams.y
#define lightSpecularRoughness	lightSpecularParams.z

#define lightAttenuation		lightSpotParams.x
#define lightFalloffStart		lightSpotParams.y
#define lightInvFalloffWidth	lightSpotParams.z


#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"
#include "h_separable_sss.h"
#include "h_shade_lib.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position0;
	float3	positionWS : Position1;
	float3	normalWS : Normal0;

	float2	uv : TexCoord0;

	float3 viewVectorWS : TexCoord1;

#if bUseNormalMap
	float3  tangentWS : Tangent;	// view-space tangent
	float3  bitangentWS : Binormal;	// view-space binormal
#endif
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;
	OUT.positionWS = Transform_ObjectToWorld( posLocal ).xyz;

	float3 N = UnpackVertexNormal( IN.normal.xyz );
	N = Transform_Dir_ObjectToWorld( N );
	OUT.normalWS = N;

	OUT.uv = IN.texCoords;

	OUT.viewVectorWS = GetEyePositionWS() - OUT.positionWS;

#if bUseNormalMap
	float3 T = UnpackVertexNormal( IN.tangent.xyz );
	T = Transform_Dir_ObjectToWorld( T );
	T = normalize( T - dot( T, N ) * N );
	float3 B = cross( N, T );

	OUT.tangentWS = T;
	OUT.bitangentWS = B;
#endif
}

float SpecularKSK( Texture2D beckmannTex, float3 normal, float3 light, float3 view, float roughness )
{
	float3 half = view + light;
	float3 halfn = normalize(half);

	float ndotl = max(dot(normal, light), 0.0);
	float ndoth = max(dot(normal, halfn), 0.0);

	float sample = beckmannTex.SampleLevel( linearSampler, float2(ndoth, roughness), 0 ).r;

	// prevent error X3571: pow(f, e) will not work for negative f, use abs(f) or conditionally handle negative values if you expect them
	sample = abs(sample);

	float ph = pow(2.0 * sample, 10.0);
	float f = lerp(0.25, Fresnel(halfn, view, 0.028), lightSpecularFresnel);
	float ksk = max(ph * f / dot(half, half), 0.0);

	return ndotl * ksk;   
}

float Shadow( float3 worldPosition )
{
	float4 shadowPosition = mul( float4(worldPosition, 1.0), lightViewProjection );
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += shadowMapBias;
	return shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, shadowPosition.xy, shadowPosition.z * lightInvRadius ).r;
}
float ShadowPCF( float3 worldPosition, uint numSamples, float width )
{
	float4 shadowPosition = mul( float4(worldPosition, 1.0), lightViewProjection );
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += shadowMapBias;

	float shadow = 0.0;
	float offset = (numSamples - 1.0) / 2.0;
	[unroll]
	for( float x = -offset; x <= offset; x += 1.0 )
	{
		[unroll]
		for( float y = -offset; y <= offset; y += 1.0 )
		{
			float2 pos = shadowPosition.xy + width * float2( x, y ) * invShadowMapSize;
			shadow += shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, pos, shadowPosition.z * lightInvRadius ).r;
		}
	}
	shadow /= numSamples * numSamples;
	return shadow;
}

float3 PS_Main( in VSOut IN ) : SV_Target0
{
	float3	color = (float3)0;

	IN.viewVectorWS = normalize(IN.viewVectorWS);

	const float3 eyePosition = IN.positionWS;
	const float3 eyeVector = IN.viewVectorWS;

	float3	N;

#if bUseNormalMap
	// take .gr channels, see iryoku-separable-sss-f61b44c (3Dc format)
	float3 normalT;
	normalT.xy = 2 * normalMap.Sample( normalMapSampler, IN.uv ).gr - 1;
	normalT.z =  sqrt( 1.0 - normalT.x * normalT.x - normalT.y * normalT.y );
	normalT = normalize( normalT );

	float3 bumpedNormal = IN.tangentWS * normalT.x
						+ IN.bitangentWS * normalT.y
						+ IN.normalWS * normalT.z;
	bumpedNormal = normalize( bumpedNormal );
	N = bumpedNormal;
#else
	N = normalize( IN.normalWS );
#endif

	const float3 albedo = baseMap.Sample( colorMapSampler, IN.uv ).rgb;

	float3 L = lightPosition - eyePosition;
	float dist = length(L);
	L /= dist;

	float spot = dot( lightDirection, -L );

	// And the spot light falloff:
	spot = saturate( (spot - lightFalloffStart) * lightInvFalloffWidth );

	// Calculate attenuation:
	float curve = min(pow(dist * lightInvRadius, 6.0), 1.0);
	float attenuation = lerp(1.0 / (1.0 + lightAttenuation * dist * dist), 0.0, curve);
	attenuation *= 2;

	// Calculate some terms we will use later on:
	float3 f1 = lightDiffuseColor.rgb * attenuation * spot;
	float3 f2 = albedo.rgb * f1;

	// Calculate the diffuse and specular lighting:
	float3 diffuse = saturate(dot(L, N));

	
	// Calculate specular lighting:

#if bUseSpecularMap
	// Fetch albedo, specular parameters and static ambient occlusion:
	float3 specularAO = specularMap.Sample( specularMapSampler, IN.uv ).rgb;

	const float occlusion = specularAO.b;
	const float intensity = specularAO.r * lightSpecularIntensity;
	const float roughness = (specularAO.g / 0.3) * lightSpecularRoughness;

	const float specular = intensity * SpecularKSK( beckmannTexture, N, L, eyeVector, roughness );
#else
	const float occlusion = 1.0f;
	const float specular = 0.0f;
#endif

	
#if bEnableSSS	
	//const float translucency = 0.83;
	//const float sssWidth = 0.012;
	
	// too reddish
	//const float translucency = 0.9;
	//const float sssWidth = 0.025;

	//const float translucency = 0.87;
	//const float sssWidth = 0.015;

	const float translucency = 0.9;
	//const float sssWidth = 0.02;
	const float sssWidth = 0.01;

	// Add the transmittance component:
	if(1)
	{	
		color.rgb += f2 * SSSSTransmittance(
			translucency, sssWidth,
			eyePosition, N,
			L, shadowDepthMap, lightViewProjection, lightRadius
		);
	}
#endif


#if bEnableShadows
	// And also the shadowing:
	//const float shadow = Shadow( eyePosition );
	const float shadow = ShadowPCF( eyePosition, 3, 1.0 );
#else
	const float shadow = 1.0f;
#endif

	// Add the diffuse and specular components:
	color.rgb += shadow * (f2 * diffuse + f1 * specular);

	// Add the ambient component:	
	const float ambient = 0.4f;
	color.rgb += occlusion * ambient * albedo.rgb;// * irradianceTex.Sample(linearSampler, N).rgb;
	
	return color;
}

