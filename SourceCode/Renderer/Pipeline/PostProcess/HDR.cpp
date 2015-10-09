#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>

#include <Renderer/Pipeline/PostProcess.h>
#include <Renderer/Pipeline/PostProcess/HDR.h>

#include <Renderer/Util/ScreenQuad.h>



static void AddRenderTargetsToDebugViews( HDR_System* o )
{
#if MX_EDITOR
	gRenderer.editor->GetTextureInspector()->AddDebugItem( o->initialLuminance.pTexture, "InitialLum" );

	gRenderer.editor->GetTextureInspector()->AddDebugItem( o->adaptedLuminance[0].pTexture, "AdaptedLum[0]" );
	gRenderer.editor->GetTextureInspector()->AddDebugItem( o->adaptedLuminance[1].pTexture, "AdaptedLum[1]" );
#endif // MX_EDITOR
}

static void RemoveRenderTargetsFromDebugViews( HDR_System* o )
{
#if MX_EDITOR
	gRenderer.editor->GetTextureInspector()->RemoveDebugItem( o->initialLuminance.pTexture );

	gRenderer.editor->GetTextureInspector()->RemoveDebugItem( o->adaptedLuminance[0].pTexture );
	gRenderer.editor->GetTextureInspector()->RemoveDebugItem( o->adaptedLuminance[1].pTexture );
#endif // MX_EDITOR
}


/*
--------------------------------------------------------------
	HDR_System
--------------------------------------------------------------
*/
HDR_System::HDR_System()
{
	iCurrLuminanceRT = 0;

	mxCONNECT_THIS( gRenderer.BeforeMainViewportResized, HDR_System, Callback_BeforeMainViewportResized );
	mxCONNECT_THIS( gRenderer.AfterMainViewportResized, HDR_System, Callback_AfterMainViewportResized );
}

HDR_System::~HDR_System()
{
	this->Shutdown();
}

void HDR_System::Initialize( UINT viewportWidth, UINT viewportHeight )
{
	this->CreateRenderTargets( viewportWidth, viewportHeight );
}

void HDR_System::Shutdown()
{
	this->ReleaseRenderTargets();

	mxDISCONNECT_THIS( gRenderer.BeforeMainViewportResized );
	mxDISCONNECT_THIS( gRenderer.AfterMainViewportResized );
}

void HDR_System::CreateRenderTargets( UINT viewportWidth, UINT viewportHeight )
{
	// Create average luminance calculation targets.
	{
		UINT luminanceRenderTargetSize = largest( viewportWidth, viewportHeight ) / 2;

		// round size to nearest power of two
		luminanceRenderTargetSize = CeilPowerOfTwo( luminanceRenderTargetSize );

		const DXGI_FORMAT luminanceRenderTargetFormat = DXGI_FORMAT_R16_FLOAT;	// DXGI_FORMAT_R32_FLOAT

		D3D11_TEXTURE2D_DESC texDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		{
			texDesc.Format				= luminanceRenderTargetFormat;
			texDesc.Width				= luminanceRenderTargetSize;
			texDesc.Height				= luminanceRenderTargetSize;
			texDesc.MipLevels			= 1;
			texDesc.ArraySize			= 1;
			texDesc.SampleDesc.Count	= 1;
			texDesc.SampleDesc.Quality	= 0;
			texDesc.Usage				= D3D11_USAGE_DEFAULT;
			texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags		= 0;
			texDesc.MiscFlags			= 0;

			rtvDesc.Format				= luminanceRenderTargetFormat;
			rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice	= 0;

			srvDesc.Format						= luminanceRenderTargetFormat;
			srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels			= 1;
			srvDesc.Texture2D.MostDetailedMip	= 0;
		}
		graphics.resources->Create_RenderTarget( initialLuminance, texDesc, rtvDesc, srvDesc );

		{
			texDesc.Format				= luminanceRenderTargetFormat;
			texDesc.Width				= luminanceRenderTargetSize;
			texDesc.Height				= luminanceRenderTargetSize;
			texDesc.MipLevels			= 0;	// generate a full chain of subtextures
			texDesc.ArraySize			= 1;
			texDesc.SampleDesc.Count	= 1;
			texDesc.SampleDesc.Quality	= 0;
			texDesc.Usage				= D3D11_USAGE_DEFAULT;
			texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags		= 0;
			texDesc.MiscFlags			= D3D11_RESOURCE_MISC_GENERATE_MIPS;

			rtvDesc.Format				= luminanceRenderTargetFormat;
			rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice	= 0;

			srvDesc.Format						= luminanceRenderTargetFormat;
			srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels			= -1;
			srvDesc.Texture2D.MostDetailedMip	= 0;
		}
		graphics.resources->Create_RenderTarget( adaptedLuminance[0], texDesc, rtvDesc, srvDesc );
		graphics.resources->Create_RenderTarget( adaptedLuminance[1], texDesc, rtvDesc, srvDesc );

#if RX_DEBUG_RENDERER
		{
			// the lowest level contains a 1x1 subtexture
			const UINT numMipLevels = log2i( luminanceRenderTargetSize ) + 1;

			D3D11_TEXTURE2D_DESC texDesc;
			adaptedLuminance[0].pTexture->GetDesc( &texDesc );

			Assert( texDesc.MipLevels == numMipLevels );
		}
#endif
	}

	AddRenderTargetsToDebugViews( this );
}

void HDR_System::ReleaseRenderTargets()
{
	RemoveRenderTargetsFromDebugViews( this );

	initialLuminance.Release();
	adaptedLuminance[0].Release();
	adaptedLuminance[1].Release();
}

void HDR_System::Callback_BeforeMainViewportResized( UINT newWidth, UINT newHeight )
{
	this->ReleaseRenderTargets();
}

void HDR_System::Callback_AfterMainViewportResized( UINT newWidth, UINT newHeight )
{
	this->CreateRenderTargets( newWidth, newHeight );
}

ID3D11ShaderResourceView* HDR_System::MeasureAverageLuminance( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT )
{
	// Luminance mapping
	{
		rxGPU_MARKER( Measure_Scene_Luminance );

		PostFxUtil::RenderTargetsList	outputs;
		outputs.Add( &initialLuminance );

		PostFxUtil::Prepare_Outputs( pD3DContext, outputs );
		{
			// Convert HDR image to log luminance floating-point values

			GPU::p_sample_luminance_initial::sourceTexture = GPU::RT_HDR_SceneColor.pSRV;
			GPU::p_sample_luminance_initial::Set( pD3DContext );

			RenderFullScreenTriangleOnly( pD3DContext );
		}
		PostFxUtil::UnbindOutputs( pD3DContext, outputs );
	}

	// Adaptation
	{
		rxGPU_MARKER( Eye_Adaptation );

		const UINT srcLumTarget = iCurrLuminanceRT ^ 1;	// take adapted luminance from previous frame
		const UINT dstLumTarget = iCurrLuminanceRT;

		PostFxUtil::RenderTargetsList	outputs;
		outputs.Add( &adaptedLuminance[ dstLumTarget ] );

		PostFxUtil::Prepare_Outputs( pD3DContext, outputs );
		{
			GPU::p_adapt_luminance::currentLuminanceTexture = initialLuminance.pSRV;
			GPU::p_adapt_luminance::adaptedLuminanceTexture = adaptedLuminance[ srcLumTarget ].pSRV;
			GPU::p_adapt_luminance::Set( pD3DContext );

			RenderFullScreenTriangleOnly( pD3DContext );
		}
		PostFxUtil::UnbindOutputs( pD3DContext, outputs );


		pD3DContext->GenerateMips( adaptedLuminance[ dstLumTarget ].pSRV );

		iCurrLuminanceRT ^= 1;

		// Debugging.

	#if RX_DEBUG_RENDERER
			static bool bDumpLuminanceTexture = false;
			HOT_BOOL(bDumpLuminanceTexture);
			if(bDumpLuminanceTexture) {
				dxchk(D3DX11SaveTextureToFileA(
					D3DContext,
					adaptedLuminance[ iCurrLuminanceRT ].pTexture,
					D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_DDS,
					"_AdaptedLuminance.dds"
					));
				bDumpLuminanceTexture = false;
			}
	#endif // RX_DEBUG_RENDERER

	}

	return adaptedLuminance[ iCurrLuminanceRT ].pSRV;
}

struct GBlur13
{
	enum { KERNEL_SIZE = 13 };

	static void CalcOffsetsAndWeights(
		int textureSize,
		Vec2D sampleOffsets[POST_FX_MAX_TAPS],
		F4 sampleWeights[POST_FX_MAX_TAPS]
	)
	{
		static const float PixelOffsets[ KERNEL_SIZE ] =
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
		static const float BlurWeights[ KERNEL_SIZE ] = 
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

		const F4 texelSize = 1.0f / (F4)textureSize;

		for( int iSample = 0; iSample < KERNEL_SIZE; iSample++ )
		{
			const F4 texelOffset = PixelOffsets[ iSample ] * texelSize;
			sampleOffsets[ iSample ] = Vec2D( texelOffset, 0.0f );

			sampleWeights[ iSample ] = BlurWeights[ iSample ];
		}
	}
};

ID3D11ShaderResourceView* HDR_System::GenerateBloom( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT, const rxViewport& viewport )
{
	if( !rxPostProcessor::g_cvar_enable_bloom ) {
		return nil;
	}

	const int backbufferWidth = viewport.GetWidth();
	const int backbufferHeight = viewport.GetHeight();

	(void)backbufferWidth;
	(void)backbufferHeight;

	rxGPU_MARKER( Bloom );

	{
		rxGPU_MARKER( Downscale_Bright_Pass );

		PostFxUtil::RenderTargetsList	outputs;
		outputs.Add( &GPU::RT_Downscaled_4x4 );

		PostFxUtil::Prepare_Outputs( pD3DContext, outputs );
		{
			{
				GPU::p_downscale_4x4_bright_pass::Data* pData = GPU::p_downscale_4x4_bright_pass::cb_Data.Map( pD3DContext );
				{
					PostFxUtil::CalculateSampleOffset_DownScale4x4( viewport.GetWidth(), viewport.GetHeight(), (Vec2D*)pData->sampleOffsets );
				}
				GPU::p_downscale_4x4_bright_pass::cb_Data.Unmap( pD3DContext );
			}
			GPU::p_downscale_4x4_bright_pass::sourceTexture = GPU::RT_HDR_SceneColor.pSRV;
			GPU::p_downscale_4x4_bright_pass::avgLuminanceTexture = adaptedLuminance[ iCurrLuminanceRT ].pSRV;
			GPU::p_downscale_4x4_bright_pass::Set( pD3DContext );

			RenderFullScreenTriangleOnly( pD3DContext );
		}
		PostFxUtil::UnbindOutputs( pD3DContext, outputs );
	}

	{
		rxGPU_MARKER( Horizontal_Bloom_Blur );

		PostFxUtil::Gaussian_Blur_Texture( pD3DContext, GPU::RT_Downscaled_4x4, GPU::RT_Downscaled_4x4_B, true );
	}

	{
		rxGPU_MARKER( Vertical_Bloom_Blur );

		PostFxUtil::Gaussian_Blur_Texture( pD3DContext, GPU::RT_Downscaled_4x4_B, GPU::RT_Downscaled_4x4, false );
	}

	return GPU::RT_Downscaled_4x4.pSRV;
}

// Draw the high dynamic range scene texture to the low dynamic range
// back buffer. As part of this final pass, the scene will be tone-mapped
// using the user's current adapted luminance, blue shift will occur
// if the scene is determined to be very dark, and the post-process lighting
// effect textures will be added to the scene.
// 
void HDR_System::ApplyToneMapping( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT, const rxViewport& viewport )
{
	ID3D11ShaderResourceView* pSRV_Luminance = this->MeasureAverageLuminance( pD3DContext, hdrSceneRT );
	ID3D11ShaderResourceView* pSRV_Bloom = this->GenerateBloom( pD3DContext, hdrSceneRT, viewport );

	rxGPU_MARKER( Tone_Map_And_Composite );

	pD3DContext->OMSetRenderTargets( 1, &viewport.mainRT.pRTV.Ptr, nil );
	pD3DContext->RSSetViewports( 1, &viewport.d );

	{
		GPU::p_hdr_final_scene_pass::sourceTexture = GPU::RT_HDR_SceneColor.pSRV;
		GPU::p_hdr_final_scene_pass::averageLuminanceTexture = pSRV_Luminance;
		GPU::p_hdr_final_scene_pass::bloomTexture = pSRV_Bloom;
		GPU::p_hdr_final_scene_pass::Set( pD3DContext );

		RenderFullScreenTriangleOnly( pD3DContext );
	}
}

NO_EMPTY_FILE

