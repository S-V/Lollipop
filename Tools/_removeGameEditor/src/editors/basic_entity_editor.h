#pragma once

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
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
