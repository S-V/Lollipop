// content database used in development/editor mode;
// asset files are 'loosely' located in disk folders,
// some resources can be serialized into highly flexible/modifiable/human readable format
// (e.g. JSON, XML, plain text).
// in release mode, another resource database will be used.
// 
#pragma once

#include <Core/Resources.h>

#include <EditorSupport/EditorSupport.h>

#include <EditorSupport/AssetPipeline.h>
#include <EditorSupport/AssetPipeline/AssetProcessor.h>
#include <EditorSupport/Util/DevFileMonitor.h>

class DevFileMonitor;

// 'baseFolder' can be nil if database is being unloaded
//typedef void F_OnAssetDatabaseChanged( const char* baseFolder );

extern const char*	ASSET_DB_FILE_EXTENSION;

///*
//-----------------------------------------------------------------------------
//	AssetGuidGenerator
//-----------------------------------------------------------------------------
//*/
//class AssetGuidGenerator
//{
//	UINT	m_maxGuid;	// for generating unique IDs
//
//public:
//	AssetGuidGenerator();
//	~AssetGuidGenerator();
//
//	UINT GenerateUniqueId();
//
//	void Serialize( mxArchive & archive );
//};


// for quick mapping between resource names and integer IDs
typedef TMap< String, ObjectGUID >	GuidByNameMap;
typedef TMap< ObjectGUID, String  >	NameByGuidMap;

/*
-----------------------------------------------------------------------------
	AssetGuidsCache

	maps string resource ids to integers and vice versa
-----------------------------------------------------------------------------
*/
class AssetGuidsCache
{
	// resource name -> resource guid
	GuidByNameMap		m_guidByName;

	// resource guid -> resource name
	NameByGuidMap		m_nameByGuid;

public:
	AssetGuidsCache();

	void Build( const char* pathToSrcAssets );
	void Serialize( mxArchive & archive );
	void Clear();

	const ObjectGUID* FindGuidByName( const char* assetName ) const;

	void Remove( const char* assetName );
	void Register( const char* assetName );

public_internal:	//=== for Win32_ProcessDirectoryRecursive()
	bool edEnterFolder( PCHARS folder, int unused );
	void edLeaveFolder( PCHARS folder, int unused );
	void edProcessFile( PCHARS fileName, int unused );
};

typedef TMap< ObjectGUID, SAssetInfo >	InfoByGuidMap;

/*
-----------------------------------------------------------------------------
	DevAssetManager
-----------------------------------------------------------------------------
*/
class DevAssetManager : public AContentDatabase
	, public FileWatcherListener

	// for now, only single resource database is supported
	, SingleInstance< DevAssetManager >
{
public:
	mxDECLARE_CLASS( DevAssetManager, AContentDatabase );

	DevAssetManager();

	// doesn't save the asset database automatically
	~DevAssetManager();



	struct SCreateArgs
	{
		String	pathToSrcAssets;
		String	pathToIntAssets;
		String	pathToBinAssets;
	};

	// creates a new asset database
	// accepts path to source assets
	// and folders for storing compiled asset cache
	//
	void CreateNew( const SCreateArgs& cInfo );


	bool OpenExisting( const char* filePath );


	void SaveToFile( const char* filePath );


	// does a full rebuild from source;
	// rescans the source directory, compares file times,
	// regenerates cache with compiled assets
	//
	void Refresh();


	void Close();


	bool IsOpened() const;


	// returns the number of assets in the database
	UINT NumFiles() const;



	const String& GetPathToSrcAssets() const { return m_pathToSrcAssets; }

	bool Check_ValidAssetFilePath( const char* filePath ) const;


	// tests if the file is an asset and is registered in the database
	bool ContainsAsset( const char* filePath ) const;


	// returns the description of the asset
	const SAssetInfo* Find_Asset_Info_By_Guid( ObjectGUIDArg assetGuid ) const;
	const SAssetInfo* Find_Asset_Info_By_Name( const char* assetName ) const;
	const SAssetInfo* Find_Asset_Info_By_Path( const char* filePath ) const;

	// get or load
	const SAssetInfo* Get_Asset_Info_By_Path( const char* filePath ) const;

	AssetData* LoadAssetData( const SAssetInfo& assetInfo );


	// Creates a new asset at path.
	// You must ensure that the path uses a supported extension ('.mat' for materials, '.cubemap' for cubemaps, '.GUISkin' for skins, '.anim' for animations and '.asset' for arbitrary other assets.)
	// All paths are relative to the project folder. Like: "Assets/MyTextures/hello.png"
	void CreateAsset( const void* o, const mxType& typeInfo, const char* filePath );

	template< typename TYPE >
	void CreateAsset( const TYPE& o, const char* filePath )
	{
		this->CreateAsset( &o, TYPE::StaticGetReflection(), filePath );
	}





public:	//== AContentDatabase

	virtual PakFileHandle OpenFile( ObjectGUIDArg fileGuid ) override;

	virtual void CloseFile( PakFileHandle file ) override;

	// Summary:
	//   Get the file size in bytes (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual UINT GetFileSize( PakFileHandle file ) override;

	// Reads the file into the preallocated buffer
	virtual SizeT ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead ) override;



	//// Get the GUID for the asset at path.
	//// All paths are relative to the project folder. Like: "Assets/MyTextures/hello.png"
	//ObjectGUIDArg AssetPathToGUID( const char* filePath ) const;


	//// Translate a GUID to its current asset path.
	//// All paths are relative to the project folder. Like: "Assets/MyTextures/hello.png"
	//const String AssetPathToGUID( ObjectGUIDArg assetGuid ) const;

	virtual String GetBasePath() override;

	// Translate a GUID to its current asset path.
	// this is mainly used for serialization into human-readable text formats
	//
	virtual String GuidToAssetPath(
		ObjectGUIDArg resourceGuid
	) const override;

	// Get the GUID for the asset at path
	//
	virtual ObjectGUIDArg AssetPathToGuid(
		const String& resourcePath
	) const override;



public:	//=-- FileWatcherListener
	virtual void onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId ) override;
	virtual void onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId ) override;

public_internal:	//=== for Win32_ProcessDirectoryRecursive()
	bool edEnterFolder( PCHARS folder, void* userData );
	void edLeaveFolder( PCHARS folder, void* userData );
	void edProcessFile( PCHARS fileName, void* userData );

private:
	void Serialize( mxArchive & archive );

	void Install_File_Watcher();
	void Remove_File_Watcher();

	// erase previous information about the asset at the specified path
	bool Import_New_Asset( const char* fileName );

	// recompile if the source asset data was modified
	bool Process_Changed_Asset( const char* fileName );
	bool Process_Renamed_Asset( const char* oldFileName, const char* newFileName );
	bool Remove_Existing_Asset( const char* fileName );

	bool Check_Source_Asset_File_Name_Is_Ok( const char* fileName ) const;
	bool Check_Source_Asset_File_Path_Is_Ok( const char* filePath ) const;
	bool Check_Source_Asset_File_Is_In_Database( const char* filePath ) const;
	bool Process_Asset_Internal( const ProcessFileInput& input, ProcessFileOutput &output );

	// returns an empty string in case of error
	const String Get_Asset_Name_From_File_Path( const char* filePath ) const;

private:

	// persistent variables

	// path to source art with their corresponding build options (i.e. 'configs', 'presets')
	String		m_pathToSrcAssets;

	// path to assets in intermediate form
	String		m_pathToIntAssets;

	// path to compiled assets, engine can only load resources in binary formats
	String		m_pathToBinAssets;


	AssetGuidsCache		m_assetIDs;

	InfoByGuidMap		m_infoByGuid;

	// all binary files ever created - for garbage collection
	TList< String >		m_compiledFiles;

	// in memory only

	DevFileMonitor		m_fileWatcher;	// file monitor for detecting source file changes
};


namespace Editor
{
	inline
	DevAssetManager* GetAssetDb()
	{
		DevAssetManager* assMgr = UpCast< DevAssetManager >( gCore.resources->GetContentDatabase() );
		AssertPtr(assMgr);
		return assMgr;
	}

}//namespace Editor
