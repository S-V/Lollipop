// common editor stuff
#include "stdafx.h"

#include <Renderer/Core/Material.h>

#include <EditorSupport/Serialization/TextSerializer.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include "editor_common.h"
#include "editor_app.h"
#include "editors/material_editor.h"

/*
-----------------------------------------------------------------------------
	EdGlobalSettings
-----------------------------------------------------------------------------
*/
EdGlobalSettings::EdGlobalSettings()
{
	pathToProjects = (QCoreApplication::applicationDirPath() + ED_DEFAULT_PROJECT_DIR).toAscii().data();
	F_NormalizePath( pathToProjects );

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
	mxCONNECT_THIS( EdSystem::Get().Event_SerializeAppData, EdAppChild, SerializeAddData );
}
EdAppChild::~EdAppChild()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_SerializeAppData );
}

/*
-----------------------------------------------------------------------------
	EdWidgetChild
-----------------------------------------------------------------------------
*/
EdWidgetChild::EdWidgetChild()
{
	mxCONNECT_THIS( EdSystem::Get().Event_SerializeLayout, EdWidgetChild, SerializeWidgetLayout );
}
EdWidgetChild::~EdWidgetChild()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_SerializeLayout );
}

/*
-----------------------------------------------------------------------------
	EdProjectChild
-----------------------------------------------------------------------------
*/
EdProjectChild::EdProjectChild()
{
	mxCONNECT_THIS( EdSystem::Get().Event_ProjectLoaded, EdProjectChild, OnProjectLoaded );
	mxCONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded, EdProjectChild, OnProjectUnloaded );
	mxCONNECT_THIS( EdSystem::Get().Event_SerializeProjectData, EdProjectChild, SerializeProjectData );
}

EdProjectChild::~EdProjectChild()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_ProjectLoaded );
	mxDISCONNECT_THIS( EdSystem::Get().Event_ProjectUnloaded );
	mxDISCONNECT_THIS( EdSystem::Get().Event_SerializeProjectData );
}

/*
-----------------------------------------------------------------------------
	EdSystemChild
-----------------------------------------------------------------------------
*/
EdSystemChild::EdSystemChild()
{
	//mxCONNECT_THIS( EdApp::Get().OnSerializeSubSystemData, EdSystemChild, SerializeMyOwnData );

	mxCONNECT_THIS( EdSystem::Get().Event_SelectionChanged, EdSystemChild, OnSelectionChanged );
	mxCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed, EdSystemChild, OnObjectDestroyed );
}

EdSystemChild::~EdSystemChild()
{
	mxDISCONNECT_THIS( EdSystem::Get().Event_SelectionChanged );
	mxDISCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed );

	//mxDISCONNECT_THIS( EdApp::Get().OnSerializeSubSystemData );
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





/*
-----------------------------------------------------------------------------
	APlaceable
-----------------------------------------------------------------------------
*/
APlaceable::APlaceable()
{

}

APlaceable::~APlaceable()
{
	EdSystem::Get().Event_PlaceableBeingDestroyed( this );
}

void APlaceable::SetOrigin( const Vec3D& newPos )
{
	//if( newPos.IsInBounds() ) {
	//	mxDBG_UNREACHABLE_X("Invalid position!\n");
	//	return;
	//}
	this->SetOrigin_Impl( newPos );
}

void APlaceable::SetOrientation( const Quat& newRot )
{
	if( !newRot.IsNormalized() ) {
		mxPutf("Invalid orientation, quaternion must be normalized!\n");
		return;
	}
	this->SetOrientation_Impl( newRot );
}

void APlaceable::SetScale( const FLOAT newScale )
{
	if( newScale <= 1e-4f ) {
		mxPutf("Invalid scale value: '%f', must be positive!\n",newScale);
		return;
	}
	this->SetScale_Impl( newScale );
}

Matrix4 APlaceable::GetWorldTransform() const
{
	return Matrix4::mat4_identity;
}

const Vec3D& APlaceable::GetOrigin() const
{
	return this->GetWorldTransform().GetTranslation();
}

const Quat& APlaceable::GetOrientation() const
{
	return this->GetWorldTransform().ToMat3().ToQuat();
}

const FLOAT APlaceable::GetScale() const
{
	// Only uniform scaling is not supported.
	return this->GetWorldTransform()[0][0];
}

void APlaceable::GetWorldAABB( AABB & bbox ) const
{
	bbox.SetInfinity();
}

//void APlaceable::GetLocalAABB( AABB & bbox ) const
//{
//	bbox.SetInfinity();
//}

Sphere APlaceable::GetBoundingSphere() const
{
	AABB	bounds;
	this->GetWorldAABB( bounds );
	return bounds.ToSphere();
}

void APlaceable::SetOrigin_Impl( const Vec3D& newPos )
{
	mxUNUSED(newPos);
	mxDBG_UNREACHABLE;
}

void APlaceable::SetOrientation_Impl( const Quat& newRot )
{
	mxUNUSED(newRot);
	mxDBG_UNREACHABLE;
}

void APlaceable::SetScale_Impl( const FLOAT newScale )
{
	mxUNUSED(newScale);
	mxDBG_UNREACHABLE;
}

void APlaceable::edCreateCommonProperties( EdPropertyList * properties )
{
	EdPropertyList* myProperties = new EdPropertyList( "Placeable", properties );
	myProperties->SetPropertyFlags(PF_ReadOnly);

	MxProperty( myProperties, APlaceable, EdProperty_Vector3D, "position", GetOrigin, SetOrigin );
	MxProperty( myProperties, APlaceable, EdProperty_Quat, "orientation", GetOrientation, SetOrientation );
	MxProperty( myProperties, APlaceable, EdProperty_Float, "scale", GetScale, SetScale );
}


AEditable* F_Get_Editor_For_Resource( SResPtrBase& resRef, EAssetType resType )
{
	CHK_VRET_NIL_IF_NOT( resRef.IsValid() );

	if( resType == EAssetType::Asset_Graphics_Material )
	{
		rxMaterial* pMaterial = resRef.m_pointer->UpCast< rxMaterial >();
		if( pMaterial->pEditor.isNull() ) {
			Material_Editor::Create_Editor_For_Material( pMaterial );
		}
		return pMaterial->pEditor;
	}

	return nil;
}

//const SAssetInfo* F_URL_To_Asset_Info( const QMimeData* mimeData )
//{
//	if( mimeData->hasUrls() )
//	{
//		const QList<QUrl> urlsList = mimeData->urls();
//		Assert( urlsList.count() == 1 );
//
//		const QUrl url = urlsList.first();
//
//		//DBGOUT("RL = %s\n", url.toString().toAscii().data());
//
//		const QString localFile = url.toLocalFile();
//
//		if( !localFile.isEmpty() )
//		{
//			//DBGOUT("local file = %s\n", localFile.toAscii().data());
//
//			const SAssetInfo* pAssetInfo = pAssetDb->Get_Asset_Info_By_Path( localFile.toAscii().data() );
//
//			return pAssetInfo;
//		}
//	}
//	return nil;
//}

const ObjectGUID F_URL_To_Asset_Guid( const QMimeData* mimeData )
{
	if( mimeData->hasUrls() )
	{
		const QList<QUrl> urlsList = mimeData->urls();
		Assert( urlsList.count() == 1 );

		const QUrl url = urlsList.first();

		//DBGOUT("RL = %s\n", url.toString().toAscii().data());

		const QString localFile = url.toLocalFile();

		if( !localFile.isEmpty() )
		{
			//DBGOUT("local file = %s\n", localFile.toAscii().data());

			//const SAssetInfo* pAssetInfo = pAssetDb->Get_Asset_Info_By_Path( localFile.toAscii().data() );

			const ObjectGUID assetGuid = Resources::AssetPathToGuid( localFile.toAscii().data() );
			return assetGuid;
		}
	}
	return ObjectGUID(_InitInvalid);
}
