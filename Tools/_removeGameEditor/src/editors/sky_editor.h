#pragma once

#include "editor_common.h"
#include "basic_entity_editor.h"

/*
-----------------------------------------------------------------------------
	Sky_Editor
-----------------------------------------------------------------------------
*/
struct Sky_Editor : public AObjectEditor
{
	TValidPtr< Graphics_Scene_Editor >	m_parent;
	TValidPtr< rxRenderWorld >	m_scene;
	TValidRef< rxSkyModel >		m_sky;

public:
	mxDECLARE_CLASS(Sky_Editor,AObjectEditor);

	Sky_Editor( Graphics_Scene_Editor* theParent, rxRenderWorld* theScene );
	~Sky_Editor();

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
