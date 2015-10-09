/*
=============================================================================
	File:	p_deferred_lighting
	Desc:	
=============================================================================
*/

/*
=============================================================================
	p_deferred_directional_light
=============================================================================
*/
Shader p_deferred_directional_light
<
	bLight_CastShadows = 0
	bEnableSoftShadows = 1	// softer shadows via edge-tap smoothing PCF
	bVisualizeCascades = 0	// for debugging only
>
{
Inputs : Globals View GBuffer
{
	cbuffer Data(PS)
	{
		float4	lightVectorVS;		// normalized light vector in view space ( L = -1 * direction )
		//float4	halfVectorVS;		// normalized half vector in view space ( H = (L + E)/2 or H = normalize(L+ E) )
		float4	lightDiffuseColor;
		float4	lightSpecularColor;
		//float4	lightAmbientColor;
		float4x4	shadowMatrices[NUM_SHADOW_CASCADES];
		float4		cascadeSplits;
	};

	Texture2D<float>	shadowDepthMap(PS);

}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_shade_lib.h"

#if bLight_CastShadows

//--------------------------------------------------------------------------------------
// Samples the shadow map cascades based on the pixel position, using edge-tap
// smoothing PCF for filtering
//--------------------------------------------------------------------------------------
float SampleShadowCascade( in float3 position, in uint cascadeIndex )
{
	float4x4 shadowMatrix = shadowMatrices[ cascadeIndex ];
	float3 shadowPosition = mul( float4(position, 1.0f), shadowMatrix ).xyz;
	float2 shadowTexCoord = shadowPosition.xy;
	float shadowDepth = shadowPosition.z;

	float shadowVisibility;

#if bEnableSoftShadows

	// Edge tap smoothing

	//const int filterSize = 3;
	//Radius = (filterSize - 1.0f) / 2.0f;

	const int Radius = 2;
	const float SHADOW_MAP_DBL_SIZE = SHADOW_MAP_SIZE * 2;
	const int NumSamples = (Radius * 2 + 1) * (Radius * 2 + 1);

	float2 fracs = frac( shadowTexCoord.xy * SHADOW_MAP_DBL_SIZE );
	float leftEdge = 1.0f - fracs.x;
	float rightEdge = fracs.x;
	float topEdge = 1.0f - fracs.y;
	float bottomEdge = fracs.y;

	shadowVisibility = 0.0f;

	[unroll(NumSamples)]
	for( int y = -Radius; y <= Radius; y++ )
	{
		[unroll(NumSamples)]
		for( int x = -Radius; x <= Radius; x++ )
		{
			float2 offset = float2(x, y) * (1.0f / SHADOW_MAP_DBL_SIZE);
			float2 sampleCoord = shadowTexCoord + offset;

			// NOTE: don't use hardware PCF filtering here
			float sample = shadowDepthMap.SampleCmp( shadowMapSampler, sampleCoord, shadowDepth ).x;

			float xWeight = 1;
			float yWeight = 1;

			if( x == -Radius ) {
				xWeight = leftEdge;
			}
			else if(x == Radius) {
				xWeight = rightEdge;
			}

			if(y == -Radius) {
				yWeight = topEdge;
			}
			else if(y == Radius) {
				yWeight = bottomEdge;
			}

			shadowVisibility += sample * xWeight * yWeight;
		}
	}

	shadowVisibility  /= NumSamples;
	shadowVisibility *= 1.55f;

#else

	// cheap hardware bilinear PCF

	shadowVisibility = 0.0f;

	const float comparisonValue = shadowDepth;
	shadowVisibility = shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, shadowTexCoord, comparisonValue ).x;

#endif // bEnableSoftShadows

	return shadowVisibility;
}

//--------------------------------------------------------------------------------------
// Computes the visibility term by performing the shadow test
//--------------------------------------------------------------------------------------
float3 ShadowVisibility( in float3 position, in float depthVS )
{
	float3 shadowVisibility = 1.0f;
	uint cascadeIndex = 0;

	// Figure out which cascade to sample from
	[unroll]
	for( uint i = 0; i < NUM_SHADOW_CASCADES - 1; ++i )
	{
		[flatten]
		if(depthVS > cascadeSplits[i]) {
			cascadeIndex = i + 1;
		}
	}

	shadowVisibility = SampleShadowCascade( position, cascadeIndex );

	// Sample the next cascade, and blend between the two results to
	// smooth the transition
	const float BlendThreshold = 0.1f;
	float nextSplit = cascadeSplits[ cascadeIndex ];
	float splitSize = i == 0 ? nextSplit : nextSplit - cascadeSplits[ cascadeIndex - 1 ];
	float splitDist = (nextSplit - depthVS) / splitSize;

	float nextSplitVisibility = SampleShadowCascade( position, cascadeIndex + 1 );
	float lerpAmount = smoothstep( 0.0f, BlendThreshold, splitDist );
	shadowVisibility = lerp( nextSplitVisibility, shadowVisibility, lerpAmount );

	return shadowVisibility;
}

float3 GetCascadeColor( in float depthVS )
{
	uint cascadeIndex = 0;

	[unroll]
	for( uint i = 0; i < NUM_SHADOW_CASCADES - 1; ++i ) {
		[flatten]
		if( depthVS > cascadeSplits[i] ) {
			cascadeIndex = i + 1;
		}
	}

	const float3 cascadeColors[ NUM_SHADOW_CASCADES ] = {
		float3( 1, 0, 0 ),
		float3( 0, 1, 0 ),
		float3( 0, 0, 1 ),
		float3( 1, 1, 0 ),
	};

	return cascadeColors[ cascadeIndex ];
}

#endif // bLight_CastShadows


struct PSOut
{
	float4	color : SV_Target0;
};

void PS_Main( in VS_ScreenQuadOutput IN, out PSOut OUT )
{
	GSurface	surface;
	ReadSurfaceData( IN.texCoord, surface );

	const float pixelDepth = SamplePixelDepth( IN.texCoord );
	const float3 positionVS  = IN.eyeRayVS * pixelDepth;
	const float3 normalVS  = surface.normal;

	const float3 eyeDirVS = normalize( positionVS );
	const float3 H = normalize( eyeDirVS + lightVectorVS.xyz );

	const float NdotL = dot( normalVS, lightVectorVS.xyz );

	float3 totalColor = surface.albedo * NdotL * lightDiffuseColor.rgb;

	// @todo: specular:
/*
	const float k = pow( saturate( dot(normalVS, H) ), surface.specularPower );

	const float3 specularColor = surface.albedo * k * specularAmount;
*/
	totalColor *= surface.specularAmount;


#if bLight_CastShadows
	float depthVS = UnpackDepth( pixelDepth );
	//const float3 positionWS = mul( float4(positionVS,1), inverseViewMatrix ).xyz;

	#if bVisualizeCascades
		totalColor *= GetCascadeColor( depthVS );
	#else
		float3 shadowVisibility = ShadowVisibility( positionVS, depthVS );
		totalColor *= shadowVisibility;
	#endif // bVisualizeCascades

#endif // bLight_CastShadows

	OUT.color = float4( totalColor, 1 );
}


}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenQuad_VS
	PixelShader		PS_Main
}



/*
=============================================================================
	p_deferred_fullscreen_point_light
=============================================================================
*/
Shader p_deferred_fullscreen_point_light
<
	bLight_EnableSpecular = 0
>
{
Inputs : Globals View GBuffer LocalLightData
{
}//Inputs
//---------------------------------------------------------------------------
Code
{
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

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenQuad_VS
	PixelShader		PS_Main
}


/*
=============================================================================
	p_deferred_local_point_light
=============================================================================
*/
Shader p_deferred_local_point_light
<
	bLight_EnableSpecular = 0
>
{
Inputs : Globals View GBuffer LocalLightData
{
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_shade_lib.h"
#include "h_deferred_lighting.h"

void PS_Main( in PSIn IN, out PSOut OUT )
{
	GSurface	surface;
	float3		positionVS;
	PS_LightVolume_ReadPixelAttributes( IN, surface, positionVS );

	float3 outputColor = 0.0f;
	outputColor = CalcPointLight( surface, positionVS );

	OUT.color = float4( outputColor, 1.0f );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_LightVolume_Main
	PixelShader		PS_Main
}


/*
=============================================================================
	p_deferred_fullscreen_spot_light
=============================================================================
*/
Shader p_deferred_fullscreen_spot_light
<
	bLight_EnableSpecular = 0
	bSpotLight_ProjectsTexture = 0
	bSpotLight_CastShadows = 0
>
{
Inputs : Globals View GBuffer LocalLightData
{
Texture2D<float3>	projector(PS);
Texture2D<float>	shadowDepthMap(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"
#include "h_shade_lib.h"
#include "h_deferred_lighting.h"

void PS_Main( in VS_ScreenQuadOutput IN, out PSOut OUT )
{
	GSurface	surface;
	float3		positionVS;
	PS_ScreenQuad_ReadPixelAttributes( IN, surface, positionVS );

	float3 outputColor = 0.0f;
	outputColor = CalcSpotLight( surface, positionVS );

	OUT.color = float4( outputColor, 1.0f );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenQuad_VS
	PixelShader		PS_Main
}

/*
=============================================================================
	p_deferred_local_spot_light
=============================================================================
*/
Shader p_deferred_local_spot_light
<
	bLight_EnableSpecular = 0
	bSpotLight_ProjectsTexture = 0
	bSpotLight_CastShadows = 0
>
{
Inputs : Globals View GBuffer LocalLightData
{
Texture2D<float3>	projector(PS);
Texture2D<float>	shadowDepthMap(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_shade_lib.h"
#include "h_deferred_lighting.h"

void PS_Main( in PSIn IN, out PSOut OUT )
{
	GSurface	surface;
	float3		positionVS;
	PS_LightVolume_ReadPixelAttributes( IN, surface, positionVS );

	float3 outputColor = 0.0f;
	outputColor = CalcSpotLight( surface, positionVS );

	OUT.color = float4( outputColor, 1.0f );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_LightVolume_Main
	PixelShader		PS_Main
}

