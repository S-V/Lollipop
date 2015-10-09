#pragma once

#include "editor_common.h"
//#include "basic_entity_editor.h"


/*
-----------------------------------------------------------------------------
	Lights_Editor
-----------------------------------------------------------------------------
*/
struct Lights_Editor : public AObjectEditor
{
	TValidPtr< Graphics_Scene_Editor >	m_parent;
	TValidPtr< rxRenderWorld >	m_scene;

public:
	mxDECLARE_CLASS(Lights_Editor,AObjectEditor);

	Lights_Editor( Graphics_Scene_Editor* theParent, rxRenderWorld* theScene );
	~Lights_Editor();

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
