#include "stdafx.h"
#include "MeshConverter.h"

#include <Core/System/Serialization.h>

#include <Renderer/Core/Geometry.h>

#pragma comment(lib,"Renderer.lib")





void CalculateVertexIndexCount(irr::scene::IMesh* mesh, UINT &outVertexCount, UINT &outIndexCount )
{
	UINT numVertices = 0;
	UINT numIndices = 0;

	const UINT numSubmeshes = mesh->getMeshBufferCount();
	for ( UINT iSubMesh = 0; iSubMesh < numSubmeshes; iSubMesh++ )
	{
		irr::scene::IMeshBuffer* subMesh = mesh->getMeshBuffer(iSubMesh);
		numVertices += subMesh->getVertexCount();
		numIndices += subMesh->getIndexCount();
	}

	outVertexCount = numVertices;
	outIndexCount = numIndices;
}
//-----------------------------------------------------------------------------
FORCEINLINE const Vec3D& as_vec3( const irr::core::vector3df& v )
{
	return (const Vec3D&)v;
}
FORCEINLINE const Vec2D& as_vec2( const irr::core::vector2df& v )
{
	return (const Vec2D&)v;
}
FORCEINLINE const AABB& as_aabb( const irr::core::aabbox3df& bbox )
{
	return (const AABB&)bbox;
}
//-----------------------------------------------------------------------------
Sphere CalcSubmeshBounds(irr::scene::IMeshBuffer* subMesh)
{
	Sphere	bounds;
	irr::core::aabbox3df bbox = subMesh->getBoundingBox();

	bounds.SetOrigin(as_vec3(bbox.getCenter()));
	bounds.SetRadius(bbox.getExtent().getLength() * 0.5f);

	return bounds;
}
//-----------------------------------------------------------------------------
bool ConvertMesh( irr::scene::IMesh* mesh, mxStreamWriter& outputFile, PCSTR relFilePath )
{
	UINT totalNumVertices = 0;
	UINT totalNumIndices = 0;
	CalculateVertexIndexCount(mesh,totalNumVertices,totalNumIndices);


	TList<Vec3D>	positions(EMemHeap::HeapTemp);
	TList<Vec2D>	texCoords(EMemHeap::HeapTemp);
	TList<Vec3D>	tangents(EMemHeap::HeapTemp);
	TList<Vec3D>	binormals(EMemHeap::HeapTemp);
	TList<Vec3D>	normals(EMemHeap::HeapTemp);

	positions.Reserve(totalNumVertices);
	texCoords.Reserve(totalNumVertices);
	tangents.Reserve(totalNumVertices);
	binormals.Reserve(totalNumVertices);
	normals.Reserve(totalNumVertices);


	TList<U32>	indices32bit(EMemHeap::HeapTemp);
	indices32bit.SetNum( totalNumIndices );


	AABB	meshBounds(_InitInfinity);


	UINT	accumVertexOffset = 0;
	UINT	accumIndexCount = 0;

	const UINT numSubmeshes = mesh->getMeshBufferCount();


	TList<SubMesh>	subsets(EMemHeap::HeapTemp);
	subsets.SetNum(numSubmeshes);

	for ( UINT iSubMesh = 0; iSubMesh < numSubmeshes; iSubMesh++ )
	{
		irr::scene::IMeshBuffer* subMesh = mesh->getMeshBuffer(iSubMesh);

		Assert(subMesh->getVertexType() == irr::video::E_VERTEX_TYPE::EVT_TANGENTS);

		const irr::video::S3DVertexTangents* verts = 
			c_cast(const irr::video::S3DVertexTangents*) subMesh->getVertices();

		const UINT submeshVertexCount = subMesh->getVertexCount();

		for ( UINT iVertex = 0; iVertex < submeshVertexCount; iVertex++ )
		{
			const irr::video::S3DVertexTangents& srcVertex = verts[ iVertex ];

			positions.Add( as_vec3(srcVertex.Pos) );
			texCoords.Add( as_vec2(srcVertex.TCoords) );
			tangents.Add( as_vec3(srcVertex.Tangent) );
			binormals.Add( as_vec3(srcVertex.Binormal) );
			normals.Add( as_vec3(srcVertex.Normal) );
		}


		const UINT submeshIndexCount = subMesh->getIndexCount();
		const void* submeshIndices = subMesh->getIndices();

		if( subMesh->getIndexType() == irr::video::E_INDEX_TYPE::EIT_16BIT )
		{
			const U16* srcIndices = c_cast(const U16*)submeshIndices;

			for ( UINT i = 0; i < submeshIndexCount; i++ )
			{
				indices32bit.ToPtr()[ i + accumIndexCount ] = (U32)srcIndices[i] + accumVertexOffset;
			}
		}
		else
		{
			const U32* srcIndices = c_cast(const U32*)submeshIndices;

			for ( UINT i = 0; i < submeshIndexCount; i++ )
			{
				indices32bit.ToPtr()[ i + accumIndexCount ] = srcIndices[i] + accumVertexOffset;
			}
		}



		SubMesh & subset = subsets[iSubMesh];
		subset.baseVertex = accumVertexOffset;
		subset.startIndex = accumIndexCount;
		subset.indexCount = submeshIndexCount;
		subset.vertexCount = submeshVertexCount;
		subset.bounds = CalcSubmeshBounds(subMesh);
		//subset.material = "default";

		meshBounds.AddBounds(as_aabb(subMesh->getBoundingBox()));

		accumVertexOffset += submeshVertexCount;
		accumIndexCount += submeshIndexCount;
	}


	IndexedMesh	indexedMesh;
	indexedMesh.positions = positions.ToPtr();
	indexedMesh.texCoords = texCoords.ToPtr();
	indexedMesh.tangents = tangents.ToPtr();
	indexedMesh.binormals = binormals.ToPtr();
	indexedMesh.normals = normals.ToPtr();
	indexedMesh.numVertices = totalNumVertices;

	indexedMesh.indices = (U32*)indices32bit.ToPtr();
	indexedMesh.numIndices = totalNumIndices;

	indexedMesh.bounds = meshBounds;

	indexedMesh.subsets = subsets.ToPtr();




	rxStaticMesh	staticMesh;

	MeshData &	meshData = staticMesh.GetRawMeshData();

	meshData.topology = EPrimitiveType::PT_TriangleList;
	rxLocalVertexData::VERTEX_TYPE::AssembleVertexData( indexedMesh, meshData.vertexData );

	if( totalNumIndices > MAX_UINT16 )
	{
		const UINT indexSize = sizeof U32;

		meshData.indexData.stride = indexSize;
		meshData.indexData.CopyFrom(indices32bit.ToPtr(),totalNumIndices);
	}
	else
	{
		const UINT indexSize = sizeof U16;

		meshData.indexData.stride = indexSize;
		meshData.indexData.data.SetNum( indexSize * totalNumIndices );

		U16* dest = c_cast(U16*) meshData.indexData.data.ToPtr();

		for ( UINT i = 0; i < totalNumIndices; i++ )
		{
			meshData.indexData.ToU16()[ i ] = indices32bit.ToPtr()[i];
		}
	}


	meshData.batches = subsets;

	{
		const irr::core::aabbox3df& meshAABB = mesh->getBoundingBox();

		const Vec3D center = as_vec3(meshAABB.getCenter());
		const Vec3D extents = as_vec3(meshAABB.getExtent()) * 0.5f;
		meshData.localBounds.Center = as_float3(center);
		meshData.localBounds.Extents = as_float3(extents);
	}

	staticMesh.Save( outputFile );

	return true;
}


/*
=============================================================================
	MeshConverter
=============================================================================
*/
MeshConverter::MeshConverter()
{
	device = nil;
}

MeshConverter::~MeshConverter()
{
	Close();
}

bool MeshConverter::Setup()
{
	irr::video::E_DRIVER_TYPE	driverType = irr::video::E_DRIVER_TYPE::EDT_NULL;

	this->device = irr::createDevice(
		driverType, irr::core::dimension2d<irr::u32>(100, 100),
		16, false, false, false, nil);

	if( nil == this->device)
	{
		return false;
	}

	return true;
}

void MeshConverter::Convert( const Settings& settings )
{
	for( UINT iSrcFile = 0; iSrcFile < settings.inputFiles.Num(); iSrcFile++ )
	{
		const OSPathName& inputFileName = settings.inputFiles[ iSrcFile ];

		String	outputFile(inputFileName.ToChars());
		outputFile.StripPath();

		outputFile = settings.outputDir.ToChars() + outputFile;

		outputFile.StripFileExtension();
		outputFile += '.';
		outputFile += MESH_FILE_EXTENSION;

		this->Convert( inputFileName.ToChars(), outputFile.ToChars() );
	}
}

bool MeshConverter::Convert( const char* inputFile, const char* outputFile )
{
	irr::scene::IAnimatedMesh *	mesh = this->device->getSceneManager()->getMesh( inputFile );
	if( nil == mesh)
	{
		MX_DEBUG_BREAK;
		return false;
	}

	irr::scene::IMeshManipulator *	meshManip = this->device->getSceneManager()->getMeshManipulator();

	irr::scene::IMesh*	meshWithTangents = meshManip->createMeshWithTangents(mesh);

	if( nil == meshWithTangents )
	{
		MX_DEBUG_BREAK;
		mesh->drop();
		return false;
	}

	FileWriter	outputStream(outputFile);
	if (!outputStream.IsOpen())
	{
		MX_DEBUG_BREAK;
		return false;
	}

	// e.g.: "R:/_/Bin"
	// e.g.: "R:/_/Bin"
	// e.g.: "R:/_/Bin"
	const QString appPath = QCoreApplication::applicationDirPath();

	// e.g.: "R:/"
	// e.g.: "R:/TEMP"
	// e.g.: "R:/_/Bin/textures"
	const QString absPath = outputFile;	

	// e.g.: "../../"
	// e.g.: "../../TEMP"
	// e.g.: "textures"
	const QString relPath = QDir(appPath).
		relativeFilePath( absPath );



	const bool bOk = ConvertMesh( meshWithTangents, outputStream, relPath.toAscii().data() );

	if(bOk) {
		mxPutf("created mesh %s\n",outputFile);
	}

	meshWithTangents->drop();

	return bOk;
}

void MeshConverter::Close()
{
	if( this->device != nil )
	{
		this->device->drop();
	}
	this->device = nil;
}

