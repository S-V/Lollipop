// p_post_processing.fx
//---------------------------------------------------------------
//		Shaders for post-processing.
//---------------------------------------------------------------


/*
=============================================================================
	p_horizontal_blur
=============================================================================
*/
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

/*
=============================================================================
	p_vertical_blur
=============================================================================
*/
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
//---------------------------------------------------------------------------


/*
=============================================================================
	p_horizontal_gaussian_blur
=============================================================================
*/
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


/*
=============================================================================
	p_vertical_gaussian_blur
=============================================================================
*/
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
	bEnable_SSAO = 1
>
{
Inputs : Globals View GBuffer
{
	Texture2D sceneColorTexture(PS);
	Texture2D sceneDepthTexture(PS);
	Texture2D blurredSceneTexture(PS);	// Blurred image for DoF

	Texture2D randomNormalsTexture(PS);	// RGBA8 linear texture containing random normals for SSAO
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_post_processing.h"


static const float sampleRadius = 0.1;//0.01;
static const float distanceScale = 1;

#define NUMSAMPLES 8

//Sample Vectors
static const float4 samples[ NUMSAMPLES ] =
{
	float4(0.355512, 	-0.709318, 	-0.102371,	0.0 ),
	float4(0.534186, 	0.71511, 	-0.115167,	0.0 ),
	float4(-0.87866, 	0.157139, 	-0.115167,	0.0 ),
	float4(0.140679, 	-0.475516, 	-0.0639818,	0.0 ),
	float4(-0.207641, 	0.414286, 	0.187755,	0.0 ),
	float4(-0.277332, 	-0.371262, 	0.187755,	0.0 ),
	float4(0.63864, 	-0.114214, 	0.262857,	0.0 ),
	float4(-0.184051, 	0.622119, 	0.262857,	0.0 )
};


float4 PS_Main( in VS_ScreenQuadOutput input ) : SV_Target
{
	//return sceneColorTexture.Sample( pointSampler, input.texCoord );
	//return PS_DOF_Comp( input, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );
	//return randomNormalsTexture.Sample( pointSampler, input.texCoord ) * 0.5;

	const float pixelDepth = UnpackDepth( sceneDepthTexture.Sample( pointSampler, input.texCoord ).r );

	//return pixelDepth * GetInverseFarZ();


	//Sample a random normal vector
	//float3 randNormal = randomNormalsTexture.Sample( pointSampler, input.texCoord /* 200.0f*/ ).xyz;
	//randNormal = normalize(randNormal);
	const float3 randNormal = float3(0,1,0);


	//return float4(randNormal.xyz,1);

	//Sample the Normal for this pixel

	GSurface	surface;
	ReadSurfaceData( input.texCoord, surface );

	// view-space normal
	const float3 normal = surface.normal;


	const float3 positionVS  = input.eyeRayVS * pixelDepth;


	//return PS_DOF_Comp( input.texCoord, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );
	return float4(positionVS,1);

#if 0

#define ONE_OVER_RAND_TEXTURE_SIZE 0.000976563
#define MAX_RAND_SAMPLES 14

static const float3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
{
	float3(1, 0, 0),
	float3(	-1, 0, 0),
	float3(0, 1, 0),
	float3(0, -1, 0),
	float3(0, 0, 1),
	float3(0, 0, -1),
	normalize(float3(1, 1, 1)),
	normalize(float3(-1, 1, 1)),
	normalize(float3(1, -1, 1)),
	normalize(float3(1, 1, -1)),
	normalize(float3(-1, -1, 1)),
	normalize(float3(-1, 1, -1)),
	normalize(float3(1, -1, -1)),
	normalize(float3(-1, -1, -1))
};

#endif




#if 0
	float3 se = positionVS;

	float finalColor = 0.0f;


	//SSAO loop
	[unroll]
	for (int i = 0; i < NUMSAMPLES; i++)
	{
		//Calculate the Reflection Ray
		float3 ray = reflect( samples[i].xyz, randNormal ) * sampleRadius;

		//Test the Reflection Ray against the surface normal
		if( dot(ray, normal) < 0 ) {
			ray += normal * sampleRadius;
		}

		//Calculate the Sample vector
		float4 sample = float4( se + ray, 1.0f );

		//Project the Sample vector into ScreenSpace
		float4 ss = mul(sample, projectionMatrix);
		ss.xy /= ss.w;

		//Convert SS into UV space
		//float2 sampleTexCoord = 0.5f * ss.xy / ss.w + float2(0.5f, 0.5f);
		float2 sampleTexCoord = ClipPosToTexCoords(  ss.xy );

		//Sample the Depth along the ray
		//float sampleDepth = tex2D(GBuffer2, sampleTexCoord).g;
		float sampleDepth = UnpackDepth( sceneDepthTexture.Sample( pointSampler, sampleTexCoord ).r );

		//Check the sampled depth value
		if (sampleDepth == 1.0)
		{
			//Non-Occluded sample
			finalColor++;
		}
		else
		{	
			//Calculate Occlusion
			float occlusion = distanceScale * max(sampleDepth - pixelDepth, 0.0f);

			//Accumulate to finalColor
			finalColor += 1.0f / (1.0f + occlusion * occlusion * 0.1);
		}
	}

	//Output the Average of finalColor
	//return float4(finalColor / NUMSAMPLES, finalColor / NUMSAMPLES, finalColor / NUMSAMPLES, 1.0f);
	return (float4) (finalColor / NUMSAMPLES);
#endif
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenQuad_VS
	PixelShader		PS_Main
}
