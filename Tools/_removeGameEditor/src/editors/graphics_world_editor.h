#pragma once

#include "editor_common.h"
#include "basic_entity_editor.h"
#include "light_editor.h"
#include "sky_editor.h"

/*
-----------------------------------------------------------------------------
	Graphics_Scene_Editor
-----------------------------------------------------------------------------
*/
struct Graphics_Scene_Editor : public AObjectEditor
{
	TValidPtr< World_Editor >	m_parent;
	TValidPtr< rxRenderWorld >	m_scene;

	Lights_Editor	m_lightsEditor;
	Sky_Editor		m_skyEditor;

public:
	mxDECLARE_CLASS(Graphics_Scene_Editor,AObjectEditor);

	Graphics_Scene_Editor( World_Editor* theParent, World* theWorld );
	~Graphics_Scene_Editor();

	virtual void PostLoad() override;

	mxIMPLEMENT_ED_GROUP_2( m_lightsEditor, m_skyEditor );

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
