/*
=============================================================================
	File:	InPlaceMemoryReader.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include "InPlaceMemoryStream.h"

mxNAMESPACE_BEGIN

/*================================
		InPlaceMemoryReader
================================*/

InPlaceMemoryReader::InPlaceMemoryReader( const void* data, SizeT dataSize )
	: mData( c_cast(const BYTE*)data ), mDataSize( dataSize ), mReadOffset( 0 )
{
	this->DbgSetName( "InPlaceMemoryReader" );
	AssertPtr(data);
	Assert(dataSize > 0);
}

InPlaceMemoryReader::~InPlaceMemoryReader()
{
	//const UINT bytesRead = c_cast(UINT) mReadOffset;
	//DBGOUT("~InPlaceMemoryReader: %u bytes read.\n",bytesRead);
}

SizeT InPlaceMemoryReader::GetSize() const
{
	return mDataSize;
}

SizeT InPlaceMemoryReader::Read( void *pDest, SizeT numBytes )
{
	Assert(mReadOffset < mDataSize + numBytes);
	SizeT bytesToRead = Min<SizeT>( mDataSize - mReadOffset, numBytes );
	MemCopy( pDest, mData + mReadOffset, bytesToRead );
	mReadOffset += bytesToRead;
	return bytesToRead;
}

/*================================
		InPlaceMemoryWriter
================================*/

InPlaceMemoryWriter::InPlaceMemoryWriter( void *dstBuf, SizeT bufSize )
	: mBuffer( c_cast(BYTE*)dstBuf ), mCurrPos( mBuffer ), mMaxSize( bufSize )
{
	this->DbgSetName( "InPlaceMemoryWriter" );
	AssertPtr(dstBuf);
	Assert(bufSize > 0);
}

InPlaceMemoryWriter::~InPlaceMemoryWriter()
{
	//const UINT bytesWritten = c_cast(UINT) this->BytesWritten();
	//DBGOUT("~InPlaceMemoryWriter: %u bytes written.\n",bytesWritten);
}

SizeT InPlaceMemoryWriter::Write( const void *pSrc, SizeT numBytes )
{
	const SizeT writtenSoFar = this->BytesWritten();
	Assert(writtenSoFar + numBytes < mMaxSize );
	SizeT bytesToWrite = Min<SizeT>( mMaxSize - writtenSoFar, numBytes );
	MemCopy( mCurrPos, pSrc, bytesToWrite );
	mCurrPos += bytesToWrite;
	return bytesToWrite;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
