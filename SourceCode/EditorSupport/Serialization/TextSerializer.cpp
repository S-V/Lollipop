/*
=============================================================================
	File:	TextSerializer.cpp
	Desc:	text serializers
	ToDo:	refactor - remove ugly casts
	Bugs:	comments are not saved (it's the problem with JsonCpp)
=============================================================================
*/
#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <Base/Object/Reflection.h>

#include <Core/Object.h>

//debugging
#include <Engine/Entities/StaticModelEntity.h>

#include <EditorSupport/Serialization/JsonSerializationCommon.h>

#include <EditorSupport/AssetPipeline/DevAssetManager.h>

#include "TextSerializer.h"

#include <EditorSupport/Serialization/cJSON/cJSON.h>

//---------------------------------------------------------------------------

static
void F_Show_Message_Parse_Error()
{
	mxErrf("Failed to parse JSON\n");
}

//---------------------------------------------------------------------------


/*
-----------------------------------------------------------------------------
	TextWriter
-----------------------------------------------------------------------------
*/
TextWriter::TextWriter()
{
	m_rootValue = cJSON_CreateObject();
	AssertPtr(m_rootValue);
	m_currNode = m_rootValue;
}

TextWriter::~TextWriter()
{
	if( m_rootValue != nil )
	{
		cJSON_Delete( m_rootValue );
	}
	m_currNode = nil;
}

bool TextWriter::WriteAllToStream( AStreamWriter& stream ) const
{
	CHK_VRET_FALSE_IF_NIL( m_rootValue );

	char * text = cJSON_Print( m_rootValue );
	CHK_VRET_FALSE_IF_NIL( text );

	stream.Write( text, mxStrLenAnsi(text) );

	cJSON_Free( text );

	return true;
}

void TextWriter::Enter_Scope( const char* name )
{
	cJSON *	newValue = cJSON_CreateObject();
	{
		StackItem	newItem;
		newItem.name = name;
		newItem.value = m_currNode;

		m_scopeStack.push_back( newItem );
	}
	m_currNode = newValue;
}

void TextWriter::Leave_Scope()
{
	StackItem topItem = m_scopeStack.back();

	cJSON_AddItemToObject( topItem.value, topItem.name, m_currNode );

	m_currNode = topItem.value;

	m_scopeStack.pop_back();
}

bool TextWriter::Serialize_Bool( const char* name, bool & rValue )
{
	cJSON *	booleanValue = cJSON_CreateBool( rValue );
	cJSON_AddItemToObject( m_currNode, name, booleanValue );
	return true;
}

bool TextWriter::Serialize_Int8( const char* name, INT8 & rValue )
{
	cJSON *	intValue = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, intValue );
	return true;
}

bool TextWriter::Serialize_Uint8( const char* name, UINT8 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Int16( const char* name, INT16 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Uint16( const char* name, UINT16 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Int32( const char* name, INT32 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Uint32( const char* name, UINT32 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Int64( const char* name, INT64 & rValue )
{
	//Signed64_Union	union64;
	//union64.v = rValue;

	//Json::Value	int64Value;
	//{
	//	int64Value["Low_Part"] = union64.lo;
	//	int64Value["High_Part"] = union64.hi;
	//}
	//m_currNode[ name ] = int64Value;
	UNDONE;
	return true;
}

bool TextWriter::Serialize_Uint64( const char* name, UINT64 & rValue )
{
	//Unsigned64_Union	union64;
	//union64.v = rValue;

	//Json::Value	uint64Value;
	//{
	//	uint64Value["Low_Part"] = union64.lo;
	//	uint64Value["High_Part"] = union64.hi;
	//}
	//m_currNode[ name ] = uint64Value;
	UNDONE;
	return true;
}

bool TextWriter::Serialize_Float32( const char* name, F4 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_Float64( const char* name, F8 & rValue )
{
	cJSON *	newNode = cJSON_CreateNumber( rValue );
	cJSON_AddItemToObject( m_currNode, name, newNode );
	return true;
}

bool TextWriter::Serialize_String( const char* name, String & rValue )
{
//	cJSON *	newNode = cJSON_CreateString( rValue );
//	cJSON_AddItemToObject( m_currNode, name, newNode );
	cJSON_AddStringToObject( m_currNode, name, rValue );
	return true;
}

bool TextWriter::Serialize_StringList( const char* name, StringListType & rValue )
{
	cJSON *	stringArrayValue = cJSON_CreateArray();

	const UINT numItems = rValue.Num();
	for( Json::Value::UInt i = 0; i < numItems; i++ )
	{
		const String& o = rValue[ i ];

		cJSON *	itemValue = cJSON_CreateString( o );
		cJSON_AddItemToArray( stringArrayValue, itemValue );
	}

	cJSON_AddItemToObject( m_currNode, name, stringArrayValue );

	return true;
}

/*
-----------------------------------------------------------------------------
	TextObjectWriter
-----------------------------------------------------------------------------
*/
TextObjectWriter::TextObjectWriter()
{
//	Assert(gEditor.assets != nil);
}

TextObjectWriter::~TextObjectWriter()
{
	DBGOUT("~TextObjectWriter");
}

void TextObjectWriter::SetRootComment( const char* comment )
{
	AssertPtr(comment);
	CHK_VRET_IF_NIL(comment);

	const INT pos = String::FindText( comment, "//", false );
	Assert( pos != INDEX_NONE );
	if( pos == INDEX_NONE ) {
		mxErr("TextObjectWriter::SetRootComment(): comments must start with '//'.\n");
		return;
	}

	m_rootValue.setComment( comment, Json::CommentPlacement::commentBefore );
}

bool TextObjectWriter::WriteAllToStream( AStreamWriter& stream ) const
{
	CHK_VRET_FALSE_IF_NOT( !m_rootValue.isNull() );

//	JSON::DbgPrintValue( value );

	Json::StyledWriter	writer;

	const std::string	data = writer.write( m_rootValue );

	//OutputDebugStringA(data.c_str());

	stream.Write( data.c_str(), data.length() );

	return true;
}

void TextObjectWriter::Serialize( const void* o, const mxType& typeInfo )
{
	AssertPtr( o );
	m_rootValue[ NODE_TYPE_TAG ] = typeInfo.m_name;
	m_rootValue[ NODE_DATA_TAG ] = JSON::Serialize( typeInfo, o, 0/*offset*/ );
	//JSON::DbgPrintValue( m_rootValue );
}

/*
-----------------------------------------------------------------------------
	TextReader
-----------------------------------------------------------------------------
*/
TextReader::TextReader( AStreamReader& stream )
	: m_currNode( m_rootValue )
{
	const bool bOk = JSON::Read_Value_From_Stream( stream, m_rootValue );
	if( !bOk ) {
		F_Show_Message_Parse_Error();
	}
}

TextReader::~TextReader()
{

}

void TextReader::Enter_Scope( const char* name )
{
	m_scopeStack.push_back( m_currNode );

	m_currNode = m_currNode[ name ];
	//Assert( !m_currNode.isNull() );
	if( m_currNode.isNull() ) {
		mxWarnf("TextReader: null scope: '%s'\n", name);
	}
}

void TextReader::Leave_Scope()
{
	const Json::Value topItem = m_scopeStack.back();
	m_scopeStack.pop_back();

	m_currNode = topItem;
}

bool TextReader::Serialize_Bool( const char* name, bool & rValue )
{
	const Json::Value	booleanValue = m_currNode[ name ];
	if( !booleanValue.isNull() )
	{
		Assert( booleanValue.isBool() );
		rValue = booleanValue.asBool();
		return true;
	}
	else
	{
		return false;
	}
}

bool TextReader::Serialize_Int8( const char* name, INT8 & rValue )
{
	const Json::Value	intValue = m_currNode[ name ];
	if( !intValue.isNull() )
	{
		Assert( intValue.isInt() );
		Assert( intValue.asInt() < MAX_INT8 );
		rValue = intValue.asInt();
		return true;
	}
	else
	{
		mxWarnf("Read_Int8( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Uint8( const char* name, UINT8 & rValue )
{
	const Json::Value	uintValue = m_currNode[ name ];
	if( !uintValue.isNull() )
	{
		Assert( uintValue.isUInt() );
		Assert( uintValue.asUInt() < MAX_UINT8 );
		rValue = uintValue.asUInt();
		return true;
	}
	else
	{
		mxWarnf("Read_Uint8( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Int16( const char* name, INT16 & rValue )
{
	const Json::Value	intValue = m_currNode[ name ];
	if( !intValue.isNull() )
	{
		Assert( intValue.isInt() );
		Assert( intValue.asInt() < MAX_INT16 );
		rValue = intValue.asInt();
		return true;
	}
	else
	{
		mxWarnf("Read_Int16( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Uint16( const char* name, UINT16 & rValue )
{
	const Json::Value	uintValue = m_currNode[ name ];
	if( !uintValue.isNull() )
	{
		Assert( uintValue.isUInt() );
		Assert( uintValue.asUInt() < MAX_UINT16 );
		rValue = uintValue.asUInt();
		return true;
	}
	else
	{
		mxWarnf("Read_Uint16( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Int32( const char* name, INT32 & rValue )
{
	const Json::Value	intValue = m_currNode[ name ];
	if( !intValue.isNull() )
	{
		Assert( intValue.isInt() );
		rValue = intValue.asInt();
		return true;
	}
	else
	{
		mxWarnf("Read_Int32( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Uint32( const char* name, UINT32 & rValue )
{
	const Json::Value	uintValue = m_currNode[ name ];
	if( !uintValue.isNull() )
	{
		JSON::Deserialize_UInt32( uintValue, rValue );
		return true;
	}
	else
	{
		mxWarnf("Read_Uint32( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Int64( const char* name, INT64 & rValue )
{
	const Json::Value	int64Value = m_currNode[ name ];

	if( !int64Value.isNull() )
	{
		Assert( int64Value.isObject() );

		Signed64_Union	union64;

		this->Serialize_Int32( "Low_Part", union64.lo );
		this->Serialize_Int32( "High_Part", union64.hi );

		rValue = union64.v;
		return true;
	}
	else
	{
		mxWarnf("Read_Int64( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Uint64( const char* name, UINT64 & rValue )
{
	const Json::Value	uint64Value = m_currNode[ name ];

	if( !uint64Value.isNull() )
	{
		Assert( uint64Value.isObject() );

		Unsigned64_Union	union64;

		this->Serialize_Uint32( "Low_Part", union64.lo );
		this->Serialize_Uint32( "High_Part", union64.hi );

		rValue = union64.v;
		return true;
	}
	else
	{
		mxWarnf("Read_UInt64( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Float32( const char* name, F4 & rValue )
{
	const Json::Value	doubleValue = m_currNode[ name ];
	if( !doubleValue.isNull() )
	{
		Assert( doubleValue.isDouble() );
		rValue = doubleValue.asDouble();
		return true;
	}
	else
	{
		mxWarnf("Read_Float32( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_Float64( const char* name, F8 & rValue )
{
	const Json::Value	doubleValue = m_currNode[ name ];
	if( !doubleValue.isNull() )
	{
		Assert( doubleValue.isDouble() );
		rValue = doubleValue.asDouble();
		return true;
	}
	else
	{
		mxWarnf("Read_Float64( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_String( const char* name, String & rValue )
{
	const Json::Value	stringValue = m_currNode[ name ];
	if( !stringValue.isNull() )
	{
		Assert( stringValue.isString() );
		rValue = stringValue.asCString();
		return true;
	}
	else
	{
		mxWarnf("Read_String( '%s' ) = null\n", name);
		return false;
	}
}

bool TextReader::Serialize_StringList( const char* name, StringListType & rValue )
{
	const Json::Value	stringArrayValue = m_currNode[ name ];

	if( !stringArrayValue.isNull() )
	{
		Assert( stringArrayValue.isArray() );

		const UINT numItems = stringArrayValue.size();
		rValue.SetNum( numItems );

		for( Json::Value::UInt i = 0; i < numItems; i++ )
		{
			const Json::Value	objectData = stringArrayValue[ i ];
			rValue[ i ] = objectData.asCString();
		}

		return true;
	}
	else
	{
		mxWarnf("Read_StringList( '%s' ) = null\n", name);
		return false;
	}
}

/*
-----------------------------------------------------------------------------
	TextObjectReader
-----------------------------------------------------------------------------
*/
TextObjectReader::TextObjectReader( AStreamReader& stream )
{
	const bool bOk = JSON::Read_Value_From_Stream( stream, m_rootValue );
	if( !bOk ) {
		F_Show_Message_Parse_Error();
	}
}

TextObjectReader::~TextObjectReader()
{
	DBGOUT("~TextObjectReader");
}

void TextObjectReader::Deserialize( void * o, const mxType& typeInfo )
{
	AssertPtr( o );

	//JSON::DbgPrintValue( m_rootValue );

	CHK_VRET_IF_NOT(!m_rootValue.isNull());

	const Json::Value metaData = m_rootValue[ NODE_TYPE_TAG ];
	CHK_VRET_IF_NOT(!metaData.isNull());

	const std::string typeName = metaData.asString();
	CHK_VRET_IF_NOT( mxStrEquAnsi( typeName.c_str(), typeInfo.m_name ) );

	const Json::Value objectData = m_rootValue[ NODE_DATA_TAG ];
	JSON::Deserialize( objectData, typeInfo, o, 0/*offset*/ );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
