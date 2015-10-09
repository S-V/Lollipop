//--------------------------------------------------------------------------------------
// File: SDKMesh.cpp
//
// The SDK Mesh format (.sdkmesh) is not a recommended file format for games.
// It was designed to meet the specific needs of the SDK samples.  Any real-world
// applications should avoid this file format in favor of a destination format that
// meets the specific needs of the application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <EditorSupport_PCH.h>
#pragma hdrstop

mxSWIPED("Matt Pettineo (MJP)");

#include <Base/Util/DirectX_Helpers.h>

#include "SDKMesh.h"



//--------------------------------------------------------------------------------------
HRESULT SDKMesh::CreateFromFile( LPCTSTR szFileName )
{
    HRESULT hr = S_OK;

    // Open the file
    m_hFile = CreateFile( szFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                          NULL );
    if( INVALID_HANDLE_VALUE == m_hFile )
        return E_FAIL;

    // Get the file size
    LARGE_INTEGER FileSize;
    GetFileSizeEx( m_hFile, &FileSize );
    UINT cBytes = FileSize.LowPart;

    // Allocate memory
    m_pStaticMeshData = new BYTE[ cBytes ];
    if( !m_pStaticMeshData )
    {
        CloseHandle( m_hFile );
        return E_OUTOFMEMORY;
    }

    // Read in the file
    DWORD dwBytesRead;
    if( !ReadFile( m_hFile, m_pStaticMeshData, cBytes, &dwBytesRead, NULL ) )
        hr = E_FAIL;

    CloseHandle( m_hFile );

    if( SUCCEEDED( hr ) )
    {
        hr = CreateFromMemory( m_pStaticMeshData,
                               cBytes,
                               false );
        if( FAILED( hr ) )
            delete []m_pStaticMeshData;
    }

    return hr;
}

HRESULT SDKMesh::CreateFromMemory( BYTE* pData,
								  UINT DataBytes,
								  bool bCopyStatic )
{
	HRESULT hr = E_FAIL;
    D3DXVECTOR3 lower; 
    D3DXVECTOR3 upper; 

    if( bCopyStatic )
    {
        SDKMESH_HEADER* pHeader = ( SDKMESH_HEADER* )pData;

        SIZE_T StaticSize = ( SIZE_T )( pHeader->HeaderSize + pHeader->NonBufferDataSize );
        m_pHeapData = new BYTE[ StaticSize ];
        if( !m_pHeapData )
            return hr;

        m_pStaticMeshData = m_pHeapData;

        CopyMemory( m_pStaticMeshData, pData, StaticSize );
    }
    else
    {
        m_pHeapData = pData;
        m_pStaticMeshData = pData;
    }

    // Pointer fixup
    m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
    m_pVertexBufferArray = ( SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
    m_pIndexBufferArray = ( SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
    m_pMeshArray = ( SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
    m_pSubsetArray = ( SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
    m_pFrameArray = ( SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
    m_pMaterialArray = ( SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );

    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

    // error condition
    if( m_pMeshHeader->Version != SDKMESH_FILE_VERSION )
    {
        hr = E_NOINTERFACE;
        goto Error;
    }

    // Setup buffer data pointer
    BYTE* pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Create Adjacency Indices
    //if( pDev10 && bCreateAdjacencyIndices )
    //    CreateAdjacencyIndices( pDev10, 0.001f, pBufferData - BufferDataStart );

    // Create VBs
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        BYTE* pVertices = NULL;
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

        //if( pDev10 )
        //    CreateVertexBuffer( pDev10, &m_pVertexBufferArray[i], pVertices, pLoaderCallbacks10 );
        //else if( pDev9 )
        //    CreateVertexBuffer( pDev9, &m_pVertexBufferArray[i], pVertices, pLoaderCallbacks9 );

        m_ppVertices[i] = pVertices;
    }

    // Create IBs
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );

        //if( pDev10 )
        //    CreateIndexBuffer( pDev10, &m_pIndexBufferArray[i], pIndices, pLoaderCallbacks10 );
        //else if( pDev9 )
        //    CreateIndexBuffer( pDev9, &m_pIndexBufferArray[i], pIndices, pLoaderCallbacks9 );

        m_ppIndices[i] = pIndices;
    }

    // Load Materials
    //if( pDev10 )
    //    LoadMaterials( pDev10, m_pMaterialArray, m_pMeshHeader->NumMaterials, pLoaderCallbacks10 );
    //else if( pDev9 )
    //    LoadMaterials( pDev9, m_pMaterialArray, m_pMeshHeader->NumMaterials, pLoaderCallbacks9 );

    // Create a place to store our bind pose frame matrices
    m_pBindPoseFrameMatrices = new D3DXMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pBindPoseFrameMatrices )
        goto Error;

    // Create a place to store our transformed frame matrices
    m_pTransformedFrameMatrices = new D3DXMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pTransformedFrameMatrices )
        goto Error;

    hr = S_OK;

Error:        
    return hr;
}

//--------------------------------------------------------------------------------------
// transform bind pose frame using a recursive traversal
//--------------------------------------------------------------------------------------
void SDKMesh::TransformBindPoseFrame( UINT iFrame, D3DXMATRIX* pParentWorld )
{
    if( !m_pBindPoseFrameMatrices )
        return;

    // Transform ourselves
    D3DXMATRIX LocalWorld;
    D3DXMatrixMultiply( &LocalWorld, &m_pFrameArray[iFrame].Matrix, pParentWorld );
    m_pBindPoseFrameMatrices[iFrame] = LocalWorld;

    // Transform our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        TransformBindPoseFrame( m_pFrameArray[iFrame].SiblingFrame, pParentWorld );

    // Transform our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        TransformBindPoseFrame( m_pFrameArray[iFrame].ChildFrame, &LocalWorld );
}

//--------------------------------------------------------------------------------------
// transform frame using a recursive traversal
//--------------------------------------------------------------------------------------
void SDKMesh::TransformFrame( UINT iFrame, D3DXMATRIX* pParentWorld, double fTime )
{
    // Get the tick data
    D3DXMATRIX LocalTransform;
    UINT iTick = GetAnimationKeyFromTime( fTime );

    if( INVALID_ANIMATION_DATA != m_pFrameArray[iFrame].AnimationDataIndex )
    {
        SDKANIMATION_FRAME_DATA* pFrameData = &m_pAnimationFrameData[ m_pFrameArray[iFrame].AnimationDataIndex ];
        SDKANIMATION_DATA* pData = &pFrameData->pAnimationData[ iTick ];

        // turn it into a matrix (Ignore scaling for now)
        D3DXVECTOR3 parentPos = pData->Translation;
        D3DXMATRIX mTranslate;
        D3DXMatrixTranslation( &mTranslate, parentPos.x, parentPos.y, parentPos.z );

        D3DXQUATERNION quat;
        D3DXMATRIX mQuat;
        quat.w = pData->Orientation.w;
        quat.x = pData->Orientation.x;
        quat.y = pData->Orientation.y;
        quat.z = pData->Orientation.z;
        if( quat.w == 0 && quat.x == 0 && quat.y == 0 && quat.z == 0 )
            D3DXQuaternionIdentity( &quat );
        D3DXQuaternionNormalize( &quat, &quat );
        D3DXMatrixRotationQuaternion( &mQuat, &quat );
        LocalTransform = ( mQuat * mTranslate );
    }
    else
    {
        LocalTransform = m_pFrameArray[iFrame].Matrix;
    }

    // Transform ourselves
    D3DXMATRIX LocalWorld;
    D3DXMatrixMultiply( &LocalWorld, &LocalTransform, pParentWorld );
    m_pTransformedFrameMatrices[iFrame] = LocalWorld;

    // Transform our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        TransformFrame( m_pFrameArray[iFrame].SiblingFrame, pParentWorld, fTime );

    // Transform our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        TransformFrame( m_pFrameArray[iFrame].ChildFrame, &LocalWorld, fTime );
}

//--------------------------------------------------------------------------------------
// transform frame assuming that it is an absolute transformation
//--------------------------------------------------------------------------------------
void SDKMesh::TransformFrameAbsolute( UINT iFrame, double fTime )
{
    D3DXMATRIX mTrans1;
    D3DXMATRIX mTrans2;
    D3DXMATRIX mRot1;
    D3DXMATRIX mRot2;
    D3DXQUATERNION quat1;
    D3DXQUATERNION quat2;
    D3DXMATRIX mTo;
    D3DXMATRIX mInvTo;
    D3DXMATRIX mFrom;

    UINT iTick = GetAnimationKeyFromTime( fTime );

    if( INVALID_ANIMATION_DATA != m_pFrameArray[iFrame].AnimationDataIndex )
    {
        SDKANIMATION_FRAME_DATA* pFrameData = &m_pAnimationFrameData[ m_pFrameArray[iFrame].AnimationDataIndex ];
        SDKANIMATION_DATA* pData = &pFrameData->pAnimationData[ iTick ];
        SDKANIMATION_DATA* pDataOrig = &pFrameData->pAnimationData[ 0 ];

        D3DXMatrixTranslation( &mTrans1, -pDataOrig->Translation.x,
                               -pDataOrig->Translation.y,
                               -pDataOrig->Translation.z );
        D3DXMatrixTranslation( &mTrans2, pData->Translation.x,
                               pData->Translation.y,
                               pData->Translation.z );

        quat1.x = pDataOrig->Orientation.x;
        quat1.y = pDataOrig->Orientation.y;
        quat1.z = pDataOrig->Orientation.z;
        quat1.w = pDataOrig->Orientation.w;
        D3DXQuaternionInverse( &quat1, &quat1 );
        D3DXMatrixRotationQuaternion( &mRot1, &quat1 );
        mInvTo = mTrans1 * mRot1;

        quat2.x = pData->Orientation.x;
        quat2.y = pData->Orientation.y;
        quat2.z = pData->Orientation.z;
        quat2.w = pData->Orientation.w;
        D3DXMatrixRotationQuaternion( &mRot2, &quat2 );
        mFrom = mRot2 * mTrans2;

        D3DXMATRIX mOutput = mInvTo * mFrom;
        m_pTransformedFrameMatrices[iFrame] = mOutput;
    }
}

//--------------------------------------------------------------------------------------
#define MAX_D3D10_VERTEX_STREAMS D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT

//--------------------------------------------------------------------------------------
SDKMesh::SDKMesh() :           m_hFile( 0 ),
                               m_hFileMappingObject( 0 ),
                               m_pMeshHeader( NULL ),
                               m_pStaticMeshData( NULL ),
                               m_pHeapData( NULL ),
                               m_pAdjacencyIndexBufferArray( NULL ),
                               m_pAnimationData( NULL ),
                               m_ppVertices( NULL ),
                               m_ppIndices( NULL ),
                               m_pBindPoseFrameMatrices( NULL ),
                               m_pTransformedFrameMatrices( NULL )
{
}


//--------------------------------------------------------------------------------------
SDKMesh::~SDKMesh()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
HRESULT SDKMesh::Create( LPCTSTR szFileName )
{
    return CreateFromFile( szFileName );
}

//--------------------------------------------------------------------------------------
void SDKMesh::Destroy()
{
    SAFE_DELETE_ARRAY( m_pAdjacencyIndexBufferArray );

    SAFE_DELETE_ARRAY( m_pHeapData );
    m_pStaticMeshData = NULL;
    SAFE_DELETE_ARRAY( m_pAnimationData );
    SAFE_DELETE_ARRAY( m_pBindPoseFrameMatrices );
    SAFE_DELETE_ARRAY( m_pTransformedFrameMatrices );

    SAFE_DELETE_ARRAY( m_ppVertices );
    SAFE_DELETE_ARRAY( m_ppIndices );

    m_pMeshHeader = NULL;
    m_pVertexBufferArray = NULL;
    m_pIndexBufferArray = NULL;
    m_pMeshArray = NULL;
    m_pSubsetArray = NULL;
    m_pFrameArray = NULL;
    m_pMaterialArray = NULL;

    m_pAnimationHeader = NULL;
    m_pAnimationFrameData = NULL;

}

//--------------------------------------------------------------------------------------
// transform the bind pose
//--------------------------------------------------------------------------------------
void SDKMesh::TransformBindPose( D3DXMATRIX* pWorld )
{
    TransformBindPoseFrame( 0, pWorld );
}

//--------------------------------------------------------------------------------------
// transform the mesh frames according to the animation for time fTime
//--------------------------------------------------------------------------------------
void SDKMesh::TransformMesh( D3DXMATRIX* pWorld, double fTime )
{
    if( !m_pAnimationHeader )
        return;

    if( FTT_RELATIVE == m_pAnimationHeader->FrameTransformType )
    {
        TransformFrame( 0, pWorld, fTime );

        // For each frame, move the transform to the bind pose, then
        // move it to the final position
        D3DXMATRIX mInvBindPose;
        D3DXMATRIX mFinal;
        for( UINT i = 0; i < m_pMeshHeader->NumFrames; i++ )
        {
            D3DXMatrixInverse( &mInvBindPose, NULL, &m_pBindPoseFrameMatrices[i] );
            mFinal = mInvBindPose * m_pTransformedFrameMatrices[i];
            m_pTransformedFrameMatrices[i] = mFinal;
        }
    }
    else if( FTT_ABSOLUTE == m_pAnimationHeader->FrameTransformType )
    {
        for( UINT i = 0; i < m_pAnimationHeader->NumFrames; i++ )
            TransformFrameAbsolute( i, fTime );
    }
}

//--------------------------------------------------------------------------------------
D3D10_PRIMITIVE_TOPOLOGY SDKMesh::GetPrimitiveType10( SDKMESH_PRIMITIVE_TYPE PrimType )
{
    D3D10_PRIMITIVE_TOPOLOGY retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch( PrimType )
    {
        case PT_TRIANGLE_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case PT_TRIANGLE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        case PT_LINE_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case PT_LINE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        case PT_POINT_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case PT_TRIANGLE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
            break;
        case PT_TRIANGLE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            break;
        case PT_LINE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            break;
        case PT_LINE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            break;
    };

    return retType;
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT SDKMesh::GetIBFormat10( UINT iMesh )
{
    switch( m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType )
    {
        case IT_16BIT:
            return DXGI_FORMAT_R16_UINT;
        case IT_32BIT:
            return DXGI_FORMAT_R32_UINT;
    };
    return DXGI_FORMAT_R16_UINT;
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumMeshes()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumMeshes;
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumMaterials()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumMaterials;
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumVBs()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumVertexBuffers;
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumIBs()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumIndexBuffers;
}

//--------------------------------------------------------------------------------------
BYTE* SDKMesh::GetRawVerticesAt( UINT iVB )
{
    return m_ppVertices[iVB];
}

//--------------------------------------------------------------------------------------
BYTE* SDKMesh::GetRawIndicesAt( UINT iIB )
{
    return m_ppIndices[iIB];
}

//--------------------------------------------------------------------------------------
SDKMESH_MATERIAL* SDKMesh::GetMaterial( UINT iMaterial )
{
    return &m_pMaterialArray[ iMaterial ];
}

//--------------------------------------------------------------------------------------
SDKMESH_MESH* SDKMesh::GetMesh( UINT iMesh )
{
    return &m_pMeshArray[ iMesh ];
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumSubsets( UINT iMesh )
{
    return m_pMeshArray[ iMesh ].NumSubsets;
}

//--------------------------------------------------------------------------------------
SDKMESH_SUBSET* SDKMesh::GetSubset( UINT iMesh, UINT iSubset )
{
    return &m_pSubsetArray[ m_pMeshArray[ iMesh ].pSubsets[iSubset] ];
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetVertexStride( UINT iMesh, UINT iVB )
{
    return ( UINT )m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].StrideBytes;
}

//--------------------------------------------------------------------------------------
SDKMESH_FRAME* SDKMesh::FindFrame( char* pszName )
{
    for( UINT i = 0; i < m_pMeshHeader->NumFrames; i++ )
    {
        if( _stricmp( m_pFrameArray[i].Name, pszName ) == 0 )
        {
            return &m_pFrameArray[i];
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------------------
UINT64 SDKMesh::GetNumVertices( UINT iMesh, UINT iVB )
{
    return m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices;
}

//--------------------------------------------------------------------------------------
UINT64 SDKMesh::GetNumIndices( UINT iMesh )
{
    return m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices;
}

//--------------------------------------------------------------------------------------
D3DXVECTOR3 SDKMesh::GetMeshBBoxCenter( UINT iMesh )
{
    return m_pMeshArray[iMesh].BoundingBoxCenter;
}

//--------------------------------------------------------------------------------------
D3DXVECTOR3 SDKMesh::GetMeshBBoxExtents( UINT iMesh )
{
    return m_pMeshArray[iMesh].BoundingBoxExtents;
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetOutstandingBufferResources()
{
    UINT outstandingResources = 0;
    if( !m_pMeshHeader )
        return 1;

    return outstandingResources;
}
//--------------------------------------------------------------------------------------
UINT SDKMesh::GetNumInfluences( UINT iMesh )
{
    return m_pMeshArray[iMesh].NumFrameInfluences;
}
//--------------------------------------------------------------------------------------
const D3DXMATRIX* SDKMesh::GetMeshInfluenceMatrix( UINT iMesh, UINT iInfluence )
{
    UINT iFrame = m_pMeshArray[iMesh].pFrameInfluences[ iInfluence ];
    return &m_pTransformedFrameMatrices[iFrame];
}

//--------------------------------------------------------------------------------------
UINT SDKMesh::GetAnimationKeyFromTime( double fTime )
{
    UINT iTick = ( UINT )( m_pAnimationHeader->AnimationFPS * fTime );

    iTick = iTick % ( m_pAnimationHeader->NumAnimationKeys - 1 );
    iTick ++;

    return iTick;
}

SDKMESH_INDEX_TYPE SDKMesh::GetIndexType( UINT iMesh ) 
{
    return ( SDKMESH_INDEX_TYPE ) m_pIndexBufferArray[m_pMeshArray[ iMesh ].IndexBuffer].IndexType;
}
