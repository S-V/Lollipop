#pragma once

#include <Core/System/Serialization.h>

#include <Renderer/Core/Geometry.h>

#pragma comment(lib,"Renderer.lib")

#include <Engine/StaticModel.h>




//template<>
//struct TObjectManager< MeshResource >
//	: TResourceManager< MeshResource >
//{};
//
//inline
//DataLoader& operator & ( DataLoader& loader, MeshResource *& o )
//{
//	/* create a valid instance */
//	TResourceManager< MeshResource >::PreLoad( loader.GetStream(), o );
//	/* serialize as usual */
//	*o & loader;
//	return loader;
//}






//////////////////////////////////////////////////////////////////////////

class MeshConverter
{
	SetupCoreUtil	setupCore;

	irr::IrrlichtDevice *	device;

public:

	MeshConverter()
	{
		device = nil;
	}
//-----------------------------------------------------------------------------
	~MeshConverter()
	{
		Close();
	}
//-----------------------------------------------------------------------------
	bool Setup()
	{
		irr::video::E_DRIVER_TYPE	driverType = irr::video::E_DRIVER_TYPE::EDT_NULL;

		this->device = irr::createDevice(
			driverType, irr::core::dimension2d<irr::u32>(100, 100),
			16, false, false, false, nil);
		
		if(!this->device)
		{
			return false;
		}

		return true;
	}
//-----------------------------------------------------------------------------
	bool Convert( const char* inputFile, const char* outputFile )
	{
		irr::scene::IAnimatedMesh*	mesh = this->device->getSceneManager()->getMesh( inputFile );
		if(!mesh)
		{
			return false;
		}

		irr::scene::IMeshManipulator*	meshManip = this->device->getSceneManager()->getMeshManipulator();

		irr::scene::IMesh*	meshWithTangents = meshManip->createMeshWithTangents(mesh);

		if(!meshWithTangents)
		{
			mesh->drop();
			return false;
		}
		
		FileWriter	outputStream(outputFile);

		OSFileName	fileName;
		fileName.SetString(outputFile);

		const bool bOk = ConvertMesh(meshWithTangents,outputStream,fileName.ToChars());

		if(bOk) {
			mxPutf("created mesh %s\n",outputFile);
		}

		meshWithTangents->drop();

		return bOk;
	}
//-----------------------------------------------------------------------------
	void Close()
	{
		if(this->device != nil)
		{
			this->device->drop();
		}
		this->device = nil;
	}
//-----------------------------------------------------------------------------
	void CalculateVertexIndexCount(irr::scene::IMesh* mesh, UINT &vertexCount, UINT &indexCount )
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

		vertexCount = numVertices;
		indexCount = numIndices;
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
	template< typename DEST_TYPE, typename SRC_TYPE >
	void TCopy( DEST_TYPE* dest, const SRC_TYPE* src, const SizeT num )
	{
		//StaticAssert(sizeof DEST_TYPE >= sizeof SRC_TYPE);
		for ( UINT i = 0; i < num; i++ )
		{
			dest[i] = src[i];
		}
	}
	template< typename DEST_TYPE, typename SRC_TYPE >
	void TCopyIndices( DEST_TYPE* dest, const SRC_TYPE* src, const SizeT num, const UINT add )
	{
		//StaticAssert(sizeof DEST_TYPE >= sizeof SRC_TYPE);
		for ( UINT i = 0; i < num; i++ )
		{
			dest[i] = src[i];
		}
	}
//-----------------------------------------------------------------------------
	void CollectIndices(
		void* dest, const UINT destIndexSize,
		const void* src, const UINT numIndices,
		const UINT addedIndexOffset,
		const UINT srcIndexSize )
	{
		if( destIndexSize == sizeof(U16))
		{
			if( destIndexSize == sizeof(U16) )
			{
				TCopyIndices((U16*)dest,(const U16*)src,numIndices,addedIndexOffset);
			}
			else if( destIndexSize == sizeof(U32) )
			{
				TCopyIndices((U16*)dest,(const U32*)src,numIndices,addedIndexOffset);
			}
			else
				Unreachable;
		}
		else if( destIndexSize == sizeof(U32))
		{
			if( srcIndexSize == sizeof(U16) )
			{
				TCopyIndices((U32*)dest,(const U16*)src,numIndices,addedIndexOffset);
			}
			else if( srcIndexSize == sizeof(U32) )
			{
				TCopyIndices((U32*)dest,(const U32*)src,numIndices,addedIndexOffset);
			}
			else
				Unreachable;
		}
		else
			Unreachable;
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
	bool ConvertMesh( irr::scene::IMesh* mesh, mxStreamWriter& outputFile, PCSTR fileName )
	{
		UINT numVertices = 0;
		UINT numIndices = 0;
		CalculateVertexIndexCount(mesh,numVertices,numIndices);


		TList<Vec3D>	positions(EMemHeap::HeapTemp);
		TList<Vec2D>	texCoords(EMemHeap::HeapTemp);
		TList<Vec3D>	tangents(EMemHeap::HeapTemp);
		TList<Vec3D>	binormals(EMemHeap::HeapTemp);
		TList<Vec3D>	normals(EMemHeap::HeapTemp);

		positions.Reserve(numVertices);
		texCoords.Reserve(numVertices);
		tangents.Reserve(numVertices);
		binormals.Reserve(numVertices);
		normals.Reserve(numVertices);


		TList<BYTE>	indices(EMemHeap::HeapTemp);
		const UINT indexSize = sizeof U32;
		indices.SetNum( indexSize * numIndices );


		AABB	meshBounds(_InitInfinity);


		UINT	accumVertexOffset = 0;
		UINT	accumIndexOffset = 0;

		const UINT numSubmeshes = mesh->getMeshBufferCount();

		TList<SubMesh>	subsets(EMemHeap::HeapTemp);
		subsets.SetNum(numSubmeshes);

		for ( UINT iSubMesh = 0; iSubMesh < numSubmeshes; iSubMesh++ )
		{
			irr::scene::IMeshBuffer* subMesh = mesh->getMeshBuffer(iSubMesh);

			Assert(subMesh->getVertexType() == irr::video::E_VERTEX_TYPE::EVT_TANGENTS);

			const irr::video::S3DVertexTangents* verts = 
				cast(const irr::video::S3DVertexTangents*) subMesh->getVertices();

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

			CollectIndices(
				indices.ToPtr() + accumIndexOffset,
				indexSize,
				submeshIndices, submeshIndexCount,
				accumVertexOffset,
				(subMesh->getIndexType() == irr::video::E_INDEX_TYPE::EIT_16BIT) ? sizeof U16 : sizeof U32
				);

			SubMesh & subset = subsets[iSubMesh];
			subset.baseVertex = accumVertexOffset;
			subset.startIndex = accumIndexOffset;
			subset.indexCount = submeshIndexCount;
			subset.vertexCount = submeshVertexCount;
			subset.bounds = CalcSubmeshBounds(subMesh);
			subset.material = "default";

			meshBounds.AddBounds(as_aabb(subMesh->getBoundingBox()));

			accumVertexOffset += submeshVertexCount;
			accumIndexOffset += submeshIndexCount;
		}



		IndexedMesh	indexedMesh;
		indexedMesh.positions = positions.ToPtr();
		indexedMesh.texCoords = texCoords.ToPtr();
		indexedMesh.tangents = tangents.ToPtr();
		indexedMesh.binormals = binormals.ToPtr();
		indexedMesh.normals = normals.ToPtr();
		indexedMesh.numVertices = numVertices;

		indexedMesh.indices = (U32*)indices.ToPtr();
		indexedMesh.numIndices = numIndices;

		indexedMesh.bounds = meshBounds;

		indexedMesh.subsets = subsets.ToPtr();


		MeshResource	meshData;

		ResourceId	resId;
		ResourceUtil::GenerateResourceId( fileName, resId );
		meshData.AssignResourceId(resId);

		meshData.topology = EPrimitiveType::PT_TriangleList;
		rxLocalVertexData::VERTEX_TYPE::CreateVertexData( indexedMesh, meshData.vertexData );
		meshData.indexData.data = indices;
		meshData.indexData.stride = indexSize;
		meshData.batches = subsets;





		DataSaver	writer( outputFile );


		//meshData & writer;
		writer & meshData;




		//FileWriter	fw2( "R:/_/Bin/test.txt" );
		//DataSaver	writer2( fw2 );

		//writer2 & &meshData;


		if(0)
		{
			FileId fileId = "R:/_/Bin/dwarf.geo";

			//MeshResource* mesh = TLoadResource< MeshResource >( fileId );


			FileReader	reader(fileId.ToChars());
			DataLoader	loader( reader );

			MeshResource* mesh = nil;

			loader & mesh;

			AssertPtr(mesh);
		}


		return true;
	}

};

