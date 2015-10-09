/*
=============================================================================
	File:	ConfigFile.cpp
	Desc:
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Base/Text/Lexer.h>
#include <Core/io/IOSystem.h>
#include "ConfigFile.h"

/*================================
			ConfigFile
================================*/

ConfigFile::ConfigFile()
{}

ConfigFile::~ConfigFile()
{}

bool ConfigFile::Load( const char* fileName )
{
	idLexer	parser( LEXFL_ALLOWPATHNAMES | LEXFL_NOSTRINGESCAPECHARS | LEXFL_NOSTRINGCONCAT );

	FileReader	fileStream( fileName );
	if( !fileStream.IsOpen() ) {
		mxWarnf("Failed to load config file '%s'.\n",fileName);
		return false;
	}

	char	buffer[ MAX_CONFIG_FILE_SIZE ] = { '\0' };

	SizeT size = fileStream.GetSize();
	if( size > MAX_CONFIG_FILE_SIZE ) {
		mxWarnf("Config file '%s' of size '%u' is too big\n",fileName,size);
		size = MAX_CONFIG_FILE_SIZE;
	}

	fileStream.Read( buffer, size );

	if( ! parser.LoadMemory( buffer, size, fileName ) ) {
		return false;
	}

	idToken		token;

	while( ! parser.EndOfFile()
		&& parser.ReadToken( &token ) )
	{
		if( token == "[" )
		{
			// config section name
			parser.ReadToken( &token );

			//SEntry * pEntry = this->FindEntry( token.ToChars() );
			//if( !pEntry ) {
			//	pEntry = &m_entries.Add();
			//	pEntry->name = token;
			//} else {
			//	mxWarnf("[file: %s, line: %d] Config file section '%s' has already been defined\n",
			//		parser.GetFileName(),parser.GetLineNum(), pEntry->name.ToChars());
			//}

			parser.ExpectTokenString("]");
		}
		else
		if( token.type == TT_NAME )
		{
			// key in the current config section
			SEntry * pEntry = this->FindEntry( token );
			if( !pEntry ) {
				pEntry = &m_entries.Add();
				pEntry->name = token;
			} else {
				mxWarnf("[file: %s, line: %d] Key '%s' in has already been defined\n",
					parser.GetFileName(),parser.GetLineNum(), token.ToChars());
			}

			// read value
			if( parser.PeekTokenString("=") )
			{
				parser.ExpectTokenString("=");
			}

			parser.ExpectAnyToken( &pEntry->value );
		}
		else
		{
			mxWarnf("invalid token '%s' in config file '%s', line %d\n",
				token.ToChars(),fileName,parser.GetLineNum());
			return false;
		}
	}//while not EOF

	DEVOUT("Loaded INI file '%s' (%u sections)\n",
		fileName,(UINT)m_entries.Num());

	return true;
}

bool ConfigFile::Save( const char* fileName )
{
	UNDONE;
	return true;
}

bool ConfigFile::GetString( const char* key, String &outValue )
{
	if( idToken * value = FindValue( String(key) ) )
	{
		if( value->type == TT_STRING )
		{
			outValue = *value;
			return true;
		}
		else {
			mxWarnf("Variable '%s' should be an string\n",key);
		}
	}
	return false;
}

bool ConfigFile::GetUInt( const char* key, UINT &outValue, UINT min, UINT max )
{
	AssertPtr( key );

	if( idToken * value = FindValue( key ) )
	{
		if( value->type == TT_NUMBER && (value->subtype & TT_INTEGER ) )
		{
			const mxULong result = value->GetUnsignedLongValue();
			if( result < min || result > max ) {
				mxWarnf("Integer '%s' should be in range [%u..%u]\n",key,min,max);
				return false;
			}
			outValue = result;
			return true;
		}
		else {
			mxWarnf("Variable '%s' should be an unsigned int\n",key);
		}
	}
	return false;
}

bool ConfigFile::GetFloat( const char* key, FLOAT &outValue, FLOAT min, FLOAT max )
{
	Unimplemented;
	return false;
}

bool ConfigFile::GetBool( const char* key, bool &outValue )
{
	AssertPtr( key );

	if( idToken * value = FindValue( key ) )
	{
		if( value->type == TT_NUMBER && (value->subtype & TT_INTEGER ) )
		{
			const mxULong result = value->GetUnsignedLongValue();
			if( result < 0 || result > 1 ) {
				goto L_Error;
			}
			outValue = (result != 0);
			return true;
		}
		else if( value->type == TT_STRING )
		{
			if( *value == "true" ) {
				outValue = true;
				return true;
			}
			else if( *value == "false" ) {
				outValue = false;
				return true;
			}
			else {
				goto L_Error;
			}
		}
		else {
			goto L_Error;
		}
	}

	goto L_End;

L_Error:
	mxWarnf("Variable '%s' should be a boolean\n",key);

L_End:
	return false;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
