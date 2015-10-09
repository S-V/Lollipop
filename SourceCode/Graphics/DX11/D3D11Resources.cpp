/*
=============================================================================
	File:	D3D11Resources.cpp
	Desc:	
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

// for FixedTextBufferANSI
#include <Base/Text/TextBuffer.h>

#include "DX11Private.h"


/*
--------------------------------------------------------------
	RenderTarget
--------------------------------------------------------------
*/
RenderTarget::RenderTarget()
{
}

void RenderTarget::Create( D3D11_TEXTURE2D_DESC * pTexDesc )
{
	//DBG_TRACE_CALL;
	Assert(this->IsNull());

	//These have to be set to have a render target
	Assert(pTexDesc->BindFlags & (D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE));
	Assert(pTexDesc->Usage == D3D11_USAGE_DEFAULT);
	Assert(pTexDesc->CPUAccessFlags == 0);

	ID3D11Device* pD3DDevice = GetD3DDevice();

	mxPLATFORM_PROBLEM("driver bug: CreateShaderResourceView() fails if the second param is NULL");
#if 0
	dxchk( pD3DDevice->CreateTexture2D( pTexDesc, NULL, &pTexture.Ptr ) );
	dxchk( pD3DDevice->CreateShaderResourceView( pTexture, NULL, &pSRV.Ptr ) );
	dxchk( pD3DDevice->CreateRenderTargetView( pTexture, NULL, &pRTV.Ptr ) );
#else
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	D3D_GetTexture2D_ShaderResourceView_Desc( pTexDesc->Format, srDesc );

	D3D11_RENDER_TARGET_VIEW_DESC	rtDesc;
	rtDesc.Format = pTexDesc->Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	dxchk( pD3DDevice->CreateTexture2D( pTexDesc, NULL, &pTexture.Ptr ) );
	dxchk( pD3DDevice->CreateShaderResourceView( pTexture, &srDesc, &pSRV.Ptr ) );
	dxchk( pD3DDevice->CreateRenderTargetView( pTexture, &rtDesc, &pRTV.Ptr ) );
#endif
}

void RenderTarget::Release()
{
	//DBG_TRACE_CALL;
	pTexture = nil;
	pRTV = nil;
	pSRV = nil;
}

void RenderTarget::CalcWidthHeight( UINT &OutWidth, UINT &OutHeight ) const
{
	D3D_CalcTexture2DSize( pTexture, OutWidth, OutHeight );
}

UINT RenderTarget::CalcSizeInBytes() const
{
	D3D11_TEXTURE2D_DESC	texDesc;
	pTexture->GetDesc( &texDesc );

	const UINT textureSize = DXGIFormat_GetElementSize( texDesc.Format ) * (texDesc.Width * texDesc.Height);
	return textureSize;
}

/*
--------------------------------------------------------------
	DepthStencil
--------------------------------------------------------------
*/

DepthStencil::DepthStencil()
{}

void DepthStencil::Create(
						  UINT nWidth,
						  UINT nHeight,
						  DXGI_FORMAT eFormat,
						  bool bSampleDepthStencil,
						  UINT nSampleCount,
						  UINT nSampleQuality
						  )
{
	Assert(this->IsNull());

	ID3D11Device* device = GetD3DDevice();

	UINT bindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (bSampleDepthStencil)
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;

	DXGI_FORMAT dsTexFormat;
	if( bSampleDepthStencil ) {
		dsTexFormat = D3D_GetDepthStencilSRVFormat(eFormat);
	} else {
		dsTexFormat = eFormat;
	}

	//if (!bSampleDepthStencil)
	//	dsTexFormat = format;
	//else if (format == DXGI_FORMAT_D16_UNORM)
	//	dsTexFormat = DXGI_FORMAT_R16_TYPELESS;
	//else if(format == DXGI_FORMAT_D24_UNORM_S8_UINT)
	//	dsTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
	//else
	//	dsTexFormat = DXGI_FORMAT_R32_TYPELESS;

	D3D11_TEXTURE2D_DESC desc;
	{
		desc.Width = nWidth;
		desc.Height = nHeight;
		desc.ArraySize = 1;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = 0;
		desc.Format = dsTexFormat;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = nSampleCount;
		desc.SampleDesc.Quality = nSampleQuality;
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	dxchk(device->CreateTexture2D(&desc, NULL, &pTexture.Ptr));


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	{
		dsvDesc.Format = eFormat;
		dsvDesc.ViewDimension = nSampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;        
		dsvDesc.Flags = 0;
	}
	dxchk(device->CreateDepthStencilView(pTexture, &dsvDesc, &pDSV.Ptr));


	if (bSampleDepthStencil)
	{
		DXGI_FORMAT dsSRVFormat;
		if (eFormat == DXGI_FORMAT_D16_UNORM)
			dsSRVFormat = DXGI_FORMAT_R16_UNORM;
		else if(eFormat == DXGI_FORMAT_D24_UNORM_S8_UINT)
			dsSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS ;
		else
			dsSRVFormat = DXGI_FORMAT_R32_FLOAT;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		{
			srvDesc.Format = dsSRVFormat;
			srvDesc.ViewDimension = nSampleCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}
		dxchk(device->CreateShaderResourceView(pTexture, &srvDesc, &pSRV.Ptr));
	}
}

void DepthStencil::CalcWidthHeight( UINT &OutWidth, UINT &OutHeight ) const
{
	D3D_CalcTexture2DSize( pTexture, OutWidth, OutHeight );
}

UINT DepthStencil::CalcSizeInBytes() const
{
	D3D11_TEXTURE2D_DESC	texDesc;
	pTexture->GetDesc( &texDesc );

	const UINT textureSize = DXGIFormat_GetElementSize( texDesc.Format ) * (texDesc.Width * texDesc.Height);
	return textureSize;
}

//-------------------------------------------------------------------------------------------------------------//



//-------------------------------------------------------------------------------------------------------------//

ID3D11Texture2D* D3D_CreateTexture2D(
	const D3D11_TEXTURE2D_DESC& desc,
	const D3D11_SUBRESOURCE_DATA* initialData
)
{
	ID3D11Texture2D * pNewTexture = nil;

	dxchk(D3DDevice->CreateTexture2D(
		&desc,
		initialData,
		&pNewTexture
	));

	return pNewTexture;
}

//-------------------------------------------------------------------------------------------------------------//
ID3D11ShaderResourceView* D3D_LoadResourceFromFile(
	const char* fileName
	)
{
	AssertPtr(fileName);

	ID3D11ShaderResourceView* p = nil;

	dxchk(D3DX11CreateShaderResourceViewFromFileA(
		D3DDevice,
		fileName,
		nil,	// D3DX11_IMAGE_LOAD_INFO* pLoadInfo
		nil,	// ID3DX11ThreadPump *pPump
		&p,
		nil
	));

	return p;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11ShaderResourceView* D3D_LoadResourceFromMemory(
	const void* data, const SizeT size
	)
{
	AssertPtr(data);
	Assert(size > 0);

	ID3D11ShaderResourceView* p = nil;

	dxchk(D3DX11CreateShaderResourceViewFromMemory(
		D3DDevice,
		data,
		size,
		nil,	// D3DX11_IMAGE_LOAD_INFO* pLoadInfo
		nil,	// ID3DX11ThreadPump *pPump
		&p,
		nil
	));

	return p;
}

//-------------------------------------------------------------------------------------------------------------//


/*================================
	rxHardwareBuffer
================================*/

void* rxHardwareBuffer::Lock()
{
	return D3D_LockBuffer( D3DContext, pD3DBuffer );
}

void rxHardwareBuffer::Unlock()
{
	D3D_UnlockBuffer( D3DContext, pD3DBuffer );
}

//-------------------------------------------------------------------------------------------------------------//

ID3D11Buffer* D3D_CreateVertexBuffer(
	UINT size,
	UINT stride,
	const void* data,
	bool dynamic
){
	Assert( size > 0 );
	Assert( stride > 0 );

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage		= dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth	= size;
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags	= 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Buffer *	d3dBuffer;
	dxchk(D3DDevice->CreateBuffer(
		&bufferDesc,
		data ? &initData : nil,
		&d3dBuffer
	));

	return d3dBuffer;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11Buffer* D3D_CreateIndexBuffer(
	UINT size,
	UINT stride,
	const void* data,
	bool dynamic
){
	Assert( size > 0 );
	Assert( stride == sizeof(UINT16) || stride == sizeof(UINT32) );

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage		= dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth	= size;
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags	= 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	ID3D11Buffer *	d3dBuffer;
	dxchk(D3DDevice->CreateBuffer(
		&bufferDesc,
		data ? &initData : nil,
		&d3dBuffer
	));

	return d3dBuffer;
}
//-------------------------------------------------------------------------------------------------------------//
ID3D11InputLayout* D3D_CreateInputLayout(
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	UINT NumElements
)
{
	// Compile a dummy vertex shader.

	typedef FixedTextBufferANSI<2048>	CodeSnippet;

	CodeSnippet	dummyVertexShaderCode;

	CodeSnippet::OStream codeWriter = dummyVertexShaderCode.GetOStream();

	mxTextWriter	tw(codeWriter);


/*	Old code:
 *	
		tw << "struct VS_In {\n";

		for(UINT iVertexElement = 0;
			iVertexElement < NumElements;
			iVertexElement++)
		{
			const D3D11_INPUT_ELEMENT_DESC & elemDesc = pInputElementDescs[ iVertexElement ];
			tw.Putf("	%s v_%u : %s;\n"
				,DXGIFormat_TypeString(elemDesc.Format)
				,iVertexElement
				,elemDesc.SemanticName
			);
		}

		tw << "};\n";


		tw << "float4 VS_Main( in VS_In input ) : SV_Position\n"
			"{\n"
			"	return float4(0,0,0,1);\n"
			"}"
		;

		//DBGOUT(dummyVertexShaderCode.ToChars());

		dxPtr< ID3DBlob > compiledCode = D3D_CompileShader(
			dummyVertexShaderCode.ToChars(),
			dummyVertexShaderCode.Num(),
			D3D_GetVertexShaderProfile(),
			"VS_Main"
		);
*/


	{
		tw << "void F(\n";

		for(UINT iVertexElement = 0;
			iVertexElement < NumElements;
			iVertexElement++)
		{
			const D3D11_INPUT_ELEMENT_DESC & elemDesc = pInputElementDescs[ iVertexElement ];
			tw.Putf("%s _%u : %s%s\n"
				,DXGIFormat_TypeString(elemDesc.Format)
				,iVertexElement
				,elemDesc.SemanticName
				, (iVertexElement != NumElements-1) ? "," : ""
			);
		}

		tw << "){}\n";
		;
	}

	dxPtr< ID3DBlob > compiledCode = D3D_CompileShader(
		dummyVertexShaderCode.ToChars(),
		dummyVertexShaderCode.Num(),
		D3D_GetVertexShaderProfile(),
		"F"
	);


	const void* VSBytecode = compiledCode->GetBufferPointer();
	SizeT bytecodeLength = compiledCode->GetBufferSize();

	AssertPtr(VSBytecode);
	Assert_GZ(bytecodeLength);

	// Create a new vertex declaration.

	ID3D11InputLayout *	pNewVertexLayout = nil;

	dxchk( D3DDevice->CreateInputLayout(
		pInputElementDescs,
		NumElements,
		VSBytecode,
		bytecodeLength,
		& pNewVertexLayout )
	);

	return pNewVertexLayout;
}


/*
--------------------------------------------------------------
	GrIndexBuffer
--------------------------------------------------------------
*/
void GrIndexBuffer::Create( const void* data, const UINT size, const UINT stride )
{
	Assert( this->IsNull() );
	Assert( this->format == DXGI_FORMAT_UNKNOWN );

	Assert( size > 0 );
	Assert( stride == 2 || stride == 4 );

	graphics.resources->Create_IndexBuffer(
		*this,
		size,
		stride,
		data
		);
}


/*
--------------------------------------------------------------
	D3D11ResourceSystem
--------------------------------------------------------------
*/

D3D11ResourceSystem::D3D11ResourceSystem()
{
}
//-------------------------------------------------------------------------------------------------------------//
D3D11ResourceSystem::~D3D11ResourceSystem()
{
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_SamplerState(
	ID3D11SamplerStatePtr &r,
	const D3D11_SAMPLER_DESC& desc
)
{
	Assert(r.IsNull());

	ID3D11SamplerState* newSamplerState = nil;

	dxchk(D3DDevice->CreateSamplerState(
		&desc,
		&newSamplerState
	));

	//if(PtrToBool( newSamplerState ))
	{
		r = newSamplerState;
	}
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_SamplerState(
	ID3D11SamplerStatePtr &r
)
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_DepthStencilState(
	ID3D11DepthStencilStatePtr &r,
	const D3D11_DEPTH_STENCIL_DESC& desc
)
{
	Assert(r.IsNull());

	ID3D11DepthStencilState* newDepthStencilState = nil;

	dxchk(D3DDevice->CreateDepthStencilState(
		&desc,
		&newDepthStencilState
	));

	r = newDepthStencilState;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_DepthStencilState( ID3D11DepthStencilStatePtr &r )
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_BlendState(
	ID3D11BlendStatePtr &r,
	const D3D11_BLEND_DESC& desc
)
{
	Assert(r.IsNull());

	ID3D11BlendState* newBlendState = nil;

	dxchk(D3DDevice->CreateBlendState(
		&desc,
		&newBlendState
	));

	r = newBlendState;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_BlendState( ID3D11BlendStatePtr &r )
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_RasterizerState(
	ID3D11RasterizerStatePtr &r,
	const D3D11_RASTERIZER_DESC& desc
)
{
	Assert(r.IsNull());

	ID3D11RasterizerState* newRasterizerState = nil;

	dxchk(D3DDevice->CreateRasterizerState(
		&desc,
		&newRasterizerState
	));

	r = newRasterizerState;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_RasterizerState( ID3D11RasterizerStatePtr &r )
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_RenderTarget(
	RenderTarget &r,
	const D3D11_TEXTURE2D_DESC& tex2DDesc,
	const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc,
	const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc
	)
{
	Assert( r.IsNull() );

	ID3D11Device* pD3DDevice = D3DDevice;

	ID3D11Texture2D* pTexture = nil;
	ID3D11RenderTargetView* pRTV = nil;
	ID3D11ShaderResourceView* pSRV = nil;

	dxchk( pD3DDevice->CreateTexture2D( &tex2DDesc, nil, &pTexture ));
	dxchk( pD3DDevice->CreateRenderTargetView(  pTexture, &rtvDesc, &pRTV ));
	dxchk( pD3DDevice->CreateShaderResourceView( pTexture, &srvDesc, &pSRV ) );

	r.pTexture = pTexture;
	r.pRTV = pRTV;
	r.pSRV = pSRV;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_RenderTarget( RenderTarget &r )
{
	r.Release();
#if 0
	r.pTexture = nil;
	r.pRTV = nil;
	r.pSRV = nil;
#endif
}

//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_InputLayout(
	ID3D11InputLayoutPtr &r,
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	UINT NumElements
	)
{
	Assert(r.IsNull());

	ID3D11InputLayout* p = D3D_CreateInputLayout( pInputElementDescs, NumElements );
	r = p;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_InputLayout( ID3D11InputLayoutPtr &r )
{
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_VertexBuffer(
	ID3D11BufferPtr & r,
	UINT size,
	UINT stride,
	const void* data,
	bool dynamic
)
{
	//DBG_TRACE_CALL;
	Assert(r.IsNull());

	ID3D11Buffer* newBuffer = D3D_CreateVertexBuffer(size,stride,data,dynamic);
	r = newBuffer;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_VertexBuffer(
	ID3D11BufferPtr &r
)
{
	mxDBG_TRACE_CALL;
	r = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Create_IndexBuffer(
	GrIndexBuffer & r,
	UINT size,
	UINT stride,
	const void* data,
	bool dynamic
)
{
	//DBG_TRACE_CALL;
	Assert(r.IsNull());
	Assert((stride == sizeof U2) || (stride == sizeof U4));

	ID3D11Buffer* newBuffer = D3D_CreateIndexBuffer(size,stride,data,dynamic);
	r.pD3DBuffer = newBuffer;
	r.format = (stride == sizeof U2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11ResourceSystem::Destroy_IndexBuffer(
	GrIndexBuffer &r
)
{
	mxDBG_TRACE_CALL;
	r.Clear();
}
//-------------------------------------------------------------------------------------------------------------//



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
