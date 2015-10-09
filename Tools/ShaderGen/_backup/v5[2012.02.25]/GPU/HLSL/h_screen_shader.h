/*
=============================================================================
	File:	h_screen_shader.h
	Desc:	
=============================================================================
*/
#ifndef H_SCREEN_SHADER_HLSL
#define H_SCREEN_SHADER_HLSL

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

inline void GetFullScreenTrianglePosTexCoord(
	in uint vertexID: SV_VertexID,
	out float4 position,
    out float2 texCoord
	)
{
	// Produce a fullscreen triangle.
	// NOTE: z = 1 (the farthest possible distance) -> optimization for deferred lighting/shading engines:
	// skybox outputs z = w = 1 -> if we set depth test to 'less' then the sky won't be shaded.
	// or you can just set your viewport so that the skybox is clamped to the far clipping plane, which is done by setting MinZ and MaxZ to 1.0f.
	//
	if( vertexID == 0 )// lower-left
	{
		position = float4( -1.0f, -3.0f, 1.0f, 1.0f );
		texCoord = float2( 0.0f, 2.0f );
	}
	else if ( vertexID == 1 )// upper-left
	{
		position = float4( -1.0f,  1.0f, 1.0f, 1.0f );
		texCoord = float2( 0.0f, 0.0f );
	}
	else //if ( vertexID == 2 )// upper-right
	{
		position = float4(  3.0f,  1.0f, 1.0f, 1.0f );
		texCoord = float2( 2.0f, 0.0f );
	}
	/*
	another way to calc position (from Intel's Deferred Shading sample, 2010):

	struct FullScreenTriangleVSOut
	{
		float4 positionViewport : SV_Position;
	};

	// Parametrically work out vertex location for full screen triangle
	FullScreenTriangleVSOut output;
	float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
	output.positionViewport = float4(grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 1.0f, 1.0f);

	or yet another one:
	Out.position.x = (vertexID == 2)?  3.0 : -1.0;
	Out.position.y = (vertexID == 0)? -3.0 :  1.0;
	Out.position.zw = 1.0;
	*/
}

//****************************************************************************************
/**
	FullScreenTriangle_VS

	rendered as a single triangle:

	ID3D11DeviceContext* context;

	(uses no vertex/index buffers)

	context->IASetInputLayout(nil);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3,0);
*/
VS_ScreenOutput FullScreenTriangle_VS( in uint vertexID: SV_VertexID )
{
    VS_ScreenOutput output;

	GetFullScreenTrianglePosTexCoord( vertexID, output.position, output.texCoord );

    return output;
}

//****************************************************************************************

inline void GetFullScreenQuadPosTexCoord(
	in uint vertexID: SV_VertexID,
	out float4 position,
    out float2 texCoord
	)
{
	// Produce a fullscreen triangle.
	// NOTE: z = 1 (the farthest possible distance) -> optimization for deferred lighting/shading engines:
	// skybox outputs z = w = 1 -> if we set depth test to 'less' then the sky won't be shaded.
	// or you can just set your viewport so that the skybox is clamped to the far clipping plane, which is done by setting MinZ and MaxZ to 1.0f.
	//
	if( vertexID == 0 )// upper-left
	{
		position = float4( -1.0f,  1.0f, 1.0f, 1.0f );
		texCoord = float2( 0.0f, 0.0f );
	}
	else if ( vertexID == 1 )// lower-left
	{
		position = float4( -1.0f, -1.0f, 1.0f, 1.0f );
		texCoord = float2( 0.0f, 1.0f );
	}
	else if ( vertexID == 2 )// upper-right
	{
		position = float4(  1.0f,  1.0f, 1.0f, 1.0f );
		texCoord = float2( 1.0f, 0.0f );
	}
	else //if ( vertexID == 3 )// lower-right
	{
		position = float4(  1.0f, -1.0f, 1.0f, 1.0f );
		texCoord = float2( 1.0f, 1.0f );
	}
}


#ifdef PER_VIEW_CONSTANTS_DEFINED

/**
	FullScreenQuad_VS
	rendered as a triangle strip:

	ID3D11DeviceContext* context;

	(uses no vertex/index buffers)

	context->IASetInputLayout(nil);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4,0);
*/
struct VS_ScreenQuadOutput
{
	float4 position : SV_Position;
	float2 texCoord : TexCoord0;
	float3 eyeRayVS : TexCoord1;	// ray to far frustum corner in view space, used for restoring view-space pixel position from depth
	//float3 eyeRayWS : TexCoord2;	// ray to far frustum corner in world space, used for restoring world-space pixel position from depth
};

VS_ScreenQuadOutput FullScreenQuad_VS( in uint vertexID: SV_VertexID )
{
	VS_ScreenQuadOutput output;

	GetFullScreenQuadPosTexCoord( vertexID, output.position, output.texCoord );

	if( vertexID == 0 )// upper-left
	{
		output.eyeRayVS = frustumCornerVS_FarTopLeft.xyz;
	}
	else if ( vertexID == 1 )// lower-left
	{
		output.eyeRayVS = frustumCornerVS_FarBottomLeft.xyz;
	}
	else if ( vertexID == 2 )// upper-right
	{
		output.eyeRayVS = frustumCornerVS_FarTopRight.xyz;
	}
	else //if ( vertexID == 3 )// lower-right
	{
		output.eyeRayVS = frustumCornerVS_FarBottomRight.xyz;
	}

	//@fixme: this is wrong:
	//output.eyeRayWS = mul( float4( output.eyeRayVS, 0.0f ), inverseViewMatrix ).xyz;
	//output.eyeRayWS = normalize( output.eyeRayWS );

	return output;
}
#endif // PER_VIEW_CONSTANTS_DEFINED
//****************************************************************************************

#endif // H_SCREEN_SHADER_HLSL
