/*
=============================================================================
	File:	GraphicsSystem.cpp
	Desc:	Low-level graphics system, implementation.
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

#include "DX11Private.h"


GraphicsSystem graphics;

static NiftyCounter		gLowLevelGraphicsInitCounter;
static TBlob16<D3D11ResourceSystem>	gD3D11ResourceSystem;
static TBlob16<D3D11ShaderSystem>	gD3D11ShaderSystem;

GraphicsSystem::GraphicsSystem()
{
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::Initialize()
{
	if( gLowLevelGraphicsInitCounter.IncRef() )
	{
		Initializable::OneTimeInit();

#if MX_DEVELOPER
		{
			DevBuildOptionsList	buildOptions;

			if( RX_DEBUG_RENDERER ) {
				buildOptions.Add("RX_DEBUG_RENDERER");
			}
			if( RX_DEBUG_SHADERS ) {
				buildOptions.Add("RX_DEBUG_SHADERS");
			}
			if( RX_PROFILE ) {
				buildOptions.Add("RX_PROFILE");
			}
			if( RX_D3D_USE_PERF_HUD ) {
				buildOptions.Add("RX_D3D_USE_PERF_HUD");
			}
			if( RX_USE_PROXY_DEVICE_CONTEXT ) {
				buildOptions.Add("RX_USE_PROXY_DEVICE_CONTEXT");
			}

			if( RX_HW_D3D11 ) {
				buildOptions.Add("RX_HW_D3D11");
			}

			ANSICHAR	buildOptionsStr[ MAX_STRING_CHARS ];
			buildOptions.ToChars( buildOptionsStr, NUMBER_OF(buildOptionsStr) );

			DEVOUT("Graphics library build settings:\n\t%s\n", buildOptionsStr);
		}


		DEVOUT( "sizeof GrIndexBuffer = %u\n", UINT(sizeof GrIndexBuffer) );
		DEVOUT( "sizeof GrVertexData = %u\n", UINT(sizeof GrVertexData) );
#endif // MX_DEVELOPER


		VRET_FALSE_IF_NOT( device.Initialize() );

		resources = gD3D11ResourceSystem.Construct();

		shaders = gD3D11ShaderSystem.Construct();
	}

	return true;
}

void GraphicsSystem::Shutdown()
{
	if( gLowLevelGraphicsInitCounter.DecRef() )
	{
		Initializable::OneTimeDestroy();

		shaders.Destruct();

		resources.Destruct();

		device.Shutdown();
	}
}

void GraphicsSystem::SetState( const StateBlock& newState )
{
	ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();

	newState.Set(pD3DContext);
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
