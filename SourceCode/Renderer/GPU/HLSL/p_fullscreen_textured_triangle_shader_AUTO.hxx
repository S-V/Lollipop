// p_fullscreen_textured_triangle_shader_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_screen_shader.fx' (1)

SamplerState linearSampler : register(s0);
Texture2D sourceTexture : register(t0);

#include "h_screen_shader.h"

float3 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	float3 outputColor = 0.0f;
	outputColor = sourceTexture.Sample( linearSampler, input.texCoord ).rgb;
	return outputColor;
}
