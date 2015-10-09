// p_separable_sss_reflectance_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_skin_shaders.fx' (462)
// Defines:
//	bVerticalBlurDirection = 0
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


Texture2D sceneColorTexture : register(t0);
Texture2D sceneDepthTexture : register(t1);

#include "h_vertex_lib.h"
//#include "h_geometry_buffer.h"
#include "h_post_processing.h"
#include "h_separable_sss.h"

VS_ScreenOutput VS_Main( in uint vertexID: SV_VertexID )
{
	VS_ScreenOutput output;
	GetFullScreenTrianglePosTexCoord( vertexID, output.position, output.texCoord );
	return output;
}

//-----------------------------------------------------------------------------
// Separable SSS Reflectance Pixel Shader

float4 MySSSSBlurPS(
        /**
         * The usual quad texture coordinates.
         */
        float2 texcoord,

        /**
         * This is a SRGB or HDR color input buffer, which should be the final
         * color frame, resolved in case of using multisampling. The desired
         * SSS strength should be stored in the alpha channel (1 for full
         * strength, 0 for disabling SSS). If this is not possible, you an
         * customize the source of this value using SSSS_STREGTH_SOURCE.
         *
         * When using non-SRGB buffers, you
         * should convert to linear before processing, and back again to gamma
         * space before storing the pixels (see Chapter 24 of GPU Gems 3 for
         * more info)
         *
         * IMPORTANT: WORKING IN A NON-LINEAR SPACE WILL TOTALLY RUIN SSS!
         */
        SSSSTexture2D colorTex,

        /**
         * The linear depth buffer of the scene, resolved in case of using
         * multisampling. The resolve should be a simple average to avoid
         * artifacts in the silhouette of objects.
         */
        SSSSTexture2D depthTex,

        /**
         * This parameter specifies the global level of subsurface scattering
         * or, in other words, the width of the filter. It's specified in
         * world space units.
         */
        float sssWidth,

        /**
         * Direction of the blur:
         *   - First pass:   float2(1.0, 0.0)
         *   - Second pass:  float2(0.0, 1.0)
         */
        float2 dir
)
{
	// Fetch color of current pixel:
	float4 colorM = SSSSSamplePoint(colorTex, texcoord);

	// Fetch linear depth of current pixel:
	float depthM = SSSSSamplePoint(depthTex, texcoord).r;
	depthM *= GetFarZ();

	// Calculate the sssWidth scale (1.0 for a unit plane sitting on the
	// projection window):
	float scale = gDistanceToProjectionWindow / depthM;

	// Calculate the final step to fetch the surrounding pixels:
	float2 finalStep = sssWidth * scale * dir;

	//finalStep *= 0.99;
	//finalStep *= SSSS_STREGTH_SOURCE; // Modulate it using the alpha channel.

	finalStep *= 1.0 / 3.0; // Divide by 3 as the kernels range from -3 to 3.

	// Accumulate the center sample:
	float4 colorBlurred = colorM;
	colorBlurred.rgb *= kernel[0].rgb;

    // Accumulate the other samples:
    SSSS_UNROLL
    for( int i = 1; i < SSSS_N_SAMPLES; i++ )
	{
		// Fetch color and depth for current sample:
		float2 offset = texcoord + kernel[i].a * finalStep;
		float4 color = SSSSSample(colorTex, offset);

#if SSSS_FOLLOW_SURFACE == 1
		// If the difference in depth is huge, we lerp color back to "colorM":
		float depth = SSSSSample(depthTex, offset).r;
		depthM *= GetFarZ();

		float s = SSSSSaturate( 300.0f * gDistanceToProjectionWindow * sssWidth * abs(depthM - depth) );
		color.rgb = SSSSLerp(color.rgb, colorM.rgb, s);
#endif

		// Accumulate:
		colorBlurred.rgb += kernel[i].rgb * color.rgb;
	}

    return colorBlurred;
}

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	//return sceneColorTexture.Sample( pointSampler, input.texCoord );
	//const float sssWidth = 0.015;
	const float sssWidth = 0.01;
	//const float sssWidth = 0.02;
#if bVerticalBlurDirection
	return MySSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(0,1) );
#else
	return MySSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(1,0) );
#endif
	//return SSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(1,0), false );
}


