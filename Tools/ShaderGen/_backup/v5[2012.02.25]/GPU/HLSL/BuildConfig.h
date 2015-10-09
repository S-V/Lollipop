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
//#ifndef SHADOW_MAP_SIZE
//#define SHADOW_MAP_SIZE			256
//#else
#define SHADOW_MAP_SIZE			(1024)
//#endif //MX_EDITOR



#endif // __RX_GPU_BUILD_CONFIG_H__
