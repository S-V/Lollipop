/*
=============================================================================
	File:	BuildConfig.h
	Desc:	This is a shared header file
			included by both host application (C++) code and shader code (HLSL).
=============================================================================
*/

// included by most shaders. contains common macros, types, functions, constants and variables

#ifndef __RX_GPU_BUILD_CONFIG_H__
#define __RX_GPU_BUILD_CONFIG_H__

#ifndef __cplusplus

// See: http://www.spuify.co.uk/?p=138
#pragma pack_matrix( row_major )

#define RX_ALLOW_ONLY_UNIFORM_SCALING	(1)

#endif // !__cplusplus

//-------------------------------------------------------------

/*
 * Common constants for shadow mapping
*/

// number of view volume splits for CSM/PSSM
#define NUM_SHADOW_CASCADES		4
//#define NUM_SHADOW_CASCADES		3


// size of (square) shadow map for each cascade
#if 0
	//#define SHADOW_MAP_SIZE			256
	#define SHADOW_MAP_SIZE			512
#else
	#define SHADOW_MAP_SIZE			(1024)
#endif

#define INV_SHADOW_MAP_SIZE		(1.0f/SHADOW_MAP_SIZE)


/*
 * Common constants for post processing
*/

// Maximum number of texture grabs
// Don't change!
#define POST_FX_MAX_TAPS	16
//enum { MAX_SAMPLES = 16 };


#define NUM_16	16
#define NUM_8	8
#define NUM_4	4

#endif // __RX_GPU_BUILD_CONFIG_H__
