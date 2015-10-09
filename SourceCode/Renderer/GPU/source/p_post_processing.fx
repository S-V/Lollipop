// p_post_processing.fx
//---------------------------------------------------------------
//		Shaders for post-processing.
//---------------------------------------------------------------


/*
=============================================================================
	p_downscale_4x4
	reduces size of input by 4 (area by 16)
=============================================================================
*/
Shader p_downscale_4x4
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
	//SamplerState sourceSampler(PS) = SS_Point;

	cbuffer Data(PS)
	{
		float2	sampleOffsets[NUM_16];
	};

}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float4 sample = 0.0f;

	[unroll]
	for( int i=0; i < 16; i++ )
	{
		sample += sourceTexture.Sample( pointSampler, input.texCoord + sampleOffsets[i] );
	}

	return sample * (1.0f / 16.0f);
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}


/*
=============================================================================
	p_downscale_4x4_bright_pass
	reduces size of input by 4 (area by 16)
=============================================================================
*/
Shader p_downscale_4x4_bright_pass
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
	Texture2D avgLuminanceTexture(PS);

	cbuffer Data(PS)
	{
		float4	sampleOffsets[NUM_8];	// 16 * float2
	};

}//Inputs
//---------------------------------------------------------------------------
Code
{
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

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

/*
=============================================================================
	p_weighted_blur
=============================================================================
*/
Shader p_weighted_blur
<
	iNumSamplesBit0 = 1
	iNumSamplesBit1 = 0
	iNumSamplesBit2 = 0
	iNumSamplesBit3 = 0
>
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
	SamplerState sourceSampler(PS) = SS_PointClamp;

	cbuffer Data(PS)
	{
		float4	sampleOffsets[NUM_8];	// array of float2 vectors
		float4	sampleWeights[NUM_4];	// array of float1 scalars
	};
}//Inputs
//---------------------------------------------------------------------------
Code
{
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

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}






/*
=============================================================================
	p_blur
=============================================================================
* /
Shader p_blur
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
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
	return VerticalGaussianBlur_PS( input.texCoord, sourceTexture, pointSampler );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
/**/



/*
=============================================================================
	p_sample_luminance_initial

	Takes the HDR Scene texture as input and starts the process of 
	determining the average luminance by converting to grayscale, taking
	the log(), and scaling the image to a single pixel by averaging sample 
	points.
=============================================================================
*/
Shader p_sample_luminance_initial
{
Inputs : Globals PostProcessData
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"

float PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	const float3 srcColor = sourceTexture.Sample( linearSampler, input.texCoord.xy ).rgb;

	// Delta contains a small value to handle the case of pure black texels.
	const float DELTA = 0.001f;

	float luminance = log( dot( srcColor, LUMINANCE_VECTOR ) + DELTA );
	//float luminance = max( dot( srcColor, LUMINANCE_VECTOR ), 0.0001f );
	luminance = max( luminance, 0.00001f );

	return luminance;
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}


/*
=============================================================================
	p_resample_average_luminance

	Continue to scale down the luminance texture.
=============================================================================
* /
Shader p_sample_luminance_iterative
{
Inputs : Globals PostProcessData
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	// Scale down the luminance texture by blending sample points
	float fResampleSum = 0.0f; 

	for(int iSample = 0; iSample < 16; iSample++)
	{
		// Compute the sum of luminance throughout the sample points
		fResampleSum += sourceTexture.Sample( pointClampSampler, input.texCoord.xy + sampleOffsets[iSample].xy ).r;
	}

	// Divide the sum to complete the average
	fResampleSum /= 16;

	return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}


}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}
*/


/*
=============================================================================
	p_sample_average_luminance_exp

	Sample the texture to a single pixel and perform an exp() to complete
	the evalutation.
=============================================================================
* /
Shader p_sample_average_luminance_exp
{
Inputs : Globals PostProcessData
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	// Extract the average luminance of the image by completing the averaging and taking the exp() of the result
	// Scale down the luminance texture by blending sample points
	float fResampleSum = 0.0f; 

	for(int iSample = 0; iSample < 16; iSample++)
	{
		// Compute the sum of luminance throughout the sample points
		fResampleSum += sourceTexture.Sample( pointClampSampler, input.texCoord.xy + sampleOffsets[iSample].xy ).r;
	}

	// Divide the sum to complete the average, and perform an exp() to complete
	// the average luminance calculation
	fResampleSum = exp(fResampleSum/16);

	return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}
*/

/*
=============================================================================
	p_adapt_luminance
=============================================================================
*/
Shader p_adapt_luminance
{
Inputs : Globals
{
	Texture2D currentLuminanceTexture(PS);
	Texture2D adaptedLuminanceTexture(PS);

	cbuffer Data(PS)
	{
		float4	sampleOffsets[NUM_8];	// 16 * float2
	};

}//Inputs
//---------------------------------------------------------------------------
Code
{
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

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

/*
=============================================================================
	p_hdr_final_scene_pass

	Perform blue shift, tone map the scene, and add post-processed light
	effects.
=============================================================================
*/
Shader p_hdr_final_scene_pass
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
	Texture2D< float > averageLuminanceTexture(PS);
	Texture2D< float > bloomTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
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
float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	const float3 srcHdrColor = sourceTexture.Sample( pointSampler, input.texCoord ).rgb;

	const float averageLuminance = GetAverageLuminance();

	const float3 bloomColor = bloomTexture.Sample( linearSampler, input.texCoord ).rgb;

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

	const float bloomScale = 0.6;
	o += bloomColor * bloomScale;


	//o = LinearToGamma(o);

	return float4( o, 1 );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}


/*
=============================================================================
	p_horizontal_blur
=============================================================================
* /
Shader p_horizontal_blur
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"

VS_OUTPUT_BLUR VS_Main( in uint vertexID: SV_VertexID )
{
    float4 position;
    float2 texCoord;
	GetFullScreenTrianglePosTexCoord( vertexID, position, texCoord );
    return VS_Blur( position, texCoord, NUM_BLUR_SAMPLES, float2(1, 0) );
}

float4 PS_Main( in VS_OUTPUT_BLUR input ) : SV_Target
{
	return PS_Blur7( input, sourceTexture, pointSampler, weights7 );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
/**/

/*
=============================================================================
	p_vertical_blur
=============================================================================
* /
Shader p_vertical_blur
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"


VS_OUTPUT_BLUR VS_Main( in uint vertexID: SV_VertexID )
{
    float4 position;
    float2 texCoord;
	GetFullScreenTrianglePosTexCoord( vertexID, position, texCoord );
    return VS_Blur( position, texCoord, NUM_BLUR_SAMPLES, float2(0, 1) );
}

float4 PS_Main( in VS_OUTPUT_BLUR input ) : SV_Target
{
	return PS_Blur7( input, sourceTexture, linearSampler, weights7 );
}


}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
/**/
//---------------------------------------------------------------------------



/*
=============================================================================
	p_horizontal_gaussian_blur
=============================================================================
* /
Shader p_horizontal_gaussian_blur
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
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
	return HorizontalGaussianBlur_PS( input.texCoord, sourceTexture, pointSampler );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
/**/

/*
=============================================================================
	p_vertical_gaussian_blur
=============================================================================
* /
Shader p_vertical_gaussian_blur
{
Inputs : Globals
{
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
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
	return VerticalGaussianBlur_PS( input.texCoord, sourceTexture, pointSampler );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
/**/


//---------------------------------------------------------------------------
/*
program p_bright_pass
{
#include "inc/base.fxh"
#include "inc/g_buffer.fxh"
#include "inc/screen_shader.hlsl"
#include "inc/post_processing.hlsl"

VS_ScreenOutput VS_Main( in VS_ScreenInput input )
{
    VS_ScreenOutput output;
    output.position = input.position;
	output.texCoord = input.texCoord;
    return output;
}

float3 PS_Main( in VS_ScreenOutput IN ) : SV_Target
{
	float3 vColor = sourceTexture0.SampleLevel( linearClampSampler, IN.texCoord, 0 ).rgb;

	float luminance = dot( vColor, LUMINANCE);

    // Bright pass and tone mapping
#if 1
    vColor = max( 0.0f, vColor - BRIGHT_THRESHOLD );
    vColor *= MIDDLE_GRAY / (luminance + 0.001f);
    vColor *= (1.0f + vColor/LUM_WHITE);
    vColor /= (1.0f + vColor);
#endif

	return vColor;
}

}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
*/


/*
=============================================================================
	p_depth_of_field

	Pixel shader for blending the blurred depth of field with the scene color.
=============================================================================
*/
Shader p_depth_of_field
{
Inputs : Globals View GBuffer
{
	Texture2D sceneColorTexture(PS);
	Texture2D sceneDepthTexture(PS);
	Texture2D blurredSceneTexture(PS);	// Blurred image
}//Inputs
//---------------------------------------------------------------------------
Code
{
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

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}



/*
=============================================================================
	p_uber_post_processing_shader
=============================================================================
*/
Shader p_uber_post_processing_shader
<
	bEnable_Bloom = 1
	bEnable_HDR = 1
	bEnable_DOF = 1
	//bEnable_SSAO = 1
>
{
Inputs : Globals View GBuffer
{
	Texture2D sceneColorTexture(PS);

	Texture2D sceneDepthTexture(PS);
	Texture2D blurredSceneTexture(PS);	// Blurred image for DoF

	Texture2D< float > averageLuminanceTexture(PS);
	Texture2D< float > bloomTexture(PS);

	//Texture2D randomNormalsTexture(PS);	// RGBA8 linear texture containing random normals for SSAO
}//Inputs
//---------------------------------------------------------------------------
Code
{
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


}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenQuad_VS
	PixelShader		PS_Main
}
