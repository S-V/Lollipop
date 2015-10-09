/*
=============================================================================
	File:	ZipFileSystem.h
	Desc:	An archive filesystem wrapper for ZIP files. 

	Uses the zlib and the minizip package under the hood.

	Limitations:
	* No write access.
	* No seek on compressed data, the ZipFileSystem will generally decompress 
	an entire file into memory at once, so that the ZipStreamClass can
	provide random access on the decompressed data. Thus the typical 
	"audio streaming scenario" is not possible from zip files (that's
	what XACT's sound banks is there for anyway ;)

	How to fix the no-seek problem:
	* zlib processes datas in chunks, and cannot seek randomly within 
	a chunk, and the chunk size is dependent on the compress application
	being used to create the zip file(?), if those internals are known,
	it would be possible to write a chunked filesystem which keeps
	buffered chunks around for each client, probably not worth the effort.
	* Another appoach would be to split stream-files into "chunk-files"
	before compressing, and to read the next complete chunk files
	when new data is needed. This approach doesn't require changes to 
	the strip filesystem.

	(C) 2006 Radon Labs GmbH
=============================================================================
*/

#include <Core/IO/FileArchive.h>

mxNAMESPACE_BEGIN

#if 0
/*
-----------------------------------------------------------------------------
	ZipFileSystem

	represents a folder on a local drive.
	it's very slow compared to binary packages.
-----------------------------------------------------------------------------
*/
class ZipFileSystem : public FileArchive
{
public:
	ZipFileSystem();
	virtual ~ZipFileSystem();

	virtual bool Open( const char* strURL );
	virtual void Close();

	virtual bool IsOpen() const;

	virtual mxDataStream* OpenReader( const char* filePath );
	virtual mxDataStream* OpenWriter( const char* filePath );

	const OSPathName& Path() const { return this->path; }

private:PREVENT_COPY(ZipFileSystem);

	OSPathName	path;	// location of the zip archive file
	bool		isOpen;
};

/*
-----------------------------------------------------------------------------
	ZipFileStream

	Wraps a file in a zip archive into a stream. Allows random access
	to the file by caching the entire file contents into RAM (the
	zip file system doesn't allow seeking in files). Also note that
	ZipFileStreams are read-only.

	(C) 2006 Radon Labs GmbH
-----------------------------------------------------------------------------
*/
class ZipFileStream : public mxDataStream
{
public:
	OVERRIDES(mxDataStream)	bool	Open();
	OVERRIDES(mxDataStream)	void	Close();

	OVERRIDES(mxDataStream)	bool	IsOpen() const;
	OVERRIDES(mxDataStream)	bool	CanWrite() const;
	OVERRIDES(mxDataStream)	bool	CanRead() const;
	OVERRIDES(mxDataStream)	bool	CanSeek() const;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	OVERRIDES(mxDataStream)	SizeT	Read( void* pBuffer, SizeT numBytes );

	// Write to the stream and return the number of bytes written.
	//
	OVERRIDES(mxDataStream)	SizeT	Write( const void* pBuffer, SizeT numBytes );

	// Set the file pointer to the given offset from the beginning.
	//
	OVERRIDES(mxDataStream)	void	Seek( FileOffset offset, ESeekOrigin origin = ESeekOrigin::Begin );

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	OVERRIDES(mxDataStream)	SizeT	GetSize() const;

	// Returns true if EOF has been reached.
	OVERRIDES(mxDataStream)	bool	AtEnd() const;

	// Flush unsaved data (this function is called automatically when the stream is closed).
	OVERRIDES(mxDataStream)	void	Flush();

	// Returns true if the stream provides direct memory access.
	OVERRIDES(mxDataStream)	bool	CanBeMapped() const;
	
	// Map stream to memory.
	OVERRIDES(mxDataStream)	void *	Map();
	OVERRIDES(mxDataStream)	void	Unmap();

	// Return a time value for reload operations.
	OVERRIDES(mxDataStream)	FileTime GetTimeStamp();

	//
	//	mxFileStream interface :
	//

	// Returns the current byte offset from beginning.
	//
	FilePosition	Tell() const;

	bool	IsMapped() const;

public_internal:

	const OSPathName& Path() const { return mPath; }

	ZipFileStream();

	ZipFileStream(
		const OSPathName& path,
		FileHandle fileHandle,
		EAccessMode accessMode,
		EAccessPattern accessPattern
	);

	OVERRIDES(mxDataStream)	~ZipFileStream();

private:
	SizeT		mSize;	// size of this file

	FileHandle		mHandle;
	void *			mMappedData;
	OSPathName		mPath;
};
#endif
mxNAMESPACE_END
