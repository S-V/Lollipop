// p_adapt_luminance_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (431)
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
Texture2D currentLuminanceTexture : register(t0);
Texture2D adaptedLuminanceTexture : register(t1);

#include "h_screen_shader.h"
#include "h_post_processing.h"

// Adapts the luminance using Pattanaik's technique.
// Calculate the luminance that the camera is current adapted to, using
// the most recent adaptation level, the current scene luminance, and
// the time elapsed since last calculated
//
float PS_CalculateAdaptedLuminance( in float currLum, in float oldLum )
{
#if 1
	//determine if rods or cones are active
	//Perceptual Effects in Real-time Tone Mapping: Equ(7)    
	float sigma = saturate(0.4/(0.04+currLum));

	static const float TauCone = 0.01;
	static const float TauRod = 0.04;

	// 1 - instant adaptation, 10 - fast, 100 - normal,1000 - slow
	static const float AdaptationScale = 50;
	//static const float AdaptationScale = 1000;

	//interpolate tau from taurod and taucone depending on lum
	//Perceptual Effects in Real-time Tone Mapping: Equ(12)
	float Tau = lerp(TauCone,TauRod,sigma) * AdaptationScale;

	//calculate adaption
	//Perceptual Effects in Real-time Tone Mapping: Equ(5)

	return oldLum + (currLum - oldLum) * (1 - exp(-(GetDeltaTime() )/Tau));
#else
	// The user's adapted luminance level is simulated by closing the gap between
	// adapted luminance and current luminance by 2% every frame, based on a
	// 30 fps rate. This is not an accurate model of human adaptation, which can
	// take longer than half an hour.
	float fNewAdaptation = oldLum + (currLum - oldLum) * ( 1 - pow( 0.98f, 30 * GetDeltaTime() ) );
	return fNewAdaptation;
#endif
}
float PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float currLum = currentLuminanceTexture.Sample( pointSampler, input.texCoord ).x;
	//return currLum;
	
	float oldLum = adaptedLuminanceTexture.Sample( pointSampler, input.texCoord ).x;
	//float oldLum = SampleLuminance( adaptedLuminanceTexture );
	oldLum = max(oldLum,0.00001);

	float o = PS_CalculateAdaptedLuminance( currLum, oldLum );
	return o;
}

