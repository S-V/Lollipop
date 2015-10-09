#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>


#include "r_render_states.hxx"

namespace GPU
{
	//===========================================================================
	//	Sampler states
	//===========================================================================
	
	SamplerState samplerState_SS_Point;
	SamplerState samplerState_SS_Bilinear;
	
	static void SetupSamplerStates()
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZERO_OUT( samplerDesc );
		
		{
			samplerDesc.Filter	= D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.MipLODBias		= 0;
			samplerDesc.MaxAnisotropy	= 1;
			samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
			samplerDesc.BorderColor[0]	= 1.0f;
			samplerDesc.BorderColor[1]	= 1.0f;
			samplerDesc.BorderColor[2]	= 1.0f;
			samplerDesc.BorderColor[3]	= 1.0f;
			samplerDesc.MinLOD			= -FLT_MAX;
			samplerDesc.MaxLOD			= +FLT_MAX;
			
			graphics.resources->Create_SamplerState( samplerDesc, samplerState_SS_Point );
		}
		
		{
			samplerDesc.Filter	= D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.MipLODBias		= 0;
			samplerDesc.MaxAnisotropy	= 1;
			samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
			samplerDesc.BorderColor[0]	= 1.0f;
			samplerDesc.BorderColor[1]	= 1.0f;
			samplerDesc.BorderColor[2]	= 1.0f;
			samplerDesc.BorderColor[3]	= 1.0f;
			samplerDesc.MinLOD			= -FLT_MAX;
			samplerDesc.MaxLOD			= +FLT_MAX;
			
			graphics.resources->Create_SamplerState( samplerDesc, samplerState_SS_Bilinear );
		}
	}
	//===========================================================================
	//	Depth-Stencil states
	//===========================================================================
	
	DepthStencilState depthStencilState_DS_NormalZTestWriteNoStencil;
	DepthStencilState depthStencilState_DS_NoZTestWriteNoStencil;
	
	static void SetupDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZERO_OUT( depthStencilDesc );
		
		{
			depthStencilDesc.DepthEnable	= TRUE;
			depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc	= D3D11_COMPARISON_LESS;
			depthStencilDesc.StencilEnable	= FALSE;
			depthStencilDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;
			depthStencilDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
			
			graphics.resources->Create_DepthStencilState( depthStencilDesc, depthStencilState_DS_NormalZTestWriteNoStencil );
		}
		
		{
			depthStencilDesc.DepthEnable	= FALSE;
			depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc	= D3D11_COMPARISON_LESS;
			depthStencilDesc.StencilEnable	= FALSE;
			depthStencilDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;
			depthStencilDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
			
			graphics.resources->Create_DepthStencilState( depthStencilDesc, depthStencilState_DS_NoZTestWriteNoStencil );
		}
	}
	//===========================================================================
	//	Rasterizer states
	//===========================================================================
	
	RasterizerState rasterizerState_RS_NoCull;
	RasterizerState rasterizerState_RS_CullBack;
	
	static void SetupRasterizerStates()
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZERO_OUT( rasterizerDesc );
		
		{
			rasterizerDesc.FillMode				= D3D11_FILL_SOLID;
			rasterizerDesc.CullMode				= D3D11_CULL_NONE;
			rasterizerDesc.FrontCounterClockwise	= FALSE;
			rasterizerDesc.DepthBias				= 0;
			rasterizerDesc.DepthBiasClamp		= 0.0f;
			rasterizerDesc.SlopeScaledDepthBias	= 0.0f;
			rasterizerDesc.DepthClipEnable		= TRUE;
			rasterizerDesc.ScissorEnable			= FALSE;
			rasterizerDesc.MultisampleEnable		= FALSE;
			rasterizerDesc.AntialiasedLineEnable	= FALSE;
			
			graphics.resources->Create_RasterizerState( rasterizerDesc, rasterizerState_RS_NoCull );
		}
		
		{
			rasterizerDesc.FillMode				= D3D11_FILL_SOLID;
			rasterizerDesc.CullMode				= D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise	= FALSE;
			rasterizerDesc.DepthBias				= 0;
			rasterizerDesc.DepthBiasClamp		= 0.0f;
			rasterizerDesc.SlopeScaledDepthBias	= 0.0f;
			rasterizerDesc.DepthClipEnable		= TRUE;
			rasterizerDesc.ScissorEnable			= FALSE;
			rasterizerDesc.MultisampleEnable		= FALSE;
			rasterizerDesc.AntialiasedLineEnable	= FALSE;
			
			graphics.resources->Create_RasterizerState( rasterizerDesc, rasterizerState_RS_CullBack );
		}
	}
	//===========================================================================
	//	Blend states
	//===========================================================================
	
	BlendState blendState_BS_NoBlending;
	
	static void SetupBlendStates()
	{
		D3D11_BLEND_DESC blendDesc;
		ZERO_OUT( blendDesc );
		
		{
			blendDesc.AlphaToCoverageEnable	= FALSE;
			blendDesc.IndependentBlendEnable	= FALSE;
			blendDesc.RenderTarget[0].BlendEnable	= FALSE;
			blendDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend		= D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask	= 0x0F;
			
			graphics.resources->Create_BlendState( blendDesc, blendState_BS_NoBlending );
		}
	}
	//===========================================================================
	//	State blocks
	//===========================================================================
	
	StateBlock renderState_Default;
	StateBlock renderState_Debug_NoCull;
	
	static void SetupStateBlocks()
	{
		
		{
			renderState_Default.rasterizer   = &rasterizerState_RS_CullBack;
			
			renderState_Default.depthStencil = &depthStencilState_DS_NormalZTestWriteNoStencil;
			renderState_Default.stencilRef   = 0;
			
			renderState_Default.blend           = &blendState_BS_NoBlending;
			renderState_Default.blendFactorRGBA = FColor( 0.000000f, 0.000000f, 0.000000f, 1.000000f );
			renderState_Default.sampleMask      = 0xFFFFFFFF;
		}
		
		{
			renderState_Debug_NoCull.rasterizer   = &rasterizerState_RS_NoCull;
			
			renderState_Debug_NoCull.depthStencil = &depthStencilState_DS_NoZTestWriteNoStencil;
			renderState_Debug_NoCull.stencilRef   = 0;
			
			renderState_Debug_NoCull.blend           = &blendState_BS_NoBlending;
			renderState_Debug_NoCull.blendFactorRGBA = FColor( 0.000000f, 0.000000f, 0.000000f, 1.000000f );
			renderState_Debug_NoCull.sampleMask      = 0xFFFFFFFF;
		}
	}
	
	//===========================================================================
	//	Creation / Destruction function
	//===========================================================================
	
	void InitializeModule_r_render_states()
	{
		SetupSamplerStates();
		SetupDepthStencilStates();
		SetupRasterizerStates();
		SetupBlendStates();
		SetupStateBlocks();
	}
	
	void ShutdownModule_r_render_states()
	{
		
		graphics.resources->Destroy_SamplerState( samplerState_SS_Point );
		graphics.resources->Destroy_SamplerState( samplerState_SS_Bilinear );
		
		graphics.resources->Destroy_DepthStencilState( depthStencilState_DS_NormalZTestWriteNoStencil );
		graphics.resources->Destroy_DepthStencilState( depthStencilState_DS_NoZTestWriteNoStencil );
		
		graphics.resources->Destroy_RasterizerState( rasterizerState_RS_NoCull );
		graphics.resources->Destroy_RasterizerState( rasterizerState_RS_CullBack );
		
		graphics.resources->Destroy_BlendState( blendState_BS_NoBlending );
		
		ZERO_OUT( renderState_Default );
		ZERO_OUT( renderState_Debug_NoCull );
	}
}
