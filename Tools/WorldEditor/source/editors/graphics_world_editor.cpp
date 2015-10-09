#include "stdafx.h"

#include <Engine/Worlds.h>

#include "graphics_model_editor.h"
#include "graphics_world_editor.h"
#include "world_editor.h"

/*
-----------------------------------------------------------------------------
	Graphics_Scene_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Graphics_Scene_Editor);

Graphics_Scene_Editor::Graphics_Scene_Editor( World_Editor& theParent, World& theWorld )
	: m_parent( theParent )
	, m_scene( theWorld.m_renderWorld )

	, m_models( this )
	, m_localLights( *this, m_scene )
	, m_globalLights( *this, m_scene )
	, m_skies( this, &m_scene )
{

}

Graphics_Scene_Editor::~Graphics_Scene_Editor()
{

}

void Graphics_Scene_Editor::PostLoad()
{
	Super::PostLoad();

	m_models.PostLoad();
	
	m_localLights.PostLoad();
	m_globalLights.PostLoad();

	m_skies.PostLoad();
}

AEditable* Graphics_Scene_Editor::edGetParent()
{
	return &m_parent;
}

const char* Graphics_Scene_Editor::edGetName() const
{
	return "Graphics Scene";
}

void Graphics_Scene_Editor::Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	m_models.Draw_Editor_Stuff( viewport, sceneContext );
	m_localLights.Draw_Editor_Stuff( viewport, sceneContext );
}

void Graphics_Scene_Editor::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	m_models.Draw_Hit_Proxies( viewport, sceneContext );
	m_localLights.Draw_Hit_Proxies( viewport, sceneContext );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
