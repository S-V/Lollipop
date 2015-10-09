/*
=============================================================================
	File:	Files.cpp
	Desc:	Files.
=============================================================================
*/


#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

static const char* UNKNOWN_FILE_NAME = "?";

/*================================
			FileReader
================================*/

FileReader::FileReader( const char* fileName, FileReadFlags flags )
	: mHandle( nil ), mMappedData( nil )
{
	this->Open( fileName, flags );
}

bool FileReader::Open( const char* fileName, FileReadFlags flags )
{
	AssertPtr( fileName );
	this->DbgSetName( fileName );

	mHandle = FS_OpenFile( fileName, EAccessMode::ReadAccess );

	if( !FS_IsValid(mHandle) && !(flags & FileRead_NoOpenError) ) {
		mxErrf( "Failed to open file '%s' for reading\n", fileName );
		return false;
	}

	if( !(flags & FileRead_NoReadError) )
	{
		if( this->IsOpen() && FS_GetFileSize(mHandle) == 0 ) {
			mxErrf( "Failed to open file '%s' of zero size for reading\n", fileName );
			this->Close();
			return false;
		}
	}

	if( FS_IsValid(mHandle) ) {
		DBGOUT("Opened file '%s' (%u bytes) for reading\n",fileName,(UINT)this->GetSize());
		return true;
	} else {
		DBGOUT( "Couldn't open file '%s' for reading\n", fileName );\
		return false;
	}

	return false;
}

FileReader::FileReader( ENoInit )
{
	mHandle = InvalidFileHandle;
	mMappedData = nil;
}

FileReader::FileReader( FileHandle hFile )
{
	Assert( hFile != InvalidFileHandle );
	mHandle = hFile;
	mMappedData = nil;
}

FileReader::~FileReader()
{
	if( this->IsOpen() ) {
		this->Close();
	}
}

bool FileReader::IsOpen() const
{
	return FS_IsValid(mHandle);
}

SizeT FileReader::Read( void* pBuffer, SizeT numBytes )
{
	AssertPtr(pBuffer);
	const SizeT read = FS_ReadFile( mHandle, pBuffer, numBytes );
	Assert(read > 0);
	return read;
}

void FileReader::Close()
{
	if( this->IsMapped() ) {
		this->Unmap();
	}
	if( FS_IsValid(mHandle) ) {
		FS_CloseFile( mHandle );
	}
	mHandle = InvalidFileHandle;
	//DBGOUT("Closed real-only file '%s'\n",mFileName);
}

void FileReader::Seek( FileOffset offset )
{
	Assert(this->IsOpen());
	FS_SeekFile( mHandle, offset, ESeekOrigin::Begin );
}

void FileReader::Skip( SizeT bytes )
{
	Assert(this->IsOpen());
	SizeT newPos = this->Tell() + bytes;
	this->Seek( newPos );
}

SizeT FileReader::GetSize() const
{
	Assert(this->IsOpen());

	const SizeT fileSize = FS_GetFileSize( mHandle );
	Assert( fileSize != INVALID_FILE_SIZE );

	return fileSize;
}

SizeT FileReader::Tell() const
{
	Assert(this->IsOpen());
	return FS_TellFilePos( mHandle );
}

bool FileReader::AtEnd() const
{
	Assert(this->IsOpen());
	return FS_Eof( mHandle ) != 0;
}

bool FileReader::CanBeMapped() const
{
	return true;
}

void * FileReader::Map()
{
	Assert(nil == mMappedData);
	SizeT size = this->GetSize();
	Assert( size > 0 );
	void * data = mxAllocX( EMemHeap::HeapStreaming, size );
	Seek( 0 );
	SizeT readSize = this->Read( data, size );
	Assert( readSize == size );
	(void)readSize;
	mMappedData = data;
	return mMappedData;
}

void FileReader::Unmap()
{
	Assert(nil != mMappedData);
	mxFreeX( EMemHeap::HeapStreaming, mMappedData );
	mMappedData = nil;
}

bool FileReader::IsMapped() const
{
	return (mMappedData != nil);
}

/*================================
			FileWriter
================================*/

FileWriter::FileWriter( const char* fileName, FileWriteFlags flags )
{
	AssertPtr( fileName );
	this->DbgSetName( fileName );

	EAccessMode	accessMode = EAccessMode::WriteAccess;

	if( flags & FileWrite_Append ) {
		accessMode = EAccessMode::AppendAccess;
	}

	mHandle = FS_OpenFile( fileName, accessMode );

	if( !FS_IsValid(mHandle) && !(flags & FileWrite_NoErrors) ) {
		mxErrf( "Failed to open file '%s' for writing\n", fileName );
		return;
	}

	if( FS_IsValid(mHandle) ) {
		DBGOUT("Opened file '%s' for writing\n",fileName);
	} else {
		DBGOUT( "Couldn't open file '%s' for writing\n", fileName );
	}
	//mFileName = fileName;
}

FileWriter::FileWriter( FileHandle hFile )
{
	Assert( hFile != InvalidFileHandle );
	mHandle = hFile;
	//mFileName = UNKNOWN_FILE_NAME;
}

FileWriter::~FileWriter()
{
	if( this->IsOpen() ) {
		this->Close();
	}
}

bool FileWriter::IsOpen() const
{
	return (mHandle != InvalidFileHandle);
}

void FileWriter::Close()
{
	if( mHandle != InvalidFileHandle ) {
		FS_CloseFile( mHandle );
	}
	mHandle = InvalidFileHandle;
	//DBGOUT("Closed write-only file '%s'\n",mFileName);
}

SizeT FileWriter::Write( const void* pBuffer, SizeT numBytes )
{
	AssertPtr(pBuffer);
	Assert(mHandle != InvalidFileHandle);
	const SizeT written = FS_WriteFile( mHandle, pBuffer, numBytes );
	Assert(written > 0);
	return written;
}

SizeT FileWriter::Tell() const
{
	Assert(this->IsOpen());
	return FS_TellFilePos( mHandle );
}

void FileWriter::Seek( FileOffset offset )
{
	Assert(this->IsOpen());
	FS_SeekFile( mHandle, offset, ESeekOrigin::Begin );
}

/*================================
		mxFileStream
================================*/

mxFileStream::mxFileStream()
	: mHandle( InvalidFileHandle )
{
}

mxFileStream::mxFileStream(
	const char* filePath,
	EAccessMode accessMode,
	EAccessPattern accessPattern
	)
{
	this->Open( filePath, accessMode, accessPattern );
}

mxFileStream::mxFileStream( FileHandle fileHandle )
{
	Assert( fileHandle != InvalidFileHandle );
	mHandle = fileHandle;
}

mxFileStream::~mxFileStream()
{
	if( this->IsOpen() ) {
		this->Close();
	}
}

bool mxFileStream::Open(
	const char* filePath,
	EAccessMode accessMode,
	EAccessPattern accessPattern
	)
{
	Assert( !this->IsOpen() );

	mHandle = FS_OpenFile(
		filePath,
		accessMode,
		accessPattern
	);
	if( InvalidFileHandle == mHandle ) {
		return false;
	}

	DEVOUT("Opened file '%s' (%s)\n",filePath,mxFileAccessModeToStr(accessMode));

	return true;
}

void mxFileStream::Close()
{
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);

	FS_CloseFile( mHandle );
	mHandle = InvalidFileHandle;
}

bool mxFileStream::IsOpen() const
{
	return (mHandle != InvalidFileHandle);
}

SizeT mxFileStream::Read( void* pBuffer, SizeT numBytes )
{
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	Assert(numBytes > 0);
	return FS_ReadFile( mHandle, pBuffer, numBytes );
}

SizeT mxFileStream::Write( const void* pBuffer, SizeT numBytes )
{
	Assert(numBytes > 0);
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	Assert(nil != pBuffer);
	return FS_WriteFile( mHandle, pBuffer, numBytes );
}

void mxFileStream::Seek( FileOffset offset, ESeekOrigin origin )
{
	Assert(IsOpen());
	Assert(InvalidFileHandle != mHandle);
	FS_SeekFile( mHandle, offset, origin );
}

SizeT mxFileStream::GetSize() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_GetFileSize( mHandle );
}

bool mxFileStream::AtEnd() const
{
	Assert(this->IsOpen());
	Assert(InvalidFileHandle != mHandle);
	return FS_Eof( mHandle );
}

FilePosition mxFileStream::Tell() const
{
	Assert(InvalidFileHandle != mHandle);
	return FS_TellFilePos( mHandle );
}

void mxFileStream::Flush()
{
	Assert(this->IsOpen());
	Assert(0 != mHandle);
	FS_Flush( mHandle );
}

FileTime mxFileStream::GetTimeStamp()
{
	Assert(InvalidFileHandle != mHandle);
	FileTime	lastWriteTime;
	FS_GetFileTimeStamp( mHandle, lastWriteTime );
	return lastWriteTime;
}

void mxFileStream::Rewind()
{
	Assert(InvalidFileHandle != mHandle);
	this->Seek( 0 );
}

void mxFileStream::Skip( SizeT bytes )
{
	Assert(InvalidFileHandle != mHandle);
	this->Seek( bytes, ESeekOrigin::Current );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
