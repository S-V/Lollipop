// p_batched_lines_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_editor_shaders.fx' (6)
// Defines:
//	bTextureMap = 0

cbuffer Data : register(b0)
{
	float4x4 transform;
};
SamplerState linearSampler : register(s0);
Texture2D textureMap : register(t0);

struct VSIn
{
	float3	xyz : Position;
	float2	uv : TexCoord;
	float4	color : Color;
};
struct PSIn
{
	float4 posH : SV_Position;
	float2 uv : TexCoord;
	float4 color : Color;
};
void VSMain( in VSIn IN, out PSIn OUT )
{
	OUT.posH = mul( float4( IN.xyz, 1 ), transform );
	OUT.uv = IN.uv;
	OUT.color = IN.color;
}
float4 PSMain( in PSIn IN ) : SV_Target
{
	float4 outputColor = IN.color;

#if bTextureMap
	outputColor *= textureMap.Sample( linearSampler, IN.uv ).rgba;
#endif

	clip( outputColor.a - 0.6 );

	return outputColor;
}
