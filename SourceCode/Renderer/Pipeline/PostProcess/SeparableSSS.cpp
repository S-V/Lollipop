/*
=============================================================================
	File:	SeparableSSS.cpp
	Desc:	Screen-Space Subsurface Scattering
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Core/Util/Tweakable.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>
#include <Renderer/Pipeline/PostProcess/SeparableSSS.h>
#include <Renderer/Util/ScreenQuad.h>

SeparableSSS::SeparableSSS()
{

}

SeparableSSS::~SeparableSSS()
{

}

void SeparableSSS::Go( const RenderTarget& mainRT, const DepthStencil& mainDS )
{
	ID3D11DeviceContext *	pD3DContext = GetD3DDeviceContext();

	dxSaveRenderTargetsAndViewports		rememberRenderTargetsAndDepthStencil( pD3DContext );
	dxSaveShaderResources	rememberShaderResources( pD3DContext );

	ID3D11RenderTargetView	*	nullRenderTarget = nil;

	pD3DContext->OMSetRenderTargets( 1, &nullRenderTarget, nil );

    // Clear the temporal render target:

	pD3DContext->ClearRenderTargetView( GPU::RT_Ping.pRTV, FColor::BLACK.ToFloatPtr() );
	pD3DContext->OMSetRenderTargets( 1, &GPU::RT_Ping.pRTV.Ptr, mainDS.pDSV );

	// viewport is already set

	typedef GPU::p_separable_sss_reflectance ShaderType;

	// Run the horizontal pass:

	rxShaderInstanceId	shaderInstanceId = ShaderType::DefaultInstanceId;
	{
		ShaderType::sceneColorTexture = mainRT.pSRV;
		ShaderType::sceneDepthTexture = GPU::Shared_GBuffer::RT_linearDepth;
		ShaderType::Set( pD3DContext );

		RenderFullScreenTriangleOnly( pD3DContext );
	}

	// And finish with the vertical one:

	shaderInstanceId |= ShaderType::bVerticalBlurDirection;

	// unbind shader resources to prevent D3D11 runtime warnings
	ID3D11ShaderResourceView* nullSRV[1] = {nil};
	pD3DContext->PSSetShaderResources( ShaderType::SR_Slot_sceneColorTexture, 1, nullSRV );

	pD3DContext->OMSetRenderTargets( 1, &mainRT.pRTV.Ptr, mainDS.pDSV );

	{
		GPU::p_separable_sss_reflectance::sceneColorTexture = GPU::RT_Ping.pSRV;
		GPU::p_separable_sss_reflectance::sceneDepthTexture = GPU::Shared_GBuffer::RT_linearDepth;
		GPU::p_separable_sss_reflectance::Set( pD3DContext );

		RenderFullScreenTriangleOnly( pD3DContext );
	}
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
