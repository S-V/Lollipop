#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <Base/Object/Reflection.h>

#include <Core/Object.h>

#include <EditorSupport/Serialization/XmlSerializer.h>
#include <EditorSupport/Serialization/XML/pugixml.hpp>

 
#if 0
/*
-----------------------------------------------------------------------------
	my_xml_writer
-----------------------------------------------------------------------------
*/
class my_xml_writer : public pugi::xml_writer
{
	AStreamWriter &	m_stream;

public:
	inline my_xml_writer( AStreamWriter& stream )
		: m_stream( stream )
	{
	}
	// Write memory chunk into stream/file/whatever
	virtual void write(const void* data, size_t size) override
	{
		m_stream.Write( data, size );
	}
};

/*
-----------------------------------------------------------------------------
	XmlWriter
-----------------------------------------------------------------------------
*/
XmlWriter::XmlWriter( AStreamWriter& stream )
	: m_stream( stream )
{

}
void XmlWriter::Serialize_Bool( const char* name, bool & rValue )
{

}
void XmlWriter::Serialize_Int8( const char* name, INT8 & rValue )
{

}
void XmlWriter::Serialize_Uint8( const char* name, UINT8 & rValue )
{

}
void XmlWriter::Serialize_Int16( const char* name, INT16 & rValue )
{

}
void XmlWriter::Serialize_Uint16( const char* name, UINT16 & rValue )
{

}
void XmlWriter::Serialize_Int32( const char* name, INT32 & rValue )
{

}
void XmlWriter::Serialize_Uint32( const char* name, UINT32 & rValue )
{

}
void XmlWriter::Serialize_Int64( const char* name, INT64 & rValue )
{

}
void XmlWriter::Serialize_Uint64( const char* name, UINT64 & rValue )
{

}
void XmlWriter::Serialize_Float32( const char* name, F4 & rValue )
{

}
void XmlWriter::Serialize_Float64( const char* name, F8 & rValue )
{

}
#endif
NO_EMPTY_FILE
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
