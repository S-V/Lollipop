#include "stdafx.h"

#include <Renderer/Core/Texture.h>
#include "util/load_texture_from_file_util.h"

namespace GraphicsUtil
{

ID3D11ShaderResourceView* F_Load_Texture_From_File( const char* fileName )
{
	ID3D11ShaderResourceView* pSRV = nil;
	if(FAILED( D3DX11CreateShaderResourceViewFromFileA(D3DDevice,fileName,nil,nil,&pSRV,nil) ))
	{
		mxDEBUG_BREAK;
		pSRV = rxTextureManager::Get().GetFallbackTexture().pSRV;
	}
	return pSRV;
}

}//namespace GraphicsUtil
