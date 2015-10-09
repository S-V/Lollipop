RasterizerState RS_WireframeNoCullNoClip
{
	FillMode	Wireframe
	CullMode	None
	DepthClipEnable	false
	MultisampleEnable	false
}


/*
-----------------------------------------------------------------------------
	Deferred pass
-----------------------------------------------------------------------------
*/
Shader p_batched_lines
{
	Inputs
	{
		cbuffer Data
		{
			float4x4 viewProjectionMatrix;
			float4 lineColor;
		};
	}
	Code
	{
		struct VSIn
		{
			float4	xyz : Position;
			float4	color : Color;
		};
		struct PSIn
		{
			float4 posH : SV_Position;
			float4 color : Color;
		};
		void VSMain( in VSIn IN, out PSIn OUT )
		{
			OUT.posH = mul( IN.xyz, viewProjectionMatrix );
			OUT.color = IN.color;
		}
		float4 PSMain( in PSIn IN ) : SV_Target
		{
			return IN.color * lineColor;
		}
	}

}={
	VertexShader	VSMain
	PixelShader		PSMain
}



