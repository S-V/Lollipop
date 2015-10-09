/*
=============================================================================
	p_debug_gbuffer_show_diffuse
=============================================================================
*/
Shader p_debug_gbuffer_show_diffuse
{
Inputs : Globals View GBuffer
{
}//Inputs
Code
{
#include "h_screen_shader.h"
#include "h_geometry_buffer.h"

float3 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float3 diffuse = SampleAlbedo( input.texCoord );
	return diffuse;
}

}//Code
}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}

/*
=============================================================================
	p_sprite_text
=============================================================================
* /
program p_sprite_text
{
	#include "base.fxh"

	Texture2D	g_texDiffuse : register(t0);
	SamplerState g_spriteSampler : register(s0);

	void VS_Main(
		in float3 inPos : Position,
		in float4 inColor : Color,
		in float2 inTexCoord : TexCoord,
		out float4 posH : SV_Position,
		out float2 uv : TexCoord,
		out float4 color : Color
	)
	{
		posH = float4( inPos, 1.0f );
		uv = inTexCoord;
		color = inColor;
	}
	
	float4 PS_Main(
		in float4 posH : SV_Position,
		in float2 uv : TexCoord,
		in float4 color : Color
		) : SV_Target
	{
		float4 diffuseColor = g_texDiffuse.Sample( g_spriteSampler, uv ) * color;
		return diffuseColor;
	}
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}

RasterizerState RS_Sprite
{
	AntialiasedLineEnable = false;
	CullMode = none;
	DepthBias = 0;
	DepthBiasClamp = 1.0f;
	DepthClipEnable = true;
	FillMode = Solid;
	FrontCounterClockwise = false;
	MultisampleEnable = false;
	ScissorEnable = false;
	SlopeScaledDepthBias = 0.0f;
}
BlendState BS_SpriteBlend
{
	AlphaToCoverageEnable		false
	IndependentBlendEnable		false
	
	BlendEnable[0]				true

	SrcBlend[0]					src_alpha
	DestBlend[0]				inv_src_alpha
	BlendOp[0]					add

	SrcBlendAlpha[0]			one
	DestBlendAlpha[0]			zero
	BlendOpAlpha[0]				add

	RenderTargetWriteMask[0]	color_write_enable_all
}

DepthStencilState DS_DisableDepth
{
	DepthEnable		false
}

SamplerState SS_Sprite
{
	Filter			MIN_MAG_LINEAR_MIP_POINT
	AddressU		Wrap
	AddressV		Wrap
};

/*
technique RenderUI
{
	Shader					p_debug_sprite_text
	DepthStencilState		( DisableDepth, 0 )
	BlendState				( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF )
}
*/
