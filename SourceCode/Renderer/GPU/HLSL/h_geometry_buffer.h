/*
=============================================================================
	File:	h_geometry_buffer.h
	Desc:	
=============================================================================
*/
#ifndef __GEOMETRY_BUFFER_HLSL__
#define __GEOMETRY_BUFFER_HLSL__

#include "h_normals_packing.h"

// this structure is packed into geometry buffer
//@todo: instead of view-space xyz pass only z
struct GBufferInput
{
	float3	position;	// view-space position
	float3	normal;		// view-space normal

	float3	albedo;	// diffuse reflectance

	float	specularAmount;	// intensity/shininess, in range [0..1], also treated as a multiplier on albedo
	float	specularPower;	// spread factor/specular hardness/glossiness, spec exp in range [1..255]
};

struct PS_Out_GBuffer
{
	float4	normal_specMul	: SV_Target0;
	float4	diffuse_specExp	: SV_Target1;
	float1	depth			: SV_Target2;
};

// this structure is read from geometry buffer
struct GBufferPixel
{
	float4	normal_specMul;
	float4	diffuse_specExp;
	float1	depth;
};


/**
 * Packs view-space pixel depth.
 */
float PackDepth( in float3 positionVS )
{
	// inverse of far clipping clipping plane
	const float invFarClipPlane = GetInverseFarZ();
	return positionVS.z * invFarClipPlane;	// scale to [N/F..1]
}

/**
 * Unpacks view-space pixel depth.
 */
float UnpackDepth( in float packedDepth )
{
	return packedDepth * GetFarZ();
}

/**
 * PackSurfaceData
 */
void PackSurfaceData(
	in GBufferInput IN,
	out PS_Out_GBuffer OUT )
{
	// write albedo
	OUT.diffuse_specExp.rgb = IN.albedo;

	// write specular exponent
	OUT.diffuse_specExp.a = IN.specularPower;	// NOTE: power should already have been normalized to range [0..1]

	// write normal
	OUT.normal_specMul.rgb = CompressNormal( IN.normal );

	// write specular brightness
	OUT.normal_specMul.a = IN.specularAmount;

	// write depth
	OUT.depth.r = PackDepth( IN.position );
}

#ifdef RX_GBUFFER_READS_ENABLED

//-------------------------------------------------------------------

/**
 * Samples the geometry buffer.
 */
GBufferPixel SampleGBuffer( in float2 texCoord )
{
	GBufferPixel	o;

	o.normal_specMul	= RT_normal_specMul.	SampleLevel( pointSampler, texCoord, 0 ).rgba;
	o.diffuse_specExp	= RT_diffuse_specExp.	SampleLevel( pointSampler, texCoord, 0 ).rgba;
	o.depth				= RT_linearDepth.		SampleLevel( pointSampler, texCoord, 0 ).r;

	return o;
}

/**
 * Samples diffuse reflectance.
 */
float3 SampleAlbedo( in float2 texCoord )
{
	float3 diffuseColor = RT_diffuse_specExp.SampleLevel( pointSampler, texCoord, 0 ).rgb;
	return diffuseColor;
}

/**
 * SamplePixelDepth - samples depth RT without any unpacking.
 */
float SamplePixelDepth( in float2 texCoord )
{
	float pixelDepth = RT_linearDepth.SampleLevel( pointSampler, texCoord, 0 ).r;
	return pixelDepth;
}

/**
 * Restores view-space pixel position from view-space depth.
 * Accepts clip-space pixel position.
 */
float3 RestoreVSPositionFromDepth( in float viewSpaceZ, in float2 screenXY )
{
	float3 posVS;

#if 0
	posVS.x = screenXY.x * (viewSpaceZ * tanHalfFoV.x);
	posVS.y = screenXY.y * (viewSpaceZ * tanHalfFoV.y);
	posVS.z = viewSpaceZ;
#else
	posVS = (float3)viewSpaceZ;
	posVS.xy *= (screenXY * tanHalfFoV.xy);
#endif

	return posVS;
}

/*
float3 GetPixelPositionWS( in float pixelDepth, in float3 eyeRayWS )
{
	float3 positionWS = eyeRayWS * pixelDepth + GetEyePositionWS();
	return positionWS;
}
*/

//
//	GSurface - contains common materials properties
//	that we can read from the geometry buffer (they are cached in the g-buffer).
//
struct GSurface
{
	float3	normal;	// view-space normal

	float3	albedo;	// diffuse color

	float	specularAmount;	// intensity/shininess, in range [0..1], also treated as a multiplier on albedo
	float	specularPower;	// spread factor/glossiness, spec exp in range [1..255]
};

/**
 * ReadSurfaceData
 */
void ReadSurfaceData(
	in float2 texCoord,
	out GSurface s )
{
	float4 G0 = RT_normal_specMul.SampleLevel( pointSampler, texCoord, 0 ).rgba;
	float4 G1 = RT_diffuse_specExp.SampleLevel( pointSampler, texCoord, 0 ).rgba;

	s.normal = ExpandNormal( G0.rgb );
	s.normal = normalize( s.normal );

	s.albedo = G1.rgb;

	s.specularAmount = G0.a;
	s.specularPower = G1.a * 254.0f + 1.0f;	// [0..1] -> [1..255]
}

#endif // RX_GBUFFER_READ_WRITE

#endif // __GEOMETRY_BUFFER_HLSL__
