/*
=============================================================================
	File:	Worlds.h
	Desc:	Scene management.
=============================================================================
*/
#pragma once

#include <Core/Editor.h>
#include <Core/Object.h>
#include <Core/Entity/System.h>

//#include <Physics/Physics.h>

#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/RenderWorld.h>

#include <Engine/Engine.h>
#include <Engine/Entity.h>

mxNAMESPACE_BEGIN

extern bool	g_cvar_tick_physics_world;
extern bool	g_cvar_clip_player_movement;

/*
-----------------------------------------------------------------------------
	WorldDescription
-----------------------------------------------------------------------------
*/
struct WorldDescription
{
	//AABB	size;

	WorldDescription()
	//	: size(_InitInfinity)
	{}
};

/*
-----------------------------------------------------------------------------
	EdLevelInformation
-----------------------------------------------------------------------------
*/
struct EdLevelInformation : public SBaseType
{
	String		name;
	String		author;
	String		description;
	mxVersion	version;

public:
	mxDECLARE_CLASS(EdLevelInformation,SBaseType);
	mxDECLARE_REFLECTION;

	EdLevelInformation()
	{
		this->name = "Unnamed";
		this->author = "Unknown";
		this->description = "No description";
		this->version.v = 0;
	}
	friend mxArchive& operator && ( mxArchive & serializer, EdLevelInformation & o )
	{
		return serializer
			&& o.name && o.author && o.description && o.version
			;
	}
};

/*
-----------------------------------------------------------------------------
	EdLevelViewInfo
	saved editor viewport state
-----------------------------------------------------------------------------
*/
struct EdLevelViewInfo : public SBaseType
{
	Vec3D	right;			// Right direction.
	Vec3D	up;				// Up direction.
	Vec3D	look;			// Look direction.
	Vec3D	origin;			// Eye position - Location of the camera, expressed in world space.
	FLOAT	nearZ, farZ;	// Near and far clipping planes.

public:
	mxDECLARE_CLASS(EdLevelViewInfo,SBaseType);

	EdLevelViewInfo()
	{
		rxView	defaultView;
		this->From( defaultView );
	}
	void From( const rxView& view )
	{
		this->up		= view.up;
		this->look		= view.look;
		this->right		= view.right;
		this->origin	= view.origin;
		this->nearZ		= view.nearZ;
		this->farZ		= view.farZ;
	}
	void To( rxView &view ) const
	{
		view.up			= this->up;
		view.look		= this->look;
		view.right		= this->right;
		view.origin		= this->origin;
		view.nearZ		= this->nearZ;
		view.farZ		= this->farZ;
	}
};
mxDECLARE_POD_TYPE(EdLevelViewInfo);

/*
-----------------------------------------------------------------------------
	SWorldEditInfo
	saved editor states (viewports, entities, etc.)
-----------------------------------------------------------------------------
*/
struct SWorldEditInfo : public SBaseType
{
	SCamera			camera;
	//EntityId			selectedEntity;

public:
	mxDECLARE_CLASS(SWorldEditInfo,SBaseType);
	mxDECLARE_REFLECTION;

	SWorldEditInfo()
	{
		//selectedEntity = NULL_ENTITY_ID;
	}
};

/*
-----------------------------------------------------------------------------
	World
	abstract world interface
-----------------------------------------------------------------------------
*/
class World
	: public AObject
{
	// physics
	//SWorldPhysics	m_physics;

	// graphics
	rxRenderWorld	m_renderWorld;

	// entities
	TList< AEntity* >	m_entities;

public:	// Editor

	// saved editor view params
	SWorldEditInfo		m_editorInfo;

	// name of this world
	String		m_name;

	// the editor associated with this world
	TPtr< class World_Editor >	m_editor;

public:
	virtual void Clear();

	virtual void AddEntity( AEntity* newEntity );
	virtual void DeleteEntity( AEntity* theEntity );

	virtual void RunPhysics( FLOAT deltaSeconds );
	virtual void UpdateEntities( FLOAT deltaSeconds );

	//pxWorld& GetPhysicsWorld();
	rxRenderWorld& GetRenderWorld();

	virtual void Optimize();

	virtual void PostLoad() override;

public_internal:

	mxDECLARE_CLASS(World,AObject);
	mxDECLARE_CLASS_ALLOCATOR( HeapSceneData, World );
	mxDECLARE_REFLECTION;

	World();
	~World();

private:
	void Register_Entity_Components( AEntity* newEntity );
	void Unregister_Entity_Components( AEntity* theEntity );

public:	// Editor

#if MX_EDITOR

	friend class Graphics_Scene_Editor;
	friend class World_Editor;
	friend class World_Entities_List;

#endif //MX_EDITOR

};

FORCEINLINE
rxRenderWorld& World::GetRenderWorld()
{
	return m_renderWorld;
}



/*
-----------------------------------------------------------------------------
	LevelData

	groups together all worlds
	so that they can be processed more efficiently
	(e.g. it enables us to load the whole level 'in-place',)
	(which is, basically, one fread() into one memory block)
	(and patching the resulting memory image).

	When in runtime mode, the loaded level is typically loaded
    directly into a single memory block, with all of its entities attached.
-----------------------------------------------------------------------------
*/
struct LevelData : public SBaseType
{
	// Versioning
	mxVersion				m_engineVersion;
	mxVersion				m_formatVersion;
	STimeStamp				m_timestamp;

	// All scenes
	TList< World* >			m_worlds;

	// Editor
 	EdLevelInformation	m_levelInfo;

public:
	mxDECLARE_CLASS(LevelData,SBaseType);
	mxDECLARE_REFLECTION;

	LevelData();

	void PostLoad();
};

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
