/*
=======================================================================
	File:	DX11Private.h
	Desc:	DirectX 11 graphics system, private header file.
	Todo:	No OOP crap, good old plain C-like functional interface.
			Rationale: functions can be called from scripts
			which allows for rapid prototyping and experimentation.
=======================================================================
*/

#pragma once

// for shader stuff
#include <Base/Templates/Containers/HashMap/TMap.h>
#include <Base/Text/Parse_Util.h>	// for CompileMessage

//--------------------------------------------------------------------------------------
//  Logging and error reporting
//--------------------------------------------------------------------------------------

void dxDbgOut( const char* message, ... );

// used to display DirectX messages (e.g. from ID3D11Blob's)
void dxMsg( const char* message );
void dxMsgf( const char* message, ... );

void dxWarnf( const char* message, ... );
void dxWarnf( HRESULT errorCode, const char* message, ... );

void dxErrf( const char* message, ... );
void dxErrf( HRESULT errorCode, const char* message, ... );

//------------------------------------------------------------------------
//	Debugging and run-time checks - they slow down a lot!
//------------------------------------------------------------------------

// Converts an HRESULT to a meaning full message
#define DX_LAST_ERR_STR		::DXGetErrorStringA(::GetLastError())

//--------------------------------------------------------------------------------------
//  Debug text rendering
//--------------------------------------------------------------------------------------

void DbgFont_Setup();
void DbgFont_Close();

// must be called after rendering the main scene
void DbgFont_RenderTexts();




//------------------------------------------------------------------------
//	Declarations
//------------------------------------------------------------------------






//------------------------------------------------------------------------
//	Functions
//------------------------------------------------------------------------



typedef ShaderId F_MapShaderNameToId(
	PCSTR
);

// maps shader indices to their names and vice versa
struct ShaderNameMap
{
	virtual ~ShaderNameMap() {}

	// total number of shader programs
	virtual UINT TotalNumShaders() = 0;

	virtual UINT ShaderNameToIndex( PCSTR str ) = 0;

	virtual PCSTR ShaderIndexToName( UINT index ) = 0;
};

/*
=======================================================================

	GPU programs

=======================================================================
*/

/*
--------------------------------------------------------------
	MessageListener
	used for reporting messages when compiling shaders
--------------------------------------------------------------
*/
class MessageListener
{
public:
	virtual void Put( const char* message );

	static MessageListener	defaultInstance;
};

// transform the shader source code text into bytecode
// returns a null pointer in case of failure
//
ID3DBlob* D3D_CompileShader(
	const char* sourceCode,
	SizeT sourceCodeLength,
	const char* profile,
	const char* entryPoint,
	const D3D_SHADER_MACRO* defines = nil,
	ID3DInclude* pInclude = nil,
	MessageListener& msgFunc = MessageListener::defaultInstance
);

ID3DBlob* D3D_DisassembleShader(
	const void* compiledByteCode,
	SizeT byteCodeLength,
	bool bHTML = false,	// plain text or HTML format?
	const char* comments = nil
);

// creates a vertex shader from compiled shader bytecode
//
ID3D11VertexShader* D3D_CreateVertexShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage = nil
);

// creates a geometry shader from compiled shader bytecode
//
ID3D11GeometryShader* D3D_CreateGeometryShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage = nil
);

// creates a pixel shader from compiled shader bytecode
//
ID3D11PixelShader* D3D_CreatePixelShader(
	const void* compiledBytecode,
	SizeT bytecodeLength,
	ID3D11ClassLinkage* linkage = nil
);


/*
=======================================================================

	Input layouts

=======================================================================
*/

//
//	dxVertexFormat
//
struct dxVertexFormat
{
	typedef TStaticList
	<
		D3D11_INPUT_ELEMENT_DESC, RX_MAX_VERTEX_ELEMENTS
	>
	ElementList;

	ElementList		elements;

public:
	dxVertexFormat();	// initializes with default values
	explicit dxVertexFormat(ENoInit);
	explicit dxVertexFormat(EInitZero);

	bool Equals( const dxVertexFormat& other ) const;
	UINT GetHash()const;

	UINT CalcSize() const;

	FORCEINLINE bool operator == ( const dxVertexFormat& other ) const
	{
		return this->Equals( other );
	}

	void Clear();

private:
	// this function is never called, it only contains compile-time assertions
	static void Checks()
	{ 
		{mxSTATIC_ASSERT( RX_MAX_INPUT_SLOTS <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT );}
		{mxSTATIC_ASSERT( RX_MAX_VERTEX_ELEMENTS <= D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT );}
	}
};

// generates a dummy vertex shader for creating a new input layout
//
ID3D11InputLayout* D3D_CreateInputLayout(
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	UINT NumElements
);



/*
=======================================================================

	Buffer resources

=======================================================================
*/

// Buffers.

ID3D11Buffer* D3D_CreateVertexBuffer(
	UINT size,
	UINT stride,
	const void* data = nil,
	bool dynamic = false
);
ID3D11Buffer* D3D_CreateIndexBuffer(
	UINT size,
	UINT stride,
	const void* data = nil,
	bool dynamic = false
);

/*
=======================================================================

	GPU constant buffers

=======================================================================
*/

// creates a constant buffer which can only be updated as a whole via UpdateSubresource() and cannot be mapped
ID3D11Buffer* D3D_CreateConstantBuffer(
	UINT size,	// Size of the constant buffer, in bytes. Must be aligned on 16-byte boundary
	const void* data = nil	// Pointer to initial data.
);
// creates a constant buffer which can be updated partially via Map()/Unmap()
ID3D11Buffer* D3D_CreateMappableConstantBuffer(
	UINT size,	// Size of the constant buffer, in bytes. Must be aligned on 16-byte boundary
	const void* data = nil	// Pointer to initial data.
);

#if 0
//
//	TypedConstantBuffer - is a typed constant buffer and it can only be updated as a whole.
//	(i.e. it cannot be mapped, but can only be updated using a copy operation.)
//
//	NOTE: it keeps a shadow copy in system memory.
//
//	NOTE: Be careful NOT to use any class with a virtual function table,
//	or appropriately pad your shader side constant buffer...
//
mxTODO("Align by 16?");
mxREFACTOR("TShadowConstantBuffer");
template< typename STRUCT >
class TypedConstantBuffer : public STRUCT
{
public:
	TypedConstantBuffer()
		: mD3DContext( nil )
	{}
	~TypedConstantBuffer()
	{}

	void Create( ID3D11DeviceContext* d3dContext )
	{
		Assert(d3dContext);
		mD3DBuffer = D3D_CreateConstantBuffer( sizeof(STRUCT) );
		mD3DContext = d3dContext;
	}
	void Destroy()
	{
		mD3DBuffer = nil;
	}

	FORCEINLINE void Update()
	{
		mD3DContext->UpdateSubresource(
			mD3DBuffer,	// ID3D11Resource *pDstResource
			0,			// UINT DstSubresource
			nil,		// const D3D11_BOX *pDstBox
			this,		// const void *pSrcData
			0,			// UINT SrcRowPitch
			0			// UINT SrcDepthPitch
		);
	}

	// 'slot' must be in range [0..D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) !

	FORCEINLINE void BindTo_VS( UINT slot )
	{
		mD3DContext->VSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_GS( UINT slot )
	{
		mD3DContext->GSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS( UINT slot )
	{
		mD3DContext->PSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS_VS( UINT slot )
	{
		BindTo_VS( slot );
		BindTo_PS( slot );
	}

private:
	dxPtr< ID3D11Buffer >	mD3DBuffer;
	ID3D11DeviceContext *	mD3DContext;
};

//
//	UntypedConstantBuffer - is an untyped constant buffer that can be mapped (and partially updated).
//
class UntypedConstantBuffer
{
public:
	UntypedConstantBuffer()
		: mD3DContext( nil )
	{}
	~UntypedConstantBuffer()
	{}

	void Create( UINT sizeInBytes, ID3D11DeviceContext* d3dContext )
	{
		AssertPtr(d3dContext);
		mSize = ALIGN16( sizeInBytes );
		mD3DBuffer = D3D_CreateMappableConstantBuffer( mSize );
		mD3DContext = d3dContext;
	}
	void Destroy()
	{
		mD3DBuffer = nil;
		mD3DContext = nil;
		mSize = 0;
	}

	INLINE void* Lock( UINT size )
	{
		Assert(size <= mSize);
		return D3D_LockBuffer( mD3DContext, mD3DBuffer );
	}

	INLINE void Unlock()
	{
		D3D_UnlockBuffer( mD3DContext, mD3DBuffer );
	}

	template< typename TYPE >
	INLINE TYPE* Lock()
	{
		return (TYPE*) Lock( sizeof(TYPE) );
	}

	template< typename TYPE >
	FORCEINLINE void Update( const TYPE& srcData )
	{
		void* pData = Lock( sizeof(TYPE) );
		MemCopy( pData, &srcData, sizeof(TYPE) );
		Unlock();
	}

	// 'slot' must be in range [0..D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) !

	FORCEINLINE void BindTo_VS( UINT slot )
	{
		mD3DContext->VSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_GS( UINT slot )
	{
		mD3DContext->GSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS( UINT slot )
	{
		mD3DContext->PSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS_VS( UINT slot )
	{
		BindTo_VS( slot );
		BindTo_PS( slot );
	}

private:
	dxPtr< ID3D11Buffer >	mD3DBuffer;
	ID3D11DeviceContext *	mD3DContext;
	UINT					mSize;	// size of this buffer, in bytes
};

#endif

mxSWIPED("Intel deferred shading demo, 2010");
// NOTE: Ensure that T is exactly the same size/layout as the shader structure!
template <typename T>
class StructuredBuffer
{
public:
    // Construct a structured buffer
    StructuredBuffer(ID3D11Device* d3dDevice, int elements,
                     UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
                     bool dynamic = false);
    
    ~StructuredBuffer();

    ID3D11Buffer* GetBuffer() { return mBuffer; }
    ID3D11UnorderedAccessView* GetUnorderedAccess() { return mUnorderedAccess; }
    ID3D11ShaderResourceView* GetShaderResource() { return mShaderResource; }

    // Only valid for dynamic buffers
    // TODO: Support NOOVERWRITE ring buffer?
    T* MapDiscard(ID3D11DeviceContext* d3dDeviceContext);
    void Unmap(ID3D11DeviceContext* d3dDeviceContext);

private:
    // Not implemented
    StructuredBuffer(const StructuredBuffer&);
    StructuredBuffer& operator=(const StructuredBuffer&);

    int mElements;
    ID3D11Buffer* mBuffer;
    ID3D11ShaderResourceView* mShaderResource;
    ID3D11UnorderedAccessView* mUnorderedAccess;
};

template <typename T>
StructuredBuffer<T>::StructuredBuffer(ID3D11Device* d3dDevice, int elements,
                                      UINT bindFlags, bool dynamic)
    : mElements(elements), mShaderResource(0), mUnorderedAccess(0)
{
    CD3D11_BUFFER_DESC desc(sizeof(T) * elements, bindFlags,
        dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
        dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
        D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
        sizeof(T));

	d3dDevice->CreateBuffer(&desc, 0, &mBuffer);

    if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
        d3dDevice->CreateUnorderedAccessView(mBuffer, 0, &mUnorderedAccess);
    }

    if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
        d3dDevice->CreateShaderResourceView(mBuffer, 0, &mShaderResource);
    }
}

template <typename T>
StructuredBuffer<T>::~StructuredBuffer()
{
    if (mUnorderedAccess) mUnorderedAccess->Release();
    if (mShaderResource) mShaderResource->Release();
    mBuffer->Release();
}

template <typename T>
T* StructuredBuffer<T>::MapDiscard(ID3D11DeviceContext* d3dDeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    d3dDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    return static_cast<T*>(mappedResource.pData);
}

template <typename T>
void StructuredBuffer<T>::Unmap(ID3D11DeviceContext* d3dDeviceContext)
{
    d3dDeviceContext->Unmap(mBuffer, 0);
}

/*
=======================================================================

	Texture resources

=======================================================================
*/

ID3D11Texture2D* D3D_CreateTexture2D(
	const D3D11_TEXTURE2D_DESC& desc,
	const D3D11_SUBRESOURCE_DATA* initialData = nil
);


ID3D11ShaderResourceView* D3D_LoadResourceFromFile(
	const char* fileName
);
ID3D11ShaderResourceView* D3D_LoadResourceFromMemory(
	const void* data, const SizeT size
);



FORCEINLINE
void UnbindRenderTargetsAndDepthStencil( ID3D11DeviceContext* context )
{
	// Cleanup (aka make the runtime happy)
	context->OMSetRenderTargets(
		0,	//NumViews
		nil,//ppRenderTargetViews
		nil	//pDepthStencilView
	);
}


/*
=======================================================================

	Rendering

=======================================================================
*/

void D3D_DefaultSwapChainDesc( WindowHandle	hWnd, DXGI_SWAP_CHAIN_DESC &desc );

bool D3D_InitializeViewport( DXGI_SWAP_CHAIN_DESC& desc,
							const bool bDepthStencil,
							GrViewport &viewport );

bool D3D_ResizeViewport( GrViewport &viewport, UINT newWidth, UINT newHeight, bool bFullScreen );

/*
-----------------------------------------------------------------------------
	D3D11DebugDeviceContext

	proxy device context,
	used for catching programming errors
	and for profiling and gathering statistics
-----------------------------------------------------------------------------
*/
class D3D11DebugDeviceContext : public ID3D11DeviceContext
{
public:
	D3D11DebugDeviceContext( ID3D11DeviceContext* context );
	~D3D11DebugDeviceContext();

	// IUnknown interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);

	virtual ULONG STDMETHODCALLTYPE Release( void);


	// ID3D11DeviceChild interface

	virtual void STDMETHODCALLTYPE GetDevice( 
		/* [annotation] */ 
		__out  ID3D11Device **ppDevice);

	virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__inout  UINT *pDataSize,
		/* [annotation] */ 
		__out_bcount_opt( *pDataSize )  void *pData);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in  UINT DataSize,
		/* [annotation] */ 
		__in_bcount_opt( DataSize )  const void *pData);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in_opt  const IUnknown *pData);


	// ID3D11DeviceContext interface

	virtual void STDMETHODCALLTYPE VSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE PSSetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE PSSetShader( 
		__in_opt  ID3D11PixelShader *pPixelShader,
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE PSSetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE VSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11VertexShader *pVertexShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE DrawIndexed( 
		/* [annotation] */ 
		__in  UINT IndexCount,
		/* [annotation] */ 
		__in  UINT StartIndexLocation,
		/* [annotation] */ 
		__in  INT BaseVertexLocation);

	virtual void STDMETHODCALLTYPE Draw( 
		/* [annotation] */ 
		__in  UINT VertexCount,
		/* [annotation] */ 
		__in  UINT StartVertexLocation);

	virtual HRESULT STDMETHODCALLTYPE Map( 
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

	virtual void STDMETHODCALLTYPE Unmap( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in  UINT Subresource);

	virtual void STDMETHODCALLTYPE PSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE IASetInputLayout( 
		/* [annotation] */ 
		__in_opt  ID3D11InputLayout *pInputLayout);

	virtual void STDMETHODCALLTYPE IASetVertexBuffers( 
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

	virtual void STDMETHODCALLTYPE IASetIndexBuffer( 
		/* [annotation] */ 
		__in_opt  ID3D11Buffer *pIndexBuffer,
		/* [annotation] */ 
		__in  DXGI_FORMAT Format,
		/* [annotation] */ 
		__in  UINT Offset);

	virtual void STDMETHODCALLTYPE DrawIndexedInstanced( 
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

	virtual void STDMETHODCALLTYPE DrawInstanced( 
		/* [annotation] */ 
		__in  UINT VertexCountPerInstance,
		/* [annotation] */ 
		__in  UINT InstanceCount,
		/* [annotation] */ 
		__in  UINT StartVertexLocation,
		/* [annotation] */ 
		__in  UINT StartInstanceLocation);

	virtual void STDMETHODCALLTYPE GSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE GSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11GeometryShader *pShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE IASetPrimitiveTopology( 
		/* [annotation] */ 
		__in  D3D11_PRIMITIVE_TOPOLOGY Topology);

	virtual void STDMETHODCALLTYPE VSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE VSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE Begin( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync);

	virtual void STDMETHODCALLTYPE End( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync);

	virtual HRESULT STDMETHODCALLTYPE GetData( 
		/* [annotation] */ 
		__in  ID3D11Asynchronous *pAsync,
		/* [annotation] */ 
		__out_bcount_opt( DataSize )  void *pData,
		/* [annotation] */ 
		__in  UINT DataSize,
		/* [annotation] */ 
		__in  UINT GetDataFlags);

	virtual void STDMETHODCALLTYPE SetPredication( 
		/* [annotation] */ 
		__in_opt  ID3D11Predicate *pPredicate,
		/* [annotation] */ 
		__in  BOOL PredicateValue);

	virtual void STDMETHODCALLTYPE GSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE GSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE OMSetRenderTargets( 
		/* [annotation] */ 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
		/* [annotation] */ 
		__in_opt  ID3D11DepthStencilView *pDepthStencilView);

	virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews( 
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

	virtual void STDMETHODCALLTYPE OMSetBlendState( 
		/* [annotation] */ 
		__in_opt  ID3D11BlendState *pBlendState,
		/* [annotation] */ 
		__in_opt  const FLOAT BlendFactor[ 4 ],
		/* [annotation] */ 
		__in  UINT SampleMask);

	virtual void STDMETHODCALLTYPE OMSetDepthStencilState( 
		/* [annotation] */ 
		__in_opt  ID3D11DepthStencilState *pDepthStencilState,
		/* [annotation] */ 
		__in  UINT StencilRef);

	virtual void STDMETHODCALLTYPE SOSetTargets( 
		/* [annotation] */ 
		__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount_opt(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
		/* [annotation] */ 
		__in_ecount_opt(NumBuffers)  const UINT *pOffsets);

	virtual void STDMETHODCALLTYPE DrawAuto( void);

	virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual void STDMETHODCALLTYPE DrawInstancedIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual void STDMETHODCALLTYPE Dispatch( 
		/* [annotation] */ 
		__in  UINT ThreadGroupCountX,
		/* [annotation] */ 
		__in  UINT ThreadGroupCountY,
		/* [annotation] */ 
		__in  UINT ThreadGroupCountZ);

	virtual void STDMETHODCALLTYPE DispatchIndirect( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pBufferForArgs,
		/* [annotation] */ 
		__in  UINT AlignedByteOffsetForArgs);

	virtual void STDMETHODCALLTYPE RSSetState( 
		/* [annotation] */ 
		__in_opt  ID3D11RasterizerState *pRasterizerState);

	virtual void STDMETHODCALLTYPE RSSetViewports( 
		/* [annotation] */ 
		__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
		/* [annotation] */ 
		__in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports);

	virtual void STDMETHODCALLTYPE RSSetScissorRects( 
		/* [annotation] */ 
		__in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
		/* [annotation] */ 
		__in_ecount_opt(NumRects)  const D3D11_RECT *pRects);

	virtual void STDMETHODCALLTYPE CopySubresourceRegion( 
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

	virtual void STDMETHODCALLTYPE CopyResource( 
		/* [annotation] */ 
		__in  ID3D11Resource *pDstResource,
		/* [annotation] */ 
		__in  ID3D11Resource *pSrcResource);

	virtual void STDMETHODCALLTYPE UpdateSubresource( 
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

	virtual void STDMETHODCALLTYPE CopyStructureCount( 
		/* [annotation] */ 
		__in  ID3D11Buffer *pDstBuffer,
		/* [annotation] */ 
		__in  UINT DstAlignedByteOffset,
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pSrcView);

	virtual void STDMETHODCALLTYPE ClearRenderTargetView( 
		/* [annotation] */ 
		__in  ID3D11RenderTargetView *pRenderTargetView,
		/* [annotation] */ 
		__in  const FLOAT ColorRGBA[ 4 ]);

	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( 
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
		/* [annotation] */ 
		__in  const UINT Values[ 4 ]);

	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( 
		/* [annotation] */ 
		__in  ID3D11UnorderedAccessView *pUnorderedAccessView,
		/* [annotation] */ 
		__in  const FLOAT Values[ 4 ]);

	virtual void STDMETHODCALLTYPE ClearDepthStencilView( 
		/* [annotation] */ 
		__in  ID3D11DepthStencilView *pDepthStencilView,
		/* [annotation] */ 
		__in  UINT ClearFlags,
		/* [annotation] */ 
		__in  FLOAT Depth,
		/* [annotation] */ 
		__in  UINT8 Stencil);

	virtual void STDMETHODCALLTYPE GenerateMips( 
		/* [annotation] */ 
		__in  ID3D11ShaderResourceView *pShaderResourceView);

	virtual void STDMETHODCALLTYPE SetResourceMinLOD( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		FLOAT MinLOD);

	virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource);

	virtual void STDMETHODCALLTYPE ResolveSubresource( 
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

	virtual void STDMETHODCALLTYPE ExecuteCommandList( 
		/* [annotation] */ 
		__in  ID3D11CommandList *pCommandList,
		BOOL RestoreContextState);

	virtual void STDMETHODCALLTYPE HSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE HSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11HullShader *pHullShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE HSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE HSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE DSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE DSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11DomainShader *pDomainShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE DSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE DSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE CSSetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews( 
		/* [annotation] */ 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
		/* [annotation] */ 
		__in_ecount(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
		/* [annotation] */ 
		__in_ecount(NumUAVs)  const UINT *pUAVInitialCounts);

	virtual void STDMETHODCALLTYPE CSSetShader( 
		/* [annotation] */ 
		__in_opt  ID3D11ComputeShader *pComputeShader,
		/* [annotation] */ 
		__in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	virtual void STDMETHODCALLTYPE CSSetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

	virtual void STDMETHODCALLTYPE CSSetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

	virtual void STDMETHODCALLTYPE VSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE PSGetShaderResources( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		/* [annotation] */ 
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE PSGetShader( 
		/* [annotation] */ 
		__out  ID3D11PixelShader **ppPixelShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE PSGetSamplers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		/* [annotation] */ 
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE VSGetShader( 
		/* [annotation] */ 
		__out  ID3D11VertexShader **ppVertexShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE PSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE IAGetInputLayout( 
		/* [annotation] */ 
		__out  ID3D11InputLayout **ppInputLayout);

	virtual void STDMETHODCALLTYPE IAGetVertexBuffers( 
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

	virtual void STDMETHODCALLTYPE IAGetIndexBuffer( 
		/* [annotation] */ 
		__out_opt  ID3D11Buffer **pIndexBuffer,
		/* [annotation] */ 
		__out_opt  DXGI_FORMAT *Format,
		/* [annotation] */ 
		__out_opt  UINT *Offset);

	virtual void STDMETHODCALLTYPE GSGetConstantBuffers( 
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		/* [annotation] */ 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		/* [annotation] */ 
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE GSGetShader( 
		/* [annotation] */ 
		__out  ID3D11GeometryShader **ppGeometryShader,
		/* [annotation] */ 
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		/* [annotation] */ 
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology( 
		/* [annotation] */ 
		__out  D3D11_PRIMITIVE_TOPOLOGY *pTopology);

	virtual void STDMETHODCALLTYPE VSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE VSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE GetPredication( 
		__out_opt  ID3D11Predicate **ppPredicate,
		__out_opt  BOOL *pPredicateValue);

	virtual void STDMETHODCALLTYPE GSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE GSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE OMGetRenderTargets( 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		__out_ecount_opt(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
		__out_opt  ID3D11DepthStencilView **ppDepthStencilView);

	virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews( 
		__in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
		__out_ecount_opt(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
		__out_opt  ID3D11DepthStencilView **ppDepthStencilView,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
		__out_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

	virtual void STDMETHODCALLTYPE OMGetBlendState( 
		__out_opt  ID3D11BlendState **ppBlendState,
		__out_opt  FLOAT BlendFactor[ 4 ],
		__out_opt  UINT *pSampleMask);

	virtual void STDMETHODCALLTYPE OMGetDepthStencilState( 
		__out_opt  ID3D11DepthStencilState **ppDepthStencilState,
		__out_opt  UINT *pStencilRef);

	virtual void STDMETHODCALLTYPE SOGetTargets( 
		__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets);

	virtual void STDMETHODCALLTYPE RSGetState( 
		__out  ID3D11RasterizerState **ppRasterizerState);

	virtual void STDMETHODCALLTYPE RSGetViewports( 
		__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
		__out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports);

	virtual void STDMETHODCALLTYPE RSGetScissorRects( 
		__inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
		__out_ecount_opt(*pNumRects)  D3D11_RECT *pRects);

	virtual void STDMETHODCALLTYPE HSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE HSGetShader( 
		__out  ID3D11HullShader **ppHullShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE HSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE HSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE DSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE DSGetShader( 
		__out  ID3D11DomainShader **ppDomainShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE DSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE DSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE CSGetShaderResources( 
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		__out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

	virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews( 
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
		__out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

	virtual void STDMETHODCALLTYPE CSGetShader( 
		__out  ID3D11ComputeShader **ppComputeShader,
		__out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		__inout_opt  UINT *pNumClassInstances);

	virtual void STDMETHODCALLTYPE CSGetSamplers( 
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		__out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

	virtual void STDMETHODCALLTYPE CSGetConstantBuffers( 
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		__in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		__out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

	virtual void STDMETHODCALLTYPE ClearState( void);

	virtual void STDMETHODCALLTYPE Flush( void);

	virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType( void);

	virtual UINT STDMETHODCALLTYPE GetContextFlags( void);

	virtual HRESULT STDMETHODCALLTYPE FinishCommandList( 
		BOOL RestoreDeferredContextState,
		__out_opt  ID3D11CommandList **ppCommandList);


private:
	void DeleteThis();

	void ClearCachedState();

	// check if draw calls can be issued
	void Check_CanDrawNow() const;

private:
	dxPtr< ID3D11DeviceContext >	m_pD3DContext;
	LONG							m_refCount;

	// Keep track of current states.

	TPtr< ID3D11InputLayout >		m_pCurrentInputLayout;

	TPtr< ID3D11Buffer >			m_pCurrentIndexBuffer;
	D3D11_PRIMITIVE_TOPOLOGY		m_nCurrentTopology;

	TPtr< ID3D11RasterizerState >	m_pCurrentRasterizerState;
	TPtr< ID3D11DepthStencilState >	m_pCurrentDepthStencilState;
	UINT							m_nCurrentStencilValue;
	TPtr< ID3D11BlendState >		m_pCurrentBlendState;

	TPtr< ID3D11VertexShader >		m_pCurrentVertexShader;
	TPtr< ID3D11HullShader >		m_pCurrentHullShader;
	TPtr< ID3D11DomainShader >		m_pCurrentDomainShader;
	TPtr< ID3D11GeometryShader >	m_pCurrentGeometryShader;
	TPtr< ID3D11PixelShader >		m_pCurrentPixelShader;
};

/*
-----------------------------------------------------------------------------
	D3D11DeviceProxy
-----------------------------------------------------------------------------
*/
class D3D11DeviceProxy : public ID3D11Device
{
public:
	D3D11DeviceProxy( ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DContext );
	~D3D11DeviceProxy();

	// IUnknown interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);

	virtual ULONG STDMETHODCALLTYPE Release( void);

	// ID3D11Device interface

	virtual HRESULT STDMETHODCALLTYPE CreateBuffer( 
		/* [annotation] */ 
		__in  const D3D11_BUFFER_DESC *pDesc,
		/* [annotation] */ 
		__in_opt  const D3D11_SUBRESOURCE_DATA *pInitialData,
		/* [annotation] */ 
		__out_opt  ID3D11Buffer **ppBuffer);

	virtual HRESULT STDMETHODCALLTYPE CreateTexture1D( 
		/* [annotation] */ 
		__in  const D3D11_TEXTURE1D_DESC *pDesc,
		/* [annotation] */ 
		__in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize)  const D3D11_SUBRESOURCE_DATA *pInitialData,
		/* [annotation] */ 
		__out_opt  ID3D11Texture1D **ppTexture1D);

	virtual HRESULT STDMETHODCALLTYPE CreateTexture2D( 
		/* [annotation] */ 
		__in  const D3D11_TEXTURE2D_DESC *pDesc,
		/* [annotation] */ 
		__in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize)  const D3D11_SUBRESOURCE_DATA *pInitialData,
		/* [annotation] */ 
		__out_opt  ID3D11Texture2D **ppTexture2D);

	virtual HRESULT STDMETHODCALLTYPE CreateTexture3D( 
		/* [annotation] */ 
		__in  const D3D11_TEXTURE3D_DESC *pDesc,
		/* [annotation] */ 
		__in_xcount_opt(pDesc->MipLevels)  const D3D11_SUBRESOURCE_DATA *pInitialData,
		/* [annotation] */ 
		__out_opt  ID3D11Texture3D **ppTexture3D);

	virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in_opt  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
		/* [annotation] */ 
		__out_opt  ID3D11ShaderResourceView **ppSRView);

	virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in_opt  const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
		/* [annotation] */ 
		__out_opt  ID3D11UnorderedAccessView **ppUAView);

	virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in_opt  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
		/* [annotation] */ 
		__out_opt  ID3D11RenderTargetView **ppRTView);

	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView( 
		/* [annotation] */ 
		__in  ID3D11Resource *pResource,
		/* [annotation] */ 
		__in_opt  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
		/* [annotation] */ 
		__out_opt  ID3D11DepthStencilView **ppDepthStencilView);

	virtual HRESULT STDMETHODCALLTYPE CreateInputLayout( 
		/* [annotation] */ 
		__in_ecount(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
		/* [annotation] */ 
		__in_range( 0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT )  UINT NumElements,
		/* [annotation] */ 
		__in  const void *pShaderBytecodeWithInputSignature,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__out_opt  ID3D11InputLayout **ppInputLayout);

	virtual HRESULT STDMETHODCALLTYPE CreateVertexShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11VertexShader **ppVertexShader);

	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11GeometryShader **ppGeometryShader);

	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_ecount_opt(NumEntries)  const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
		/* [annotation] */ 
		__in_range( 0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT )  UINT NumEntries,
		/* [annotation] */ 
		__in_ecount_opt(NumStrides)  const UINT *pBufferStrides,
		/* [annotation] */ 
		__in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumStrides,
		/* [annotation] */ 
		__in  UINT RasterizedStream,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11GeometryShader **ppGeometryShader);

	virtual HRESULT STDMETHODCALLTYPE CreatePixelShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11PixelShader **ppPixelShader);

	virtual HRESULT STDMETHODCALLTYPE CreateHullShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11HullShader **ppHullShader);

	virtual HRESULT STDMETHODCALLTYPE CreateDomainShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11DomainShader **ppDomainShader);

	virtual HRESULT STDMETHODCALLTYPE CreateComputeShader( 
		/* [annotation] */ 
		__in  const void *pShaderBytecode,
		/* [annotation] */ 
		__in  SIZE_T BytecodeLength,
		/* [annotation] */ 
		__in_opt  ID3D11ClassLinkage *pClassLinkage,
		/* [annotation] */ 
		__out_opt  ID3D11ComputeShader **ppComputeShader);

	virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage( 
		/* [annotation] */ 
		__out  ID3D11ClassLinkage **ppLinkage);

	virtual HRESULT STDMETHODCALLTYPE CreateBlendState( 
		/* [annotation] */ 
		__in  const D3D11_BLEND_DESC *pBlendStateDesc,
		/* [annotation] */ 
		__out_opt  ID3D11BlendState **ppBlendState);

	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState( 
		/* [annotation] */ 
		__in  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
		/* [annotation] */ 
		__out_opt  ID3D11DepthStencilState **ppDepthStencilState);

	virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState( 
		/* [annotation] */ 
		__in  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
		/* [annotation] */ 
		__out_opt  ID3D11RasterizerState **ppRasterizerState);

	virtual HRESULT STDMETHODCALLTYPE CreateSamplerState( 
		/* [annotation] */ 
		__in  const D3D11_SAMPLER_DESC *pSamplerDesc,
		/* [annotation] */ 
		__out_opt  ID3D11SamplerState **ppSamplerState);

	virtual HRESULT STDMETHODCALLTYPE CreateQuery( 
		/* [annotation] */ 
		__in  const D3D11_QUERY_DESC *pQueryDesc,
		/* [annotation] */ 
		__out_opt  ID3D11Query **ppQuery);

	virtual HRESULT STDMETHODCALLTYPE CreatePredicate( 
		/* [annotation] */ 
		__in  const D3D11_QUERY_DESC *pPredicateDesc,
		/* [annotation] */ 
		__out_opt  ID3D11Predicate **ppPredicate);

	virtual HRESULT STDMETHODCALLTYPE CreateCounter( 
		/* [annotation] */ 
		__in  const D3D11_COUNTER_DESC *pCounterDesc,
		/* [annotation] */ 
		__out_opt  ID3D11Counter **ppCounter);

	virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext( 
		UINT ContextFlags,
		/* [annotation] */ 
		__out_opt  ID3D11DeviceContext **ppDeferredContext);

	virtual HRESULT STDMETHODCALLTYPE OpenSharedResource( 
		/* [annotation] */ 
		__in  HANDLE hResource,
		/* [annotation] */ 
		__in  REFIID ReturnedInterface,
		/* [annotation] */ 
		__out_opt  void **ppResource);

	virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport( 
		/* [annotation] */ 
		__in  DXGI_FORMAT Format,
		/* [annotation] */ 
		__out  UINT *pFormatSupport);

	virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels( 
		/* [annotation] */ 
		__in  DXGI_FORMAT Format,
		/* [annotation] */ 
		__in  UINT SampleCount,
		/* [annotation] */ 
		__out  UINT *pNumQualityLevels);

	virtual void STDMETHODCALLTYPE CheckCounterInfo( 
		/* [annotation] */ 
		__out  D3D11_COUNTER_INFO *pCounterInfo);

	virtual HRESULT STDMETHODCALLTYPE CheckCounter( 
		/* [annotation] */ 
		__in  const D3D11_COUNTER_DESC *pDesc,
		/* [annotation] */ 
		__out  D3D11_COUNTER_TYPE *pType,
		/* [annotation] */ 
		__out  UINT *pActiveCounters,
		/* [annotation] */ 
		__out_ecount_opt(*pNameLength)  LPSTR szName,
		/* [annotation] */ 
		__inout_opt  UINT *pNameLength,
		/* [annotation] */ 
		__out_ecount_opt(*pUnitsLength)  LPSTR szUnits,
		/* [annotation] */ 
		__inout_opt  UINT *pUnitsLength,
		/* [annotation] */ 
		__out_ecount_opt(*pDescriptionLength)  LPSTR szDescription,
		/* [annotation] */ 
		__inout_opt  UINT *pDescriptionLength);

	virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport( 
		D3D11_FEATURE Feature,
		/* [annotation] */ 
		__out_bcount(FeatureSupportDataSize)  void *pFeatureSupportData,
		UINT FeatureSupportDataSize);

	virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__inout  UINT *pDataSize,
		/* [annotation] */ 
		__out_bcount_opt(*pDataSize)  void *pData);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in  UINT DataSize,
		/* [annotation] */ 
		__in_bcount_opt(DataSize)  const void *pData);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		/* [annotation] */ 
		__in  REFGUID guid,
		/* [annotation] */ 
		__in_opt  const IUnknown *pData);

	virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel( void);

	virtual UINT STDMETHODCALLTYPE GetCreationFlags( void);

	virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void);

	virtual void STDMETHODCALLTYPE GetImmediateContext( 
		/* [annotation] */ 
		__out  ID3D11DeviceContext **ppImmediateContext);

	virtual HRESULT STDMETHODCALLTYPE SetExceptionMode( 
		UINT RaiseFlags);

	virtual UINT STDMETHODCALLTYPE GetExceptionMode( void);

private:
	void DeleteThis();

private:
	dxPtr< ID3D11Device >			m_pD3DDevice;
	dxPtr< ID3D11DeviceContext >	m_pImmediateContext;

	LONG		m_refCount;
};


/*
===============================================================================

	Resource management

===============================================================================
*/


/*
--------------------------------------------------------------
	D3D11ResourceSystem
--------------------------------------------------------------
*/
class D3D11ResourceSystem : public GrResourceSystem
{
public:
	D3D11ResourceSystem();
	virtual ~D3D11ResourceSystem();

	virtual void Create_SamplerState(
		ID3D11SamplerStatePtr &r,
		const D3D11_SAMPLER_DESC& desc
	);

	virtual void Destroy_SamplerState(
		ID3D11SamplerStatePtr &r
	);


	virtual void Create_DepthStencilState(
		ID3D11DepthStencilStatePtr &r,
		const D3D11_DEPTH_STENCIL_DESC& desc
	);

	virtual void Destroy_DepthStencilState(
		ID3D11DepthStencilStatePtr &r
	);

	virtual void Create_BlendState(
		ID3D11BlendStatePtr &r,
		const D3D11_BLEND_DESC& desc
	);

	virtual void Destroy_BlendState(
		ID3D11BlendStatePtr &r
	);


	virtual void Create_RasterizerState(
		ID3D11RasterizerStatePtr &r,
		const D3D11_RASTERIZER_DESC& desc
	);

	virtual void Destroy_RasterizerState(
		ID3D11RasterizerStatePtr &r
	);


	virtual void Create_RenderTarget(
		RenderTarget &r,
		const D3D11_TEXTURE2D_DESC& tex2DDesc,
		const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc,
		const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc
	);

	virtual void Destroy_RenderTarget(
		RenderTarget &r
	);

	virtual void Create_InputLayout(
		ID3D11InputLayoutPtr &r,
		const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
		UINT NumElements
	);
	virtual void Destroy_InputLayout(
		ID3D11InputLayoutPtr &r
	);


	virtual void Create_VertexBuffer(
		ID3D11BufferPtr & r,
		UINT size,
		UINT stride,
		const void* data = nil,
		bool dynamic = false
	);
	virtual void Destroy_VertexBuffer(
		ID3D11BufferPtr &r
	);

	virtual void Create_IndexBuffer(
		GrIndexBuffer & r,
		UINT size,
		UINT stride,
		const void* data = nil,
		bool dynamic = false
	);
	virtual void Destroy_IndexBuffer(
		GrIndexBuffer &r
	);

public:	// Editor


};


/*
===============================================================================

	Shader management

===============================================================================
*/


#if MX_EDITOR

	// shader object visible to the editor
	struct EdShaderDataProxy : public AEditableRefCounted
	{
		GrShaderData &	d;
		const UINT	internalIndex;

	public:
		EdShaderDataProxy( GrShaderData & r, UINT index );
		~EdShaderDataProxy();

	public:	// Editor
		mxDECLARE_ABSTRACT_CLASS(EdShaderDataProxy,AEditableRefCounted);

		virtual UINT edNumRows() const override;
		virtual AEditable* edItemAt( UINT index ) override;
		virtual UINT edIndexOf( const AEditable* child ) const override;
		virtual const char* edToChars( UINT column ) const override;
		virtual AEditable* edGetParent() override;

		// currently used only by the editor
	};

#endif // MX_EDITOR


struct rxSourceFileProvider
{
	virtual GrCharBuffer* GetFileContents( const char* fileName ) = 0;

protected:
	rxSourceFileProvider()
	{}
	virtual ~rxSourceFileProvider()
	{}
};

//
//	rxShaderData - compiled shader byte code. Can be used for creating input layouts.
//
struct rxShaderData
{
	const void *	shaderBytecode;	// A pointer to shader data.
	SizeT			bytecodeLength;	// Length of shader data that pBytecode points to.
};

// abstract shader compiler
//
struct rxShaderCompiler
{
	virtual ID3DBlob* CompileShaderFromFile(
		const char* fileName,
		const char* profile,
		const char* entryPoint,
		const D3D_SHADER_MACRO* defines = nil
	) = 0;

	virtual ID3D11VertexShader* NewVertexShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	) = 0;

	virtual ID3D11GeometryShader* NewGeometryShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	) = 0;

	virtual ID3D11PixelShader* NewPixelShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	) = 0;

protected:
	rxShaderCompiler()
	{}
	virtual ~rxShaderCompiler()
	{}
};

/*
--------------------------------------------------------------
	FileIncludeProxy
--------------------------------------------------------------
*/
class FileIncludeProxy : public ID3DInclude
{
	TPtr< FileIncludeInterface >	mFileInclude;

public:
	FileIncludeProxy( FileIncludeInterface* fileInclude );
	virtual ~FileIncludeProxy();

	// Override ( ID3DInclude ) :

	STDMETHOD(Open)(
		THIS_ D3D_INCLUDE_TYPE IncludeType,
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID *ppData,
		UINT *pBytes
	);

	STDMETHOD(Close)(
		THIS_ LPCVOID pData
	);
};

/*
--------------------------------------------------------------
	FileIncludeManager
--------------------------------------------------------------
*/
class FileIncludeManager : public ID3DInclude
{
	TList< FileIncludeInterface* >	mFileIncludes;

public:
	FileIncludeManager();
	virtual ~FileIncludeManager();

	void Add( FileIncludeInterface* p );

	// Override ( ID3DInclude ) :

	STDMETHOD(Open)(
		THIS_ D3D_INCLUDE_TYPE IncludeType,
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID *ppData,
		UINT *pBytes
	);

	STDMETHOD(Close)(
		THIS_ LPCVOID pData
	);
};

/*
--------------------------------------------------------------
	CompilationLog
--------------------------------------------------------------
*/
class CompilationLog : public MessageListener
{
	TList< MessageListener* >	listeners;

	GrCharBuffer	log;

public:
	CompilationLog();

	void Attach( MessageListener* listener );
	void Detach( MessageListener* listener );

	virtual void Put( const char* message );

	const GrCharBuffer& Text() const
	{
		return this->log;
	}
};

/*
--------------------------------------------------------------
	LoggingMessageListener
--------------------------------------------------------------
*/
struct LoggingMessageListener : public MessageListener
{
	TList< CompileMessage >	messages;
	CompilationLog& log;

public:
	LoggingMessageListener( CompilationLog& theLog );
	~LoggingMessageListener();

	virtual void Put( const char* message );
};

/*
--------------------------------------------------------------
	D3D11CompiledShader

	 - compiled shader byte code.
--------------------------------------------------------------
*/
struct D3D11CompiledShader
{
	TList< BYTE >		data;	// compiled shader microcode that is consumed by the GPU driver

	inline D3D11CompiledShader( ID3DBlob* byteCode )
		: data(EMemHeap::HeapTemp)
	{
		AssertPtr(byteCode);
		this->Construct( byteCode );
	}

	inline const void* GetBufferPointer() const
	{
		return this->data.ToPtr();
	}
	inline SizeT GetBufferSize() const
	{
		return this->data.GetDataSize();
	}

	inline friend bool operator == ( const D3D11CompiledShader& a, const D3D11CompiledShader& b )
	{
		if( a.data.Num() != b.data.Num() ) {
			return false;
		}
		return (0 == MemCmp( a.data.ToPtr(), b.data.ToPtr(), b.data.GetDataSize() ));
	}

	inline friend U4 mxGetHashCode( const D3D11CompiledShader& key )
	{
		return MurmurHash( key.data.ToPtr(), key.data.GetDataSize(), 0 /*seed*/ );
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const D3D11CompiledShader& o )
	{
		file << o.data;
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, D3D11CompiledShader& o )
	{
		file >> o.data;
		return file;
	}

public_internal:	// needed for hash map

	inline D3D11CompiledShader()
		: data(EMemHeap::HeapTemp)
	{}
	inline D3D11CompiledShader( const D3D11CompiledShader& other )
		: data(EMemHeap::HeapTemp)
	{
		*this = other;
	}
	inline void operator = ( const D3D11CompiledShader& other )
	{
		this->data.Copy( other.data );
	}

private:
	void Construct( ID3DBlob* byteCode );
};

struct D3D11CreatedShader
{
#pragma pack (push,1)
	// pointer to created shader object (e.g.: ID3D11VertexShader, ID3D11PixelShader, etc)
	dxPtr< ID3D11DeviceChild >	pD3DShader;

	// EShaderType - needed for creating proper shader object when loading shader cache
	U2		eHWShaderType;

	// index of compiled bytecode
	U2		iCodeIndex;
#pragma pack (pop)

public:
	template< class SHADER_CLASS >
	FORCEINLINE SHADER_CLASS* GetShader()
	{
		return static_cast< SHADER_CLASS* >( pD3DShader.Ptr );
	}
	friend AStreamWriter& operator << ( AStreamWriter& file, const D3D11CreatedShader& o )
	{
		file << o.eHWShaderType << o.iCodeIndex;
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, D3D11CreatedShader& o )
	{
		file >> o.eHWShaderType >> o.iCodeIndex;
		return file;
	}
};

/*
--------------------------------------------------------------
	D3D11ShaderCache
	stores unique shader permutations and provides fast access to them,
	filters out duplicate shader microcode
--------------------------------------------------------------
*/
class D3D11ShaderCache

#if MX_EDITOR
	: public TRefCountedObjectList< EdShaderDataProxy >
#endif // MX_EDITOR

{
public:
	D3D11ShaderCache();
	~D3D11ShaderCache();

	void RegisterNewShader( GrShaderData * shaderInfo );

	void Save( AStreamWriter& stream );
	bool Load( AStreamReader& stream, const FileTime& srcTimeStamp, rxShaderCompiler& compiler );

	void RecompileAllShaders( rxShaderCompiler& compiler );

	// returns the number of registered shaders
	UINT Num() const
	{
		return m_shadersList.Num();
	}

	const D3D11CompiledShader& GetShaderByteCodeByIndex( UINT codeIndex ) const
	{
		return m_compiledCodeMap.GetPairs()[ codeIndex ].key;
	}

	void RecompileShader( rxShaderCompiler& compiler, GrShaderData* shaderData );

private:
	void CompileShaderInstance( rxShaderCompiler& compiler, GrShaderData* shaderData, UINT iShaderInstance );

	ID3D11VertexShader* GetCachedVertexShader( rxShaderCompiler& compiler, const dxPtr< ID3DBlob >& compiledByteCode );
	ID3D11PixelShader* GetCachedPixelShader( rxShaderCompiler& compiler, const dxPtr< ID3DBlob >& compiledByteCode );

	void CacheShader( const D3D11CompiledShader& code, ID3D11DeviceChild* newShader );

	U4 CalculateCheckSum();


	template< const EShaderType SHADER_TYPE_ENUM >
	inline ID3DBlob* TCompileShader( rxShaderCompiler& compiler, const GrShaderData* shaderInfo, const D3D_SHADER_MACRO* defines )
	{
		switch( SHADER_TYPE_ENUM )
		{
		case EShaderType::ST_Vertex_Shader : return compiler.CompileShaderFromFile(
				shaderInfo->file,
				D3D_GetVertexShaderProfile(),
				shaderInfo->VS_EntryPoint,
				defines
			);

		case EShaderType::ST_Pixel_Shader :
			// pixel shader can be null (e.g. shadow depth shader)
			if( shaderInfo->PS_EntryPoint != nil )
			{
				return compiler.CompileShaderFromFile(
					shaderInfo->file,
					D3D_GetPixelShaderProfile(),
					shaderInfo->PS_EntryPoint,
					defines
				);
			}
			else
			{
				return nil;
			}

		default: Unimplemented;
		}
		return nil;
	}

	inline ID3D11DeviceChild* CreateShader( rxShaderCompiler& compiler, const D3D11CompiledShader& compiledByteCode, const EShaderType shaderType )
	{
		switch( shaderType )
		{
		case EShaderType::ST_Vertex_Shader :
			{
				ID3D11VertexShader* newVertexShader = compiler.NewVertexShader(
					compiledByteCode.GetBufferPointer(),
					compiledByteCode.GetBufferSize()
				);
				return newVertexShader;
			}
			break;

		case EShaderType::ST_Pixel_Shader :
			{
				ID3D11PixelShader* newPixelShader = compiler.NewPixelShader(
					compiledByteCode.GetBufferPointer(),
					compiledByteCode.GetBufferSize()
				);
				return newPixelShader;
			}
			break;

		default: Unimplemented;
		}
		return nil;
	}

	template< class SHADER_CLASS, const EShaderType SHADER_TYPE_ENUM >
	SHADER_CLASS* TGetCachedShader( rxShaderCompiler& compiler, GrShaderData* shaderData, const D3D_SHADER_MACRO* defines, UINT &codeIndex )
	{
		// Compile the shader.
		dxPtr< ID3DBlob >	compiledByteCode = this->TCompileShader< SHADER_TYPE_ENUM >( compiler, shaderData, defines );
		if( !compiledByteCode )
		{
			return nil;
		}

		// Try to find an existing shader object...

		RX_OPTIMIZE("reduce memory allocations and copying");
		D3D11CompiledShader	code( compiledByteCode );

		const UINT* cachedShaderIndex = m_compiledCodeMap.Find( code );

		// ... and return it, if it's present in cache.

		if(PtrToBool( cachedShaderIndex ))
		{
			m_numByteCodeDuplicates[ SHADER_TYPE_ENUM ]++;

			codeIndex = *cachedShaderIndex;
			D3D11CreatedShader& createdShaderData = m_createdShaders[ *cachedShaderIndex ];

			return createdShaderData.GetShader<SHADER_CLASS>();
		}

		// cache it

		ID3D11DeviceChild* newShader = this->CreateShader( compiler, code, SHADER_TYPE_ENUM );

		const UINT	newShaderIndex = m_createdShaders.Num();

		codeIndex = newShaderIndex;

		D3D11CreatedShader& newShaderData = m_createdShaders.Add();
		newShaderData.pD3DShader = newShader;
		newShaderData.eHWShaderType = SHADER_TYPE_ENUM;
		newShaderData.iCodeIndex = m_compiledCodeMap.NumEntries();

		m_compiledCodeMap.Set( code, newShaderIndex );

		return newShaderData.GetShader<SHADER_CLASS>();
	}


	mxOPTIMIZE("don't keep compiled shader bytecode in release mode, it's only needed to create shaders and filter out duplicates");



private:
	TList< GrShaderData* >	m_shadersList;	// all registered shaders (shader data is not owned by the list)

	// NOTE: we keep indices, not pointers, because elements' memory locations can change (e.g. after resizing).
	// Besides, serializing them is easier.

	// used to filter out duplicate compiled shader bytecode;
	// maps compiled code into indices of created shaders
	TMap< D3D11CompiledShader, UINT >		m_compiledCodeMap;	// maps (unique) instances of compiled shader code to indices in 'm_createdShaders' array

	// all (unique) created shader objects
	TList< D3D11CreatedShader >		m_createdShaders;

	// Stats.
	TStaticArray_InitZeroed< UINT, NumShaderTypes >	m_numByteCodeDuplicates;

#if MX_EDITOR
public:	// Editor
	virtual const char* edToChars( UINT column ) const override;
	virtual AEditable* edGetParent() override;
#endif // MX_EDITOR
};

/*
--------------------------------------------------------------
	D3D11ShaderSystem
--------------------------------------------------------------
*/
class D3D11ShaderSystem
	: public GrShaderSystem
	, public rxShaderCompiler
{
public:
	D3D11ShaderSystem();
	virtual ~D3D11ShaderSystem();


public:	// GrShaderSystem overrides

	virtual void Create_ConstantBuffer(
		ID3D11BufferPtr &r,
		UINT size,
		const void* data = nil
	);

	virtual void Destroy_ConstantBuffer(
		ID3D11BufferPtr &r
	);



	virtual void RegisterShader(
		GrShaderData * shaderInfo
	);

	virtual void GetShaderSource(
		const ANSICHAR* fileName,
		const ANSICHAR*&fileData,
		UINT &fileSize
	);
	virtual void ReloadShader(
		GrShaderData * shaderInfo,
		const ANSICHAR* source,
		const UINT sourceLength
	);


	virtual void CompileShaders(
		const GrShaderCacheInfo& settings
	);
	virtual void SaveCompiledShaders(
		PCSTR pathToShaderCache
	);

	virtual const char* GetCompilationErrors();

public:	// rxShaderCompiler

	virtual ID3DBlob* CompileShaderFromFile(
		const char* fileName,
		const char* profile,
		const char* entryPoint,
		const D3D_SHADER_MACRO* defines = nil
	);

	virtual ID3D11VertexShader* NewVertexShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	);

	virtual ID3D11GeometryShader* NewGeometryShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	);

	virtual ID3D11PixelShader* NewPixelShader(
		const void* compiledBytecode,
		SizeT bytecodeLength,
		ID3D11ClassLinkage* linkage = nil
	);

#if MX_EDITOR
public:	// Editor
	mxIMPLEMENT_ED_GROUP_1( m_shaderCache );
	virtual const char* edToChars( UINT column ) const override;
#endif // MX_EDITOR

	D3D11ShaderCache& GetShaderCache() { return m_shaderCache; }

private:
	void GetFullPathToFile( PCSTR fileName, OSPathName &OutPath );

protected:
	D3D11ShaderCache	m_shaderCache;
	OSPathName			m_pathToSources;
	CompilationLog		m_compilationLog;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
