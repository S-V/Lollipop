#pragma once

#include <EditorSupport/AssetPipeline.h>

#include "editor_common.h"


/*
-----------------------------------------------------------------------------
	AEntityEditor
-----------------------------------------------------------------------------
*/
class AEntityEditor : public AObjectEditor
{
	TValidPtr< World_Entities_List >	m_parent;
	TValidPtr< AEntity >	m_entity;

public:
	mxDECLARE_ABSTRACT_CLASS(AEntityEditor,AObjectEditor);

	AEntityEditor( World_Entities_List* theParent, AEntity* theEntity );

	virtual AEditable* edGetParent() override;

	virtual void edRemoveSelf() override;

	AEntity* GetEntity() { return m_entity; }

public:
	// creates an editor object for the given entity
	static AEntityEditor* Create_Editor_For_Entity(
		World_Entities_List* theParent, AEntity* theEntity );

	static AEntityEditor* Create_Entity_From_Resource(
		World_Entities_List* theParent, const SAssetInfo& assetInfo, const AssetData::Ref& pSrcAssetData );
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
