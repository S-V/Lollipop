/*
=============================================================================
	File:	DataStream.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

/*================================
			mxDataStream
================================*/

mxDataStream::mxDataStream()
	: mAccessMode( EAccessMode::ReadAccess )
	, mAccessPattern( EAccessPattern::Sequential )
{}

mxDataStream::~mxDataStream()
{
	Assert( !IsOpen() );
}

bool mxDataStream::Open()
{
	Assert( !IsOpen() );
	return true;
}

void mxDataStream::Close()
{
	Assert( IsOpen() );
}

bool mxDataStream::IsOpen() const
{
	return false;
}

bool mxDataStream::CanWrite() const
{
	return false;
}

bool mxDataStream::CanRead() const
{
	return false;
}

bool mxDataStream::CanSeek() const
{
	return false;
}

EAccessMode mxDataStream::GetAccessMode() const
{
	return mAccessMode;
}

void mxDataStream::SetAccessMode( EAccessMode newAccessMode )
{
	Assert( !IsOpen() );
	mAccessMode = newAccessMode;
}

EAccessPattern mxDataStream::GetAccessPattern() const
{
	return mAccessPattern;
}

void mxDataStream::SetAccessPattern( EAccessPattern newAccessPattern )
{
	Assert( !IsOpen() );
	mAccessPattern = newAccessPattern;
}

void mxDataStream::Flush()
{
	Assert(IsOpen());
//	Assert(!IsMapped());
}

/**
    This method must return true if the stream supports direct memory
    access through the Map()/Unmap() methods.
*/
bool mxDataStream::CanBeMapped() const
{
	return false;
}

/**
    If the stream provides memory mapping, this method will return a pointer
    to the beginning of the stream data in memory. The application is 
    free to read and write to the stream through direct memory access. Special
    care must be taken to not read or write past the end of the mapped data
    (indicated by GetSize()). The normal Read()/Write() method are not
    valid while the stream is mapped, also the read/write cursor position
    will not be updated.
*/
void * mxDataStream::Map()
{
    Assert(IsOpen());
    Assert(CanBeMapped());
    return nil;
}

/**
This will unmap a memory-mapped stream.
*/
void mxDataStream::Unmap()
{
	Assert(IsOpen());
	Assert(CanBeMapped());
}

FileTime mxDataStream::GetTimeStamp()
{
	return FileTime();
}

bool mxDataStream::IsOk() const
{
	return true;
}

void mxDataStream::OnDelete()
{
	delete this;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
