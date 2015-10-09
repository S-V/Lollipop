Shader p_fullscreen_textured_triangle_shader
{
Inputs
{
	SamplerState linearSampler(PS) = SS_Bilinear;
	Texture2D sourceTexture(PS);
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"

float3 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float3 outputColor = 0.0f;
	outputColor = sourceTexture.Sample( linearSampler, input.texCoord ).rgb;
	return outputColor;
}
}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

//---------------------------------------------------------------------------

Shader p_fullscreen_colored_triangle_shader
{
Inputs
{
	cbuffer Data(PS)
	{
		float4 color;
	};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_screen_shader.h"

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	return color;
}
}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

//---------------------------------------------------------------------------
/*
Shader p_text_shader
{
Inputs
{
	cbuffer Data
	{
		//float2 textureSize;
		//float2 viewportSize;
		float4 textColor;
	};
	SamplerState fontTextureSampler = SS_Point;
	Texture2D fontTexture;
}//Inputs
//---------------------------------------------------------------------------
Code
{
struct VS_in
{
	float2	xy : Position;
	float2	uv : TexCoord;
};
struct VS_out
{
	float4 posH : SV_Position;
	float2 uv : TexCoord;
};
VS_out VS_Main( in VS_in IN )
{
	VS_out	OUT;
	OUT.posH = float4( IN.xy, 0, 1 );
	OUT.uv = IN.uv;
	return OUT;
}
float4 PS_Main( in VS_out IN ) : SV_Target
{

	float4 c = fontTexture.Sample( fontTextureSampler, IN.uv );
	c *= textColor;
	return c;
}
}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
*/

/*
Shader p_screen_shader
{
	#include "h_screen_shader.h"
}={
	VertexShader	ScreenVertexShader
	PixelShader		ScreenPixelShader
}

vertex ScreenVertex
{
	vertex_shader "p_screen_shader"
	{
		SemanticName			"Position"
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32A32_FLOAT
		AlignedByteOffset		0
		InputSlot				0
		InputSlotClass			D3D11_INPUT_PER_VERTEX_DATA
		InstanceDataStepRate	0
	}
	{
		SemanticName			"TexCoord"
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32_FLOAT
		AlignedByteOffset		D3D11_APPEND_ALIGNED_ELEMENT
	}
}
*/
