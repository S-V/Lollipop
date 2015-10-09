// p_shadow_mapping.fx

/*
=============================================================================
	p_build_hw_shadow_map
=============================================================================
*/
// depth-only shader
Shader p_build_hw_shadow_map
{
Inputs : Globals View
{
cbuffer Data(VS)
{
	float4x4 lightWVP;
};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSIn
{
	float3	position : Position;
};

struct VSOut
{
	float4	posH : SV_Position;
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
//	IN.xyz += input.normal * u_bias; // reduce self-shadowing

	float4 posLocal = float4( IN.position, 1 );

	OUT.posH = mul( posLocal, lightWVP );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		nil
}



/*
=============================================================================
	p_build_hw_shadow_map_with_alpha
=============================================================================
* /
Shader p_build_hw_shadow_map_with_alpha
{
Inputs : Globals View
{
cbuffer Data(VS)
{
	float4x4 lightWVP;
};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSIn
{
	float3	position : Position;
};

struct VSOut
{
	float4	posH : SV_Position;
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
//	IN.xyz += input.normal * u_bias; // reduce self-shadowing

	float4 posLocal = float4( IN.position, 1 );

	OUT.posH = mul( posLocal, lightWVP );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}

*/

