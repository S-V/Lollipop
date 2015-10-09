/*
=============================================================================
	File:	Geometry.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Renderer/Common.h>
#include "Geometry.h"

/*================================
		MeshData
================================*/

bool RawMeshData::DbgCheckValid() const
{
	CHK_VRET_FALSE_IF_NOT(vertexData.streams.IsEmpty() == false);
	CHK_VRET_FALSE_IF_NOT(indexData.NumIndices() > 0);
	CHK_VRET_FALSE_IF_NOT(topology != EPrimitiveType::PT_Unknown);
	return true;
}

void RawMeshData::Clear()
{
	vertexData.streams.Empty();
	indexData.Clear();
	topology = EPrimitiveType::PT_Unknown;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
