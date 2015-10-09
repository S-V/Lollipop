#pragma once

#include "editor_common.h"
#include "basic_entity_editor.h"

/*
-----------------------------------------------------------------------------
	StaticModelEntityEditor
-----------------------------------------------------------------------------
*/
class StaticModelEntityEditor : public AEntityEditor
{
	TValidPtr< StaticModelEntity >	m_staticModelEntity;

public:
	mxDECLARE_CLASS(StaticModelEntityEditor,AEntityEditor);

	StaticModelEntityEditor( World_Entities_List* theParent, StaticModelEntity* theEntity );

	virtual const char* edGetName() const override;
};



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
