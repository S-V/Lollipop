/*
=============================================================================
	File:	Files.h
	Desc:	
=============================================================================
*/

#ifndef __MX_FILES_H__
#define __MX_FILES_H__

mxNAMESPACE_BEGIN


typedef TStackString< FS_MAX_PATH >			OSPathName;
typedef TStackString< MAX_FILENAME_CHARS >	OSFileName;

struct mxPathName : public TStackString< FS_MAX_PATH >
{
	typedef TStackString< FS_MAX_PATH >	Super;

	inline explicit mxPathName( PCHARS pString )
		: Super( _InitSlow, pString )
	{
		this->FixSlashes();
	}
	inline mxPathName( const mxPathName& other )
		: Super( other.ToChars(), other.Length() )
	{
		this->FixSlashes();
	}

	// Converts the path to the uniform form (Unix-style).
	inline void FixSlashes()
	{
		F_NormalizePath( *this );
	}
};



enum EFileReadFlags
{
	FileRead_NoOpenError	= BIT(0),	// don't cause an error if the file could not be opened
	FileRead_NoReadError	= BIT(1),	// don't cause an error if the file has zero size

	FileRead_NoErrors		= FileRead_NoOpenError|FileRead_NoReadError,
};
typedef TBits< EFileReadFlags, U4 >	FileReadFlags;

enum EFileWriteFlags
{
	FileWrite_NoErrors	= BIT(0),	// erases existing contents
	FileWrite_Append	= BIT(1),	// appends new data to existing file
};
typedef TBits< EFileWriteFlags, U4 >	FileWriteFlags;

/*
-----------------------------------------------------------------------------
	FileReader
-----------------------------------------------------------------------------
*/
class FileReader : public AStreamReader
{
public:
	explicit FileReader( const char* fileName, FileReadFlags flags = 0 );
	~FileReader();

	bool	IsOpen() const;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	SizeT	Read( void* pBuffer, SizeT numBytes ) override;

	// Set the file pointer to the given offset (in bytes) from the beginning.
	//
	void	Seek( FileOffset offset );

	void	Skip( SizeT numBytes );

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	SizeT	GetSize() const;

	SizeT	Tell() const;

	// Returns true if the end has been reached.
	bool	AtEnd() const;

	// Returns true if the stream provides direct memory access.
	bool	CanBeMapped() const;

	// Map stream to memory.
	void *	Map();
	void	Unmap();
	bool	IsMapped() const;


	void	Close();

public_internal:

	// initializes with null values
	explicit FileReader( ENoInit );

	// initializes from the supplied file handle
	explicit FileReader( FileHandle hFile );

	FileHandle GetFileHandle() const
	{
		return mHandle;
	}

	bool Open( const char* fileName, FileReadFlags flags = 0 );

private:
	FileHandle mHandle;
	void *	mMappedData;
};

/*
-----------------------------------------------------------------------------
	FileWriter
-----------------------------------------------------------------------------
*/
class FileWriter : public AStreamWriter
{
public:
	explicit FileWriter( const char* fileName, FileWriteFlags flags = 0 );
	~FileWriter();

	bool	IsOpen() const;
	void	Close();

	virtual SizeT Write( const void* pBuffer, SizeT numBytes ) override;


	SizeT	Tell() const;

	// Set the file pointer to the given offset (in bytes) from the beginning.
	//
	void	Seek( FileOffset offset );

public_internal:
	FileWriter( FileHandle hFile );

	FileHandle GetFileHandle() const
	{
		return mHandle;
	}

private:
	FileHandle mHandle;
	//const char* mFileName;
};

/*
-----------------------------------------------------------------------------
	mxFileStream

	A data stream which offers read/write access to files.
-----------------------------------------------------------------------------
*/
class mxFileStream : public AStreamReader, public AStreamWriter
{
public:
	mxFileStream();

	mxFileStream(
		const char* filePath,
		EAccessMode accessMode,
		EAccessPattern accessPattern = EAccessPattern::Sequential
	);

	mxFileStream( FileHandle fileHandle );

	virtual	~mxFileStream();

public:

	bool Open(
		const char* filePath,
		EAccessMode accessMode,
		EAccessPattern accessPattern
	);

	void	Close();

	bool	IsOpen() const;


	// Return the size of data (length of the file if this is a file), in bytes.
	//
	virtual	SizeT	GetSize() const override;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	virtual	SizeT	Read( void* pBuffer, SizeT numBytes ) override;

	// Write to the stream and return the number of bytes written.
	//
	virtual	SizeT	Write( const void* pBuffer, SizeT numBytes ) override;

	// Set the file pointer to the given offset from the beginning.
	//
	void	Seek( FileOffset offset, ESeekOrigin origin = ESeekOrigin::Begin );

	void Skip( SizeT bytes );


	// Returns the current byte offset from beginning.
	//
	FilePosition Tell() const;

	// Returns true if EOF has been reached.
	bool	AtEnd() const;

	// Flush unsaved data (this function is called automatically when the stream is closed).
	void	Flush();

	// Return a time value for reload operations.
	FileTime GetTimeStamp();

public:
	// seeks to the beginning
	void Rewind();

private:
	FileHandle		mHandle;
};



inline
bool Util_LoadFileToMemory(AStreamReader& inFile,
						   MemoryBlob &outFileData)
{
	const SizeT fileSize = inFile.GetSize();
	Assert(fileSize > 0);
	if( fileSize  == 0 )
	{
		DBGOUT("File '%s' has zero size.\n",inFile.DbgGetName());
		return false;
	}

	outFileData.Reserve(fileSize);
	inFile.Read(outFileData.ToPtr(),fileSize);

	return true;
}

inline
bool Util_LoadFileToMemory(PCHARS inFilePath,
						   MemoryBlob &outFileData)
{
	FileReader	file( inFilePath );
	CHK_VRET_FALSE_IF_NOT( file.IsOpen() );

	return Util_LoadFileToMemory( file, outFileData );
}

inline
bool Util_SaveBlobToFile(PCHARS fileName,
					const MemoryBlob& data)
{
	FileWriter	file( fileName );
	CHK_VRET_FALSE_IF_NOT( file.IsOpen() );

	file.Write( data.ToPtr(), data.GetDataSize() );

	return true;
}

mxNAMESPACE_END

#endif // !__MX_FILES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
