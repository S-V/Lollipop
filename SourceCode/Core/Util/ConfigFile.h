/*
=============================================================================
	File:	ConfigFile.h
	Desc:	A minimalistic implementation of INI file.
	Note:	it's not recommended to use because of its poor memory management
			(many small mallocs/frees, etc)
=============================================================================
*/

#ifndef __MX_CONFIG_FILE_H__
#define __MX_CONFIG_FILE_H__

#include <Base/Text/Token.h>
#include <Core/Core.h>

//
//	ConfigFile
//
class ConfigFile : public INIConfigFile {
public:
	// max. allowed size of INI file including nil terminator
	enum { MAX_CONFIG_FILE_SIZE = 4096 };	// 4 Kb

				ConfigFile();
	virtual		~ConfigFile();

	bool	Load( const char* fileName );
	bool	Save( const char* fileName );

public:	//== INIConfigFile
	virtual bool GetString( const char* key, String &outValue ) override;
	virtual bool GetUInt( const char* key, UINT &outValue, UINT min = 0, UINT max = MAX_UINT32 ) override;
	virtual bool GetFloat( const char* key, FLOAT &outValue, FLOAT min = 0.0f, FLOAT max = MX_INFINITY ) override;
	virtual bool GetBool( const char* key, bool &outValue ) override;

public:
	//mxOPTIMIZE("use a fast hash table (StringHashMap with HashedString)");
	struct SEntry
	{
		String	name;
		idToken	value;
	};

	SEntry * FindEntry( const String& name )
	{
		for(UINT i=0; i < m_entries.Num(); i++) {
			SEntry & r = m_entries[i];
			if( r.name == name ) {
				return &r;
			}
		}
		return nil;
	}

	idToken * FindValue( const String& key ) {
		SEntry * pEntry = FindEntry( key );
		if( pEntry ) {
			return &pEntry->value;
		}
		return nil;
	}

private:
	//mxOPTIMIZE("use hashed strings? use binary search on sorted array?");
	TList< SEntry >	m_entries;
};

#endif // !__MX_CONFIG_FILE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
