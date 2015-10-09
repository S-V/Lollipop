/*
=============================================================================
	File:	SphereTree.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Renderer/Scene/SphereTree.h>
#include <Renderer/Util/BatchRenderer.h>

/*
-----------------------------------------------------------------------------
	SphereTree
-----------------------------------------------------------------------------
*/

enum { ROOT_NODE_ID = 0 };
enum { NULL_NODE_ID = U2(-1) };

static inline
void InitNode( SphereNode & newNode )
{
	newNode.bounds.Clear();

	newNode.entity = INDEX_NONE;

	newNode.parent = NULL_NODE_ID;
	newNode.firstKid = NULL_NODE_ID;

	newNode.nextSibling = NULL_NODE_ID;
	newNode.prevSibling = NULL_NODE_ID;

	newNode.numKids = 0;
}

static inline
TreeNodeID AllocNode( SphereTree & t )
{
	SphereNode & newNode = t.m_nodes.Add();
	{
		//InitNode( newNode );
		(void)newNode;
	}
	//return newNode;
	return t.m_nodes.Num() - 1;
}

static inline
SphereNode& GetRoot( SphereTree & t )
{
	return t.m_nodes.GetFirst();
}

static inline
SphereNode& GetNode( SphereTree & t, TreeNodeID nodeIndex )
{
	return t.m_nodes[ nodeIndex ];
}
static inline
const SphereNode& GetNode( const SphereTree & t, TreeNodeID nodeIndex )
{
	return t.m_nodes[ nodeIndex ];
}


/*================================
		SphereTree
================================*/

SphereTree::SphereTree( rxSpatialDatabase::IClientInfo* client )
{
	AssertPtr(client);
	m_client = client;

	m_nodes.Reserve( client->ExpectedNumObjects() );

	// add root
	m_nodes.SetNum(1);
	SphereNode& root = GetRoot(*this);
	InitNode( root );
	root.bounds = client->GetWorldSize().ToSphere();
}

void SphereTree::Refit( const AABB& worldBounds, UINT numObjects )
{
	//
}

void SphereTree::Clear()
{
	m_nodes.SetNum(1);	//reserve space for root
}


static inline
void AppendNode( SphereTree & t, TreeNodeID parentID, TreeNodeID newChildID )
{
	SphereNode& parent = GetRoot( t );
	SphereNode& child = GetNode( t, newChildID );

	Assert( parent.bounds.ContainsSphere( child.bounds ) );
	AssertX( child.parent == NULL_NODE_ID, "The node must be detached from its parent" );

	child.parent = parentID;

	// this actually prepends new child node to the list

	const TreeNodeID prevFirstKid = parent.firstKid;

	// 'newChildID' - new head of list
	parent.firstKid = newChildID;

	// his next is my old next
	child.nextSibling = prevFirstKid;

	// if the parent had any children, tell them they have a new sibling
	if( prevFirstKid != NULL_NODE_ID ) {
		GetNode( t, prevFirstKid ).prevSibling = newChildID; // previous now this
	}

	parent.numKids++;
}

TreeNodeID SphereTree::Insert( ActorID newEntity )
{
	Sphere	objectBounds;
	m_client->GetBoundingSphere( newEntity, objectBounds );

	TreeNodeID newNodeId = AllocNode( *this );
	SphereNode& newNode = GetNode( *this, newNodeId );
	{
		newNode.bounds = objectBounds;

		newNode.entity = newEntity;

		newNode.parent = NULL_NODE_ID;
		newNode.firstKid = NULL_NODE_ID;

		newNode.nextSibling = NULL_NODE_ID;
		newNode.prevSibling = NULL_NODE_ID;

		newNode.numKids = 0;
	}

	AppendNode( *this, ROOT_NODE_ID, newNodeId );

	return newNodeId;
}

// moves node from 'srcIndex' to 'destIndex' and patches all pointers
//
static
void MoveNode( TreeNodeID srcIndex, TreeNodeID destIndex )
{
	UNDONE;
}

void SphereTree::Remove( TreeNodeID nodeHandle )
{
	Assert( nodeHandle != ROOT_NODE_ID );

	SphereNode& node = GetNode( *this, nodeHandle );

	AssertX( node.numKids == 0 && node.firstKid == NULL_NODE_ID, "can't unlink guys with children!" );

	// unlink the node from its parent

	TreeNodeID parentID = node.parent;
	SphereNode& parent = GetNode( *this, parentID );

	// NOTE: 'node.prevSibling' or 'nodeHandle' or 'node.nextSibling' can be 'parent.firstKid'


	// unlink the node from the doubly-linked list of its siblings
	{
		if( node.prevSibling != NULL_NODE_ID )
		{
			SphereNode& prevSibling = GetNode( *this, node.prevSibling );
			Assert( prevSibling.nextSibling == nodeHandle );
			prevSibling.nextSibling = node.nextSibling;
		}
		if( node.nextSibling != NULL_NODE_ID )
		{
			SphereNode& nextSibling = GetNode( *this, node.nextSibling );
			Assert( nextSibling.prevSibling == nodeHandle );
			nextSibling.prevSibling = node.prevSibling;
		}
	}

	node.parent = NULL_NODE_ID;

	if( parent.firstKid == nodeHandle )
	{
		Assert( parent.numKids == 1 );

		parent.firstKid = NULL_NODE_ID;

		// now the parent node has no children and may be deleted (if it's not the root node)
	}

	parent.numKids--;

	// release the node, return it back to pool
	const TreeNodeID lastNodeID = m_nodes.Num() - 1;
	if( nodeHandle != lastNodeID )
	{
		UNDONE;
		// move the last node into the place of the node being deleted
		MoveNode( lastNodeID, nodeHandle );

		m_nodes.PopBack();
	}
}

typedef void F_EnumerateNode( const TreeNodeID nodeId, void* userData );

// recursively processes the node and all its children
//
static
void ProcessSubtree_R( const SphereTree & t, const TreeNodeID nodeId,
					  F_EnumerateActorID* callback, void* userData )
{
	const SphereNode& node = GetNode( t, nodeId );
	(*callback)( node.entity, userData );

	TreeNodeID childId = node.firstKid;
	while( childId != NULL_NODE_ID )
	{
		const SphereNode& child = GetNode( t, childId );

		(*callback)( child.entity, userData );

		childId = child.nextSibling;
	}
}

// NOTE: starts by visiting the children, doesn't process the given node.
//
void FrustumQuery_R( const rxViewFrustum& frustum, const SphereTree & t, const TreeNodeID nodeId,
					F_EnumerateActorID* callback, void* userData )
{
	const SphereNode& node = GetNode( t, nodeId );

	TreeNodeID childId = node.firstKid;
	while( childId != NULL_NODE_ID )
	{
		const SphereNode& child = GetNode( t, childId );

		const ClipStatus_t status = frustum.TestSphere( child.bounds );
		if( status == CS_Intersect ) {
			(*callback)( child.entity, userData );
			FrustumQuery_R( frustum, t, childId, callback, userData );
		}
		else if( status == CS_FullyInside ) {
			ProcessSubtree_R( t, childId, callback, userData );
		}

		childId = child.nextSibling;
	}
}

void SphereTree::ProcessVisibleObjects( const rxSceneContext& sceneContext,
									   F_EnumerateActorID* callback, void* userData )
{
	return FrustumQuery_R( sceneContext.frustum, *this, ROOT_NODE_ID, callback, userData );
}

static inline
const FColor GetDbgColorForTreeDepth( UINT depth )
{
	static const FColor colors[]=
	{
		FColor::RED,	//root
		FColor::WHITE,
		FColor::YELLOW,
		FColor::LIGHT_YELLOW_GREEN,
		FColor::GREEN,
		FColor::BLUE,
		FColor::MAGENTA,
		FColor::LIGHT_GREY,
	};
	depth = smallest( depth, NUMBER_OF(colors)-1 );
	return colors[ depth ];
}

static
void DebugDrawNode_R( const rxSceneContext& sceneContext, const SphereTree & t, TreeNodeID nodeId, BatchRenderer & batchRenderer, UINT depth )
{
	Assert( nodeId != NULL_NODE_ID );

	const SphereNode& node = GetNode( t, nodeId );

	if( sceneContext.frustum.TestSphere( node.bounds ) == CS_Disjoint ) {
		return;
	}

	batchRenderer.DrawCircle(
		node.bounds.Center,
		sceneContext.GetCameraRightVector(), sceneContext.GetCameraUpVector(),
		GetDbgColorForTreeDepth(depth),
		node.bounds.Radius,
		32
	);

	TreeNodeID childId = node.firstKid;
	while( childId != NULL_NODE_ID )
	{
		const SphereNode& child = GetNode( t, childId );

		DebugDrawNode_R( sceneContext, t, childId, batchRenderer, depth+1 );

		// move to the next child
		childId = child.nextSibling;
	}
}

void SphereTree::DebugDraw( const rxSceneContext& sceneContext )
{
	BatchRenderer & batchRenderer = gRenderer.GetDrawHelper();

	DebugDrawNode_R( sceneContext, *this, ROOT_NODE_ID, batchRenderer, 0 );
}



#if 0
// Recursive Dimensional Clustering finds groups of potentially overlapping objects
//
namespace RDC
{
	// boundary of interval
	struct Brace
	{
		F4		f;	// lowest bit: 1 if max, 0 if min
		ActorID	o;	// entity handle

	public:
		// returns true if this boundary represents an "open bracket"
		FORCEINLINE bool IsOpening() const
		{
			_flint	t;
			t.f = f;
			return (t.i & 1) == 0;
		}
		FORCEINLINE bool IsClosing() const
		{
			return !this->IsOpening();
		}
		FORCEINLINE void SetOpening()
		{
			_flint	t;
			t.f = f;
			t.i &= ~1;
			f = t.f;
		}
		FORCEINLINE void SetClosing()
		{
			_flint	t;
			t.f = f;
			t.i |= 1;
			f = t.f;
		}
	};

	//typedef AABB BoundsType;
	typedef Sphere BoundsType;

	//Open-Close boundary list
	typedef TList< Brace >	OpenCloseBoundaryList;


	// interface for client -> RDC algorithm communication
	//
	struct IClientInfo
	{
		virtual UINT NumObjects() const = 0;
		virtual void GetBounds( UINT objectIndex, BoundsType &bounds ) const = 0;
		virtual ActorID GetHandle( UINT objectIndex ) const = 0;

	protected:
		IClientInfo() {}
		virtual ~IClientInfo() {}
	};


	inline
	void Get_Open_Close_Boundaries( IClientInfo* client, const Vec3D& axis, OpenCloseBoundaryList & braces )
	{
		braces.Empty();

		const UINT numObjects = client->NumObjects();

		for( UINT iObject = 0; iObject < numObjects; iObject++ )
		{
			BoundsType	bounds;
			client->GetBounds( iObject, bounds );

			const U4 objectHandle = client->GetHandle( iObject );

			Brace & min = braces.Add();
			min.SetOpening();
			min.o = objectHandle;

			Brace & max = braces.Add();
			max.SetClosing();
			max.o = objectHandle;

			bounds.AxisProjection( axis, min.f, max.f );
		}
	}

	inline
	void Sort_Open_Close_Boundaries( OpenCloseBoundaryList & elements )
	{
		Assert( elements.NonEmpty() );

		struct Compare
		{
			bool operator () ( const Brace& a, const Brace& b ) const
			{
				return a.f < b.f;
			}
		};
		Compare	predicate;

		Brace * start = elements.ToPtr();
		Brace * end = start + elements.Num() - 1;

		NxQuickSort( start, end, predicate );
	}

	inline
	void Get_Objects_Overlapping_Along_Axis( IClientInfo* client, const EAxisType inAxis, TList<ActorID> & overlappingObjects )
	{
		overlappingObjects.Empty();

		const UINT numObjects = client->NumObjects();
		const Vec3D	axis = Vec3D::StaticGetAxis( inAxis );

		OpenCloseBoundaryList	braces(EMemHeap::HeapTemp);
		braces.Reserve( numObjects * 2 );

		Get_Open_Close_Boundaries( client, axis, braces );
		Sort_Open_Close_Boundaries( braces );

		UINT braceCount = 0;

		for( UINT i=0; i < braces.Num(); i++ )
		{
			const Brace& brace = braces[i];
			if( brace.IsOpening() )
			{
				braceCount++;
				//this entity lies within a cluster group
				overlappingObjects.Add( brace.o );
			}
			else // brace.IsClosing()
			{
				braceCount--;

				if( braceCount == 0 )
				{
					//found the end of a cluster group - take subgroup
					//and call recursively on remaining axis'
					// 
					UNDONE;
				}
			}
		}//for
	}

}//namespace RDC
#endif
NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
