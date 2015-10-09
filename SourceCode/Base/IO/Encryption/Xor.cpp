/*
=============================================================================
	File:	Xor.cpp
	Desc:
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

void EncodeString_Xor( const char* src, SizeT size, UINT key, char *dest )
{
	for( SizeT iChar = 0; iChar < size; iChar++ )
	{
		dest[ iChar ] = src[ iChar ] ^ key;
	}
}

void DecodeString_Xor( const char* src, SizeT size, UINT key, char *dest )
{
	for( SizeT iChar = 0; iChar < size; iChar++ )
	{
		dest[ iChar ] = src[ iChar ] ^ key;
	}
}

void WriteEncodedString( AStreamWriter & stream, const char* src, SizeT size, UINT seed )
{
	for( SizeT iChar = 0; iChar < size; iChar++ )
	{
		char c = src[ iChar ] ^ seed;
		stream << c;
	}
}
void ReadDecodedString( AStreamReader & stream, char* dest, SizeT size, UINT seed )
{
	for( SizeT iChar = 0; iChar < size; iChar++ )
	{
		char c;
		stream >> c;
		dest[ iChar ] = c ^ seed;
	}
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
