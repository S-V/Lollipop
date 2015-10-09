//--------------------------------------------------------------------------------------
// File: SDKMesh.h
//
// Disclaimer:
//   The SDK Mesh format (.sdkmesh) is not a recommended file format for shipping titles.
//   It was designed to meet the specific needs of the SDK samples.  Any real-world
//   applications should avoid this file format in favor of a destination format that
//   meets the specific needs of the application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef _SDKMESH_
#define _SDKMESH_

mxSWIPED("Matt Pettineo (MJP)");

#include <vector>

//--------------------------------------------------------------------------------------
// Hard Defines for the various structures
//--------------------------------------------------------------------------------------
#define SDKMESH_FILE_VERSION 101
#define MAX_VERTEX_ELEMENTS 32
#define MAX_VERTEX_STREAMS 16
#define MAX_FRAME_NAME 100
#define MAX_MESH_NAME 100
#define MAX_SUBSET_NAME 100
#define MAX_MATERIAL_NAME 100
#define MAX_TEXTURE_NAME MAX_PATH
#define MAX_MATERIAL_PATH MAX_PATH
#define INVALID_FRAME ((UINT)-1)
#define INVALID_MESH ((UINT)-1)
#define INVALID_MATERIAL ((UINT)-1)
#define INVALID_SUBSET ((UINT)-1)
#define INVALID_ANIMATION_DATA ((UINT)-1)
#define ERROR_RESOURCE_VALUE 1
#define INVALID_SAMPLER_SLOT ((UINT)-1)

template<typename TYPE> BOOL IsErrorResource( TYPE data )
{
    if( ( TYPE )ERROR_RESOURCE_VALUE == data )
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------
// Enumerated Types.  These will have mirrors in both D3D9 and D3D10
//--------------------------------------------------------------------------------------
enum SDKMESH_PRIMITIVE_TYPE
{
    PT_TRIANGLE_LIST = 0,
    PT_TRIANGLE_STRIP,
    PT_LINE_LIST,
    PT_LINE_STRIP,
    PT_POINT_LIST,
    PT_TRIANGLE_LIST_ADJ,
    PT_TRIANGLE_STRIP_ADJ,
    PT_LINE_LIST_ADJ,
    PT_LINE_STRIP_ADJ,
};

enum SDKMESH_INDEX_TYPE
{
    IT_16BIT = 0,
    IT_32BIT,
};

enum FRAME_TRANSFORM_TYPE
{
    FTT_RELATIVE = 0,
    FTT_ABSOLUTE,		//This is not currently used but is here to support absolute transformations in the future
};

// This is needed for correct mapping of data structures from disk to memory
#pragma pack (push,8)

//--------------------------------------------------------------------------------------
// Structures.  Unions with pointers are forced to 64bit.
//--------------------------------------------------------------------------------------
struct SDKMESH_HEADER
{
    //Basic Info and sizes
    UINT Version;
    BYTE IsBigEndian;
    UINT64 HeaderSize;
    UINT64 NonBufferDataSize;
    UINT64 BufferDataSize;

    //Stats
    UINT NumVertexBuffers;
    UINT NumIndexBuffers;
    UINT NumMeshes;
    UINT NumTotalSubsets;
    UINT NumFrames;
    UINT NumMaterials;

    //Offsets to Data
    UINT64 VertexStreamHeadersOffset;
    UINT64 IndexStreamHeadersOffset;
    UINT64 MeshDataOffset;
    UINT64 SubsetDataOffset;
    UINT64 FrameDataOffset;
    UINT64 MaterialDataOffset;
};

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    UINT64 NumVertices;
    UINT64 SizeBytes;
    UINT64 StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
//        IDirect3DVertexBuffer9* pVB9;
//#ifdef D3D10_SDK_VERSION
//        ID3D10Buffer*			pVB10;
//#endif
    };
};

struct SDKMESH_INDEX_BUFFER_HEADER
{
    UINT64 NumIndices;
    UINT64 SizeBytes;
    UINT IndexType;
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
//        IDirect3DIndexBuffer9* pIB9;
//#ifdef D3D10_SDK_VERSION
//        ID3D10Buffer*			pIB10;
//#endif
    };
};


struct SDKMESH_MESH
{
    char    Name[MAX_MESH_NAME];
    BYTE NumVertexBuffers;
    UINT    VertexBuffers[MAX_VERTEX_STREAMS];
    UINT IndexBuffer;
    UINT NumSubsets;
    UINT NumFrameInfluences; //aka bones

    D3DXVECTOR3 BoundingBoxCenter;
    D3DXVECTOR3 BoundingBoxExtents;

    union
    {
        UINT64 SubsetOffset;	//Offset to list of subsets (This also forces the union to 64bits)
        UINT* pSubsets;	    //Pointer to list of subsets
    };
    union
    {
        UINT64 FrameInfluenceOffset;  //Offset to list of frame influences (This also forces the union to 64bits)
        UINT* pFrameInfluences;      //Pointer to list of frame influences
    };
};

struct SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
	//UINT	_pad;
    UINT MaterialID;
    UINT PrimitiveType;
    UINT64 IndexStart;
    UINT64 IndexCount;
    UINT64 VertexStart;
    UINT64 VertexCount;
};

struct SDKMESH_FRAME
{
    char Name[MAX_FRAME_NAME];
	//UINT	_pad;
    UINT Mesh;
    UINT ParentFrame;
    UINT ChildFrame;
    UINT SiblingFrame;
    D3DXMATRIX Matrix;
    UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame
};
#pragma pack (pop)

struct SDKMESH_MATERIAL
{
    char    Name[MAX_MATERIAL_NAME];

    // Use MaterialInstancePath
    char    MaterialInstancePath[MAX_MATERIAL_PATH];

    // Or fall back to d3d8-type materials
    char    DiffuseTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    SpecularTexture[MAX_TEXTURE_NAME];

    D3DXVECTOR4 Diffuse;
    D3DXVECTOR4 Ambient;
    D3DXVECTOR4 Specular;
    D3DXVECTOR4 Emissive;
    FLOAT Power;

    union
    {
        UINT64 Force64_1;			//Force the union to 64bits
        IDirect3DTexture9* pDiffuseTexture9;
#ifdef D3D10_SDK_VERSION
        ID3D10Texture2D*			pDiffuseTexture10;
#endif
    };
    union
    {
        UINT64 Force64_2;			//Force the union to 64bits
        IDirect3DTexture9* pNormalTexture9;
#ifdef D3D10_SDK_VERSION
        ID3D10Texture2D*			pNormalTexture10;
#endif
    };
    union
    {
        UINT64 Force64_3;			//Force the union to 64bits
        IDirect3DTexture9* pSpecularTexture9;
#ifdef D3D10_SDK_VERSION
        ID3D10Texture2D*			pSpecularTexture10;
#endif
    };

    union
    {
        UINT64 Force64_4;			//Force the union to 64bits
#ifdef D3D10_SDK_VERSION
        ID3D10ShaderResourceView*	pDiffuseRV10;
#endif
    };
    union
    {
        UINT64 Force64_5;		    //Force the union to 64bits
#ifdef D3D10_SDK_VERSION
        ID3D10ShaderResourceView*	pNormalRV10;
#endif
    };
    union
    {
        UINT64 Force64_6;			//Force the union to 64bits
#ifdef D3D10_SDK_VERSION
        ID3D10ShaderResourceView*	pSpecularRV10;
#endif
    };

};

struct SDKANIMATION_FILE_HEADER
{
    UINT Version;
    BYTE IsBigEndian;
    UINT FrameTransformType;
    UINT NumFrames;
    UINT NumAnimationKeys;
    UINT AnimationFPS;
    UINT64 AnimationDataSize;
    UINT64 AnimationDataOffset;
};

struct SDKANIMATION_DATA
{
    D3DXVECTOR3 Translation;
    D3DXVECTOR4 Orientation;
    D3DXVECTOR3 Scaling;
};

struct SDKANIMATION_FRAME_DATA
{
    char FrameName[MAX_FRAME_NAME];
    union
    {
        UINT64 DataOffset;
        SDKANIMATION_DATA* pAnimationData;
    };
};
//#pragma pack (pop)


#ifndef _CONVERTER_APP_

//--------------------------------------------------------------------------------------
// AsyncLoading callbacks
//--------------------------------------------------------------------------------------
typedef void ( CALLBACK*LPCREATETEXTUREFROMFILE9 )( IDirect3DDevice9* pDev, char* szFileName,
                                                    IDirect3DTexture9** ppTexture, void* pContext );
typedef void ( CALLBACK*LPCREATEVERTEXBUFFER9 )( IDirect3DDevice9* pDev, IDirect3DVertexBuffer9** ppBuffer,
                                                 UINT iSizeBytes, DWORD Usage, DWORD FVF, D3DPOOL Pool, void* pData,
                                                 void* pContext );
typedef void ( CALLBACK*LPCREATEINDEXBUFFER9 )( IDirect3DDevice9* pDev, IDirect3DIndexBuffer9** ppBuffer,
                                                UINT iSizeBytes, DWORD Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
                                                void* pData, void* pContext );
struct SDKMESH_CALLBACKS9
{
    LPCREATETEXTUREFROMFILE9 pCreateTextureFromFile;
    LPCREATEVERTEXBUFFER9 pCreateVertexBuffer;
    LPCREATEINDEXBUFFER9 pCreateIndexBuffer;
    void* pContext;
};
typedef void ( CALLBACK*LPCREATETEXTUREFROMFILE10 )( ID3D10Device* pDev, char* szFileName,
                                                     ID3D10ShaderResourceView** ppRV, void* pContext, bool srgb );
typedef void ( CALLBACK*LPCREATEVERTEXBUFFER10 )( ID3D10Device* pDev, ID3D10Buffer** ppBuffer,
                                                  D3D10_BUFFER_DESC BufferDesc, void* pData, void* pContext );
typedef void ( CALLBACK*LPCREATEINDEXBUFFER10 )( ID3D10Device* pDev, ID3D10Buffer** ppBuffer,
                                                 D3D10_BUFFER_DESC BufferDesc, void* pData, void* pContext );
struct SDKMESH_CALLBACKS10
{
    LPCREATETEXTUREFROMFILE10 pCreateTextureFromFile;
    LPCREATEVERTEXBUFFER10 pCreateVertexBuffer;
    LPCREATEINDEXBUFFER10 pCreateIndexBuffer;
    void* pContext;
};

//--------------------------------------------------------------------------------------
// SDKMesh class.  This class reads the sdkmesh file format for use by the samples
//--------------------------------------------------------------------------------------
class SDKMesh
{
private:
    //BYTE*                         m_pBufferData;
    HANDLE m_hFile;
    HANDLE m_hFileMappingObject;
	std::vector<BYTE*> m_MappedPointers;

protected:
    //These are the pointers to the two chunks of data loaded in from the mesh file
    BYTE* m_pStaticMeshData;
    BYTE* m_pHeapData;
    BYTE* m_pAnimationData;
    BYTE** m_ppVertices;
    BYTE** m_ppIndices;

    //General mesh info
    SDKMESH_HEADER* m_pMeshHeader;
    SDKMESH_VERTEX_BUFFER_HEADER* m_pVertexBufferArray;
    SDKMESH_INDEX_BUFFER_HEADER* m_pIndexBufferArray;
    SDKMESH_MESH* m_pMeshArray;
    SDKMESH_SUBSET* m_pSubsetArray;
    SDKMESH_FRAME* m_pFrameArray;
    SDKMESH_MATERIAL* m_pMaterialArray;

    // Adjacency information (not part of the m_pStaticMeshData, so it must be created and destroyed separately )
    SDKMESH_INDEX_BUFFER_HEADER* m_pAdjacencyIndexBufferArray;

    //Animation (TODO: Add ability to load/track multiple animation sets)
    SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
    SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;
    D3DXMATRIX* m_pBindPoseFrameMatrices;
    D3DXMATRIX* m_pTransformedFrameMatrices;

protected:

    virtual HRESULT                 CreateFromFile( LPCTSTR szFileName );
    virtual HRESULT                 CreateFromMemory( BYTE* pData,
                                                      UINT DataBytes,
                                                      bool bCopyStatic );

    //frame manipulation
    void                            TransformBindPoseFrame( UINT iFrame, D3DXMATRIX* pParentWorld );
    void                            TransformFrame( UINT iFrame, D3DXMATRIX* pParentWorld, double fTime );
    void                            TransformFrameAbsolute( UINT iFrame, double fTime );

public:
                                    SDKMesh();
    virtual                         ~SDKMesh();

    virtual HRESULT                 Create( LPCTSTR szFileName );
    virtual void                    Destroy();

    //Frame manipulation
    void                            TransformBindPose( D3DXMATRIX* pWorld );
    void                            TransformMesh( D3DXMATRIX* pWorld, double fTime );

    //Helpers (D3D10 specific)
    static D3D10_PRIMITIVE_TOPOLOGY GetPrimitiveType10( SDKMESH_PRIMITIVE_TYPE PrimType );
    DXGI_FORMAT                     GetIBFormat10( UINT iMesh );
    SDKMESH_INDEX_TYPE GetIndexType( UINT iMesh );


    //Helpers (general)
	const SDKMESH_HEADER* GetHeader() const { return m_pMeshHeader; }

    UINT                            GetNumMeshes();
    UINT                            GetNumMaterials();
    UINT                            GetNumVBs();
    UINT                            GetNumIBs();
    ID3D10Buffer* GetVB10At( UINT iVB );
    ID3D10Buffer* GetIB10At( UINT iIB );
    BYTE* GetRawVerticesAt( UINT iVB );
    BYTE* GetRawIndicesAt( UINT iIB );
    SDKMESH_MATERIAL* GetMaterial( UINT iMaterial );
    SDKMESH_MESH* GetMesh( UINT iMesh );
    UINT                            GetNumSubsets( UINT iMesh );
    SDKMESH_SUBSET* GetSubset( UINT iMesh, UINT iSubset );
    UINT                            GetVertexStride( UINT iMesh, UINT iVB );
    SDKMESH_FRAME* FindFrame( char* pszName );
    UINT64                          GetNumVertices( UINT iMesh, UINT iVB );
    UINT64                          GetNumIndices( UINT iMesh );
    D3DXVECTOR3                     GetMeshBBoxCenter( UINT iMesh );
    D3DXVECTOR3                     GetMeshBBoxExtents( UINT iMesh );
    UINT                            GetOutstandingBufferResources();

    //Animation
    UINT                            GetNumInfluences( UINT iMesh );
    const D3DXMATRIX* GetMeshInfluenceMatrix( UINT iMesh, UINT iInfluence );
    UINT                            GetAnimationKeyFromTime( double fTime );
};
#endif

#endif

