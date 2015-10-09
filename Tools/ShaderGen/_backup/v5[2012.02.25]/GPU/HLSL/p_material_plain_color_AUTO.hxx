// p_material_plain_color_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_materials.fx' (13)
cbuffer PerFrame : register(b0)
{
	float4 screenSize_invSize;
	float1 globalTimeInSeconds;
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
float2 GetViewportSize() { return screenSize_invSize.xy; }
// returns texel size
float2 GetInvViewportSize() { return screenSize_invSize.zw; }
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
};

#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position;
	float3	normalVS : Normal0;
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;

	float3 normalOS = UnpackVertexNormal( IN.normal.xyz );
	OUT.normalVS = Transform_Dir_ObjectToView( normalOS );
}

void PS_Main( in VSOut IN, out PS_Out_GBuffer OUT )
{
	GBufferInput		s;

	s.position = IN.positionVS;
	s.normal = normalize(IN.normalVS);		// view-space normal

	s.albedo = materialDiffuseColor.rgb;

	s.specularAmount = materialDiffuseColor.a;
	s.specularPower = materialSpecularColor.a;	// normalized, in range [0..1]

	PackSurfaceData( s, OUT );
}

