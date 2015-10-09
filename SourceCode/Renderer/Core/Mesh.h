/*
=============================================================================
	File:	Mesh.h
	Desc:	graphics meshes
=============================================================================
*/
#pragma once

#include <Renderer/GPU/VertexFormats.hxx>

class RawMeshData;

enum
{
	// vertex positions are always needed
	VERTEX_POSITIONS_STREAM_INDEX = 0,
};

/*
--------------------------------------------------------------
	TVertexSource

	vertex buffers used for rendering
--------------------------------------------------------------
*/
template< class VERTEX >
struct TVertexSource
{
	typedef	VERTEX	VERTEX_TYPE;
	enum { NumStreams = VERTEX::NumStreams };

	dxPtr< ID3D11Buffer >	m_streams[ NumStreams ];

public:
	TVertexSource()
	{
		this->Clear();
	}

	void Clear()
	{
		for( UINT iVertexStream = 0; iVertexStream < NumStreams; iVertexStream++ )
		{
			m_streams[ iVertexStream ] = nil;
		}
	}
	bool IsValid() const
	{
		for( UINT iVertexStream = 0; iVertexStream < NumStreams; iVertexStream++ )
		{
			if( nil == m_streams[ iVertexStream ] ) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE void Bind( ID3D11DeviceContext* context )
	{
		// Set the vertex layout.

		context->IASetInputLayout( VERTEX::layout );

		// Set the vertex buffers.

		const UINT*	strides = VERTEX::GetStreamStridesArray();

		UINT	offsets[ NumStreams ];
		ZERO_OUT(offsets);

		context->IASetVertexBuffers(
			0,
			NumStreams,
			(ID3D11Buffer**) m_streams,
			strides,
			offsets
		);
	}

	// used for rendering to depth texture (for shadow mapping)
	FORCEINLINE void BindPositionsStreamOnly( ID3D11DeviceContext* context )
	{
		context->IASetInputLayout( VERTEX::layout );

		const UINT*	strides = VERTEX::GetStreamStridesArray();

		UINT	offsets[ NumStreams ];
		ZERO_OUT(offsets);

		context->IASetVertexBuffers(
			VERTEX_POSITIONS_STREAM_INDEX,
			1,
			(ID3D11Buffer**) m_streams,
			strides,
			offsets
		);
	}

	void Create( const VERTEX* vertices, const UINT sizeInBytes )
	{
		Assert( sizeInBytes > 0 );
		AssertPtr( vertices );
		mxSTATIC_ASSERT(NumStreams == 1);
		const UINT stride = sizeof VERTEX_TYPE;

		graphics.resources->Create_VertexBuffer(
			m_streams[ 0 ],
			sizeInBytes,
			stride,
			vertices
		);
	}

	// creates HW vertex buffers, uploads vertex data to GPU
	void Load( const VertexData& sourceData )
	{
		Assert(sourceData.streams.Num() == NumStreams);
		//Assert(sourceData.FVF == VERTEX::UID);

		this->Clear();

		const UINT*	strides = VERTEX::GetStreamStridesArray();

		for( UINT iVertexStream = 0; iVertexStream < NumStreams; iVertexStream++ )
		{
			const UINT stride = strides[ iVertexStream ];

			const VertexStream& sourceStream = sourceData.streams[ iVertexStream ];
			Assert(sourceStream.stride == stride);

			graphics.resources->Create_VertexBuffer(
				m_streams[ iVertexStream ],
				sourceStream.data.GetDataSize(),
				sourceStream.stride,
				sourceStream.ToVoidPtr()
			);
		}
	}
};







/*
-----------------------------------------------------------------------------
	rxMesh

	represents a static mesh;
	it's basically a collection of hardware mesh buffers used for rendering.
	raw mesh data is loaded in-place and creates VBs and IB.
	doesn't keep shadow copy in system memory.
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct rxMesh)
	: public SResourceObject
{
	GrVertexData	m_vertexData;	//64 vertex streams
	GrIndexBuffer	m_indexData;	//8 index buffer
	rxTopology		m_topology;		//4 primitive type
	UINT	m_numVertices;	//4 total vertex count
	UINT	m_numIndices;	//4 total index count
	rxAABB		m_localBounds;	//24 mesh bounds in local-space (changes only when geometry changes)

public:
	typedef TResPtr< rxMesh >	Ref;

	static inline EAssetType StaticGetResourceType()
	{
		return EAssetType::Asset_Static_Mesh;
	}

	rxMesh();
	~rxMesh();
};

/*
-----------------------------------------------------------------------------
	rxMeshManager
-----------------------------------------------------------------------------
*/
class rxMeshManager
	: public AResourceManager
	, SingleInstance<rxMeshManager>
{
public:

//	virtual SResourceObject* CreateResource( EResourceType resourceType ) override;

	virtual SResourceObject* LoadResource( SResourceLoadArgs & loadArgs ) override;

//	virtual SResourceObject* GetDefaultResource() override;



	// Prefabs
#if 0//MX_EDITOR
	EdStaticMeshData* Prefab_CreateBox( FLOAT width = 1.0f, FLOAT height = 1.0f, FLOAT depth = 1.0f );
	EdStaticMeshData* Prefab_CreateQuad();
	EdStaticMeshData* Prefab_CreateSphere();
#endif // MX_EDITOR

public_internal:

	rxMeshManager();
	~rxMeshManager();

private:
	TList< rxMesh* >	m_allMeshes;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
