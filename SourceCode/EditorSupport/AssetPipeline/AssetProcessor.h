#pragma once

#include <Core/Resources.h>

#include <EditorSupport/AssetPipeline.h>


namespace Editor
{

void SetupContentPipeline();
void CloseContentPipeline();

}//namespace Editor

/*
-----------------------------------------------------------------------------
	AssetProcessor
-----------------------------------------------------------------------------
*/
struct ProcessFileInput
{
	String		filePath;	// path to file with raw (source) asset data
};
struct ProcessFileOutput
{
	// type of resource
	EAssetType	assetType;

	// file extension of final, compiled asset data for loading by the engine
	String		fileExtension;	// e.g. ".dds", ".geo"

	// [optional]
	// converted asset with lots of useful information
	MemoryBlob	intermediateData;

	// [required]
	// compiled asset data in binary form, ready to be loaded by the engine
	MemoryBlob	compiledData;

public:
	ProcessFileOutput();

	bool IsValid() const;
};

/*
-----------------------------------------------------------------------------
	AssetProcessor

	represents a single node in asset pipeline;

	converts source assets into binary form
	suitable for loading by the engine
-----------------------------------------------------------------------------
*/
struct AssetProcessor
{
	virtual EAssetType GetAssetType() const = 0;

	virtual void ProcessFile( const ProcessFileInput& input, ProcessFileOutput &output ) = 0;

	virtual void SaveIntermediateData( AssetData* assetData, AStreamWriter &stream );
	virtual AssetData* LoadIntermediateData( AStreamReader& stream );

	virtual ~AssetProcessor() {}
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace Editor
{

//extern
//const char* EAssetType_To_FileExtension( EResourceType eResType );

//extern
//const char* EAssetType_To_BinFileExtension( EResourceType eResType );

// extension must include dot '.'
void F_RegisterAssetProcessor(PCHARS szFileExtension,
						   AssetProcessor* pAssetImporter);

void F_ProcessAsset(const ProcessFileInput& input,
				 ProcessFileOutput &output);

AssetProcessor* F_GetAssetProcessorByAssetType( EAssetType assetType );

}//namespace Editor
