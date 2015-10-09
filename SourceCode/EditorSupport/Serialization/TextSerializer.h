#pragma once

#include <vector>

#include <Base/Object/ClassDescriptor.h>

#include <EditorSupport/Serialization/ASerializer.h>
#include <EditorSupport/Serialization/JSON/json.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// asset database is used to perform conversions between string and integer resource ids
class DevAssetManager;

namespace Json
{
	class Value;

	// for serialization 'scopes'
	struct StackItem
	{
		String	name;
		Value	value;
	};

}//namespace Json

class cJSON;

/*
-----------------------------------------------------------------------------
	TextWriter
-----------------------------------------------------------------------------
*/
class TextWriter : public ATextSerializer
{
public:
	TextWriter();
	~TextWriter();

	// writes serialized data in text form to stream
	bool WriteAllToStream( AStreamWriter& stream ) const;

	//=-- ATextSerializer

	virtual void Enter_Scope( const char* name ) override;
	virtual void Leave_Scope() override;

	virtual bool Serialize_Bool( const char* name, bool & rValue ) override;
	virtual bool Serialize_Int8( const char* name, INT8 & rValue ) override;
	virtual bool Serialize_Uint8( const char* name, UINT8 & rValue ) override;
	virtual bool Serialize_Int16( const char* name, INT16 & rValue ) override;
	virtual bool Serialize_Uint16( const char* name, UINT16 & rValue ) override;
	virtual bool Serialize_Int32( const char* name, INT32 & rValue ) override;
	virtual bool Serialize_Uint32( const char* name, UINT32 & rValue ) override;
	virtual bool Serialize_Int64( const char* name, INT64 & rValue ) override;
	virtual bool Serialize_Uint64( const char* name, UINT64 & rValue ) override;
	virtual bool Serialize_Float32( const char* name, F4 & rValue ) override;
	virtual bool Serialize_Float64( const char* name, F8 & rValue ) override;
	virtual bool Serialize_String( const char* name, String & rValue ) override;
	virtual bool Serialize_StringList( const char* name, StringListType & rValue ) override;

	virtual bool IsLoading() const { return false; }
	virtual bool IsStoring() const { return true; }

private:
	struct StackItem
	{
		cJSON *	value;
		String	name;
	};

private:
	cJSON *		m_rootValue;
	cJSON *		m_currNode;
	std::vector< StackItem >	m_scopeStack;
};


/*
-----------------------------------------------------------------------------
	TextObjectWriter
-----------------------------------------------------------------------------
*/
class TextObjectWriter : public AObjectWriter
{
	Json::Value		m_rootValue;

public:
	// need asset database to perform conversions between string and integer resource ids
	TextObjectWriter();
	~TextObjectWriter();

	// NOTE: comments must begin with "//".
	void SetRootComment( const char* comment );

	// writes serialized data in text form to stream
	bool WriteAllToStream( AStreamWriter& stream ) const;

//protected:

	//=== AObjectWriter

	virtual void Serialize( const void* o, const mxType& typeInfo ) override;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
-----------------------------------------------------------------------------
	TextReader
-----------------------------------------------------------------------------
*/
class TextReader : public ATextSerializer
{
public:
	TextReader( AStreamReader& stream );
	~TextReader();

	//=-- ATextSerializer

	virtual void Enter_Scope( const char* name ) override;
	virtual void Leave_Scope() override;

	virtual bool Serialize_Bool( const char* name, bool & rValue ) override;
	virtual bool Serialize_Int8( const char* name, INT8 & rValue ) override;
	virtual bool Serialize_Uint8( const char* name, UINT8 & rValue ) override;
	virtual bool Serialize_Int16( const char* name, INT16 & rValue ) override;
	virtual bool Serialize_Uint16( const char* name, UINT16 & rValue ) override;
	virtual bool Serialize_Int32( const char* name, INT32 & rValue ) override;
	virtual bool Serialize_Uint32( const char* name, UINT32 & rValue ) override;
	virtual bool Serialize_Int64( const char* name, INT64 & rValue ) override;
	virtual bool Serialize_Uint64( const char* name, UINT64 & rValue ) override;
	virtual bool Serialize_Float32( const char* name, F4 & rValue ) override;
	virtual bool Serialize_Float64( const char* name, F8 & rValue ) override;
	virtual bool Serialize_String( const char* name, String & rValue ) override;
	virtual bool Serialize_StringList( const char* name, StringListType & rValue ) override;

	virtual bool IsLoading() const { return true; }
	virtual bool IsStoring() const { return false; }

protected:
	Json::Value			m_rootValue;
	Json::Value &  		m_currNode;
	std::vector< Json::Value >	m_scopeStack;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
-----------------------------------------------------------------------------
	TextObjectReader
-----------------------------------------------------------------------------
*/
class TextObjectReader : public AObjectReader
{
public:
	// need asset database to perform conversions between string and integer resource ids
	TextObjectReader( AStreamReader& stream );
	~TextObjectReader();

//protected:
	//=== AObjectReader

	virtual void Deserialize( void * o, const mxType& typeInfo ) override;

private:
	Json::Value		m_rootValue;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

