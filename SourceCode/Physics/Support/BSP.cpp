/*
=============================================================================
	File:	BSP.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Convex.h>
#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Support/BSP.h>
#include <Physics/Support/pxUtilities.h>

enum { MAX_TRACE_PLANES = 32 };

typedef TStaticList<Plane3D,MAX_TRACE_PLANES>	PlaneStack;


// keep 1/8 unit away to keep the position valid before network snapping
// and to avoid various numeric issues
//#define	SURFACE_CLIP_EPSILON	(0.125)
#define SURFACE_CLIP_EPSILON	(1/32.0f)



FORCEINLINE
F4 CalcAabbEffectiveRadius( const Vec3D& boxHalfSize, const Plane3D& plane )
{
	return
		mxFabs( boxHalfSize[0] * plane.Normal()[0] ) +
		mxFabs( boxHalfSize[1] * plane.Normal()[1] ) +
		mxFabs( boxHalfSize[2] * plane.Normal()[2] );
}

/*
-----------------------------------------------------------------------------
	BSP_Tree
-----------------------------------------------------------------------------
*/
BSP_Tree::BSP_Tree()
	: m_nodes(EMemHeap::HeapPhysics)
	, m_planes(EMemHeap::HeapPhysics)
{

}

#if MX_EDITOR

typedef TList< idFixedWinding >	PolygonList;

/*
-----------------------------------------------------------------------------
	SBspStats
-----------------------------------------------------------------------------
*/
SBspStats::SBspStats()
{
	this->Reset();
}

void SBspStats::Reset()
{
	ZERO_OUT( *this );

	m_beginTimeMS = mxGetMilliseconds();
}

void SBspStats::Stop()
{
	m_elapsedTimeMS = mxGetMilliseconds() - m_beginTimeMS;
}

void SBspStats::Print()
{
	DBGOUT( "\n=== BSP statistics ========\n"				);
	DBGOUT( "Num. Polys(Begin): %d\n", m_numOrigPolygons	 );
	//DBGOUT( "Num. Polys(End):   %d\n", m_numPolygons		 );
	DBGOUT( "Num. Splits:	    %d\n", m_numSplits			);
	DBGOUT( "Num. Inner Nodes:  %d\n", m_numInternalNodes );
	DBGOUT( "Num. Solid Leaves: %d\n", m_numSolidLeaves	 );
	DBGOUT( "Num. Empty Leaves: %d\n", m_numEmptyLeaves	 );
	//DBGOUT( "Tree Depth:        %d\n", depth			 );
	DBGOUT( "Time elapsed:      %d msec\n", m_elapsedTimeMS		 );
	DBGOUT( "==== End ====================\n"			 );
}

static
EPolyStatus F_ClassifyPolygon(
							const Plane3D& plane,
							const Vec3D* verts, const UINT numVerts,
							const FLOAT epsilon = 0.013f )
{
	UINT	numPointsInFront = 0;
	UINT	numPointsInBack = 0;
	UINT	numPointsOnPlane = 0;

	for( UINT iVertex = 0; iVertex < numVerts; iVertex++ )
	{
		const Vec3D& point = verts[ iVertex ];

		const EPlaneSide ePlaneSide = plane.Side( point, epsilon );

		if( ePlaneSide == EPlaneSide::PLANESIDE_FRONT )
		{
			numPointsInFront++;
		}
		if( ePlaneSide == EPlaneSide::PLANESIDE_BACK )
		{
			numPointsInBack++;
		}
		if( ePlaneSide == EPlaneSide::PLANESIDE_ON )
		{
			numPointsOnPlane++;
		}
	}

	if( numVerts == numPointsInFront ) {
		return Poly_Front;
	}
	if( numVerts == numPointsInBack ) {
		return Poly_Back;
	}
	if( numVerts == numPointsOnPlane ) {
		return Poly_Coplanar;
	}
	return Poly_Split;
}

/*
The selection of the base polygon and the partitioning plane
is the crucial part of the BSP-tree construction. Depend-
ing on criteria for the base polygon selection different BSP-
trees can be obtained. In our work we use two different ap-
proaches: "naive" selection, where the polygon is randomly
selected from the polygon list, and "optimized" selection.
The optimization means using selection criteria that allow
for obtaining a tree with the following properties:
• Minimization of polygon splitting operations to reduce
the total number of nodes and the number of operations
in the function evaluation
• Minimization of computational errors during the function
evaluation and BSP-tree construction;
• Balancing the BSP tree, i.e., minimization of difference
between positive and negative list for the minimization of
the depth of the tree.
*/
// function for picking an optimal partitioning plane.
// we have two conflicting goals:
// 1) keep the tree balanced
// 2) avoid splitting the polygons
// and avoid introducing new partitioning planes
//
static
UINT F_FindBestSplitterIndex( const PolygonList& polygons
						   , const SBuildBspArgs& options = SBuildBspArgs() )
{
	INT		numFrontFaces = 0;
	INT		numBackFaces = 0;
	INT		numSplitFaces = 0;
	INT		numCoplanarFaces = 0;

	UINT	bestSplitter = 0;
	FLOAT	bestScore = 1e6f;	// the less value the better

	for(UINT iPolyA = 0;
		iPolyA < polygons.Num();
		iPolyA++)
	{
		// select potential splitter
		const idFixedWinding& polyA = polygons[ iPolyA ];

		// potential splitting plane
		Plane3D	planeA;
		polyA.GetPlane( planeA );

		// test other polygons against the potential splitter

		for(UINT iPolyB = 0;
			iPolyB < polygons.Num();
			iPolyB++)
		{
			if( iPolyA == iPolyB ) {
				continue;
			}

			const idFixedWinding& polyB = polygons[ iPolyB ];

			// evaluate heuristic cost and select the best candidate

			const int planeSide = polyB.PlaneSide( planeA, options.planeEpsilon );

			switch( planeSide )
			{
			case PLANESIDE_FRONT :	numFrontFaces++;	break;
			case PLANESIDE_BACK :	numBackFaces++;		break;
			case PLANESIDE_ON :		numCoplanarFaces++;	break;
			case PLANESIDE_CROSS :	numSplitFaces++;	break;
			default:				Unreachable;
			}

			// diff == 0 => tree is perfectly balanced
			const UINT diff = Abs<INT>( numFrontFaces - numBackFaces );

			F4 score = (diff * options.balanceVsCuts)
				+ (numSplitFaces * options.splitCost) * (1.0f - options.balanceVsCuts)
				;

			if( planeA.Type() < PLANETYPE_TRUEAXIAL )
			{
				score *= 0.8f;	// axial is better
			}

			// A smaller score will yield a better tree.
		    if( score < bestScore )
		    {
		        bestScore = score;
                bestSplitter = iPolyA;
		    }

		}//for all tested polygons
	}//for all potential splitters

	return bestSplitter;
}

#define	NORMAL_EPSILON		0.00001f
#define	DIST_EPSILON		0.01f

static
UINT GetPlaneIndex(
				   BSP_Tree & tree, const Plane3D& plane,
				   const float normalEps = NORMAL_EPSILON,
				   const float distEps = DIST_EPSILON
				   )
{
	Plane3D	normalizedPlane = plane;
	normalizedPlane.FixDegeneracies( distEps );

	Assert( distEps <= 0.125f );

	const UINT numExistingPlanes = tree.m_planes.Num();

	for( UINT iPlane = 0; iPlane < numExistingPlanes; iPlane++ )
	{
		const Plane3D existingPlane = tree.m_planes[ iPlane ];

		if( existingPlane.Compare( normalizedPlane, normalEps, distEps ) )
		{
			return iPlane;
		}
	}

	const UINT newPlaneIndex = numExistingPlanes;
	Assert( newPlaneIndex <= BSP_MAX_PLANES );

	tree.m_planes.Add( normalizedPlane );

	return newPlaneIndex;
}

// returns index of the splitting plane
//
static
UINT PartitionPolygons(
					   BSP_Tree & tree,
					   PolygonList & polygons,
					   PolygonList & frontPolys,
					   PolygonList & backPolys,
					   const FLOAT epsilon = 0.13f
					   )
{
	// select the best partitioner

	SBuildBspArgs	settings;
	// we don't need polygons for collision detection
	settings.splitCost = 0;
	settings.balanceVsCuts = 1;
	settings.planeEpsilon = 0.1f;

	const UINT bestSplitter = F_FindBestSplitterIndex( polygons, settings );

	Plane3D	partitioner;
	polygons[ bestSplitter ].GetPlane( partitioner );

	//polygons.RemoveAt_Fast( bestSplitter );

	// partition the list

	for( UINT iPoly = 0; iPoly < polygons.Num(); iPoly++ )
	{
		if( iPoly == bestSplitter ) {
			continue;
		}

		idFixedWinding &	polygon = polygons[ iPoly ];

		idFixedWinding		backPoly;

		const int planeSide = polygon.Split( &backPoly, partitioner, epsilon );

		if( planeSide == PLANESIDE_FRONT )
		{
			frontPolys.Add( polygon );
			continue;
		}
		if( planeSide == PLANESIDE_BACK )
		{
			backPolys.Add( polygon );
			continue;
		}
		if( planeSide == PLANESIDE_CROSS )
		{
			frontPolys.Add( polygon );
			backPolys.Add( backPoly );

			tree.m_stats.m_numSplits++;
			continue;
		}

		Assert( planeSide == PLANESIDE_ON );
		// continue
	}

	return GetPlaneIndex( tree, partitioner );
}

#if 0
static
void Dbg_ValidateNode_R( BSP_Tree & tree, UINT nodeIndex )
{
	const BspNode& node = tree.m_nodes[ nodeIndex ];

	if( node.IsInternal() )
	{
		Assert( tree.m_planes.IsValidIndex( node.node.plane ) );
		Assert( tree.m_nodes.IsValidIndex( node.node.pos ) );
		Assert( tree.m_nodes.IsValidIndex( node.node.neg ) );
		
		Dbg_ValidateNode_R( tree, node.node.pos );
		Dbg_ValidateNode_R( tree, node.node.neg );
	}
}

static
void Dbg_DumpNode_R( BSP_Tree & tree, UINT nodeIndex, UINT depth = 0 )
{
	const BspNode& node = tree.m_nodes[ nodeIndex ];

	for(UINT i=0; i<depth; i++)
	{
		DBGOUT(" ");
	}
	if( node.IsInternal() )
	{
		DBGOUT("Inner node@%u: plane=%u, neg=%u, pos=%u\n"
			,nodeIndex,(UINT)node.node.plane,(UINT)node.node.neg,(UINT)node.node.pos
			);

		Dbg_DumpNode_R( tree, node.node.pos, depth+1 );
		Dbg_DumpNode_R( tree, node.node.neg, depth+1 );
	}
	else
	{
		DBGOUT("%s leaf@%u\n",
			node.type==BN_Solid ? "Solid" : "Empty", nodeIndex);
	}
}
#endif



static
inline
UINT F_AllocateNode( BSP_Tree & tree )
{
	const UINT newNodeIndex = tree.m_nodes.Num();
	Assert( newNodeIndex <= BSP_MAX_NODES );

	BspNode & newNode = tree.m_nodes.Add();

#if MX_DEBUG
	MemSet(&newNode,-1,sizeof BspNode);
	//DBGOUT("! creating node %u\n",newNodeIndex);
#endif //MX_DEBUG

	return newNodeIndex;
}

static
FORCEINLINE
BspNode* GetNodeByIndex( BSP_Tree & tree, UINT nodeIndex )
{
	return &tree.m_nodes[ nodeIndex ];
}

static
inline
UINT F_NewInternalNode( BSP_Tree & tree )
{
	const UINT newNodeIndex = F_AllocateNode( tree );
	BspNode & newNode = tree.m_nodes[ newNodeIndex ];
	{
		(void)newNode;
		//newNode.type = BN_Polys;
	}
	tree.m_stats.m_numInternalNodes++;

	return newNodeIndex;
}


// returns index of new node
//
static
inline
UINT F_NewEmptyLeaf( BSP_Tree & tree )
{
	tree.m_stats.m_numEmptyLeaves++;
#if 0
	const UINT newNodeIndex = F_AllocateNode( tree );
	BspNode & newNode = tree.m_nodes[ newNodeIndex ];
	{
		newNode.type = BN_Empty;
	}
#else
	const UINT newNodeIndex = BSP_EMPTY_LEAF;
#endif
	return newNodeIndex;
}

// returns index of new node
//
static
inline
UINT F_NewSolidLeaf( BSP_Tree & tree )
{
	tree.m_stats.m_numSolidLeaves++;
#if 0
	const UINT newNodeIndex = F_AllocateNode( tree );
	BspNode & newNode = tree.m_nodes[ newNodeIndex ];
	{
		newNode.type = BN_Solid;
	}
#else
	const UINT newNodeIndex = BSP_SOLID_LEAF;
#endif
	return newNodeIndex;
}


// returns index of new node
//
static
UINT BuildTree_R( BSP_Tree & tree, PolygonList & polygons )
{
	Assert( polygons.NonEmpty() );

	// allocate a new internal node

	const UINT newNodeIndex = F_NewInternalNode( tree );

	// partition the list

	PolygonList	frontPolys(EMemHeap::HeapTemp);
	PolygonList	backPolys(EMemHeap::HeapTemp);

	const UINT splitPlane = PartitionPolygons( tree, polygons, frontPolys, backPolys );

	GetNodeByIndex( tree, newNodeIndex )->node.plane = splitPlane;

	// recursively process children

	if( frontPolys.Num() )
	{
		GetNodeByIndex( tree, newNodeIndex )->node.pos = BuildTree_R( tree, frontPolys );
	}
	else
	{
		GetNodeByIndex( tree, newNodeIndex )->node.pos = F_NewEmptyLeaf( tree );
	}


	if( backPolys.Num() )
	{
		GetNodeByIndex( tree, newNodeIndex )->node.neg = BuildTree_R( tree, backPolys );
	}
	else
	{
		GetNodeByIndex( tree, newNodeIndex )->node.neg = F_NewSolidLeaf( tree );
	}

	return newNodeIndex;
}

struct pxPolygonCollector : pxTriangleIndexCallback
{
	PolygonList &	m_polygons;

	pxPolygonCollector( PolygonList & polygons )
		: m_polygons( polygons )
	{
	}
	virtual void ProcessTriangle( const Vec3D& p0, const Vec3D& p1, const Vec3D& p2 ) override
	{
		idFixedWinding & newPolygon = m_polygons.Add();

		// need to reverse winding (different culling in D3D11 renderer and id's winding)
#if 0
		newPolygon.AddPoint( p0 );
		newPolygon.AddPoint( p1 );
		newPolygon.AddPoint( p2 );
#else
		newPolygon.AddPoint( p2 );
		newPolygon.AddPoint( p1 );
		newPolygon.AddPoint( p0 );
#endif
	}
};

void BSP_Tree::Build( pxTriangleMeshInterface* triangleMesh )
{
	PolygonList	polygons(EMemHeap::HeapTemp);

	pxPolygonCollector		collectPolys( polygons );
	triangleMesh->ProcessAllTriangles( &collectPolys );

	m_stats.Reset();
	m_stats.m_numOrigPolygons = polygons.Num();

	BuildTree_R( *this, polygons );

	//Dbg_ValidateNode_R( *this, BSP_ROOT_NODE );

	m_nodes.Shrink();
	m_planes.Shrink();

	m_stats.Stop();
	m_stats.Print();

	DBGOUT("BSP tree: memory used = %u (%u planes)\n",
		(UINT)this->GetMemoryUsed(),m_planes.Num());
}

#endif // MX_EDITOR

struct SOverlapArgs
{
	UINT	prevNode;	// index of parent (internal) BSP node
	UINT	currNode;	// index of current BSP node
	pxVec3	closestPt;
	F4		minDist;	// distance from the convex to the previous node's plane

public:
	SOverlapArgs()
	{
		currNode = BSP_ROOT_NODE;
		prevNode = INDEX_NONE;
		closestPt.SetAll(PX_LARGE_FLOAT);
		minDist = PX_LARGE_FLOAT;
	}
};

bool BSP_Tree::PointInSolid( const Vec3D& point ) const
{
	TStaticList<UINT,BSP_MAX_DEPTH>	nodeStack;
	nodeStack.Add( BSP_ROOT_NODE );

	while( true )
	{
		const UINT nodeId = nodeStack.GetLast();
		nodeStack.PopBack();

		if( nodeId == BSP_EMPTY_LEAF ) {
			return false;
		}
		if( nodeId == BSP_SOLID_LEAF ) {
			return true;
		}

		const BspNode& node = m_nodes[ nodeId ];

		const Plane3D& plane = m_planes[ node.node.plane ];

		const F4 dist = plane.Distance( point );

		if( dist >= 0.0f )
		{
			nodeStack.Add( node.node.pos );
			continue;
		}
		else//if( dist < 0 )
		{
			nodeStack.Add( node.node.neg );
			continue;
		}
	}
}

F4 BSP_Tree::DistanceToPoint( const Vec3D& point ) const
{
	TStaticList<UINT,BSP_MAX_DEPTH>	nodeStack;

	nodeStack.Add( BSP_ROOT_NODE );

	TStaticList<F4,BSP_MAX_DEPTH>	distStack;

	while( true )
	{
		const UINT nodeId = nodeStack.GetLast();
		nodeStack.PopBack();

		MX_UNDONE("this is incorrect, find distance to convex hull");
		// if point is outside the brush
		if( nodeId == BSP_EMPTY_LEAF ) {
			// we need to find the closest point
			// on the convex hull formed by intersection of planes
Unimplemented;
			return 0.0f;
		}
		// if point is inside the brush
		if( nodeId == BSP_SOLID_LEAF ) {
			// take minimum distance from point to planes
			F4 minDist = -PX_LARGE_FLOAT;
			for( UINT i = 0; i < distStack.Num(); i++ )
			{
				// take maximum because the point is behind all planes
				minDist = maxf( minDist, distStack[i] );
			}
			return minDist;
		}

		const BspNode& node = m_nodes[ nodeId ];

		const Plane3D& plane = m_planes[ node.node.plane ];

		const F4 dist = plane.Distance( point );

		distStack.Add( dist );

		if( dist >= 0.0f )
		{
			nodeStack.Add( node.node.pos );
			continue;
		}
		else//if( dist < 0 )
		{
			nodeStack.Add( node.node.neg );
			continue;
		}
	}

	Unreachable;

	return PX_LARGE_FLOAT;
}

struct STraceIn
{
	Vec3D	start;
	Vec3D	end;
	F4		radius;
	UINT	currNode;
};
struct STraceOut
{
	F4	fraction;
};


struct STraceWorks
{
	//Vec3D	start;
	//Vec3D	end;
	//F4 radius;
	AABB	boxsize;// size of the box being swept through the model
	Vec3D	extents;// half size of the box

	Vec3D	normal;// surface normal at impact, transformed to world space
	F4		planeDist;
	F4		fraction;	// time completed, 1.0 = didn't hit anything
	bool	startsolid;
	bool	allsolid;

public:
	STraceWorks()
	{
		// fill in a default trace
		//radius = 0;
		boxsize.SetZero();
		extents.SetZero();

		normal.SetZero();
		planeDist = 0.0f;
		// assume it goes the entire distance until shown otherwise
		fraction = 1;
		startsolid = false;
		allsolid = false;
	}
};

enum ETraceResult
{
	Trace_Empty,
	Trace_Solid,
	Trace_Done,
};

enum {
	PlaneSide_Front = 0,
	PlaneSide_Back = 1,
};

// NOTE: portions of code taken and modified from quake/darkplaces engine sources
// this is not 100% correct (esp. edge collisions) (should use beveling planes)
//
static
ETraceResult TraceBox_R( STraceWorks & tw, const BSP_Tree& t, /*const*/ UINT nodeId, const Vec3D& start, const Vec3D& end, const F4 f1, const F4 f2 )
{
L_Start:
	// check if this is a leaf node
	if( BSP_EMPTY_LEAF == nodeId )
	{
		return Trace_Empty;
	}
	if( BSP_SOLID_LEAF == nodeId )
	{
		return Trace_Solid;
	}

	const BspNode& node = t.m_nodes[ nodeId ];


	Plane3D	plane = t.m_planes[ node.node.plane ];

	// calculate offset for the size of the box and
	// adjust the plane distance appropriately for mins/maxs
	plane.d -= CalcAabbEffectiveRadius( tw.extents, plane );

	// distance from plane for trace start and end
	const F4 d1 = plane.Distance( start );
	const F4 d2 = plane.Distance( end );


	// see which sides we need to consider

	int planeSide;	// 0 - check front side first, 1 - check back side first

	// if start point in air
	if( d1 >= 0.0f )
	{
		if( d2 >= 0.0f )// endpoint in air
		{
			// completely in front of plane
			nodeId = node.node.pos;
			goto L_Start;
		}

		// d1 >= 0 && d2 < 0

		planeSide = PlaneSide_Front;
	}
	else // d1 < 0
	{
		// start point in solid

		if( d2 < 0.0f )// endpoint in solid
		{
			// completely behind plane
			nodeId = node.node.neg;
			goto L_Start;
		}
		// endpoint in air

		// d1 < 0 && d2 >= 0

		planeSide = PlaneSide_Back;
	}


	// intersecting the plane, split the line segment into two
	// and check both sides, starting from 'planeSide'

	const F4 midf = clampf( d1 / (d1 - d2), f1, f2 );
	Assert( midf >= 0.0f && midf <= 1.0f );

	const Vec3D midp = start + (end - start) * midf;


	// we're interested in case where 'start' is in empty space
	// and 'end' is in solid region.
	// 
	ETraceResult	ret;

	// check the nearest side first
	ret = TraceBox_R( tw, t, node.node.kids[planeSide], start, midp, f1, midf );
	// if this side is not empty, return what it is (solid or done)
	if( ret != Trace_Empty ) {
		return ret;
	}
	// good, 'start' point is in empty space

	ret = TraceBox_R( tw, t, node.node.kids[planeSide^1], midp, end, midf, f2 );
	// if other side is not solid, return what it is (empty or done)
	if( ret != Trace_Solid ) {
		return ret;
	}
	// now 'end' point is in solid space

	// front is air and back is solid, this is the impact point

#if 0
	tw.normal = plane.Normal();
	tw.planeDist = plane.d;
	tw.fraction = midf;
#else
	// calculate the return fraction which is nudged off the surface a bit

	const float real_midf = clampf( (d1 - DIST_EPSILON) / (d1 - d2), 0.0f, 1.0f );

	tw.normal = plane.Normal();
	tw.planeDist = plane.d;
	tw.fraction = real_midf;
#endif
	return Trace_Done;
}

void BSP_Tree::TraceAABB(
	const AABB& boxsize, const Vec3D& start, const Vec3D& end,
	FLOAT & fraction, Vec3D & normal
	) const
{
	STraceWorks	tw;

	tw.boxsize = boxsize;
	tw.extents = boxsize.GetHalfSize();


	TraceBox_R( tw, *this, BSP_ROOT_NODE, start, end, 0, 1 );


	fraction = tw.fraction;
	normal = tw.normal;
}

pxVec3 BSP_Tree::CalcSupportingVertex( const pxVec3& dir ) const
{
	UNDONE;
	return pxVec3();
}

SizeT BSP_Tree::GetMemoryUsed() const
{
	return m_nodes.GetAllocatedMemory()
		+ m_planes.GetAllocatedMemory()
		+ sizeof(*this)
		;
}

void BSP_Tree::Serialize( mxArchive& archive )
{
	archive && m_nodes;
	archive && m_planes;

	//@todo: optimize the tree during saving/loading?
	if( archive.IsReading() )
	{
		//
	}
}


// @todo:
// weld coplanar polygon faces
//


NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
