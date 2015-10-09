/*
=============================================================================
	File:	D3D11RunTime.cpp
	Desc:	
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

#include "DX11Private.h"

// valid only if RX_PROFILE is enabled
//
rxBackendStats	gfxBEStats;

/*
-----------------------------------------------------------------------------------------------------------------
	D3D11DebugDeviceContext
-----------------------------------------------------------------------------------------------------------------
*/
D3D11DebugDeviceContext::D3D11DebugDeviceContext( ID3D11DeviceContext* context )
{
	AssertPtr(context);
	m_pD3DContext = context;

	m_refCount = 0;

	this->ClearCachedState();
}
//-------------------------------------------------------------------------------------------------------------//
D3D11DebugDeviceContext::~D3D11DebugDeviceContext()
{
	Assert( m_refCount == 0 );
	Assert( m_pD3DContext == nil );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::ClearCachedState()
{
	m_pCurrentInputLayout = nil;

	m_pCurrentIndexBuffer = nil;
	m_nCurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_pCurrentRasterizerState = nil;
	m_pCurrentDepthStencilState = nil;
	m_nCurrentStencilValue = 0;
	m_pCurrentBlendState = nil;

	m_pCurrentVertexShader = nil;
	m_pCurrentHullShader = nil;
	m_pCurrentDomainShader = nil;
	m_pCurrentGeometryShader = nil;
	m_pCurrentPixelShader = nil;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DeleteThis()
{
	Assert( m_refCount == 0 );
	//MX_DEBUG_BREAK;

	this->ClearCachedState();

	m_pD3DContext = nil;

	// this object is allocated statically, so don't call 'delete this'
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT D3D11DebugDeviceContext::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return m_pD3DContext->QueryInterface( riid, ppvObject );
}
//-------------------------------------------------------------------------------------------------------------//
ULONG D3D11DebugDeviceContext::AddRef(void)
{
	const LONG numRefs = ::InterlockedIncrement( &m_refCount );
	//DBGOUT("D3D11DebugDeviceContext::AddRef(): %u refs\n",(UINT)numRefs);
	return (ULONG)numRefs;
}
//-------------------------------------------------------------------------------------------------------------//
ULONG D3D11DebugDeviceContext::Release(void)
{
	Assert( m_refCount > 0 );

	const ULONG numRefs = ::InterlockedDecrement( &m_refCount );

	//DBGOUT("D3D11DebugDeviceContext::Release(): %u refs\n",(UINT)numRefs);

	if( numRefs == 0 )
	{
		this->DeleteThis();
	}
	return numRefs;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::GetDevice( 
	/* [annotation] */ 
	__out  ID3D11Device **ppDevice)
{
	m_pD3DContext->GetDevice( ppDevice );
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT D3D11DebugDeviceContext::GetPrivateData( 
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
HRESULT D3D11DebugDeviceContext::SetPrivateData( 
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
HRESULT D3D11DebugDeviceContext::SetPrivateDataInterface( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__in_opt  const IUnknown *pData)
{
	return m_pD3DContext->SetPrivateDataInterface( guid, pData );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::VSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	gfxBEStats.numVSCBChanges++;
	m_pD3DContext->VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::PSSetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	m_pD3DContext->PSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::PSSetShader( 
	__in_opt  ID3D11PixelShader *pPixelShader,
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	if( m_pCurrentPixelShader == pPixelShader ) {
		return;
	}
	m_pCurrentPixelShader = pPixelShader;
	gfxBEStats.numPSChanges++;

	m_pD3DContext->PSSetShader( pPixelShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::PSSetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	/* [annotation] */ 
	__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers)
{
	m_pD3DContext->PSSetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::VSSetShader( 
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
	gfxBEStats.numVSChanges++;

	m_pD3DContext->VSSetShader( pVertexShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DrawIndexed( 
	/* [annotation] */ 
	__in  UINT IndexCount,
	/* [annotation] */ 
	__in  UINT StartIndexLocation,
	/* [annotation] */ 
	__in  INT BaseVertexLocation)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->DrawIndexed( IndexCount, StartIndexLocation, BaseVertexLocation );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::Draw( 
	/* [annotation] */ 
	__in  UINT VertexCount,
	/* [annotation] */ 
	__in  UINT StartVertexLocation)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->Draw( VertexCount, StartVertexLocation );
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT D3D11DebugDeviceContext::Map( 
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
	gfxBEStats.numMapCalls++;
	return m_pD3DContext->Map( pResource, Subresource, MapType, MapFlags, pMappedResource );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::Unmap( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in  UINT Subresource)
{
	m_pD3DContext->Unmap( pResource, Subresource );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::PSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	gfxBEStats.numPSCBChanges++;
	m_pD3DContext->PSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::IASetInputLayout( 
	/* [annotation] */ 
	__in_opt  ID3D11InputLayout *pInputLayout)
{
	if( m_pCurrentInputLayout == pInputLayout ) {
		return;
	}
	m_pCurrentInputLayout = pInputLayout;
	gfxBEStats.numInputLayoutChanges++;
	m_pD3DContext->IASetInputLayout( pInputLayout );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::IASetVertexBuffers( 
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
	gfxBEStats.numVBChanges++;
	m_pD3DContext->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::IASetIndexBuffer( 
	/* [annotation] */ 
	__in_opt  ID3D11Buffer *pIndexBuffer,
	/* [annotation] */ 
	__in  DXGI_FORMAT Format,
	/* [annotation] */ 
	__in  UINT Offset)
{
	if( m_pCurrentIndexBuffer == pIndexBuffer ) {
		return;
	}
	m_pCurrentIndexBuffer = pIndexBuffer;
	gfxBEStats.numIBChanges++;
	m_pD3DContext->IASetIndexBuffer( pIndexBuffer, Format, Offset );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DrawIndexedInstanced( 
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
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	return m_pD3DContext->DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DrawInstanced( 
	/* [annotation] */ 
	__in  UINT VertexCountPerInstance,
	/* [annotation] */ 
	__in  UINT InstanceCount,
	/* [annotation] */ 
	__in  UINT StartVertexLocation,
	/* [annotation] */ 
	__in  UINT StartInstanceLocation)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->DrawInstanced( VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::GSSetConstantBuffers( 
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	/* [annotation] */ 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	/* [annotation] */ 
	__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers)
{
	gfxBEStats.numGSCBChanges++;
	m_pD3DContext->GSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::GSSetShader( 
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
	gfxBEStats.numGSChanges++;
	m_pD3DContext->GSSetShader( pShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::IASetPrimitiveTopology( 
	/* [annotation] */ 
	__in  D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	if( m_nCurrentTopology == Topology ) {
		return;
	}
	m_nCurrentTopology = Topology;
	gfxBEStats.numPrimTopologyChanges++;
	m_pD3DContext->IASetPrimitiveTopology( Topology );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::VSSetShaderResources( 
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
void D3D11DebugDeviceContext::VSSetSamplers( 
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
void D3D11DebugDeviceContext::Begin( 
	/* [annotation] */ 
	__in  ID3D11Asynchronous *pAsync)
{
	m_pD3DContext->Begin( pAsync );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::End( 
/* [annotation] */ 
__in  ID3D11Asynchronous *pAsync)
{
	m_pD3DContext->End( pAsync );
}
//-------------------------------------------------------------------------------------------------------------//
HRESULT D3D11DebugDeviceContext::GetData( 
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
void D3D11DebugDeviceContext::SetPredication( 
	/* [annotation] */ 
	__in_opt  ID3D11Predicate *pPredicate,
	/* [annotation] */ 
	__in  BOOL PredicateValue)
{
	m_pD3DContext->SetPredication( pPredicate, PredicateValue );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::GSSetShaderResources( 
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
void D3D11DebugDeviceContext::GSSetSamplers( 
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
void D3D11DebugDeviceContext::OMSetRenderTargets( 
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
void D3D11DebugDeviceContext::OMSetRenderTargetsAndUnorderedAccessViews( 
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
void D3D11DebugDeviceContext::OMSetBlendState( 
	/* [annotation] */ 
	__in_opt  ID3D11BlendState *pBlendState,
	/* [annotation] */ 
	__in_opt  const FLOAT BlendFactor[ 4 ],
	/* [annotation] */ 
	__in  UINT SampleMask)
{
	if( m_pCurrentBlendState == pBlendState ) {
		return;
	}
	m_pCurrentBlendState = pBlendState;
	gfxBEStats.numBSStateChanges++;
	m_pD3DContext->OMSetBlendState( pBlendState, BlendFactor, SampleMask );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::OMSetDepthStencilState( 
	/* [annotation] */ 
	__in_opt  ID3D11DepthStencilState *pDepthStencilState,
	/* [annotation] */ 
	__in  UINT StencilRef)
{
	if( m_pCurrentDepthStencilState == pDepthStencilState && m_nCurrentStencilValue == StencilRef ) {
		return;
	}
	m_pCurrentDepthStencilState = pDepthStencilState;
	m_nCurrentStencilValue = StencilRef;
	gfxBEStats.numDSStateChanges++;
	m_pD3DContext->OMSetDepthStencilState( pDepthStencilState, StencilRef );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::SOSetTargets( 
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
void D3D11DebugDeviceContext::DrawAuto(void)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->DrawAuto();
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DrawIndexedInstancedIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->DrawIndexedInstancedIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::DrawInstancedIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	Check_CanDrawNow();
	gfxBEStats.numDrawCalls++;
	m_pD3DContext->DrawInstancedIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::Dispatch( 
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
void D3D11DebugDeviceContext::DispatchIndirect( 
	/* [annotation] */ 
	__in  ID3D11Buffer *pBufferForArgs,
	/* [annotation] */ 
	__in  UINT AlignedByteOffsetForArgs)
{
	m_pD3DContext->DispatchIndirect( pBufferForArgs, AlignedByteOffsetForArgs );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::RSSetState( 
	/* [annotation] */ 
	__in_opt  ID3D11RasterizerState *pRasterizerState)
{
	if( m_pCurrentRasterizerState == pRasterizerState ) {
		return;
	}
	m_pCurrentRasterizerState = pRasterizerState;
	gfxBEStats.numRSStateChanges++;
	m_pD3DContext->RSSetState( pRasterizerState );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::RSSetViewports( 
	/* [annotation] */ 
	__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
	/* [annotation] */ 
	__in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports)
{
	m_pD3DContext->RSSetViewports( NumViewports, pViewports );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::RSSetScissorRects( 
	/* [annotation] */ 
	__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
	/* [annotation] */ 
	__in_ecount_opt(NumRects)  const D3D11_RECT *pRects)
{
	m_pD3DContext->RSSetScissorRects( NumRects, pRects );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::CopySubresourceRegion( 
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

void D3D11DebugDeviceContext::CopyResource( 
	/* [annotation] */ 
	__in  ID3D11Resource *pDstResource,
	/* [annotation] */ 
	__in  ID3D11Resource *pSrcResource)
{
	m_pD3DContext->CopyResource( pDstResource, pSrcResource );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::UpdateSubresource( 
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

void D3D11DebugDeviceContext::CopyStructureCount( 
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

void D3D11DebugDeviceContext::ClearRenderTargetView( 
	/* [annotation] */ 
	__in  ID3D11RenderTargetView *pRenderTargetView,
	/* [annotation] */ 
	__in  const FLOAT ColorRGBA[ 4 ])
{
	m_pD3DContext->ClearRenderTargetView( pRenderTargetView, ColorRGBA );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::ClearUnorderedAccessViewUint( 
	/* [annotation] */ 
	__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
	/* [annotation] */ 
	__in  const UINT Values[ 4 ])
{
	m_pD3DContext->ClearUnorderedAccessViewUint( pUnorderedAccessView, Values );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::ClearUnorderedAccessViewFloat( 
	/* [annotation] */ 
	__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
	/* [annotation] */ 
	__in  const FLOAT Values[ 4 ])
{
	m_pD3DContext->ClearUnorderedAccessViewFloat( pUnorderedAccessView, Values );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::ClearDepthStencilView( 
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

void D3D11DebugDeviceContext::GenerateMips( 
	/* [annotation] */ 
	__in  ID3D11ShaderResourceView *pShaderResourceView)
{
	m_pD3DContext->GenerateMips( pShaderResourceView );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::SetResourceMinLOD( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	FLOAT MinLOD)
{
	m_pD3DContext->SetResourceMinLOD( pResource, MinLOD );
}
//-------------------------------------------------------------------------------------------------------------//

FLOAT D3D11DebugDeviceContext::GetResourceMinLOD( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource)
{
	return m_pD3DContext->GetResourceMinLOD( pResource );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::ResolveSubresource( 
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

void D3D11DebugDeviceContext::ExecuteCommandList( 
	/* [annotation] */ 
	__in  ID3D11CommandList *pCommandList,
	BOOL RestoreContextState)
{
	m_pD3DContext->ExecuteCommandList( pCommandList, RestoreContextState );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSSetShaderResources( 
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

void D3D11DebugDeviceContext::HSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11HullShader *pHullShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->HSSetShader( pHullShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSSetSamplers( 
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

void D3D11DebugDeviceContext::HSSetConstantBuffers( 
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

void D3D11DebugDeviceContext::DSSetShaderResources( 
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

void D3D11DebugDeviceContext::DSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11DomainShader *pDomainShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->DSSetShader( pDomainShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::DSSetSamplers( 
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

void D3D11DebugDeviceContext::DSSetConstantBuffers( 
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

void D3D11DebugDeviceContext::CSSetShaderResources( 
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

void D3D11DebugDeviceContext::CSSetUnorderedAccessViews( 
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

void D3D11DebugDeviceContext::CSSetShader( 
	/* [annotation] */ 
	__in_opt  ID3D11ComputeShader *pComputeShader,
	/* [annotation] */ 
	__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
	UINT NumClassInstances)
{
	m_pD3DContext->CSSetShader( pComputeShader, ppClassInstances, NumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSSetSamplers( 
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

void D3D11DebugDeviceContext::CSSetConstantBuffers( 
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

void D3D11DebugDeviceContext::VSGetConstantBuffers( 
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

void D3D11DebugDeviceContext::PSGetShaderResources( 
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

void D3D11DebugDeviceContext::PSGetShader( 
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

void D3D11DebugDeviceContext::PSGetSamplers( 
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

void D3D11DebugDeviceContext::VSGetShader( 
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

void D3D11DebugDeviceContext::PSGetConstantBuffers( 
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

void D3D11DebugDeviceContext::IAGetInputLayout( 
	/* [annotation] */ 
	__out  ID3D11InputLayout **ppInputLayout)
{
	m_pD3DContext->IAGetInputLayout( ppInputLayout );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::IAGetVertexBuffers( 
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

void D3D11DebugDeviceContext::IAGetIndexBuffer( 
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

void D3D11DebugDeviceContext::GSGetConstantBuffers( 
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

void D3D11DebugDeviceContext::GSGetShader( 
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

void D3D11DebugDeviceContext::IAGetPrimitiveTopology( 
	/* [annotation] */ 
	__out  D3D11_PRIMITIVE_TOPOLOGY *pTopology)
{
	m_pD3DContext->IAGetPrimitiveTopology( pTopology );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::VSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->VSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::VSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->VSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::GetPredication( 
	__out_opt  ID3D11Predicate **ppPredicate,
	__out_opt  BOOL *pPredicateValue)
{
	m_pD3DContext->GetPredication( ppPredicate, pPredicateValue );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::GSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->GSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::GSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->GSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::OMGetRenderTargets( 
	__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
	__out_ecount_opt(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
	__out_opt  ID3D11DepthStencilView **ppDepthStencilView)
{
	m_pD3DContext->OMGetRenderTargets( NumViews, ppRenderTargetViews, ppDepthStencilView );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::OMGetRenderTargetsAndUnorderedAccessViews( 
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

void D3D11DebugDeviceContext::OMGetBlendState( 
	__out_opt  ID3D11BlendState **ppBlendState,
	__out_opt  FLOAT BlendFactor[ 4 ],
	__out_opt  UINT *pSampleMask)
{
	m_pD3DContext->OMGetBlendState( ppBlendState, BlendFactor, pSampleMask );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::OMGetDepthStencilState( 
	__out_opt  ID3D11DepthStencilState **ppDepthStencilState,
	__out_opt  UINT *pStencilRef)
{
	m_pD3DContext->OMGetDepthStencilState( ppDepthStencilState, pStencilRef );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::SOGetTargets( 
	__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets)
{
	m_pD3DContext->SOGetTargets( NumBuffers, ppSOTargets );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::RSGetState( 
	__out  ID3D11RasterizerState **ppRasterizerState)
{
	m_pD3DContext->RSGetState( ppRasterizerState );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::RSGetViewports( 
	__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
	__out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports)
{
	m_pD3DContext->RSGetViewports( pNumViewports, pViewports );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::RSGetScissorRects( 
	__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
	__out_ecount_opt(*pNumRects)  D3D11_RECT *pRects)
{
	m_pD3DContext->RSGetScissorRects( pNumRects, pRects );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->HSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSGetShader( 
	__out  ID3D11HullShader **ppHullShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->HSGetShader( ppHullShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->HSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::HSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->HSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::DSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->DSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::DSGetShader( 
	__out  ID3D11DomainShader **ppDomainShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->DSGetShader( ppDomainShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::DSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->DSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::DSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->DSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSGetShaderResources( 
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
	__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews)
{
	m_pD3DContext->CSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSGetUnorderedAccessViews( 
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
	__out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews)
{
	m_pD3DContext->CSGetUnorderedAccessViews( StartSlot, NumUAVs, ppUnorderedAccessViews );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSGetShader( 
	__out  ID3D11ComputeShader **ppComputeShader,
	__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
	__inout_opt  UINT *pNumClassInstances)
{
	m_pD3DContext->CSGetShader( ppComputeShader, ppClassInstances, pNumClassInstances );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSGetSamplers( 
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
	__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers)
{
	m_pD3DContext->CSGetSamplers( StartSlot, NumSamplers, ppSamplers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::CSGetConstantBuffers( 
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
	__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
	__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers)
{
	m_pD3DContext->CSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::ClearState(void)
{
	m_pCurrentInputLayout = nil;

	m_pCurrentIndexBuffer = nil;
	m_nCurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_pCurrentRasterizerState = nil;
	m_pCurrentDepthStencilState = nil;
	m_pCurrentBlendState = nil;

	m_pCurrentVertexShader = nil;
	m_pCurrentHullShader = nil;
	m_pCurrentDomainShader = nil;
	m_pCurrentGeometryShader = nil;
	m_pCurrentPixelShader = nil;

	//if( nil != m_pD3DContext )
	{
		m_pD3DContext->ClearState();
	}
}
//-------------------------------------------------------------------------------------------------------------//

void D3D11DebugDeviceContext::Flush(void)
{
	//if( nil != m_pD3DContext )
	{
		m_pD3DContext->Flush();
	}
}
//-------------------------------------------------------------------------------------------------------------//

D3D11_DEVICE_CONTEXT_TYPE D3D11DebugDeviceContext::GetType(void)
{
	return m_pD3DContext->GetType();
}
//-------------------------------------------------------------------------------------------------------------//

UINT D3D11DebugDeviceContext::GetContextFlags(void)
{
	return m_pD3DContext->GetContextFlags();
}
//-------------------------------------------------------------------------------------------------------------//

HRESULT D3D11DebugDeviceContext::FinishCommandList( 
	BOOL RestoreDeferredContextState,
	__out_opt  ID3D11CommandList **ppCommandList)
{
	return m_pD3DContext->FinishCommandList( RestoreDeferredContextState, ppCommandList );
}
//-------------------------------------------------------------------------------------------------------------//
void D3D11DebugDeviceContext::Check_CanDrawNow() const
{
	Assert( m_nCurrentTopology != D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED );

	Assert( m_pCurrentRasterizerState != nil );
	Assert( m_pCurrentDepthStencilState != nil );
	//Assert( m_pCurrentBlendState != nil );	// blend state can be null which is the default blend state

	Assert( m_pCurrentVertexShader != nil );
	//Assert( m_pCurrentPixelShader != nil );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
