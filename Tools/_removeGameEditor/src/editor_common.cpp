// common editor stuff
#include "stdafx.h"

#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include "editor_common.h"
#include "app.h"

/*
-----------------------------------------------------------------------------
	EdGlobalSettings
-----------------------------------------------------------------------------
*/
EdGlobalSettings::EdGlobalSettings()
{
	pathToProjects = (QCoreApplication::applicationDirPath() + ED_DEFAULT_PROJECT_DIR).toAscii().data();
	FS_FixPathSlashes( pathToProjects );

	pathToAssetDb = pathToProjects;

	pathToSrcAssets = "Assets/";
	pathToIntAssets = "Assets_Int/";
	pathToBinAssets = "Assets_Bin/";
}

void EdGlobalSettings::Serialize( ATextSerializer & s )
{
	s.Serialize_String("Path_To_Qt_Style_Sheet", pathToQtStyleSheet);
	s.Serialize_String("PathToProjects", pathToProjects);
	s.Serialize_String("PathToAssetDb", pathToAssetDb);

	s.Serialize_String("PathToSourceAssets", pathToSrcAssets);
	s.Serialize_String("PathToIntermediateAssets", pathToIntAssets);
	s.Serialize_String("PathToCompiledAssets", pathToBinAssets);
}

/*
-----------------------------------------------------------------------------
	EdAppChild
-----------------------------------------------------------------------------
*/
EdAppChild::EdAppChild()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_SerializeAppData, EdAppChild, SerializeAddData );
}
EdAppChild::~EdAppChild()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_SerializeAppData );
}

/*
-----------------------------------------------------------------------------
	EdWidgetChild
-----------------------------------------------------------------------------
*/
EdWidgetChild::EdWidgetChild()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_SerializeLayout, EdWidgetChild, SerializeWidgetLayout );
}
EdWidgetChild::~EdWidgetChild()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_SerializeLayout );
}

/*
-----------------------------------------------------------------------------
	EdProjectChild
-----------------------------------------------------------------------------
*/
EdProjectChild::EdProjectChild()
{
	MX_CONNECT_THIS( EdSystem::Get().Event_ProjectLoaded, EdProjectChild, OnProjectLoaded );
	MX_CONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded, EdProjectChild, OnProjectUnloaded );
	MX_CONNECT_THIS( EdSystem::Get().Event_SerializeProjectData, EdProjectChild, SerializeProjectData );
}

EdProjectChild::~EdProjectChild()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_ProjectLoaded );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_SerializeProjectData );
}

/*
-----------------------------------------------------------------------------
	EdSystemChild
-----------------------------------------------------------------------------
*/
EdSystemChild::EdSystemChild()
{
	//MX_CONNECT_THIS( EdApp::Get().OnSerializeSubSystemData, EdSystemChild, SerializeMyOwnData );

	MX_CONNECT_THIS( EdSystem::Get().Event_SelectionChanged, EdSystemChild, OnSelectionChanged );
	MX_CONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed, EdSystemChild, OnObjectDestroyed );
}

EdSystemChild::~EdSystemChild()
{
	MX_DISCONNECT_THIS( EdSystem::Get().Event_SelectionChanged );
	MX_DISCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed );

	//MX_DISCONNECT_THIS( EdApp::Get().OnSerializeSubSystemData );
}


namespace EditorApp
{
	/*
		returns the path to the selected folder
	*/

	QString SelectDirectoryOnly(
		QString &outSelectedPath,
		QWidget* parentWidget,
		const QString& dialogTitle
		)
	{
		QFileDialog	dialog( parentWidget, dialogTitle );
		{
			dialog.setAcceptMode(QFileDialog::AcceptOpen);

			dialog.setFileMode(QFileDialog::Directory);
			dialog.setOption(QFileDialog::ShowDirsOnly);
		}

		// initial value is be used for 'warm starting'
		dialog.setDirectory( outSelectedPath );


		if( dialog.exec() )
		{
			const QStringList selectedFiles( dialog.selectedFiles() );
			Assert( selectedFiles.count() == 1 );

			if( selectedFiles.count() > 0 )
			{
				outSelectedPath = selectedFiles.first();
			}
			//outSelectedPath = dialog.directory().path();
		}
		else
		{
			outSelectedPath.clear();
		}

		return outSelectedPath;
	}



	QString Get_AssetPath_SaveFileAs( const char* title, const char* fileName, const char* filter )
	{
		QFileDialog	saveFileDialog( GetMainFrame(), title );
		{
			saveFileDialog.setFileMode( QFileDialog::AnyFile );
			saveFileDialog.setAcceptMode( QFileDialog::AcceptSave );
			saveFileDialog.setNameFilter( filter );
			saveFileDialog.selectFile( fileName );
		}
UNDONE;
//		const String& pathToSrcAssets = gEditor.assets->GetPathToSrcAssets();

		QString		saveAsFilePath;

//L_TryAgain:
//		if( saveFileDialog.exec() )
//		{
//			const QStringList selectedFiles( saveFileDialog.selectedFiles() );
//			Assert( selectedFiles.count() == 1 );
//
//			if( selectedFiles.count() > 0 )
//			{
//				saveAsFilePath = selectedFiles.first();
//
//				if( !saveAsFilePath.startsWith( pathToSrcAssets.ToChars() ) )
//				{
//					ShowQMessageBox("Invalid file path - should be a subfolder of source assets directory!");
//					goto L_TryAgain;
//				}
//			}
//
//			Assert( !saveAsFilePath.isEmpty() );
//		}

		return saveAsFilePath;
	}

}//EditorUtil

