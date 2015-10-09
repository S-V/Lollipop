#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Core/Serialization/PackageFile.h>

#include <EditorSupport/AssetPipeline/BuildPakFile.h>

bool EdPakFileBuilder::Build_Optimized_Pak_File(
	const char* destFilePath,
	const StringListType& referencedAssets
	)
{
	const UINT numAssets = referencedAssets.Num();
	CHK_VRET_FALSE_IF_NOT(numAssets > 0);


	FileWriter	pakFileWriter( destFilePath );
	CHK_VRET_FALSE_IF_NOT(pakFileWriter.IsOpen());


	AContentDatabase* assetDb = gCore.resources->GetContentDatabase();

	// could get rid of it by using redirecting stream
	MemoryBlob	assetData(EMemHeap::HeapTemp);
	assetData.Reserve(32*mxMEGABYTE);


	OptimizedPakFile	packageFile;

	// Reserve space for the header.

	pakFileWriter << packageFile.m_header;


	// Reserve space for the table of contents.

	const FilePosition tocOffset = pakFileWriter.Tell();

	packageFile.m_entries.SetNum( numAssets );

	pakFileWriter << packageFile.m_entries;


	// Serialize file data.

	SizeT totalPakFileSize = 0;


	for( UINT iAsset = 0; iAsset < numAssets; iAsset++ )
	{
		const String& assetPath = referencedAssets[ iAsset ];
		const ObjectGUID assetGuid = assetDb->AssetPathToGuid( assetPath );
		Assert( assetGuid.IsValid() );
		if( assetGuid.IsNull() ) {
			continue;
		}

		PakFileHandle fileHandle = assetDb->OpenFile( assetGuid );
		Assert( fileHandle != BadPakFileHandle );

		const SizeT fileSize = assetDb->GetFileSize( fileHandle );
		Assert( fileSize > 0 );

		assetData.SetSize( fileSize );

		assetDb->ReadFile( fileHandle, 0, assetData.ToPtr(), fileSize );


		// data must start at aligned address
		Pak_SeekAlignedOffset( pakFileWriter );

		PakFileEntry & entry = packageFile.m_entries[ iAsset ];
		entry.offset = pakFileWriter.Tell() / PAK_BLOCK_ALIGNMENT;
		entry.uncompressedSize = fileSize;

		pakFileWriter.Write( assetData.ToPtr(), assetData.GetDataSize() );

		totalPakFileSize += entry.uncompressedSize;
	}

	// Update the package header.

	packageFile.m_header.totalSize = totalPakFileSize;
	packageFile.m_header.crc32 = -1;
	packageFile.m_header.md5 = -1;

	pakFileWriter << packageFile.m_header;
	

	// Write the table of contents.

	pakFileWriter.Seek( tocOffset );
	pakFileWriter << packageFile.m_entries;


	DEVOUT("Saved package to file '%s' (%u entries)\n",destFilePath,numAssets);


	// Save package metadata.


	return true;
}


bool EdPakFileBuilder::Build_Hashed_Pak_File(
	const char* destFilePath
	)
{
	TList< ObjectGUID >	loadedAssets;
	gCore.resources->GetLoadedResources( loadedAssets );

	const UINT numAssets = loadedAssets.Num();
	CHK_VRET_FALSE_IF_NOT(numAssets > 0);


	FileWriter	pakFileWriter( destFilePath );
	CHK_VRET_FALSE_IF_NOT(pakFileWriter.IsOpen());


	AContentDatabase* assetDb = gCore.resources->GetContentDatabase();

	// could get rid of it by using redirecting stream
	MemoryBlob	assetData(EMemHeap::HeapTemp);
	assetData.Reserve(32*mxMEGABYTE);


	HashedPakFile	packageFile;

	// Reserve space for the header.

	pakFileWriter << packageFile.m_header;


	// Reserve space for the table of contents.

	const FilePosition tocOffset = pakFileWriter.Tell();
	//packageFile.m_entries.Reserve( numAssets );
	for( UINT iAsset = 0; iAsset < numAssets; iAsset++ )
	{
		PakFileEntry	dummy;
		ZERO_OUT( dummy );
		packageFile.m_entries.Set( loadedAssets[iAsset], dummy );
	}
	pakFileWriter << packageFile.m_entries;


	// Serialize file data.

	SizeT totalPakFileSize = 0;


	for( UINT iAsset = 0; iAsset < numAssets; iAsset++ )
	{
		const ObjectGUID assetGuid = loadedAssets[iAsset];
		Assert( assetGuid.IsValid() );
		if( assetGuid.IsNull() ) {
			continue;
		}

		PakFileHandle fileHandle = assetDb->OpenFile( assetGuid );
		Assert( fileHandle != BadPakFileHandle );

		const SizeT fileSize = assetDb->GetFileSize( fileHandle );
		Assert( fileSize > 0 );

		assetData.SetSize( fileSize );

		assetDb->ReadFile( fileHandle, 0, assetData.ToPtr(), fileSize );


		// data must start at aligned address
		Pak_SeekAlignedOffset( pakFileWriter );

		PakFileEntry * entry = packageFile.m_entries.Find( assetGuid );
		Assert(entry);
		if( !entry ) {
			continue;
		}
		entry->offset = pakFileWriter.Tell() / PAK_BLOCK_ALIGNMENT;
		entry->uncompressedSize = fileSize;

		pakFileWriter.Write( assetData.ToPtr(), assetData.GetDataSize() );

		totalPakFileSize += entry->uncompressedSize;

		char	tmp[32];
		assetGuid.ToChars(tmp);
		DBGOUT( "GUID[%u] = %s\n", iAsset, tmp );
	}

	// Update the package header.

	packageFile.m_header.totalSize = totalPakFileSize;
	packageFile.m_header.crc32 = -1;
	packageFile.m_header.md5 = -1;

	pakFileWriter << packageFile.m_header;
	

	// Write the table of contents.

	pakFileWriter.Seek( tocOffset );
	pakFileWriter << packageFile.m_entries;


	DEVOUT("Saved package to file '%s' (%u entries)\n",destFilePath,numAssets);


	// Save package metadata.


	return true;
}

