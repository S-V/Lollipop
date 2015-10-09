#pragma once

#include "editor_common.h"
#include "basic_entity_editor.h"
#include "light_editor.h"
#include "sky_editor.h"
#include "graphics_model_editor.h"

/*
-----------------------------------------------------------------------------
	Graphics_Scene_Editor
-----------------------------------------------------------------------------
*/
struct Graphics_Scene_Editor : public AObjectEditor
{
	World_Editor &	m_parent;
	rxRenderWorld &	m_scene;

	Graphics_Models_List		m_models;

	Scene_Editor_Local_Lights	m_localLights;
	Scene_Editor_Global_Lights	m_globalLights;

	Sky_Editor				m_skies;

public:
	mxDECLARE_CLASS(Graphics_Scene_Editor,AObjectEditor);

	Graphics_Scene_Editor( World_Editor& theParent, World& theWorld );
	~Graphics_Scene_Editor();

	void Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );

	virtual void PostLoad() override;

	mxIMPLEMENT_ED_GROUP_4( m_models, m_localLights, m_globalLights, m_skies );

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
