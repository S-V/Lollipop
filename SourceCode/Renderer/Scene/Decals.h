/*
=============================================================================
	File:	Decals.h
	Desc:	
=============================================================================
*/
#pragma once

/*
===============================================================================
	Decals are lightweight primitives for bullet / blood marks.
===============================================================================
*/



// This structure is mirrored in shaders so if it's changed,
// the corresponding shaders must be changed too!
struct SBillboardData
{
	float3	center;
	float1	radius;	// = length(size)
	float2	size;
	float2	_pad1;
	float3	color;
};



// This structure is mirrored in shaders so if it's changed,
// the corresponding shaders must be changed too!

struct SHeightFogData
{
	float3		fogColor;
	float		fogDensity;

	float		fogStartDistance;
	float		fogExtinctionDistance;
	float		minHeight;
	float		maxHeight;
};



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
