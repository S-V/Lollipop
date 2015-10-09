/*
=============================================================================
	File:	Serialization.cpp
	Desc:	
=============================================================================
*/

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
	ArchivePODWriter
-----------------------------------------------------------------------------
*/
ArchivePODWriter::ArchivePODWriter( AStreamWriter& stream )
	: m_stream( stream )
{
}
//---------------------------------------------------------------------------
ArchivePODWriter::~ArchivePODWriter()
{
}
//---------------------------------------------------------------------------
void ArchivePODWriter::SerializeMemory( void* ptr, SizeT size )
{
	m_stream.Write( ptr, size );
}
//---------------------------------------------------------------------------
void ArchivePODWriter::SerializeResourceGuid( ObjectGUID & resourceGuid )
{
	m_stream << resourceGuid.v;
}

/*
-----------------------------------------------------------------------------
	ArchiveWriter
-----------------------------------------------------------------------------
*/
ArchiveWriter::ArchiveWriter( AStreamWriter& stream )
	: Super( stream )
	, m_registered( BinarySerialization::DEFAULT_HASH_TABLE_SIZE, EMemHeap::HeapTemp )
{
	// Insert a pairing between the null pointer and zero index.
	AObject* nullPointer = nil;
	ObjectUid nullPointerID = 0;

	m_registered.Set( nullPointer, nullPointerID );

#if 0
	// write header
	mxArchiveHeader	header;
	m_stream << header;
#endif
}
//---------------------------------------------------------------------------
ArchiveWriter::~ArchiveWriter()
{

}
//---------------------------------------------------------------------------
void ArchiveWriter::SerializePtr( AObject *& o )
{
	// An object can be serialized only once.
	
	if( ObjectUid* registeredID = m_registered.Find( o ) )
	{
		// this object has already been registered and serialized

		// write this object's unique id
		m_stream << *registeredID;
	}
	else
	{
		// this object hasn't yet been registered

		// null pointer is already registered
		//AssertPtr(o);

		// generate a new object id and remember it

		const ObjectUid uniqueID = this->InsertObject( o );

		// Write the unique identifier for the object to stream.
		// This is used during loading and linking.
		m_stream << uniqueID;

		// Write the type info for factory function lookup during Load.
		WriteTypeGuid( m_stream, o->rttiGetTypeGuid() );

		// write object header
		//o->rttiGetClass().PreSave( m_stream, o );

		// serialize the object to stream
		o->Serialize( *this );
	}

}
//---------------------------------------------------------------------------
void ArchiveWriter::InsertRootObject( AObject* o )
{
	this->InsertObject( o );
}
//---------------------------------------------------------------------------
ObjectUid ArchiveWriter::InsertObject( AObject* o )
{
	AssertPtr(o);

	const ObjectUid uniqueID = m_registered.NumEntries();

	m_registered.Set( o, uniqueID );

	return uniqueID;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

mxNAMESPACE_END

NO_EMPTY_FILE
