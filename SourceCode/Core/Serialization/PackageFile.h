#pragma once

#include <Core/Resources.h>


// Structure defining the header of our resource files.
struct PakFileHeader
{
#pragma pack (push,1)
	mxSessionInfo	session;

	FileTime	whenCreated;
	FileTime	lastModified;

	U4		totalSize;	// Total size of the PAK file
	U4		crc32;	// CRC32 checksum
	U4		md5;	// MD5 checksum
#pragma pack (pop)

public:
	explicit PakFileHeader( U4 fourCC = MAKEFOURCC('R','P','K','0') );

	bool Matches( const PakFileHeader& other ) const;
};
mxDECLARE_POD_TYPE(PakFileHeader);



// this structure is used for locating individual resource file within a resource package
#pragma pack (push,1)
struct PakFileEntry
{
	U4	offset;	// Position of the entry relative to the beginning of the package file, in multiples of PAK_BLOCK_ALIGNMENT
	U4	uncompressedSize;// Size of the entry in PACK file
};
#pragma pack (pop)
mxDECLARE_POD_TYPE(PakFileEntry);


/*
-----------------------------------------------------------------------------
	OptimizedPakFile

	this is read-only,
	release exes only contain code to read archives.

-----------------------------------------------------------------------------
*/
class OptimizedPakFile : public AFilePackage
{
	TList< PakFileEntry >		m_entries;//+persistent

	FileReader		m_fileReader;

	PakFileHeader	m_header;//+persistent

public:

	OptimizedPakFile();

	// opens the pack file
	OptimizedPakFile( const char* fileName );

	// closes the pack file
	~OptimizedPakFile();

	bool IsOpen() const;

	void Close();

	//=-- AFilePackage

	// fast access to file by file handle;
	// returns BadPakFileHandle if not found
	//
	virtual PakFileHandle OpenFile( ObjectGUIDArg fileGuid ) override;

	virtual void CloseFile( PakFileHandle file ) override;

	// Summary:
	//   Get the file size in bytes (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual UINT GetFileSize( PakFileHandle file ) override;

	// Reads the file into the preallocated buffer
	virtual SizeT ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead ) override;

private:
	friend class EdPakFileBuilder;
};



/*
-----------------------------------------------------------------------------
	HashedPakFile

	this is read-only,
	release exes only contain code to read archives.
-----------------------------------------------------------------------------
*/
class HashedPakFile : public AContentDatabase
{
	TMap< ObjectGUID, PakFileEntry >	m_entries;//+persistent

	FileReader		m_fileReader;

	PakFileHeader	m_header;//+persistent

public:

	HashedPakFile();

	// opens the pack file
	HashedPakFile( const char* fileName );

	// closes the pack file
	~HashedPakFile();

	bool Open( const char* fileName );

	bool IsOpen() const;

	void Close();

	//=-- AFilePackage

	// fast access to file by file handle;
	// returns BadPakFileHandle if not found
	//
	virtual PakFileHandle OpenFile( ObjectGUIDArg fileGuid ) override;

	virtual void CloseFile( PakFileHandle file ) override;

	// Summary:
	//   Get the file size in bytes (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual UINT GetFileSize( PakFileHandle file ) override;

	// Reads the file into the preallocated buffer
	virtual SizeT ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead ) override;

private:
	const PakFileEntry* FindEntryByIndex( PakFileHandle index ) const;

private:
	friend class EdPakFileBuilder;
};



// Each file must be aligned to a start on a disk sector, typically 2048 bytes on a DVD
enum { PAK_BLOCK_ALIGNMENT = 0x800 };// 2048 bytes


FORCEINLINE SizeT Pak_CalcAlignedSize( SizeT unaligned )
{
	SizeT alignedSize = 0;
	alignedSize = (unaligned + (PAK_BLOCK_ALIGNMENT-1)) & (~(PAK_BLOCK_ALIGNMENT-1));
	return alignedSize;
}

template< class STREAM >
inline
void Pak_SeekAlignedOffset( STREAM & file )
{
	const SizeT currOffset = file.Tell();
	const SizeT alignedOffset = Pak_CalcAlignedSize( currOffset );

	file.Seek( alignedOffset );
}

FORCEINLINE U4 Pak_GetFileEntryOffset( const PakFileEntry& fileInfo )
{
	return fileInfo.offset * PAK_BLOCK_ALIGNMENT;
}


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
