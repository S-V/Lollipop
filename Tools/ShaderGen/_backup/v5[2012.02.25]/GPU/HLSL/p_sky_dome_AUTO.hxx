// p_sky_dome_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_sky_shaders.fx' (8)
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

cbuffer Data : register(b2)
{
	float4x4 skyDomeTransform;
};
SamplerState skyTextureSampler : register(s14);
Texture2D skyTexture : register(t0);

struct VS_IN
{
	float3	position : Position;
	float2	texCoord : TexCoord;
};
struct VS_OUT
{
	float4 position : SV_Position;
	float2 texCoord : TexCoord;
	//float4 color : Color;
};

void VS_Main( in VS_IN IN, out VS_OUT OUT )
{
	OUT.position = mul( float4( IN.position, 1 ), skyDomeTransform );
	OUT.position.w = OUT.position.z;

	OUT.texCoord = IN.texCoord;
	//OUT.color = IN.color;
}

float4 PS_Main( in VS_OUT IN ) : SV_Target
{
	return skyTexture.Sample( skyTextureSampler, IN.texCoord ).rgba;
}

