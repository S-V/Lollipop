/*
=============================================================================
	File:	pxBroadphase_BVH.h
	Desc:	Broadphase based on dynamic bounding volume hierarchy.
	Note:	for (almost) static objects we use a KD-trie
	(elements are stored in leaves, ones that straddle splitting planes
	are duplicated in leaves, moving objects will be reinserted into tree;
	for dynamic & frequently moving objects an AABB tree is used
	(it's a hybrid between a loose octree and ABT).
	we 'intersect' those two trees when colliding static set vs dynamic set.
=============================================================================
*/

#ifndef __PX_BROAD_PHASE_H__
#define __PX_BROAD_PHASE_H__



/*
	internal (splitting) nodes always have 2 children (leaves).

	each leaf node has a pointer to the corresponding collision object.
*/

struct BVH_SplitNode
{
	float4	plane;	//«16 splitting plane
	U2		parent;	// index of parent node
	U2		kids[2];// indices of child nodes
};

struct BVH_LeafNode
{
	float4	sphere;	//«16 bounding sphere
};

MX_ALIGN_16(struct) BVH_Node
{
	struct LeafData
	{
	};
	struct SplitNodeData 
	{
	};
	float4	splitPlane;	//«16 splitting plane
};

// node type
enum EBvhNodeType
{
	BNT_Leaf = 0,

	// axial splitting plane types
	BNT_Plane_Axis_X,
	BNT_Plane_Axis_Y,
	BNT_Plane_Axis_Z,

	BNT_MAX
};

struct BVTree_Node
{
	U4	i;	//«4 node type
	F4	split;	//«4 splitting plane distance
	U2	numPrims;
	U2	wtf;
	U4	u1;
};
struct BVTree_Leaf
{
};


//
//	pxBroadphase_BVH
//
class pxBroadphase_BVH
{
	TList< BVH_Node >		m_nodes;

public:
			pxBroadphase_BVH();
	virtual	~pxBroadphase_BVH();

	// Adds an object to this broadphase.
	virtual void Add( pxCollideable* object ) = 0;

	// Removes an object from the broadphase.
	virtual void Remove( pxCollideable* object ) = 0;

	// Returns the total number of objects added to this broadphase.
	virtual pxUInt GetNumObjects() const = 0;

	// Returns the maximum allowed number of objects that can be added to this broadphase.
	virtual pxUInt GetMaxObjects() const = 0;

	// Removes all objects from the broadphase.
	virtual void Clear() = 0;

	// Performs a broad-phase collision detection and feeds potentially colliding pairs to the collision handler.
	virtual void Collide( pxCollisionDispatcher & handler ) = 0;

	//testing & debugging
	virtual void validate() {};
};

#endif // !__PX_BROAD_PHASE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
