// p_fullscreen_colored_triangle_shader_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_screen_shader.fx' (28)

cbuffer Data : register(b0)
{
	float4 color;
};

#include "h_screen_shader.h"

float4 PS_Main( in VS_ScreenOutput input ) : SV_Target
{
	return color;
}
