/*
=============================================================================
	File:	ZipFileSystem.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include "ZipFileSystem.h"

mxNAMESPACE_BEGIN
#if 0

/*================================
		ZipFileSystem
================================*/

ZipFileSystem::ZipFileSystem()
{
	this->isOpen = false;
}

ZipFileSystem::~ZipFileSystem()
{
	//Assert(!this->IsOpen());
	if(this->IsOpen())
	{
		this->Close();
	}
}

bool ZipFileSystem::Open( const char* strURL )
{
	Assert(!this->IsOpen());

	const bool bOk = FS_PathExists( strURL );
	Assert(bOk);

	if( bOk )
	{
		path.SetString( strURL );
		FS_FixSlashes( path );
		this->isOpen = true;
		return true;
	}

	mxWarnf("Failed to open folder '%s'\n",strURL);

	return false;
}

void ZipFileSystem::Close()
{
	Assert(this->IsOpen());

	path.Clear();

	this->isOpen = false;
}

bool ZipFileSystem::IsOpen() const
{
	return this->isOpen;
}

static mxDataStream* OpenFileStream( const OSPathName& nativePath,
									EAccessMode accessMode, EAccessPattern accessPattern )
{
	FileHandle fileHandle = FS_OpenFile(
		nativePath.ToChars(),
		accessMode,
		accessPattern
	);

	if( InvalidFileHandle != fileHandle )
	{
		ZipFileStream* newStream = new_one( ZipFileStream(
			nativePath,
			fileHandle,
			accessMode,
			accessPattern
		));

		return newStream;
	}

	return nil;
}

mxDataStream* ZipFileSystem::OpenReader( const char* filePath )
{
	Assert(this->IsOpen());

	OSPathName	nativePathName( this->path );
	nativePathName.Append( fileId.ToChars() );

	EAccessMode accessMode = EAccessMode::ReadAccess;
	EAccessPattern accessPattern = EAccessPattern::Sequential;

	return OpenFileStream( nativePathName, accessMode, accessPattern );
}

mxDataStream* ZipFileSystem::OpenWriter( const char* filePath )
{
	Assert(this->IsOpen());

	OSPathName	nativePathName( this->path );
	nativePathName.Append( fileId.ToChars() );

	EAccessMode accessMode = EAccessMode::WriteAccess;
	EAccessPattern accessPattern = EAccessPattern::Sequential;

	return OpenFileStream( nativePathName, accessMode, accessPattern );
}

/*================================
		ZipFileStream
================================*/

ZipFileStream::ZipFileStream()
	: mHandle( InvalidFileHandle )
	, mMappedData( nil )
{
}

ZipFileStream::ZipFileStream(
	const OSPathName& path,
	FileHandle fileHandle,
	EAccessMode accessMode,
	EAccessPattern accessPattern
	)
	: mHandle( fileHandle )
	, mMappedData( nil )
	, mPath( path )
{
	mAccessMode = accessMode;
	mAccessPattern = accessPattern;
}

ZipFileStream::~ZipFileStream()
{
	if( this->IsOpen() ) {
		this->Close();
	}
}

bool ZipFileStream::Open()
{
	Assert( !IsOpen() );

	const char* fileName = this->Path().ToChars();
//	Assert( FS_FileExists( fileName ) );

	mHandle = FS_OpenFile(
		fileName,
		this->GetAccessMode(),
		this->GetAccessPattern()
	);
	if( InvalidFileHandle == mHandle ) {
		return false;
	}

	return true;
}

void ZipFileStream::Close()
{
	mxDataStream::Close();
	Assert(IsOpen());
	Assert(InvalidFileHandle != mHandle);

	if( this->IsMapped() ) {
		this->Unmap();
	}

	FS_CloseFile( mHandle );
	mHandle = InvalidFileHandle;
}

bool ZipFileStream::IsOpen() const
{
	return (mHandle != InvalidFileHandle);
}

bool ZipFileStream::CanWrite() const
{
	return false;
}

bool ZipFileStream::CanRead() const
{
	return true;
}

bool ZipFileStream::CanSeek() const
{
	return true;
}

SizeT ZipFileStream::Read( void* pBuffer, SizeT numBytes )
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	Assert(numBytes > 0);
	return FS_ReadFile( mHandle, pBuffer, numBytes );
}

SizeT ZipFileStream::Write( const void* pBuffer, SizeT numBytes )
{
	Assert(!this->IsMapped());
	Assert(numBytes > 0);
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	return FS_WriteFile( mHandle, pBuffer, numBytes );
}

void ZipFileStream::Seek( FileOffset offset, ESeekOrigin origin )
{
	Assert(!IsMapped());
	Assert(IsOpen());
	Assert(InvalidFileHandle != mHandle);
	FS_SeekFile( mHandle, offset, origin );
}

SizeT ZipFileStream::GetSize() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_GetFileSize( mHandle );
}

bool ZipFileStream::AtEnd() const
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	return FS_Eof( mHandle );
}

FilePosition ZipFileStream::Tell() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_TellFilePos( mHandle );
}

void ZipFileStream::Flush()
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(0 != mHandle);
	FS_Flush( mHandle );
}

bool ZipFileStream::CanBeMapped() const
{
	return true;
}

void * ZipFileStream::Map()
{
	Assert(this->IsOpen());
	Assert(nil == mMappedData);
	mxDataStream::Map();
	SizeT size = this->GetSize();
	Assert( size > 0 );
	void * data = mxAllocX( EMemHeap::HeapStreaming, size );
	this->Seek( 0, ESeekOrigin::Begin );
	SizeT readSize = this->Read( data, size );
	Assert( readSize == size );
	(void)readSize;
	mMappedData = data;
	return mMappedData;
}

void ZipFileStream::Unmap()
{
	Assert(nil != mMappedData);
	mxDataStream::Unmap();
	mxFreeX( EMemHeap::HeapStreaming, mMappedData );
	mMappedData = nil;
}

/**
    Returns true if the stream is currently mapped.
*/
bool ZipFileStream::IsMapped() const
{
    return (mMappedData != nil);
}

FileTime ZipFileStream::GetTimeStamp()
{
	FileTime	lastWriteTime;
	FS_GetFileTimeStamp( mHandle, lastWriteTime );
	return lastWriteTime;
}
#endif
mxNAMESPACE_END

NO_EMPTY_FILE
