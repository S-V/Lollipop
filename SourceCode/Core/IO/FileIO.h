/*
=============================================================================
	File:	FileIO.h
	Desc:	
=============================================================================
*/

#ifndef __MX_FILE_IO_H__
#define __MX_FILE_IO_H__

MX_NAMESPACE_BEGIN

typedef String		mxPathName;
typedef String		mxFileName;
typedef String		mxStreamId;

typedef TFixedString< MAX_PATH_CHARS >		OSPathName;
typedef TFixedString< MAX_FILENAME_CHARS >	OSFileName;

enum EArchiveType
{
	Archive_Unknown = 0,
	Archive_Folder = 0,
	Archive_ZIP = 0,
	Archive_TAR = 0,
};

enum EFileReadFlags
{
	FileRead_NoErrors	= BIT(0),
};
typedef TBits< EFileReadFlags, U32 >	FileReadFlags;

enum EFileWriteFlags
{
	FileWrite_NoErrors= BIT(0),
};
typedef TBits< EFileWriteFlags, U32 >	FileWriteFlags;

/*
-----------------------------------------------------------------------------
	FileReader
-----------------------------------------------------------------------------
*/
class FileReader : public mxStreamReader
{
public:
	FileReader( const char* fileName, FileReadFlags flags = 0 );
	~FileReader();

	bool	IsOpen() const;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	OVERRIDEN SizeT Read( void* pBuffer, SizeT numBytes );

	// Set the file pointer to the given offset (in bytes) from the beginning.
	//
	void	Seek( FileOffset offset );


	void	Skip( long bytes );


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

private:
	void *	mHandle;
	void *	mMappedData;
};

/*
-----------------------------------------------------------------------------
	FileWriter
-----------------------------------------------------------------------------
*/
class FileWriter : public mxStreamWriter
{
public:
	FileWriter( const char* fileName, FileWriteFlags flags = 0 );
	~FileWriter();

	bool	IsOpen() const;
	void	Close();

	OVERRIDEN SizeT Write( const void* pBuffer, SizeT numBytes );

private:
	void* mHandle;
};

MX_NAMESPACE_END

#endif // !__MX_FILE_IO_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
