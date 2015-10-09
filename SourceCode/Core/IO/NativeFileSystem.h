/*
=============================================================================
	File:	NativeFileSystem.h
	Desc:	
=============================================================================
*/
#pragma once

#include <Core/IO/FileArchive.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	NativeFileSystem

	represents a folder on a local drive
	(a folder, directory, catalog, or drawer).
	it's very slow compared to binary packages.
-----------------------------------------------------------------------------
*/
class NativeFileSystem : public FileArchive
{
public:
	mxDECLARE_CLASS(NativeFileSystem,FileArchive);

	NativeFileSystem();
	NativeFileSystem( const char* path );
	virtual ~NativeFileSystem();

	void SetURI( const char* strURL );
	virtual const char* GetURI() const;

	virtual bool Open();
	virtual void Close();

	virtual bool IsOpen() const;

	// very slow; involves creating a stream object
	virtual mxDataStream* OpenReader( const char* filePath );
	virtual mxDataStream* OpenWriter( const char* filePath );

	virtual void Serialize( mxArchive& archive );

public:
	UINT edNumRows() const;
	AEditable* edItemAt( UINT index );
	UINT edIndexOf( const AEditable* child ) const;

private:

	StackString		m_relativePath;

	bool		m_isOpen;
};

/*
-----------------------------------------------------------------------------
	mxLocalFileStream

	A data stream which offers read/write access to files.
-----------------------------------------------------------------------------
*/
class mxLocalFileStream : public mxDataStream
{
public:
	virtual	bool	Open() override;
	virtual	void	Close() override;

	virtual	bool	IsOpen() const override;
	virtual	bool	CanWrite() const override;
	virtual	bool	CanRead() const override;
	virtual	bool	CanSeek() const override;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	virtual	SizeT	Read( void* pBuffer, SizeT numBytes ) override;

	// Write to the stream and return the number of bytes written.
	//
	virtual	SizeT	Write( const void* pBuffer, SizeT numBytes ) override;

	// Set the file pointer to the given offset from the beginning.
	//
	virtual	void	Seek( FileOffset offset, ESeekOrigin origin = ESeekOrigin::Begin ) override;

	// Return the size of data (length of the file if this is a file), in bytes.
	//
	virtual	SizeT	GetSize() const override;

	// Returns true if EOF has been reached.
	virtual	bool	AtEnd() const override;

	// Flush unsaved data (this function is called automatically when the stream is closed).
	virtual	void	Flush() override;

	// Returns true if the stream provides direct memory access.
	virtual	bool	CanBeMapped() const override;
	
	// Map stream to memory.
	virtual	void *	Map() override;
	virtual	void	Unmap() override;

	// Return a time value for reload operations.
	virtual	FileTime GetTimeStamp();

	//
	//	mxFileStream interface :
	//

	// Returns the current byte offset from beginning.
	//
	FilePosition	Tell() const;

	bool	IsMapped() const;

public_internal:

	const OSPathName& Path() const { return mPath; }

	mxLocalFileStream();

	mxLocalFileStream(
		const OSPathName& path,
		FileHandle fileHandle,
		EAccessMode accessMode,
		EAccessPattern accessPattern
	);

	virtual	~mxLocalFileStream();

private:
	FileHandle		mHandle;
	void *			mMappedData;
	OSPathName		mPath;
};

/*
-----------------------------------------------------------------------------
	mxLocalFileResource

	A data stream which offers read/write access to files.
-----------------------------------------------------------------------------
*/
class mxPermanentLocalFile : public mxLocalFileStream
{
public:
	mxPermanentLocalFile();

	virtual void Destroy()
	{
		// Nothing.
	}
};



mxNAMESPACE_END
