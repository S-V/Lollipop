/*
=============================================================================
	File:	D3D11Viewports.cpp
	Desc:	Direct3D 11 viewports.
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

#include "DX11Private.h"

namespace
{
	enum { target_FPS = 60 };
}//namespace

//-------------------------------------------------------------------------------------------------------------//
static bool FindSuitableRefreshRate(
	const dxPtr< IDXGIOutput >& inDXGIOutput,
	const UINT inBackbufferWidth, const UINT inBackbufferHeight,
	const DXGI_FORMAT inBackBufferFormat,
	DXGI_RATIONAL &outRefreshRate )
{
#if 0
	outRefreshRate.Numerator = target_FPS;	// 0 - auto
	outRefreshRate.Denominator = 1;
#else
	// Microsoft best practices advises this step.
	outRefreshRate.Numerator = 0;
	outRefreshRate.Denominator = 0;
#endif
	const UINT	flags = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING;

	UINT	numDisplayModes = 0;

	HRESULT	hr = inDXGIOutput->GetDisplayModeList(
		inBackBufferFormat,
		flags,
		&numDisplayModes,
		0
	);
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to get the number of display modes.\n" );
		return false;
	}

	enum { MAX_DISPLAY_MODES = 32 };

	DXGI_MODE_DESC	displayModes[ MAX_DISPLAY_MODES ];

	numDisplayModes = Min( numDisplayModes, ARRAY_SIZE(displayModes) );

	hr = inDXGIOutput->GetDisplayModeList(
		inBackBufferFormat,
		flags,
		&numDisplayModes,
		displayModes
	);
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to get display mode list\n" );
		return false;
	}

	dxMsgf( "List of supported (%d) display modes matching the color format '%s':\n",
			numDisplayModes, DXGIFormat_ToString( inBackBufferFormat ) );

	for(UINT iDisplayMode = 0; iDisplayMode < numDisplayModes; iDisplayMode++)
	{
		const DXGI_MODE_DESC& displayMode = displayModes[ iDisplayMode ];

		dxMsgf( "[%u]: %ux%u, %u Hz\n",
			iDisplayMode+1, displayMode.Width, displayMode.Height,
			displayMode.RefreshRate.Numerator / displayMode.RefreshRate.Denominator );

		if( displayMode.Width == inBackbufferWidth
			&& displayMode.Height == inBackbufferHeight )
		{
			outRefreshRate.Numerator = displayMode.RefreshRate.Numerator;
			outRefreshRate.Denominator = displayMode.RefreshRate.Denominator;
		}
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static bool FindSuitableDisplayMode(
	const dxPtr< IDXGIOutput >& inDXGIOutput,
	const UINT inBackbufferWidth, const UINT inBackbufferHeight,
	const DXGI_FORMAT inBackBufferFormat,
	const DXGI_RATIONAL& inRefreshRate,
	DXGI_MODE_DESC &outBufferDesc
	)
{
	DXGI_MODE_DESC	desiredDisplayMode;
	ZERO_OUT( desiredDisplayMode );

	desiredDisplayMode.Width	= inBackbufferWidth;
	desiredDisplayMode.Height 	= inBackbufferHeight;
	desiredDisplayMode.Format 	= inBackBufferFormat;
	desiredDisplayMode.RefreshRate = inRefreshRate;

	desiredDisplayMode.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desiredDisplayMode.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;

	const HRESULT hr = inDXGIOutput->FindClosestMatchingMode(
		&desiredDisplayMode,
		&outBufferDesc,
		NULL
	);
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to find a supported video mode.\n" );
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
UINT GetNumMSAASamples(
	const DXGI_FORMAT inBackBufferFormat,
	const dxPtr< ID3D11Device >& inD3DDevice
	)
{
	const bool	bMultisampleAntiAliasing = false;

	if( bMultisampleAntiAliasing )
	{
		UINT	numMsaaSamples = 32;

		// Get the number of quality levels available during multisampling.
		while( numMsaaSamples > 1 )
		{
			UINT	numQualityLevels = 0;
			if(SUCCEEDED( inD3DDevice->CheckMultisampleQualityLevels( inBackBufferFormat, numMsaaSamples, &numQualityLevels ) ))
			{
				if( numQualityLevels > 0 ) {
					// The format and sample count combination are supported for the installed adapter.
					break;
				}
			}
			numMsaaSamples /= 2;
		}

		dxMsgf("Multisample Anti-Aliasing: %d samples.\n",numMsaaSamples);

		return numMsaaSamples;
	}
	else
	{
		return 1;
	}
}
//-------------------------------------------------------------------------------------------------------------//
static void GetSwapChainDescription(
	const HWND inWindowHandle, const BOOL inWindowedMode,
	const UINT inBackbufferWidth, const UINT inBackbufferHeight,
	const DXGI_FORMAT inBackBufferFormat,
	const DXGI_RATIONAL& inRefreshRate,
	DXGI_SWAP_CHAIN_DESC &outSwapChainDesc
	)
{
	// for switching between windowed/fullscreen modes
	const bool	allowModeSwitch = true;

	const bool	bSampleBackBuffer = false;

	DXGI_USAGE	backBufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//backBufferUsage |= DXGI_USAGE_BACK_BUFFER;	//<- this line is optional

	if( bSampleBackBuffer ) {
		backBufferUsage |= DXGI_USAGE_SHADER_INPUT;
	}


	ZERO_OUT( outSwapChainDesc );

	outSwapChainDesc.BufferCount	= 1;

	outSwapChainDesc.BufferDesc.Width			= inBackbufferWidth;
	outSwapChainDesc.BufferDesc.Height 			= inBackbufferHeight;
	outSwapChainDesc.BufferDesc.Format 			= inBackBufferFormat;
	outSwapChainDesc.BufferDesc.RefreshRate		= inRefreshRate;
	outSwapChainDesc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	outSwapChainDesc.BufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;

	outSwapChainDesc.BufferUsage	= backBufferUsage;

	outSwapChainDesc.OutputWindow	= inWindowHandle;	// the window that the swap chain will use to present images on the screen

	outSwapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;	// the contents of the back buffer are discarded after calling IDXGISwapChain::Present()

	outSwapChainDesc.Windowed	= inWindowedMode;

	if( allowModeSwitch ) {
		outSwapChainDesc.Flags	= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// flag for switching between windowed/fullscreen modes
	}

#if 0
	FindSuitableDisplayMode(
		inDXGIOutput,
		inBackbufferWidth, inBackbufferHeight, inBackBufferFormat, inRefreshRate,
		sd.BufferDesc );
#endif

	outSwapChainDesc.SampleDesc.Count = GetNumMSAASamples( outSwapChainDesc.BufferDesc.Format, GetD3DDevice() );
	outSwapChainDesc.SampleDesc.Quality	= 0;
}
//-------------------------------------------------------------------------------------------------------------//
static bool CreateDirect3DSwapChain(
	DXGI_SWAP_CHAIN_DESC& sd,
	const dxPtr< IDXGIFactory >& inDXGIFactory,
	const dxPtr< ID3D11Device >& inD3DDevice,
	dxPtr< IDXGISwapChain > &outSwapChain
	)
{
	dxMsg("Creating a swap chain...\n");
	dxMsgf("Selected display mode: %ux%u\n",sd.BufferDesc.Width,sd.BufferDesc.Height);
	dxMsgf("Back buffer format: %s\n",DXGIFormat_ToString(sd.BufferDesc.Format));
	if( sd.BufferDesc.RefreshRate.Denominator != 0 ) {
		dxMsgf("Selected refresh rate: %u Hertz\n", sd.BufferDesc.RefreshRate.Numerator/sd.BufferDesc.RefreshRate.Denominator );
	} else {
		dxMsgf("Selected default refresh rate\n");
	}
	dxMsgf("Scanline ordering: %s\n",DXGI_ScanlineOrder_ToStr(sd.BufferDesc.ScanlineOrdering));
	dxMsgf("Scaling: %s\n",DXGI_ScalingMode_ToStr(sd.BufferDesc.Scaling));

	const HRESULT hr = inDXGIFactory->CreateSwapChain( inD3DDevice, &sd, &outSwapChain.Ptr );
	if( FAILED( hr ) ) {
		dxErrf( hr, "Failed to create a swap chain\n" );
		return false;
	}

/*
	An alternative way to create device and swap chain:

	hr = D3D11CreateDeviceAndSwapChain(
		pDXGIAdapter0,
		driverType,
		( HMODULE )null,	// HMODULE Software rasterizer
		createDeviceFlags,
		featureLevels,	// array of feature levels, null means 'get the greatest feature level available'
		ARRAY_SIZE(featureLevels),	// numFeatureLevels
		D3D11_SDK_VERSION,
		&swapChainInfo,
		&swapChain.Ptr,
		&device.Ptr,
		&selectedFeatureLevel,
		&immediateContext.Ptr
	);
	if( FAILED( hr ) ) {
		dxErrf( hr,
			"Failed to create device and swap chain.\n"
			"This application requires a Direct3D 11 class device "
			"running on Windows Vista (or later)\n" );
		RETURN_ON_ERROR;
	}
*/

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static bool DisableAltEnter( const HWND inWindowHandle,
							const dxPtr< IDXGIFactory >& inDXGIFactory )
{
	// Prevent DXGI from monitoring message queue for the Alt-Enter key sequence
	// (which causes the application to switch from windowed to fullscreen or vice versa).
	// IDXGIFactory::MakeWindowAssociation is recommended,
	// because a standard control mechanism for the user is strongly desired.

	const HRESULT hr = inDXGIFactory->MakeWindowAssociation(
		inWindowHandle,
		DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER /*| DXGI_MWA_NO_PRINT_SCREEN*/
	);
	if( FAILED( hr ) ) {
		dxWarnf( hr, "MakeWindowAssociation() failed\n" );
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static void SetDebugNames( RenderTarget & theMainRenderTarget, DepthStencil & theMainDepthStencil )
{
	dxDbgSetName(theMainRenderTarget.pTexture,"Back buffer texture");
	dxDbgSetName(theMainRenderTarget.pRTV,"Back buffer render target view");
	dxDbgSetName(theMainRenderTarget.pSRV,"Back buffer shader resource view");

	dxDbgSetName(theMainDepthStencil.pTexture,"Depth-stencil texture");
	dxDbgSetName(theMainDepthStencil.pDSV,"Depth-stencil view");
	dxDbgSetName(theMainDepthStencil.pSRV,"Depth-stencil shader resource view");
}
//-------------------------------------------------------------------------------------------------------------//
static bool CreateBackBuffer( const dxPtr< ID3D11Device >& inD3DDevice,
							 const dxPtr< IDXGISwapChain >& inDXGISwapChain,
							 const UINT inBackbufferWidth, const UINT inBackbufferHeight,
							 const bool bDepthStencil,
							 RenderTarget &outMainRenderTarget, DepthStencil &outMainDepthStencil )
{
	const bool bSampleBackBuffer = false;

	// The amount of memory occupied by the backbuffer and depth-stencil.
	UINT frameBufferSize = 0;


	DXGI_SWAP_CHAIN_DESC	swapChainInfo;
	inDXGISwapChain->GetDesc( &swapChainInfo );

	UINT	backbufferWidth = inBackbufferWidth;
	UINT	backbufferHeight = inBackbufferHeight;

	RenderTarget& mainRenderTarget = outMainRenderTarget;
	DepthStencil& mainDepthStencil = outMainDepthStencil;

	//
	//	Initialize back buffer. Grab back buffer texture and create main depth-stencil texture.
	//
	{
		// Get a pointer to the back buffer.

		dxchk( inDXGISwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &mainRenderTarget.pTexture.Ptr ) ));
		
		// Create a render-target view for accessing the back buffer.

		dxchk( inD3DDevice->CreateRenderTargetView(  mainRenderTarget.pTexture, nil, & mainRenderTarget.pRTV.Ptr ));

		if( bSampleBackBuffer ) {
			dxchk( inD3DDevice->CreateShaderResourceView( mainRenderTarget.pTexture, nil, & mainRenderTarget.pSRV.Ptr ) );
		}

		// Create depth-stencil.

		if( bDepthStencil )
		{
			D3D11_TEXTURE2D_DESC  desc;
			desc.Width				= backbufferWidth;
			desc.Height				= backbufferHeight;
			desc.MipLevels 			= 1;
			desc.ArraySize 			= 1;
			desc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc			= swapChainInfo.SampleDesc;
			desc.Usage				= D3D11_USAGE_DEFAULT;
			desc.BindFlags 			= D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags		= 0;
			desc.MiscFlags			= 0;
			dxchk( inD3DDevice->CreateTexture2D( &desc, nil, & mainDepthStencil.pTexture.Ptr ));

			D3D11_DEPTH_STENCIL_VIEW_DESC	descDSV;
			descDSV.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDSV.ViewDimension		= (swapChainInfo.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Flags				= 0;	//(D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL) can only be specified in D3D11 or later
			descDSV.Texture2D.MipSlice	= 0;
			dxchk( inD3DDevice->CreateDepthStencilView( mainDepthStencil.pTexture, &descDSV, & mainDepthStencil.pDSV.Ptr ));

			if( bDepthStencil )
			{
				frameBufferSize += (backbufferWidth * backbufferHeight) * DXGIFormat_GetElementSize(descDSV.Format);
			}
		}

		// Calculate the amount of memory occupied by the backbuffer and depth-stencil.

		D3D11_TEXTURE2D_DESC  backbufferTexDesc;
		mainRenderTarget.pTexture->GetDesc(&backbufferTexDesc);

		frameBufferSize += (backbufferWidth * backbufferHeight) * DXGIFormat_GetElementSize(backbufferTexDesc.Format);
	}

	SetDebugNames( outMainRenderTarget, outMainDepthStencil );

	dxMsgf("Frame buffer size(%ux%u): %u Mb\n", backbufferWidth, backbufferHeight, frameBufferSize / (1024*1024) );

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
bool ReadDirect3DSettings(
	const dxPtr< IDXGISwapChain >& inSwapChain,
	UINT &outWindowWidth, UINT &outWindowHeight )
{
	HRESULT hr = E_FAIL;

	DXGI_SWAP_CHAIN_DESC	desc;

	hr = inSwapChain->GetDesc( &desc );

	if( FAILED( hr ) ) {
		dxWarnf( hr, "IDXGISwapChain::GetDesc() failed\n" );
		return false;
	}

	outWindowWidth = desc.BufferDesc.Width;
	outWindowHeight = desc.BufferDesc.Height;

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
static bool SetFullScreenMode(
	const dxPtr< IDXGISwapChain >& inSwapChain,
	const dxPtr< IDXGIOutput >& inDXGIOutput
	)
{
	HRESULT hr = E_FAIL;

	// Set the display mode to full-screen.

	hr = inSwapChain->SetFullscreenState( TRUE, inDXGIOutput );
	if( FAILED( hr ) ) {
		dxWarnf( hr, "SetFullscreenState() failed\n" );
		return false;
	}

	hr = inSwapChain->ResizeBuffers(
		0,	// UINT BufferCount
		0,	// UINT Width
		0,	// UINT Height
		DXGI_FORMAT_UNKNOWN,	// DXGI_FORMAT NewFormat
		0	// UINT SwapChainFlags
	);
	if( FAILED( hr ) ) {
		dxWarnf( hr, "ResizeBuffers() failed\n" );
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
void D3D_DefaultSwapChainDesc( WindowHandle	hWnd, DXGI_SWAP_CHAIN_DESC &desc )
{
	const HWND	windowHandle = hWnd;
	const BOOL	isWindowed = true;

	UINT	windowWidth = 0;
	UINT	windowHeight = 0;
	Win_GetWindowSize( windowHandle, windowWidth, windowHeight );

	Assert(windowWidth > 0);
	Assert(windowHeight > 0);

	mxUNDONE
	const DXGI_FORMAT	backBufferFormat =
		DXGI_FORMAT_R8G8B8A8_UNORM
		//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
		;

	DXGI_RATIONAL refreshRate;
#if 0
	refreshRate.Numerator = target_FPS;
	refreshRate.Denominator = 1;
#else
	// Microsoft best practices advises this step.
	refreshRate.Numerator = 0;
	refreshRate.Denominator = 0;
#endif

	GetSwapChainDescription(
		windowHandle, isWindowed,
		windowWidth, windowHeight, backBufferFormat,
		refreshRate,
		desc
	);

	// Set this flag to enable an application to render using GDI on a swap chain or a surface.
	// This will allow the application to call IDXGISurface1::GetDC on the 0th back buffer or a surface.
	//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
}
//-------------------------------------------------------------------------------------------------------------//
bool D3D_InitializeViewport( DXGI_SWAP_CHAIN_DESC& desc,
							const bool bDepthStencil,
							GrViewport &viewport )
{
	viewport.Release();

	dxPtr< IDXGIFactory >	pDXGIFactory = GetDXGIFactory();
	dxPtr< ID3D11Device >	pD3DDevice = GetD3DDevice();
	dxPtr< IDXGIOutput >	pDXGIOutput = GetDXGIOutput();

	// Create a swap chain.

	dxPtr< IDXGISwapChain >		pDXGISwapChain;

	VRET_FALSE_IF_NOT(CreateDirect3DSwapChain(
		desc,
		pDXGIFactory, pD3DDevice,
		pDXGISwapChain
	));

	//DisableAltEnter( desc.OutputWindow, pDXGIFactory );

	if( desc.Windowed == FALSE )
	{
		SetFullScreenMode( pDXGISwapChain, pDXGIOutput );
	}

	const UINT	windowWidth = desc.BufferDesc.Width;
	const UINT	windowHeight = desc.BufferDesc.Height;

	VRET_FALSE_IF_NOT(CreateBackBuffer(
		pD3DDevice, pDXGISwapChain,
		windowWidth, windowHeight,
		bDepthStencil,
		viewport.mainRT,
		viewport.mainDS
	));

	viewport.d.Width	= (FLOAT)windowWidth;
	viewport.d.Height	= (FLOAT)windowHeight;
	viewport.d.MinDepth	= 0.0f;
	viewport.d.MaxDepth	= 1.0f;
	viewport.d.TopLeftX	= 0.0f;
	viewport.d.TopLeftY	= 0.0f;

	viewport.swapChain = pDXGISwapChain;

	return true;
}
//-------------------------------------------------------------------------------------------------------------//
bool D3D_ResizeViewport( GrViewport &viewport, UINT newWidth, UINT newHeight, bool bFullScreen )
{
	AssertX( !bFullScreen, "Not impl" );

#if 0
	const BOOL isFullScreen = viewport.IsFullScreen();

	if( isFullScreen )
	{
		width = GetSystemMetrics( SM_CXSCREEN );
		height = GetSystemMetrics( SM_CYSCREEN );
	}
#endif

	DXGI_SWAP_CHAIN_DESC	sd;
	viewport.swapChain->GetDesc( &sd );

	D3D11_TEXTURE2D_DESC	descDepthStencil;
	viewport.mainDS.pTexture->GetDesc( &descDepthStencil );

	D3D11_DEPTH_STENCIL_VIEW_DESC	descDSV;
	viewport.mainDS.pDSV->GetDesc( &descDSV );


	viewport.mainRT.Release();
	viewport.mainDS.Release();


	//	A swapchain cannot be resized unless all outstanding references
	//	to its back buffers have been released.
	//	The application must release all of its direct and indirect references
	//	on the backbuffers in order for ResizeBuffers to succeed.
	//	Direct references are held by the application after calling AddRef on a resource.
	//	Indirect references are held by views to a resource,
	//	binding a view of the resource to a device context,
	//	a command list that used the resource,
	//	a command list that used a view to that resource,
	//	a command list that executed another command list that used the resource, etc.

	//DbgPutNumRefs(viewport.swapChain.Ptr);	//should be 2

	//	Before calling ResizeBuffers,
	//	ensure that the application releases all references
	//	(by calling the appropriate number of Release invocations) on the resources,
	//	any views to the resource, any command lists that use either the resources or views,
	//	and ensure that neither the resource, nor a view is still bound to a device context.
	//	ClearState can be used to ensure this.
	//	If a view is bound to a deferred context,
	//	then the partially built command list must be discarded as well
	//	(by calling ClearState, FinishCommandList, then Release on the command list).

	if(FAILED( viewport.swapChain->ResizeBuffers(
		sd.BufferCount,
		newWidth,
		newHeight,
		sd.BufferDesc.Format,
		0	// UINT SwapChainFlags
	)))
	{
		mxDEBUG_BREAK;
		dxWarnf("ResizeBuffers() failed.\n");
		return false;
	}

	// The application can re-query interfaces after calling ResizeBuffers
	// via IDXGISwapChain::GetBuffer.

	// Recreate render target.
	{
		dxchk(viewport.swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), c_cast(void**) &viewport.mainRT.pTexture.Ptr ));
		dxchk(D3DDevice->CreateRenderTargetView( viewport.mainRT.pTexture, nil, &viewport.mainRT.pRTV.Ptr ));
	}

	// Recreate depth-stencil.
	{
		descDepthStencil.Width = newWidth;
		descDepthStencil.Height = newHeight;

		dxchk(D3DDevice->CreateTexture2D( &descDepthStencil, nil, &viewport.mainDS.pTexture.Ptr ));
		dxchk(D3DDevice->CreateDepthStencilView( viewport.mainDS.pTexture, &descDSV, &viewport.mainDS.pDSV.Ptr ));
	}

	SetDebugNames( viewport.mainRT,  viewport.mainDS );

	viewport.d.Width = newWidth;
	viewport.d.Height = newHeight;

	return true;
}

//-------------------------------------------------------------------------------------------------------------//

GrViewport::GrViewport()
{
	Release();
}

GrViewport::~GrViewport()
{
	Release();
}

void GrViewport::Release()
{
	d.Width		= 0.0f;
	d.Height	= 0.0f;
	d.MinDepth	= 0.0f;
	d.MaxDepth	= 1.0f;
	d.TopLeftX	= 0.0f;
	d.TopLeftY	= 0.0f;

	mainRT.Release();
	mainDS.Release();

	swapChain = nil;
}

bool GrViewport::IsNull() const
{
	return 1
		&& swapChain.IsNull()
		;
}

bool GrViewport::IsValid() const
{
	return !this->IsNull();
}

bool GrViewport::CheckIsOk() const
{
	if( !mainDS.IsNull())
	{
		// DepthStencilViews may only be used with RenderTargetViews
		// if the effective dimensions of the Views are equal,
		// as well as the Resource types, multisample count, and multisample quality.

		//UINT	textureWidth, textureHeight;

		UINT	rtW, rtH;
		UINT	dsW, dsH;
		mainRT.CalcWidthHeight( rtW, rtH );
		mainDS.CalcWidthHeight( dsW, dsH );
		return 1
			&& !IsNull()
			&& (rtW == dsW)
			&& (rtH == dsH)
			;
	}

	return 1
		&& !IsNull()
		;
}

BOOL GrViewport::IsFullScreen() const
{
	BOOL isFullScreen;
	dxPtr<IDXGIOutput>	pIDXGIOutput;
	if(FAILED( swapChain->GetFullscreenState( &isFullScreen, &pIDXGIOutput.Ptr )))
	{
		dxWarnf("GetFullscreenState() failed.\n");
		return FALSE;
	}
	return isFullScreen;
}

void GrViewport::Set( ID3D11DeviceContext* theContext ) const
{
	Assert(!this->IsNull());
	// set main render target and depth-stencil
	theContext->OMSetRenderTargets( 1, &mainRT.pRTV.Ptr, mainDS.pDSV.Ptr );
	theContext->RSSetViewports( 1, &d );
}

//bool GrViewport::isOk() const
//{
//	return minZ < maxZ
//		&& minZ >= 0.0f
//		&& maxZ <= 1.0f
//		;
//}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
