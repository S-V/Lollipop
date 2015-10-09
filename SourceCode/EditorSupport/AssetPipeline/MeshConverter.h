// mesh asset importer
// 
#pragma once

#include <EditorSupport/AssetPipeline.h>
#include <EditorSupport/AssetPipeline/AssetProcessor.h>

struct MeshConverter : AssetProcessor
{
	MeshConverter();
	~MeshConverter();

	virtual EAssetType GetAssetType() const override
	{
		return EAssetType::Asset_Static_Mesh;
	}

	virtual void ProcessFile(
		const ProcessFileInput& input,
		ProcessFileOutput &output
	) override;
};




/*
-----------------------------------------------------------------------------
	ATriangleMeshData
-----------------------------------------------------------------------------
*/
struct ATriangleMeshData
	: public AssetData
{
public:
	mxDECLARE_CLASS(ATriangleMeshData,AssetData);

	typedef TRefPtr< ATriangleMeshData >	Ref;
};

/*
-----------------------------------------------------------------------------
	MeshPart
-----------------------------------------------------------------------------
*/
struct MeshPart
{
	//String	material;
	//AABB	aabb;	// local-space bounding box
	//Sphere	sphere;	// local-space bounding sphere

	U4		baseVertex;	// index of the first vertex
	U4		startIndex;	// offset of the first index
	U4		indexCount;	// number of indices
	U4		vertexCount;// number of vertices

public:
	friend mxArchive& operator && ( mxArchive & serializer, MeshPart & o )
	{
		return serializer && o.baseVertex && o.startIndex && o.indexCount && o.vertexCount;
	}
};

/*
-----------------------------------------------------------------------------
	StaticTriangleMeshData

	this is exactly what is serialized in intermediate format
	(RawMeshData is written (compiled) into binary form)
-----------------------------------------------------------------------------
*/
struct StaticTriangleMeshData
	: public ATriangleMeshData
{
	// vertex data
	TList< Vec3D >		positions;
	TList< Vec2D >		texCoords;
	TList< Vec3D >		normals;
	TList< Vec3D >		tangents;
	//TList< Vec3D >		binormals;

	// index data
	TList< BYTE >		indices;
	UINT				indexStride;	// 2 or 4 bytes

	// rxTopology primType = EPrimitiveType::PT_TriangleList

	AABB	localBounds;	// mesh bounds in local-space

	TList< MeshPart >	batches;

public:
	typedef TRefPtr< StaticTriangleMeshData >	Ref;

	mxDECLARE_CLASS(StaticTriangleMeshData,ATriangleMeshData);

	StaticTriangleMeshData()
	{
	}
	~StaticTriangleMeshData()
	{
	}
	void SetVertexCount( UINT numVertices )
	{
		positions.SetNum( numVertices );
		texCoords.SetNum( numVertices );
		tangents.SetNum( numVertices );
		//binormals.SetNum( numVertices );
		normals.SetNum( numVertices );
	}
	FORCEINLINE UINT NumVertices() const
	{
		return positions.Num();
	}
	FORCEINLINE UINT NumIndices() const
	{
		return indices.GetDataSize() / indexStride;
	}

	virtual void Serialize( mxArchive & s ) override;
};

