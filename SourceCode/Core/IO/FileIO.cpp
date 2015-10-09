/*
=============================================================================
	File:	FileIO.cpp
	Desc:	
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#if 0

#include <physfs-2.0.2/physfs.h>

MX_NAMESPACE_BEGIN

#define PHYSFS_FILE_HANDLE(x)		cast(PHYSFS_File*)(x)

/*================================
			FileReader
================================*/

FileReader::FileReader( const char* fileName, FileReadFlags flags )
	: mHandle( nil ), mMappedData( nil )
{
	mHandle = PHYSFS_openRead( fileName );
	if( !mHandle && !(flags & FileRead_NoErrors) ) {
		mxErrf( "Failed to open file '%s' for reading\n", fileName );
	}
}

FileReader::~FileReader()
{
	if( IsOpen() ) {
		Close();
	}
}

bool FileReader::IsOpen() const
{
	return (mHandle != nil);
}

SizeT FileReader::Read( void* pBuffer, SizeT numBytes )
{
	SizeT read = PHYSFS_read( PHYSFS_FILE_HANDLE(mHandle), pBuffer, 1, (PHYSFS_uint32)numBytes );
	Assert(read > 0);
	return read;
}

void FileReader::Close()
{
	if( IsMapped() ) {
		Unmap();
	}
	if( mHandle ) {
		PHYSFS_close( PHYSFS_FILE_HANDLE(mHandle) );
	}
	mHandle = nil;
}

void FileReader::Seek( FileOffset offset )
{
	int ret = PHYSFS_seek( PHYSFS_FILE_HANDLE(mHandle), offset );
	Assert_NZ(ret);
}

void FileReader::Skip( long bytes )
{
	SizeT newPos = Tell() + bytes;
	Seek( newPos );
}

SizeT FileReader::GetSize() const
{
	return PHYSFS_fileLength( PHYSFS_FILE_HANDLE(mHandle) );
}

SizeT FileReader::Tell() const
{
	return PHYSFS_tell( PHYSFS_FILE_HANDLE(mHandle) );
}

bool FileReader::AtEnd() const
{
	return PHYSFS_eof( PHYSFS_FILE_HANDLE(mHandle) ) != 0;
}

bool FileReader::CanBeMapped() const
{
	return true;
}

void * FileReader::Map()
{
	Assert(nil == mMappedData);
	SizeT size = GetSize();
	Assert( size > 0 );
	void * data = mxAllocX( EMemHeap::HeapStreaming, size );
	Seek( 0 );
	SizeT readSize = Read( data, size );
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
	mHandle = PHYSFS_openWrite( fileName );
	if( !mHandle && !(flags & FileWrite_NoErrors) ) {
		mxErrf( "Failed to open file '%s' for writing\n", fileName );
	}
}

FileWriter::~FileWriter()
{
	if( IsOpen() ) {
		Close();
	}
}

bool FileWriter::IsOpen() const
{
	return (mHandle != nil);
}

void FileWriter::Close()
{
	if( mHandle ) {
		PHYSFS_close( PHYSFS_FILE_HANDLE(mHandle) );
	}
	mHandle = nil;
}

SizeT FileWriter::Write( const void* pBuffer, SizeT numBytes )
{
	SizeT read = PHYSFS_write( PHYSFS_FILE_HANDLE(mHandle), pBuffer, 1, (PHYSFS_uint32)numBytes );
	Assert(read > 0);
	return read;
}

#endif

MX_NAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
