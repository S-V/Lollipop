#pragma once

#include <Core/Editor.h>

class rxRenderContext;

/*
=======================================================================
	
	Post-processing pipeline

=======================================================================
*/


struct Size2D
{
	F4	Width;
	F4	Height;
};

struct PostFxUtil
{
	enum { MAX_RENDER_TARGETS = 8 };

	typedef TStaticList
	<
		RenderTarget*, MAX_RENDER_TARGETS
	>
	RenderTargetsList;

	// Binds necessary render targets and viewports.
	//
	static Size2D Prepare_Outputs( D3DDeviceContext* pD3DContext, const RenderTargetsList& outputs );
	static void UnbindOutputs( D3DDeviceContext* pD3DContext, const RenderTargetsList& outputs );

	static void CalculateSampleOffset_DownScale4x4( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] );
	static void CalculateSampleOffset_DownScale3x3( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] );
	static void CalculateSampleOffset_DownScale2x2( int dwWidth, int dwHeight, Vec2D avSampleOffsets[POST_FX_MAX_TAPS] );

	static void CalculateGaussianOffsetsAndWeights( int kernelRadius, F4 tU, F4 tV,
		Vec2D sampleOffsets[POST_FX_MAX_TAPS], F4 sampleWeights[POST_FX_MAX_TAPS],
		F4 deviation = 1.0f, F4 multiplier = 1.0f );

	static void Gaussian_Blur_Texture( D3DDeviceContext* pD3DContext, const RenderTarget& srcRT, RenderTarget &destRT, bool bHorizontal );
};



// Texture coordinate rectangle
//
struct CoordSubRect
{
    float fLeftU,  fTopV;
    float fRightU, fBottomV;

public:
	CoordSubRect()
	{
		// Start with a default mapping of the complete source surface to complete 
		// destination surface
		// Full screen quad coords
		fLeftU   = 0.0f;
		fTopV    = 0.0f;
		fRightU  = 1.0f;
		fBottomV = 1.0f;
	}
};









/*
--------------------------------------------------------------
	rxPostProcessEffect
--------------------------------------------------------------
*/
struct rxPostProcessEffect : AEditable
{
	virtual ~rxPostProcessEffect() {}
};


/*
--------------------------------------------------------------
	rxPostFx_DepthOfField
--------------------------------------------------------------
*/
struct rxPostFx_DepthOfField : rxPostProcessEffect
{
	rxPostFx_DepthOfField();
	~rxPostFx_DepthOfField();
};

enum { MAX_HDR_RENDER_TARGETS = 16 };





/*
--------------------------------------------------------------
	rxPostProcessor
--------------------------------------------------------------
*/
class rxPostProcessor
{
public:
	rxPostProcessor();
	~rxPostProcessor();

	void Initialize( UINT viewportWidth, UINT viewportHeight );
	void Shutdown();

	// set texture for accumulating scene color (into which lighting can be additively blended)
	void BindLightAccumulationBuffer( const rxRenderContext& context );

	// applies post-processing effects and writes the result to the main render target
	void WriteFinalColorToBackBuffer( const rxRenderContext& context );

private:

private:
	//UINT	m_cachedViewportWidth, m_cachedViewportHeight;

public:
	static bool g_cvar_high_quality_DOF;	// should we downscale blur texture for performance?
	static bool g_cvar_enable_bloom;	// bloom/glow post processing effects
};

extern bool g_cvar_enable_post_processing_effects;

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
