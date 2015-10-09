/*
=============================================================================
	File:	Mesh.cpp
	Desc:	graphics meshes
	ToDo:	don't use operator new(), store them in a contiguous memory pool
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>

/*
-----------------------------------------------------------------------------
	rxMesh
-----------------------------------------------------------------------------
*/
rxMesh::rxMesh()
{
	m_topology = EPrimitiveType::PT_Unknown;
	m_numVertices = m_numIndices = 0;
	rxAABB_Infinity( m_localBounds );
}

rxMesh::~rxMesh()
{
}

/*
-----------------------------------------------------------------------------
	MeshManager
-----------------------------------------------------------------------------
*/
namespace
{
	struct SVertexFormatInfo
	{
		UINT	m_streamStrides[ RX_MAX_INPUT_SLOTS ];
		UINT	m_streamOffsets[ RX_MAX_INPUT_SLOTS ];
		UINT	m_numStreams;	// number of vertex buffers

		TPtr< ID3D11InputLayout >	m_inputLayout;

	public:
		inline SVertexFormatInfo()
		{
			ZERO_OUT(m_streamStrides);
			ZERO_OUT(m_streamOffsets);
			m_numStreams = 0;
		}
		bool ChkIsValid() const
		{
			CHK_VRET_FALSE_IF_NOT( m_numStreams > 0 );
			CHK_VRET_FALSE_IF_NIL( m_inputLayout );
			return true;
		}
	};

	struct MeshManagerData
	{
		TStaticArray< SVertexFormatInfo, GPU::NUM_VERTEX_FORMATS >	m_vertexInfo;
	};

	static TPtr< MeshManagerData >	gData;

	template< class VERTEX_TYPE >
	static
	void F_RegisterVertexFormat()
	{
		SVertexFormatInfo & rVtxInfo = gData->m_vertexInfo[ VERTEX_TYPE::UID ];

		Assert( rVtxInfo.m_inputLayout == nil );
		mxSTATIC_ASSERT( VERTEX_TYPE::NumStreams <= RX_MAX_INPUT_SLOTS );

		TMemCopyArray( rVtxInfo.m_streamStrides, VERTEX_TYPE::GetStreamStridesArray(), VERTEX_TYPE::NumStreams );

		//F_CopyArray( rVtxInfo.m_streamOffsets, VERTEX_TYPE::GetStreamOffsetsArray(), VERTEX_TYPE::NumStreams );
		ZERO_OUT( rVtxInfo.m_streamOffsets );	//unused

		rVtxInfo.m_numStreams = VERTEX_TYPE::NumStreams;

		rVtxInfo.m_inputLayout = VERTEX_TYPE::layout;

		Assert( rVtxInfo.ChkIsValid() );
	}

	static
	void F_RegisterVertexFormats()
	{
	#define RX_PROCESS_VERTEX_TYPE_WITH_MACRO( VERTEX_TYPE )\
						F_RegisterVertexFormat< GPU::VERTEX_TYPE >();

		#include <GPU/VertexFormats.inl>

	#undef RX_PROCESS_VERTEX_TYPE_WITH_MACRO
	}

	static inline
	const SVertexFormatInfo& F_GetVertexFormatInfo( const rxVertexFormatID vertexFormatID )
	{
		return gData->m_vertexInfo[ vertexFormatID ];
	}

	static
	void F_CreateVertexBuffer( ID3D11BufferPtr& rVB, const VertexStream& sourceData )
	{
		graphics.resources->Create_VertexBuffer(
			rVB,
			sourceData.SizeInBytes(),
			sourceData.stride,
			sourceData.ToVoidPtr()
		);
	}

	static
	void F_CreateVertexBuffers( GrVertexData& rVBs, const VertexData& sourceData )
	{
		const rxVertexFormatID vertexFormatID = sourceData.formatID;
		Assert( vertexFormatID != INDEX_NONE );

		const SVertexFormatInfo& vertexFormatInfo = F_GetVertexFormatInfo( vertexFormatID );
		const UINT numStreams = vertexFormatInfo.m_numStreams;

		TMemCopyArray( rVBs.m_streamStrides, vertexFormatInfo.m_streamStrides, numStreams );
		TMemCopyArray( rVBs.m_streamOffsets, vertexFormatInfo.m_streamOffsets, numStreams );
		rVBs.m_numStreams = numStreams;

		rVBs.m_inputLayout = vertexFormatInfo.m_inputLayout;

		for( UINT iVertexStream = 0; iVertexStream < numStreams; iVertexStream++ )
		{
			const UINT stride = vertexFormatInfo.m_streamStrides[ iVertexStream ];

			const VertexStream& sourceStream = sourceData.streams[ iVertexStream ];
			Assert(sourceStream.stride == stride);

			F_CreateVertexBuffer( rVBs.m_streams[ iVertexStream ], sourceStream );
		}
	}

	static
	void F_CreateIndexBuffer( GrIndexBuffer& rIB, const IndexData& sourceData )
	{
		const void* indexData = sourceData.ToVoidPtr();
		const UINT indexDataSize = sourceData.SizeInBytes();
		const UINT indexStride = sourceData.stride;

		rIB.Create( indexData, indexDataSize, indexStride );
	}

	static
	void F_LoadMesh( rxMesh &theMesh, const RawMeshData& sourceData )
	{
		F_CreateVertexBuffers( theMesh.m_vertexData, sourceData.vertexData );
		F_CreateIndexBuffer( theMesh.m_indexData, sourceData.indexData );
		theMesh.m_topology = sourceData.topology;

		theMesh.m_numVertices = sourceData.vertexData.streams[0].NumVertices();
		theMesh.m_numIndices = sourceData.indexData.NumIndices();

		theMesh.m_localBounds = sourceData.localBounds;
	}

}//namespace

rxMeshManager::rxMeshManager()
{
	gData.ConstructInPlace();

	F_RegisterVertexFormats();

	gCore.resources->SetManager( Asset_Static_Mesh, this );
}

rxMeshManager::~rxMeshManager()
{
	m_allMeshes.DeleteContents();

	gData.Destruct();
}

SResourceObject* rxMeshManager::LoadResource( SResourceLoadArgs & loadArgs )
{
	RawMeshData *	meshData;

	InPlace::Loader		loader( loadArgs.Map(), loadArgs.GetSize() );
	loader.LoadObject( meshData );

	CHK_VRET_NIL_IF_NOT(meshData->DbgCheckValid());

	rxMesh* newMesh = new rxMesh();

	F_LoadMesh( *newMesh, *meshData );

	m_allMeshes.Add( newMesh );

	return newMesh;
}


#if 0//MX_EDITOR

EdStaticMeshData* rxMeshManager::Prefab_CreateBox( FLOAT width, FLOAT height, FLOAT depth )
{
	//mxSWIPED("Ogre");

	const int NUM_VERTICES = 4 * 6; // 4 vertices per side * 6 sides
	const int NUM_ENTRIES_PER_VERTEX = 8;
	const int NUM_VERTEX_ENTRIES = NUM_VERTICES * NUM_ENTRIES_PER_VERTEX;
	const int NUM_INDICES = 3 * 2 * 6; // 3 indices per face * 2 faces per side * 6 sides

	//const float CUBE_SIZE = 1.0f;
	//const float CUBE_HALF_SIZE = CUBE_SIZE / 2.0f;

	(void)NUM_VERTEX_ENTRIES;

#if 0
	// Create 4 vertices per side instead of 6 that are shared for the whole cube.
	// The reason for this is with only 6 vertices the normals will look bad
	// since each vertex can "point" in a different direction depending on the face it is included in.

	Vec3D	positions[ NUM_VERTICES ] =
	{
		// front side
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),       // pos
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE,  CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE,  CUBE_HALF_SIZE, CUBE_HALF_SIZE),

		// back side
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),

		// left side
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),

		// right side
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE),

		// up side
		Vec3D(-CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE),

		// down side
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE),
		Vec3D(CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),
		Vec3D(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE),
	};

	Vec3D	normals[ NUM_VERTICES ] =
	{
		// front side
		Vec3D(0,0,1),  // normal
		Vec3D(0,0,1),
		Vec3D(0,0,1),
		Vec3D(0,0,1),

		// back side
		Vec3D(0,0,-1),
		Vec3D(0,0,-1),
		Vec3D(0,0,-1),
		Vec3D(0,0,-1),

		// left side
		Vec3D(-1,0,0),
		Vec3D(-1,0,0),
		Vec3D(-1,0,0),
		Vec3D(-1,0,0),

		// right side
		Vec3D(1,0,0),
		Vec3D(1,0,0),
		Vec3D(1,0,0),
		Vec3D(1,0,0),

		// up side
		Vec3D(0,1,0),
		Vec3D(0,1,0),
		Vec3D(0,1,0),
		Vec3D(0,1,0),

		// down side
		Vec3D(0,-1,0),
		Vec3D(0,-1,0),
		Vec3D(0,-1,0),
		Vec3D(0,-1,0),
	};

	Vec2D	texCoords[ NUM_VERTICES ] =
	{
		// front side
		Vec2D(0,1),    // texcoord
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),

		// back side
		Vec2D(0,1),
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),

		// left side
		Vec2D(0,1),
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),

		// right side
		Vec2D(0,1),
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),

		// up side
		Vec2D(0,1),
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),

		// down side
		Vec2D(0,1),
		Vec2D(1,1),
		Vec2D(1,0),
		Vec2D(0,0),
	};

	U2 indices[ NUM_INDICES ] =
	{
		// front
		0,1,2,
		0,2,3,

		// back
		4,5,6,
		4,6,7,

		// left
		8,9,10,
		8,10,11,

		// right
		12,13,14,
		12,14,15,

		// up
		16,17,18,
		16,18,19,

		// down
		20,21,22,
		20,22,23
	};

#else

	Vec3D	positions[ NUM_VERTICES ] =
	{
		// Fill in the front face vertex data.
		Vec3D(-0.5f, -0.5f, -0.5f),
		Vec3D(-0.5f,  0.5f, -0.5f),
		Vec3D( 0.5f,  0.5f, -0.5f),
		Vec3D( 0.5f, -0.5f, -0.5f),

		// Fill in the back face vertex data.
		Vec3D(-0.5f, -0.5f, 0.5f),
		Vec3D( 0.5f, -0.5f, 0.5f),
		Vec3D( 0.5f,  0.5f, 0.5f),
		Vec3D(-0.5f,  0.5f, 0.5f),

		// Fill in the top face vertex data.
		Vec3D(-0.5f, 0.5f, -0.5f),
		Vec3D(-0.5f, 0.5f,  0.5f),
		Vec3D( 0.5f, 0.5f,  0.5f),
		Vec3D( 0.5f, 0.5f, -0.5f),

		// Fill in the bottom face vertex data.
		Vec3D(-0.5f, -0.5f, -0.5f),
		Vec3D( 0.5f, -0.5f, -0.5f),
		Vec3D( 0.5f, -0.5f,  0.5f),
		Vec3D(-0.5f, -0.5f,  0.5f),

		// Fill in the left face vertex data.
		Vec3D(-0.5f, -0.5f,  0.5f),
		Vec3D(-0.5f,  0.5f,  0.5f),
		Vec3D(-0.5f,  0.5f, -0.5f),
		Vec3D(-0.5f, -0.5f, -0.5f),

		// Fill in the right face vertex data.
		Vec3D( 0.5f, -0.5f, -0.5f),
		Vec3D( 0.5f,  0.5f, -0.5f),
		Vec3D( 0.5f,  0.5f,  0.5f),
		Vec3D( 0.5f, -0.5f,  0.5f),
	};

	Vec3D	tangents[ NUM_VERTICES ] =
	{
		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),

		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),

		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),
		Vec3D( 1.0f, 0.0f, 0.0f),

		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),

		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),

		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 
	};

	Vec3D	normals[ NUM_VERTICES ] =
	{
		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),
		Vec3D(0.0f, 0.0f, -1.0f),

		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 
		Vec3D(0.0f, 0.0f, 1.0f), 

		Vec3D(0.0f, 1.0f, 0.0f), 
		Vec3D(0.0f, 1.0f, 0.0f), 
		Vec3D(0.0f, 1.0f, 0.0f), 
		Vec3D(0.0f, 1.0f, 0.0f), 

		Vec3D(0.0f, -1.0f, 0.0f),
		Vec3D(0.0f, -1.0f, 0.0f),
		Vec3D(0.0f, -1.0f, 0.0f),
		Vec3D(0.0f, -1.0f, 0.0f),

		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),
		Vec3D(-1.0f, 0.0f, 0.0f),

		Vec3D(1.0f, 0.0f, 0.0f), 
		Vec3D(1.0f, 0.0f, 0.0f), 
		Vec3D(1.0f, 0.0f, 0.0f), 
		Vec3D(1.0f, 0.0f, 0.0f), 
	};

	Vec2D	texCoords[ NUM_VERTICES ] =
	{
		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),
		Vec2D(1.0f, 1.0f),

		Vec2D(1.0f, 1.0f),
		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),

		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),
		Vec2D(1.0f, 1.0f),

		Vec2D(1.0f, 1.0f),
		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),

		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),
		Vec2D(1.0f, 1.0f),

		Vec2D(0.0f, 1.0f),
		Vec2D(0.0f, 0.0f),
		Vec2D(1.0f, 0.0f),
		Vec2D(1.0f, 1.0f),
	};

	U2 indices[ NUM_INDICES ] =
	{
		// front
		0,1,2,
		0,2,3,

		// back
		4,5,6,
		4,6,7,

		// left
		8,9,10,
		8,10,11,

		// right
		12,13,14,
		12,14,15,

		// up
		16,17,18,
		16,18,19,

		// down
		20,21,22,
		20,22,23
	};

#endif

	IndexedMesh		srcMesh;
	{
		srcMesh.positions = positions;
		srcMesh.texCoords = texCoords;
		srcMesh.tangents = tangents;
		srcMesh.binormals = normals;mxUNDONE;
		srcMesh.normals = normals;

		srcMesh.numVertices = NUM_VERTICES;

		srcMesh.indices = indices;
		srcMesh.indexStride = sizeof indices[0];
		srcMesh.numIndices = NUM_INDICES;

		const Vec3D	halfSize(Vec3D(width,height,depth)*0.5f);
		srcMesh.bounds.Set( -halfSize, halfSize );

		EdSubMesh	batch;
		batch.baseVertex = 0;
		batch.startIndex = 0;
		batch.indexCount = srcMesh.numIndices;
		batch.vertexCount = srcMesh.numVertices;
		batch.bounds = srcMesh.bounds.ToSphere();
		batch.material = MX_NULL_RESOURCE_GUID;

		srcMesh.subsets = &batch;
		srcMesh.numSubsets = 1;

		srcMesh.primType = EPrimitiveType::PT_TriangleList;
	}


	EdStaticMeshData* pNewMesh = new EdStaticMeshData();
	pNewMesh->Build( srcMesh );
	pNewMesh->name = "Box";
	return pNewMesh;
}

EdStaticMeshData* rxMeshManager::Prefab_CreateQuad()
{
	enum { NUM_VERTICES = 4 };
	enum { NUM_INDICES = 6 };

	// Create vertex buffer.

	const FLOAT halfSizeX = 0.5f;
	const FLOAT halfSizeZ = 0.5f;
	const FLOAT height = 0.0f;

	// bottom left
	// top left
	// top right
	// bottom right

	Vec3D	positions[ NUM_VERTICES ] =
	{
		Vec3D( -halfSizeX, height, -halfSizeZ ),
		Vec3D( -halfSizeX, height,  halfSizeZ ),
		Vec3D(  halfSizeX, height,  halfSizeZ ),
		Vec3D(  halfSizeX, height, -halfSizeZ ),
	};

	Vec3D	tangents[ NUM_VERTICES ] =
	{
		Vec3D::vec3_unit_z,
		Vec3D::vec3_unit_z,
		Vec3D::vec3_unit_z,
		Vec3D::vec3_unit_z,
	};

	Vec3D	binormals[ NUM_VERTICES ] =
	{
		Vec3D::vec3_unit_x,
		Vec3D::vec3_unit_x,
		Vec3D::vec3_unit_x,
		Vec3D::vec3_unit_x,
	};

	Vec3D	normals[ NUM_VERTICES ] =
	{
		Vec3D::vec3_unit_y,
		Vec3D::vec3_unit_y,
		Vec3D::vec3_unit_y,
		Vec3D::vec3_unit_y,
	};

	Vec2D	UVScale(1.0f);

	Vec2D	texCoords[ NUM_VERTICES ] =
	{
		Vec2D( 0.0f,				1.0f * UVScale.y	),
		Vec2D( 0.0f,				0.0f				),
		Vec2D( 1.0f * UVScale.x,	0.0f				),
		Vec2D( 1.0f * UVScale.x,	1.0f * UVScale.y	),
	};

	// Create the index buffer.

	U2 indices[ NUM_INDICES ] =
	{
		0,	1,	2,
		0,	2,	3,
	};

	IndexedMesh		srcMesh;
	{
		srcMesh.positions = positions;
		srcMesh.texCoords = texCoords;
		srcMesh.tangents = tangents;
		srcMesh.binormals = binormals;
		srcMesh.normals = normals;

		srcMesh.numVertices = NUM_VERTICES;

		srcMesh.indices = indices;
		srcMesh.indexStride = sizeof indices[0];
		srcMesh.numIndices = NUM_INDICES;

		const Vec3D	halfSize(Vec3D(halfSizeX,0,halfSizeZ));
		srcMesh.bounds.Set( -halfSize, halfSize );

		EdSubMesh	batch;
		batch.baseVertex = 0;
		batch.startIndex = 0;
		batch.indexCount = srcMesh.numIndices;
		batch.vertexCount = srcMesh.numVertices;
		batch.bounds = srcMesh.bounds.ToSphere();
		batch.material = MX_NULL_RESOURCE_GUID;

		srcMesh.subsets = &batch;
		srcMesh.numSubsets = 1;

		srcMesh.primType = EPrimitiveType::PT_TriangleList;
	}

	EdStaticMeshData* pNewMesh = new EdStaticMeshData();
	pNewMesh->Build( srcMesh );
	pNewMesh->name = "Quad";
	return pNewMesh;
}

EdStaticMeshData* rxMeshManager::Prefab_CreateSphere()
{
	Unimplemented;
	return nil;
}
#endif // MX_EDITOR


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
