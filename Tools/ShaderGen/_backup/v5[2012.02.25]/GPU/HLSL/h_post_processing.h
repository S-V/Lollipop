
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

MX_SWIPED("DOF taken from Havok Physics SDK");

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

static const float  g_fFocalPlaneDistance   = 7;// Focal plane distance
static const float  g_fNearBlurPlaneDistance = 1;// Near blur plance distance
static const float  g_fFarBlurPlaneDistance  = 10;// Far blur plane distance
static const float  g_fFarBlurLimit          = 1;//Far blur limit [0,1]
static const float  g_fMaxDiscRadius 		= 0.5f;

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

	MX_OPTIMIZE("precompute blur direction");

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

MX_OPTIMIZE("precompute texel offsets");

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



#if 0

#ifndef __cplusplus

	Texture2D< float4 >	 sourceTexture0 : register(t0);
	Texture2D< float4 >	 sourceTexture1 : register(t1);

#endif

DECLARE_CB( cbHDRConstants, 3 )
{
	float4 sampleOffsets[15];
	float4 sampleWeights[15];
};

#endif

// The per-color weighting to be used for luminance calculations in RGB order.
static const float3 LUMINANCE = float3(0.2125f, 0.7154f, 0.0721f);
static const float  MIDDLE_GRAY = 0.5f;
static const float  LUM_WHITE = 1.5f;
static const float  BRIGHT_THRESHOLD = 0.5f;	//fBrightPassThreshold


float3 GammaToLinear (float3 rgb )
{
	return pow( abs(rgb), 2.2f );
}

float3 LinearToGamma( float3 rgb )
{
	return pow( abs(rgb), 1.0f / 2.2f );
}

// From Uncharted 2 GDC slides.
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

