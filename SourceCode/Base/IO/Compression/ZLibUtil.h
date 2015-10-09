/*
=============================================================================
	File:	ZLibUtil.h
	Desc:	ZLibUtil.
=============================================================================
*/

#ifndef __MX_ZLIB_UTIL_H__
#define __MX_ZLIB_UTIL_H__

mxSWIPED("Hammer Engine");

mxNAMESPACE_BEGIN

enum CompressionMode
{
	COMPRESS_NORMAL = 0,
	COMPRESS_FAST,
	COMPRESS_BEST,
	COMPRESS_NONE
};

U4 GetMaxCompressedSize( U4 srcSize );

// pre-allocated routines
bool Compress( const void* buffer, U4 sizeBytes, CompressionMode mode, void* compressed, U4* ptrSize );
bool Decompress( const void* buffer, U4 sizeBytes, void* uncompressed, U4* uncompSizeBytes );

mxNAMESPACE_END

#endif // !__MX_ZLIB_UTIL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
