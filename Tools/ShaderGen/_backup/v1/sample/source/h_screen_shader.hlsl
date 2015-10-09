
//
//	This is used for drawing screen-aligned quads.
//
struct VS_ScreenInput
{
    float4 position : Position;
    float2 texCoord : TexCoord0;
};
struct VS_ScreenOutput
{
    float4 position : SV_Position;
    float2 texCoord : TexCoord0;
};

VS_ScreenOutput ScreenVertexShader( in VS_ScreenInput input )
{
    VS_ScreenOutput output;
    output.position = input.position;
	output.texCoord = input.texCoord;
    return output;
}

//float3 ScreenPixelShader( in VS_ScreenOutput IN ) : SV_Target
//{
//	return float3(1,1,1);
//}


VS_ScreenOutput FullScreenTriangle_VS( in uint vertexID: SV_VertexID )
{
    VS_ScreenOutput output;

	// Produce a fullscreen triangle.
	// NOTE: z = 1 (the farthest possible distance) -> optimization for deferred lighting/shading engines:
	// skybox outputs z = w = 1 -> if we set depth test to less than the sky won't be shaded
	//
	if( vertexID == 0 )// lower-left
	{
		output.position = float4( -1.0f, -3.0f, 1.0f, 1.0f );
		output.texCoord = float2( 0.0f, 2.0f );
	}
	else if ( vertexID == 1 )// upper-left
	{
		output.position = float4( -1.0f,  1.0f, 1.0f, 1.0f );
		output.texCoord = float2( 0.0f, 0.0f );
	}
	else //if ( vertexID == 2 )// upper-right
	{
		output.position = float4(  3.0f,  1.0f, 1.0f, 1.0f );
		output.texCoord = float2( 2.0f, 0.0f );
	}

    return output;
}
