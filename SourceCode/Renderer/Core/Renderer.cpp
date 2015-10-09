/*
=============================================================================
	File:	Renderer.cpp
	Desc:	High level render system implementation.
	ToDo:	support for multiple shader libraries
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

// for GetCurrentDateString()
#include <Base/Text/TextUtils.h>

#include <Core/Core.h>
#include <Core/Util/Tweakable.h>

#include <Graphics/DX11/DX11Private.h>

#include <Renderer/Renderer.h>
#include <Renderer/Core/Font.h>
#include <Renderer/Core/Texture.h>
#include <Renderer/Core/Material.h>
#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>
#include <Renderer/Util/BatchRenderer.h>
#include <Renderer/Scene/SpatialDatabase.h>
#include <Renderer/Scene/Model.h>
#include <Renderer/Pipeline/Backend.h>

#include <Renderer/Materials/SubsurfaceScattering.h>

mxDEFINE_ABSTRACT_CLASS(ATexturePreview);

AHitTesting	AHitTesting::NullObject;

void ATexturePreview::GetSize( UINT &OutWidth, UINT &OutHeight )
{
	Unreachable;
}
PCSTR ATexturePreview::GetName() const
{
	Unreachable;
	return "";
}
ID3D11ShaderResourceView* ATexturePreview::GetSRV()
{
	Unreachable;
	return nil;
}

void ATextureInspector::AddDebugItem( ID3D11Texture2D* pTexture, PCSTR name )
{
}
void ATextureInspector::RemoveDebugItem( ID3D11Texture2D* pTexture )
{
}
ATexturePreview* ATextureInspector::GetTextureByName( PCSTR name )
{
	Unreachable;
	return nil;
}

void EdGraphics::InitHitTesting( UINT viewportWidth, UINT viewportHeight )
{
}
void EdGraphics::CloseHitTesting()
{
}
ATextureInspector* EdGraphics::GetTextureInspector()
{
	static ATextureInspector dummy;
	return &dummy;
}

//---------------------------------------------------------------------------

// used as an optimization when resizing multiple viewports

struct rxViewportResizeCmd
{
	rxViewport *	vp;
	UINT			width;
	UINT			height;
	UINT			time;	// relative time, in milliseconds
};

typedef TStaticList_InitZeroed
<
	rxViewportResizeCmd,
	RX_MAX_VIEWPORTS
>
rxViewportResizeCmdList;

// batches up resize commands
//
class ResizeViewportsUtil
{
	rxViewportResizeCmdList	viewportsToResize;

	UINT	lastTimeViewportAdded;	// in milliseconds
	UINT	delayInMilliSeconds;	// in milliseconds

public:
	ResizeViewportsUtil()
	{
		lastTimeViewportAdded = 0;
		delayInMilliSeconds = 2000;
	}
	UINT CurrentTimeMilliseconds() const
	{
		return mxGetMilliseconds();
	}
	void Add( rxViewport* viewport, UINT newWidth, UINT newHeight )
	{
		rxViewportResizeCmd* newCmd = nil;

		for( UINT iViewport = 0; iViewport < viewportsToResize.Num(); iViewport++ )
		{
			rxViewportResizeCmd & cmd = viewportsToResize[ iViewport ];
			if ( cmd.vp == viewport )
			{
				newCmd = &cmd;
				break;
			}
		}

		if( newCmd == nil )
		{
			newCmd = &viewportsToResize.AddFast_Unsafe();
		}

		newCmd->vp = viewport;
		newCmd->width = newWidth;
		newCmd->height = newHeight;
		newCmd->time = CurrentTimeMilliseconds();

		lastTimeViewportAdded = CurrentTimeMilliseconds();
	}
	void Remove( rxViewport* viewport )
	{
		for( UINT iViewport = 0; iViewport < viewportsToResize.Num(); iViewport++ )
		{
			rxViewportResizeCmd & cmd = viewportsToResize[ iViewport ];
			if ( cmd.vp == viewport )
			{
				viewportsToResize.RemoveAt( iViewport );
			}
		}
	}
	bool ShouldResizeViewports() const
	{
		return (viewportsToResize.Num() > 0)
			&& (CurrentTimeMilliseconds() - lastTimeViewportAdded >= delayInMilliSeconds)
			;
	}
	void Clear()
	{
		viewportsToResize.Empty();
	}
	rxViewportResizeCmd* ToPtr()
	{
		return viewportsToResize.ToPtr();
	}
	UINT Num() const
	{
		return viewportsToResize.Num();
	}
};

//---------------------------------------------------------------------------

namespace
{
	struct RendererData : SetupCoreUtil
	{
		BatchRenderer	drawHelper;

		String PATH_TO_SHADER_SRC;
		String PATH_TO_SHADER_CACHE;

	public:
		RendererData()
		{
			PATH_TO_SHADER_SRC = "HLSL/";
			PATH_TO_SHADER_CACHE = "Cache/";

			gCore.config->GetString("PathToShaderSource",PATH_TO_SHADER_SRC);
			F_NormalizePath(PATH_TO_SHADER_SRC);
			gCore.config->GetString("PathToShaderCache",PATH_TO_SHADER_CACHE);
			F_NormalizePath(PATH_TO_SHADER_CACHE);
		}
		~RendererData()
		{
		}
	};

	TPtr< rxViewport >	gCurrentViewport;

	NiftyCounter			gNumViewports;
	TPtr< RendererData >	gData;

	ResizeViewportsUtil	gResizeViewportsUtil;
	static const bool RX_DEFER_VIEWPORTS_RESIZING = true;

}//namespace

//---------------------------------------------------------------------------

static bool InBeginEnd()
{
	return gCurrentViewport != nil;
}

static void ValidateViewportSize( UINT & width, UINT & height )
{
	Assert(width > 0 && width <= RX_MAX_RENDER_TARGET_SIZE);
	Assert(height > 0 && height <= RX_MAX_RENDER_TARGET_SIZE);

	enum { MIN_VIEWPORT_SIZE = 256/4 };

	Assert(IsInRangeInc<UINT>( width, MIN_VIEWPORT_SIZE, RX_MAX_RENDER_TARGET_SIZE ));
	Assert(IsInRangeInc<UINT>( height, MIN_VIEWPORT_SIZE, RX_MAX_RENDER_TARGET_SIZE ));

	width = Clamp<UINT>( width, MIN_VIEWPORT_SIZE, RX_MAX_RENDER_TARGET_SIZE );
	height = Clamp<UINT>( height, MIN_VIEWPORT_SIZE, RX_MAX_RENDER_TARGET_SIZE );
}


/*
--------------------------------------------------------------
	Renderer
--------------------------------------------------------------
*/

rxRenderer gRenderer;

rxRenderer::rxRenderer()
{

}

rxRenderer::~rxRenderer()
{

}

static void InitializeRenderSystem( UINT viewportWidth, UINT viewportHeight )
{
	// register classes
	{
		UINT dummy = 0;
		dummy += Material_SSS::StaticClass().GetTypeGuid();
		(void)dummy;
	}

	// at this point, you can perform test runs, determine machine capabilities,
	// calculate optimal batch size, select optimal algorithms, etc.

#if MX_DEVELOPER
	DEVOUT("InitializeRenderSystem():\n");
	DEVOUT("sizeof rxMesh = %u\n", (UINT)sizeof rxMesh );
	DEVOUT("sizeof rxModel = %u\n", (UINT)sizeof rxModel );
	DEVOUT("sizeof rxSurface = %u\n", (UINT)sizeof rxSurface );
	DEVOUT("OFFSET_OF(BatchSortKey64,material) = %u\n", (UINT)OFFSET_OF(BatchSortKey64,material) );
	DEVOUT("sizeof rxRenderQueue = %u\n", (UINT)sizeof rxRenderQueue );
#endif // MX_DEVELOPER


	gData.ConstructInPlace();

	// Setup the low-level graphics device.

	graphics.Initialize();

	// Initialize debugger first because it's used by the shader libraries.
#if MX_EDITOR
	Assert( gRenderer.editor != nil );
#endif // MX_EDITOR

	// Load the core shader library.

	// Create render targets, states and input layouts, register all shaders.
	{
		GrShaderLibraryContext	context;
		context.backBufferWidth = viewportWidth;
		context.backBufferHeight = viewportHeight;

		GPU::Initialize( context );
	}


	// Compile all shaders.
	{
		GrShaderCacheInfo	settings;
		settings.pathToShaderSource = gData->PATH_TO_SHADER_SRC;
		settings.pathToShaderCache = gData->PATH_TO_SHADER_CACHE;
		settings.timeStamp = GPU::TIME_STAMP;

		graphics.shaders->CompileShaders( settings );
	}


#if MX_EDITOR
	Assert( gRenderer.editor != nil );
	gRenderer.editor->InitHitTesting( viewportWidth, viewportHeight );
#endif // MX_EDITOR


	gRenderer.textures.ConstructInPlace();
	gRenderer.materials.ConstructInPlace();
	gRenderer.meshes.ConstructInPlace();

	// Setup rendering pipeline.
	Pipeline_Init( viewportWidth, viewportHeight );

	gData->drawHelper.Initialize();


	DbgFont_Setup();


	gRenderer.OnInitialize();


	HOT_BOOL(g_cvar_debug_draw_spatial_database);
}

static void ShutdownRenderSystem()
{
	Assert( gCurrentViewport == nil );

/*
	DXDbgOut("\n-------- Stat counters -------------------------\n");
	DXDbgOut("Total time: %u seconds",			totalElapsedTimeMS/1000);
	DXDbgOut("Total frames: %u",				gStats.frameCount);
	DXDbgOut("Average FPS: %u",					averageFPS);
	DXDbgOut("VFC/AABB: %u cycles",				gStats.cull_aabb_cycles);
	DXDbgOut("VFC/Sphere: %u cycles",			gStats.cull_sphere_cycles);
	DXDbgOut("Render queue sorting: %u cycles",	gStats.render_queue_sort_cycles);
	DXDbgOut("Filling MRTs: %u cycles",			gStats.fat_buffer_filling_cycles);
	DXDbgOut("Render to SM: %u cycles",			gStats.render_to_shadow_map_cycles);
	DXDbgOut("Render queue size: %u bytes",		sizeof(dxRenderQueue));
	DXDbgOut("sizeof(dxRenderer): %u bytes",	sizeof(dxRenderer));
*/

	DbgFont_Close();


	Pipeline_Close();

	gData->drawHelper.Shutdown();


	gRenderer.meshes.Destruct();
	gRenderer.materials.Destruct();
	gRenderer.textures.Destruct();


	graphics.shaders->SaveCompiledShaders( gData->PATH_TO_SHADER_CACHE );

	GPU::Shutdown();

#if MX_EDITOR
	gRenderer.editor->CloseHitTesting();
#endif // MX_EDITOR

	graphics.Shutdown();

	gData.Destruct();

	gRenderer.OnShutdown();

	{
		gRenderer.OnInitialize.Clear();
		gRenderer.OnShutdown.Clear();
		gRenderer.BeforeMainViewportResized.Clear();
		gRenderer.AfterMainViewportResized.Clear();
		gRenderer.BeforeMainViewportDestroyed.Clear();
	}
}

bool rxRenderer::CreateViewport( const rxViewportConfig& config, rxViewport &OutViewport )
{
	Assert( gNumViewports.NumRefs() < RX_MAX_VIEWPORTS );
	Assert(OutViewport.IsNull());
	Assert( OutViewport.uniqueId == INDEX_NONE );
	Assert( gCurrentViewport == nil );

	if( gNumViewports.NumRefs() >= RX_MAX_VIEWPORTS ) {
		dxErrf("Failed to create a new viewport.\n");
		return false;
	}

	UINT	windowWidth = 0;
	UINT	windowHeight = 0;
	Win_GetWindowSize( config.windowHandle, windowWidth, windowHeight );

	ValidateViewportSize( windowWidth, windowHeight );

	const UINT newViewportIndex = gNumViewports.NumRefs();
	const bool bIsMainViewport = (newViewportIndex == RX_MAIN_VIEWPORT_ID);

	DBGNEWLINE();
	if( bIsMainViewport ) {
		DBGOUT( "Creating the main viewport: %ux%u.\n", windowWidth, windowHeight );
	} else {
		DBGOUT( "Creating viewport [%u]: %ux%u.\n", newViewportIndex, windowWidth, windowHeight );
	}

	// check if we need to initialize for the first time
	if( gNumViewports.IncRef() )
	{
		InitializeRenderSystem( windowWidth, windowHeight );
	}

	DXGI_SWAP_CHAIN_DESC	spawnChainDesc;
	D3D_DefaultSwapChainDesc( config.windowHandle, spawnChainDesc );

	VRET_FALSE_IF_NOT(D3D_InitializeViewport( spawnChainDesc, config.bDepthStencil, OutViewport ));

	Assert(!OutViewport.IsNull());

	OutViewport.uniqueId = newViewportIndex;

	return true;
}

void rxRenderer::DestroyViewport( rxViewport &viewport )
{
	Assert( gCurrentViewport == nil );

	Assert( viewport.uniqueId != INDEX_NONE );

	if( viewport.IsNull() )
	{
		return;
	}

	const bool bIsMainViewport = this->IsMainViewport( viewport );

	if( bIsMainViewport )
	{
		this->BeforeMainViewportDestroyed();
	}

	if(MX_DEBUG)
	{
		UINT	windowWidth = viewport.GetWidth();
		UINT	windowHeight = viewport.GetHeight();
		DBGNEWLINE();
		if( bIsMainViewport ) {
			DBGOUT( "Destroying the main viewport: %ux%u.\n", windowWidth, windowHeight );
		} else {
			DBGOUT( "Destroying viewport: %ux%u.\n", windowWidth, windowHeight );
		}
	}

	viewport.Release();
	viewport.uniqueId = INDEX_NONE;

	if( gNumViewports.DecRef() )
	{
		ShutdownRenderSystem();
	}
}

static void ResizeSingleViewport( rxViewport& viewport, UINT newWidth, UINT newHeight )
{
	const UINT oldWidth = viewport.GetWidth();
	const UINT oldHeight = viewport.GetHeight();

	if( oldWidth == newWidth && oldHeight == newHeight )
	{
		return;
	}

	ValidateViewportSize( newWidth, newHeight );

	const bool bIsMainViewport = gRenderer.IsMainViewport( viewport );

	if( bIsMainViewport )
	{
		gRenderer.BeforeMainViewportResized( newWidth, newHeight );

		Pipeline_ReleaseBuffers();
	}

	DBGNEWLINE();
	if( bIsMainViewport ) {
		DBGOUT( "Resizing the main viewport: %ux%u -> %ux%u.\n", oldWidth, oldHeight, newWidth, newHeight );
	} else {
		DBGOUT( "Resizing viewport: %ux%u -> %ux%u.\n", oldWidth, oldHeight, newWidth, newHeight );
	}

	bool bFullScreen = false;
	mxENSURE(D3D_ResizeViewport( viewport, newWidth, newHeight, bFullScreen ));

	if( bIsMainViewport )
	{
		Pipeline_ResizeBuffers( newWidth, newHeight );

		gRenderer.AfterMainViewportResized( newWidth, newHeight );
	}
}

struct rxViewportManager
{
	static void ResizeViewports()
	{
		rxViewportResizeCmd* cmds = gResizeViewportsUtil.ToPtr();
		const UINT numCmds = gResizeViewportsUtil.Num();

		for( UINT i = 0; i < numCmds; i++ )
		{
			rxViewportResizeCmd& cmd = cmds[i];
			rxViewport& viewport = *cmd.vp;

			const UINT newWidth = cmd.width;
			const UINT newHeight = cmd.height;

			ResizeSingleViewport( viewport, newWidth, newHeight );
		}

		gResizeViewportsUtil.Clear();
	}
};//rxViewportManager

void rxRenderer::OnViewportResized( rxViewport& viewport, UINT newWidth, UINT newHeight )
{
	if( RX_DEFER_VIEWPORTS_RESIZING )
	{
		gResizeViewportsUtil.Add( &viewport, newWidth, newHeight );
	}
	else
	{
		ResizeSingleViewport( viewport, newWidth, newHeight );

	} // !RX_DEFER_VIEWPORTS_RESIZING
}

bool rxRenderer::IsMainViewport( const rxViewport &viewport ) const
{
	return viewport.uniqueId == RX_MAIN_VIEWPORT_ID;
}

void rxRenderer::BeginScene( rxViewport* viewport, const rxFrameContext& frameContext )
{
	Assert( gCurrentViewport == nil );
	AssertPtr( viewport );

	gCurrentViewport = viewport;

	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	const GrViewport & vp = *viewport;

	Assert(vp.CheckIsOk());

	pD3DContext->ClearRenderTargetView(
		vp.mainRT.pRTV,
		frameContext.clearColor.ToFloatPtr()
	);

	// Depth-stencil is optional.
	if( vp.mainDS.pDSV != nil )
	{
		pD3DContext->ClearDepthStencilView(
			vp.mainDS.pDSV,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,	// depth buffer value
			0	// UINT8 stencil value
		);
	}
}

void rxRenderer::DrawScene( const rxSceneContext& sceneContext )
{
	Assert( gCurrentViewport != nil );
	Pipeline_RenderScene( gCurrentViewport, sceneContext );
}

void rxRenderer::EndScene()
{
	Assert( gCurrentViewport != nil );

	gData->drawHelper.Flush();

	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	gCurrentViewport->swapChain->Present(
		0,	// UINT SyncInterval
		0	// UINT Flags
	);

	pD3DContext->ClearState();

	gCurrentViewport = nil;


	// Resize viewports if needed.

	if( RX_DEFER_VIEWPORTS_RESIZING )
	{
		if( gResizeViewportsUtil.ShouldResizeViewports() )
		{
			rxViewportManager::ResizeViewports();
		}
	}//RX_DEFER_VIEWPORTS_RESIZING
}

BatchRenderer& rxRenderer::GetDrawHelper()
{
	return gData->drawHelper;
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
