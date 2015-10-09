/*
-----------------------------------------------------------------------------
	p_batched_lines
-----------------------------------------------------------------------------
*/
Shader p_batched_lines
<
	bTextureMap = 0
>
{
Inputs
{
cbuffer Data(VS)
{
	float4x4 transform;
};
SamplerState	linearSampler(PS) = SS_Bilinear;
Texture2D		textureMap(PS);
}//Inputs--------------------------------------------------------------------
Code
{
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
}//Code----------------------------------------------------------------------

}={
	VertexShader	VSMain
	PixelShader		PSMain
}


/*
-----------------------------------------------------------------------------
	p_hit_proxy
-----------------------------------------------------------------------------
*/
Shader p_hit_proxy
<
	bEnableColorWrites = 0
>
{
Inputs
{
cbuffer Data(VS PS)
{
	float4x4 transform;
	float4	hitProxyId;
};
}//Inputs
//---------------------------------------------------------
Code
{

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

}//Code
//---------------------------------------------------------
}={
	VertexShader	VSMain
	PixelShader		PSMain
}




