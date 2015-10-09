// p_sky_shaders.fx

/*
=============================================================================
	p_sky_dome
=============================================================================
*/
Shader p_sky_dome
<
	//bBlendTextureMap = 0
	bDrawSun = 0
>
{
Inputs : Globals View
{
	cbuffer Data(VS PS)
	{
		// multiplied with viewProjectionMatrix
		// (sky geometry is moving with the player)
		float4x4	skyDomeTransform;

		float4		sunDirection;

		// x - sky color bias, yzw - unused
		float4		skyParams;

		// scattering coefs, etc.
	};

	SamplerState	skyTextureSampler(PS) = SS_Bilinear;
	Texture2D		skyTexture(PS);

}//Inputs
//---------------------------------------------------------------------------
Code
{
struct VS_IN
{
	float3	position : Position;
	float2	texCoord : TexCoord;
};
struct VS_OUT
{
	float4 position : SV_Position;
	float2 texCoord : TexCoord;
	//float4 color : Color;
};

void VS_Main( in VS_IN IN, out VS_OUT OUT )
{
	OUT.position = mul( float4( IN.position, 1 ), skyDomeTransform );
	OUT.position.w = OUT.position.z;

	OUT.texCoord = IN.texCoord;
	//OUT.color = IN.color;
}

float3 PS_Main( in VS_OUT IN ) : SV_Target
{
	float3 skyColor = skyTexture.Sample( skyTextureSampler, IN.texCoord ).rgb;
	skyColor *= 1.2;
	return skyColor;
}

}//Code
//---------------------------------------------------------------------------
}={
	VertexShader	VS_Main
	PixelShader		PS_Main
}
