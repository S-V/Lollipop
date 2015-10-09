#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <Core/Util/ConfigFile.h>

#include <EditorSupport/AssetPipeline/AssetProcessor.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>

static NiftyCounter	gEditorInitCounter;

mxEditor	gEditor;

static const char*	DEFAULT_EDITOR_CONFIG_FILE_NAME("Editor.ini");

//---------------------------------------------------------------------------

void SetupEditorSystem()
{
	if( gEditorInitCounter.IncRef() )
	{
		SetupCoreSubsystem();

		// Open config file.
		{
			TPtr<ConfigFile> pConfigFile;
			pConfigFile.ConstructInPlace();
			gEditor.config = pConfigFile;

			pConfigFile->Load( DEFAULT_EDITOR_CONFIG_FILE_NAME );
		}

		// Initialize content pipeline.

		Editor::SetupContentPipeline();
	}
}

//---------------------------------------------------------------------------

void CloseEditorSystem()
{
	if( gEditorInitCounter.DecRef() )
	{
		Editor::CloseContentPipeline();

		gEditor.config.Destruct();

		ShutdownCoreSubsystem();
	}
}

NO_EMPTY_FILE
