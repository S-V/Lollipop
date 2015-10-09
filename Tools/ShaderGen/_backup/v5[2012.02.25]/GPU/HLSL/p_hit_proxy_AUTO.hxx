// p_hit_proxy_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_editor_shaders.fx' (65)
// Defines:
//	bEnableColorWrites = 0

cbuffer Data : register(b0)
{
	float4x4 transform;
	float4 hitProxyId;
};


struct VSIn
{
	float3	xyz : Position;
#if bEnableColorWrites
	float4	color : Color;
#endif
};

struct PSIn
{
	float4 posH : SV_Position;
#if bEnableColorWrites
	float4	color : Color;
#endif
};

void VSMain( in VSIn IN, out PSIn OUT )
{
	OUT.posH = mul( float4( IN.xyz, 1 ), transform );
#if bEnableColorWrites
	OUT.color = IN.color;
#endif
}

struct PSOut
{
	float4	pixelId : SV_Target0;
#if bEnableColorWrites
	float4	color : SV_Target1;
#endif
};

void PSMain( in PSIn IN, out PSOut OUT )
{
#if bEnableColorWrites
	clip( IN.color.a - 0.5 );
#endif

	OUT.pixelId = hitProxyId;
#if bEnableColorWrites
	OUT.color = IN.color;
#endif
}

