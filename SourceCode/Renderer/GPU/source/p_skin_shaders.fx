/*
-----------------------------------------------------------------------------
	builds hardware shadow map (filled with linear 0..1 depth values)

	see: http://www.mvps.org/directx/articles/linear_z/linearz.htm
-----------------------------------------------------------------------------
*/
Shader p_skin_shader_build_shadow_map
{
Inputs : Globals View
{
cbuffer Data(VS)
{
	float4x4 lightWVP;
	//float	invFarPlane;
};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"

struct VSIn
{
	float3	position : Position;
};

struct VSOut
{
	float4	posH : SV_Position;
	//float	depth : TexCoord0;
};

void VS_Main( in VSIn IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.posH = mul( posLocal, lightWVP );
	OUT.posH.z *= OUT.posH.w; // We want linear positions
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		nil
}






/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
Shader p_skin_shader_gbuffer_pass
<
	bHasNormalMap = 0
	bHasSpecularMap = 0
>
{
Inputs : Globals View Object
{
	Texture2D<float4>	baseMap(PS);	// [required]
	Texture2D<float4>	normalMap(PS);	// [optional]
	Texture2D<float4>	specularMap(PS);// [optional] specular and AO
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position;
	float3	normalVS : Normal0;

	float2	uv : TexCoord0;

#if bHasNormalMap
	float3  tangentVS : Tangent;	// view-space tangent
	float3  bitangentVS : Binormal;	// view-space binormal
#endif
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;

	float3 N = UnpackVertexNormal( IN.normal.xyz );
	N = Transform_Dir_ObjectToView( N );
	OUT.normalVS = N;

	OUT.uv = IN.texCoords;

#if bHasNormalMap
	float3 T = UnpackVertexNormal( IN.tangent.xyz );
	T = Transform_Dir_ObjectToView( T );
	T = normalize( T - dot( T, N ) * N );
	float3 B = cross( N, T );

	OUT.tangentVS = T;
	OUT.bitangentVS = B;
#endif
}

void PS_Main( in VSOut IN, out PS_Out_GBuffer OUT )
{
	GBufferInput		s;

	s.position = IN.positionVS;

#if bHasNormalMap
	// take .gr channels, see iryoku-separable-sss-f61b44c (3Dc format)
	float3 normalT;
	normalT.xy = 2 * normalMap.Sample( normalMapSampler, IN.uv ).gr - 1;
	normalT.z =  sqrt( 1.0 - normalT.x * normalT.x - normalT.y * normalT.y );
	normalT = normalize( normalT );

	float3 bumpedNormal = IN.tangentVS * normalT.x
						+ IN.bitangentVS * normalT.y
						+ IN.normalVS * normalT.z;
	bumpedNormal = normalize( bumpedNormal );

	s.normal = bumpedNormal;
#else
	s.normal = normalize( IN.normalVS );
#endif


	s.albedo = baseMap.Sample( colorMapSampler, IN.uv ).rgb;

mxUNDONE
#if bUseSpecularMap
	// Fetch albedo, specular parameters and static ambient occlusion:
	float3 specularAO = specularMap.Sample( specularMapSampler, IN.uv ).rgb;

	s.specularAmount = 0.0f;
	s.specularPower = 0.0f;	// normalized, in range [0..1]
#else
	const float occlusion = 1.0f;
	const float specular = 0.0f;

	s.specularAmount = 0.0f;
	s.specularPower = 0.0f;	// normalized, in range [0..1]
#endif


	PackSurfaceData( s, OUT );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}



/*
-----------------------------------------------------------------------------
	p_skin_shader_forward_pass
-----------------------------------------------------------------------------
*/
Shader p_skin_shader_forward_pass
<
	bEnableSSS = 0	// enable subsurface scattering simulation
	bUseNormalMap = 0
	bUseSpecularMap = 0
	bEnableShadows = 0	// but shadow map is still needed for subsurface scattering
>
{
Inputs : Globals View Object
{
	// material data for deferred pass
	cbuffer Data(PS)
	{
		float4		lightPos_Radius;	// in world space
		float4		lightDir_InvRadius;	// in world space
		float4		lightDiffuseColor;

		// x - specularFresnel (0.82), y - specularIntensity (1.88f), z - specularRoughness (0.3f)
		float4		lightSpecularParams;

		// x - attenuation, y - start angle of spot falloff in radians, z - inverse of spot falloff width
		float4		lightSpotParams;

		float4x4	lightViewProjection;// multiplied with scale/offset texture matrix
		// x - shadow map bias, y - inverse shadow map size
		float4		lightShadowParams;

	};

	Texture2D<float4>	baseMap(PS);
	Texture2D<float4>	normalMap(PS);
	Texture2D<float4>	specularMap(PS);	// specular and AO

	Texture2D		shadowDepthMap(PS);
	Texture2D		beckmannTexture(PS);
	//TextureCube		irradianceMap(PS);	// environment params for image-based lighting

}//Inputs
//---------------------------------------------------------------------------
Code
{
#define lightPosition	lightPos_Radius.xyz
#define lightDirection	lightDir_InvRadius.xyz
#define lightRadius		lightPos_Radius.w
#define lightInvRadius	lightDir_InvRadius.w
#define shadowMapBias		lightShadowParams.x
#define invShadowMapSize	lightShadowParams.y

#define lightSpecularFresnel	lightSpecularParams.x
#define lightSpecularIntensity	lightSpecularParams.y
#define lightSpecularRoughness	lightSpecularParams.z

#define lightAttenuation		lightSpotParams.x
#define lightFalloffStart		lightSpotParams.y
#define lightInvFalloffWidth	lightSpotParams.z


#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"
#include "h_separable_sss.h"
#include "h_shade_lib.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position0;
	float3	positionWS : Position1;
	float3	normalWS : Normal0;

	float2	uv : TexCoord0;

	float3 viewVectorWS : TexCoord1;

#if bUseNormalMap
	float3  tangentWS : Tangent;	// view-space tangent
	float3  bitangentWS : Binormal;	// view-space binormal
#endif
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;
	OUT.positionWS = Transform_ObjectToWorld( posLocal ).xyz;

	float3 N = UnpackVertexNormal( IN.normal.xyz );
	N = Transform_Dir_ObjectToWorld( N );
	OUT.normalWS = N;

	OUT.uv = IN.texCoords;

	OUT.viewVectorWS = GetEyePositionWS() - OUT.positionWS;

#if bUseNormalMap
	float3 T = UnpackVertexNormal( IN.tangent.xyz );
	T = Transform_Dir_ObjectToWorld( T );
	T = normalize( T - dot( T, N ) * N );
	float3 B = cross( N, T );

	OUT.tangentWS = T;
	OUT.bitangentWS = B;
#endif
}

float SpecularKSK( Texture2D beckmannTex, float3 normal, float3 light, float3 view, float roughness )
{
	float3 half = view + light;
	float3 halfn = normalize(half);

	float ndotl = max(dot(normal, light), 0.0);
	float ndoth = max(dot(normal, halfn), 0.0);

	float sample = beckmannTex.SampleLevel( linearSampler, float2(ndoth, roughness), 0 ).r;

	// prevent error X3571: pow(f, e) will not work for negative f, use abs(f) or conditionally handle negative values if you expect them
	sample = abs(sample);

	float ph = pow(2.0 * sample, 10.0);
	float f = lerp(0.25, Fresnel(halfn, view, 0.028), lightSpecularFresnel);
	float ksk = max(ph * f / dot(half, half), 0.0);

	return ndotl * ksk;   
}

float Shadow( float3 worldPosition )
{
	float4 shadowPosition = mul( float4(worldPosition, 1.0), lightViewProjection );
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += shadowMapBias;
	return shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, shadowPosition.xy, shadowPosition.z * lightInvRadius ).r;
}
float ShadowPCF( float3 worldPosition, uint numSamples, float width )
{
	float4 shadowPosition = mul( float4(worldPosition, 1.0), lightViewProjection );
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += shadowMapBias;

	float shadow = 0.0;
	float offset = (numSamples - 1.0) / 2.0;
	[unroll]
	for( float x = -offset; x <= offset; x += 1.0 )
	{
		[unroll]
		for( float y = -offset; y <= offset; y += 1.0 )
		{
			float2 pos = shadowPosition.xy + width * float2( x, y ) * invShadowMapSize;
			shadow += shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, pos, shadowPosition.z * lightInvRadius ).r;
		}
	}
	shadow /= numSamples * numSamples;
	return shadow;
}

float3 PS_Main( in VSOut IN ) : SV_Target0
{
	float3	color = (float3)0;

	IN.viewVectorWS = normalize(IN.viewVectorWS);

	const float3 eyePosition = IN.positionWS;
	const float3 eyeVector = IN.viewVectorWS;

	float3	N;

#if bUseNormalMap
	// take .gr channels, see iryoku-separable-sss-f61b44c (3Dc format)
	float3 normalT;
	normalT.xy = 2 * normalMap.Sample( normalMapSampler, IN.uv ).gr - 1;
	normalT.z =  sqrt( 1.0 - normalT.x * normalT.x - normalT.y * normalT.y );
	normalT = normalize( normalT );

	float3 bumpedNormal = IN.tangentWS * normalT.x
						+ IN.bitangentWS * normalT.y
						+ IN.normalWS * normalT.z;
	bumpedNormal = normalize( bumpedNormal );
	N = bumpedNormal;
#else
	N = normalize( IN.normalWS );
#endif

	const float3 albedo = baseMap.Sample( colorMapSampler, IN.uv ).rgb;

	float3 L = lightPosition - eyePosition;
	float dist = length(L);
	L /= dist;

	float spot = dot( lightDirection, -L );

	// And the spot light falloff:
	spot = saturate( (spot - lightFalloffStart) * lightInvFalloffWidth );

	// Calculate attenuation:
	float curve = min(pow(dist * lightInvRadius, 6.0), 1.0);
	float attenuation = lerp(1.0 / (1.0 + lightAttenuation * dist * dist), 0.0, curve);
	attenuation *= 2;

	// Calculate some terms we will use later on:
	float3 f1 = lightDiffuseColor.rgb * attenuation * spot;
	float3 f2 = albedo.rgb * f1;

	// Calculate the diffuse and specular lighting:
	float3 diffuse = saturate(dot(L, N));

	
	// Calculate specular lighting:

#if bUseSpecularMap
	// Fetch albedo, specular parameters and static ambient occlusion:
	float3 specularAO = specularMap.Sample( specularMapSampler, IN.uv ).rgb;

	const float occlusion = specularAO.b;
	const float intensity = specularAO.r * lightSpecularIntensity;
	const float roughness = (specularAO.g / 0.3) * lightSpecularRoughness;

	const float specular = intensity * SpecularKSK( beckmannTexture, N, L, eyeVector, roughness );
#else
	const float occlusion = 1.0f;
	const float specular = 0.0f;
#endif

	
#if bEnableSSS	
	//const float translucency = 0.83;
	//const float sssWidth = 0.012;
	
	// too reddish
	//const float translucency = 0.9;
	//const float sssWidth = 0.025;

	//const float translucency = 0.87;
	//const float sssWidth = 0.015;

	const float translucency = 0.9;
	const float sssWidth = 0.02;

	// Add the transmittance component:
	if(1)
	{	
		color.rgb += f2 * SSSSTransmittance(
			translucency, sssWidth,
			eyePosition, N,
			L, shadowDepthMap, lightViewProjection, lightRadius
		);
	}
#endif


#if bEnableShadows
	// And also the shadowing:
	//const float shadow = Shadow( eyePosition );
	const float shadow = ShadowPCF( eyePosition, 3, 1.0 );
#else
	const float shadow = 1.0f;
#endif

	// Add the diffuse and specular components:
	color.rgb += shadow * (f2 * diffuse + f1 * specular);

	// Add the ambient component:	
	const float ambient = 0.4f;
	color.rgb += occlusion * ambient * albedo.rgb;// * irradianceTex.Sample(linearSampler, N).rgb;
	
	return color;
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}










/*
-----------------------------------------------------------------------------
	Post-processing shader used in Separable Subsurface Scattering.
	Blurs and softens the skin.
-----------------------------------------------------------------------------
*/
Shader p_separable_sss_reflectance
<
	bVerticalBlurDirection = 0	// 0 - horizontal blur
>
{
Inputs : Globals View //GBuffer
{
	Texture2D sceneColorTexture(PS);
	Texture2D sceneDepthTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
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
	const float sssWidth = 0.02;
#if bVerticalBlurDirection
	return MySSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(0,1) );
#else
	return MySSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(1,0) );
#endif
	//return SSSSBlurPS( input.texCoord, sceneColorTexture, sceneDepthTexture, sssWidth, float2(1,0), false );
}


}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}

