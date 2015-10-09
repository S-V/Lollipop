/*
=============================================================================
	File:	Entity.h
	Desc:	Generic entity type, your typical OOP crap
	Note:	this was slapped together in a hurry
			to get visual results quickly.
=============================================================================
*/
#pragma once

class rxRenderEntity;

struct SEntitySpawnContext
{
	TPtr< World >		world;
};

struct SEntityUpdateContext
{
	FLOAT	deltaSeconds;
};

/*
-----------------------------------------------------------------------------
	AEntity

	represents a generic entity

	@todo: remove this, use only integer ids instead of entity objects
-----------------------------------------------------------------------------
*/
class AEntity : public AObject
{
public:
	mxDECLARE_ABSTRACT_CLASS( AEntity, AObject );

	// Construction/Destruction
	AEntity();
	virtual ~AEntity();

	// Entity Registration
	// called before entity is added to world
	virtual void Initialize( const SEntitySpawnContext& initContext );

	// called after entity is removed from world
	virtual void Shutdown();


	// real-time updates and thinking
	virtual void Tick( const SEntityUpdateContext& updateContext ) {}

public:	// Components

	virtual rxRenderEntity* GetVisuals();

public:

	// the editor associated with this entity
	//TPtr< AObjectEditor >	m_editor;

};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
