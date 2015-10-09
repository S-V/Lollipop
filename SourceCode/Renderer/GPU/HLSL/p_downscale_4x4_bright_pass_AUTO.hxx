// p_downscale_4x4_bright_pass_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (59)
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
};
Texture2D sourceTexture : register(t0);
Texture2D avgLuminanceTexture : register(t1);

#include "h_screen_shader.h"
#include "h_post_processing.h"

float3 PS_BrightPassFilter( in float3 hdrColor, in float averageLuminance )
{
	float3 color = hdrColor;

#if 0
	//fixup for illegal floats
	if(abs(dot(color, 0.333f)) > 10000.0f) color =1.0f;
#endif

#if 0
	// Determine what the pixel's value will be after tone-mapping occurs
	color.rgb *= MIDDLE_GRAY / (averageLuminance + 0.001f);

	// Subtract out dark pixels and clamp to 0
	color.rgb = max( 0.0f, color - BRIGHT_PASS_THRESHOLD );

	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene 
	// objects.
	color.rgb /= (BRIGHT_PASS_OFFSET + color);

#else

/*
	const float KeyValue = 0.35;
	float linearExposure = (KeyValue / averageLuminance);
	float exposure = log2(max(linearExposure, 0.0001f));
	const float BloomExposureOffset = 0.95;
	exposure -= BloomExposureOffset;
	return exp2(exposure) * color;
*/

	// Determine what the pixel's value will be after tone-mapping occurs
	const float middleGray = 0.72;
	color.rgb *= middleGray / (averageLuminance*1.2 + 0.001f);
	// Subtract out dark pixels and clamp to 0
	const float brightPassThreshold = 0.5;
	color.rgb = max( 0.0f, color - brightPassThreshold );

	const float  whiteLuminance = 1.5f;
	color.rgb *= (1.0f + color.rgb/whiteLuminance);

	// Map the resulting value into the 0 to 1 range. Higher values for
	// brightPassOffset will isolate lights from illuminated scene 
	// objects.
	const float brightPassOffset = 1.0f;
	color.rgb /= (brightPassOffset + color.rgb);

#endif

	return color;
}

/*
// Uses a lower exposure to produce a value suitable for a bloom pass
float3 PS_BrightPass_Threshold( in float3 hdrColor, in float avgLuminance )
{
	static const float BloomThreshold = 0.3;
    // Tone map it to threshold
	float exposure = 0;
	return ToneMap( hdrColor, avgLuminance, BloomThreshold, exposure );
}
*/
inline float2 GetSampleOffset( in uint iSample )
{
#if 0
	const uint i = iSample / 2;	// sizeof float4 = 2 * sizeof float2
	const float4 v = sampleOffsets[i];
	return (iSample % 2 == 0) ? v.xy : v.zw;
#else
	const float2 arr[16] = (const float2[16]) sampleOffsets;
	return arr[ iSample ];
#endif
}
float3 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float3 sample = 0.0f;

	[unroll]
	for( int i=0; i < 16; i++ )
	{
		sample += sourceTexture.Sample( pointClampSampler, input.texCoord + GetSampleOffset(i) ).rgb;
	}

	sample *= (1.0f / 16.0f);
	//return sample;

	//float avgLuminance = SampleLuminance( avgLuminanceTexture );
	//avgLuminance = log2(avgLuminance);
	float avgLuminance = SampleAverageLuminance( avgLuminanceTexture );

	return PS_BrightPassFilter( sample, avgLuminance );
	//return PS_BrightPassFilter( sample, avgLuminance );
}

