/*
=============================================================================
	File:	SpatialDatabase.h
	Desc:	Spatial structures for accelerating spatial queries on the scene
			(e.g. hierarchical view frustum culling).
=============================================================================
*/
#pragma once

// handle to entity stored in spatial database
typedef U4	ActorID;

// handle to spatial info stored in entity
typedef U4	TreeNodeID;

typedef void F_EnumerateActorID( ActorID actorHandle, void* userData );

/*
=======================================================================

	rxSpatialDatabase

	Graphics scene database.

=======================================================================
*/
struct rxSpatialDatabase
{
	// interface for client -> spatial database communication
	//
	struct IClientInfo
	{
		virtual void GetBoundingBox( ActorID entity, AABB &bounds ) = 0;

		virtual void GetBoundingSphere( ActorID entity, Sphere &bounds )
		{
			AABB	aabb;
			this->GetBoundingBox( entity, aabb );
			bounds = aabb.ToSphere();
		}

		virtual AABB GetWorldSize() const
		{
			//return AABB::infinite_aabb;
			return AABB(Vec3D(-1000), Vec3D(+1000));
		}
		virtual UINT ExpectedNumObjects() const
		{
			return 16;
		}

		virtual ~IClientInfo() {}
	};


	virtual ~rxSpatialDatabase() {}
};


extern bool g_cvar_debug_draw_spatial_database;


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
