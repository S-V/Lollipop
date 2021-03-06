// p_weighted_blur_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (185)
// Defines:
//	iNumSamplesBit0 = 1
//	iNumSamplesBit1 = 0
//	iNumSamplesBit2 = 0
//	iNumSamplesBit3 = 0
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


cbuffer Data : register(b1)
{
	float4 sampleOffsets[ NUM_8 ];
	float4 sampleWeights[ NUM_4 ];
};
SamplerState sourceSampler : register(s14);
Texture2D sourceTexture : register(t0);

#include "h_screen_shader.h"
#include "h_post_processing.h"

// NOTE: kernel size is odd (to get optimum number of texture fetches and avoid checking for zero case)
// see: http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
static const uint numSamples = 1 + (iNumSamplesBit0 * 1) + (iNumSamplesBit1 * 2) + (iNumSamplesBit2 * 4) + (iNumSamplesBit3 * 8);

inline float2 GetSampleOffset( in uint iSample )
{
	const float2 arr[16] = (const float2[16]) sampleOffsets;
	return arr[ iSample ];
}
inline float GetSampleWeight( in uint iSample )
{
	const float arr[16] = (const float[16]) sampleWeights;
	return arr[ iSample ];
}

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float4 sample = 0.0f;

	[unroll]
	for( uint i=0; i < numSamples; i++ )
	{
		sample += sourceTexture.Sample( sourceSampler, input.texCoord + GetSampleOffset(i) ) * GetSampleWeight(i);
	}

	return sample;
}

