/*
=============================================================================
	File:	
	Desc:	
=============================================================================
*/
#include "stdafx.h"
#pragma hdrstop
#include "ShaderGen.h"
using namespace ShaderGen;

//#define INPUT_FOLDER	"e:/_/Projects/tests/test_render/"
#define OUTPUT_FOLDER	"R:/"
#define INPUT_FILE_0	"E:/_/Engine/Development/SourceCode/Renderer/GPU/source/m_globals.fx"
#define INPUT_FILE_1	"E:/_/Engine/Development/SourceCode/Renderer/GPU/source/p_deferred_fill_buffers.fx"

int mxAppMain()
{
	//FileLogUtil		fileLog;

	Options	config;

	{
		config.srcFiles.Add(OSPathName(INPUT_FILE_0));
		config.srcFiles.Add(OSPathName(INPUT_FILE_1));
	}

	config.outputFolderHLSL = OUTPUT_FOLDER;
	config.outputFolderCPP = OUTPUT_FOLDER;

	config.bDebugMode = true;


	Generator* p = Generator::Create( config );

	p->GenerateShaderLib();

	Generator::Destroy( p );

	return 0;
}

MX_APPLICATION_ENTRY_POINT
