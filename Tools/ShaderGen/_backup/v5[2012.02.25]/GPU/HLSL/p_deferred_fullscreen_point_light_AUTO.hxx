// p_deferred_fullscreen_point_light_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_deferred_lighting.fx' (246)
// Defines:
//	bLight_EnableSpecular = 0
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
cbuffer PerLocalLight : register(b3)
{
	float4 lightDiffuseColor;
	float4 lightPosition;
	float4 lightRadiusInvRadius;
	float4 lightSpecularColor;
	float4 spotLightDirection;
	float4 spotLightAngles;
	float4x4 lightShapeTransform;
	float4x4 eyeToLightProjection;
	float lightProjectorIntensity;
	float lightShadowDepthBias;
};




#include "h_screen_shader.h"
#include "h_shade_lib.h"
#include "h_deferred_lighting.h"

void PS_Main( in VS_ScreenQuadOutput IN, out PSOut OUT )
{
	GSurface	surface;
	float3		positionVS;
	PS_ScreenQuad_ReadPixelAttributes( IN, surface, positionVS );

	float3 outputColor = 0.0f;
	outputColor = CalcPointLight( surface, positionVS );

	OUT.color = float4( outputColor, 1.0f );
}

