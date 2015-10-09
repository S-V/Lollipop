#include "stdafx.h"

#include <Core/Util/Tweakable.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>
#include <Renderer/GPU/VertexFormats.hxx>

#include "render/hit_testing.h"


/*
-----------------------------------------------------------------------------
	AHitProxy
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( AHitProxy );

AHitProxy::AHitProxy( UINT flags )
	: m__flags( flags )
{
}

AHitProxy::~AHitProxy()
{
	if( !(m__flags & HitProxy_Permanent) )
	{
		// for safety
		GetHitTesting().RemoveHitProxy( this );
	}
}

namespace
{
	typedef GPU::p_hit_proxy	HitProxyShader;

	const DXGI_FORMAT HitProxyBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	FORCEINLINE float4 HitProxy_IndexToRGBAColor( const UINT iHitProxy )
	{
		FColor	color;
		color.FromRGBA(iHitProxy);

#if MX_DEBUG
		static bool bDebugHitProxyRT = false;
		if( HOT_BOOL(bDebugHitProxyRT) )
		{
			color = FColor::RED;	//debug
		}
#endif // MX_DEBUG

		return color.mSimdQuad;
	}
	FORCEINLINE UINT HitProxy_RGBAColorToIndex( const R8G8B8A8 color )
	{
		//NOTE: this is incorrect:
		//UINT	iHitProxy = MAKEFOURCC(color.R,color.G,color.B,color.A);
		// because:
		// A - the lowest bits, R - the highest ones
		//
		UINT	iHitProxy = MAKEFOURCC(color.A,color.B,color.G,color.R);
		return iHitProxy;
	}

	struct PrivateData
	{
		// Hit testing

		TList<AHitProxy*>	hitProxies;
		TPtr<AHitProxy>		currentHitProxy;

		float4x4	currTransform;

		// the same dimensions as the main viewport
		RenderTarget	hitProxyRT;

		// readable by CPU
		dxPtr<ID3D11Texture2D>	hitProxyBufferCPU;

		UINT	hitProxyBufferWidth, hitProxyBufferHeight;
		TList<R8G8B8A8>		hitProxyPixels;
		//TList<U32>			hitProxyZBuffer;

	public:
		PrivateData( UINT newWidth, UINT newHeight )
			: hitProxies(EMemHeap::HeapEditor)
		{
			hitProxyBufferWidth = 0;
			hitProxyBufferHeight = 0;

			this->OnMainViewportResized( newWidth, newHeight );

			this->Reset();

			mxCONNECT_THIS( gRenderer.AfterMainViewportResized, PrivateData, OnMainViewportResized );
		}
		~PrivateData()
		{
			mxDISCONNECT_THIS( gRenderer.AfterMainViewportResized );
		}
		void Reset()
		{
			hitProxies.Empty();
			hitProxies.Add(NULL_HIT_PROXY);
			currentHitProxy = nil;

			currTransform = XMMatrixIdentity();

			HitProxyShader::Data* pData = HitProxyShader::cb_Data.Map( D3DContext );
			{
				pData->transform = currTransform;
				pData->hitProxyId = XMVectorSet(-1,-1,-1,-1);
			}
			HitProxyShader::cb_Data.Unmap( D3DContext );
		}
		void OnMainViewportResized( UINT newWidth, UINT newHeight )
		{
			//DBGOUT( "OnMainViewportResized: %ux%u.\n", newWidth, newHeight );

			if( hitProxyRT.pTexture != nil )
			{
				gRenderer.editor->GetTextureInspector()->RemoveDebugItem(hitProxyRT.pTexture);
				hitProxyRT.Release();
			}
			if( hitProxyBufferCPU != nil )
			{
				//textureInspector.RemoveDebugItem(hitProxyBufferCPU);
				hitProxyBufferCPU = nil;
			}

			ID3D11Device* pD3DDevice = GetD3DDevice();

			DX11::Texture2DDescription	texDesc( HitProxyBufferFormat, newWidth, newHeight );
			texDesc
				.ArraySize_(1)
				.MipLevels_(1)
				.SampleCount_(1)
				.SampleQuality_(0)
				;

			{
				texDesc
					.Usage_(D3D11_USAGE_DEFAULT)
					.BindFlags_(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET)
					.CPUAccessFlags_(0)
					;
				hitProxyRT.Create( &texDesc );
				gRenderer.editor->GetTextureInspector()->AddDebugItem( hitProxyRT.pTexture, "HitProxyRT" );
			}

			{
				texDesc
					.Usage_(D3D11_USAGE_STAGING)
					.BindFlags_(0)
					.CPUAccessFlags_(D3D11_CPU_ACCESS_READ)
					;
				dxchk( pD3DDevice->CreateTexture2D( &texDesc, NULL, &hitProxyBufferCPU.Ptr ) );
				//textureInspector.AddDebugItem(hitProxyBufferCPU);
			}
		}
	};

	TPtr<PrivateData>	self;

	bool inBeginEnd = false;

}//namespace


HitTesting::HitTesting()
{
}

HitTesting::~HitTesting()
{
}

void HitTesting::Initialize( UINT viewportWidth, UINT viewportHeight )
{
	mxSTATIC_IN_PLACE_CTOR_X( self, PrivateData, viewportWidth, viewportHeight );
}

void HitTesting::Shutdown()
{
	self.Destruct();
}

static
void HitProxy_ReadPixelsFromGpuMem( TList<R8G8B8A8> &OutPixels, UINT &OutWidth, UINT &OutHeight )
{
	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	ID3D11Texture2D* pSrcResource = self->hitProxyRT.pTexture;
	ID3D11Texture2D* pDstResource = self->hitProxyBufferCPU;

	const UINT iDstSubResource = 0;

	pD3DContext->CopyResource(
		pDstResource,
		pSrcResource
	);

	dxScopedLock	mappedTexture( pD3DContext, pDstResource, D3D11_MAP_READ, iDstSubResource );

	const R8G8B8A8* pixels = mappedTexture.As< const R8G8B8A8* >();

	const UINT rowPitchInBytes = mappedTexture.GetMappedData().RowPitch;
	const UINT depthPitchInBytes = mappedTexture.GetMappedData().DepthPitch;
	const UINT iNumRows = depthPitchInBytes / rowPitchInBytes;
	const UINT numColumns = rowPitchInBytes / sizeof pixels[0];

	mxWHY("textureSize != mappedTextureSize?");

	OutWidth = numColumns;
	OutHeight = iNumRows;
	OutPixels.SetNum( iNumRows * numColumns );

	MemCopy( OutPixels.ToPtr(), pixels, depthPitchInBytes );
}

void HitTesting::Reset()
{
	self->Reset();
}

void HitTesting::Begin( const rxViewport& viewport )
{
	Assert(!inBeginEnd);
	inBeginEnd = true;

	Assert( gRenderer.IsMainViewport( viewport ) );

	this->Reset();

	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	{
		//const RenderTarget & mainRT = viewport.mainRT;
		const DepthStencil & mainDS = viewport.mainDS;

		const bool bCanPerformHitTests = (mainDS.pTexture != nil);
		Assert(bCanPerformHitTests);

		RenderTarget & hitProxyRT = self->hitProxyRT;

		Assert(D3D_EqualDimensions( hitProxyRT.pTexture, mainDS.pTexture ));

		FColor	clearColor;
		clearColor.SetAll(0.0f);

		pD3DContext->ClearRenderTargetView( hitProxyRT.pRTV, clearColor.ToFloatPtr() );

		// NOTE: need to clear depth-stencil because weird artifacts occur
		// even with depth func = less_or_equal
		pD3DContext->ClearDepthStencilView( mainDS.pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,	0 );

		pD3DContext->OMSetRenderTargets( 1, &hitProxyRT.pRTV.Ptr, mainDS.pDSV );

		pD3DContext->RSSetViewports( 1, &viewport.d );


		{
			//HitProxyShader::cb_Data.transform = self->currentTransform;
			HitProxyShader::Set( pD3DContext );
		}

		pD3DContext->IASetInputLayout( GPU::Vertex_P3f::layout );
	}
}

void HitTesting::End()
{
	Assert(inBeginEnd);
	inBeginEnd = false;

	HitProxy_ReadPixelsFromGpuMem( self->hitProxyPixels, self->hitProxyBufferWidth, self->hitProxyBufferHeight );

#if(MX_DEBUG)
	{
		static bool bDumpHitProxyBuffer = false;
		bDumpHitProxyBuffer = HOT_BOOL(bDumpHitProxyBuffer);
		if( bDumpHitProxyBuffer )
		{
			dxchk(D3DX11SaveTextureToFileA(
				D3DContext,
				self->hitProxyBufferCPU,
				D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_DDS,
				"HitProxyRenderTarget.dds"
				));

			bDumpHitProxyBuffer = false;
		}
	}
#endif
}

void HitTesting::BeginHitProxy( AHitProxy* pNewHitProxy )
{
	Assert(self->currentHitProxy == nil);

	const UINT iNewHitProxyIndex = self->hitProxies.Num();

	pNewHitProxy->internalIndex = iNewHitProxyIndex;
	self->hitProxies.Add(pNewHitProxy);

	self->currentHitProxy = pNewHitProxy;
	{
		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

		HitProxyShader::Data* pData = HitProxyShader::cb_Data.Map( pD3DContext );
		{
			pData->transform = self->currTransform;
			pData->hitProxyId = HitProxy_IndexToRGBAColor( iNewHitProxyIndex );
		}
		HitProxyShader::cb_Data.Unmap( pD3DContext );
	}

	//return iNewHitProxyIndex;
}

void HitTesting::EndHitProxy()
{
	Assert(self->currentHitProxy != nil);
	self->currentHitProxy = nil;
}

AHitProxy* HitTesting::GetHitProxy( const rxViewport& viewport, const UINT x, const UINT y )
{
	Assert( gRenderer.IsMainViewport( viewport ) );

	if( self->hitProxies.Num() <= 1 )	// [0] is NULL_HIT_PROXY
	{
		return nil;
	}

	const UINT offset = x + y * self->hitProxyBufferWidth;

	if( !self->hitProxyPixels.IsValidIndex(offset) )
	{
		return nil;
	}

	const R8G8B8A8 color = self->hitProxyPixels[ offset ];
	const U4 hitProxyIndex = HitProxy_RGBAColorToIndex( color );

	if( hitProxyIndex > NULL_HIT_PROXY_ID && hitProxyIndex < self->hitProxies.Num() )
	{
		//DBGOUT("At x=%u, y=%u: hitProxyId = %u\n\n",
		//	x,y,hitProxyIndex);
		AHitProxy* pHitProxy = self->hitProxies[ hitProxyIndex ];
		return pHitProxy;
	}
	return nil;
}

void HitTesting::RemoveHitProxy( AHitProxy* pHitProxy )
{
	CHK_VRET_IF_NIL(pHitProxy);

	const UINT hitProxyIndex = self->hitProxies.FindIndexOf( pHitProxy );
	VRET_IF_NOT( hitProxyIndex != INDEX_NONE );

	self->hitProxies[ hitProxyIndex ] = nil;
}

void HitTesting::ReadPixels( TList<R8G8B8A8> &OutPixels, UINT &OutWidth, UINT &OutHeight )
{
	OutPixels = self->hitProxyPixels;
	OutWidth = self->hitProxyBufferWidth;
	OutHeight = self->hitProxyBufferHeight;
}

void HitTesting::Clear()
{
	self->Reset();
}

void HitTesting::SetTransform( mat4_carg transform )
{
	Assert(inBeginEnd);

	self->currTransform = transform;


	HitProxyShader::Data* pData = HitProxyShader::cb_Data.Map( D3DContext );

	pData->transform = transform;

	if(self->currentHitProxy != nil)
	{
		pData->hitProxyId = HitProxy_IndexToRGBAColor(self->currentHitProxy->internalIndex);
	}

	HitProxyShader::cb_Data.Unmap( D3DContext );
}
