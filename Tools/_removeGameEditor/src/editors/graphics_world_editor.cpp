#include "stdafx.h"

#include <Engine/Worlds.h>

#include "graphics_world_editor.h"
#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	Graphics_Scene_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Graphics_Scene_Editor);

Graphics_Scene_Editor::Graphics_Scene_Editor( World_Editor* theParent, World* theWorld )
	: m_parent( theParent )
	, m_scene( &theWorld->m_renderWorld )
	, m_lightsEditor( this, m_scene )
	, m_skyEditor( this, m_scene )
{

}

Graphics_Scene_Editor::~Graphics_Scene_Editor()
{

}

void Graphics_Scene_Editor::PostLoad()
{
	Super::PostLoad();
}

AEditable* Graphics_Scene_Editor::edGetParent()
{
	return m_parent;
}

const char* Graphics_Scene_Editor::edGetName() const
{
	return "Graphics Scene";
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
