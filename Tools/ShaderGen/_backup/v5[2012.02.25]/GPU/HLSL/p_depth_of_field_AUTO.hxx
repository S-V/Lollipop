// p_depth_of_field_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (208)
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
Texture2D RT_normal_specMul : register(t0);
Texture2D RT_diffuse_specExp : register(t1);
Texture2D RT_linearDepth : register(t2);

#define RX_GBUFFER_READS_ENABLED
#include "h_geometry_buffer.h"

Texture2D sceneColorTexture : register(t3);
Texture2D sceneDepthTexture : register(t4);
Texture2D blurredSceneTexture : register(t5);

#include "h_screen_shader.h"
#include "h_post_processing.h"

VS_ScreenOutput VS_Main( in uint vertexID: SV_VertexID )
{
	VS_ScreenOutput output;
	GetFullScreenTrianglePosTexCoord( vertexID, output.position, output.texCoord );
	return output;
}

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	return PS_DOF_Comp( input.texCoord, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );
}

