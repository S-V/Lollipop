/*
-----------------------------------------------------------------------------
	Deferred pass
-----------------------------------------------------------------------------
*/
Shader p_deferred_fill_buffers
{
Inputs : Globals View
{
	// frequently updated constants
	cbuffer cbPerObject
	{
		float4x4 worldMatrix;	// object-to-world 
		float4x4 worldViewMatrix;
		float4x4 worldViewProjectionMatrix;
	};

	SamplerState colorMapSampler;
	SamplerState detailMapSampler;
	SamplerState normalMapSampler;
	SamplerState specularMapSampler;
	SamplerState attenuationSampler;
	SamplerState cubeMapSampler;

	Texture2D	diffuseTexture;
}
Code
{
#include "h_common_vertex_types.hlsl"
#include "g_buffer.hlsl"

void VS_Main( in vtx_static input, out PSIn o )
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


