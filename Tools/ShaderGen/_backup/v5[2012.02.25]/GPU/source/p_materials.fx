/*
=============================================================================
	File:	p_materials
	Desc:	
=============================================================================
*/

/*
-----------------------------------------------------------------------------
	p_material_plain_color
-----------------------------------------------------------------------------
*/
Shader p_material_plain_color
{
Inputs : Globals View Object
{
	cbuffer Data(PS)
	{
		float4		materialDiffuseColor;	// rgb - color, a - spec. intensity [0..1]
		float4		materialSpecularColor;	// rgb - color, a - spec. power [0..1]
		//float4		materialEmissiveColor;
	};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position;
	float3	normalVS : Normal0;
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;

	float3 normalOS = UnpackVertexNormal( IN.normal.xyz );
	OUT.normalVS = Transform_Dir_ObjectToView( normalOS );
}

void PS_Main( in VSOut IN, out PS_Out_GBuffer OUT )
{
	GBufferInput		s;

	s.position = IN.positionVS;
	s.normal = normalize(IN.normalVS);		// view-space normal

	s.albedo = materialDiffuseColor.rgb;

	s.specularAmount = materialDiffuseColor.a;
	s.specularPower = materialSpecularColor.a;	// normalized, in range [0..1]

	PackSurfaceData( s, OUT );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}

/*
-----------------------------------------------------------------------------
	p_material_phong
-----------------------------------------------------------------------------
*/
Shader p_material_phong
<
	bHasBaseMap = 0
	bHasNormalMap = 0
	bHasSpecularMap = 0
>
{
Inputs : Globals View Object
{
	// material data for deferred pass
	cbuffer Data(PS)
	{
		float4		materialDiffuseColor;	// rgb - color, a - spec. intensity normalized to range [0..1]
		float4		materialSpecularColor;	// rgb - color, a - spec. power normalized to range [0..1]
		float4		materialEmissiveColor;
	};

	Texture2D<float4>	baseMap(PS);
	Texture2D<float4>	normalMap(PS);
	Texture2D<float4>	specularMap(PS);

}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_vertex_lib.h"
#include "h_geometry_buffer.h"

struct VSOut
{
	float4	position : SV_Position;
	float3	positionVS : Position;
	float3	normalVS : Normal0;

	float2	uv : TexCoord0;

#if bHasNormalMap
	float3  tangentVS : Tangent;	// view-space tangent
	float3  bitangentVS : Binormal;	// view-space binormal
#endif
};

void VS_Main( in Vertex_P3f_TEX2f_N4Ub_T4Ub IN, out VSOut OUT )
{
	float4 posLocal = float4( IN.position, 1 );

	OUT.position = Transform_ObjectToClip( posLocal );
	OUT.positionVS = Transform_ObjectToView( posLocal ).xyz;

	float3 N = UnpackVertexNormal( IN.normal.xyz );
	N = Transform_Dir_ObjectToView( N );
	OUT.normalVS = N;

	OUT.uv = IN.texCoords;

#if bHasNormalMap
	float3 T = UnpackVertexNormal( IN.tangent.xyz );
	T = Transform_Dir_ObjectToView( T );
	T = normalize( T - dot( T, N ) * N );
	float3 B = cross( N, T );

	OUT.tangentVS = T;
	OUT.bitangentVS = B;
#endif
}

void PS_Main( in VSOut IN, out PS_Out_GBuffer OUT )
{
	GBufferInput		s;

	s.position = IN.positionVS;


#if bHasNormalMap
	float3 normalT = normalMap.Sample( colorMapSampler, IN.uv ).rgb;
	normalT = ExpandNormal(normalT);

	float3 bumpedNormal = IN.tangentVS * normalT.x
						+ IN.bitangentVS * normalT.y
						+ IN.normalVS * normalT.z;
	bumpedNormal = normalize( bumpedNormal );

	s.normal = bumpedNormal;
#else
	s.normal = normalize( IN.normalVS );
#endif


#if bHasBaseMap
	s.albedo = baseMap.Sample( colorMapSampler, IN.uv ).rgb;
#else
	s.albedo = materialDiffuseColor.rgb;
#endif


	s.specularAmount = materialDiffuseColor.a;
	s.specularPower = materialSpecularColor.a;	// normalized, in range [0..1]

	PackSurfaceData( s, OUT );
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}

