#include <EditorSupport_PCH.h>
#pragma hdrstop

#include "CreateProperties.h"

namespace Properties
{
	void For_rxAABB( rxAABB & rAABB, const char* name, EdPropertyList *properties )
	{
		EdPropertyList* pAabbProperties = new EdPropertyList( name, properties );
		pAabbProperties->SetPropertyFlags(PF_ReadOnly);
		{
			(new EdProperty_Vector3D( pAabbProperties, "Center", as_vec3(rAABB.Center), PF_ReadOnly ));
			(new EdProperty_Vector3D( pAabbProperties, "Extents", as_vec3(rAABB.Extents), PF_ReadOnly ));
		}
	}

	void For_rxModel( rxModel & rModel, const char* name, EdPropertyList *properties )
	{
		EdPropertyList* pModelProperties = new EdPropertyList( name, properties );
		pModelProperties->SetPropertyFlags(PF_ReadOnly);
		{
			(new EdProperty_AssetReference( rModel.m_mesh, "Mesh", pModelProperties ))
				->SetPropertyFlags(PF_ReadOnly);

			Properties::For_rxAABB( rModel.m_worldAABB, "WorldAABB", pModelProperties );
			Properties::For_rxAABB( rModel.m_localAABB, "LocalAABB", pModelProperties );
			(new EdProperty_UInt32( pModelProperties, "VertexCount", rModel.m_mesh.ToPtr()->m_numVertices, PF_ReadOnly ));
			(new EdProperty_UInt32( pModelProperties, "IndexCount", rModel.m_mesh.ToPtr()->m_numIndices, PF_ReadOnly ));
		}
	}

	void For_rxModel_Batch( rxModelBatch & modelBatch, const char* name, EdPropertyList *properties )
	{
		EdPropertyList* pBatchProperties = new EdPropertyList( name, properties );
		pBatchProperties->SetPropertyFlags(PF_ReadOnly);
		{
			(new EdProperty_AssetReference( modelBatch.material, "Material", pBatchProperties ));
			(new EdProperty_UInt32( pBatchProperties, "IndexCount", modelBatch.indexCount, PF_ReadOnly ));
			(new EdProperty_UInt32( pBatchProperties, "StartIndex", modelBatch.startIndex, PF_ReadOnly ));
			(new EdProperty_UInt32( pBatchProperties, "BaseVertex", modelBatch.baseVertex, PF_ReadOnly ));
		}
	}

}//namespace Properties
