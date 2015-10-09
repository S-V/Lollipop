#ifndef VERTEX_LIB_HLSL
#define VERTEX_LIB_HLSL

#include "h_vertex_lib_AUTO.h"

// this vertex format is used for rendering static meshes
#define Vertex_Static	Vertex_P3f_TEX2f_N4Ub_T4Ub


// normal: 4 bytes -> 3 floats
float3 UnpackVertexNormal( in float3 packedNormal )
{
	return packedNormal * (1.0f / 127.5f) - 1.0f;
}

#endif // !VERTEX_LIB_HLSL

