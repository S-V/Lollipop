#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Renderer/Core/Geometry.h>

#include <EditorSupport/Util/IrrlichtBridge.h>
#include <EditorSupport/AssetPipeline/Assets/Mesh/SDKMesh.h>

#include "MeshConverter.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace lol
{

using namespace irr;

IrrlichtDevice *		GDevice = 0;
video::IVideoDriver *	GDriver = 0;
scene::ISceneManager *	GSmgr = 0;
bool					bOpened = false;

void SetupMeshLoader()
{
	mxPut("\nMesh importer: Initializing...\n");
	GDevice = createDevice(video::EDT_NULL, core::dimension2d<u32>(100, 100));
	GDriver = GDevice->getVideoDriver();
	GSmgr = GDevice->getSceneManager();
	bOpened = true;
}

void CloseMeshLoader()
{
	mxPut("\nMesh importer: Shutting down...\n");
	if( GDevice ) {
		GDevice->drop();
		GDevice = 0;
	}
	GDriver = 0;
	GSmgr = 0;
	bOpened = false;
}


struct Options
{
	float			uvScale;
	const float *	transform;	// [optional] initial transform, [4]x[4] floats

	enum
	{
		PRINT_INFO = (1<<0),	// verbose
		DEBUG_INFO = (1<<1),

		ALL = -1
	};
	unsigned flags;

public:
	Options()
		: uvScale( 0.003f )
		, transform(nil)
		, flags(ALL)
	{}
};

static
void F_DbgPrintMeshInfo( scene::IMesh* theMesh )
{
	mxPutf( "\nMeshbuffers: %u.\n{", theMesh->getMeshBufferCount() );
	for( u32 iBuf = 0; iBuf < theMesh->getMeshBufferCount(); iBuf++ )
	{
		scene::IMeshBuffer * pBuf = theMesh->getMeshBuffer(iBuf);
		mxPutf( "\nMeshbuffer[%u]: %u verts, %u indices.",
			iBuf, pBuf->getVertexCount(), pBuf->getIndexCount() );
	}
	mxPutf( "\n}\n" );
}

static
scene::IMesh* F_CalcMeshData( scene::IMesh* pMesh, const Options& options )
{
	if( options.transform )
	{
		core::matrix4 m;
		m.setM( options.transform );
		GSmgr->getMeshManipulator()->transformMesh( pMesh, m );
	}

	GSmgr->getMeshManipulator()->makePlanarTextureMapping(
		pMesh, options.uvScale );

	scene::IMesh* tangentMesh = GSmgr->getMeshManipulator()->
		createMeshWithTangents( pMesh );

	return tangentMesh;
}

static
void F_CalcMeshInfo( scene::IMesh* theMesh,
					size_t &numVertices, size_t &numIndices, size_t &sizeOfIndices )
{
	numVertices = 0;
	numIndices = 0;
	sizeOfIndices = 0;

	for( u32 iBuf = 0; iBuf < theMesh->getMeshBufferCount(); iBuf++ )
	{
		scene::IMeshBuffer * pBuf = theMesh->getMeshBuffer(iBuf);

		Assert( pBuf->getVertexType() == video::E_VERTEX_TYPE::EVT_TANGENTS );

		numVertices += pBuf->getVertexCount();

		numIndices += pBuf->getIndexCount();

		const u32 indexSize = (pBuf->getIndexType() == video::EIT_16BIT) ? 2 : 4;
		sizeOfIndices += numIndices * indexSize;
	}
}

template< typename DST_INDEX_TYPE, typename SRC_INDEX_TYPE >
void F_CopyIndices( DST_INDEX_TYPE *destIndices, const SRC_INDEX_TYPE* srcIndices,
				   UINT numIndices, UINT indexOffset = 0 )
{
	mxSTATIC_ASSERT( sizeof SRC_INDEX_TYPE == 2 || sizeof SRC_INDEX_TYPE == 4 );
	//mxSTATIC_ASSERT( sizeof destIndices[0] >= sizeof srcIndices[0] );
	Assert( numIndices > 2 );

	// if copying from 32-bit to 16-bit
	if( sizeof destIndices[0] < sizeof srcIndices[0] )
	{
		Assert( numIndices <= MAX_UINT16 );
	}

	for( UINT i=0; i < numIndices; i++ )
	{
		destIndices[i] = srcIndices[i] + indexOffset;
	}
}

static
StaticTriangleMeshData* F_CreateStaticMesh( scene::IMesh * theMesh,
										   const size_t numVertices, const size_t numIndices )
{
	StaticTriangleMeshData* newMeshData = new StaticTriangleMeshData();
	//newMeshData->SetVertexCount( numVertices );

	TList< Vec3D > &	positions	= newMeshData->positions;
	TList< Vec2D > &	texCoords	= newMeshData->texCoords;
	TList< Vec3D > &	normals		= newMeshData->normals;
	//TList< Vec3D > &	binormals	= newMeshData->binormals;
	TList< Vec3D > &	tangents	= newMeshData->tangents;

	positions.Reserve( numVertices );
	texCoords.Reserve( numVertices );
	normals.Reserve( numVertices );
	tangents.Reserve( numVertices );



	// 32-bit indices
	TList< UINT32 >		indices32( EMemHeap::HeapTemp );
	indices32.SetNum( numIndices );



	const UINT numBatches = theMesh->getMeshBufferCount();
	newMeshData->batches.SetNum( numBatches );

	//size_t  iCurrVertex = 0;
	//size_t  iCurrIndex = 0;
	size_t  nTotalVertexCount = 0;
	size_t  nTotalIndexCount = 0;




	//*****************************************************************
	//*** Collect all vertex and index data into one mesh buffer
	//*** and adjust indices accordingly
	//*****************************************************************

	for( u32 iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		scene::IMeshBuffer * pSrcBuf = theMesh->getMeshBuffer( iBatch );

		const UINT numVertsInBatch = pSrcBuf->getVertexCount();
		const UINT numIndicesInBatch = pSrcBuf->getIndexCount();

		const UINT numVerticesSoFar = nTotalVertexCount;
		const UINT numIndicesSoFar = nTotalIndexCount;


		MeshPart & newBatch = newMeshData->batches[ iBatch ];
		{
			newBatch.baseVertex = numVerticesSoFar;	// A value added to each index before reading a vertex from the vertex buffer.
			newBatch.startIndex = numIndicesSoFar;	// The location of the first index read by the GPU from the index buffer.
			newBatch.indexCount = numIndicesInBatch;
			newBatch.vertexCount = numVertsInBatch;
		}


		//*****************************************************************
		//*** Collect vertices
		//*****************************************************************
		Assert( numVerticesSoFar == positions.Num() );//just in case
		Assert( numVerticesSoFar == normals.Num() );//just in case
		Assert( numVerticesSoFar == tangents.Num() );//just in case
		Assert( numVerticesSoFar == texCoords.Num() );//just in case
		{
			Assert( pSrcBuf->getVertexType() == video::E_VERTEX_TYPE::EVT_TANGENTS );
			const video::S3DVertexTangents * pVertices = (const video::S3DVertexTangents*) pSrcBuf->getVertices();

			for( u32 iVertex = 0; iVertex < numVertsInBatch; iVertex++ )
			{
				const video::S3DVertexTangents & srcVertex = pVertices[ iVertex ];

				positions.Add( ToMyVec3D( srcVertex.Pos ) );
				normals.Add( ToMyVec3D( srcVertex.Normal ) );
				tangents.Add( ToMyVec3D( srcVertex.Tangent ) );
				texCoords.Add( ToMyVec2D( srcVertex.TCoords ) );
			}
		}


		//*****************************************************************
		//*** Collect indices
		//*****************************************************************
		{
			UINT32* destIndices = indices32.ToPtr() + numIndicesSoFar;

			if( pSrcBuf->getIndexType() == video::E_INDEX_TYPE::EIT_16BIT )
			{
				const u16* srcIndices = (const u16*) pSrcBuf->getIndices();
				F_CopyIndices( destIndices, srcIndices, numIndicesInBatch );
			}
			else
			{
				Assert( pSrcBuf->getIndexType() == video::E_INDEX_TYPE::EIT_32BIT );

				const u32* srcIndices = (const u32*) pSrcBuf->getIndices();
				F_CopyIndices( destIndices, srcIndices, numIndicesInBatch );
			}
		}

		nTotalVertexCount += numVertsInBatch;
		nTotalIndexCount += numIndicesInBatch;

	}//for each mesh batch


	Assert( nTotalVertexCount == numVertices );
	Assert( nTotalIndexCount == numIndices );


	const bool b16bitIndicesAreEnough = (nTotalIndexCount <= MAX_UINT16);

	if( b16bitIndicesAreEnough )
	{
		const size_t sizeOfIndexData = nTotalIndexCount * (sizeof UINT16);
		newMeshData->indices.SetNum( sizeOfIndexData );

		const u32* srcIndices = indices32.ToPtr();
		u16* dstIndices = (u16*) newMeshData->indices.ToPtr();

		F_CopyIndices( dstIndices, srcIndices, nTotalIndexCount );

		newMeshData->indexStride = sizeof UINT16;
	}
	else
	{
		const size_t sizeOfIndexData = indices32.GetDataSize();
		newMeshData->indices.SetNum( sizeOfIndexData );
		MemCopy( newMeshData->indices.ToPtr(), indices32.ToPtr(), sizeOfIndexData );

		newMeshData->indexStride = sizeof UINT32;
	}



	newMeshData->localBounds = ToMyAABB( theMesh->getBoundingBox() );


	return newMeshData;
}




static
const char* D3DDECLTYPE_To_Chars( UINT/*D3DDECLTYPE*/ declType )
{
#define CASE_ENUM_TO_STR( ENUM )	if( declType == ENUM ) { return #ENUM; }

	//switch( declType )
	{
		CASE_ENUM_TO_STR( D3DDECLTYPE_FLOAT1 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_FLOAT2 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_FLOAT3 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_FLOAT4 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_D3DCOLOR );

		CASE_ENUM_TO_STR( D3DDECLTYPE_UBYTE4 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_SHORT2 );
		CASE_ENUM_TO_STR( D3DDECLTYPE_SHORT4 );
	}

#undef CASE_ENUM_TO_STR

	mxDBG_UNREACHABLE;
	return "<D3DDECLTYPE-ERROR-UNKNOWN>";
}


static
const char* D3DDECLUSAGE_To_Chars( UINT/*D3DDECLUSAGE*/ declUsage )
{
#define CASE_ENUM_TO_STR( ENUM )	if( declUsage == ENUM ) { return #ENUM; }

	//switch( declType )
	{
		CASE_ENUM_TO_STR( D3DDECLUSAGE_POSITION );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_BLENDWEIGHT );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_BLENDINDICES );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_NORMAL );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_PSIZE );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_TEXCOORD );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_TANGENT );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_BINORMAL );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_TESSFACTOR );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_POSITIONT );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_COLOR );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_FOG );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_DEPTH );
		CASE_ENUM_TO_STR( D3DDECLUSAGE_SAMPLE );
	}

#undef CASE_ENUM_TO_STR

	mxDBG_UNREACHABLE;
	return "<D3DDECLUSAGE-ERROR-UNKNOWN>";
}


static
void F_Dbg_Print_D3D9_Vertex_Elem( const D3DVERTEXELEMENT9* pD3D9VertexElem )
{
	DBGOUT(
		"============\n"
		"Stream = %u,\n"
		"Offset = %u,\n"
		"Type = %s,\n"
		"Method = %u,\n"
		"Usage = %s,\n"
		"UsageIndex = %u\n"
		"============\n"
		, (UINT)pD3D9VertexElem->Stream
		, (UINT)pD3D9VertexElem->Offset
		, D3DDECLTYPE_To_Chars(pD3D9VertexElem->Type)
		, (UINT)pD3D9VertexElem->Method
		, D3DDECLUSAGE_To_Chars(pD3D9VertexElem->Usage)
		, (UINT)pD3D9VertexElem->UsageIndex
		);
}

#if 0
static
StaticTriangleMeshData* F_Create_Static_Mesh_From_SDK_SubMesh(
	SDKMesh& sdkMesh,
	const UINT iMesh
	)
{
	const UINT numVBs = sdkMesh.GetNumVBs();
	Assert( numVBs == 1 );
	for( UINT iVB = 0; iVB < numVBs; iVB++ )
	{
		const D3DVERTEXELEMENT9* pD3D9VertexElem = sdkMesh.VBElements( iVB );
		F_Dbg_Print_D3D9_Vertex_Elem( pD3D9VertexElem );

		Assert( pD3D9VertexElem->Stream == 0 );
		Assert( pD3D9VertexElem->Offset == 0 );
		Assert( pD3D9VertexElem->Type == D3DDECLTYPE_FLOAT3 );
		Assert( pD3D9VertexElem->Usage == D3DDECLUSAGE_POSITION );
	}

	const SDKMESH_MESH* pMesh = sdkMesh.GetMesh( iMesh );

	const UINT numSubsets = sdkMesh.GetNumSubsets( iMesh );
	Assert( numSubsets == 1 );

	const SDKMESH_SUBSET* pSubset = sdkMesh.GetSubset( iMesh, 0 );
	const UINT numVertices = pSubset->VertexCount;

	const Vec3D* srcPositions = (const Vec3D*) sdkMesh.GetRawVerticesAt(0);

	TList< irr::video::S3DVertex >	dstVertices;
	dstVertices.SetNum( numVertices );

	for( UINT iVertex = 0; iVertex < numVertices; iVertex++ )
	{
		video::S3DVertex & dstVertex = dstVertices[ iVertex ];

		dstVertex.Pos = irr::FromMyVec3D( srcPositions[ iVertex ] );
		//srcVertex.Normal.;
		//srcVertex.Tangent;
		//srcVertex.TCoords;
	}


	//scene::SMesh	irrMesh;

	//for( UINT iSubset = 0; iSubset < numSubsets; iSubset++ )
	//{
	//	const SDKMESH_SUBSET* pSubset = sdkMesh.GetSubset( iMesh, iSubset );

	//	const D3D11_PRIMITIVE_TOPOLOGY primType = SDKMesh::GetPrimitiveType11( (SDKMESH_PRIMITIVE_TYPE) pSubset->PrimitiveType );

	//	Assert( primType == D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	//	scene::SMeshBuffer* pNewIrrMeshBuffer = new scene::SMeshBuffer();

	//	pNewIrrMeshBuffer->append( );


	//	//video::S3DVertex * pDstVerticesArray = pNewIrrMeshBuffer->getVertices();

	//	for( u32 iVertex = 0; iVertex < numVertsInBatch; iVertex++ )
	//	{
	//		video::S3DVertex & dstVertex = pVertices[ iVertex ];

	//		positions.Add( ToMyVec3D( srcVertex.Pos ) );
	//		normals.Add( ToMyVec3D( srcVertex.Normal ) );
	//		tangents.Add( ToMyVec3D( srcVertex.Tangent ) );
	//		texCoords.Add( ToMyVec2D( srcVertex.TCoords ) );
	//	}
	//}



	StaticTriangleMeshData* newMeshData = new StaticTriangleMeshData();
	//newMeshData->SetVertexCount( numVertices );
	return newMeshData;
}

static
StaticTriangleMeshData* F_Create_Static_Mesh_From_SDK_Mesh( const char* filename )
{
	SDKMesh		sdkMesh;

	if( FAILED(sdkMesh.Create(mxTO_UNICODE(filename ))) ) {
		return nil;
	}

	const UINT numMeshes = sdkMesh.GetNumMeshes();
	if( numMeshes > 1 ) {
		Unimplemented;
		return nil;
	}

	//for( UINT iMesh = 0; iMesh < numMeshes; iMesh++ )
	//{
	//	const SDKMESH_MESH* pMesh = sdkMesh.GetMesh( iMesh );

	//	const UINT numSubsets = sdkMesh.GetNumSubsets( iMesh );
	//	for( UINT iSubset = 0; iSubset < numSubsets; iSubset++ )
	//	{
	//		const SDKMESH_SUBSET* pSubset = sdkMesh.GetSubset( iMesh, iSubset );
	//	}
	//}

	return F_Create_Static_Mesh_From_SDK_SubMesh( sdkMesh, 0 );
}
#endif

ATriangleMeshData* LoadMeshFromFile( const char* filename, const Options& options = Options() )
{
	StaticTriangleMeshData* newMeshData = nil;

	if( !bOpened ) {
		mxPutf("\nError: LoadMeshFromFile() - Resource loader must be initialized first!\n");
		return nil;
	}

	String	strFileName( filename );
	String	strFileExt;
	strFileName.ExtractFileExtension( strFileExt );
	if( strFileExt.Icmp("static_mesh") == 0 )
	{
		FileReader	srcFile( filename );
		CHK_VRET_NIL_IF_NOT( srcFile.IsOpen() );

		ArchivePODReader	archive( srcFile );

		newMeshData = new StaticTriangleMeshData();
		newMeshData->Serialize( archive );

		return newMeshData;
	}

	scene::IAnimatedMesh* aMesh = GSmgr->getMesh( filename );
	if( !aMesh ) {
		mxDEBUG_BREAK;
		mxPutf("\nError: LoadMeshFromFile() - Failed to load mesh from file '%s'\n", filename );
		return nil;
	}

	const UINT numAnimFrames = aMesh->getFrameCount();
	if( numAnimFrames > 1 ) {
		mxDEBUG_BREAK;
		DEVOUT("Mesh: numAnimFrames = %u\n",numAnimFrames);
	}


	//*****************************************************************
	//*** Calculate mesh data
	//*****************************************************************

	const u32 startTime = GDevice->getTimer()->getRealTime();

	if( options.flags & Options::PRINT_INFO )
	{
		mxPutf( "\nLoading mesh '%s'...\nCalculating normals, tangents and texture coordinates...\n", filename );
	}

	irr::RefPtr< scene::IMesh >	tangentMesh = F_CalcMeshData( aMesh->getMesh(0), options );

	if( !tangentMesh )
	{
		mxPutf("\nError: LoadMeshFromFile() - Failed to create mesh '%s' with tangents\n", filename );
		return nil;
	}

	if( options.flags & Options::PRINT_INFO )
	{
		u32 endTime = GDevice->getTimer()->getRealTime();
		mxPutf( "\nTaken %u msec to calculate vertex attributes", endTime - startTime );
	}

	//*****************************************************************
	//*** Convert mesh into our own formats
	//*****************************************************************

	scene::IMesh * theMesh = tangentMesh;

	if( options.flags & Options::DEBUG_INFO )
	{
		F_DbgPrintMeshInfo( theMesh );
	}

	size_t  numVertices = 0;
	size_t  numIndices = 0;
	size_t  sizeOfIndices = 0;	// in bytes

	F_CalcMeshInfo( theMesh, numVertices, numIndices, sizeOfIndices );

	const bool bUse32bitIndices = (numVertices > MAX_UINT16-1);	// 65535

	if( options.flags & Options::PRINT_INFO )
	{
		mxPutf( "\nMesh '%s' consists of %u vertices and %u indices, using %s-bit indices\n",
			filename, numVertices, numIndices, bUse32bitIndices?"32":"16" );
	}

	newMeshData = F_CreateStaticMesh( theMesh, numVertices, numIndices );
	if( !newMeshData ) {
		return nil;
	}

	if( options.flags & Options::PRINT_INFO )
	{
		u32 endTime = GDevice->getTimer()->getRealTime();
		mxPutf( "\nTaken %u msec to load mesh '%s'...\n", endTime - startTime, filename );
	}

	return newMeshData;
}

}//end of namespace lol

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
-----------------------------------------------------------------------------
	MeshConverter
-----------------------------------------------------------------------------
*/
MeshConverter::MeshConverter()
{
	lol::SetupMeshLoader();

	Editor::F_RegisterAssetProcessor(".3ds",this);

	Editor::F_RegisterAssetProcessor(".x",this);
	Editor::F_RegisterAssetProcessor(".X",this);

	Editor::F_RegisterAssetProcessor(".obj",this);
	Editor::F_RegisterAssetProcessor(".mesh",this);

	Editor::F_RegisterAssetProcessor(".static_mesh",this);
	//Editor::F_RegisterAssetProcessor(".sdkmesh",this);
}

MeshConverter::~MeshConverter()
{
	lol::CloseMeshLoader();
}

void MeshConverter::ProcessFile(
	const ProcessFileInput& input,
	ProcessFileOutput &output
	)
{
	output.assetType = EAssetType::Asset_Static_Mesh;
	output.fileExtension = ".geo";




	// convert the mesh into intermediate form
	ATriangleMeshData::Ref	newMeshData = lol::LoadMeshFromFile( input.filePath );
	if( !newMeshData ) {
		return;
	}

	StaticTriangleMeshData* staticMeshData = SafeCast< StaticTriangleMeshData >( newMeshData );
	AssertPtr( staticMeshData );
	if( !staticMeshData ) {
		return;
	}

	{
		MemoryBlobWriter	streamWriter( output.intermediateData );

		this->SaveIntermediateData( newMeshData, streamWriter );
	}




	// compile the mesh to the final format
	{

		JsonBuildConfig	buildConfig;
		JsonBuildConfig::F_Get_Asset_Build_Config( input.filePath, buildConfig );

		bool	is_static_mesh = false;
		buildConfig.GetBool("is_static_mesh", is_static_mesh);




		IndexedMesh		tempMesh;
		{
			tempMesh.positions = staticMeshData->positions.ToPtr();
			tempMesh.texCoords = staticMeshData->texCoords.ToPtr();
			tempMesh.tangents = staticMeshData->tangents.ToPtr();
			tempMesh.normals = staticMeshData->normals.ToPtr();

			tempMesh.numVertices = staticMeshData->positions.Num();

			tempMesh.indices = staticMeshData->indices.ToPtr();
			tempMesh.indexStride = staticMeshData->indexStride;
			tempMesh.numIndices = staticMeshData->NumIndices();

			tempMesh.bounds = staticMeshData->localBounds;
		}


		// vertex format used for rendering static meshes
		typedef GPU::Vertex_P3f_TEX2f_N4Ub_T4Ub	StaticVertexType;


		RawMeshData		rawMeshData;
		rawMeshData.Build< StaticVertexType >( tempMesh );

		InPlace::Serializer		serializer;
		serializer.SaveObject( rawMeshData, output.compiledData );
	}
}

/*
-----------------------------------------------------------------------------
	ATriangleMeshData
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(ATriangleMeshData);

/*
-----------------------------------------------------------------------------
	StaticTriangleMeshData
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(StaticTriangleMeshData);

void StaticTriangleMeshData::Serialize( mxArchive & s )
{
	Super::Serialize( s );

	s && positions;
	s && texCoords;
	s && normals;
	s && tangents;

	s && indices;
	s && indexStride;

	s && localBounds;

	s && batches;
}
