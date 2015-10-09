/*
=============================================================================
File:	PostProcess.cpp
Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Base/Util/Rectangle.h>

#include <CodeGen/ResGuids_Engine_AUTO.h>
#include <Core/Util/Tweakable.h>

#include <Renderer/Core/Texture.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>

#include <Renderer/Pipeline/PostProcess.h>
#include <Renderer/Pipeline/PostProcess/HDR.h>

#include <Renderer/Util/ScreenQuad.h>

bool g_cvar_enable_post_processing_effects = true;

bool rxPostProcessor::g_cvar_high_quality_DOF = true;
bool rxPostProcessor::g_cvar_enable_bloom = true;

struct PrivateData
{
	HDR_System	hdr;

public:

};
static TPtr< PrivateData >	self;

/*
--------------------------------------------------------------
Utility functions
--------------------------------------------------------------
*/

// Binds necessary render targets and viewports.
//
Size2D PostFxUtil::Prepare_Outputs( D3DDeviceContext* pD3DContext, const RenderTargetsList& outputs )
{
	const UINT numRenderTargets = outputs.Num();
	Assert( numRenderTargets > 0 );

	// Set destination render targets and viewports.

	{
		ID3D11RenderTargetView*	renderTargets[ MAX_RENDER_TARGETS ];

		for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
		{
			renderTargets[ iRenderTarget ] = outputs[ iRenderTarget ]->pRTV;
		}

		pD3DContext->OMSetRenderTargets( numRenderTargets, renderTargets, nil );
	}

	Size2D	firstViewportSize;

	{
		D3D11_VIEWPORT	viewports[ MAX_RENDER_TARGETS ];

		for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
		{
			D3D11_TEXTURE2D_DESC texDesc;
			outputs[ iRenderTarget ]->pTexture->GetDesc( &texDesc );

			D3D11_VIEWPORT& vp = viewports[ iRenderTarget ];

			vp.TopLeftX	= 0.0f;
			vp.TopLeftY	= 0.0f;
			vp.Width	= texDesc.Width;
			vp.Height	= texDesc.Height;
			vp.MinDepth	= 0.0f;
			vp.MaxDepth	= 1.0f;
		}

		pD3DContext->RSSetViewports( numRenderTargets, viewports );

		firstViewportSize.Width = viewports[0].Width;
		firstViewportSize.Height = viewports[0].Height;
	}

	return firstViewportSize;
}

void PostFxUtil::UnbindOutputs( D3DDeviceContext* pD3DContext, const RenderTargetsList& outputs )
{
	const UINT numRenderTargets = outputs.Num();
	Assert( numRenderTargets > 0 );

	ID3D11RenderTargetView*	nullRenderTargets[ MAX_RENDER_TARGETS ] = { nil };
	pD3DContext->OMSetRenderTargets( numRenderTargets, nullRenderTargets, nil );

	ID3D11ShaderResourceView* nullShaderResourceViews[ 16/*MAX_SHADER_RESOURCE_SLOTS*/ ] = { nil };
    pD3DContext->PSSetShaderResources( 0, numRenderTargets, nullShaderResourceViews );
}

//-----------------------------------------------------------------------------
// Name: CalculateSampleOffset_4x4Bilinear
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
//static
void PostFxUtil::CalculateSampleOffset_DownScale4x4( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] )
{
	const float tU = 1.0f / dwWidth;
	const float tV = 1.0f / dwHeight;

	// Sample from the 16 surrounding points.

	int index = 0;
	for( int y = 0; y < 4; y++ )
	{
		for( int x = 0; x < 4; x++ )
		{
			avSampleOffsets[ index ].x = (x - 2.f) * tU;
			avSampleOffsets[ index ].y = (y - 2.f) * tV;
			index++;
		}
	}
}

//static
void PostFxUtil::CalculateSampleOffset_DownScale3x3( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] )
{
	const float tU = 1.0f / dwWidth;
	const float tV = 1.0f / dwHeight;

	// Sample from the 9 surrounding points.

	int index = 0;
	for( int y = -1; y <= 1; y++ )
	{
		for( int x = -1; x <= 1; x++ )
		{
			avSampleOffsets[ index ].x = x  * tU;
			avSampleOffsets[ index ].y = y  * tV;
			index++;
		}
	}
}

//static
void PostFxUtil::CalculateSampleOffset_DownScale2x2( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] )
{
	const float tU = 1.0f / dwWidth;
	const float tV = 1.0f / dwHeight;

	// Sample from the 4 surrounding points.

	int index = 0;
	for( int y = 0; y < 2; y++ )
	{
		for( int x = 0; x < 2; x++ )
		{
			avSampleOffsets[ index ].x = (x - 1.f) * tU;
			avSampleOffsets[ index ].y = (y - 1.f) * tV;
			index++;
		}
	}
}

mxOPTIMIZE("use hw bilinear filtering for speed;"
"see:"
"http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/"
"http://www.geeks3d.com/20100909/shader-library-gaussian-blur-post-processing-filter-in-glsl/"
"http://www.realtimerendering.com/blog/quick-gaussian-filtering/"
"http://www.gamasutra.com/view/feature/3102/four_tricks_for_fast_blurring_in_.php?print=1"
"http://www.gamedev.net/topic/427696-hlsl---fast-gaussian-blur-c-dx9-ps2/page__view__findpost__p__3844656"
);
//static
void PostFxUtil::CalculateGaussianOffsetsAndWeights( int kernelRadius, F4 tU, F4 tV,
													Vec2D sampleOffsets[POST_FX_MAX_TAPS], F4 sampleWeights[POST_FX_MAX_TAPS],
													F4 deviation, F4 multiplier )
{
	const int numSamples = kernelRadius * 2 + 1;
	Assert( numSamples < POST_FX_MAX_TAPS );

	F4 sum = 0.0f;

	for( int iSample = 0; iSample < numSamples; iSample++ )
	{
		const F4 x = F4(iSample - kernelRadius);

		sampleOffsets[ iSample ] = Vec2D( x * tU, x * tV );

		const F4 weight = GaussianDistribution1D( x, deviation );
		sampleWeights[ iSample ] = weight;

		sum += weight;
	}

	// Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    //
	const F4 invSum = 1.0f / sum;
	for( int iSample = 0; iSample < numSamples; iSample++ )
	{
		sampleWeights[ iSample ] *= invSum;
		sampleWeights[ iSample ] *= multiplier;
	}
}

//-----------------------------------------------------------------------------
// Calculates the texture coordinate offsets to be used inside the GaussBlur5x5
// pixel shader.
//-----------------------------------------------------------------------------
//
static
void GetSampleOffsets_GaussBlur5x5(int textureWidth,
								   int textureHeight,
								   Vec2D* sampleOffsets,
								   float* sampleWeights,
								   float fMultiplier = 1.0f )
{
	Assert( textureWidth > 0 );
	Assert( textureHeight > 0 );
	AssertPtr( sampleOffsets );
	AssertPtr( sampleWeights );

	const float tu = 1.0f / (float)textureWidth ;
	const float tv = 1.0f / (float)textureHeight ;

	float totalWeight = 0.0f;
	int index = 0;
	for( int x = -2; x <= 2; x++ )
	{
		for( int y = -2; y <= 2; y++ )
		{
			// Exclude pixels with a block distance greater than 2. This will
			// create a kernel which approximates a 5x5 kernel using only 13
			// sample points instead of 25; this is necessary since 2.0 shaders
			// only support 16 texture grabs.
			if( abs(x) + abs(y) > 2 ) {
				continue;
			}
			// Get the unscaled Gaussian intensity for this offset
			sampleOffsets[ index ] = Vec2D( x * tu, y * tv );
			sampleWeights[ index ] = GaussianDistribution2D( (float)x, (float)y, 1.0f );
			totalWeight += sampleWeights[ index ];

			index++;
		}
	}

	// Divide the current weight by the total weight of all the samples; Gaussian
	// blur kernels add to 1.0f to ensure that the intensity of the image isn't
	// changed when the blur occurs. An optional multiplier variable is used to
	// add or remove image intensity during the blur.
	for( int i=0; i < index; i++ )
	{
		sampleWeights[i] /= totalWeight;
		sampleWeights[i] *= fMultiplier;
	}
}


static
void CalcViewport( int width, int height, D3D11_VIEWPORT &OutViewport )
{
	OutViewport.TopLeftX	= 0.0f;
	OutViewport.TopLeftY	= 0.0f;
	OutViewport.Width		= width;
	OutViewport.Height		= height;
	OutViewport.MinDepth	= 0.0f;
	OutViewport.MaxDepth	= 1.0f;
}

enum { MAX_BLUR_WEIGHTS = POST_FX_MAX_TAPS };

typedef TStaticList< F4, MAX_BLUR_WEIGHTS >	BlurWeights;

mxSWIPED("NVidia Direct3D 10 SDK [2007]");

//-----------------------------------------------------------------------------
// Calculate Gaussian weights based on kernel size
//-----------------------------------------------------------------------------
// generate array of weights for Gaussian blur
//
void GenerateGaussianWeights( int kernelRadius, BlurWeights &weights )
{
	const int size = kernelRadius * 2 + 1;
	weights.SetNum( size );

	float	s = floor(kernelRadius / 4.0f);

	float	sum = 0.0f;

	for( int i = 0; i < size; i++ )
	{
		float x = (float)(i - kernelRadius);

		// True Gaussian
#if 1
		weights[ i ] = expf(-x*x/(2.0f*s*s)) / (s*sqrtf(2.0f*D3DX_PI));
#else
		// This sum of exps is not really a separable kernel but produces a very interesting star-shaped effect
		weights[ i ] = expf( -0.0625f * x * x )
			+ 2 * expf( -0.25f * x * x )
			+ 4 * expf( - x * x )
			+ 8 * expf( - 4.0f * x * x )
			+ 16 * expf( - 16.0f * x * x )
			;
#endif

		sum += weights[i];
	}

	for( int i = 0; i < size; i++ ) {
		weights[i] /= sum;
	}
}

//-----------------------------------------------------------------------------
// Name: CalculateOffsets_GaussianBilinear
//
//  We want the general convolution:
//    a*f(i) + b*f(i+1)
//  Linear texture filtering gives us:
//    f(x) = (1-alpha)*f(i) + alpha*f(i+1);
//  It turns out by using the correct weight and offset we can use a linear lookup to achieve this:
//    (a+b) * f(i + b/(a+b))
//  as long as 0 <= b/(a+b) <= 1.
//
//  Given a standard deviation, we can calculate the size of the kernel and vice versa.
//
//-----------------------------------------------------------------------------
//
void CalculateOffsets_GaussianBilinear( float texSize, float *coordOffsets, float *gaussWeights, int maxSamples )
{
	const float du = 1.0f / texSize;

	//  store all the intermediate offsets & weights, then compute the bilinear
	//  taps in a second pass
	BlurWeights	tmpWeightArray;
	GenerateGaussianWeights( maxSamples, tmpWeightArray );

	// Bilinear filtering taps 
	// Ordering is left to right.
	float sScale;
	float sFrac;

	for( int i = 0; i < maxSamples; i++ )
	{
		sScale = tmpWeightArray[i*2 + 0] + tmpWeightArray[i*2 + 1];
		sFrac  = tmpWeightArray[i*2 + 1] / sScale;

		coordOffsets[i] = ( (2.0f*i - maxSamples) + sFrac ) * du;
		gaussWeights[i] = sScale;
	}
}

//-----------------------------------------------------------------------------
// Name: CalculateOffsetsAndQuadCoords
// Desc: Every time we resize the screen we will recalculate an array of
// vertices and texture coordinates to be used to render the full screen 
// primitive for the different post processing effects. Note that different 
// effects use a different set of values! 
//-----------------------------------------------------------------------------
//
void CalculateOffsetsAndQuadCoords( int bufferWidth, int bufferHeight )
{
	Vec2D sampleOffsets_blurV[POST_FX_MAX_TAPS];
	Vec2D sampleOffsets_blurH[POST_FX_MAX_TAPS];

	float	gaussWeights[POST_FX_MAX_TAPS];
	float	coordOffsets[POST_FX_MAX_TAPS];

	CalculateOffsets_GaussianBilinear( (float)bufferWidth, coordOffsets, gaussWeights, POST_FX_MAX_TAPS );

	for( int i = 0; i < POST_FX_MAX_TAPS; i++ )
	{
		sampleOffsets_blurH[i].x = coordOffsets[i];
		sampleOffsets_blurH[i].y = 0;

		sampleOffsets_blurV[i].x = 0;
		sampleOffsets_blurV[i].y = coordOffsets[i] * bufferWidth / bufferHeight;
	}
}

/*
enum{ NUM_BLUR_TAPS = 16 };
Vec2D	sampleOffsets[NUM_BLUR_TAPS];
FLOAT	sampleWeights[NUM_BLUR_TAPS];// Contains weights for Gaussian blurring
//CalculateGaussianBlurTaps( true, NUM_BLUR_TAPS, sampleOffsets, sampleWeights );
GetSampleOffsets_GaussBlur5x5( 1400,900, sampleOffsets, sampleWeights );


printf("\n\nTaps(%u):\n",(UINT)NUM_BLUR_TAPS);
for( UINT i=0; i < NUM_BLUR_TAPS; i++ )
{
printf("	float2( %.7f, %.7f ),\n", sampleOffsets[i].x, sampleOffsets[i].y );
}
printf("\n\nWeights(%u):\n",(UINT)NUM_BLUR_TAPS);
for( UINT i=0; i < NUM_BLUR_TAPS; i++ )
{
printf("	%.7f,\n", sampleWeights[i] );
}
*/



void PostFxUtil::Gaussian_Blur_Texture( D3DDeviceContext* pD3DContext, const RenderTarget& srcRT, RenderTarget &destRT, bool bHorizontal )
{
	PostFxUtil::RenderTargetsList	outputs;
	outputs.Add( &destRT );

	const Size2D destSize = PostFxUtil::Prepare_Outputs( pD3DContext, outputs );
	{
		rxShaderInstanceId	shaderInstanceId = GPU::p_weighted_blur::DefaultInstanceId;
		{
			const UINT kernelSize = 6;
			const UINT numSamples = kernelSize * 2 + 1;

			if( numSamples & BIT(0) ) {
				shaderInstanceId |= GPU::p_weighted_blur::iNumSamplesBit0;
			}
			if( numSamples & BIT(1) ) {
				shaderInstanceId |= GPU::p_weighted_blur::iNumSamplesBit1;
			}
			if( numSamples & BIT(2) ) {
				shaderInstanceId |= GPU::p_weighted_blur::iNumSamplesBit2;
			}
			if( numSamples & BIT(3) ) {
				shaderInstanceId |= GPU::p_weighted_blur::iNumSamplesBit3;
			}

			GPU::p_weighted_blur::Data* pData = GPU::p_weighted_blur::cb_Data.Map( pD3DContext );
			{
				const F4 tU = bHorizontal ? 1.0f / (F4)destSize.Width : 0.0f;
				const F4 tV = bHorizontal ? 0.0f : 1.0f / (F4)destSize.Height;
				PostFxUtil::CalculateGaussianOffsetsAndWeights( kernelSize, tU, tV, (Vec2D*)pData->sampleOffsets, (F4*)pData->sampleWeights, 5.0f, 1.0f );
			}
			GPU::p_weighted_blur::cb_Data.Unmap( pD3DContext );
		}
		GPU::p_weighted_blur::sourceTexture = srcRT.pSRV;
		GPU::p_weighted_blur::Set( pD3DContext, shaderInstanceId );

		RenderFullScreenTriangleOnly( pD3DContext );
	}
	PostFxUtil::UnbindOutputs( pD3DContext, outputs );
}


/*
--------------------------------------------------------------
rxPostFx_DepthOfField
--------------------------------------------------------------
*/

struct SSAO_Settings
{
	float	m_ssaoBias;
	float	m_ssaoScale;
	float	m_ssaoIntensity;
	float	m_ssaoRadius;

public:
	SSAO_Settings()
	{
		m_ssaoBias		= 0.1f;
		m_ssaoScale		= 1.0f;
		m_ssaoIntensity = 3.0f;
		m_ssaoRadius	= 0.01f;
	}
};

/*
--------------------------------------------------------------
rxPostProcessor
--------------------------------------------------------------
*/
rxPostProcessor::rxPostProcessor()
{
	//m_cachedViewportWidth = 0;
	//m_cachedViewportHeight = 0;
	// 
	self.ConstructInPlace();

	HOT_BOOL(g_cvar_enable_post_processing_effects);
	HOT_BOOL(g_cvar_high_quality_DOF);

#if MX_DEVELOPER

	UINT	bytesRenderTargetsVRAM = 0;

	struct EnumHelper
	{
		static void CalcRenderTargetSize( RenderTarget* p, void* userData )
		{
			UINT * pTotalSize = (UINT*) userData;

			(*pTotalSize) += p->CalcSizeInBytes();
		}
	};
	GPU::EnumerateRenderTargets( &EnumHelper::CalcRenderTargetSize, &bytesRenderTargetsVRAM );
	DEVOUT("Memory used by all render targets: %u MiB\n", bytesRenderTargetsVRAM/mxMEBIBYTE );


	UINT	lightingBufferSize = 0;
	lightingBufferSize += GPU::RT_HDR_SceneColor.CalcSizeInBytes();
	DEVOUT("Lighting buffers' size: %u MiB\n", lightingBufferSize/mxMEBIBYTE );

#endif //MX_DEVELOPER
}

rxPostProcessor::~rxPostProcessor()
{
	self.Destruct();
}

void rxPostProcessor::Initialize( UINT viewportWidth, UINT viewportHeight )
{
	self->hdr.Initialize( viewportWidth, viewportHeight );
}

void rxPostProcessor::Shutdown()
{
	self->hdr.Shutdown();
}

void rxPostProcessor::BindLightAccumulationBuffer( const rxRenderContext& context )
{
	const rxViewport* viewport = context.v;
	D3DDeviceContext *	pD3DContext = context.pD3D;

	if( g_cvar_enable_post_processing_effects )
	{
		// the resulting colors of illuminated objects can extend far beyond the 1.0f cutoff;
		// a floating-point texture is not used to capture these HDR values.
		
		// The scene is first rendered onto a floating-point texture.

		pD3DContext->ClearRenderTargetView( GPU::RT_HDR_SceneColor.pRTV, FColor::BLACK.ToFloatPtr() );
		pD3DContext->OMSetRenderTargets( 1, &GPU::RT_HDR_SceneColor.pRTV.Ptr, viewport->mainDS.pDSV );

		pD3DContext->RSSetViewports( 1, &viewport->d );
	}
	else
	{
		viewport->Set( pD3DContext );
	}
}

void rxPostProcessor::WriteFinalColorToBackBuffer( const rxRenderContext& context )
{
//#if RX_D3D_USE_PERF_HUD
//	PIXEvent	applyPostFxEvent(L"Post processing");
//#endif // RX_D3D_USE_PERF_HUD

	const rxViewport &	viewport = *context.v;

	const int backbufferWidth = viewport.GetWidth();
	const int backbufferHeight = viewport.GetHeight();

	(void)backbufferWidth;
	(void)backbufferHeight;

	D3DDeviceContext *	pD3DContext = context.pD3D;

	viewport.Set( pD3DContext );

	if( g_cvar_enable_post_processing_effects )
	{
		GPU::SolidNoCullNoZTestNoClipNoBlend.Set( pD3DContext );

		//GPU::Shared_PostProcessData::Set( pD3DContext );

		// DOF
		if(1)
		{
			rxGPU_MARKER( DOF_Blur );
			{
				rxGPU_MARKER( Horizontal_DOF_Blur );
				PostFxUtil::Gaussian_Blur_Texture( pD3DContext, GPU::RT_HDR_SceneColor, GPU::RT_Ping, true );
			}

			{
				rxGPU_MARKER( Vertical_DOF_Blur );
				PostFxUtil::Gaussian_Blur_Texture( pD3DContext, GPU::RT_Ping, GPU::RT_Pong, false );
			}

			//pD3DContext->OMSetRenderTargets( 1, &GPU::RT_HDR_SceneColor.pRTV.Ptr, nil );

			//{
			//	GPU::p_depth_of_field::sceneColorTexture = ?;
			//	GPU::p_depth_of_field::sceneDepthTexture = ?;
			//	GPU::p_depth_of_field::blurredSceneTexture = ?;
			//	GPU::p_depth_of_field::Set( pD3DContext );

			//	RenderFullScreenTriangleOnly( pD3DContext );
			//}
		}

		//self->hdr.ApplyToneMapping( pD3DContext, GPU::RT_HDR_SceneColor, *viewport );


		ID3D11ShaderResourceView* pSRV_Luminance = self->hdr.MeasureAverageLuminance( pD3DContext, GPU::RT_HDR_SceneColor );
		ID3D11ShaderResourceView* pSRV_Bloom = self->hdr.GenerateBloom( pD3DContext, GPU::RT_HDR_SceneColor, viewport );

		rxGPU_MARKER( Tone_Map_And_Composite );

		pD3DContext->OMSetRenderTargets( 1, &viewport.mainRT.pRTV.Ptr, nil );
		pD3DContext->RSSetViewports( 1, &viewport.d );

		GPU::Shared_View::Set( pD3DContext );

		{
			rxShaderInstanceId shaderInstanceId = GPU::p_uber_post_processing_shader::DefaultInstanceId;

			if( g_cvar_high_quality_DOF )
			{
				shaderInstanceId |= GPU::p_uber_post_processing_shader::bEnable_DOF;
			}
			if( g_cvar_enable_bloom )
			{
				shaderInstanceId |= GPU::p_uber_post_processing_shader::bEnable_Bloom;
			}

			GPU::p_uber_post_processing_shader::sceneColorTexture = GPU::RT_HDR_SceneColor.pSRV;
			GPU::p_uber_post_processing_shader::sceneDepthTexture = GPU::RT_LinearDepth.pSRV;
			GPU::p_uber_post_processing_shader::blurredSceneTexture = GPU::RT_Pong.pSRV;
			GPU::p_uber_post_processing_shader::averageLuminanceTexture = pSRV_Luminance;
			GPU::p_uber_post_processing_shader::bloomTexture = pSRV_Bloom;
			GPU::p_uber_post_processing_shader::Set( pD3DContext, shaderInstanceId );

			//RenderFullScreenTriangleOnly( pD3DContext );
			RenderFullScreenQuadOnly( pD3DContext );
		}

	}
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
