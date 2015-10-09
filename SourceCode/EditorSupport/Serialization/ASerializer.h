#pragma once

#include <Core/Serialization.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
-----------------------------------------------------------------------------
	ASerializer
	abstract high-level serializer
-----------------------------------------------------------------------------
*/
class ASerializer
{
public:
	// serialize a pointer to an object
	//virtual void ProcessPointer( SBaseType *& o, const mxType& typeInfo ) = 0;

protected:
	virtual ~ASerializer() {}
};

/*
-----------------------------------------------------------------------------
	ATextSerializer
-----------------------------------------------------------------------------
*/
class ATextSerializer : public ASerializer
{
public:
	virtual void Enter_Scope( const char* name ) = 0;
	virtual void Leave_Scope() = 0;

	virtual bool Serialize_Bool( const char* name, bool & rValue ) = 0;
	virtual bool Serialize_Int8( const char* name, INT8 & rValue ) = 0;
	virtual bool Serialize_Uint8( const char* name, UINT8 & rValue ) = 0;
	virtual bool Serialize_Int16( const char* name, INT16 & rValue ) = 0;
	virtual bool Serialize_Uint16( const char* name, UINT16 & rValue ) = 0;
	virtual bool Serialize_Int32( const char* name, INT32 & rValue ) = 0;
	virtual bool Serialize_Uint32( const char* name, UINT32 & rValue ) = 0;
	virtual bool Serialize_Int64( const char* name, INT64 & rValue ) = 0;
	virtual bool Serialize_Uint64( const char* name, UINT64 & rValue ) = 0;
	virtual bool Serialize_Float32( const char* name, F4 & rValue ) = 0;
	virtual bool Serialize_Float64( const char* name, F8 & rValue ) = 0;
	//virtual void Serialize_Buffer( void* pBuffer, size elementSize, size count ) = 0;
	//virtual void Serialize_Enum( INT32 & rValue, UINT32 nameCount, const tchar* const* ppNames ) = 0;
	//virtual void Serialize_Enum( INT32 & rValue, const Helium::Reflect::Enumeration* pEnumeration ) = 0;
	//virtual void Serialize_CharName( CharName & rValue ) = 0;
	//virtual void Serialize_WideName( WideName & rValue ) = 0;
	//virtual void Serialize_CharString( CharString & rValue ) = 0;
	//virtual void Serialize_WideString( WideString & rValue ) = 0;
	// 
	virtual bool Serialize_String( const char* name, String & rValue ) = 0;
	virtual bool Serialize_StringList( const char* name, StringListType & rValue ) = 0;

	virtual bool IsLoading() const { return false; }
	virtual bool IsStoring() const { return false; }

protected:
	virtual ~ATextSerializer() {}
};

class TextScope
{
public:
	TextScope( ATextSerializer& serializer, const char* scopeName )
		: m_serializer( serializer )
	{
		m_serializer.Enter_Scope( scopeName );
	}
	~TextScope()
	{
		m_serializer.Leave_Scope();
	}
private:
	ATextSerializer &	m_serializer;
};

/*
-----------------------------------------------------------------------------
	ABinaryObjectWriter
-----------------------------------------------------------------------------
*/
class ABinaryObjectWriter : public ArchivePODWriter
{
public:
	ABinaryObjectWriter( AStreamWriter& stream );

	template< class KLASS >	// where KLASS : AObject
	void Save( KLASS & o )
	{
		this->SaveObject_Impl( &o );
	}

protected:
	virtual void SaveObject_Impl( AObject* o );
};

/*
-----------------------------------------------------------------------------
	ABinaryObjectReader
-----------------------------------------------------------------------------
*/
class ABinaryObjectReader : public ArchivePODReader
{
public:
	ABinaryObjectReader( AStreamReader& stream );

	template< class KLASS >	// where KLASS : AObject
	void Load( KLASS *& o )
	{
		this->LoadObject_Impl( (AObject*&)o );
	}

protected:
	virtual void LoadObject_Impl( AObject *& o );
};


