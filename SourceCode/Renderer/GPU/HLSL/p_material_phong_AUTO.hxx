// p_material_phong_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_materials.fx' (75)
// Defines:
//	bHasBaseMap = 0
//	bHasNormalMap = 0
//	bHasSpecularMap = 0
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
	float4 materialDiffuseColor;
	float4 materialSpecularColor;
	float4 materialEmissiveColor;
};
Texture2D baseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position;
	float3	normalVS : Normal0;

	float2	uv : TexCoord0;

#if bHasNormalMap
	float3  tangentVS : Tangent;	// view-space tangent
	float3  bitangentVS : Binormal;	// view-space binormal
#endif
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;

	float3 N = UnpackVertexNormal( IN.normal.xyz );
	N = Transform_Dir_ObjectToView( N );
	OUT.normalVS = N;

	OUT.uv = IN.texCoords;

#if bHasNormalMap
	float3 T = UnpackVertexNormal( IN.tangent.xyz );
	T = Transform_Dir_ObjectToView( T );
	T = normalize( T - dot( T, N ) * N );
	float3 B = cross( N, T );

	OUT.tangentVS = T;
	OUT.bitangentVS = B;
#endif
}

void PS_Main( in VSOut IN, out PS_Out_GBuffer OUT )
{
	GBufferInput		s;

	s.position = IN.positionVS;


#if bHasNormalMap
	float3 normalT = normalMap.Sample( colorMapSampler, IN.uv ).rgb;
	normalT = ExpandNormal(normalT);

	float3 bumpedNormal = IN.tangentVS * normalT.x
						+ IN.bitangentVS * normalT.y
						+ IN.normalVS * normalT.z;
	bumpedNormal = normalize( bumpedNormal );

	s.normal = bumpedNormal;
#else
	s.normal = normalize( IN.normalVS );
#endif


#if bHasBaseMap
	s.albedo = baseMap.Sample( colorMapSampler, IN.uv ).rgb;
#else
	s.albedo = materialDiffuseColor.rgb;
#endif


	s.specularAmount = materialDiffuseColor.a;
	s.specularPower = materialSpecularColor.a;	// normalized, in range [0..1]

	PackSurfaceData( s, OUT );
}

