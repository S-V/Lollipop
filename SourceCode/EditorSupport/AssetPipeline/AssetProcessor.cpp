#include <EditorSupport_PCH.h>
#pragma hdrstop

#include "TextureConverter.h"
#include "MeshConverter.h"
#include "MaterialProcessor.h"

namespace Editor
{

	struct PrivateData
	{
		TMap< String, AssetProcessor* >	importersByFileExtension;
		TMap< UINT, AssetProcessor* >	importersByAssetTypeEnum;
	};
	static TPtr< PrivateData >	gData;


	struct RegisterImportersUtil
	{
		TextureConverter	textureImporter;
		MeshConverter		meshImporter;
		MaterialProcessor	materialProcessor;
	};
	static TPtr< RegisterImportersUtil >	gRegisterImportersUtil;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//const char* EAssetType_To_FileExtension( EResourceType eResType )
//{
//	Assert( eResType != EResourceType::Asset_Unknown );
//
//	switch( eResType )
//	{
//	case EResourceType::Asset_Texture2D :
//		return ".texture";
//
//	case EResourceType::Asset_Graphics_Material :
//		return ".material";
//
//	case EResourceType::Asset_Static_Mesh :
//		return ".mesh";
//
//	case EResourceType::Asset_Unknown :
//	default:
//		/*Unreachable*/;
//	}
//	return ".unknown";
//}

//const char* EAssetType_To_BinFileExtension( EResourceType eResType )
//{
//	Assert( eResType != EResourceType::Asset_Unknown );
//
//	switch( eResType )
//	{
//	case EResourceType::Asset_Texture2D :
//		return ".dds";
//
//	case EResourceType::Asset_Graphics_Material :
//		return ".mtl";
//
//	case EResourceType::Asset_Mesh :
//		return ".geo";
//
//	case EResourceType::Asset_Unknown :
//	default:
//		Unreachable;
//	}
//	return ".unknown";
//}

static
String GetNormalizedFileExtension( PCHARS szFileExtension )
{
	String	strFileExtension( szFileExtension );

	strFileExtension.ToLower();
	strFileExtension.StripLeading('.');
	strFileExtension.StripTrailing('.');

	return strFileExtension;
}

//---------------------------------------------------------------------------

static
AssetProcessor* FindAssetImporterByFileExtension( PCHARS szFileExtension )
{
	AssetProcessor* pExistingProcessor = gData->importersByFileExtension.FindRef( szFileExtension );
	//AssertPtr(pExistingProcessor);
	return pExistingProcessor;
}

//---------------------------------------------------------------------------

void F_RegisterAssetProcessor(PCHARS szFileExtension,
					   AssetProcessor* pAssetImporter)
{
	const String strFileExtension = GetNormalizedFileExtension( szFileExtension );

	{
		AssetProcessor* pExistingProcessor = gData->importersByFileExtension.FindRef( szFileExtension );
		if( pExistingProcessor != nil )
		{
			mxWarnf("Asset importer for files with extension '%s' has already been registered\n",strFileExtension.ToChars());
		}

		mxPutf("RegisterAssetImporter: file extension '%s'\n",szFileExtension);

		gData->importersByFileExtension.Set( strFileExtension, pAssetImporter );
	}


	{
		const EAssetType assetType = pAssetImporter->GetAssetType();
		CHK_VRET_IF_NOT( assetType != EAssetType::Asset_Unknown );

		AssetProcessor* pExistingProcessor = gData->importersByAssetTypeEnum.FindRef( assetType );
		if( pExistingProcessor != nil )
		{
			if( pExistingProcessor != pAssetImporter ) {
				mxWarnf("Asset importer for resources of type '%s' has already been registered\n",EAssetType_To_Chars(assetType));
			}
		}

		gData->importersByAssetTypeEnum.Set( assetType, pAssetImporter );
	}
}

//---------------------------------------------------------------------------

void F_ProcessAsset(const ProcessFileInput& input,
				 ProcessFileOutput &output)
{

	String	strFileExt;
	input.filePath.ExtractFileExtension( strFileExt );

	strFileExt = GetNormalizedFileExtension( strFileExt );


	AssetProcessor* pProcessor = FindAssetImporterByFileExtension( strFileExt );

	if( pProcessor != nil )
	{
		mxPutf("Processing file '%s'\n",input.filePath.ToChars());

		return pProcessor->ProcessFile(input,output);
	}
	else
	{
		mxWarnf("Failed to process file '%s'\n",input.filePath.ToChars());
	}
}

AssetProcessor* F_GetAssetProcessorByAssetType( EAssetType assetType )
{
	CHK_VRET_NIL_IF_NOT( assetType != EAssetType::Asset_Unknown );
	AssetProcessor* pExistingProcessor = gData->importersByAssetTypeEnum.FindRef( assetType );
	AssertPtr(pExistingProcessor);
	return pExistingProcessor;
}

}//namespace Editor

//---------------------------------------------------------------------------

ProcessFileOutput::ProcessFileOutput()
	: intermediateData( EMemHeap::HeapTemp )
	, compiledData( EMemHeap::HeapTemp )
{
	assetType = EAssetType::Asset_Unknown;
}

bool ProcessFileOutput::IsValid() const
{
	return assetType != EAssetType::Asset_Unknown
		&& compiledData.IsValid()
		;
}


void AssetProcessor::SaveIntermediateData( AssetData* assetData, AStreamWriter &stream )
{
	CHK_VRET_IF_NIL( assetData );

	ABinaryObjectWriter	serializer( stream );
	serializer.Save( *assetData );
}

AssetData* AssetProcessor::LoadIntermediateData( AStreamReader& stream )
{
	ABinaryObjectReader		reader( stream );

	AssetData *		pAssetData;
	reader.Load( pAssetData );

	AssertPtr( pAssetData );

	return pAssetData;
}

//---------------------------------------------------------------------------

namespace Editor
{

static
void RegisterAssetImporters()
{
	gData.ConstructInPlace();
	gRegisterImportersUtil.ConstructInPlace();
}

static
void ShutdownAssetImporters()
{
	gRegisterImportersUtil.Destruct();
	gData.Destruct();
}

//---------------------------------------------------------------------------

void SetupContentPipeline()
{
	graphics.Initialize();

	RegisterAssetImporters();
}

void CloseContentPipeline()
{
	ShutdownAssetImporters();

	graphics.Shutdown();
}

}//namespace Editor
