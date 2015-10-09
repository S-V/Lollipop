/*
-----------------------------------------------------------------------------
	Deferred pass
-----------------------------------------------------------------------------
* /
Shader p_deferred_fill_buffers
{
Inputs : Globals View
{
	// frequently updated constants
	cbuffer Object
	{
		float4x4 worldMatrix;	// object-to-world 
		float4x4 worldViewMatrix;
		float4x4 worldViewProjectionMatrix;
	};
	cbuffer Material
	{
		float4 diffuse;
		float4 specular;
		float4 emissive;
	};

	Texture2D	diffuseTexture;
	Texture2D	normalsTexture;
}
Code
{
#include "h_common_vertex_types.hlsl"
#include "h_geometry_buffer.h"

void VS_Main( in Vertex_Static input, out PSIn o )
{
	float4 posLocal = float4( input.xyz, 1 );

	o.posVS		= mul( posLocal, worldViewMatrix ).xyz;
	o.posH		= mul( posLocal, worldViewProjectionMatrix );

	// vertex's normal in local space
	float3 N = UnpackVertexNormal( input.normal.xyz );

	o.normalVS = mul( N, (float3x3)worldViewMatrix );
	o.normalWS = mul( N, (float3x3)worldMatrix );
}

void PS_Main( in PSIn input, out PS_Out_GBuffer o )
{
	SurfaceData		s;

	s.position = input.posVS;

	s.normal = normalize(input.normalVS);

	s.diffuseColor = float3(1,1,1);

	s.specularIntensity 	= materialSpecularIntensity;
	s.specularPower			= materialSpecularPower;

	PackSurfaceData( s, o );
}
}

}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
*/

/*
-----------------------------------------------------------------------------
	Deferred pass
-----------------------------------------------------------------------------
*/
/*
// Rasterizes GBuffers
Technique T_Deferred_Fill_Buffers
{
	Pass P0
	{
		State_Opaque;
	}
	Pass Fill_Buffers
	{
	}
}
*/




/*
-----------------------------------------------------------------------------
*/
Shader p_deferred_fill_buffers_fallback
{
Inputs : Globals View Object
{
	//cbuffer Data
	//{
	//	float4 color;
	//};
}
Code
{
#include "h_common_vertex_types.hlsl"
#include "h_geometry_buffer.h"

void VS_Main( in float3 xyz : Position, out float4 posH : SV_Position )
{
	float4 posLocal = float4( xyz, 1 );

	posH = mul( posLocal, worldViewProjectionMatrix );
}

void PS_Main( in float4 posH : SV_Position, out PS_Out_GBuffer o )
{
	SurfaceData	s;

	s.position = float3(0,0,0);
	s.normal = float3(1,1,1);
	s.specularIntensity = 1.0f;
	s.diffuseColor = float3(1,1,1);
	s.specularPower = 0.5f;

	PackSurfaceData(s,o);
}

}//Code

}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
