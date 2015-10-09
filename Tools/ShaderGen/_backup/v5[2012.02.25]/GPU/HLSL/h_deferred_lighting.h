/*
=============================================================================
	File:	h_deferred_lighting.h
=============================================================================
*/

// uses Globals View GBuffer LocalLightData

#ifndef __DEFERRED_LIGHTING_HLSL__
#define __DEFERRED_LIGHTING_HLSL__

#include "h_base.h"

// for lights that are rendered as screen-aligned quads
#include "h_screen_shader.h"


// 1 - use HW accelerated bilinear 2x2 percentage closer shadow texture filtering for cheap shadow mapping
// (compares and bilinear filtering will be done by hardware.)
#define MX_HARDWARE_PCF		(1)


//
// Vertex type for drawing light shapes.
//
struct VSIn
{
	float3	position : Position;
};

struct PSIn
{
	float4	position : SV_Position;
	float3	screenPos : TexCoord;
};

struct PSOut
{
	float4	color : SV_Target0;
};

// for lights that are rendered as meshes
inline
void VS_LightVolume_Main( in VSIn IN, out PSIn OUT )
{
	const float4 posLocal = float4( IN.position, 1 );
	OUT.position	= mul( posLocal, lightShapeTransform );
	OUT.screenPos = float3( OUT.position.xy, OUT.position.w );
}

// for lights that are rendered as meshes
inline
void PS_LightVolume_ReadPixelAttributes(
	in PSIn IN,
	out GSurface surface,
	out float3 positionVS
	)
{
	float2 screenXY = IN.screenPos.xy / IN.screenPos.z;
	float2 texCoord = ClipPosToTexCoords( screenXY );

	ReadSurfaceData( texCoord, surface );

	const float viewSpaceZ = UnpackDepth( SamplePixelDepth( texCoord ) );
	positionVS  = RestoreVSPositionFromDepth( viewSpaceZ, screenXY );
}

// for lights that are rendered as screen-aligned quads
inline
void PS_ScreenQuad_ReadPixelAttributes(
	in VS_ScreenQuadOutput IN,
	out GSurface surface,
	out float3 positionVS
	)
{
	ReadSurfaceData( IN.texCoord, surface );

	const float pixelDepth = SamplePixelDepth( IN.texCoord );
	positionVS  = IN.eyeRayVS * pixelDepth;
}

/*
===========================================================
	CalcPointLight
===========================================================
*/
inline
float3 CalcPointLight(
	in GSurface surface,
	in float3 positionVS
)
{
	float3 outputColor = 0.0f;

	const float3 normalVS  = surface.normal;

	const float lightRange =  lightRadiusInvRadius.x;
	const float lightInvRange =  lightRadiusInvRadius.y;

#if 0
	float attenuation = CalcPointLightAttenuation(
		positionVS, normalVS,
		lightPosition.xyz, lightRange, lightInvRange
	);
#else
	float3 lightVec = lightPosition.xyz - positionVS.xyz;
	float distance = length(lightVec);
	lightVec /= distance;

	// compute point light attenuation
	float NdotL = saturate(dot(normalVS,lightVec));
	float attenuation = saturate( 1.0f - distance * lightInvRange ) * NdotL;
#endif

	// compute diffuse term
	outputColor += (surface.albedo.rgb * lightDiffuseColor.rgb);

#if bLight_EnableSpecular
	// compute specular term
	float specularFactor = pow(saturate(dot(reflect(normalize(positionVS), normalVS), lightVec)), surface.specularPower);
	float3 specularColor = surface.specularAmount * lightSpecularColor.rgb * specularFactor;
	outputColor += specularColor;
#endif // bLight_EnableSpecular

	outputColor *= attenuation;

	return outputColor;
}


/*
===========================================================
	CalcSpotLight
===========================================================
*/
inline
float3 CalcSpotLight(
	in GSurface surface,
	in float3 positionVS
)
{
	float3 outputColor = 0.0f;


#if bSpotLight_ProjectsTexture || bSpotLight_CastShadows
	// Transform from view space to light projection space.
	float4 lightSpacePosH = mul( float4(positionVS, 1.0f), eyeToLightProjection );
	lightSpacePosH.xyz /= lightSpacePosH.w;

	// texture coordinates for sampling shadow map/projective texture
	float2 projTexCoords = ClipPosToTexCoords( lightSpacePosH.xy );

	float3 projTexColor = projector.Sample( linearSampler, projTexCoords ).rgb;
#endif // bSpotLight_ProjectsTexture || bSpotLight_CastShadows




#if bSpotLight_CastShadows
	const float depth = lightSpacePosH.z;	// normalized depth from the point of view of the light

#if MX_HARDWARE_PCF
	const float comparisonValue = depth + lightShadowDepthBias;
	const float shadowFactor = shadowDepthMap.SampleCmpLevelZero( shadowMapPCFBilinearSampler, projTexCoords, comparisonValue );
#else
	const float shadowDepth = shadowDepthMap.Sample( shadowMapSampler, projTexCoords ).x;
	const float shadowFactor = ( depth <= shadowDepth + lightShadowDepthBias ) ? 1.0f : 0.0f;
#endif

	clip(shadowFactor - 0.03f);

#endif // bSpotLight_CastShadows





	const float3 normalVS  = surface.normal;

	const float lightRange =  lightRadiusInvRadius.x;
	const float lightInvRange =  lightRadiusInvRadius.y;

	float3 lightVec = lightPosition.xyz - positionVS.xyz;
	float distance = length(lightVec);
	lightVec /= distance;

	float NdotL = saturate(dot(normalVS,lightVec));

	// compute point light attenuation
	float attenuation = saturate( 1.0f - distance * lightInvRange ) * NdotL;

	// spot light attenuation
	{
		float cosAngle = dot( spotLightDirection.xyz, -lightVec );
		float cosPhi = spotLightAngles.y;	// cosine of half outer cone angle
		float cosTheta = spotLightAngles.x;	// cosine of half inner cone angle
		float spotFalloff = smoothstep( cosPhi, cosTheta, cosAngle );	// angular falloff
		attenuation *= spotFalloff;
	}

	// compute diffuse term
	outputColor += (surface.albedo.rgb * lightDiffuseColor.rgb);


#if bLight_EnableSpecular
	// compute specular term
	float specularFactor = pow(saturate(dot(reflect(normalize(positionVS), normalVS), lightVec)), surface.specularPower);
	float3 specularColor = surface.specularAmount * lightSpecularColor.rgb * specularFactor;
	outputColor += specularColor;
#endif // bLight_EnableSpecular


#if bSpotLight_ProjectsTexture || bSpotLight_CastShadows
	outputColor = projTexColor * lightProjectorIntensity
				+ outputColor * (1.0f - lightProjectorIntensity);
#endif // bSpotLight_ProjectsTexture || bSpotLight_CastShadows


#if bSpotLight_CastShadows
	outputColor *= shadowFactor;
#endif // bSpotLight_CastShadows


	outputColor *= attenuation;

	return outputColor;
}


#endif // __DEFERRED_LIGHTING_HLSL__
