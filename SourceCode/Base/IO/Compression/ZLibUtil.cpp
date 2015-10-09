/*
=============================================================================
	File:	ZLibUtil.cpp
	Desc:	ZLibUtil.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

// zlib compression library
#include <zlib/zlib.h>

#include "ZLibUtil.h"

mxNAMESPACE_BEGIN

int MapZLibCompressionMode( CompressionMode mode )
{
	int	compressLevel = Z_NO_COMPRESSION;
	switch( mode )
	{
	case COMPRESS_NORMAL:	compressLevel = Z_DEFAULT_COMPRESSION;	break;
	case COMPRESS_FAST:		compressLevel = Z_BEST_SPEED;			break;
	case COMPRESS_BEST:		compressLevel = Z_BEST_COMPRESSION;		break;
	case COMPRESS_NONE:		compressLevel = Z_NO_COMPRESSION;		break;
	}
	return compressLevel;
}


bool Compress( const void* buffer, U4 sizeBytes, CompressionMode mode, void* compressed, U4* ptrSize )
{
	Assert(buffer);
	Assert(sizeBytes);
	Assert(compressed);

	int compressLevel = MapZLibCompressionMode( mode );

	// allocate destination buffer
	uLongf destSize;
	if( compress2(reinterpret_cast<byte*>(compressed), &destSize, reinterpret_cast<const byte*>(buffer), 
		static_cast<uLongf>(sizeBytes), compressLevel) != Z_OK )		
	{
		return false;
	}

	// ok
	*ptrSize = static_cast<U4>(destSize);

	return true;
}

bool Decompress( const void* buffer, U4 sizeBytes, void* uncompressed, U4* uncompSizeBytes /*in/out*/ )
{
	Assert(buffer);
	Assert(uncompressed);
	Assert(uncompSizeBytes);

	uLongf destSize = static_cast<uLongf>(*uncompSizeBytes);
	if( uncompress(reinterpret_cast<byte*>(uncompressed), &destSize, 
		static_cast<const byte*>(buffer), static_cast<uLongf>(sizeBytes)) != Z_OK )	
	{
		return false;
	}

	// ok
	*uncompSizeBytes = static_cast<U4>(destSize);

	return true;
}

U4 GetMaxCompressedSize( U4 srcSize )
{
	return static_cast<U4>( compressBound((uLong)srcSize) );
}


mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
