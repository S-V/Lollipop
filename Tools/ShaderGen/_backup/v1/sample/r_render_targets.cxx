#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>


#include "r_render_targets.hxx"

namespace GPU
{
	//===========================================================================
	//	Render targets
	//===========================================================================
	
	RenderTarget renderTarget_RT0;
	RenderTarget renderTarget_RT1;
	RenderTarget renderTarget_RT2;
	
	static void SetupRenderTargets()
	{
		D3D11_TEXTURE2D_DESC texDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZERO_OUT( texDesc );
		ZERO_OUT( rtvDesc );
		ZERO_OUT( srvDesc );
		
		{
			texDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.Width				= GetBackbufferWidth();
			texDesc.Height				= GetBackbufferHeight();
			texDesc.MipLevels			= 1;
			texDesc.ArraySize			= 1;
			texDesc.SampleDesc.Count	= 1;
			texDesc.SampleDesc.Quality	= 0;
			texDesc.Usage				= D3D11_USAGE_DEFAULT;
			texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags		= 0;
			texDesc.MiscFlags			= 0;
			
			rtvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice	= 0;
			
			srvDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels			= 1;
			srvDesc.Texture2D.MostDetailedMip	= 0;
			
			graphics.resources->Create_RenderTarget( texDesc, rtvDesc, srvDesc, renderTarget_RT0 );
		}
		
		{
			texDesc.Format				= DXGI_FORMAT_R11G11B10_FLOAT;
			texDesc.Width				= GetBackbufferWidth();
			texDesc.Height				= GetBackbufferHeight();
			texDesc.MipLevels			= 1;
			texDesc.ArraySize			= 1;
			texDesc.SampleDesc.Count	= 1;
			texDesc.SampleDesc.Quality	= 0;
			texDesc.Usage				= D3D11_USAGE_DEFAULT;
			texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags		= 0;
			texDesc.MiscFlags			= 0;
			
			rtvDesc.Format				= DXGI_FORMAT_R11G11B10_FLOAT;
			rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice	= 0;
			
			srvDesc.Format						= DXGI_FORMAT_R11G11B10_FLOAT;
			srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels			= 1;
			srvDesc.Texture2D.MostDetailedMip	= 0;
			
			graphics.resources->Create_RenderTarget( texDesc, rtvDesc, srvDesc, renderTarget_RT1 );
		}
		
		{
			texDesc.Format				= DXGI_FORMAT_R16_FLOAT;
			texDesc.Width				= GetBackbufferWidth();
			texDesc.Height				= GetBackbufferHeight();
			texDesc.MipLevels			= 1;
			texDesc.ArraySize			= 1;
			texDesc.SampleDesc.Count	= 1;
			texDesc.SampleDesc.Quality	= 0;
			texDesc.Usage				= D3D11_USAGE_DEFAULT;
			texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags		= 0;
			texDesc.MiscFlags			= 0;
			
			rtvDesc.Format				= DXGI_FORMAT_R16_FLOAT;
			rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice	= 0;
			
			srvDesc.Format						= DXGI_FORMAT_R16_FLOAT;
			srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels			= 1;
			srvDesc.Texture2D.MostDetailedMip	= 0;
			
			graphics.resources->Create_RenderTarget( texDesc, rtvDesc, srvDesc, renderTarget_RT2 );
		}
	}
	
	//===========================================================================
	//	Creation / Destruction function
	//===========================================================================
	
	void InitializeModule_r_render_targets()
	{
		SetupRenderTargets();
	}
	
	void ShutdownModule_r_render_targets()
	{
		graphics.resources->Destroy_RenderTarget( renderTarget_RT0 );
		graphics.resources->Destroy_RenderTarget( renderTarget_RT1 );
		graphics.resources->Destroy_RenderTarget( renderTarget_RT2 );
		
	}
}
