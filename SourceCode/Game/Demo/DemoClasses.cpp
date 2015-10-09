#include "Game_PCH.h"
#pragma hdrstop

#include <Renderer/Renderer.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/Core/Font.h>
#include <Renderer/Util/BatchRenderer.h>
#include <Renderer/Util/ScreenQuad.h>

#include <Engine/Engine.h>
#include <Engine/Worlds.h>
#include <Engine/Entities.h>

#include <Game/Item/GameItem.h>
#include <Game/Demo/DemoClasses.h>

/*
-----------------------------------------------------------------------------
	TorchLight
-----------------------------------------------------------------------------
*/

mxDEFINE_CLASS( TorchLight, gmItem );

TorchLight::TorchLight()
{

}

/*
-----------------------------------------------------------------------------
	MyPhysDebugDrawer
-----------------------------------------------------------------------------
*/


class MyPhysDebugDrawer : public pxDebugDrawer
{
	TPtr< BatchRenderer >	m_lineBatcher;

public:
	MyPhysDebugDrawer()
	{
		m_lineBatcher = &gRenderer.GetDrawHelper();
	}
	virtual void drawLine(const pxVec3& from,const pxVec3& to,const pxVec3& color) override
	{
		m_lineBatcher->DrawLine3D(
			from.As_Vec3D(),
			to.As_Vec3D(),
			color.mVec128,
			color.mVec128
			);
	}
};

static void RenderDebugInfo( BatchRenderer & batchRenderer, const rxView& view, F4 fps, World* world )
{
	DbgFont_BeginRendering();

	UNICODECHAR buffer[ MAX_STRING_CHARS ];

	UINT textYpos = 1;

	{
		mxMemoryStatistics	memStats;
		F_GetGlobalMemoryStats(memStats);

		MX_SPRINTF_UNICODE( buffer,
			L"bytesAllocated=%u, totalNbAllocations=%u, totalNbDeallocations=%u\n"
			L"peakMemoryUsage: %u\n"

			, memStats.bytesAllocated, memStats.totalNbAllocations, memStats.totalNbDeallocations
			, memStats.peakMemoryUsage
			);

		DbgFont_DrawString( buffer, 1, textYpos+=1, FColor::YELLOW.ToRGBA32(), 13 );
	}


	//const FLOAT deltaSeconds = (DOUBLE)gfxBEStats.lastFrameRenderTime * 1e-6f;

	Dbg_DrawBackEndStats( fps, 10, textYpos+=40 );
	DrawCameraStats( view, 1, textYpos+=100 );

	{
		pxWorld* physWorld = world->GetPhysicsWorld();

		MX_SPRINTF_UNICODE( buffer,
			L"num. bodies: %u\n"
			L"addedPairs %u, searchedPairs %u, removedPairs %u\n"
			L"numContactManifolds %u\n"

			, physWorld->NumRigidBodies()
			, gPhysStats.addedPairs, gPhysStats.searchedPairs, gPhysStats.removedPairs
			, gPhysStats.numContactManifolds
			);

		DbgFont_DrawString( buffer, 2, textYpos+=200, FColor::YELLOW.ToRGBA32(), 13 );
	}

	DbgFont_EndRendering();
}


/*
-----------------------------------------------------------------------------
	DemoGame
-----------------------------------------------------------------------------
*/

DemoGame::DemoGame()
	: m_savedGameFileName("Saved.game")
{
	m_lastFrameDeltaSeconds = 0;
}

DemoGame::~DemoGame()
{

}

void DemoGame::PreInit_CreateViewports( Window* mainWindow )
{
	{
		rxViewportConfig	config;
		config.windowHandle = mainWindow->getWindowHandle();
		config.bIsSceneViewport = true;
		config.bDepthStencil = true;

		gRenderer.CreateViewport( config, m_mainViewport );
	}

	m_window = mainWindow;
}

bool DemoGame::Initialize( ConstCharPtr& fileName )
{
	FileReader		file( fileName.ToChars() );
	VRET_X_IF_NOT( file.IsOpen(), false );

	ArchiveReader	archive( file );

	SEngineLoadArgs		loadArgs;
	loadArgs.client = this;

	if( !gEngine.LoadFromFile( archive, loadArgs ) ) {
		return false;
	}

	return this->Init_PostLoad();
}

void DemoGame::Shutdown()
{
	this->SaveGameState();

	gRenderer.DestroyViewport( m_mainViewport );

	if( m_world != nil )
	{
		m_world->RemoveEntity(m_player);
	}

	m_player = nil;

	m_window = nil;
}

static ConstCharPtr	PERSISTENT_GAME_STATE_FILE("__session");

void DemoGame::SaveGameState( const char* fileName )
{
	VRET_IF_NOT( m_player != nil );

	FileWriter	file( fileName ? fileName : PERSISTENT_GAME_STATE_FILE.ToChars() );
	VRET_IF_NOT(file.IsOpen());

	ArchivePODWriter	archive( file );

	rxView	view = m_player->GetView();

	archive && view;
}

void DemoGame::LoadGameState( const char* fileName )
{
	VRET_IF_NOT( m_player != nil );

	FileReader	file( fileName ? fileName : PERSISTENT_GAME_STATE_FILE.ToChars() );
	VRET_IF_NOT(file.IsOpen());

	ArchivePODReader	archive( file );

	rxView	view;
	archive && view;

	m_player->SetView( view );
}

bool DemoGame::QuickSaveGame()
{
	//if( !gEngine.SaveToFile( m_savedGameFileName.ToChars() ) )
	//{
	//	return false;
	//}

	//

	return true;
}

bool DemoGame::LoadSavedGame()
{
	UNDONE;
	//if( !gEngine.LoadFromFile( m_savedGameFileName.ToChars() ) )
	//{
	//	return false;
	//}

	//return this->Init_PostLoad();
	return false;
}

void DemoGame::Tick( FLOAT deltaSeconds )
{
	//const bool bPhysicsSimPaused = m_window->IsKeyToggled(EKeyCode::Key_P);
	//g_cvar_tick_physics_world = !bPhysicsSimPaused;

#if 0
	if( m_window->IsKeyPressed(EKeyCode::Key_W) )
	{
		m_player->MoveForward();
	}
	if( m_window->IsKeyPressed(EKeyCode::Key_A) )
	{
		m_player->StrafeLeft();
	}
	if( m_window->IsKeyPressed(EKeyCode::Key_S) )
	{
		m_player->MoveBackward();
	}
	if( m_window->IsKeyPressed(EKeyCode::Key_D) )
	{
		m_player->StrafeRight();
	}
	if( m_window->IsKeyPressed(EKeyCode::Key_C) )
	{
		m_player->Crouch();
	}
	if( m_window->IsKeyPressed(EKeyCode::Key_Space) )
	{
		m_player->JumpUp();
	}
#endif

	//m_player->UpdateView(deltaSeconds);

	if( m_world != nil )
	{
		if( m_window->IsKeyPressed(EKeyCode::Key_Minus) )
		{
			m_world->GetPhysicsWorld()->GetGravity().y += 0.001f;
		}
		if( m_window->IsKeyPressed(EKeyCode::Key_Plus) )
		{
			m_world->GetPhysicsWorld()->GetGravity().y -= 0.001f;
		}
	}

	gEngine.Tick( deltaSeconds );

	//gPhysStats.Reset();

	m_lastFrameDeltaSeconds = deltaSeconds;
}

void DemoGame::OnResize( UINT newWidth, UINT newHeight, bool bFullScreen )
{
	DEVOUT("Resizing viewport\n");
	const FLOAT newAspectRatio = (FLOAT)newWidth / newHeight;
	if( m_player != nil )
	{
		m_player->SetAspectRatio( newAspectRatio );
	}
	gRenderer.OnViewportResized( m_mainViewport, newWidth, newHeight );
}

void DemoGame::Draw()
{
	rxFrameContext	frameContext;
	frameContext.clearColor = FColor::BLACK;
	gRenderer.BeginScene( &m_mainViewport, frameContext );

	if( m_world != nil )
	{
		rxView	camera = m_player->GetView();

		{
			camera.nearZ = 1;
			camera.farZ = 150;
		}


		rxSceneContext	sceneContext( camera, m_world->m_renderWorld );
		gRenderer.DrawScene( sceneContext );


		ID3D11DeviceContext* pD3DContext = GetD3DDeviceContext();
		m_mainViewport.Set( pD3DContext );
		GPU::Default.Set( pD3DContext );

		BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();
		{
			batchRenderer.SetTransform( sceneContext.viewProjectionMatrix );

			MyPhysDebugDrawer	myDebugDrawer;
			m_world->GetPhysicsWorld()->DebugDraw(&myDebugDrawer);

			batchRenderer.DrawInfiniteGrid( camera.origin, 0.01f, FColor::GRAY );
			batchRenderer.DrawAxes();
		}
		batchRenderer.Flush();

		const FLOAT fps = m_fpsCounter.CalculateFPS(m_lastFrameDeltaSeconds);
		RenderDebugInfo(batchRenderer,camera,fps,m_world);
	}

	gRenderer.EndScene();
}

void DemoGame::OnKeyPressed( EKeyCode key )
{
	if( m_player != nil )
	{
		m_player->OnKeyPressed( key );
	}

	if( key == EKeyCode::Key_O )
	{
		InvertBool( g_cvar_clip_player_movement );
	}
	if( key == EKeyCode::Key_P )
	{
		InvertBool( g_cvar_tick_physics_world );
	}

	// Quick save
	if( key == EKeyCode::Key_F6 )
	{
		this->QuickSaveGame();
	}
	// Load last saved game
	if( key == EKeyCode::Key_F9 )
	{
		this->LoadSavedGame();
	}
}

void DemoGame::OnKeyReleased( EKeyCode key )
{
	if( m_player != nil )
	{
		m_player->OnKeyReleased( key );
	}
}

void DemoGame::OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed )
{
}

void DemoGame::OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY )
{
	if( m_player != nil )
	{
		m_player->OnMouseMove( mouseX, mouseY, mouseDeltaX, mouseDeltaY );
	}
}

void DemoGame::OnMouseWheel( int scroll )
{
}

bool DemoGame::Init_PostLoad()
{
	{
		g_cvar_tick_physics_world = 1;
	}

	if( gEngine.worlds->IsEmpty() )
	{
		return false;
	}

	World* pFirstWorld = gEngine.worlds->At(0);

	m_world = pFirstWorld;


	m_player = new Player();
	{
		rxView camera = pFirstWorld->m_editorInfo.camera.GetView();

		camera.nearZ = 1;
		camera.farZ = 150;

		m_player->SetView( camera );
	}
	pFirstWorld->AddEntity( m_player );


	pxWorld* physicsWorld = m_world->GetPhysicsWorld();

	{
		//physicsWorld->SetGravity(Vec3D(0, -9.8f, 0));
		//physicsWorld->SetGravity(Vec3D(0, -0.4f, 0));
		//physicsWorld->SetGravity(Vec3D(0, -0.001f, 0));
		physicsWorld->SetGravity(Vec3D(0, 0, 0));

		physicsWorld->GetSolver()->Settings().minIterations = 4;
		physicsWorld->GetSolver()->Settings().maxIterations = 8;
		physicsWorld->GetSolver()->Settings().erp = 0.4f;
		physicsWorld->GetSolver()->Settings().precision = 0.001f;

		// bottom plane, floor
		if(1)
		{
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D::plane_y );
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(0,1,0,1) );
		}

		// top plane, ceiling
		if(1)
		{
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(0,-1,0,+10) );
		}

		if(1)
		{
			// left wall
			//Plane3D	leftWall( Vec3D::vec3_unit_x, Vec3D(-10,0,0) );
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(1,0,0,+10) );

			// right wall
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(-1,0,0,+10) );

			// front wall
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(0,0,1,+10) );

			// back wall
			//pxUtil_AddStaticPlane( physicsWorld, Plane3D(0,0,-1,+10) );
		}
	}



#if 0
	for( UINT iWorld = 0; iWorld < gEngine.worlds->Num(); iWorld++ )
	{
		World* w = gEngine.worlds->ItemAt( iWorld );

		const UINT numEntities = w->m_entities.Num();
		for( UINT iEntity = 0; iEntity < numEntities; iEntity++ )
		{
			AEntity* entity = w->m_entities[ iEntity ];

			//if( iEntity == 1 )
			{
				const SEntityBuildPhysicsArgs	args;
				entity->edRebuildPhysics( args );
			}
		}
	}
#endif

#if 0
	{
		const F4 radius = 1.0f;

		rxStaticMesh* staticMesh = (rxStaticMesh*)gCore.resources->GetResourceByGuid( ResMgr_Mesh, highly_tesselated_sphere_3ds );

		RigidBodyEntity::Ref newRigidBodyEntity = new RigidBodyEntity();

		{
			newRigidBodyEntity->m_renderModel.edConstruct( staticMesh );

			rxMaterial::Ref material = gCore.resources->GetResourceHandleByGuid(ResMgr_Material,);
			newRigidBodyEntity->m_renderModel.SetMaterial();

			const F4 scale = 0.5f / newRigidBodyEntity->m_renderModel.m_localAABB.Extents.x;
			newRigidBodyEntity->m_renderModel.SetScale(scale);
		}

		newRigidBodyEntity->m_rigidBody = pxUtil_AddSphere(physicsWorld,radius);

		m_world->AddEntity( newRigidBodyEntity );
	}
#endif

	this->LoadGameState();

	this->OnResize( m_mainViewport.GetWidth(), m_mainViewport.GetHeight(), m_mainViewport.IsFullScreen() );

	return true;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
