/*
=============================================================================
	File:	Model.cpp
	Desc:	Graphics model used for rendering.
	ToDo:	optimize, split into hot/cold data
			(divide into sets for culling/sorting/rendering)
			m_worldAABB should be stored in a separate array.
=============================================================================
*/
#pragma once

#include <Renderer/Core/Mesh.h>
#include <Renderer/Core/Material.h>
#include <Renderer/Scene/RenderEntity.h>

/*
-----------------------------------------------------------------------------
	rxModelBatch
-----------------------------------------------------------------------------
*/
struct rxModelBatch : public SBaseType
{
	rxMaterial::Ref		material;	//4 pointer to graphics material
	U4					indexCount;	//4 number of indices
	U4					startIndex;	//4 offset of the first index
	U4					baseVertex;	//4 index of the first vertex

public:
	mxDECLARE_CLASS( rxModelBatch, SBaseType );
	mxDECLARE_REFLECTION;
};

/*
-----------------------------------------------------------------------------
	rxModel

	is a graphics model used for rendering;
	it basically represents an instance of a renderable mesh in a scene.
	several models can share one mesh.
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct rxModel) : public SBaseType
{
	float4x4 				m_localToWorld;	//64 local-to-world transform ('world matrix')
	rxMesh::Ref				m_mesh;			//4 mesh for rendering
	TList< rxModelBatch >	m_batches;		//16 mesh subsets
	rxAABB					m_localAABB;	//24 bounds in local space (changes only when geometry changes)
	rxAABB					m_worldAABB;	//24 bounds in world space for coarse culling (should be updated properly)

public:
	mxDECLARE_CLASS( rxModel, SBaseType );
	mxDECLARE_REFLECTION;

	rxModel();

	// this function is called to submit visible batches for rendering
	void SubmitBatches( const rxEntityViewContext& context );

	void RenderShadowDepth( const rxShadowRenderContext& context ) const;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
