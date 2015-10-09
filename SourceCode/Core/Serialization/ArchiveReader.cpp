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
	ArchivePODReader
-----------------------------------------------------------------------------
*/
ArchivePODReader::ArchivePODReader( AStreamReader& stream )
	: m_stream( stream )
{
}
//---------------------------------------------------------------------------
ArchivePODReader::~ArchivePODReader()
{
}
//---------------------------------------------------------------------------
void ArchivePODReader::SerializeMemory( void* ptr, SizeT size )
{
	m_stream.Read( ptr, size );
}
//---------------------------------------------------------------------------
void ArchivePODReader::SerializeResourceGuid( ObjectGUID & resourceGuid )
{
	m_stream >> resourceGuid.v;
}




/*
-----------------------------------------------------------------------------
	ArchiveReader
-----------------------------------------------------------------------------
*/
ArchiveReader::ArchiveReader( AStreamReader& stream )
	: ArchivePODReader( stream )
	, m_loaded( BinarySerialization::DEFAULT_HASH_TABLE_SIZE, EMemHeap::HeapTemp )
{
	// Insert a pairing between the null pointer and zero index.
	AEditableRefCounted* nullPointer = nil;
	ObjectUid nullPointerID = 0;

	m_loaded.Set( nullPointerID, nullPointer );

#if 0
	// read header
	mxArchiveHeader	currHeader;

	mxArchiveHeader	readHeader;
	m_stream >> readHeader;

	if( !currHeader.Matches(readHeader) )
	{
		mxErrf("Incompatible archives, version mismatch!\n");
	}
#endif
}
//---------------------------------------------------------------------------
ArchiveReader::~ArchiveReader()
{

}
//---------------------------------------------------------------------------
void ArchiveReader::SerializePtr( AObject *& o )
{
	// read object id
	ObjectUid uniqueId;
	m_stream >> uniqueId;

	AObject** existing = m_loaded.Find( uniqueId );
	if(PtrToBool( existing ))
	{
		// this object has already been registered and deserialized
		// NOTE: 'existing' may point to the null pointer.
		o = *existing;
	}
	else
	{
		// this object hasn't yet been deserialized...

		// read object header
 
		// Get the factory function for the type of object about to be read.
		const TypeGUID typeId = ReadTypeGuid(m_stream);
		Assert(typeId != mxNULL_TYPE_GUID);

		const mxClass * typeInfo = TypeRegistry::Get().FindClassInfoByGuid( typeId );
		AssertPtr(typeInfo);

		// create a valid instance

		o = typeInfo->CreateInstance();
		AssertPtr(o);
		Assert( typeInfo == &o->rttiGetClass() );

		// deserialize the object from stream
		o->Serialize( *this );

		// and remember it
		m_loaded.Set( uniqueId, o );
	}
}
//---------------------------------------------------------------------------
void ArchiveReader::InsertRootObject( AObject* o )
{
	this->InsertObject( o );
}
//---------------------------------------------------------------------------
ObjectUid ArchiveReader::InsertObject( AObject* o )
{
	AssertPtr(o);

	const ObjectUid uniqueID = m_loaded.NumEntries();

	m_loaded.Set( uniqueID, o );

	return uniqueID;
}


mxNAMESPACE_END

NO_EMPTY_FILE
