Shader p_test_shader
{
Inputs : Globals
	{
		cbuffer Data
		{
			float4x4	wvp;
		};

		Texture2D diffuseTexture;
	}
	Code
	{
		
		struct appToVS
		{
			float3 xyz : Position;
			float2 uv : TexCoord;
			//uint4 normal : Normal;
			//uint4 tangent : Tangent;
		};

		struct VS_to_PS
		{
			float4 hPos : SV_Position;
			float2 texCoord : TexCoord;
		};
		void VS_Main( in appToVS IN, out VS_to_PS OUT )
		{
			OUT.hPos = mul(float4(IN.xyz,1),wvp);
			OUT.texCoord = IN.uv;
		}
		float3 PS_Main( in VS_to_PS input ) : SV_Target
		{
			float3 outputColor = 0.0f;
			outputColor = diffuseTexture.Sample( linearSampler, input.texCoord ).rgb;
			return outputColor;
		}
	}

}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
