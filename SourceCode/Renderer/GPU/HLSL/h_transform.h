/*
=============================================================================
	File:	h_transform.h
	Desc:	
	Note:	uses View, Object
	ToDo:	remove this file, split functionality into .fx modules
=============================================================================
*/
#ifndef __TRANSFORM_HLSL__
#define __TRANSFORM_HLSL__

#include "h_base.h"

#ifdef PER_OBJECT_CONSTANTS_DEFINED
	float4 Transform_ObjectToWorld( float4 objectSpacePosition )
	{
		return mul( objectSpacePosition, worldMatrix );
	}
	float4 Transform_ObjectToView( float4 objectSpacePosition )
	{
		return mul( objectSpacePosition, worldViewMatrix );
	}
	float4 Transform_ObjectToClip( float4 objectSpacePosition )
	{
		return mul( objectSpacePosition, worldViewProjectionMatrix );
	}
	float3 Transform_Dir_ObjectToWorld( float3 objectSpaceDir )
	{
	#ifdef RX_ALLOW_ONLY_UNIFORM_SCALING
		return mul( objectSpaceDir, (float3x3)worldMatrix );
	#else
		return mul( objectSpaceDir, inverseTransposeWorldMatrix );
	#endif
	}
#endif // PER_OBJECT_CONSTANTS_DEFINED



#ifdef PER_VIEW_CONSTANTS_DEFINED
	float4 Transform_WorldToView( float4 worldSpacePosition )
	{
		return mul( worldSpacePosition, viewMatrix );
	}
	float4 Transform_WorldToClip( float4 worldSpacePosition )
	{
		return mul( worldSpacePosition, viewProjectionMatrix );
	}
	float4 Transform_ViewToClip( float4 viewSpacePosition )
	{
		return mul( viewSpacePosition, projectionMatrix );
	}
	float3 Transform_Dir_ObjectToView( float3 objectSpaceDir )
	{
	#ifdef RX_ALLOW_ONLY_UNIFORM_SCALING
		return mul( objectSpaceDir, (float3x3)worldViewMatrix );
	#else
		return mul( objectSpaceDir, inverseTransposeWorldViewMatrix );
	#endif
	}
#endif // PER_VIEW_CONSTANTS_DEFINED





/*
float3 Transform_Dir_ObjectToClip( float3 objectSpaceDir )
{
	return mul( objectSpaceDir, inverseTransposeWorldViewProjectionMatrix );
}

float3 Transform_Dir_WorldToView( float3 worldSpaceDir )
{
	return mul( worldSpaceDir, inverseTransposeViewMatrix );
}

float3 Transform_Dir_WorldToClip( float3 worldSpaceDir )
{
	return mul( worldSpaceDir, inverseTransposeViewProjectionMatrix );
}

float3 Transform_Dir_ViewToClip( float3 viewSpaceDir )
{
	return mul( viewSpaceDir, inverseTransposeProjectionMatrix );
}
*/


#if 0


float3 GetBumpedNormal( in float2 texCoord, in float3 T, in float3 B, in float3 N )
{
	// sample tangent-space normal
	float3 normalT = ExpandNormal( SampleNormalMap( texCoord ).rgb );

	// transform normal from tangent space into TBN space
	float3 bumpedNormal = T * normalT.x
						+ B * normalT.y
						+ N * normalT.z;
	return normalize(bumpedNormal);
}
#endif



#endif // __TRANSFORM_HLSL__
