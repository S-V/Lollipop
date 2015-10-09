Shader p_fullscreen_textured_triangle_shader
{
	Inputs
	{
		SamplerState linearSampler = SS_Bilinear;
		Texture2D sourceTexture;
	}
	Code
	{
		#include "h_screen_shader.hlsl"
		
		float3 PS_Main( in VS_ScreenOutput input ) : SV_Target
		{
			float3 outputColor = 0.0f;
			outputColor = sourceTexture.Sample( linearSampler, input.texCoord ).rgb;
			return outputColor;
		}
	}

}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}



Shader p_fullscreen_colored_triangle_shader
{
	Inputs
	{
		cbuffer Data
		{
			float4 color;
		};
	}
	Code
	{
		#include "h_screen_shader.hlsl"
		
		float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
		{
			return color;
		}
	}

}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

/*
Shader p_screen_shader
{
	#include "h_screen_shader.hlsl"
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