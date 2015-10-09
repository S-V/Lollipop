/*
=============================================================================
	File:	SphereTree.h
	Desc:	Sphere tree.
	ToDo:	remove this crap
			(low memory & computational overhead, but very branch heavy)
=============================================================================
*/
#pragma once

#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/SpatialDatabase.h>

/*
 * sphere tree nodes can have several children
 * that are completely contained within the parent node;
 * nodes are organized in a linked-list;
 * each node has a handle to the corresponding entity.
*/

/*
-----------------------------------------------------------------------------
	SphereNode
-----------------------------------------------------------------------------
*/
struct SphereNode
{
	Sphere	bounds;	//«16 in world space

	ActorID	entity;	//«4 object stored in this node (invalid handle if root)

	U2		firstKid;	// index of the first child
	U2		parent;	// index of the parent node

	U2		nextSibling;
	U2		prevSibling;

	U2		numKids;	// number of children in this node

	U2		_pad32;	// reserved for future use

	// 32 bytes in total
};
mxDECLARE_POD_TYPE( SphereNode );




/*
-----------------------------------------------------------------------------
	SphereTree
-----------------------------------------------------------------------------
*/
struct SphereTree : public rxSpatialDatabase
{
	TList< SphereNode >	m_nodes;	// 0 - root node

	TPtr< rxSpatialDatabase::IClientInfo >	m_client;

public:
	SphereTree( rxSpatialDatabase::IClientInfo* client );

	// (Re)initializes the tree.
	void Refit( const AABB& worldBounds, UINT numObjects = 0 );

	void Clear();

	TreeNodeID Insert( ActorID newEntity );
	void Remove( TreeNodeID nodeHandle );

	void OnObjectMoved( const Vec3D& newPosition );

	// execute callback for potentially visible objects
	//
	void ProcessVisibleObjects(
		const rxSceneContext& sceneContext,
		F_EnumerateActorID* callback, void* userData );

	void DebugDraw( const rxSceneContext& sceneContext );
};

//typedef SphereTree rxSpatialDatabaseType;

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
