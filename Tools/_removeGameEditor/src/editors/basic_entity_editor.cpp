#include "stdafx.h"

#include "basic_entity_editor.h"
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
	m_parent->Add( this );
}

AEditable* AEntityEditor::edGetParent()
{
	return m_parent;
}

void AEntityEditor::edRemoveSelf()
{
	m_parent->Remove( this );
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
