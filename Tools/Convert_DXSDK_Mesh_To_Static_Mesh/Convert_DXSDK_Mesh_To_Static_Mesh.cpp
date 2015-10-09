// Convert_DXSDK_Mesh_To_Static_Mesh.cpp\
//
#include "stdafx.h"
#pragma hdrstop

#include <Base/Util/LogUtil.h>
#include <Base/Util/FourCC.h>
#include <Core/Serialization.h>
#include <Core/Util/Timer.h>

#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/AssetPipeline/Assets/Mesh/SDKMesh.h>
#include <EditorSupport/AssetPipeline/MeshConverter.h>
#pragma comment( lib, "EditorSupport.lib" )


struct SDKMeshVertex
{
	Vec3D	position;
	Vec3D	normal;
	Vec2D	texCoord;
	Vec3D	tangent;
};

mxAPPLICATION_ENTRY_POINT

int mxAppMain()
{
	SetupBaseUtil	setupBase;
	//FileLogUtil		fileLog;
	SetupCoreUtil	setupCore;


	const String	srcFileName(
		"D:/dev/_assets/_diplomarbeit/iryoku-head/Head.sdkmesh"
		//"D:/research/_current/iryoku-separable-sss-f61b44c/Demo/Media/Head.sdkmesh"
		//"D:/dev/_assets/Head.sdkmesh"
		);


	FileReader	srcFile( srcFileName );
	CHK_VRET_X_IF_NOT( srcFile.IsOpen(), -1 );


	StaticTriangleMeshData	staticMeshData;

	{
		SDKMesh		sdkMesh;
		if( FAILED(sdkMesh.Create(mxTO_UNICODE( srcFileName ))) ) {
			return -1;
		}

		const SDKMESH_HEADER& header = *sdkMesh.GetHeader();
		(void)header;

		const UINT numMeshes = sdkMesh.GetNumMeshes();
		Assert(numMeshes == 1);
		const UINT iFirstMesh = 0;

		const UINT numSubsets = sdkMesh.GetNumSubsets( iFirstMesh );
		Assert( numSubsets == 1 );
		const UINT iFirstSubset = 0;

		const SDKMESH_SUBSET* pSubset = sdkMesh.GetSubset( iFirstMesh, iFirstSubset );
		const UINT numVertices = pSubset->VertexCount;
		const UINT numIndices = pSubset->IndexCount;

		const UINT numVBs = sdkMesh.GetNumVBs();
		Assert( numVBs == 1 );

		const UINT numIBs = sdkMesh.GetNumIBs();
		Assert( numIBs == 1 );

		const SDKMeshVertex* srcVertices = (const SDKMeshVertex*) sdkMesh.GetRawVerticesAt(0);

		const SDKMESH_INDEX_TYPE indexType = sdkMesh.GetIndexType( iFirstMesh );
		Assert( indexType == SDKMESH_INDEX_TYPE::IT_16BIT );

		const UINT16* srcIndices = (const UINT16*) sdkMesh.GetRawIndicesAt(0);

		{
			TList< Vec3D > &	positions	= staticMeshData.positions;
			TList< Vec2D > &	texCoords	= staticMeshData.texCoords;
			TList< Vec3D > &	normals		= staticMeshData.normals;
			//TList< Vec3D > &	binormals	= staticMeshData.binormals;
			TList< Vec3D > &	tangents	= staticMeshData.tangents;

			positions.SetNum( numVertices );
			texCoords.SetNum( numVertices );
			normals.SetNum( numVertices );
			tangents.SetNum( numVertices );

			staticMeshData.localBounds.Clear();

			for( UINT iVertex = 0; iVertex < numVertices; iVertex++ )
			{
				const SDKMeshVertex & srcVertex = srcVertices[ iVertex ];

				Assert( srcVertex.normal.IsNormalized() );
				Assert( srcVertex.tangent.IsNormalized() );

				positions[ iVertex ] = srcVertex.position;
				texCoords[ iVertex ] = srcVertex.texCoord;
				normals[ iVertex ] = srcVertex.normal;
				tangents[ iVertex ] = srcVertex.tangent;

				staticMeshData.localBounds.AddPoint( srcVertex.position );
			}

			staticMeshData.indices.AddBytes( srcIndices, numIndices * sizeof srcIndices[0] );
			staticMeshData.indexStride = sizeof srcIndices[0];

			MeshPart & firstBatch = staticMeshData.batches.Add();
			{
				firstBatch.baseVertex = pSubset->VertexStart;
				firstBatch.startIndex = pSubset->IndexStart;
				firstBatch.indexCount = pSubset->IndexCount;
				firstBatch.vertexCount = pSubset->VertexCount;
			}
		}
	}



	const String	dstPathName(
		//"D:/dev/_assets"
		"R:/"
		);

	String	dstFileName(
		"iryoku-Head"
		);
	//srcFileName.ExtractFileBase( dstFileName );
	dstFileName += ".static_mesh";


	FileWriter	dstFile( dstPathName + dstFileName );
	CHK_VRET_X_IF_NOT( dstFile.IsOpen(), -1 );

	ArchivePODWriter	archive( dstFile );

	staticMeshData.Serialize( archive );

	return 0;
}

