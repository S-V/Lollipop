// asset pipeline for the editor
// 
#pragma once


#include <EditorSupport/Serialization/TextSerializer.h>


/*
-----------------------------------------------------------------------------
	SAssetInfo
-----------------------------------------------------------------------------
*/
// NOTE: must be POD-type!
struct SAssetInfo
{
	// file name of source asset without path
	// e.g.: "robot_arm.x"
	TStackString<128>		srcFileName;

	// name of compiled file without path
	// e.g.: "robot_arm.my_mesh"
	TStackString<128>		binFileName;

	// file UUID, usually, it's a hash value obtained from the source file name
	ObjectGUID		assetGuid;

	// type of the asset
	AssetType		assetType;

	// C++ class type for looking-up reflection information => file format
	//TypeGUID		classType;

public:
	SAssetInfo();

	bool DbgChk_IsValid() const;
};
mxDECLARE_POD_TYPE( SAssetInfo );


mxREFACTOR("see all uses of this class");
struct EdAssetInfo : public AEditable
{
	const SAssetInfo 	d;

	mxDECLARE_CLASS(EdAssetInfo,AEditable);

	EdAssetInfo( const SAssetInfo& assetInfo )
		: d( assetInfo )
	{}
};


/*
-----------------------------------------------------------------------------
	AssetData

	base class for storing asset data in intermediate format,
	which is usually created by an asset importer.

	The intermediate format is either a standard format like COLLADA
	or a custom defined format that is easy for our tools to read.
	This file is exported from the content creation tool
	and stores all of the information we could possibly desire
	about the asset, both now and in the future.

	Since the intermediate format is designed to be general and easy to read,
	this intermediate format isn't optimized for space or fast loading in-game.
	To create the game-ready format, we compile the intermediate format
	into a game format.
-----------------------------------------------------------------------------
*/
struct AssetData
	: public AObject
	, public ReferenceCounted
{
public:
	mxDECLARE_CLASS(AssetData,AObject);

	typedef TRefPtr< AssetData >	Ref;

public:

	virtual void Serialize( mxArchive & archive ) override;
};


/*
-----------------------------------------------------------------------------
	AssetImporter

	converts source assets into platform-independent intermediate form
-----------------------------------------------------------------------------
*/
class AssetImporter
{
public:
	struct Inputs
	{
		String		filePath;	// path to file with raw (source) asset data
	};
	struct Outputs
	{
		// type of resource
		EAssetType	assetType;

		// converted asset with lots of useful information
		MemoryBlob	intermediateData;
	};

public:
	virtual ~AssetImporter() {}
};


/*
-----------------------------------------------------------------------------
	AssetCompiler

	converts intermediate assets into binary form
	suitable for loading by the engine;
-----------------------------------------------------------------------------
*/
class AssetCompiler
{
public:
	struct Inputs
	{
		String		filePath;	// path to file with raw (source) asset data
	};
	struct Outputs
	{
		// file extension of the compiled asset data
		String		fileExtension;	// e.g. ".dds", ".geo"

		// compiled asset data in binary form, ready to be loaded by the engine
		MemoryBlob	compiledData;
	};
public:
	virtual ~AssetCompiler() {}
};

/*
-----------------------------------------------------------------------------
	JsonBuildConfig

	contains asset import settings
-----------------------------------------------------------------------------
*/

static const char* ASSET_BUILD_CONFIG_FILE_EXTENSION = ".cfg";
static const char* ASSET_BUILD_CONFIG_FILE_EXTENSION_WITHOUT_DOT = "cfg";


class JsonBuildConfig : public INIConfigFile
{
	TAutoPtr< TextReader >	m_textReader;

public:

	static
	bool F_Get_Asset_Build_Config( const char* assetFilePath, JsonBuildConfig & assetBuildOptions )
	{
		String	asset_Build_Config_File_Name( assetFilePath );
		asset_Build_Config_File_Name.Append( ASSET_BUILD_CONFIG_FILE_EXTENSION );

		FileReader	fileReader( asset_Build_Config_File_Name, FileRead_NoOpenError );
		if( !fileReader.IsOpen() ) {
			return false;
		}

		assetBuildOptions.m_textReader = new TextReader( fileReader );

		return true;
	}

	JsonBuildConfig()
	{
	}
	~JsonBuildConfig()
	{
	}

	bool IsOpen() const
	{
		return m_textReader != nil;
	}

	//=-- INIConfigFile

	virtual bool GetString( const char* key, String &outValue ) override
	{
		if( m_textReader != nil ) {
			return m_textReader->Serialize_String( key, outValue );
		}
		return false;
	}
	virtual bool GetUInt( const char* key, UINT &outValue, UINT min = 0, UINT max = MAX_UINT32 ) override
	{
		if( m_textReader != nil ) {
			return m_textReader->Serialize_Uint32( key, outValue )
				&& IsInRangeInc( outValue, min, max );
		}
		return false;
	}
	virtual bool GetFloat( const char* key, FLOAT &outValue, FLOAT min = 0.0f, FLOAT max = MX_INFINITY ) override
	{		
		if( m_textReader != nil ) {
			return m_textReader->Serialize_Float32( key, outValue )
				&& IsInRangeInc( outValue, min, max );
		}
		return false;
	}
	virtual bool GetBool( const char* key, bool &outValue ) override
	{
		if( m_textReader != nil ) {
			return m_textReader->Serialize_Bool( key, outValue );
		}
		return false;
	}
};


/*
-----------------------------------------------------------------------------
	AssetPipeline
-----------------------------------------------------------------------------
*/
namespace AssetPipeline
{

	void Init_AssetPipeline();
	void Close_AssetPipeline();


	void Register_Asset_Importer( const char* fileExtension, AssetImporter* assetImporter );

	AssetImporter* Find_Asset_Importer_By_File_Extension( const char* fileExtension );

	void Register_Asset_Compiler( const EAssetType assetType, AssetCompiler* assetCompiler );

	AssetCompiler* Find_Asset_Compiler_By_Asset_Type( const EAssetType assetType );

};//AssetPipeline
