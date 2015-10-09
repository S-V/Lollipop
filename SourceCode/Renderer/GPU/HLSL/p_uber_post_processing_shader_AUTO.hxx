// p_uber_post_processing_shader_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (852)
// Defines:
//	bEnable_Bloom = 1
//	bEnable_HDR = 1
//	bEnable_DOF = 1
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

Texture2D sceneColorTexture : register(t3);
Texture2D sceneDepthTexture : register(t4);
Texture2D blurredSceneTexture : register(t5);
Texture2D averageLuminanceTexture : register(t6);
Texture2D bloomTexture : register(t7);

#include "h_screen_shader.h"
#include "h_post_processing.h"

//Perceptual Effects in Real-time Tone Mapping: Equ(11)
float AutoKey( in float avgLum )
{
	const float minKey = 0.1;
	return saturate(1.8 - 1.8/(avgLum*minKey+1)) + minKey;
}
float GetAverageLuminance()
{
	float averageLuminance = SampleAverageLuminance( averageLuminanceTexture );
//averageLuminance = 0.5;
	averageLuminance = clamp( averageLuminance, 0.18, 1.0 );
	return averageLuminance;
}
float4 PS_Main( in VS_ScreenQuadOutput input ) : SV_Target
{
	//return sceneColorTexture.Sample( pointSampler, input.texCoord );
	//return PS_DOF_Comp( input.texCoord, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );

	const float3 srcHdrColor = 
#if bEnable_DOF
		PS_DOF_Comp( input.texCoord, sceneColorTexture, blurredSceneTexture, sceneDepthTexture ).rgb;
#else
		sceneColorTexture.Sample( pointSampler, input.texCoord ).rgb;
#endif

	const float averageLuminance = GetAverageLuminance();

#if bEnable_Bloom
	const float3 bloomColor = bloomTexture.Sample( linearSampler, input.texCoord ).rgb;
#endif

	float3 o = srcHdrColor;

	// ToneMaping

	const float Key = 1.0;

	// Map the high range of color values into a range appropriate for
	// display, taking into account the user's adaptation level, and selected
	// values for for middle gray and white cutoff.

#if 0
	// Reinhard's Photographic Tone Reproduction Operator
	o.rgb *= Key/(averageLuminance + 0.001f);
	o.rgb = o.rgb / (o.rgb + 1) ;
#endif


#if 1
	// Modified Reinhard's Photographic Tone Reproduction Operator
	static const float3 WhiteLuminance = (float3)3.5;
	float3 scaledlum = o.rgb * Key/(averageLuminance + 0.0001f);
	o.rgb = scaledlum*(1 + scaledlum/(WhiteLuminance*WhiteLuminance))/(1.0+scaledlum); 
#endif

	// saturation
#if 1
	// 1.0=full saturation, 0.0=grayscale
	float fSaturation= 1.2f;
	float fFinalLum = dot(o.rgb, LUMINANCE_VECTOR);
	o.rgb = lerp((float3)fFinalLum, o.rgb, fSaturation);

	// enhance contrast
	// 2.0 = contrast enhanced, 1.0=normal contrast, 0.01= max contrast reduced
	float fInvContrast = 1.15;
	o.rgb = (o.rgb-0.5f)*fInvContrast+0.5f;
#endif

	// exposure
	o.rgb *= 1.3;

#if bEnable_Bloom
	const float bloomScale = 0.6;
	o += bloomColor * bloomScale;
#endif

	//o = LinearToGamma(o);

	return float4( o, 1 );
}


