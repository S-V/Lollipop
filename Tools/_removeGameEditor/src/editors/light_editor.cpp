#include "stdafx.h"

#include "light_editor.h"
#include "graphics_world_editor.h"

/*
-----------------------------------------------------------------------------
	Lights_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Lights_Editor);

Lights_Editor::Lights_Editor( Graphics_Scene_Editor* theParent, rxRenderWorld* theScene )
	: m_parent( theParent )
	, m_scene( theScene )
{

}

Lights_Editor::~Lights_Editor()
{

}

AEditable* Lights_Editor::edGetParent()
{
	return m_parent;
}

const char* Lights_Editor::edGetName() const
{
	return "Lights";
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
