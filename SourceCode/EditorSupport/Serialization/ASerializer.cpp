#include <EditorSupport_PCH.h>
#pragma hdrstop
#include <EditorSupport.h>

#include <Base/Object/Reflection.h>

#include <Core/Object.h>

#include <EditorSupport/Serialization/ASerializer.h>

/*
-----------------------------------------------------------------------------
	ABinaryObjectWriter
-----------------------------------------------------------------------------
*/

ABinaryObjectWriter::ABinaryObjectWriter( AStreamWriter& stream )
	: ArchivePODWriter( stream )
{

}

void ABinaryObjectWriter::SaveObject_Impl( AObject* o )
{
	// Write the type info for factory function lookup during Load.
	WriteTypeGuid( m_stream, o->rttiGetTypeGuid() );

	// serialize the object to stream
	o->Serialize( *this );
}

/*
-----------------------------------------------------------------------------
	ABinaryObjectReader
-----------------------------------------------------------------------------
*/

ABinaryObjectReader::ABinaryObjectReader( AStreamReader& stream )
	: ArchivePODReader( stream )
{

}

void ABinaryObjectReader::LoadObject_Impl( AObject *& o )
{
	// Get the factory function for the type of object about to be read.
	const TypeGUID typeId = ReadTypeGuid( m_stream );

	Assert(typeId != mxNULL_TYPE_GUID);

	const mxClass * typeInfo = TypeRegistry::Get().FindClassInfoByGuid( typeId );
	AssertPtr(typeInfo);

	// create a valid instance

	o = typeInfo->CreateInstance();
	AssertPtr(o);
	Assert( typeInfo == &o->rttiGetClass() );

	// deserialize the object from stream
	o->Serialize( *this );
}

