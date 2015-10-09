// p_vertical_blur_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (49)
cbuffer PerFrame : register(b0)
{
	float4 screenSize_invSize;
	float1 globalTimeInSeconds;
};
SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState anisotropicSampler : register(s2);
SamplerState pointClampSampler : register(s3);
SamplerState linearClampSampler : register(s4);
SamplerState colorMapSampler : register(s5);
SamplerState detailMapSampler : register(s6);
SamplerState normalMapSampler : register(s7);
SamplerState specularMapSampler : register(s8);
SamplerState attenuationSampler : register(s9);
SamplerState cubeMapSampler : register(s10);
SamplerComparisonState shadowMapSampler : register(s11);
SamplerComparisonState shadowMapPCFSampler : register(s12);
SamplerComparisonState shadowMapPCFBilinearSampler : register(s13);

#include "h_base.h"

// returns render target size (backbuffer dimensions)
float2 GetViewportSize() { return screenSize_invSize.xy; }
// returns texel size
float2 GetInvViewportSize() { return screenSize_invSize.zw; }

Texture2D sourceTexture : register(t0);

#include "h_screen_shader.h"
#include "h_post_processing.h"


VS_OUTPUT_BLUR VS_Main( in uint vertexID: SV_VertexID )
{
    float4 position;
    float2 texCoord;
	GetFullScreenTrianglePosTexCoord( vertexID, position, texCoord );
    return VS_Blur( position, texCoord, NUM_BLUR_SAMPLES, float2(0, 1) );
}

float4 PS_Main( in VS_OUTPUT_BLUR input ) : SV_Target
{
	return PS_Blur7( input, sourceTexture, linearSampler, weights7 );
}


