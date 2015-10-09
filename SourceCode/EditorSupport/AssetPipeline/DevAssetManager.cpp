#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Base/Math/Hashing/HashFunctions.h>

#include <Core/Resources.h>

#include <EditorSupport/AssetPipeline.h>
#include <EditorSupport/AssetPipeline/AssetProcessor.h>
#include <EditorSupport/AssetPipeline/DevAssetManager.h>
#include <EditorSupport/Serialization/TextSerializer.h>


const char*	ASSET_DB_FILE_EXTENSION = ".rdb";
const char*	ASSET_DB_FILE_EXTENSION_WITHOUT_DOT = "rdb";

static
bool F_CanProcessAssetFile( const char* filePath )
{
	const char* ext = V_GetFileExtension( filePath );
	if( nil == ext ) {
		mxWarnf("File '%s' is without extension.\n",filePath);
		return false;
	}

	// check if this is a resource catalog
	if( mxStrEquAnsi( ext, ASSET_DB_FILE_EXTENSION_WITHOUT_DOT ) ) {
		return false;
	}

	// check if the file contains asset build options
	if( mxStrEquAnsi( ext, ASSET_BUILD_CONFIG_FILE_EXTENSION_WITHOUT_DOT ) ) {
		DEVOUT("Process Assets: Skipping build configuration script '%s'\n",filePath);
		return false;
	}

	return true;
}

static
String F_Extract_Asset_Name( const char* filePath )
{
	const String fullFileName( filePath );

	// extract file name without path
	String	fileName;
	fullFileName.ExtractFileName( fileName );

	Assert( !fileName.IsEmpty() );

	return fileName;
}

static
bool F_SaveBlobAs( const MemoryBlob& data, const char* directory, const char* filename )
{
	AssertPtr(directory);
	VRET_FALSE_IF_NIL(directory);
	
	AssertPtr(filename);
	VRET_FALSE_IF_NIL(filename);

	String		normalizedFilePath( directory );
	F_NormalizePath( normalizedFilePath );

	normalizedFilePath.Append( filename );


	return Util_SaveBlobToFile( normalizedFilePath, data );
}

static
String Compose_Intermediate_Asset_File_Name( const char* filePath )
{
	const String	fileName = F_Extract_Asset_Name( filePath );
	Assert( !fileName.IsEmpty() );

	String	intAssetFileName( fileName );
	intAssetFileName.StripFileExtension();
	//intAssetFileName.SetFileExtension( output.intFileExtension );

	return intAssetFileName;
}

static
String Compose_Intermediate_Asset_File_Name( const ProcessFileInput& input, ProcessFileOutput &output )
{
	(void)output;

	return Compose_Intermediate_Asset_File_Name( input.filePath );
}

static
String GetBinAssetFileName( const ProcessFileInput& input, ProcessFileOutput &output )
{
	const String	fileName = F_Extract_Asset_Name( input.filePath );
	Assert( !fileName.IsEmpty() );

	String	binAssetFileName( fileName );
	binAssetFileName.StripFileExtension();
	binAssetFileName.SetFileExtension( output.fileExtension );

	return binAssetFileName;
}

// 1 so that it cannot be confused with mxNULL_OBJECT_GUID
enum { START_ASSET_GUID = 1 };
mxSTATIC_ASSERT( START_ASSET_GUID != mxNULL_OBJECT_GUID );



/*
-----------------------------------------------------------------------------
	AssetGuidsCache
-----------------------------------------------------------------------------
*/
AssetGuidsCache::AssetGuidsCache()
{

}

void AssetGuidsCache::Build( const char* pathToSrcAssets )
{
	Win32_ProcessDirectoryRecursive( pathToSrcAssets, *this, 0 );
}

void AssetGuidsCache::Serialize( mxArchive & archive )
{
	archive && m_guidByName;
	archive && m_nameByGuid;
}

void AssetGuidsCache::Clear()
{
	m_guidByName.Empty();
	m_nameByGuid.Empty();
}

bool AssetGuidsCache::edEnterFolder( PCHARS folder, int unused )
{
	DBGOUT("Entering folder: '%s'\n", folder);
	return false;
}

void AssetGuidsCache::edLeaveFolder( PCHARS folder, int unused )
{
	DBGOUT("Leaving folder: '%s'\n", folder);
}

void AssetGuidsCache::edProcessFile( PCHARS fileName, int unused )
{
	this->Register( fileName );
	mxUNUSED(unused);
}

const ObjectGUID* AssetGuidsCache::FindGuidByName( const char* assetName ) const
{
	return m_guidByName.Find( assetName );
}

void AssetGuidsCache::Register( const char* assetName )
{
	const char* fileName = assetName;

	DBGOUT("Processing file: '%s'\n", fileName);

	// generate unique file id

	const ObjectGUID fileGuid( FNV32_StringHash( fileName ) );

	Assert( !m_guidByName.Contains( fileName ) );
	Assert( !m_nameByGuid.Contains( fileGuid ) );

	m_guidByName.Set( fileName, fileGuid );
	m_nameByGuid.Set( fileGuid, fileName );
}

void AssetGuidsCache::Remove( const char* assetName )
{
	const ObjectGUID* existingGuid = m_guidByName.Find( assetName );
	CHK_VRET_IF_NIL( existingGuid );

	m_guidByName.Remove( assetName );
	m_nameByGuid.Remove( *existingGuid );
}

/*
-----------------------------------------------------------------------------
	DevAssetManager
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( DevAssetManager );

DevAssetManager::DevAssetManager()
	: m_fileWatcher( *this )
{
	// clear for the first time
	this->Close();
}

DevAssetManager::~DevAssetManager()
{
	// final cleanup
	this->Close();
}

void DevAssetManager::Close()
{
	gCore.resources->SetContentDatabase( nil );

	this->Remove_File_Watcher();

	m_pathToSrcAssets.Empty();
	m_pathToIntAssets.Empty();
	m_pathToBinAssets.Empty();

	m_assetIDs.Clear();
	m_infoByGuid.Empty();

	m_compiledFiles.Empty();
}

bool DevAssetManager::IsOpened() const
{
	return m_fileWatcher.IsOpened();
}

void DevAssetManager::CreateNew( const SCreateArgs& cInfo )
{
	this->Close();

	// Create a new resource database.

	m_pathToSrcAssets = cInfo.pathToSrcAssets;
	F_NormalizePath( m_pathToSrcAssets );

	m_pathToIntAssets = cInfo.pathToIntAssets;
	F_NormalizePath( m_pathToIntAssets );

	m_pathToBinAssets = cInfo.pathToBinAssets;
	F_NormalizePath( m_pathToBinAssets );

	this->Refresh();

	this->Install_File_Watcher();

	gCore.resources->SetContentDatabase( this );
}

bool DevAssetManager::OpenExisting( PCHARS filePath )
{
	AssertPtr(filePath);
	VRET_FALSE_IF_NIL(filePath);
	DEVOUT("DevAssetManager::OpenExisting( '%s' ) {\n", filePath);

	this->Close();

	// load catalog file
	{
		FileReader	file( filePath );
		VRET_FALSE_IF_NOT( file.IsOpen() );

		ArchivePODReader	archive( file );
		this->Serialize( archive );
	}

	this->Install_File_Watcher();

	gCore.resources->SetContentDatabase( this );

	return true;
}

void DevAssetManager::SaveToFile( const char* filePath )
{
	AssertPtr(filePath);
	VRET_IF_NIL(filePath);
	DEVOUT("DevAssetManager::SaveToFile( '%s' ) {\n", filePath);

	// serialize catalog file
	{
		FileWriter	file( filePath );
		VRET_IF_NOT( file.IsOpen() );

		ArchivePODWriter	archive( file );
		this->Serialize( archive );
	}
}

void DevAssetManager::Refresh()
{
	CHK_VRET_IF_NOT( !m_pathToSrcAssets.IsEmpty() );
	CHK_VRET_IF_NOT( !m_pathToIntAssets.IsEmpty() );
	CHK_VRET_IF_NOT( !m_pathToBinAssets.IsEmpty() );

	DEVOUT("\tAsset Manager : Rebuild All - BEGIN\n");
	DEVOUT("\t pathToSrcAssets: '%s'\n", m_pathToSrcAssets.ToChars());
	DEVOUT("\t pathToIntAssets: '%s'\n", m_pathToIntAssets.ToChars());
	DEVOUT("\t pathToBinAssets: '%s'\n", m_pathToBinAssets.ToChars());

	m_assetIDs.Clear();
	m_infoByGuid.Empty();

	DBGOUT("Deleting compiled assets...\n");
	for( UINT i=0; i < m_compiledFiles.Num(); i++ )
	{
		const String	filePath = m_pathToBinAssets + m_compiledFiles[i];
		FS_EraseFile( filePath );
	}
	m_compiledFiles.Empty();

	const UINT startTimeMSec = mxGetMilliseconds();

	// first pass - build a table which maps string resource ids to integer ids

	m_assetIDs.Build( m_pathToSrcAssets );

	gCore.resources->SetContentDatabase( this );

	// second pass - import and compile assets, build dependency graphs

	Win32_ProcessDirectoryRecursive( m_pathToSrcAssets, *this, (void*)nil );

	const UINT elapsedSeconds = (mxGetMilliseconds() - startTimeMSec ) / 1000;
	DEVOUT("\tAsset Manager : Rebuild All - END (%u file(s) in %u seconds)\n",
		this->NumFiles(), elapsedSeconds);
}

UINT DevAssetManager::NumFiles() const
{
	return m_infoByGuid.NumEntries();
}

void DevAssetManager::Serialize( mxArchive & archive )
{
	archive && m_pathToSrcAssets;
	archive && m_pathToIntAssets;
	archive && m_pathToBinAssets;

	m_assetIDs.Serialize( archive );

	archive && m_infoByGuid;

	archive && m_compiledFiles;


	if( archive.IsWriting() )
	{
		DEVOUT("Saved asset database (%u files)\n",
			(UINT)this->NumFiles());
	}
	if( archive.IsReading() )
	{
		DEVOUT("Loaded asset database (%u files)\n",
			(UINT)this->NumFiles());
	}
}


mxSTATIC_ASSERT( sizeof FileHandle == sizeof PakFileHandle );
static inline
const FileHandle Pak_To_OS_File_Handle( const PakFileHandle fileHandle )
{
	return (FileHandle)fileHandle;
}
static inline
const PakFileHandle OS_To_Pak_File_Handle( const FileHandle fileHandle )
{
	return (PakFileHandle)fileHandle;
}

PakFileHandle DevAssetManager::OpenFile( ObjectGUIDArg fileGuid )
{
	if( fileGuid.IsNull() ) {
		return BadPakFileHandle;
	}

	const SAssetInfo* pAssetInfo = this->Find_Asset_Info_By_Guid( fileGuid );
	const String fileName = m_pathToBinAssets + pAssetInfo->binFileName.ToChars();

	const FileHandle fileHandle = FS_OpenFile( fileName, EAccessMode::ReadAccess );

	if( fileHandle == InvalidFileHandle ) {
		mxErrf("Failed to open file '%s' for reading.\n", fileName.ToChars());
		return BadPakFileHandle;
	}

	return OS_To_Pak_File_Handle( fileHandle );
}

void DevAssetManager::CloseFile( PakFileHandle file )
{
	const FileHandle fileHandle = Pak_To_OS_File_Handle( file ); 
	return FS_CloseFile( fileHandle );
}

UINT DevAssetManager::GetFileSize( PakFileHandle file )
{
	const FileHandle fileHandle = Pak_To_OS_File_Handle( file ); 
	return FS_GetFileSize( fileHandle );
}

SizeT DevAssetManager::ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead )
{
	const FileHandle fileHandle = Pak_To_OS_File_Handle( file ); 
	CHK(FS_SeekFile( fileHandle, startOffset, ESeekOrigin::Begin ));
	return FS_ReadFile( fileHandle, buffer, bytesToRead );
}

String DevAssetManager::GetBasePath()
{
	return m_pathToSrcAssets;
}

String DevAssetManager::GuidToAssetPath( ObjectGUIDArg resourceGuid ) const
{
	if( resourceGuid.IsValid() )
	{
		const SAssetInfo* pAssetInfo = this->Find_Asset_Info_By_Guid( resourceGuid );
		AssertPtr(pAssetInfo);
		if( pAssetInfo != nil )
		{
			if(1)
			{
				// return file name without path
				String assetPath = pAssetInfo->srcFileName.ToChars();
				return assetPath;
			}
			if(0)
			{
				// return path relative to the base folder
				String basePath;

				// e.g.: "D:/dev/assets/" -> "assets/"
				m_pathToSrcAssets.ExtractFileBase( basePath );

				// e.g.: "D:/dev/assets/hand.mdl" -> "assets/hand.mdl"
				String assetPath = basePath + pAssetInfo->srcFileName.ToChars();
				return assetPath;
			}
			if(0)
			{
				// return absolute file path
				String assetPath = m_pathToSrcAssets + pAssetInfo->srcFileName.ToChars();
				return assetPath;
			}
		}
	}
	return String();
}

ObjectGUIDArg DevAssetManager::AssetPathToGuid( const String& resourcePath ) const
{
	if( !resourcePath.IsEmpty() )
	{
		// extract resource name
		const String resourceName = this->Get_Asset_Name_From_File_Path( resourcePath );

		// strip file path
		const ObjectGUID* pAssetGuid = m_assetIDs.FindGuidByName( resourceName );
		AssertPtr(pAssetGuid);
		if( pAssetGuid != nil ) {
			return *pAssetGuid;
		}
	}
	return ObjectGUID(_InitInvalid);
}

bool DevAssetManager::Check_ValidAssetFilePath( const char* filePath ) const
{
	AssertPtr(filePath);
	VRET_X_IF_NIL(filePath, false);

	//const bool bCaseSensitive = false;
	//const INT result = String::FindText( filePath, m_pathToSrcAssets, bCaseSensitive );
	//Assert( result != -1 );
	//if( result == -1 ) {
	//	mxWarnf("Invalid path '%s', should be a subfolder of '%s'\n"
	//		,filePath,m_pathToSrcAssets.ToChars());
	//	return false;
	//}

	return true;
}

const SAssetInfo* DevAssetManager::Find_Asset_Info_By_Guid( ObjectGUIDArg assetGuid ) const
{
	Assert( assetGuid.IsValid() );
	const SAssetInfo* pAssetInfo = m_infoByGuid.Find( assetGuid );
	//AssertPtr( pAssetInfo );
	return pAssetInfo;
}

const SAssetInfo* DevAssetManager::Find_Asset_Info_By_Name( const char* assetName ) const
{
	const ObjectGUID* pObjectGUID = m_assetIDs.FindGuidByName( assetName );
	//AssertPtr( pObjectGUID );
	VRET_NIL_IF_NIL( pObjectGUID );

	const SAssetInfo* pAssetInfo = this->Find_Asset_Info_By_Guid( *pObjectGUID );
	return pAssetInfo;
}

const SAssetInfo* DevAssetManager::Find_Asset_Info_By_Path( const char* filePath ) const
{
	CHK_VRET_NIL_IF_NOT( Check_ValidAssetFilePath( filePath ) );
	const String assetName = F_Extract_Asset_Name( filePath );
	const SAssetInfo* pAssetInfo = this->Find_Asset_Info_By_Name( assetName );
	return pAssetInfo;
}

const SAssetInfo* DevAssetManager::Get_Asset_Info_By_Path( const char* filePath ) const
{
	CHK_VRET_NIL_IF_NOT( Check_ValidAssetFilePath( filePath ) );
	VRET_NIL_IF_NOT( F_CanProcessAssetFile( filePath ) );

	//if( !this->ContainsAsset( filePath ) )
	//{
	//	VRET_NIL_IF_NOT( this->Import_New_Asset( filePath ) );
	//}

	//Assert( this->ContainsAsset( filePath ) );
	if( !this->ContainsAsset( filePath ) )
	{
		mxWarnf("DevAssetManager::Get_Asset_Info_By_Path( '%s' ) failed\n", filePath);
		return nil;
	}

	const SAssetInfo* pAssetInfo = this->Find_Asset_Info_By_Path( filePath );
	return pAssetInfo;
}

AssetData* DevAssetManager::LoadAssetData( const SAssetInfo& assetInfo )
{
	VRET_NIL_IF_NOT( assetInfo.DbgChk_IsValid() );

	const String filePath = m_pathToIntAssets + Compose_Intermediate_Asset_File_Name( assetInfo.srcFileName.ToChars() );

	FileReader	fileIntermediateAssetData( filePath, FileRead_NoOpenError );
	if( !fileIntermediateAssetData.IsOpen() )
	{
		mxWarnf("Failed to load intermediate asset data (file:%s)\n", filePath.ToChars());
		return nil;
	}

	AssetProcessor* pAssetProcessor = Editor::F_GetAssetProcessorByAssetType( assetInfo.assetType );
	CHK_VRET_NIL_IF_NIL( pAssetProcessor );

	AssetData * pAssetData = pAssetProcessor->LoadIntermediateData( fileIntermediateAssetData );
	return pAssetData;
}

void DevAssetManager::CreateAsset( const void* o, const mxType& typeInfo, const char* filePath )
{
	AssertPtr(o);
	VRET_IF_NIL(o);
UNDONE;
	//Assert(Check_ValidAssetFilePath(filePath));

	//FileWriter			fileWriter( filePath );
	//VRET_IF_NOT(fileWriter.IsOpen());

	//TextObjectWriter	serializer;
	//serializer.SerializeObject( o, typeInfo );

	//serializer.WriteAllToStream( fileWriter );
}

void DevAssetManager::Install_File_Watcher()
{
	m_fileWatcher.Start_Listening( m_pathToSrcAssets.ToChars() );
}

void DevAssetManager::Remove_File_Watcher()
{
	m_fileWatcher.End_Listening();
}

bool DevAssetManager::edEnterFolder( PCHARS folder, void* userData )
{
	DBGOUT("Entering folder: '%s'\n", folder);
	return false;
}

void DevAssetManager::edLeaveFolder( PCHARS folder, void* userData )
{
	DBGOUT("Leaving folder: '%s'\n", folder);
}

void DevAssetManager::edProcessFile( PCHARS fileName, void* userData )
{
	this->Import_New_Asset( fileName );
}

void DevAssetManager::onFileCreated( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	String fileName( mxTO_ANSI( filename ) );

	m_assetIDs.Register( fileName );
	this->Import_New_Asset( fileName );
}

void DevAssetManager::onFileDeleted( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	String fileName( mxTO_ANSI( filename ) );

	this->Remove_Existing_Asset( fileName );
}

void DevAssetManager::onFileChanged( const mxChar* filename, const mxChar* directory, const WatchID watchId )
{
	String fileName( mxTO_ANSI( filename ) );

	this->Process_Changed_Asset( fileName );
}

void DevAssetManager::onFileRenamed( const mxChar* oldFilename, const mxChar* newFilename, const mxChar* directory, const WatchID watchId )
{
	FileWatcherListener::onFileRenamed( oldFilename, newFilename, directory, watchId );

	this->Process_Renamed_Asset( mxTO_ANSI(oldFilename), mxTO_ANSI(newFilename) );
}

bool DevAssetManager::Check_Source_Asset_File_Name_Is_Ok( const char* fileName ) const
{
	CHK_VRET_FALSE_IF_NIL(fileName);

	String	strFileName( fileName );
	String	strFilePath;
	strFileName.ExtractFilePath( strFilePath );
	CHK_VRET_FALSE_IF_NOT( strFilePath.IsEmpty() );

	return true;
}

bool DevAssetManager::Check_Source_Asset_File_Path_Is_Ok( const char* filePath ) const
{
	AssertPtr(filePath);
	VRET_FALSE_IF_NIL(filePath);
	{
		const bool casesensitive = true;
		const INT pos = String::FindText( filePath, m_pathToSrcAssets, casesensitive );
		Assert( pos != INDEX_NONE );
		if( pos == INDEX_NONE ) {
			mxErrf("Invalid source file name: '%s'.\n", filePath);
			return false;
		}
	}
	return true;
}

const String DevAssetManager::Get_Asset_Name_From_File_Path( const char* filePath ) const
{
	String	pathOnly( filePath );
	pathOnly.StripFilename();
	F_NormalizePath( pathOnly );

	String	fileName( filePath );
	fileName.StripPath();

	if( !pathOnly.IsEmpty() )
	{
		if( pathOnly == m_pathToSrcAssets ) {
			return fileName;
		} else {
			// invalid path
			return String();
		}
	}
	else
	{
		return fileName;
	}

	//// if it starts with path to source art, remove path
	//{
	//	const bool casesensitive = true;
	//	const INT pos = String::FindText( filePath, m_pathToSrcAssets, casesensitive );
	//	if( pos != INDEX_NONE )
	//	{
	//		return false;
	//	}
	//}
}

bool DevAssetManager::Check_Source_Asset_File_Is_In_Database( const char* filePath ) const
{
	AssertPtr(filePath);
	VRET_FALSE_IF_NIL(filePath);

	const String	fileName = F_Extract_Asset_Name( filePath );

	const ObjectGUID* assetGuid = m_assetIDs.FindGuidByName( fileName );
	AssertPtr(assetGuid);
	if( assetGuid == nil ) {
		mxErrf("DevAssetManager: couldn't find GUID of '%s'.\n", filePath);
		return false;
	}

	const SAssetInfo* assetInfo = m_infoByGuid.Find( *assetGuid );
	AssertPtr(assetInfo);
	if( assetGuid == nil ) {
		mxErrf("DevAssetManager: couldn't find '%s' info.\n", filePath);
		return false;
	}

	return true;
}

bool DevAssetManager::Process_Asset_Internal( const ProcessFileInput& input, ProcessFileOutput &output )
{
	// initialize ProcessFileInput
	//{
	//	input.filePath = filePath;
	//}

	const char* filePath = input.filePath.ToChars();

	// extract file name without path
	const String	fileName = F_Extract_Asset_Name( filePath );
	Assert( !fileName.IsEmpty() );
	VRET_FALSE_IF_NOT( !fileName.IsEmpty() );


	Editor::F_ProcessAsset( input, output );

	if( !output.IsValid() ) {
		mxWarnf("Failed to process '%s'\n", filePath);
		return false;
	}

	{
		if( !output.intermediateData.IsEmpty() )
		{
			const String	intAssetFileName = Compose_Intermediate_Asset_File_Name( input, output );
			m_compiledFiles.AddUnique( intAssetFileName );

			F_SaveBlobAs( output.intermediateData, m_pathToIntAssets, intAssetFileName );
		}

		// convert the file into binary format suitable for loading by the engine

		Assert( !output.compiledData.IsEmpty() );
		if( !output.compiledData.IsEmpty() )
		{
			const String	binAssetFileName = GetBinAssetFileName( input, output );
			m_compiledFiles.AddUnique( binAssetFileName );

			F_SaveBlobAs( output.compiledData, m_pathToBinAssets, binAssetFileName );
		}
	}


	return true;
}

bool DevAssetManager::Import_New_Asset( const char* fileName )
{
	CHK_VRET_FALSE_IF_NIL( fileName );

	VRET_FALSE_IF_NOT( F_CanProcessAssetFile( fileName ) );

	DEVOUT("Importing '%s'...\n", fileName);

	const ObjectGUID* fileGuid = m_assetIDs.FindGuidByName( fileName );
	AssertPtr( fileGuid );


	ProcessFileInput	processFileInput;
	processFileInput.filePath = m_pathToSrcAssets + fileName;


	ProcessFileOutput	processFileOutput;
	VRET_FALSE_IF_NOT(this->Process_Asset_Internal( processFileInput, processFileOutput ));


	// register the asset in the database


	// generate unique file id

	const UINT serialNumber = this->NumFiles() + START_ASSET_GUID;
	mxUNUSED(serialNumber);


	SAssetInfo	assetInfo;
	{
		assetInfo.srcFileName.SetString( fileName );
		assetInfo.binFileName = GetBinAssetFileName( processFileInput, processFileOutput );
		assetInfo.assetGuid = *fileGuid;
		assetInfo.assetType = processFileOutput.assetType;
	}

	m_infoByGuid.Set( *fileGuid, assetInfo );


	{
		char	assetGuidStr[64];
		(*fileGuid).ToChars( assetGuidStr );

		DEVOUT("DevAssetManager::Import_New_Asset( \"%s\" ) - OK (GUID=%s)\n",
			fileName, assetGuidStr
		);
	}

	return true;
}

bool DevAssetManager::Process_Changed_Asset( const char* fileName )
{
	VRET_FALSE_IF_NOT( F_CanProcessAssetFile( fileName ) );

	DBGOUT("Processing '%s'...\n", fileName);

	VRET_FALSE_IF_NOT(this->Check_Source_Asset_File_Name_Is_Ok( fileName ));
	VRET_FALSE_IF_NOT(this->Check_Source_Asset_File_Is_In_Database( fileName ));


	ProcessFileInput	processFileInput;
	processFileInput.filePath = m_pathToSrcAssets + fileName;

	ProcessFileOutput	processFileOutput;
	VRET_FALSE_IF_NOT(this->Process_Asset_Internal( processFileInput, processFileOutput ));


	DEVOUT("DevAssetManager::Process_Changed_Asset( \"%s\" ) - OK\n", fileName);

	return true;
}

bool DevAssetManager::Remove_Existing_Asset( const char* fileName )
{
	DBGOUT("Removing '%s'...\n", fileName);

	VRET_FALSE_IF_NOT(this->Check_Source_Asset_File_Name_Is_Ok( fileName ));

	m_assetIDs.Remove( fileName );

	{
		const String intFilePath = m_pathToIntAssets + fileName;
		FS_EraseFile( intFilePath );
	}
	{
		const String binFilePath = m_pathToBinAssets + fileName;
		FS_EraseFile( binFilePath );
	}

	DEVOUT("DevAssetManager::Remove_Existing_Asset( \"%s\" ) - OK (%u files)\n",
		fileName, (UINT)this->NumFiles());

	return true;
}

bool DevAssetManager::ContainsAsset( const char* filePath ) const
{
	AssertPtr(filePath);
	VRET_FALSE_IF_NIL(filePath);

	String	fileName( filePath );

	// check if the file is located in source assets folder and extract its name
	{
		const bool casesensitive = true;
		const INT pos = String::FindText( filePath, m_pathToSrcAssets, casesensitive );
		if( pos != INDEX_NONE )
		{
			// get file name relative to base folder
			fileName.StripLeadingOnce( m_pathToSrcAssets );
		}
	}

	return m_assetIDs.FindGuidByName( fileName ) != nil;
}

bool DevAssetManager::Process_Renamed_Asset( const char* oldFileName, const char* newFileName )
{
	VRET_FALSE_IF_NOT( F_CanProcessAssetFile( oldFileName ) );
	VRET_FALSE_IF_NOT( F_CanProcessAssetFile( newFileName ) );

	CHK_VRET_FALSE_IF_NIL( m_assetIDs.FindGuidByName( oldFileName ) );

	//const String fullOldFileName = m_pathToSrcAssets + oldFileName;
	//const String fullNewFileName = m_pathToSrcAssets + newFileName;

	// delete the old data from the database
	this->Remove_Existing_Asset( oldFileName );

	m_assetIDs.Register( newFileName );
	this->Import_New_Asset( newFileName );

	return false;
}

