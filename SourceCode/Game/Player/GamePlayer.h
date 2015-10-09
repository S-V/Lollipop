#pragma once

#include <Engine/Entity.h>

#include <Game/Item/GameItem.h>
#include <Game/Player/PlayerPhysics.h>


/*
-----------------------------------------------------------------------------
	Player
-----------------------------------------------------------------------------
*/
class Player
	: public AEntity
	, public GamePlayerController
{
public:
	gmInventory		m_inventory;

public:
	mxDECLARE_CLASS(Player,AEntity);

	// don't call 'delete', because
	// this object is statically allocated
	virtual void Destroy() { ; }

public:
	Player();

	virtual void Initialize( const SEntitySpawnContext& initContext ) override;

	virtual void Tick( const SEntityUpdateContext& updateContext ) override;

	virtual void edCreateProperties( EdPropertyCollector & outProperties, bool bClear = true ) override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
