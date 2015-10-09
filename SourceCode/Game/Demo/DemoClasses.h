#pragma once

#include <Base/Util/FPSTracker.h>

#include <Core/App/Client.h>
#include <Core/Util/Timer.h>

#include <Game/Item/GameItem.h>
#include <Game/Player/GamePlayer.h>

/*
-----------------------------------------------------------------------------
	TorchLight
-----------------------------------------------------------------------------
*/
class TorchLight : public gmItem
{
	mxDECLARE_CLASS( TorchLight, gmItem );

public:
	TorchLight();
};

/*
-----------------------------------------------------------------------------
	DemoGame
-----------------------------------------------------------------------------
*/
class DemoGame
	: public ClientViewport
	, public AEngineClient
	, public SingleInstance<DemoGame>
{
	rxViewport	m_mainViewport;

	TPtr<World>	m_world;

	Player::Ref	m_player;

	TPtr<Window>	m_window;

	ConstCharPtr	m_savedGameFileName;

	F4	m_lastFrameDeltaSeconds;
	FPSTracker<120>	m_fpsCounter;

public:
	DemoGame();
	~DemoGame();

	void PreInit_CreateViewports( Window* mainWindow );

	bool Initialize( ConstCharPtr& fileName );
	void Shutdown();

	void SaveGameState( const char* file = nil );
	void LoadGameState( const char* file = nil  );

	bool QuickSaveGame();
	bool LoadSavedGame();

public:	//--ClientViewport
	virtual void Tick( FLOAT deltaSeconds ) override;
	virtual void Draw() override;
	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;

public:	//--InputClient

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;
	virtual void OnMouseButton( int mouseX, int mouseY, EMouseButton btn, bool pressed ) override;
	virtual void OnMouseMove( int mouseX, int mouseY, int mouseDeltaX, int mouseDeltaY ) override;
	virtual void OnMouseWheel( int scroll ) override;

private:
	bool Init_PostLoad();
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
