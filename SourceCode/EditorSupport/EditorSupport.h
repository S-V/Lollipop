/*
=============================================================================
	Editor System
=============================================================================
*/

#pragma once


//#include <Base/Templates/Delegate/Delegate.h>

#include <Core/Core.h>



class DevAssetManager;

void SetupEditorSystem();
void CloseEditorSystem();

struct InitEditorUtil
{
	InitEditorUtil()
	{
		SetupEditorSystem();
	}
	~InitEditorUtil()
	{
		CloseEditorSystem();
	}
};


struct mxEditor
{
	TPtr<INIConfigFile>		config;
};

extern mxEditor	gEditor;


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
