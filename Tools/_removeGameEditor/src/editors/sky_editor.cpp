#include "stdafx.h"

#include <Renderer/Scene/RenderWorld.h>

#include "sky_editor.h"
#include "graphics_world_editor.h"

/*
-----------------------------------------------------------------------------
	Sky_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Sky_Editor);

Sky_Editor::Sky_Editor( Graphics_Scene_Editor* theParent, rxRenderWorld* theScene )
	: m_parent( theParent )
	, m_scene( theScene )
	, m_sky( theScene->GetSky() )
{

}

Sky_Editor::~Sky_Editor()
{

}

AEditable* Sky_Editor::edGetParent()
{
	return m_parent;
}

const char* Sky_Editor::edGetName() const
{
	return "Sky";
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
