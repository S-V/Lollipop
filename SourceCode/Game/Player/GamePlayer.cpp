#include "Game_PCH.h"
#pragma hdrstop

#include <Game/Player/GamePlayer.h>

bool	g_cvar_clip_player_movement = 1;


/*
-----------------------------------------------------------------------------
	Player
-----------------------------------------------------------------------------
*/
RTTI_DEFINE_CLASS(Player,AEntity);

Player::Player()
	: GamePlayerController(this)
{

}

void Player::Initialize( const SEntitySpawnContext& initContext )
{
	Super::Initialize( initContext );
}

void Player::Tick( const SEntityUpdateContext& updateContext )	
{
	Super::Tick(updateContext);

	const FLOAT deltaSeconds = updateContext.deltaSeconds;

	GamePlayerController::UpdateView( deltaSeconds );
}

void Player::edCreateProperties( EdPropertyCollector & outProperties, bool bClear )
{
	Super::edCreateProperties(outProperties,bClear);

}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
