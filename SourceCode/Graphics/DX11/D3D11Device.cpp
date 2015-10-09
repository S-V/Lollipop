/*
=============================================================================
	File:	D3D11Device.cpp
	Desc:	Direct3D 11 device initialization and destruction.
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

#include "DX11Private.h"

namespace
{
	static D3D_FEATURE_LEVEL minimumRequiredFeatureLevel = D3D_FEATURE_LEVEL_10_0;
	static D3D_FEATURE_LEVEL selectedFeatureFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	static EDeviceVendor	deviceVendor = Vendor_Unknown;
}//namespace

//-------------------------------------------------------------------------------------------------------------//
D3D_FEATURE_LEVEL D3D_GetSelectedFeatureLevel()
{
	return selectedFeatureFeatureLevel;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetShaderModel()
{
	return "SM_4_0";
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetVertexShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_9_1 :	 return "vs_4_0_level_9_1";
	case D3D_FEATURE_LEVEL_9_2 :	 return "vs_4_0_level_9_2";
	case D3D_FEATURE_LEVEL_9_3 :	 return "vs_4_0_level_9_3";
	case D3D_FEATURE_LEVEL_10_0 :	 return "vs_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "vs_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "vs_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetGeometryShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_9_1 :	 return "gs_4_0_level_9_1";
	case D3D_FEATURE_LEVEL_9_2 :	 return "gs_4_0_level_9_2";
	case D3D_FEATURE_LEVEL_9_3 :	 return "gs_4_0_level_9_3";
	case D3D_FEATURE_LEVEL_10_0 :	 return "gs_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "gs_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "gs_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetPixelShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_9_1 :	 return "ps_4_0_level_9_1";
	case D3D_FEATURE_LEVEL_9_2 :	 return "ps_4_0_level_9_2";
	case D3D_FEATURE_LEVEL_9_3 :	 return "ps_4_0_level_9_3";
	case D3D_FEATURE_LEVEL_10_0 :	 return "ps_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "ps_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "ps_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetComputeShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_10_0 :	 return "cs_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "cs_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "cs_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetDomainShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_10_0 :	 return "ds_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "ds_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "ds_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_GetHullShaderProfile()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_10_0 :	 return "hs_4_0";
	case D3D_FEATURE_LEVEL_10_1 :	 return "hs_4_1";
	case D3D_FEATURE_LEVEL_11_0 :	 return "hs_5_0";
	}
	Unreachable;
	return nil;
}
//-------------------------------------------------------------------------------------------------------------//
EDeviceVendor D3D_GetDeviceVendor()
{
	return deviceVendor;
}
//-------------------------------------------------------------------------------------------------------------//
// Returns the maximum width/height of a texture. This value only depends upon the feature level in DX11
UINT D3D_GetMaxTextureSize()
{
	switch( selectedFeatureFeatureLevel )
	{
	case D3D_FEATURE_LEVEL_11_0:
		return D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;

	case D3D_FEATURE_LEVEL_10_1:
	case D3D_FEATURE_LEVEL_10_0:
		return D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;

	case D3D_FEATURE_LEVEL_9_3:
		return 4096;

	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_1:
		return 2048;

	default:
		Unreachable;
		return 0;
	}
}
//-------------------------------------------------------------------------------------------------------------//
static bool SetupDXGI(
	dxPtr< IDXGIFactory > & outDXGIFactory,
	dxPtr< IDXGIAdapter > & outDXGIAdapter,
	dxPtr< IDXGIOutput > & outDXGIOutput )
{
	HRESULT hr = E_FAIL;

	// Create DXGI factory.

	dxPtr< IDXGIFactory >	pDXGIFactory;
	hr = CreateDXGIFactory(
		__uuidof( IDXGIFactory ),
		(void**) &pDXGIFactory.Ptr
	);
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to create DXGIFactory\n" );
		return false;
	}

	// Enumerate adapters.
	{
		UINT iAdapter = 0;
		IDXGIAdapter * pAdapter = nil;
		while( pDXGIFactory->EnumAdapters( iAdapter, &pAdapter ) != DXGI_ERROR_NOT_FOUND )
		{
			pAdapter->Release();
			++iAdapter;
		}
		dxMsgf("Detected %d video adapter(s)\n",iAdapter);
	}

	dxPtr< IDXGIAdapter >	pDXGIAdapter0;
	hr = pDXGIFactory->EnumAdapters( 0, &pDXGIAdapter0.Ptr );
	if( hr == DXGI_ERROR_NOT_FOUND ) {
		dxErrf( "Failed to enumerate video adapter 0\n" );
		return false;
	}

	DXGI_ADAPTER_DESC  adapterDesc;
	hr = pDXGIAdapter0->GetDesc( &adapterDesc );
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to get adapter description\n" );
		return false;
	}

	{
		dxMsgf("Adapter description: %s\n",mxTO_ANSI(adapterDesc.Description));
		dxMsgf("VendorId: %u, DeviceId: %u, SubSysId: %u, Revision: %u\n",
			adapterDesc.VendorId,adapterDesc.DeviceId,adapterDesc.SubSysId,adapterDesc.Revision);

		deviceVendor = rxUtil_GetGPUVendorEnum( adapterDesc.VendorId );
		if( Vendor_Unknown != deviceVendor ) {
			dxMsgf("Device vendor: %s\n",mxTO_ANSI(rxUtil_GetGPUVendorName(deviceVendor)));
		}
		dxMsgf("Dedicated video memory: %u Mb, Dedicated system memory: %u Mb, Shared system memory: %u Mb\n",
			adapterDesc.DedicatedVideoMemory / (1024*1024),
			adapterDesc.DedicatedSystemMemory / (1024*1024),
			adapterDesc.SharedSystemMemory / (1024*1024)
		);
	}

	dxPtr< IDXGIOutput > pDXGIOutput0;
	hr = pDXGIAdapter0->EnumOutputs( 0 /* The index of the output (monitor) */,
									&pDXGIOutput0.Ptr );
	if( hr == DXGI_ERROR_NOT_FOUND )
	{
		dxErrf( "Failed to enumerate video card output 0\n" );
		return false;
	}

	DXGI_OUTPUT_DESC  oDesc;
	hr = pDXGIOutput0->GetDesc( &oDesc );
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to get video card output description\n" );
		return false;
	}

	{
		dxMsgf("Detected output device: %s\n",mxTO_ANSI(oDesc.DeviceName));
	//	dxMsgf("Desktop coordinates: left(%d),top(%d),right(%d),bottom(%d)\n",
	//		oDesc.DesktopCoordinates.left,oDesc.DesktopCoordinates.top,oDesc.DesktopCoordinates.right,oDesc.DesktopCoordinates.bottom);
		UINT deckstopScrWidth, deckstopScrHeight;
		mxGetCurrentDeckstopResolution( deckstopScrWidth, deckstopScrHeight );
		dxMsgf( "Current desktop resolution: %ux%u\n", deckstopScrWidth, deckstopScrHeight );
	}

	outDXGIFactory = pDXGIFactory;
	outDXGIAdapter = pDXGIAdapter0;
	outDXGIOutput = pDXGIOutput0;

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static UINT GetCreateDeviceFlags()
{
	UINT  createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	if(RX_DEBUG_RENDERER)
	{
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
	return createDeviceFlags;
}
//-------------------------------------------------------------------------------------------------------------//
static void SetupDebugHelpers( const dxPtr< ID3D11Device >& inD3DDevice )
{
	// NOTE: this incurs a significant overhead (frame render time went up from 2 to 15 ms) but helps to find rare driver bugs...
	if (0 && RX_DEBUG_RENDERER)
	{
		dxPtr< ID3D11Debug >	d3dDebug;
		if( SUCCEEDED( inD3DDevice->QueryInterface( IID_ID3D11Debug, (void**)&d3dDebug.Ptr ) )
			&& (d3dDebug != nil) )
		{
			dxchk(d3dDebug->SetFeatureMask( D3D11_DEBUG_FEATURE_FINISH_PER_RENDER_OP ));
		}
	}
}
//-------------------------------------------------------------------------------------------------------------//
static bool CreateDirect3DDevice(
	const dxPtr< IDXGIAdapter >& inDXGIAdapter,
	dxPtr< ID3D11Device > & outDevice,
	dxPtr< ID3D11DeviceContext > & outImmediateContext
	)
{
	//const D3D_DRIVER_TYPE	driverType = D3D_DRIVER_TYPE_UNKNOWN;

	const UINT  createDeviceFlags = GetCreateDeviceFlags();

	// Required for Direct2D interoperability with Direct3D resources.
	// Needed for supporting DXGI_FORMAT_B8G8R8A8_UNORM. Only works with 10.1 and above.
	//createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL	featureLevels[] =
	{
		//D3D_FEATURE_LEVEL_11_0,
		//D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		//D3D_FEATURE_LEVEL_9_3,
		//D3D_FEATURE_LEVEL_9_2,
		//D3D_FEATURE_LEVEL_9_1,
	};

	D3D_FEATURE_LEVEL	selectedFeatureLevel;

	HRESULT hr = E_FAIL;

	{
		// If you set the pAdapter parameter to a non-NULL value, you must also set the DriverType parameter to the D3D_DRIVER_TYPE_UNKNOWN value.
		// If you set the pAdapter parameter to a non-NULL value and the DriverType parameter to the D3D_DRIVER_TYPE_HARDWARE value, D3D11CreateDevice returns an HRESULT of E_INVALIDARG.
		// See: "D3D11CreateDevice function"
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ff476082(v=vs.85).aspx
		// http://www.gamedev.net/topic/561002-d3d11createdevice---invalid_arg/
		//
		hr = D3D11CreateDevice(
			inDXGIAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			( HMODULE )nil,	/* HMODULE Software rasterizer */
			createDeviceFlags,
			featureLevels,	// array of feature levels, null means 'get the greatest feature level available'
			NUMBER_OF(featureLevels),
			D3D11_SDK_VERSION,
			&outDevice.Ptr,
			&selectedFeatureLevel,
			&outImmediateContext.Ptr
		);
	}

	if( FAILED( hr ) )
	{
		dxErrf( hr,
			"Failed to create Direct3D 11 device.\n"
			"This application requires a Direct3D 11 class device "
			"running on Windows Vista (or later)\n" );
		return false;
	}

	SetupDebugHelpers( outDevice );

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static bool CheckFeatureLevel( const dxPtr< ID3D11Device >& pD3DDevice )
{
	const D3D_FEATURE_LEVEL selectedFeatureLevel = pD3DDevice->GetFeatureLevel();

	dxMsgf("Selected feature level: %s\n",D3D_FeatureLevelToStr( selectedFeatureLevel ));

	if( selectedFeatureLevel < minimumRequiredFeatureLevel )
	{
		dxErrf("This application requires a Direct3D 11 class pD3DDevice with at least %s support\n",
			D3D_FeatureLevelToStr(minimumRequiredFeatureLevel));
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static bool LogDirect3DInfo( const dxPtr< ID3D11Device >& inD3DDevice )
{
	// GetGPUThreadPriority() returns E_FAIL when running in PIX
#if !RX_PROFILE
	if(1)
	{
		dxPtr< IDXGIDevice >	pDXGIDevice;
		const HRESULT hr = inD3DDevice->QueryInterface(
			__uuidof( IDXGIDevice ),
			(void**) &pDXGIDevice.Ptr
		);
		if( FAILED( hr ) ) {
			dxErrf( hr, "Failed to get IDXGIDevice interface\n" );
			return false;
		}

		if( nil != pDXGIDevice )
		{
			INT renderThreadPriority;
			if(FAILED( pDXGIDevice->GetGPUThreadPriority( &renderThreadPriority ) ))
			{
				dxErrf( hr, "GetGPUThreadPriority() failed\n" );
			}
			dxMsgf("GPU thread priority: %d.\n",renderThreadPriority);
		}
	}
#endif
	if(1)
	{
		D3D11_FEATURE_DATA_THREADING  featureSupportData_Threading;

		if(SUCCEEDED(inD3DDevice->CheckFeatureSupport(
			D3D11_FEATURE_THREADING,
			&featureSupportData_Threading,
			sizeof(featureSupportData_Threading)
			)))
		{
			if( TRUE == featureSupportData_Threading.DriverConcurrentCreates ) {
				dxMsg("Driver supports concurrent resource creation\n");
			}
			if( TRUE == featureSupportData_Threading.DriverCommandLists ) {
				dxMsg("Driver supports command lists\n");
			}
		}
	}
	if(1)
	{
		D3D11_FEATURE_DATA_DOUBLES  featureSupportData_Doubles;

		if(SUCCEEDED(inD3DDevice->CheckFeatureSupport(
			D3D11_FEATURE_DOUBLES,
			&featureSupportData_Doubles,
			sizeof(featureSupportData_Doubles)
			)))
		{
			if( TRUE == featureSupportData_Doubles.DoublePrecisionFloatShaderOps ) {
				dxMsg("Driver supports double data types\n");
			}
		}
	}
	if(1)
	{
		D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS		hardwareOptions;
		if(SUCCEEDED(inD3DDevice->CheckFeatureSupport(
			D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS,
			&hardwareOptions,
			sizeof(hardwareOptions)
			)))
		{
			if( TRUE == hardwareOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x ) {
				dxMsg("Driver supports Compute Shaders Plus Raw And Structured Buffers Via Shader Model 4.x\n");
			}
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------//

D3D11DeviceProxy::D3D11DeviceProxy( ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DContext )
{
	AssertPtr(pD3DDevice);
	m_pD3DDevice = pD3DDevice;

	AssertPtr(pD3DContext);
	m_pImmediateContext = pD3DContext;

	m_refCount = 0;
}

D3D11DeviceProxy::~D3D11DeviceProxy()
{
	Assert( m_refCount == 0 );
	Assert( m_pImmediateContext == nil );
	Assert( m_pD3DDevice == nil );
}

void D3D11DeviceProxy::DeleteThis()
{
	Assert( m_refCount == 0 );
	//MX_DEBUG_BREAK;

	DbgPutNumRefs( m_pImmediateContext.Ptr, "D3DContext: " );
	m_pImmediateContext = nil;

	DbgPutNumRefs( m_pD3DDevice.Ptr, "D3DDevice: " );
	//m_pD3DDevice = nil;
	FORCE_RELEASE_COM_INTERFACE( m_pD3DDevice.Ptr );

	// this object is allocated statically, so don't call 'delete this'
}

HRESULT D3D11DeviceProxy::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return m_pD3DDevice->QueryInterface( riid, ppvObject );
}

ULONG D3D11DeviceProxy::AddRef( void)
{
	const LONG numRefs = ::InterlockedIncrement( &m_refCount );
	DBGOUT("D3D11DeviceProxy::AddRef(): %u refs\n",(UINT)numRefs);
	return (ULONG)numRefs;
}

ULONG D3D11DeviceProxy::Release( void)
{
	Assert( m_refCount > 0 );

	const ULONG numRefs = ::InterlockedDecrement( &m_refCount );

	DBGOUT("D3D11DeviceProxy::Release(): %u refs\n",(UINT)numRefs);

	if( numRefs == 0 )
	{
		this->DeleteThis();
	}

	return numRefs;
}

HRESULT D3D11DeviceProxy::CreateBuffer( 
	/* [annotation] */ 
	__in  const D3D11_BUFFER_DESC *pDesc,
	/* [annotation] */ 
	__in_opt  const D3D11_SUBRESOURCE_DATA *pInitialData,
	/* [annotation] */ 
	__out_opt  ID3D11Buffer **ppBuffer)
{
	DEVOUT("D3DDevice.CreateBuffer: Size=%u, Usage=%s\n", pDesc->ByteWidth, D3D11_USAGE_To_Chars(pDesc->Usage));
	return m_pD3DDevice->CreateBuffer( pDesc, pInitialData, ppBuffer );
}

HRESULT D3D11DeviceProxy::CreateTexture1D( 
	/* [annotation] */ 
	__in  const D3D11_TEXTURE1D_DESC *pDesc,
	/* [annotation] */ 
	__in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize)  const D3D11_SUBRESOURCE_DATA *pInitialData,
	/* [annotation] */ 
	__out_opt  ID3D11Texture1D **ppTexture1D)
{
	DEVOUT("D3DDevice.CreateTexture1D: %s, Width=%u, Usage=%s\n",
		DXGIFormat_ToString(pDesc->Format), pDesc->Width, D3D11_USAGE_To_Chars(pDesc->Usage));
	return m_pD3DDevice->CreateTexture1D( pDesc, pInitialData, ppTexture1D );
}

HRESULT D3D11DeviceProxy::CreateTexture2D( 
	/* [annotation] */ 
	__in  const D3D11_TEXTURE2D_DESC *pDesc,
	/* [annotation] */ 
	__in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize)  const D3D11_SUBRESOURCE_DATA *pInitialData,
	/* [annotation] */ 
	__out_opt  ID3D11Texture2D **ppTexture2D)
{
	DEVOUT("D3DDevice.CreateTexture2D: %s, Width=%u, Height=%u, Usage=%s\n",
		DXGIFormat_ToString(pDesc->Format), pDesc->Width, pDesc->Height, D3D11_USAGE_To_Chars(pDesc->Usage));
	return m_pD3DDevice->CreateTexture2D( pDesc, pInitialData, ppTexture2D );
}

HRESULT D3D11DeviceProxy::CreateTexture3D( 
	/* [annotation] */ 
	__in  const D3D11_TEXTURE3D_DESC *pDesc,
	/* [annotation] */ 
	__in_xcount_opt(pDesc->MipLevels)  const D3D11_SUBRESOURCE_DATA *pInitialData,
	/* [annotation] */ 
	__out_opt  ID3D11Texture3D **ppTexture3D)
{
	DEVOUT("D3DDevice.CreateTexture3D: %s, Width=%u, Height=%u, Depth=%u, Usage=%s\n",
		DXGIFormat_ToString(pDesc->Format), pDesc->Width, pDesc->Height, pDesc->Depth, D3D11_USAGE_To_Chars(pDesc->Usage));
	return m_pD3DDevice->CreateTexture3D( pDesc, pInitialData, ppTexture3D );
}

HRESULT D3D11DeviceProxy::CreateShaderResourceView( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in_opt  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
	/* [annotation] */ 
	__out_opt  ID3D11ShaderResourceView **ppSRView)
{
	DEVOUT("D3DDevice.CreateShaderResourceView: %s, Dimension=%s\n",
		DXGIFormat_ToString(pDesc->Format), D3D11_SRV_DIMENSION_To_Chars(pDesc->ViewDimension));
	return m_pD3DDevice->CreateShaderResourceView( pResource, pDesc, ppSRView );
}

HRESULT D3D11DeviceProxy::CreateUnorderedAccessView( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in_opt  const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
	/* [annotation] */ 
	__out_opt  ID3D11UnorderedAccessView **ppUAView)
{
	return m_pD3DDevice->CreateUnorderedAccessView( pResource, pDesc, ppUAView );
}

HRESULT D3D11DeviceProxy::CreateRenderTargetView( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in_opt  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
	/* [annotation] */ 
	__out_opt  ID3D11RenderTargetView **ppRTView)
{
	return m_pD3DDevice->CreateRenderTargetView( pResource, pDesc, ppRTView );
}

HRESULT D3D11DeviceProxy::CreateDepthStencilView( 
	/* [annotation] */ 
	__in  ID3D11Resource *pResource,
	/* [annotation] */ 
	__in_opt  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
	/* [annotation] */ 
	__out_opt  ID3D11DepthStencilView **ppDepthStencilView)
{
	return m_pD3DDevice->CreateDepthStencilView( pResource, pDesc, ppDepthStencilView );
}

HRESULT D3D11DeviceProxy::CreateInputLayout( 
	/* [annotation] */ 
	__in_ecount(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
	/* [annotation] */ 
	__in_range( 0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT )  UINT NumElements,
	/* [annotation] */ 
	__in  const void *pShaderBytecodeWithInputSignature,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__out_opt  ID3D11InputLayout **ppInputLayout)
{
	return m_pD3DDevice->CreateInputLayout( pInputElementDescs, NumElements,
		pShaderBytecodeWithInputSignature, BytecodeLength,
		ppInputLayout );
}

HRESULT D3D11DeviceProxy::CreateVertexShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11VertexShader **ppVertexShader)
{
	return m_pD3DDevice->CreateVertexShader( pShaderBytecode, BytecodeLength,
		pClassLinkage, ppVertexShader );
}

HRESULT D3D11DeviceProxy::CreateGeometryShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11GeometryShader **ppGeometryShader)
{
	return m_pD3DDevice->CreateGeometryShader( pShaderBytecode, BytecodeLength,
		pClassLinkage, ppGeometryShader );
}

HRESULT D3D11DeviceProxy::CreateGeometryShaderWithStreamOutput( 
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
	__out_opt  ID3D11GeometryShader **ppGeometryShader)
{
	return m_pD3DDevice->CreateGeometryShaderWithStreamOutput( pShaderBytecode, BytecodeLength,
		pSODeclaration,
		NumEntries,
		pBufferStrides, NumStrides,
		RasterizedStream,
		pClassLinkage, ppGeometryShader );
}

HRESULT D3D11DeviceProxy::CreatePixelShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11PixelShader **ppPixelShader)
{
	return m_pD3DDevice->CreatePixelShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader );
}

HRESULT D3D11DeviceProxy::CreateHullShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11HullShader **ppHullShader)
{
	return m_pD3DDevice->CreateHullShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader );
}

HRESULT D3D11DeviceProxy::CreateDomainShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11DomainShader **ppDomainShader)
{
	return m_pD3DDevice->CreateDomainShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader );
}

HRESULT D3D11DeviceProxy::CreateComputeShader( 
	/* [annotation] */ 
	__in  const void *pShaderBytecode,
	/* [annotation] */ 
	__in  SIZE_T BytecodeLength,
	/* [annotation] */ 
	__in_opt  ID3D11ClassLinkage *pClassLinkage,
	/* [annotation] */ 
	__out_opt  ID3D11ComputeShader **ppComputeShader)
{
	return m_pD3DDevice->CreateComputeShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader );
}

HRESULT D3D11DeviceProxy::CreateClassLinkage( 
	/* [annotation] */ 
	__out  ID3D11ClassLinkage **ppLinkage)
{
	return m_pD3DDevice->CreateClassLinkage( ppLinkage );
}

HRESULT D3D11DeviceProxy::CreateBlendState( 
	/* [annotation] */ 
	__in  const D3D11_BLEND_DESC *pBlendStateDesc,
	/* [annotation] */ 
	__out_opt  ID3D11BlendState **ppBlendState)
{
	return m_pD3DDevice->CreateBlendState( pBlendStateDesc, ppBlendState );
}

HRESULT D3D11DeviceProxy::CreateDepthStencilState( 
	/* [annotation] */ 
	__in  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
	/* [annotation] */ 
	__out_opt  ID3D11DepthStencilState **ppDepthStencilState)
{
	return m_pD3DDevice->CreateDepthStencilState( pDepthStencilDesc, ppDepthStencilState );
}

HRESULT D3D11DeviceProxy::CreateRasterizerState( 
	/* [annotation] */ 
	__in  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
	/* [annotation] */ 
	__out_opt  ID3D11RasterizerState **ppRasterizerState)
{
	return m_pD3DDevice->CreateRasterizerState( pRasterizerDesc, ppRasterizerState );
}

HRESULT D3D11DeviceProxy::CreateSamplerState( 
	/* [annotation] */ 
	__in  const D3D11_SAMPLER_DESC *pSamplerDesc,
	/* [annotation] */ 
	__out_opt  ID3D11SamplerState **ppSamplerState)
{
	return m_pD3DDevice->CreateSamplerState( pSamplerDesc, ppSamplerState );
}

HRESULT D3D11DeviceProxy::CreateQuery( 
	/* [annotation] */ 
	__in  const D3D11_QUERY_DESC *pQueryDesc,
	/* [annotation] */ 
	__out_opt  ID3D11Query **ppQuery)
{
	return m_pD3DDevice->CreateQuery( pQueryDesc, ppQuery );
}

HRESULT D3D11DeviceProxy::CreatePredicate( 
	/* [annotation] */ 
	__in  const D3D11_QUERY_DESC *pPredicateDesc,
	/* [annotation] */ 
	__out_opt  ID3D11Predicate **ppPredicate)
{
	return m_pD3DDevice->CreatePredicate( pPredicateDesc, ppPredicate );
}

HRESULT D3D11DeviceProxy::CreateCounter( 
	/* [annotation] */ 
	__in  const D3D11_COUNTER_DESC *pCounterDesc,
	/* [annotation] */ 
	__out_opt  ID3D11Counter **ppCounter)
{
	return m_pD3DDevice->CreateCounter( pCounterDesc, ppCounter );
}

HRESULT D3D11DeviceProxy::CreateDeferredContext( 
	UINT ContextFlags,
	/* [annotation] */ 
	__out_opt  ID3D11DeviceContext **ppDeferredContext)
{
	return m_pD3DDevice->CreateDeferredContext( ContextFlags, ppDeferredContext );
}

HRESULT D3D11DeviceProxy::OpenSharedResource( 
	/* [annotation] */ 
	__in  HANDLE hResource,
	/* [annotation] */ 
	__in  REFIID ReturnedInterface,
	/* [annotation] */ 
	__out_opt  void **ppResource)
{
	return m_pD3DDevice->OpenSharedResource( hResource, ReturnedInterface, ppResource );
}

HRESULT D3D11DeviceProxy::CheckFormatSupport( 
	/* [annotation] */ 
	__in  DXGI_FORMAT Format,
	/* [annotation] */ 
	__out  UINT *pFormatSupport)
{
	return m_pD3DDevice->CheckFormatSupport( Format, pFormatSupport );
}

HRESULT D3D11DeviceProxy::CheckMultisampleQualityLevels( 
	/* [annotation] */ 
	__in  DXGI_FORMAT Format,
	/* [annotation] */ 
	__in  UINT SampleCount,
	/* [annotation] */ 
	__out  UINT *pNumQualityLevels)
{
	return m_pD3DDevice->CheckMultisampleQualityLevels( Format, SampleCount, pNumQualityLevels );
}

void D3D11DeviceProxy::CheckCounterInfo( 
	/* [annotation] */ 
	__out  D3D11_COUNTER_INFO *pCounterInfo)
{
	m_pD3DDevice->CheckCounterInfo( pCounterInfo );
}

HRESULT D3D11DeviceProxy::CheckCounter( 
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
	__inout_opt  UINT *pDescriptionLength)
{
	return m_pD3DDevice->CheckCounter(
		pDesc,
		pType,
		pActiveCounters,
		szName,
		pNameLength,
		szUnits,
		pUnitsLength,
		szDescription,
		pDescriptionLength );
}

HRESULT D3D11DeviceProxy::CheckFeatureSupport( 
	D3D11_FEATURE Feature,
	/* [annotation] */ 
	__out_bcount(FeatureSupportDataSize)  void *pFeatureSupportData,
	UINT FeatureSupportDataSize)
{
	return m_pD3DDevice->CheckFeatureSupport( Feature, pFeatureSupportData, FeatureSupportDataSize );
}

HRESULT D3D11DeviceProxy::GetPrivateData( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__inout  UINT *pDataSize,
	/* [annotation] */ 
	__out_bcount_opt(*pDataSize)  void *pData)
{
	return m_pD3DDevice->GetPrivateData( guid, pDataSize, pData );
}

HRESULT D3D11DeviceProxy::SetPrivateData( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__in  UINT DataSize,
	/* [annotation] */ 
	__in_bcount_opt(DataSize)  const void *pData)
{
	return m_pD3DDevice->SetPrivateData( guid, DataSize, pData );
}

HRESULT D3D11DeviceProxy::SetPrivateDataInterface( 
	/* [annotation] */ 
	__in  REFGUID guid,
	/* [annotation] */ 
	__in_opt  const IUnknown *pData)
{
	return m_pD3DDevice->SetPrivateDataInterface( guid, pData );
}

D3D_FEATURE_LEVEL D3D11DeviceProxy::GetFeatureLevel( void)
{
	return m_pD3DDevice->GetFeatureLevel();
}

UINT D3D11DeviceProxy::GetCreationFlags( void)
{
	return m_pD3DDevice->GetCreationFlags();
}

HRESULT D3D11DeviceProxy::GetDeviceRemovedReason( void)
{
	return m_pD3DDevice->GetDeviceRemovedReason();
}

void D3D11DeviceProxy::GetImmediateContext( 
	/* [annotation] */ 
	__out  ID3D11DeviceContext **ppImmediateContext)
{
	*ppImmediateContext = m_pImmediateContext;
}

HRESULT D3D11DeviceProxy::SetExceptionMode( 
	UINT RaiseFlags)
{
	return m_pD3DDevice->SetExceptionMode( RaiseFlags );
}

UINT D3D11DeviceProxy::GetExceptionMode( void)
{
	return m_pD3DDevice->GetExceptionMode();
}

//-------------------------------------------------------------------------------------------------------------//


GraphicsDevice::GraphicsDevice()
{

}
GraphicsDevice::~GraphicsDevice()
{

}

bool GraphicsDevice::Initialize()
{
	dxPtr< IDXGIFactory >	pDXGIFactory;
	dxPtr< IDXGIAdapter >	pDXGIAdapter;
	dxPtr< IDXGIOutput >	pDXGIOutput;

	VRET_FALSE_IF_NOT(SetupDXGI( pDXGIFactory, pDXGIAdapter, pDXGIOutput ));

	// Create a Direct3D device.

	dxPtr< ID3D11Device >			pD3DDevice;
	dxPtr< ID3D11DeviceContext >	pD3DDeviceContext;

	VRET_FALSE_IF_NOT(CreateDirect3DDevice( pDXGIAdapter, pD3DDevice, pD3DDeviceContext ));

	VRET_FALSE_IF_NOT(CheckFeatureLevel( pD3DDevice ));




	selectedFeatureFeatureLevel = pD3DDevice->GetFeatureLevel();



	const D3D_FEATURE_LEVEL forcedFeatureLevel = D3D_FEATURE_LEVEL_10_0;
	const bool bForceFeatureLevel = false;

	if(bForceFeatureLevel) {
		selectedFeatureFeatureLevel = forcedFeatureLevel;
	}


	LogDirect3DInfo( pD3DDevice );


	bool bDebugRenderer = false;
	gCore.config->GetBool("DebugRenderer",bDebugRenderer);

	if(bDebugRenderer)
	{
		static D3D11DebugDeviceContext	myDeviceContext( pD3DDeviceContext );
		static D3D11DeviceProxy	myDevice( pD3DDevice, &myDeviceContext );

		m_pD3DDeviceContext = &myDeviceContext;
		m_pD3DDevice = &myDevice;
	}
	else
	{
		m_pD3DDeviceContext = pD3DDeviceContext;
		m_pD3DDevice = pD3DDevice;
	}

	m_pDXGIFactory = pDXGIFactory;
	m_pDXGIOutput = pDXGIOutput;

	return true;
}

void GraphicsDevice::Shutdown()
{
	if( m_pD3DDeviceContext != nil )
	{
		// Send queued-up commands in the command buffer to the GPU.
		//m_pD3DDeviceContext->Flush();

		// Remove all resources from the device by setting their pointers to NULL.
		// This should be called during shutdown of the application.
		// It helps ensure that when one is releasing all of their resources
		// that none of them are bound to the device.
		RX_KLUDGE("error LNK2019: unresolved external symbol _D3DX11UnsetAllDeviceObjects@4");
		//D3DX11UnsetAllDeviceObjects( immediateContext );

		m_pD3DDeviceContext->ClearState();	// clear device state before deleting resources to avoid D3D warnings

		m_pD3DDeviceContext = nil;
	}


	if(0)// this causes access violation
	{
		dxPtr< ID3D11Debug >	d3dDebug;
		if( SUCCEEDED( m_pD3DDevice->QueryInterface( IID_ID3D11Debug, (void**)&d3dDebug.Ptr ) )
			&& (d3dDebug != nil) )
		{
			dxchk(d3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL ));
		}
	}



	// Release the device. NOTE: all D3D resources (shaders/states/etc) should have been released prior to this point.

	if( nil != m_pD3DDevice )
	{
		if(1)
		{
			FORCE_RELEASE_COM_INTERFACE( m_pD3DDevice.Ptr );
		}
		else
		{
			const ULONG refCount = m_pD3DDevice->Release();
			if( refCount != 0 ) {
				dxWarnf("D3D11 device hasn't been released!\n");
			}
			m_pD3DDevice.Ptr = nil;
		}

		m_pD3DDevice = nil;
	}

	m_pDXGIFactory = nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
