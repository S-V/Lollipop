/*
=============================================================================
	File:	NativeFileSystem.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/IO/IOSystem.h>
#include "NativeFileSystem.h"

mxNAMESPACE_BEGIN


/*================================
		NativeFileSystem
================================*/

mxDEFINE_CLASS(NativeFileSystem);

NativeFileSystem::NativeFileSystem()
{
	m_isOpen = false;
}

NativeFileSystem::NativeFileSystem( const char* path )
{
	m_isOpen = false;

	this->SetURI( path );

	this->Open();
}

NativeFileSystem::~NativeFileSystem()
{
	if(this->IsOpen())
	{
		this->Close();
	}
}

void NativeFileSystem::SetURI( const char* strURL )
{
	m_relativePath.SetString( strURL );
}

const char* NativeFileSystem::GetURI() const
{
	return m_relativePath.ToChars();
}

bool NativeFileSystem::Open()
{
	Assert(!this->IsOpen());

	const bool bOk = FS_PathExists( this->GetURI() );
	Assert(bOk);

	if( bOk )
	{
		m_isOpen = true;
		return true;
	}

	mxWarnf("Failed to open folder '%s'\n",this->GetURI());

	return false;
}

void NativeFileSystem::Close()
{
	Assert(this->IsOpen());

	m_isOpen = false;
}

bool NativeFileSystem::IsOpen() const
{
	return m_isOpen;
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
		mxLocalFileStream* newStream = new_one( mxLocalFileStream(
			nativePath,
			fileHandle,
			accessMode,
			accessPattern
		));

		return newStream;
	}

	return nil;
}

mxDataStream* NativeFileSystem::OpenReader( const char* filePath )
{
	Assert(this->IsOpen());

	OSPathName	nativePathName( m_relativePath.ToChars(), m_relativePath.Length() );
	nativePathName.Append( StripPackageName( filePath ) );

	EAccessMode accessMode = EAccessMode::ReadAccess;
	EAccessPattern accessPattern = EAccessPattern::Sequential;

	return OpenFileStream( nativePathName, accessMode, accessPattern );
}

mxDataStream* NativeFileSystem::OpenWriter( const char* filePath )
{
	Assert(this->IsOpen());

	OSPathName	nativePathName( _InitSlow,this->GetURI() );
	nativePathName.Append( filePath );

	EAccessMode accessMode = EAccessMode::WriteAccess;
	EAccessPattern accessPattern = EAccessPattern::Sequential;

	return OpenFileStream( nativePathName, accessMode, accessPattern );
}

void NativeFileSystem::Serialize( mxArchive& archive )
{
	Super::Serialize( archive );

	archive && m_relativePath;

	if( archive.IsReading() )
	{
		this->Open();
	}
}

#if MX_EDITOR
UINT NativeFileSystem::edNumRows() const
{
	return 0;
	//return 1;
	//return this->rootDir->edNumRows();
}
AEditable* NativeFileSystem::edItemAt( UINT index )
{
	return nil;
	//return (0 == index) ? this->rootDir : nil;
	//return this->rootDir->edItemAt(index);
}
UINT NativeFileSystem::edIndexOf( const AEditable* child ) const
{
	return INDEX_NONE;
	//return (this->rootDir == child) ? 0 : INDEX_NONE;
	//return this->rootDir->edIndexOf( child );
}
#endif

/*================================
		mxLocalFileStream
================================*/

mxLocalFileStream::mxLocalFileStream()
	: mHandle( InvalidFileHandle )
	, mMappedData( nil )
{
}

mxLocalFileStream::mxLocalFileStream(
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

mxLocalFileStream::~mxLocalFileStream()
{
	if( this->IsOpen() ) {
		this->Close();
	}
}

bool mxLocalFileStream::Open()
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

void mxLocalFileStream::Close()
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

bool mxLocalFileStream::IsOpen() const
{
	return (mHandle != InvalidFileHandle);
}

bool mxLocalFileStream::CanWrite() const
{
	return true;
}

bool mxLocalFileStream::CanRead() const
{
	return true;
}

bool mxLocalFileStream::CanSeek() const
{
	return true;
}

SizeT mxLocalFileStream::Read( void* pBuffer, SizeT numBytes )
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	Assert(numBytes > 0);
	return FS_ReadFile( mHandle, pBuffer, numBytes );
}

SizeT mxLocalFileStream::Write( const void* pBuffer, SizeT numBytes )
{
	Assert(!this->IsMapped());
	Assert(numBytes > 0);
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	return FS_WriteFile( mHandle, pBuffer, numBytes );
}

void mxLocalFileStream::Seek( FileOffset offset, ESeekOrigin origin )
{
	Assert(!IsMapped());
	Assert(IsOpen());
	Assert(InvalidFileHandle != mHandle);
	FS_SeekFile( mHandle, offset, origin );
}

SizeT mxLocalFileStream::GetSize() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_GetFileSize( mHandle );
}

bool mxLocalFileStream::AtEnd() const
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	return FS_Eof( mHandle );
}

FilePosition mxLocalFileStream::Tell() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_TellFilePos( mHandle );
}

void mxLocalFileStream::Flush()
{
	Assert(!this->IsMapped());
	Assert(this->IsOpen());
	Assert(0 != mHandle);
	FS_Flush( mHandle );
}

bool mxLocalFileStream::CanBeMapped() const
{
	return true;
}

void * mxLocalFileStream::Map()
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

void mxLocalFileStream::Unmap()
{
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != mMappedData);
	mxDataStream::Unmap();
	mxFreeX( EMemHeap::HeapStreaming, mMappedData );
	mMappedData = nil;
}

/**
    Returns true if the stream is currently mapped.
*/
bool mxLocalFileStream::IsMapped() const
{
    return (mMappedData != nil);
}

FileTime mxLocalFileStream::GetTimeStamp()
{
	Assert(InvalidFileHandle != mHandle);
	FileTime	lastWriteTime;
	FS_GetFileTimeStamp( mHandle, lastWriteTime );
	return lastWriteTime;
}

mxNAMESPACE_END
