/*
=============================================================================
	File:	D3D11DeviceContext.h
	Desc:	Proxy (redirecting) device context,
			caches states and filters out redundant API calls.
	Note:	.
=============================================================================
*/
#pragma once

#define RX_USE_PROXY_DEVICE_CONTEXT		(1)



#define ProxyDeviceContext_Inline	FORCEINLINE

/*
-----------------------------------------------------------------------------
	D3D11DeviceContext
-----------------------------------------------------------------------------
*/
class D3D11DeviceContext : public ID3D11DeviceContext
{
	dxPtr< ID3D11DeviceContext >	m_pD3DContext;

	// Keep track of current states.

	ID3D11InputLayout *		m_pCurrentInputLayout;

	D3D11_PRIMITIVE_TOPOLOGY	m_nCurrentTopology;

	ID3D11RasterizerState *		m_pCurrentRasterizerState;
	ID3D11DepthStencilState *	m_pCurrentDepthStencilState;
	UINT						m_pCurrentStencilRef;
	//ID3D11BlendState *			m_pCurrentBlendState;

	ID3D11VertexShader *	m_pCurrentVertexShader;
	ID3D11HullShader *		m_pCurrentHullShader;
	ID3D11DomainShader *	m_pCurrentDomainShader;
	ID3D11GeometryShader *	m_pCurrentGeometryShader;
	ID3D11PixelShader *		m_pCurrentPixelShader;

private:
	inline void ClearCachedState()
	{
		m_pCurrentInputLayout = nil;

		m_nCurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		m_pCurrentRasterizerState = nil;
		m_pCurrentDepthStencilState = nil;
		m_pCurrentStencilRef = 0;
		//m_pCurrentBlendState = nil;

		m_pCurrentVertexShader = nil;
		m_pCurrentHullShader = nil;
		m_pCurrentDomainShader = nil;
		m_pCurrentGeometryShader = nil;
		m_pCurrentPixelShader = nil;
	}

public:
	inline D3D11DeviceContext( ID3D11DeviceContext* pD3DDeviceContext )
	{
		AssertPtr(pD3DDeviceContext);
		m_pD3DContext = pD3DDeviceContext;

		this->ClearCachedState();

		// Synchronize with the original device context.

		pD3DDeviceContext->IASetInputLayout( m_pCurrentInputLayout );
		pD3DDeviceContext->IASetPrimitiveTopology( m_nCurrentTopology );
	
		pD3DDeviceContext->RSSetState( m_pCurrentRasterizerState );
		pD3DDeviceContext->OMSetDepthStencilState( m_pCurrentDepthStencilState, m_pCurrentStencilRef );
		pD3DDeviceContext->OMSetBlendState( nil, nil, 0 );

		pD3DDeviceContext->VSSetShader( nil, nil, 0 );
		pD3DDeviceContext->HSSetShader( nil, nil, 0 );
		pD3DDeviceContext->DSSetShader( nil, nil, 0 );
		pD3DDeviceContext->GSSetShader( nil, nil, 0 );
		pD3DDeviceContext->PSSetShader( nil, nil, 0 );
	}
	inline ~D3D11DeviceContext()
	{
	}

	// IUnknown interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);

	virtual ULONG STDMETHODCALLTYPE Release( void);


	// ID3D11DeviceChild interface

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GetDevice( 
		/* [annotation] */ 
		__out  ID3D11Device **ppDevice);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE GetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__inout  UINT *pDataSize,
		/* [annotation] */ 
		__out_bcount_opt( *pDataSize )  void *pData);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE SetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in  UINT DataSize,
		/* [annotation] */ 
		__in_bcount_opt( DataSize )  const void *pData);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in_opt  const IUnknown *pData);


	// ID3D11DeviceContext interface

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSSetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSSetShader( 
		__in_opt  ID3D11PixelShader *pPixelShader,
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSSetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11VertexShader *pVertexShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawIndexed( 
		/* [annotation] */ 
		__in  UINT IndexCount,
		/* [annotation] */ 
		__in  UINT StartIndexLocation,
		/* [annotation] */ 
		__in  INT BaseVertexLocation);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE Draw( 
		/* [annotation] */ 
		__in  UINT VertexCount,
		/* [annotation] */ 
		__in  UINT StartVertexLocation);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE Map( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in  UINT Subresource,
		/* [annotation] */ 
		__in  D3D11_MAP MapType,
		/* [annotation] */ 
		__in  UINT MapFlags,
		/* [annotation] */ 
		__out  D3D11_MAPPED_SUBRESOURCE *pMappedResource);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE Unmap( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in  UINT Subresource);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IASetInputLayout( 
		/* [annotation] */ 
		__in_opt  ID3D11InputLayout *pInputLayout);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IASetVertexBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  const UINT *pStrides,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  const UINT *pOffsets);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IASetIndexBuffer( 
		/* [annotation] */ 
		__in_opt  ID3D11Buffer *pIndexBuffer,
		/* [annotation] */ 
		__in  DXGI_FORMAT Format,
		/* [annotation] */ 
		__in  UINT Offset);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawIndexedInstanced( 
		/* [annotation] */ 
		__in  UINT IndexCountPerInstance,
		/* [annotation] */ 
		__in  UINT InstanceCount,
		/* [annotation] */ 
		__in  UINT StartIndexLocation,
		/* [annotation] */ 
		__in  INT BaseVertexLocation,
		/* [annotation] */ 
		__in  UINT StartInstanceLocation);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawInstanced( 
		/* [annotation] */ 
		__in  UINT VertexCountPerInstance,
		/* [annotation] */ 
		__in  UINT InstanceCount,
		/* [annotation] */ 
		__in  UINT StartVertexLocation,
		/* [annotation] */ 
		__in  UINT StartInstanceLocation);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11GeometryShader *pShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IASetPrimitiveTopology( 
		/* [annotation] */ 
		__in  D3D11_PRIMITIVE_TOPOLOGY Topology);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE Begin( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE End( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE GetData( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync,
		/* [annotation] */ 
		__out_bcount_opt( DataSize )  void *pData,
		/* [annotation] */ 
		__in  UINT DataSize,
		/* [annotation] */ 
		__in  UINT GetDataFlags);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE SetPredication( 
		/* [annotation] */ 
		__in_opt  ID3D11Predicate *pPredicate,
		/* [annotation] */ 
		__in  BOOL PredicateValue);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMSetRenderTargets( 
		/* [annotation] */ 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
		/* [annotation] */ 
		__in_opt  ID3D11DepthStencilView *pDepthStencilView);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews( 
		/* [annotation] */ 
		__in  UINT NumRTVs,
		/* [annotation] */ 
		__in_ecount_opt(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
		/* [annotation] */ 
		__in_opt  ID3D11DepthStencilView *pDepthStencilView,
		/* [annotation] */ 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
		/* [annotation] */ 
		__in  UINT NumUAVs,
		/* [annotation] */ 
		__in_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
		/* [annotation] */ 
		__in_ecount_opt(NumUAVs)  const UINT *pUAVInitialCounts);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMSetBlendState( 
		/* [annotation] */ 
		__in_opt  ID3D11BlendState *pBlendState,
		/* [annotation] */ 
		__in_opt  const FLOAT BlendFactor[ 4 ],
		/* [annotation] */ 
		__in  UINT SampleMask);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMSetDepthStencilState( 
		/* [annotation] */ 
		__in_opt  ID3D11DepthStencilState *pDepthStencilState,
		/* [annotation] */ 
		__in  UINT StencilRef);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE SOSetTargets( 
		/* [annotation] */ 
		__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount_opt(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
		/* [annotation] */ 
		__in_ecount_opt(NumBuffers)  const UINT *pOffsets);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawAuto( void);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawIndexedInstancedIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DrawInstancedIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE Dispatch( 
		/* [annotation] */ 
		__in  UINT ThreadGroupCountX,
		/* [annotation] */ 
		__in  UINT ThreadGroupCountY,
		/* [annotation] */ 
		__in  UINT ThreadGroupCountZ);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DispatchIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSSetState( 
		/* [annotation] */ 
		__in_opt  ID3D11RasterizerState *pRasterizerState);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSSetViewports( 
		/* [annotation] */ 
		__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
		/* [annotation] */ 
		__in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSSetScissorRects( 
		/* [annotation] */ 
		__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
		/* [annotation] */ 
		__in_ecount_opt(NumRects)  const D3D11_RECT *pRects);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CopySubresourceRegion( 
		/* [annotation] */ 
		__in  ID3D11Resource *pDstResource,
		/* [annotation] */ 
		__in  UINT DstSubresource,
		/* [annotation] */ 
		__in  UINT DstX,
		/* [annotation] */ 
		__in  UINT DstY,
		/* [annotation] */ 
		__in  UINT DstZ,
		/* [annotation] */ 
		__in  ID3D11Resource *pSrcResource,
		/* [annotation] */ 
		__in  UINT SrcSubresource,
		/* [annotation] */ 
		__in_opt  const D3D11_BOX *pSrcBox);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CopyResource( 
		/* [annotation] */ 
		__in  ID3D11Resource *pDstResource,
		/* [annotation] */ 
		__in  ID3D11Resource *pSrcResource);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE UpdateSubresource( 
		/* [annotation] */ 
		__in  ID3D11Resource *pDstResource,
		/* [annotation] */ 
		__in  UINT DstSubresource,
		/* [annotation] */ 
		__in_opt  const D3D11_BOX *pDstBox,
		/* [annotation] */ 
		__in  const void *pSrcData,
		/* [annotation] */ 
		__in  UINT SrcRowPitch,
		/* [annotation] */ 
		__in  UINT SrcDepthPitch);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CopyStructureCount( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pDstBuffer,
		/* [annotation] */ 
		__in  UINT DstAlignedByteOffset,
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pSrcView);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ClearRenderTargetView( 
		/* [annotation] */ 
		__in  ID3D11RenderTargetView *pRenderTargetView,
		/* [annotation] */ 
		__in  const FLOAT ColorRGBA[ 4 ]);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( 
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
		/* [annotation] */ 
		__in  const UINT Values[ 4 ]);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( 
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
		/* [annotation] */ 
		__in  const FLOAT Values[ 4 ]);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ClearDepthStencilView( 
		/* [annotation] */ 
		__in  ID3D11DepthStencilView *pDepthStencilView,
		/* [annotation] */ 
		__in  UINT ClearFlags,
		/* [annotation] */ 
		__in  FLOAT Depth,
		/* [annotation] */ 
		__in  UINT8 Stencil);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GenerateMips( 
		/* [annotation] */ 
		__in  ID3D11ShaderResourceView *pShaderResourceView);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE SetResourceMinLOD( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		FLOAT MinLOD);

	virtual ProxyDeviceContext_Inline FLOAT STDMETHODCALLTYPE GetResourceMinLOD( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ResolveSubresource( 
		/* [annotation] */ 
		__in  ID3D11Resource *pDstResource,
		/* [annotation] */ 
		__in  UINT DstSubresource,
		/* [annotation] */ 
		__in  ID3D11Resource *pSrcResource,
		/* [annotation] */ 
		__in  UINT SrcSubresource,
		/* [annotation] */ 
		__in  DXGI_FORMAT Format);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ExecuteCommandList( 
		/* [annotation] */ 
		__in  ID3D11CommandList *pCommandList,
		BOOL RestoreContextState);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11HullShader *pHullShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11DomainShader *pDomainShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSSetUnorderedAccessViews( 
		/* [annotation] */ 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
		/* [annotation] */ 
		__in_ecount(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
		/* [annotation] */ 
		__in_ecount(NumUAVs)  const UINT *pUAVInitialCounts);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11ComputeShader *pComputeShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSGetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSGetShader( 
		/* [annotation] */ 
		__out  ID3D11PixelShader **ppPixelShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSGetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSGetShader( 
		/* [annotation] */ 
		__out  ID3D11VertexShader **ppVertexShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE PSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IAGetInputLayout( 
		/* [annotation] */ 
		__out  ID3D11InputLayout **ppInputLayout);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IAGetVertexBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount_opt(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
		/* [annotation] */ 
		__out_ecount_opt(NumBuffers)  UINT *pStrides,
		/* [annotation] */ 
		__out_ecount_opt(NumBuffers)  UINT *pOffsets);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IAGetIndexBuffer( 
		/* [annotation] */ 
		__out_opt  ID3D11Buffer **pIndexBuffer,
		/* [annotation] */ 
		__out_opt  DXGI_FORMAT *Format,
		/* [annotation] */ 
		__out_opt  UINT *Offset);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSGetShader( 
		/* [annotation] */ 
		__out  ID3D11GeometryShader **ppGeometryShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE IAGetPrimitiveTopology( 
		/* [annotation] */ 
		__out  D3D11_PRIMITIVE_TOPOLOGY *pTopology);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE VSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GetPredication( 
		__out_opt  ID3D11Predicate **ppPredicate,
		__out_opt  BOOL *pPredicateValue);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE GSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMGetRenderTargets( 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		__out_ecount_opt(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
		__out_opt  ID3D11DepthStencilView **ppDepthStencilView);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews( 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
		__out_ecount_opt(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
		__out_opt  ID3D11DepthStencilView **ppDepthStencilView,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
		__out_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMGetBlendState( 
		__out_opt  ID3D11BlendState **ppBlendState,
		__out_opt  FLOAT BlendFactor[ 4 ],
		__out_opt  UINT *pSampleMask);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE OMGetDepthStencilState( 
		__out_opt  ID3D11DepthStencilState **ppDepthStencilState,
		__out_opt  UINT *pStencilRef);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE SOGetTargets( 
		__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSGetState( 
		__out  ID3D11RasterizerState **ppRasterizerState);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSGetViewports( 
		__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
		__out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE RSGetScissorRects( 
		__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
		__out_ecount_opt(*pNumRects)  D3D11_RECT *pRects);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSGetShader( 
		__out  ID3D11HullShader **ppHullShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE HSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSGetShader( 
		__out  ID3D11DomainShader **ppDomainShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE DSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSGetUnorderedAccessViews( 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
		__out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSGetShader( 
		__out  ID3D11ComputeShader **ppComputeShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE CSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE ClearState( void);

	virtual ProxyDeviceContext_Inline void STDMETHODCALLTYPE Flush( void);

	virtual ProxyDeviceContext_Inline D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType( void);

	virtual ProxyDeviceContext_Inline UINT STDMETHODCALLTYPE GetContextFlags( void);

	virtual ProxyDeviceContext_Inline HRESULT STDMETHODCALLTYPE FinishCommandList( 
		BOOL RestoreDeferredContextState,
		__out_opt  ID3D11CommandList **ppCommandList);
};

//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GetDevice( 
	/* [annotation] */ 
	__out  ID3D11Device **ppDevice)
{
	m_pD3DContext->GetDevice( ppDevice );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::GetPrivateData( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__inout  UINT *pDataSize,
	/* [annotation] */ 
	__out_bcount_opt( *pDataSize )  void *pData)
{
	return m_pD3DContext->GetPrivateData( guid, pDataSize, pData );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::SetPrivateData( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__in  UINT DataSize,
	/* [annotation] */ 
	__in_bcount_opt( DataSize )  const void *pData)
{
	return m_pD3DContext->SetPrivateData( guid, DataSize, pData );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::SetPrivateDataInterface( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__in_opt  const IUnknown *pData)
{
	return m_pD3DContext->SetPrivateDataInterface( guid, pData );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSSetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->PSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSSetShader( 
	__in_opt  ID3D11PixelShader *pPixelShader,
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	if( m_pCurrentPixelShader == pPixelShader ) {
		return;
	}
	m_pCurrentPixelShader = pPixelShader;

	m_pD3DContext->PSSetShader( pPixelShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSSetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->PSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11VertexShader *pVertexShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	if( m_pCurrentVertexShader == pVertexShader ) {
		return;
	}
	m_pCurrentVertexShader = pVertexShader;

	m_pD3DContext->VSSetShader( pVertexShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawIndexed( 
	/* [annotation] */ 
	__in  UINT IndexCount,
	/* [annotation] */ 
	__in  UINT StartIndexLocation,
	/* [annotation] */ 
	__in  INT BaseVertexLocation)
{
	//D3D11_PRIMITIVE_TOPOLOGY currTopology;
	//this->IAGetPrimitiveTopology( &currTopology );

	m_pD3DContext->DrawIndexed( IndexCount, StartIndexLocation, BaseVertexLocation );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::Draw( 
	/* [annotation] */ 
	__in  UINT VertexCount,
	/* [annotation] */ 
	__in  UINT StartVertexLocation)
{
	m_pD3DContext->Draw( VertexCount, StartVertexLocation );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::Map( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in  UINT Subresource,
	/* [annotation] */ 
	__in  D3D11_MAP MapType,
	/* [annotation] */ 
	__in  UINT MapFlags,
	/* [annotation] */ 
	__out  D3D11_MAPPED_SUBRESOURCE *pMappedResource)
{
	return m_pD3DContext->Map( pResource, Subresource, MapType, MapFlags, pMappedResource );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::Unmap( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in  UINT Subresource)
{
	m_pD3DContext->Unmap( pResource, Subresource );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->PSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IASetInputLayout( 
	/* [annotation] */ 
	__in_opt  ID3D11InputLayout *pInputLayout)
{
	if( m_pCurrentInputLayout == pInputLayout ) {
		return;
	}
	m_pCurrentInputLayout = pInputLayout;
	m_pD3DContext->IASetInputLayout( pInputLayout );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IASetVertexBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  const UINT *pStrides,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  const UINT *pOffsets)
{
	m_pD3DContext->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IASetIndexBuffer( 
	/* [annotation] */ 
	__in_opt  ID3D11Buffer *pIndexBuffer,
	/* [annotation] */ 
	__in  DXGI_FORMAT Format,
	/* [annotation] */ 
	__in  UINT Offset)
{
	m_pD3DContext->IASetIndexBuffer( pIndexBuffer, Format, Offset );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawIndexedInstanced( 
	/* [annotation] */ 
	__in  UINT IndexCountPerInstance,
	/* [annotation] */ 
	__in  UINT InstanceCount,
	/* [annotation] */ 
	__in  UINT StartIndexLocation,
	/* [annotation] */ 
	__in  INT BaseVertexLocation,
	/* [annotation] */ 
	__in  UINT StartInstanceLocation)
{
	return m_pD3DContext->DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawInstanced( 
	/* [annotation] */ 
	__in  UINT VertexCountPerInstance,
	/* [annotation] */ 
	__in  UINT InstanceCount,
	/* [annotation] */ 
	__in  UINT StartVertexLocation,
	/* [annotation] */ 
	__in  UINT StartInstanceLocation)
{
	m_pD3DContext->DrawInstanced( VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->GSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11GeometryShader *pShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	if( m_pCurrentGeometryShader == pShader ) {
		return;
	}
	m_pCurrentGeometryShader = pShader;
	m_pD3DContext->GSSetShader( pShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IASetPrimitiveTopology( 
	/* [annotation] */ 
	__in  D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	if( m_nCurrentTopology == Topology ) {
		return;
	}
	m_nCurrentTopology = Topology;
	m_pD3DContext->IASetPrimitiveTopology( Topology );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSSetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->VSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSSetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->VSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::Begin( 
	/* [annotation] */ 
	__in  ID3D11Asynchronous *pAsync)
{
	m_pD3DContext->Begin( pAsync );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::End( 
/* [annotation] */ 
__in  ID3D11Asynchronous *pAsync)
{
	m_pD3DContext->End( pAsync );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::GetData( 
	/* [annotation] */ 
	__in  ID3D11Asynchronous *pAsync,
	/* [annotation] */ 
	__out_bcount_opt( DataSize )  void *pData,
	/* [annotation] */ 
	__in  UINT DataSize,
	/* [annotation] */ 
	__in  UINT GetDataFlags)
{
	return m_pD3DContext->GetData( pAsync, pData, DataSize, GetDataFlags );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::SetPredication( 
	/* [annotation] */ 
	__in_opt  ID3D11Predicate *pPredicate,
	/* [annotation] */ 
	__in  BOOL PredicateValue)
{
	m_pD3DContext->SetPredication( pPredicate, PredicateValue );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSSetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->GSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSSetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->GSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMSetRenderTargets( 
	/* [annotation] */ 
	__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
	/* [annotation] */ 
	__in_opt  ID3D11DepthStencilView *pDepthStencilView)
{
	m_pD3DContext->OMSetRenderTargets( NumViews, ppRenderTargetViews, pDepthStencilView );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews( 
	/* [annotation] */ 
	__in  UINT NumRTVs,
	/* [annotation] */ 
	__in_ecount_opt(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
	/* [annotation] */ 
	__in_opt  ID3D11DepthStencilView *pDepthStencilView,
	/* [annotation] */ 
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
	/* [annotation] */ 
	__in  UINT NumUAVs,
	/* [annotation] */ 
	__in_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
	/* [annotation] */ 
	__in_ecount_opt(NumUAVs)  const UINT *pUAVInitialCounts)
{
	m_pD3DContext->OMSetRenderTargetsAndUnorderedAccessViews( NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMSetBlendState( 
	/* [annotation] */ 
	__in_opt  ID3D11BlendState *pBlendState,
	/* [annotation] */ 
	__in_opt  const FLOAT BlendFactor[ 4 ],
	/* [annotation] */ 
	__in  UINT SampleMask)
{
	//if( m_pCurrentBlendState == pBlendState ) {
	//	return;
	//}
	//m_pCurrentBlendState = pBlendState;
	m_pD3DContext->OMSetBlendState( pBlendState, BlendFactor, SampleMask );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMSetDepthStencilState( 
	/* [annotation] */ 
	__in_opt  ID3D11DepthStencilState *pDepthStencilState,
	/* [annotation] */ 
	__in  UINT StencilRef)
{
	if( m_pCurrentDepthStencilState == pDepthStencilState
		&& m_pCurrentStencilRef == StencilRef )
	{
		return;
	}
	m_pCurrentDepthStencilState = pDepthStencilState;
	m_pCurrentStencilRef = StencilRef;
	m_pD3DContext->OMSetDepthStencilState( pDepthStencilState, StencilRef );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::SOSetTargets( 
	/* [annotation] */ 
	__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount_opt(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
	/* [annotation] */ 
	__in_ecount_opt(NumBuffers)  const UINT *pOffsets)
{
	m_pD3DContext->SOSetTargets( NumBuffers, ppSOTargets, pOffsets );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawAuto(void)
{
	m_pD3DContext->DrawAuto();
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawIndexedInstancedIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	m_pD3DContext->DrawIndexedInstancedIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DrawInstancedIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	m_pD3DContext->DrawInstancedIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::Dispatch( 
	/* [annotation] */ 
	__in  UINT ThreadGroupCountX,
	/* [annotation] */ 
	__in  UINT ThreadGroupCountY,
	/* [annotation] */ 
	__in  UINT ThreadGroupCountZ)
{
	m_pD3DContext->Dispatch( ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DispatchIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	m_pD3DContext->DispatchIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSSetState( 
	/* [annotation] */ 
	__in_opt  ID3D11RasterizerState *pRasterizerState)
{
	if( m_pCurrentRasterizerState == pRasterizerState ) {
		return;
	}
	m_pCurrentRasterizerState = pRasterizerState;
	m_pD3DContext->RSSetState( pRasterizerState );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSSetViewports( 
	/* [annotation] */ 
	__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
	/* [annotation] */ 
	__in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports)
{
	m_pD3DContext->RSSetViewports( NumViewports, pViewports );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSSetScissorRects( 
	/* [annotation] */ 
	__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
	/* [annotation] */ 
	__in_ecount_opt(NumRects)  const D3D11_RECT *pRects)
{
	m_pD3DContext->RSSetScissorRects( NumRects, pRects );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CopySubresourceRegion( 
	/* [annotation] */ 
	__in  ID3D11Resource *pDstResource,
	/* [annotation] */ 
	__in  UINT DstSubresource,
	/* [annotation] */ 
	__in  UINT DstX,
	/* [annotation] */ 
	__in  UINT DstY,
	/* [annotation] */ 
	__in  UINT DstZ,
	/* [annotation] */ 
	__in  ID3D11Resource *pSrcResource,
	/* [annotation] */ 
	__in  UINT SrcSubresource,
	/* [annotation] */ 
	__in_opt  const D3D11_BOX *pSrcBox)
{
	m_pD3DContext->CopySubresourceRegion(
		pDstResource, DstSubresource, DstX, DstY, DstZ,
		pSrcResource, SrcSubresource, pSrcBox );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CopyResource( 
	/* [annotation] */ 
	__in  ID3D11Resource *pDstResource,
	/* [annotation] */ 
	__in  ID3D11Resource *pSrcResource)
{
	m_pD3DContext->CopyResource( pDstResource, pSrcResource );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::UpdateSubresource( 
	/* [annotation] */ 
	__in  ID3D11Resource *pDstResource,
	/* [annotation] */ 
	__in  UINT DstSubresource,
	/* [annotation] */ 
	__in_opt  const D3D11_BOX *pDstBox,
	/* [annotation] */ 
	__in  const void *pSrcData,
	/* [annotation] */ 
	__in  UINT SrcRowPitch,
	/* [annotation] */ 
	__in  UINT SrcDepthPitch)
{
	m_pD3DContext->UpdateSubresource( pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CopyStructureCount( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pDstBuffer,
	/* [annotation] */ 
	__in  UINT DstAlignedByteOffset,
	/* [annotation] */ 
	__in  ID3D11UnorderedAccessView *pSrcView)
{
	m_pD3DContext->CopyStructureCount( pDstBuffer, DstAlignedByteOffset, pSrcView );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ClearRenderTargetView( 
	/* [annotation] */ 
	__in  ID3D11RenderTargetView *pRenderTargetView,
	/* [annotation] */ 
	__in  const FLOAT ColorRGBA[ 4 ])
{
	m_pD3DContext->ClearRenderTargetView( pRenderTargetView, ColorRGBA );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ClearUnorderedAccessViewUint( 
	/* [annotation] */ 
	__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
	/* [annotation] */ 
	__in  const UINT Values[ 4 ])
{
	m_pD3DContext->ClearUnorderedAccessViewUint( pUnorderedAccessView, Values );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ClearUnorderedAccessViewFloat( 
	/* [annotation] */ 
	__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
	/* [annotation] */ 
	__in  const FLOAT Values[ 4 ])
{
	m_pD3DContext->ClearUnorderedAccessViewFloat( pUnorderedAccessView, Values );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ClearDepthStencilView( 
	/* [annotation] */ 
	__in  ID3D11DepthStencilView *pDepthStencilView,
	/* [annotation] */ 
	__in  UINT ClearFlags,
	/* [annotation] */ 
	__in  FLOAT Depth,
	/* [annotation] */ 
	__in  UINT8 Stencil)
{
	m_pD3DContext->ClearDepthStencilView( pDepthStencilView, ClearFlags, Depth, Stencil );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GenerateMips( 
	/* [annotation] */ 
	__in  ID3D11ShaderResourceView *pShaderResourceView)
{
	m_pD3DContext->GenerateMips( pShaderResourceView );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::SetResourceMinLOD( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	FLOAT MinLOD)
{
	m_pD3DContext->SetResourceMinLOD( pResource, MinLOD );
}
//-------------------------------------------------------------------------------------------------------------//
FLOAT D3D11DeviceContext::GetResourceMinLOD( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource)
{
	return m_pD3DContext->GetResourceMinLOD( pResource );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ResolveSubresource( 
	/* [annotation] */ 
	__in  ID3D11Resource *pDstResource,
	/* [annotation] */ 
	__in  UINT DstSubresource,
	/* [annotation] */ 
	__in  ID3D11Resource *pSrcResource,
	/* [annotation] */ 
	__in  UINT SrcSubresource,
	/* [annotation] */ 
	__in  DXGI_FORMAT Format)
{
	m_pD3DContext->ResolveSubresource( pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ExecuteCommandList( 
	/* [annotation] */ 
	__in  ID3D11CommandList *pCommandList,
	BOOL RestoreContextState)
{
	m_pD3DContext->ExecuteCommandList( pCommandList, RestoreContextState );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSSetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->HSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11HullShader *pHullShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->HSSetShader( pHullShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSSetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->HSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->HSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSSetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->DSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11DomainShader *pDomainShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->DSSetShader( pDomainShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSSetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->DSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->DSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSSetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->CSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSSetUnorderedAccessViews( 
	/* [annotation] */ 
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
	/* [annotation] */ 
	__in_ecount(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
	/* [annotation] */ 
	__in_ecount(NumUAVs)  const UINT *pUAVInitialCounts)
{
	m_pD3DContext->CSSetUnorderedAccessViews( StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11ComputeShader *pComputeShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->CSSetShader( pComputeShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSSetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->CSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	m_pD3DContext->CSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSGetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->VSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSGetShaderResources( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	/* [annotation] */ 
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->PSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSGetShader( 
	/* [annotation] */ 
	__out  ID3D11PixelShader **ppPixelShader,
	/* [annotation] */ 
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	/* [annotation] */ 
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->PSGetShader( ppPixelShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSGetSamplers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->PSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSGetShader( 
	/* [annotation] */ 
	__out  ID3D11VertexShader **ppVertexShader,
	/* [annotation] */ 
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	/* [annotation] */ 
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->VSGetShader( ppVertexShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::PSGetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->PSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IAGetInputLayout( 
	/* [annotation] */ 
	__out  ID3D11InputLayout **ppInputLayout)
{
	m_pD3DContext->IAGetInputLayout( ppInputLayout );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IAGetVertexBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__out_ecount_opt(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
	/* [annotation] */ 
	__out_ecount_opt(NumBuffers)  UINT *pStrides,
	/* [annotation] */ 
	__out_ecount_opt(NumBuffers)  UINT *pOffsets)
{
	m_pD3DContext->IAGetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IAGetIndexBuffer( 
	/* [annotation] */ 
	__out_opt  ID3D11Buffer **pIndexBuffer,
	/* [annotation] */ 
	__out_opt  DXGI_FORMAT *Format,
	/* [annotation] */ 
	__out_opt  UINT *Offset)
{
	m_pD3DContext->IAGetIndexBuffer( pIndexBuffer, Format, Offset );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSGetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->GSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSGetShader( 
	/* [annotation] */ 
	__out  ID3D11GeometryShader **ppGeometryShader,
	/* [annotation] */ 
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	/* [annotation] */ 
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->GSGetShader( ppGeometryShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::IAGetPrimitiveTopology( 
	/* [annotation] */ 
	__out  D3D11_PRIMITIVE_TOPOLOGY *pTopology)
{
	m_pD3DContext->IAGetPrimitiveTopology( pTopology );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->VSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::VSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->VSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GetPredication( 
	__out_opt  ID3D11Predicate **ppPredicate,
	__out_opt  BOOL *pPredicateValue)
{
	m_pD3DContext->GetPredication( ppPredicate, pPredicateValue );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->GSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::GSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->GSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMGetRenderTargets( 
	__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
	__out_ecount_opt(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
	__out_opt  ID3D11DepthStencilView **ppDepthStencilView)
{
	m_pD3DContext->OMGetRenderTargets( NumViews, ppRenderTargetViews, ppDepthStencilView );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMGetRenderTargetsAndUnorderedAccessViews( 
	__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
	__out_ecount_opt(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
	__out_opt  ID3D11DepthStencilView **ppDepthStencilView,
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
	__out_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews)
{
	m_pD3DContext->OMGetRenderTargetsAndUnorderedAccessViews( NumRTVs, ppRenderTargetViews, ppDepthStencilView,
		UAVStartSlot, NumUAVs, ppUnorderedAccessViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMGetBlendState( 
	__out_opt  ID3D11BlendState **ppBlendState,
	__out_opt  FLOAT BlendFactor[ 4 ],
	__out_opt  UINT *pSampleMask)
{
	m_pD3DContext->OMGetBlendState( ppBlendState, BlendFactor, pSampleMask );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::OMGetDepthStencilState( 
	__out_opt  ID3D11DepthStencilState **ppDepthStencilState,
	__out_opt  UINT *pStencilRef)
{
	m_pD3DContext->OMGetDepthStencilState( ppDepthStencilState, pStencilRef );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::SOGetTargets( 
	__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets)
{
	m_pD3DContext->SOGetTargets( NumBuffers, ppSOTargets );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSGetState( 
	__out  ID3D11RasterizerState **ppRasterizerState)
{
	m_pD3DContext->RSGetState( ppRasterizerState );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSGetViewports( 
	__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
	__out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports)
{
	m_pD3DContext->RSGetViewports( pNumViewports, pViewports );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::RSGetScissorRects( 
	__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
	__out_ecount_opt(*pNumRects)  D3D11_RECT *pRects)
{
	m_pD3DContext->RSGetScissorRects( pNumRects, pRects );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->HSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSGetShader( 
	__out  ID3D11HullShader **ppHullShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->HSGetShader( ppHullShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->HSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::HSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->HSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->DSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSGetShader( 
	__out  ID3D11DomainShader **ppDomainShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->DSGetShader( ppDomainShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->DSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::DSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->DSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->CSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSGetUnorderedAccessViews( 
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
	__out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews)
{
	m_pD3DContext->CSGetUnorderedAccessViews( StartSlot, NumUAVs, ppUnorderedAccessViews );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSGetShader( 
	__out  ID3D11ComputeShader **ppComputeShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->CSGetShader( ppComputeShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->CSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::CSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->CSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::ClearState(void)
{
	this->ClearCachedState();

	//if( nil != m_pD3DContext )
	{
		m_pD3DContext->ClearState();
	}
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline void D3D11DeviceContext::Flush(void)
{
	//if( nil != m_pD3DContext )
	{
		m_pD3DContext->Flush();
	}
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline D3D11_DEVICE_CONTEXT_TYPE D3D11DeviceContext::GetType(void)
{
	return m_pD3DContext->GetType();
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline UINT D3D11DeviceContext::GetContextFlags(void)
{
	return m_pD3DContext->GetContextFlags();
}
//-------------------------------------------------------------------------------------------------------------//
ProxyDeviceContext_Inline HRESULT D3D11DeviceContext::FinishCommandList( 
	BOOL RestoreDeferredContextState,
	__out_opt  ID3D11CommandList **ppCommandList)
{
	return m_pD3DContext->FinishCommandList( RestoreDeferredContextState, ppCommandList );
}
//-------------------------------------------------------------------------------------------------------------//


#undef ProxyDeviceContext_Inline



#if RX_USE_PROXY_DEVICE_CONTEXT

	typedef D3D11DeviceContext	D3DDeviceContext;

#else

	typedef ID3D11DeviceContext	D3DDeviceContext;

#endif // RX_USE_PROXY_DEVICE_CONTEXT


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
