#include "stdafx.h"
#pragma hdrstop

#define USE_TEXT_ASSETS	(0)
#define LOAD_RESOLUTION_FROM_CONFIG	(1)

//#include <WindowsX.h>
//#include <CommCtrl.h>

#include <Base/Math/Vector/VectorTemplate.h>
#include <Base/Util/LogUtil.h>
#include <Base/Util/Rectangle.h>
#include <Base/Util/FPSTracker.h>

#include <Core/Util/Timer.h>
#include <Core/Serialization/PackageFile.h>

//#include <Graphics/DX11/DX11Private.h>

#include <Renderer/Renderer.h>
#include <Renderer/Common.h>
//#include <Renderer/GPU/Main.hxx>
#include <Renderer/Core/Font.h>
//#include <Renderer/Util/BatchRenderer.h>
//#include <Renderer/Util/ScreenQuad.h>

#include <Engine/Engine.h>
#include <Engine/Worlds.h>

#include <Driver/MyAppWindow.h>

#include <Renderer/Materials/SubsurfaceScattering.h>
#include <Renderer/Materials/Phong.h>

#if USE_TEXT_ASSETS
#include <EditorSupport/EditorSupport.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#pragma comment( lib, "EditorSupport.lib" )
#endif // USE_TEXT_ASSETS

#define APP_TITLE	"MyApp"

static const char* g_pathToAssetDb = "Data.rpk";
static const char* g_pathToSavedCameraParams = "_session.svg";


class MyApp : public AClientViewport, public AEngineClient
{
	rxViewport	m_mainViewport;

	SCamera			m_camera;
	TPtr< World >	m_world;

	LevelData	m_levelData;

#if USE_TEXT_ASSETS
	DevAssetManager	m_assetDb;
#else
	HashedPakFile	m_assetDb;
#endif

	FPSTracker<>	m_fpsTracker;
	GameTimer		m_timer;
	F4			m_avgFps;
	bool	m_bRememberSettings;

	U4	m_bAnimateLight;
	U4	m_bEnableSSS;

public:
	MyApp()
	{
		m_avgFps = 0.0f;
		m_bRememberSettings = false;

		m_bAnimateLight = false;
		m_bEnableSSS = false;

		static SizeT ptr = nil;
		ptr = Material_SSS::StaticClass().GetTypeGuid();
		ptr ^= PhongMaterial::StaticClass().GetTypeGuid();
	}
	~MyApp()
	{
	}

	bool Initialize( MyAppWindow* window )
	{
		{
			rxViewportConfig	config;
			config.windowHandle = window->getWindowHandle();
			config.bIsSceneViewport = true;
			config.bDepthStencil = true;

			gRenderer.CreateViewport( config, m_mainViewport );
		}
		window->SetClient( this );

#if USE_TEXT_ASSETS
		{
			CHK_VRET_FALSE_IF_NOT(m_assetDb.OpenExisting("D:/dev/_assets/_file_index.rdb"));
	
			SEngineInitArgs	initArgs;
			initArgs.client = this;
			CHK_VRET_FALSE_IF_NOT(gEngine.Initialize( initArgs ));
	
	
			FileReader			file(
				//"R:/_/Untitled/test_head.level.json"
				//"D:/dev/_levels/Untitled/Untitled.level.json"
				//"D:/dev/_levels/Untitled_latest/test_head.level.json"
				"test2.level.json"
				);
			CHK_VRET_FALSE_IF_NOT(file.IsOpen());
			TextObjectReader	serializer( file );
	
			SEngineLoadArgs	loadArgs;
			loadArgs.serializer = &serializer;
			CHK_VRET_FALSE_IF_NOT(gEngine.LoadState( loadArgs ));
		} 
#else
		{
			if( !m_assetDb.Open( g_pathToAssetDb ) ) {
				mxMsgBoxf("Failed to open '%s'.\n",g_pathToAssetDb);
				return false;
			}

			gCore.resources->SetContentDatabase( &m_assetDb );

			SEngineInitArgs	initArgs;
			initArgs.client = this;
			CHK_VRET_FALSE_IF_NOT(gEngine.Initialize( initArgs ));

			FileReader			file(
				//"test.world"
				//"test2.world"
				"test_head.world"
				);
			CHK_VRET_FALSE_IF_NOT(file.IsOpen());
			BinaryObjectReader	serializer( file );

			SEngineLoadArgs	loadArgs;
			loadArgs.serializer = &serializer;
			CHK_VRET_FALSE_IF_NOT(gEngine.LoadState( loadArgs ));
		}
#endif // USE_TEXT_ASSETS


		if( m_levelData.m_worlds.NonEmpty() )
		{
			m_world = m_levelData.m_worlds.GetFirst();
			m_camera = m_world->m_editorInfo.camera;
		}

		gCore.config->GetBool("bRememberSettings",m_bRememberSettings);
		if( m_bRememberSettings )
		{
			FileReader		file( g_pathToSavedCameraParams, FileRead_NoErrors );
			if( file.IsOpen() )
			{
				file.Unpack(m_camera);
				//ArchivePODReader	reader( file );
				//reader && m_camera;
				// 
				file >> m_bAnimateLight;

				file >> m_bEnableSSS;
				g_cvar_enable_subsurface_scattering = m_bEnableSSS;
				g_cvar_enable_separable_sss_post_fx = m_bEnableSSS;
			}
		}

		m_timer.Reset();

		return true;
	}

	void Shutdown()
	{
		if( m_bRememberSettings )
		{
			FileWriter		file( g_pathToSavedCameraParams, FileWrite_NoErrors );
			if( file.IsOpen() )
			{
				file.Pack(m_camera);
				//ArchivePODWriter	writer( file );
				//writer && m_camera;
				// 
				file << m_bAnimateLight;

				m_bEnableSSS = g_cvar_enable_subsurface_scattering;
				file << m_bEnableSSS;
			}
		}

		if( m_mainViewport.IsValid() ) {
			gRenderer.DestroyViewport( m_mainViewport );
		}

		gEngine.Shutdown();
	}

	bool IsValid() const
	{
		return true;
	}

public:	//=-- AClientViewport

	virtual void OnKeyPressed( EKeyCode key ) override
	{
		AClientViewport::OnKeyPressed( key );
		if( m_world != nil )
		{
			m_camera.OnKeyPressed( key );
		}
		if( key == EKeyCode::Key_F )
		{
			m_bAnimateLight ^= 1;
		}
		if( key == EKeyCode::Key_G )
		{
			g_cvar_enable_subsurface_scattering = !g_cvar_enable_subsurface_scattering;
			g_cvar_enable_separable_sss_post_fx = !g_cvar_enable_separable_sss_post_fx;
		}
	}

	virtual void OnKeyReleased( EKeyCode key ) override
	{
		AClientViewport::OnKeyReleased( key );
		if( m_world != nil )
		{
			m_camera.OnKeyReleased( key );
		}
	}

	virtual void OnMouseMove( const SMouseMoveEvent& args ) override
	{
		AClientViewport::OnMouseMove( args );
		if( m_world != nil )
		{
			m_camera.OnMouseMove( args.mouseDeltaX, args.mouseDeltaY );
		}
	}

	virtual void Draw() override
	{
		rxFrameContext	frameContext;
		frameContext.clearColor = FColor::BLACK;
		gRenderer.BeginScene( &m_mainViewport, frameContext );

		if( m_world != nil )
		{
			rxRenderWorld& renderWorld = m_world->GetRenderWorld();

			m_camera.SetAspectRatio( m_mainViewport.GetAspectRatio() );

			rxSceneContext	context( m_camera.GetView(), renderWorld );

			gRenderer.DrawScene( context );
		}

		//
		{
			DbgFont_BeginRendering();

			F4 posX = 10;
			F4 posY = 0;

			UNICODECHAR buffer[ MAX_STRING_CHARS ];

			MX_SPRINTF_UNICODE( buffer,
				L"FPS %.2f\n",
				m_avgFps
				);

			DbgFont_DrawString(
				buffer,
				posX, posY+=10,
				SColor::WHITE,
				16
			);

			posY+=10;

			DbgFont_DrawString(
				L"F - Animate light source",
				posX, posY+=20,
				SColor::YELLOW,
				16
			);
			DbgFont_DrawString(
				L"G - Toggle SSS",
				posX, posY+=20,
				SColor::YELLOW,
				16
			);

			DbgFont_EndRendering();
		}

		gRenderer.EndScene();
	}

public:	//=-- AEngineClient

	virtual bool LoadState( AObjectReader & archive ) override
	{
		archive.LoadObject( m_levelData );
		return true;
	}
	virtual bool SaveState( AObjectWriter & archive ) override
	{
		archive.SaveObject( m_levelData );
		return true;
	}
	virtual void Tick( FLOAT deltaSeconds ) override
	{
		m_camera.Update( deltaSeconds );

		rxRenderWorld& renderWorld = m_world->GetRenderWorld();
		if( renderWorld.m_localLights.Num() )
		{
			//if( this->IsKeyToggled( EKeyCode::Key_F ) )
			if( m_bAnimateLight )
			{
				rxLocalLight & firstLight = renderWorld.m_localLights.GetFirst();

				Matrix3	rot;
				rot.SetRotationAxis(deltaSeconds, Vec3D(0,1,0));

				Vec3D	oldPos = firstLight.GetOrigin();
				Vec3D	newPos = rot * oldPos;

				firstLight.SetOrigin( newPos );

				Vec3D	dir = (Vec3D(0) - newPos).GetNormalized();
				firstLight.SetDirection( dir );
			}
		}

		m_timer.TickFrame();
		if( deltaSeconds > 0.0f ) {
			m_avgFps = m_fpsTracker.CalculateFPS( m_timer.DeltaSeconds() );
		} else {
			m_avgFps = 0.0f;
		}
	}
};

mxAPPLICATION_ENTRY_POINT

int mxAppMain()
{
	SetupBaseUtil	setupBase;

	FileLogUtil		fileLog;
	SetupCoreUtil	setupCore;

	WindowsDriver	driver;

	bool bCreateConsole = false;
	gCore.config->GetBool("bConsoleWindow",bCreateConsole);
	if( bCreateConsole )
	{
		CConsole	console(APP_TITLE);
		console.setTopLeft(0,0);
	}



#if LOAD_RESOLUTION_FROM_CONFIG

	vector2d<UINT>	resolution(1024,768);
	vector2d<UINT>	topLeftCorner(0,0);

	gCore.config->GetUInt("WindowWidth",resolution.x);
	gCore.config->GetUInt("WindowHeight",resolution.y);

	resolution.x = smallest( resolution.x, 2048 );
	resolution.y = smallest( resolution.y, 2048 );

	gCore.config->GetUInt("TopLeftX",topLeftCorner.x);
	gCore.config->GetUInt("TopLeftY",topLeftCorner.y);

	topLeftCorner.x = smallest( topLeftCorner.x, 2048 );
	topLeftCorner.y = smallest( topLeftCorner.y, 2048 );

#else

#if 0
	vector2d<int>	resolution(800,600);
	vector2d<int>	topLeftCorner(800,450);
#else
	//vector2d<int>	resolution(1024,768);
	vector2d<int>	resolution(1400,900);
	vector2d<int>	topLeftCorner(0,0);
#endif

#endif


	MyAppWindow	window( resolution.x, resolution.y );
	window.bringToFront();
	window.setTopLeft( topLeftCorner.x, topLeftCorner.y );

	//DWORD winStyle = GetWindowStyle( window.getWindowHandle() );
	//winStyle |= WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	//::SendMessage( window.getWindowHandle(), TB_SETSTYLE, 0, winStyle );

	EdGraphics	editorGraphics;
	gRenderer.editor = &editorGraphics;



	MyApp		app;

	if( !app.Initialize( &window ) ) {
		return -1;
	}


	window.CaptureMouseInput(false);


	GameTimer	timer;

	while( window.isOpen() )
	{
		const F4 deltaSeconds = timer.TickFrame();

		app.Tick( deltaSeconds );

		window.Draw();

		driver.ProcessWinMessages();

		mxPROFILE_INCREMENT_FRAME_COUNTER;

		mxSleepMilliseconds(1);
	}

	app.Shutdown();

	return 0;
}

