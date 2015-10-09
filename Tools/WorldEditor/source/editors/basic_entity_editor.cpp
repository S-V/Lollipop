#include "stdafx.h"

#include "basic_entity_editor.h"
#include "static_model_editor.h"
#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	AEntityEditor
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(AEntityEditor);

AEntityEditor::AEntityEditor( World_Entities_List* theParent, AEntity* theEntity )
	: m_parent( theParent )
	, m_entity( theEntity )
{
}

AEditable* AEntityEditor::edGetParent()
{
	return m_parent;
}

void AEntityEditor::edRemoveSelf()
{
	m_parent->Remove( this );
}

AEntityEditor* AEntityEditor::Create_Editor_For_Entity(
	World_Entities_List* theParent, AEntity* theEntity )
{
	AssertPtr(theEntity);
	CHK_VRET_NIL_IF_NIL(theEntity);

	StaticModelEntity* pStaticModel = SafeCast<StaticModelEntity>( theEntity );
	if( pStaticModel != nil )
	{
		return new Static_Model_Editor( theParent, pStaticModel );
	}

	mxDBG_UNIMPLEMENTED;
	return nil;
}

AEntityEditor* AEntityEditor::Create_Entity_From_Resource(
	World_Entities_List* theParent, const SAssetInfo& assetInfo, const AssetData::Ref& pSrcAssetData )
{
	StaticTriangleMeshData* pStaticMeshData = SafeCast<StaticTriangleMeshData>( pSrcAssetData );
	if( pStaticMeshData != nil )
	{
		return Static_Model_Editor::Create_Static_Model( theParent, assetInfo, pStaticMeshData );
	}

	mxDBG_UNIMPLEMENTED;
	return nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
