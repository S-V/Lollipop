#pragma once

#include <EditorSupport/Serialization/ASerializer.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace pugi
{
	class xml_document;
}//namespace pugi

class XmlWriter : public ATextSerializer
{
	AStreamWriter &	m_stream;

	TPtr< pugi::xml_document >	m_doc;

public:
	XmlWriter( AStreamWriter& stream );
	~XmlWriter();

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
	//virtual void Serialize_Buffer( void* pBuffer, size elementSize, size count ) override;
	//virtual void Serialize_Enum( INT32 & rValue, UINT32 nameCount, const tchar* const* ppNames ) override;
	//virtual void Serialize_Enum( INT32 & rValue, const Helium::Reflect::Enumeration* pEnumeration ) override;
	//virtual void Serialize_CharName( CharName & rValue ) override;
	//virtual void Serialize_WideName( WideName & rValue ) override;
	//virtual void Serialize_CharString( CharString & rValue ) override;
	//virtual void Serialize_WideString( WideString & rValue ) override;
};

