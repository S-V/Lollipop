#ifndef H_POST_PROCESSING_H
#define H_POST_PROCESSING_H

#include "h_helpers.h"
#include "h_screen_shader.h"

mxSWIPED("some post effects taken from NVidia SDKs / Havok Physics SDK");

// scene texture can be downscaled for performance (reduces size by 4).

#if defined(bDownsampled4x4)
	// TexelSize == downsampled target here
	float2 GetTexelSize() { return GetInvViewportSize() * 4; }
#else
	float2 GetTexelSize() { return GetInvViewportSize(); }
#endif

/*
=======================================================================

	Blur / Depth Of Field

=======================================================================
*/

#define NUM_BLUR_SAMPLES		7

static const float BlurWidth <
    string UIName = "Blur width";
    string UIWidget = "Slider";
    float UIMin = 0.0f;
    float UIMax = 10.0f;
    float UIStep = 0.5f;
>
=
2.0f
;

//static const float  g_fMaxDiscRadius = 0.5f;
static const float  g_fMaxDiscRadius = 0.2f;

static const float NUM_POISSON_TAPS = 8;
static const float2 g_Poisson[8] =  
{
    float2(  0.000000f,  0.000000f ),
    float2(  0.527837f, -0.085868f ),
    float2( -0.040088f,  0.536087f ),
    float2( -0.670445f, -0.179949f ),
    float2( -0.419418f, -0.616039f ),
    float2(  0.440453f, -0.639399f ),
    float2( -0.757088f,  0.349334f ),
    float2(  0.574619f,  0.685879f ),
};


// blur filter weights
static const float weights7[NUM_BLUR_SAMPLES] = {
	0.05,
	0.1,
	0.2,
	0.3,
	0.2,
	0.1,
	0.05,
};

static const float2 g_vMaxCoC = float2( 0.03f, 0.01f );
static const float  g_fRadiusScale = 1.0f;





struct VS_OUTPUT_BLUR
{
	float4 Position  : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
	float4 TexCoord12: TEXCOORD1;
	float4 TexCoord34: TEXCOORD2;
	float4 TexCoord56: TEXCOORD3;
};

// generate texcoords for blur
VS_OUTPUT_BLUR VS_Blur(float4 Position : POSITION,
					   float2 TexCoord : TEXCOORD0,
					   uniform int numSamples,
					   uniform float2 direction
					   )
{
	VS_OUTPUT_BLUR OUT = (VS_OUTPUT_BLUR)0;
	const float2 texelSize = GetTexelSize();

	OUT.Position = Position;

	mxOPTIMIZE("precompute blur direction");

	const float2 blurDir = BlurWidth * texelSize * direction;
	const float2 s = TexCoord - (numSamples-1) * 0.5 * blurDir;

	OUT.TexCoord0 = s;
	OUT.TexCoord12.xy = s + (blurDir * 1);
	OUT.TexCoord12.zw = s + (blurDir * 2);
	OUT.TexCoord34.xy = s + (blurDir * 3);
	OUT.TexCoord34.zw = s + (blurDir * 4);
	OUT.TexCoord56.xy = s + (blurDir * 5);
	OUT.TexCoord56.zw = s + (blurDir * 6);

	return OUT;
}


// fx doesn't support variable length arrays
// otherwise we could generalize this
float4 PS_Blur7(VS_OUTPUT_BLUR IN,
			   uniform Texture2D tex,
			   uniform SamplerState sam,
			   uniform float weight[NUM_BLUR_SAMPLES]
)// : SV_Target
{
	float4 c = 0;

	c += tex.Sample( sam, IN.TexCoord0     ) * weight[0];
	c += tex.Sample( sam, IN.TexCoord12.xy ) * weight[1];
	c += tex.Sample( sam, IN.TexCoord12.zw ) * weight[2];
	c += tex.Sample( sam, IN.TexCoord34.xy ) * weight[3];
	c += tex.Sample( sam, IN.TexCoord34.zw ) * weight[4];
	c += tex.Sample( sam, IN.TexCoord56.xy ) * weight[5];
	c += tex.Sample( sam, IN.TexCoord56.zw ) * weight[6];

	return c;
}


// Constants for gaussian blur.

static const int g_cKernelSize = 13;

static const float PixelOffsets[ g_cKernelSize ] =
{
    -6,
    -5,
    -4,
    -3,
    -2,
    -1,
     0,
     1,
     2,
     3,
     4,
     5,
     6,
};

static const float BlurWeights[g_cKernelSize] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

mxOPTIMIZE("precompute texel offsets");

//-----------------------------------------------------------------------------
// Pixel shader applies a one dimensional gaussian blur filter.
// e.g. this is used twice by the bloom post process,
// first to blur horizontally, and then again to blur vertically.
//-----------------------------------------------------------------------------
//
float4 HorizontalGaussianBlur_PS(
					 in float2 texCoord : TEXCOORD0,
					 uniform Texture2D tex,
					 uniform SamplerState sam
					 )
{
	float4 outputColor = 0;

	const float2 texelSize = GetTexelSize();

	// Combine a number of weighted image filter taps.
	[unroll]
	for( int i = 0; i < g_cKernelSize; i++ )
	{
		const float2 texelOffset = float2( PixelOffsets[i], 0 ) * texelSize;
		const float2 sampleTexCoords = texCoord + texelOffset;
		outputColor += tex.Sample( sam, sampleTexCoords ) * BlurWeights[i];
	}

	return outputColor;
}

float4 VerticalGaussianBlur_PS(
					 in float2 texCoord : TEXCOORD0,
					 uniform Texture2D tex,
					 uniform SamplerState sam
					 )
{
	float4 outputColor = 0;

	const float2 texelSize = GetTexelSize();

	// Combine a number of weighted image filter taps.
	[unroll]
	for( int i = 0; i < g_cKernelSize; i++ )
	{
		const float2 texelOffset = float2( 0, PixelOffsets[i] ) * texelSize;
		const float2 sampleTexCoords = texCoord + texelOffset;
		outputColor += tex.Sample( sam, sampleTexCoords ) * BlurWeights[i];
	}

	return outputColor;
}




/*
=======================================================================

	Depth Of Field

	extracts scene depth from geometry buffer

=======================================================================
*/

/*

	Technique developed by Guennidi Riguer.
	The basic idea is to calculate the distance from the focal plane and use this to control the size of a ‘circle of confusion’,
	this circle is used to lookup the image, the bigger the circle the more blurred the final image.

	float ComputeBlur( float depth, float focalDist, float
                    focalRange )
	 {
	   return saturate(abs(depth - focalDist) * focalRange);
	 }

	 float4 RiguerDOF( float focalDist, 
					   float focalRange, 
					   float maxCoC, 
					   float2 tapOffset[NUM_OF_TAPS]
					   float2 uv )
	 {
	   float depth = Gbuffer.Pos[ uv ].z;
	   float blur = ComputeBlur( depth, focalDist,
								 focalRange);
	   float4 colourSum = Lit[ uv ]; 
	   float sizeCoC = blur * maxCoC;
	   float totalContrib = 1.0f;

	   for(i=0;i < NUM_OF_TAPS;i++)
	   {
		 float tapUV = uv + tapOffset[ i ] * maxCoC;
		 float4 tapColour = Lit[ tapUV ];
		 float tapDepth = Gbuffer.Pos[ tapUV ].z;
		 float tapContrib = (tapDepth > depth) ? 1.0f : 
		 ComputeBlur(tapDepth, focalDist, focalRange);
		 colourSum += tapContrib;
		 totalContrib += tapContrib;
	   }

	   return colourSum / totalContribution;
	 }
*/

#ifdef RX_GBUFFER_READS_ENABLED

float ComputeDepthBlur( FLOAT fDepth )
{
    // Compute depth blur
    float fDepthBlur;
    
    if( fDepth < g_fFocalPlaneDistance )
    {
        // Scale depth value between near blur distance and focal distance to [-1,0] range
        fDepthBlur = ( fDepth - g_fFocalPlaneDistance ) / ( g_fFocalPlaneDistance - g_fNearBlurPlaneDistance );
    }
    else
    {
        // Scale depth value between focal distance and far blur distance to [0,1] range
        fDepthBlur = ( fDepth - g_fFocalPlaneDistance ) / ( g_fFarBlurPlaneDistance - g_fFocalPlaneDistance );

        // Clamp the far blur to a maximum blurriness
        fDepthBlur = clamp( fDepthBlur, 0, g_fFarBlurLimit );
    }

    // Scale and bias the depth blur into the [0,1] range
    return clamp( fDepthBlur * 0.5f + 0.5f, 0, 1 );
}


float4 PS_DOF_Comp(in float2 inTexCoord,
				   uniform Texture2D sceneTexture,
				   uniform Texture2D blurredSceneTexture,
				   uniform Texture2D depthTexture)
{
	const float2 vPixelSizeHigh = GetTexelSize();

	float2 vPixelSizeLow = vPixelSizeHigh;

#if 0 // simple 

	float4 vTapLow   = tex2D( blurredSceneTexture,  inTexCoord );
	float4 vTapHigh   = tex2D( sceneTexture, inTexCoord );
	vTapLow.a = ComputeDepthBlur( vTapLow.a );
	vTapHigh.a = ComputeDepthBlur( tex2D( depthTexture, inTexCoord ).r ); // 0..1
	float fTapBlur = abs( (vTapHigh.a * 2) - 1 ); // -1..0..1, to 0..1
	float4 vTap = lerp( vTapHigh, vTapLow, fTapBlur );

	// Normalize and return result
	return float4( vTap.rgb,1);

#else

	// Save depth
	float fCenterDepth = UnpackDepth( depthTexture.Sample( pointClampSampler, inTexCoord ).r );

	// Convert depth into blur radius in pixels
	float fDiscRadius = abs( fCenterDepth * g_vMaxCoC.y - g_vMaxCoC.x );
	fDiscRadius = clamp(fDiscRadius, 0, g_fMaxDiscRadius);

	// Compute disc radius on low-res image
	float fDiscRadiusLow = fDiscRadius * g_fRadiusScale;

	// Accumulate output color across all taps
	float4 vOutColor = 0;

	for( int t=0; t<NUM_POISSON_TAPS; t++ )
	{
		// Fetch lo-res tap
		float2 vCoordLow =  inTexCoord + (vPixelSizeLow * g_Poisson[t] * fDiscRadiusLow );
		float4 vTapLow   = blurredSceneTexture.Sample( pointClampSampler, vCoordLow );

		// Fetch hi-res tap
		float2 vCoordHigh =  inTexCoord + (vPixelSizeHigh * g_Poisson[t] * fDiscRadius );
		float4 vTapHigh   = sceneTexture.Sample( pointClampSampler, vCoordHigh );

		vTapLow.a = ComputeDepthBlur( vTapLow.a );
		vTapHigh.a = ComputeDepthBlur( UnpackDepth( depthTexture.Sample( pointClampSampler, inTexCoord ).r ) ); 

		// Put tap blurriness into [1,..0..,1] range
		float fTapBlur = abs( (vTapHigh.a * 2.0f) - 1.0f );

		// Mix lo-res and hi-res taps based on blurriness
		float4 vTap = lerp( vTapHigh, vTapLow, fTapBlur );

		// Apply leaking reduction: lower weight for taps that are closer than the
		// center tap and in focus
		vTap.a = ( vTap.a >= fCenterDepth ) ? 1.0f : abs( vTap.a * 2.0001f - 1.0f );
		vTap.a = clamp( vTap.a, 0.001f, 1 ); // dx10 can get to zero causing divide by zero if all 0

		// Accumumate
		vOutColor.rgb += vTap.rgb * vTap.a;
		vOutColor.a   += vTap.a;
	}
	// Normalize and return result
	float4 c = vOutColor / vOutColor.a;
	return c;

#endif
}


#endif // RX_GBUFFER_READS_ENABLED

/*
=======================================================================

	Fog

=======================================================================
*/
/** /
float4 g_cFogColor = { 0,0,0,1 };
float4 g_iFogParams = { 0,0,0,0 };

float4 computeFog( in float viewZ, in float4 c )
{
	float scale = 0;
	float depth = viewZ;
	if (g_iFogParams.x > 2 ) // EXP2
	{	
		float ddensity = depth*g_iFogParams.w;
		scale = 1.0 / exp( ddensity*ddensity ); // 1/(e^((d*density)^2))
	}
	else if (g_iFogParams.x > 1 ) // EXP
	{
		float ddensity = depth*g_iFogParams.w;
		scale = 1.0 / exp( ddensity ); // 1/(e^(d*density))
	}
	else if (g_iFogParams.x > 0 ) // LINEAR
	{
		scale = (g_iFogParams.z - depth) / (g_iFogParams.z - g_iFogParams.y);
	}
	
	scale = clamp(scale, 0, 1);
	return ( (1 - scale) * float4(g_cFogColor.xyz,1) ) + ( scale * c); 
}
/**/


/*
=======================================================================

	SSAO (screen space ambient occlusion)

=======================================================================
*/

struct VS_SSAO_OUTPUT
{
   	float4 Position   : SV_POSITION;
    float2 TexCoord0  : TEXCOORD0;
    float4 EyeDir     : TEXCOORD1;
};









/*
=======================================================================

	Bloom and HDR

=======================================================================
*/


/*
	Glow/bloom post processing effect.
	Down-samples scene first for performance (reduces size by 4).
	Thresholds luminance for extra highlights.
	Separable filter, filters in X, then in Y.
	Takes advantage of bilinear filtering for blur.
*/

static const float SceneIntensity
<
    string UIName = "Scene intensity";
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 2.0f;
    float UIStep = 0.1f;
>
= 1.0f;

static const float GlowIntensity
<
    string UIName = "Glow intensity";
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 2.0f;
    float UIStep = 0.1f;
>
= 0.3f;

static const float HighlightThreshold
<
    string UIName = "Highlight threshold";
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 1.0f;
    float UIStep = 0.1f;
>
= 0.95f;

static const float HighlightIntensity
<
    string UIName = "Highlight intensity";
    string UIWidget = "slider";
    float UIMin = 0.0f;
    float UIMax = 10.0f;
    float UIStep = 0.1f;
>
= 2.0f;

static const float downsampleScale = 0.25;


// The per-color weighting to be used for luminance calculations in RGB order.
static const float3 LUMINANCE_VECTOR = float3(0.2125f, 0.7154f, 0.0721f);	// Reinhard luminance weights
//static const float3 LUMINANCE_WEIGHTS = float3(0.27f, 0.67f, 0.06f);
//static const float3 RGB_TO_LUMINANCE	= float3(0.299f, 0.587f, 0.114f)

static const float  MIDDLE_GRAY = 0.6f;
static const float  LUM_WHITE = 1.5f;
static const float  BRIGHT_PASS_THRESHOLD = 0.65f;	// Threshold for BrightPass filter (fBrightPassThreshold)
static const float  BRIGHT_PASS_OFFSET    = 4.0f; // Offset for BrightPass filter

// The per-color weighting to be used for blue shift under low light.
static const float3 BLUE_SHIFT_VECTOR = float3(1.05f, 0.97f, 1.27f); 

// Reinhard tone mapping operator
// input should be linear-space
// returns gamma-space
float3 Tonemap_Reinhard( float3 color, float exposure )
{
	float3 result;
	result = color * exposure;
	result = result/(1.0f + result);	// reinhard tonemapping
	return LinearToGamma(result);
}

// Filmictone mapping operator
// input should be linear-space
// returns gamma-space
// reference: http://filmicgames.com/archives/75
float3 Tonemap_Filmic( float3 color, float exposure )
{
	float3 result;
	result = color * exposure;

	float3 x = max(0.0f, result - 0.004f);
	result = (x*(6.2f*x+0.5f))/(x*(6.2f*x+1.7f)+0.06f);
	return result;		// no need to convert to gamma-space, already converted
}


// Retrieves the log-average luminance from the luminance texture.
float SampleLuminance( in Texture2D luminanceTexture )
{
	uint mipLevel = 0;
	uint width, height, numMipLevels;
	luminanceTexture.GetDimensions( mipLevel, width, height, numMipLevels );

	// sample the lowest 1x1 level
	const float sample = luminanceTexture.SampleLevel( pointSampler, float2(0.5,0.5), numMipLevels-1 ).x;
	return max( sample, 0.0001f );
	//return sample;
}
float SampleAverageLuminance( in Texture2D luminanceTexture )
{
	const float sample = SampleLuminance( luminanceTexture );
	return exp( sample );
}

mxSWIPED("Matt Pettineo (MJP)");

// Approximates luminance from an RGB value
float CalcLuminance(float3 color)
{
	return max( dot(color, LUMINANCE_VECTOR), 0.0001f );
}
/*
float CalcLuminance( float3 colorRgb )
{
	const static float3 lumFactor = float3(0.299, 0.587, 0.114);
	return dot(colorRgb, lumFactor);
}
*/

// Applies the filmic curve from John Hable's presentation
float3 ToneMapFilmicALU( in float3 color )
{
	color = max(0, color - 0.004f);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);

	// result has 1/2.2 baked in
	return LinearToGamma( color );
}

// Determines the color based on exposure settings
float3 CalcExposedColor( float3 color, float avgLuminance, float threshold, out float exposure )
{
	const float KeyValue = 0.4f;	//orig.value= 0.2
	// Use geometric mean
	avgLuminance = max(avgLuminance, 0.001f);
	float keyValue = KeyValue;
	float linearExposure = (KeyValue / avgLuminance);
	exposure = log2( max( linearExposure, 0.0001f ) );
	exposure -= threshold;
	return exp2(exposure) * color;
}

// Applies exposure and tone mapping to the specific color, and applies
// the threshold to the exposure value.
float3 ToneMap( float3 color, float avgLuminance, float threshold, out float exposure )
{
	float pixelLuminance = CalcLuminance( color );
	color = CalcExposedColor( color, avgLuminance, threshold, exposure );
	color = ToneMapFilmicALU( color );
	return color;
}

//float CalcAdaptedLum( in float adapted_lum, in float current_lum )
//{
//	return adapted_lum + (current_lum - adapted_lum) * (1 - pow(0.98f, 50 * frame_delta));
//}

// taken from Uncharted 2 GDC 2010 slides
float3 Tonemap( float3 x, float exposure )
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

	x *= exposure;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}



/*
=======================================================================

	LogLuv Encoding

	HDR color data with logluv encoding can occupy less space (R8G8B8A8),
	but cannot be blended
=======================================================================
*/

#if 0
mxSWIPED("Christer Ericson, Converting RGB to LogLuv in a fragment shader:"
"http://realtimecollisiondetection.net/blog/?p=15");

// M matrix, for encoding 
static const static float3x3 LogLuvM = float3x3(0.2209, 0.3390, 0.4184, 
								   0.1138, 0.6780, 0.7319, 
								   0.0102, 0.1130, 0.2969); 

// Inverse M matrix, for decoding 
static const static float3x3 LogLuvInverseM = float3x3( 
	6.0013,    -2.700,    -1.7995, 
	-1.332,    3.1029,    -5.7720, 
	.3007,    -1.088,    5.6268);     

float4 LogLuvEncode(in float3 vRGB) 
{         
	float4 vResult; 
	float3 Xp_Y_XYZp = mul(vRGB, LogLuvM); 
	Xp_Y_XYZp = max(Xp_Y_XYZp, float3(1e-6, 1e-6, 1e-6)); 
	vResult.xy = Xp_Y_XYZp.xy / Xp_Y_XYZp.z; 
	float Le = 2 * log2(Xp_Y_XYZp.y) + 127; 
	vResult.w = frac(Le); 
	vResult.z = (Le - (floor(vResult.w*255.0f))/255.0f)/255.0f; 
	return vResult; 
} 

float3 LogLuvDecode(in float4 vLogLuv) 
{     
	float Le = vLogLuv.z * 255 + vLogLuv.w; 
	float3 Xp_Y_XYZp; 
	Xp_Y_XYZp.y = exp2((Le - 127) / 2); 
	Xp_Y_XYZp.z = Xp_Y_XYZp.y / vLogLuv.y; 
	Xp_Y_XYZp.x = vLogLuv.x * Xp_Y_XYZp.z; 
	float3 vRGB = mul(Xp_Y_XYZp, LogLuvInverseM); 
	return max(vRGB, 0); 
}
#endif

#endif // H_POST_PROCESSING_H

