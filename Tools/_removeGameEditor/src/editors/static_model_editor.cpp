#include "stdafx.h"

#include <Engine/Entities/StaticModelEntity.h>

#include "static_model_editor.h"

/*
-----------------------------------------------------------------------------
	StaticModelEntityEditor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(StaticModelEntityEditor);

StaticModelEntityEditor::StaticModelEntityEditor( World_Entities_List* theParent, StaticModelEntity* theEntity )
	: Super( theParent, theEntity )
	, m_staticModelEntity( theEntity )
{

}

const char* StaticModelEntityEditor::edGetName() const
{
	return "Static Model";
}

MX_REMOVE_OLD_CODE
//static
//AEntityEditor* F_Create_Editor_For_Entity( World_Entities_List* theParent, AEntity* theEntity )
//{
//	Assert( !theEntity->m_editor );
//
//	StaticModelEntity* pStaticModel = SafeCast<StaticModelEntity>( theEntity );
//	if( pStaticModel != nil )
//	{
//		return new StaticModelEntityEditor( theParent, pStaticModel );
//	}
//
//	mxWarnf("Failed to create editor for entity of class '%s'\n", theEntity->rttiGetTypeName());
//
//	return nil;
//}

MX_REMOVE_OLD_CODE
//static
//bool F_Try_To_Load_Static_Model( const Editor::AssetData::Ref& assetData )
//{
//	Editor::StaticTriangleMeshData* meshData = SafeCast<Editor::StaticTriangleMeshData>( assetData );
//	if( meshData != nil )
//	{
//		StaticModelEntity* newStaticModel = new StaticModelEntity();
//		m_entities.Add_Entity( newStaticModel );
//	}
//}

//static
//void F_Load_Static_Model( const Editor::StaticTriangleMeshData& srcAssetData, StaticModelEntity* newStaticModel )
//{
//	rxAABB_From_AABB( newStaticModel->m_localAABB, srcAssetData.meshBounds );
//
//	UNDONE;
//	//newStaticModel->m_mesh.
//
//	const UINT numBatches = srcAssetData.batches.Num();
//
//	newStaticModel->m_batches.SetNum(numBatches);
//
//	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
//	{
//		//rxModelBatch& batch = batches[ iBatch ];
//
//		//FLOAT	fDist = 1.0f;
//
//		//materialContext.nSubSet = iBatch;
//		////materialContext.nMaterial = batch.material.GetResourceHandle();
//		//materialContext.fDistance = fDist;
//
//		//rxMaterial* material = batch.material.ToPtr();
//
//		//material->rfSubmitBatches( materialContext );
//	}
//}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
