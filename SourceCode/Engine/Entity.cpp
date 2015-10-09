/*
=============================================================================
	File:	Entity.cpp
	Desc:	Generic entity type, your typical OOP crap
=============================================================================
*/
#include <Engine_PCH.h>
#pragma hdrstop
#include <Engine.h>

#include <Engine/Entity.h>

/*
-----------------------------------------------------------------------------
	AEntity
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( AEntity );

AEntity::AEntity()
{
}

AEntity::~AEntity()
{

}

rxRenderEntity* AEntity::GetVisuals()
{
	return nil;
}

void AEntity::Initialize( const SEntitySpawnContext& initContext )
{
}

void AEntity::Shutdown()
{
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
