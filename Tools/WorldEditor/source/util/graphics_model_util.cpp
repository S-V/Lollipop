#include "stdafx.h"

#include "util/graphics_model_util.h"

namespace GraphicsUtil
{

void F_Create_Render_Model_From_Mesh( rxModel & rModel, const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData )
{
	rModel.m_localToWorld = XMMatrixIdentity();

	// Load mesh.

	rModel.m_mesh.SetFromGuid( assetInfo.assetGuid );

	// Copy submeshes.

	const UINT numBatches = pStaticMeshData->batches.Num();

	rModel.m_batches.SetNum( numBatches );

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const MeshPart & src = pStaticMeshData->batches[ iBatch ];
		rxModelBatch & dst = rModel.m_batches[ iBatch ];

		dst.material.SetDefaultInstance();
		dst.indexCount = src.indexCount;
		dst.startIndex = src.startIndex;
		dst.baseVertex = src.baseVertex;
	}

	rxAABB_From_AABB( rModel.m_localAABB, pStaticMeshData->localBounds );
	rModel.m_worldAABB = rModel.m_localAABB;
}


}//namespace GraphicsUtil
