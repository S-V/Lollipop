/*
=============================================================================
	File:	shared.h
	Desc:	This is a shared header file
			included by both host application (C++) code and shader code (HLSL).
=============================================================================
*/

// included by most shaders. contains common macros, types, functions, constants and variables

#ifndef __BASE_HLSL__
#define __BASE_HLSL__

#include "BuildConfig.h"

#ifndef __cplusplus

// Avoids flow control constructs.
#define MX_UNROLL											[unroll]
// Gives preference to flow control constructs.
#define MX_LOOP												[loop]
// Performs branching by using control flow instructions like jmp and label.
#define MX_BRANCH											[branch]
// Performs branching by using the cnd instructions.
#define MX_FLATTEN											[flatten]
// Executes the conditional part of an if statement when the condition is true for all threads on which the current shader is running.
#define MX_IFALL											[ifAll]
// Executes the conditional part of an if statement when the condition is true for any thread on which the current shader is running.
#define MX_IFANY											[ifAny]


/** Performs branching by using control flow instructions like jmp and label, only for Xbox. */
#if XBOX
#define BRANCH_XBOX	[branch]
#else
#define BRANCH_XBOX
#endif

/** Performs branching by using control flow instructions like jmp and label, only for PS3. */
#if PS3
#define BRANCH_PS3	[branch]
#else
#define BRANCH_PS3
#endif


#if XBOX
/** Optimizes the specified HLSL code independently of the surrounding code. */
#define ISOLATE											[isolate]
#define NOEXPRESSIONOPTIMIZATIONS						[noExpressionOptimizations]
#else
/** Not supported on other platforms */
#define ISOLATE
#define NOEXPRESSIONOPTIMIZATIONS						
#endif


//-------------------------------------------------------------
//	Markers (notes to the developer).
//-------------------------------------------------------------
//
//	Pros: bookmarks are not needed;
//		  easy to "Find All References" and no comments needed.
//	Cons: changes to the source file require recompiling;
//		  time & data have to be inserted manually.
//
#define mxNOTE( message_string )
#define mxTODO( message_string )
#define mxREFACTOR( message_string )
#define mxBUG( message_string )
#define mxFIXME( message_string )
#define mxHACK( message_string )
#define mxOPTIMIZE( message_string )
#define mxWARNING( message_string )
#define mxREMOVE_THIS
#define mxUNDONE
#define mxSWIPED( message_string )
#define mxUNSAFE
#define mxTIDY_UP_FILE
#define mxTEMP
// 'to be documented'
#define mxTBD

#define mxWHY( message_string )

//-------------------------------------------------------------------
//	Misc helpers & constants
//-------------------------------------------------------------------

#define	M_PI		3.1415926535897932384626433832795
#define	M_INV_PI	0.31830988618379067154f
#define	M_E			2.71828182845904523536f

#define	M_DEG2RAD		(M_PI / 180.0f)
#define	M_RAD2DEG		(180.0f / M_PI)
#define DEG2RAD(x)		((x) * M_DEG2RAD)
#define RAD2DEG(x)		((x) * M_RAD2DEG)

//-------------------------------------------------------------------
//	Math helpers
//-------------------------------------------------------------------

#define m4x4_identity	float4x4( 1.0f, 0.0f, 0.0f, 0.0f,	\
								 0.0f, 1.0f, 0.0f, 0.0f,	\
								 0.0f, 0.0f, 1.0f, 0.0f,	\
								 0.0f, 0.0f, 0.0f, 1.0f )


float squaref( float x ) { return (x * x); }
float cubef( float x ) { return (x * x * x); }

float sum( float3 v ) { return (v.x + v.y + v.z); }
float average3( float3 v ) { return (1.0f / 3.0f) * sum(v); }


/**
 * Aligns the clip space position so that it can be used as a texture coordinate.
 */
float2 ClipPosToTexCoords( in float2 clipPosition )
{
	// flip Y axis, [-1..+1] -> [0..1]
	//return float2( 0.5f, -0.5f ) * clipPosition + 0.5f;
	return float2(clipPosition.x, -clipPosition.y) * 0.5f + 0.5f;
}

/**
 * Aligns the [0,1] UV to match the view within the backbuffer.
 */
float2 TexCoordsToClipPos( in float2 texCoords )
{
	// [0..1] -> [-1..+1], flip Y axis
	return float2( 2.0f, -2.0f ) * texCoords + float2( -1.0f, 1.0f );
}

/** 
 * aligns the clip space position so that it can be used as a texture coordinate
 * to properly align in screen space
 */
float4 ScreenAlignedPosition( float4 screenPosition )
{
	float4 result = screenPosition;
	result.xy = (screenPosition.xy / screenPosition.w) * 0.5f + 0.5f;
	result.y = 1.0f - result.y;
	return result;
}

#endif // !__cplusplus



#define FRUSTUM_CORNER_FAR_TOP_LEFT		0
#define FRUSTUM_CORNER_FAR_TOP_RIGHT	1
#define FRUSTUM_CORNER_FAR_BOTTOM_RIGHT	2
#define FRUSTUM_CORNER_FAR_BOTTOM_LEFT	3



#endif // __BASE_HLSL__

