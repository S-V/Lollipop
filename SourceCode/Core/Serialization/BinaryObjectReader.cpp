#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Object.h>
#include <Core/Serialization.h>
#include <Core/Serialization/BinarySerializationCommon.h>
#include <Core/Resources.h>

mxNAMESPACE_BEGIN

/*
-----------------------------------------------------------------------------
	BinaryObjectReader
-----------------------------------------------------------------------------
*/
BinaryObjectReader::BinaryObjectReader( AStreamReader& stream )
	: m_stream( stream )
{
}

BinaryObjectReader::~BinaryObjectReader()
{
}

void BinaryObjectReader::Deserialize( void * o, const mxType& typeInfo )
{
	BinarySerialization::Deserialize( m_stream, typeInfo, o, 0/*offset*/ );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
