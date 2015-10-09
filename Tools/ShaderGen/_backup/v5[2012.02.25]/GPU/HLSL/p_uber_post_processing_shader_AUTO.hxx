// p_uber_post_processing_shader_AUTO.hxx
// derived from 'D:/_/Development/SourceCode/Renderer/GPU/source/p_post_processing.fx' (248)
// Defines:
//	bEnable_Bloom = 1
//	bEnable_HDR = 1
//	bEnable_DOF = 1
//	bEnable_SSAO = 1
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
cbuffer PerView : register(b1)
{
	float4x4 viewMatrix;
	float4x4 viewProjectionMatrix;
	float4x4 inverseViewMatrix;
	float4x4 projectionMatrix;
	float4x4 inverseProjectionMatrix;
	float4 tanHalfFoV;
	float4 depthClipPlanes;
	float4 frustumCornerVS_FarTopLeft;
	float4 frustumCornerVS_FarTopRight;
	float4 frustumCornerVS_FarBottomLeft;
	float4 frustumCornerVS_FarBottomRight;
};

#define PER_VIEW_CONSTANTS_DEFINED
#include "h_base.h"

float3 GetCameraRightVector() { return inverseViewMatrix[0].rgb; }
float3 GetCameraUpVector() { return inverseViewMatrix[1].rgb; }
float3 GetLookDirection() { return inverseViewMatrix[2].rgb; }
// returns camera position in world space
float3 GetEyePositionWS() { return inverseViewMatrix[3].rgb; }

float GetNearZ() { return depthClipPlanes.x; }
float GetFarZ() { return depthClipPlanes.y; }
float GetInverseNearZ() { return depthClipPlanes.z; }
float GetInverseFarZ() { return depthClipPlanes.w; }
Texture2D RT_normal_specMul : register(t0);
Texture2D RT_diffuse_specExp : register(t1);
Texture2D RT_linearDepth : register(t2);

#define RX_GBUFFER_READS_ENABLED
#include "h_geometry_buffer.h"

Texture2D sceneColorTexture : register(t3);
Texture2D sceneDepthTexture : register(t4);
Texture2D blurredSceneTexture : register(t5);
Texture2D randomNormalsTexture : register(t6);

#include "h_screen_shader.h"
#include "h_post_processing.h"


static const float sampleRadius = 0.1;//0.01;
static const float distanceScale = 1;

#define NUMSAMPLES 8

//Sample Vectors
static const float4 samples[ NUMSAMPLES ] =
{
	float4(0.355512, 	-0.709318, 	-0.102371,	0.0 ),
	float4(0.534186, 	0.71511, 	-0.115167,	0.0 ),
	float4(-0.87866, 	0.157139, 	-0.115167,	0.0 ),
	float4(0.140679, 	-0.475516, 	-0.0639818,	0.0 ),
	float4(-0.207641, 	0.414286, 	0.187755,	0.0 ),
	float4(-0.277332, 	-0.371262, 	0.187755,	0.0 ),
	float4(0.63864, 	-0.114214, 	0.262857,	0.0 ),
	float4(-0.184051, 	0.622119, 	0.262857,	0.0 )
};


static const float SSAODepthScale = 1.0f;
static const float SSAOAngleBias = 0.1f;
static const float SSAOIntensity = 3.0f;
static const float SSAORadius = 0.01f;

float3 getPosition( in float2 uv )
{
	const float pixelDepth = UnpackDepth( sceneDepthTexture.Sample( pointSampler, uv ).r );

	//GPixel gpixel = FetchGBuffer(uv.xy);
	//return gpixel.WorldPos.xyz;

	//const float3 positionVS  = eyeRayVS * pixelDepth;
	//return positionVS;
	// 
	const float3 positionVS  = RestoreVSPositionFromDepth( pixelDepth, TexCoordsToClipPos(uv) );
	const float3 positionWS  = mul( float4(positionVS,1), inverseViewMatrix ).xyz;
	return positionWS;
}

float doAmbientOcclusion( in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm ) 
{ 
	float ssaoScale		= SSAODepthScale;
	float ssaoBias		= SSAOAngleBias;
	float ssaoIntensity = SSAOIntensity;

	float3 diff = getPosition( tcoord + uv ) - p; 
	const float3 v = normalize(diff); 
	const float d = length(diff) * ssaoScale; 
	return max(0.0,dot(cnorm,v)-ssaoBias)*(1.0/(1.0+d))*ssaoIntensity; 
} 


float4 PS_Main( in VS_ScreenQuadOutput input ) : SV_Target
{
	//return sceneColorTexture.Sample( pointSampler, input.texCoord );
	//return PS_DOF_Comp( input, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );
	//return randomNormalsTexture.Sample( pointSampler, input.texCoord ) * 0.5;




	//GPixel gbuffer = FetchGBuffer(IN.Tex);

	//float3 worldPos		= gbuffer.WorldPos.xyz;
	//float3 worldNorm	= gbuffer.WorldNorm.xyz;

	GSurface	surface;
	ReadSurfaceData( input.texCoord, surface );

	const float pixelDepth = UnpackDepth( RT_linearDepth.Sample( pointSampler, input.texCoord ).r );
	const float3 positionVS  = input.eyeRayVS * pixelDepth;

	float3 worldPos		= mul( float4(positionVS,1), inverseViewMatrix ).xyz;
//return float4( (float3)worldPos, 1.0f);
	float3 worldNorm	=  mul( surface.normal, (float3x3)inverseViewMatrix ).xyz;
//return float4( (float3)worldNorm, 1.0f);

	float ao = 0.0f; 
	float rad = SSAORadius;
	const float2 ssaoOffsets[4] = { float2(1,0), float2(-1,0), float2(0,1), float2(0,-1) }; 

	int iterations = 4; 
	for (int j = 0; j < iterations; ++j) 
	{ 
		float2 coord1 = ssaoOffsets[j] * rad;
		float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707); 
		ao += doAmbientOcclusion( input.texCoord, coord1*0.25,  worldPos, worldNorm); 
		ao += doAmbientOcclusion( input.texCoord, coord2*0.5,   worldPos, worldNorm); 
		ao += doAmbientOcclusion( input.texCoord, coord1*0.75,  worldPos, worldNorm); 
		ao += doAmbientOcclusion( input.texCoord, coord2,       worldPos, worldNorm); 
	}

	ao /=(float)iterations*4.0; 
	ao = (1.0f - ao);


	float3 rgb = sceneColorTexture.Sample(pointClampSampler, input.texCoord).rgb;
	//return float4(ao.xxx * rgb, 1.0f);

	ao *= 0.7f;

	return float4( (float3)ao.xxx, 1.0f);




#if 0

	const float pixelDepth = UnpackDepth( sceneDepthTexture.Sample( pointSampler, input.texCoord ).r );

	//return pixelDepth * GetInverseFarZ();


	//Sample a random normal vector
	//float3 randNormal = randomNormalsTexture.Sample( pointSampler, input.texCoord /* 200.0f*/ ).xyz;
	//randNormal = normalize(randNormal);
	const float3 randNormal = float3(0,1,0);


	//return float4(randNormal.xyz,1);

	//Sample the Normal for this pixel

	GSurface	surface;
	ReadSurfaceData( input.texCoord, surface );

	// view-space normal
	const float3 normal = surface.normal;


	const float3 positionVS  = input.eyeRayVS * pixelDepth;


	//return PS_DOF_Comp( input.texCoord, sceneColorTexture, blurredSceneTexture, sceneDepthTexture );
	return float4(positionVS,1);

#endif








#if 0

#define ONE_OVER_RAND_TEXTURE_SIZE 0.000976563
#define MAX_RAND_SAMPLES 14

static const float3 RAND_SAMPLES[MAX_RAND_SAMPLES] =
{
	float3(1, 0, 0),
	float3(	-1, 0, 0),
	float3(0, 1, 0),
	float3(0, -1, 0),
	float3(0, 0, 1),
	float3(0, 0, -1),
	normalize(float3(1, 1, 1)),
	normalize(float3(-1, 1, 1)),
	normalize(float3(1, -1, 1)),
	normalize(float3(1, 1, -1)),
	normalize(float3(-1, -1, 1)),
	normalize(float3(-1, 1, -1)),
	normalize(float3(1, -1, -1)),
	normalize(float3(-1, -1, -1))
};

#endif




#if 0
	float3 se = positionVS;

	float finalColor = 0.0f;


	//SSAO loop
	[unroll]
	for (int i = 0; i < NUMSAMPLES; i++)
	{
		//Calculate the Reflection Ray
		float3 ray = reflect( samples[i].xyz, randNormal ) * sampleRadius;

		//Test the Reflection Ray against the surface normal
		if( dot(ray, normal) < 0 ) {
			ray += normal * sampleRadius;
		}

		//Calculate the Sample vector
		float4 sample = float4( se + ray, 1.0f );

		//Project the Sample vector into ScreenSpace
		float4 ss = mul(sample, projectionMatrix);
		ss.xy /= ss.w;

		//Convert SS into UV space
		//float2 sampleTexCoord = 0.5f * ss.xy / ss.w + float2(0.5f, 0.5f);
		float2 sampleTexCoord = ClipPosToTexCoords(  ss.xy );

		//Sample the Depth along the ray
		//float sampleDepth = tex2D(GBuffer2, sampleTexCoord).g;
		float sampleDepth = UnpackDepth( sceneDepthTexture.Sample( pointSampler, sampleTexCoord ).r );

		//Check the sampled depth value
		if (sampleDepth == 1.0)
		{
			//Non-Occluded sample
			finalColor++;
		}
		else
		{	
			//Calculate Occlusion
			float occlusion = distanceScale * max(sampleDepth - pixelDepth, 0.0f);

			//Accumulate to finalColor
			finalColor += 1.0f / (1.0f + occlusion * occlusion * 0.1);
		}
	}

	//Output the Average of finalColor
	//return float4(finalColor / NUMSAMPLES, finalColor / NUMSAMPLES, finalColor / NUMSAMPLES, 1.0f);
	return (float4) (finalColor / NUMSAMPLES);
#endif
}

