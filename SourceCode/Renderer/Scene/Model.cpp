/*
=============================================================================
	File:	Model.cpp
	Desc:	Graphics model used for rendering.
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include "Model.h"
#include <Renderer/Pipeline/Shadows.h>
#include <Renderer/Util/RenderMesh.h>

/*
-----------------------------------------------------------------------------
	rxModelBatch
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( rxModelBatch );

mxBEGIN_REFLECTION( rxModelBatch )
	mxMEMBER_FIELD( material )
	mxMEMBER_FIELD( indexCount )
	mxMEMBER_FIELD( startIndex )
	mxMEMBER_FIELD( baseVertex )
mxEND_REFLECTION

/*
-----------------------------------------------------------------------------
	rxModel
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( rxModel );

mxBEGIN_REFLECTION( rxModel )
	mxMEMBER_FIELD2( m_localToWorld, Local_To_World_Matrix )
	mxMEMBER_FIELD2( m_mesh, Graphics_Mesh )
	mxMEMBER_FIELD2( m_batches, Model_Batches_List )
	mxMEMBER_FIELD2( m_localAABB, Bounds_In_Local_Space )
	mxMEMBER_FIELD2( m_worldAABB, Bounds_In_World_Space )
mxEND_REFLECTION

rxModel::rxModel()
{
	m_localToWorld = XMMatrixIdentity();
	rxAABB_Infinity( m_localAABB );
	rxAABB_Infinity( m_worldAABB );
}

void rxModel::SubmitBatches( const rxEntityViewContext& context )
{
	const UINT numBatches = m_batches.Num();

	rxMaterialViewContext	materialContext;
	materialContext.s = context.s;
	materialContext.q = context.q;
	materialContext.entity = this;

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const rxModelBatch & batch = m_batches[ iBatch ];

		materialContext.subset = iBatch;
		materialContext.fDistance = 0.0f;mxUNDONE;

		rxMaterial* material = batch.material.ToPtr();

		material->rfSubmitBatches( materialContext );
	}
}

void rxModel::RenderShadowDepth( const rxShadowRenderContext& context ) const
{
	rxMesh& mesh = *m_mesh.ToPtr();

	F_Bind_Mesh_Positions_Only( mesh, context.pD3D );

	//@todo: check if any batches are translucent
#if 0

	const UINT numBatches = m_batches.Num();

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const rxModelBatch & batch = m_batches[ iBatch ];

		context.pD3D->DrawIndexed( batch.indexCount, batch.startIndex, batch.baseVertex );
	}

#else

	context.pD3D->DrawIndexed( mesh.m_numIndices, 0/*startIndex*/, 0/*baseVertex*/ );

#endif
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
