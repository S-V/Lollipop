/*
=============================================================================
	File:	RenderMesh.h
	Desc:	
	ToDo:	move this code into back-end
=============================================================================
*/

#pragma once

#include <Renderer/Core/Mesh.h>

static inline
void F_Bind_Mesh( const rxMesh& rMesh, ID3D11DeviceContext* pDeviceContext )
{
	rMesh.m_vertexData.Bind( pDeviceContext );
	rMesh.m_indexData.Bind( pDeviceContext );
	rxSetTopology( pDeviceContext, rMesh.m_topology );
}

static inline
void F_Bind_Vertex_Positions_Only( const GrVertexData& rVB, ID3D11DeviceContext* pDeviceContext )
{
	//NOTE: Don't change input layout, Vertex_P3f should be set prior to this call.
	//pDeviceContext->IASetInputLayout( rVB.m_inputLayout.Ptr );

	pDeviceContext->IASetVertexBuffers(
		VERTEX_POSITIONS_STREAM_INDEX,
		1,
		c_cast(ID3D11Buffer**) rVB.m_streams,
		rVB.m_streamStrides,
		rVB.m_streamOffsets
	);
}

static inline
void F_Bind_Mesh_Positions_Only( const rxMesh& rMesh, ID3D11DeviceContext* pDeviceContext )
{
	F_Bind_Vertex_Positions_Only( rMesh.m_vertexData, pDeviceContext );
	rMesh.m_indexData.Bind( pDeviceContext );
	rxSetTopology( pDeviceContext, rMesh.m_topology );
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
