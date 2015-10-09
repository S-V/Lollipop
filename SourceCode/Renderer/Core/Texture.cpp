/*
=============================================================================
	File:	Texture.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include "Renderer.h"
#include "Texture.h"

/*
-----------------------------------------------------------------------------
	rxTexture
-----------------------------------------------------------------------------
*/
rxTexture::rxTexture()
{
}

rxTexture::~rxTexture()
{
}

/*
-----------------------------------------------------------------------------
	TextureManager
-----------------------------------------------------------------------------
*/
rxTextureManager::rxTextureManager()
{
	gCore.resources->SetManager( Asset_Texture2D, this );

	// Create a 1x1 texture filled with white color.
	{
		const DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const UINT textureWidth = 1;
		const UINT textureHeight = 1;

		DX11::Texture2DDescription	texDesc( textureFormat, textureWidth, textureHeight );
		texDesc
			.ArraySize_(1)
			.MipLevels_(1)
			.SampleCount_(1)
			.SampleQuality_(0)
			.Usage_(D3D11_USAGE_IMMUTABLE)
			.BindFlags_(D3D11_BIND_SHADER_RESOURCE)
			.CPUAccessFlags_(0)
			;

		R8G8B8A8	whiteColorRGBA;
		whiteColorRGBA.asU32 = BITS_ALL;

		D3D11_SUBRESOURCE_DATA	initialData;
		initialData.pSysMem = &whiteColorRGBA;
		initialData.SysMemPitch = sizeof whiteColorRGBA;
		initialData.SysMemSlicePitch = nil;

		ID3D11Device* pD3DDevice = GetD3DDevice();

		dxchk(pD3DDevice->CreateTexture2D(
			&texDesc, &initialData, c_cast(ID3D11Texture2D**) &m_defaultTexture.pTexture.Ptr
		));


		D3D11_SHADER_RESOURCE_VIEW_DESC		srvDesc;
		D3D_GetTexture2D_ShaderResourceView_Desc( textureFormat, srvDesc );

		dxchk(pD3DDevice->CreateShaderResourceView(
			m_defaultTexture.pTexture, &srvDesc, &m_defaultTexture.pSRV.Ptr
		));
	}
}

rxTextureManager::~rxTextureManager()
{
	m_allTextures.DeleteContents();
}

SResourceObject* rxTextureManager::LoadResource( SResourceLoadArgs & loadArgs )
{
	const void* pSrcData = loadArgs.Map();
	const SizeT nSrcDataSize = loadArgs.GetSize();

	mxOPTIMIZE("Use a texture pool, don't use new(); don't use slow D3DX");

	rxTexture* pNewTexture = new rxTexture();
	m_allTextures.Add( pNewTexture );

	ID3D11Device* pD3DDevice = GetD3DDevice();

	const HRESULT hResult = ::D3DX11CreateTextureFromMemory(
		pD3DDevice,
		pSrcData,
		nSrcDataSize,
		nil,	// D3DX11_IMAGE_LOAD_INFO*   pLoadInfo
		nil,	// ID3DX11ThreadPump*        pPump
		&pNewTexture->pTexture.Ptr,
		nil		// HRESULT*                  pHResult
	);

	if(FAILED( hResult )) {
		mxWarnf("Failed to create texture\n");
		return nil;
	}


	D3D11_RESOURCE_DIMENSION	textureType;
	pNewTexture->pTexture->GetType( &textureType );


	switch( textureType )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D :
		{
			ID3D11Texture2D* pD3DTexture2D = static_cast< ID3D11Texture2D* >( pNewTexture->pTexture.Ptr );

			D3D11_TEXTURE2D_DESC	texture2DDesc;
			pD3DTexture2D->GetDesc( &texture2DDesc );

			DEVOUT("Loaded 2D texture - Size: %ux%u, Mips: %u\n",
				(UINT)texture2DDesc.Width, (UINT)texture2DDesc.Height, (UINT)texture2DDesc.MipLevels);

			D3D11_SHADER_RESOURCE_VIEW_DESC		srvDesc;
			D3D_GetTexture2D_ShaderResourceView_Desc( texture2DDesc.Format, srvDesc );

			dxchk(pD3DDevice->CreateShaderResourceView(
				pNewTexture->pTexture, &srvDesc, &pNewTexture->pSRV.Ptr
				));
		}
		break;

	default:	Unimplemented;
	}

	// or, alternatively, you can use this to find the type of the created resource:
	//hr = pRes->QueryInterface( __uuidof( ID3D11Texture2D ), (LPVOID*)ppTexture );


#if 0
	DDS_HEADER* pSurfDesc9 = ( DDS_HEADER* )( ( BYTE* )pSrcData + DDS_MAGIC_SIZE_IN_BYTES );

	UINT Width = pSurfDesc9->dwWidth;
	UINT Height = pSurfDesc9->dwHeight;
	UINT MipLevels = pSurfDesc9->dwMipMapCount;
	if( MipLevels > m_SkipMips )
		MipLevels -= m_SkipMips;
	else
		m_SkipMips = 0;
	if( 0 == MipLevels )
		MipLevels = 1;

#endif

	return pNewTexture;
}

SResourceObject* rxTextureManager::GetDefaultResource()
{
	return &m_defaultTexture;
}

rxTexture& rxTextureManager::GetFallbackTexture()
{
	return m_defaultTexture;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
