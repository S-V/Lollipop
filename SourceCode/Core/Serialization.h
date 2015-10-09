/*
=============================================================================
	File:	Serialization.h
	Desc:	Serialization framework, main header file.
	ToDo:	it's possible to do serialization using a reflection framework
			supporting complex (i.e. non-POD, nontrivial) types.
=============================================================================
*/

#pragma once

#include <Base/IO/InPlaceMemoryStream.h>
#include <Base/Templates/Containers/HashMap/TMap.h>
#include <Base/Object/TypeDescriptor.h>
#include <Base/Object/ClassDescriptor.h>
#include <Base/Object/AObject.h>


//!=- BUILD CONFIG -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
#define MX_DEBUG_SERIALIZATION	(0)

//#define MX_LOG_SERIALIZATION	(MX_DEBUG)


mxNAMESPACE_BEGIN


/*
=======================================================================

	Serialization of POD types.

=======================================================================
*/

/*-----------------------------------------------------------------------------
	DataSaver

	Templated serializer for serialization of POD types.
-----------------------------------------------------------------------------
*/

struct DataWriter
{
	typedef DataWriter THIS_TYPE;

	FORCEINLINE bool IsReading() const { return false; }
	FORCEINLINE bool IsWriting() const { return !IsReading(); }


	DataWriter( AStreamWriter& writer )
		: stream( writer )
	{
	}

	template< UINT SIZE >
	THIS_TYPE& operator & ( TStackString<SIZE> & o )
	{
		stream << o;
		return *this;
	}

	THIS_TYPE& operator & ( String & o )
	{
		stream << o;
		return *this;
	}

	template< typename TYPE >
	THIS_TYPE& operator & ( TList<TYPE> & o )
	{
		const U4 num = o.Num();
		const SizeT dataSize = o.GetDataSize();

		stream << num;

		if( TypeTrait< TYPE >::IsPlainOldDataType )
		{
			if( dataSize > 0 )
			{
				stream.Write( o.ToPtr(), dataSize );
			}
		}
		else
		{
			for( UINT i = 0; i < num; i++ )
			{
				*this & o[i];
			}
		}

		return *this;
	}


public:

	template< typename TYPE >
	inline THIS_TYPE & SerializeChunk( const TYPE& value )
	{
		stream.Write( &value, sizeof(TYPE) );
		return *this;
	}

	inline THIS_TYPE & SerializeMemory( const void* ptr, const SizeT numBytes )
	{
		stream.Write( ptr, numBytes );
		return *this;
	}

	template< typename TYPE >
	inline THIS_TYPE & SerializeViaStream( const TYPE& value )
	{
		stream << value;
		return *this;
	}

	inline AStreamWriter& GetStream() { return stream; }

	template< typename TYPE >
	inline void RelocatePointer( TYPE*& ptr )
	{
		Unreachable;
	}

protected:
	AStreamWriter &	stream;
};

/*
-----------------------------------------------------------------------------
	DataLoader

	Template for deserializing POD objects.
-----------------------------------------------------------------------------
*/
struct DataReader
{
	typedef DataReader THIS_TYPE;

	FORCEINLINE bool IsReading() const { return true; }
	FORCEINLINE bool IsWriting() const { return !IsReading(); }


	DataReader( AStreamReader& reader )
		: stream( reader )
	{
	}

	template< UINT SIZE >
	THIS_TYPE& operator & ( TStackString<SIZE> & o )
	{
		stream >> o;
		return *this;
	}

	THIS_TYPE& operator & ( String & o )
	{
		stream >> o;
		return *this;
	}

	template< typename TYPE >
	THIS_TYPE& operator & ( TList<TYPE> & o )
	{
		U4 num;
		stream >> num;

		o.SetNum( num );

		const SizeT dataSize = num * sizeof TYPE;

		if( TypeTrait< TYPE >::IsPlainOldDataType )
		{
			if( dataSize > 0 )
			{
				stream.Read( o.ToPtr(), dataSize );
			}
		}
		else
		{
			for( UINT i = 0; i < num; i++ )
			{
				*this & o[i];
			}
		}

		return *this;
	}


public:

	template< typename TYPE >
	inline THIS_TYPE & SerializeChunk( TYPE &value )
	{
		stream.Read( &value, sizeof(TYPE) );
		return *this;
	}

	inline THIS_TYPE & SerializeMemory( void *ptr, const SizeT numBytes )
	{
		stream.Read( ptr, numBytes );
		return *this;
	}

	template< typename TYPE >
	inline THIS_TYPE & SerializeViaStream( TYPE &value )
	{
		stream >> value;
		return *this;
	}

	inline AStreamReader& GetStream() { return stream; }

	template< typename TYPE >
	inline void RelocatePointer( TYPE*& ptr )
	{
		Unreachable;
	}

protected:
	AStreamReader &	stream;
};
//---------------------------------------------------------------------------


// (de-)serializes from/to archive via templated data serializers
//
template< typename T >
inline mxArchive& Serialize_ArcViaBin( mxArchive& archive, T & o )
{
	if( AStreamWriter* stream = archive.IsWriter() )
	{
		DataWriter	writer( *stream );
		writer & o;
		return archive;
	}
	if( AStreamReader* stream = archive.IsReader() )
	{
		DataReader	reader( *stream );
		reader & o;
		return archive;
	}
	return archive;
}



mxSWIPED("CryEngine");
// this enumeration details what "kind" of serialization we are
// performing, so that classes that want to, for instance, tailor
// the data they present depending on where data is being written
// to can do so
enum ESerializationTarget
{
	eST_SaveGame,
	eST_Network,
	eST_Script
};

/*
-----------------------------------------------------------------------------
	ArchivePODWriter
-----------------------------------------------------------------------------
*/
struct ArchivePODWriter : public mxArchive
{
public:
	ArchivePODWriter( AStreamWriter& stream );
	~ArchivePODWriter();

	virtual void SerializeMemory( void* ptr, SizeT size ) override;

	virtual void SerializeResourceGuid( ObjectGUID & resourceGuid ) override;

	inline AStreamWriter& GetStream() {return m_stream;}
	virtual AStreamWriter* IsWriter() override {return &m_stream;}

protected:
	AStreamWriter &	m_stream;
};

/*
-----------------------------------------------------------------------------
	ArchivePODReader
-----------------------------------------------------------------------------
*/
struct ArchivePODReader : public mxArchive
{
public:
	ArchivePODReader( AStreamReader& stream );
	~ArchivePODReader();

	virtual void SerializeMemory( void* ptr, SizeT size ) override;

	virtual void SerializeResourceGuid( ObjectGUID & resourceGuid ) override;

	inline AStreamReader& GetStream() {return m_stream;}
	virtual AStreamReader* IsReader() override {return &m_stream;}

protected:
	AStreamReader &	m_stream;
};

/*
=======================================================================

	Serialization of complex types.

	If you have object references in your graph,
	the objects will only be serialized
	on the first time that they are encountered,
	therefore the same relationships will return from serialization
	as existed when the object was persisted.
	The serializer keeps a note of the objects that it is persisting
	and stores references to previously seen items,
	so everything is the same when you rehydrate the graph.

=======================================================================
*/

// unique object id (used for serialization)
// NOTE: zero object id is reserved for null pointers
typedef U4 ObjectUid;

/*
-----------------------------------------------------------------------------
	ArchiveWriter
-----------------------------------------------------------------------------
*/
class ArchiveWriter : public ArchivePODWriter
{
public:
	typedef ArchivePODWriter Super;

	ArchiveWriter( AStreamWriter& stream );
	~ArchiveWriter();

	virtual void SerializePtr( AObject *& o ) override;
	virtual void InsertRootObject( AObject* o ) override;

public:
	ObjectUid InsertObject( AObject* o );

private:
	// so that objects get serialized only once
	TMap< const AObject*, ObjectUid >	m_registered;
};


/*
-----------------------------------------------------------------------------
	ArchiveReader
-----------------------------------------------------------------------------
*/
class ArchiveReader : public ArchivePODReader
{
public:
	typedef ArchivePODReader Super;

	ArchiveReader( AStreamReader& stream );
	~ArchiveReader();

	virtual void SerializePtr( AObject *& o ) override;
	virtual void InsertRootObject( AObject* o ) override;

public:
	ObjectUid InsertObject( AObject* o );

private:
	// so that objects get deserialized only once
	TMap< ObjectUid, AObject* >	m_loaded;
};



/*
=======================================================================

	In-Place Serialization

=======================================================================
*/

namespace InPlace
{

// for debugging
enum { MAX_DATA_SIZE = mxMEGABYTE/2 };

// EFFICIENT_ALIGNMENT
enum { DATA_ALIGNMENT = 16 };

template< class STREAM >
inline
void SeekAlignedOffset( STREAM & file )
{
	const SizeT currOffset = file.Tell();
	const SizeT alignedOffset = ALIGN_VALUE( currOffset, DATA_ALIGNMENT );
	if( alignedOffset != currOffset )
	{
		Assert( alignedOffset > currOffset );
		file.Seek( alignedOffset );
	}
}

/*
-----------------------------------------------------------------------------
	MemNode
-----------------------------------------------------------------------------
*/
struct SObjInfo
{
	const void *ptr;	// pointer to the original structure
	mxULong		offset;	// position of the structure relative to the start of the memory block
};

// used for table-driven pointer relocation
enum { MAX_POINTERS = 32 };

// relocatable pointer stored in a fix-up table
//
#pragma pack (push,1)
struct SPtr
{
	U4	ptrOffset;	// position of the pointer relative to the start of the memory block
	U4	dataOffset;	// position of the pointed data relative to the start of the memory block
};
#pragma pack (pop)
mxDECLARE_POD_TYPE(SPtr);

//
// Fix-Up table (aka relocation table)
//
// often resource data files contain relocation entries often called fixups
// that identify the places in the data where addresses need to be modified
// when the data is loaded (much like .EXE or .DLL are loaded).
//
struct FixUpTable
{
	TStaticList<SPtr,MAX_POINTERS>	pointers;

public:
	UINT CalcMemoryUsage() const
	{
		mxStreamWriter_CountBytes	stream;
		stream << *this;
		return stream.NumBytesWritten();
	}
	friend AStreamWriter& operator << ( AStreamWriter& file, const FixUpTable& o )
	{
		file << o.pointers;
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, FixUpTable& o )
	{
		file >> o.pointers;
		return file;
	}
};


#define GENERATE_DUMMY_HELPER_FUNCTIONS\
	void ProcessElement( const BYTE & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const S2 & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const U2 & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const S4 & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const U4 & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const S8 & o, const SObjInfo& containingObj ) {}\
	void ProcessElement( const U8 & o, const SObjInfo& containingObj ) {}


/*
-----------------------------------------------------------------------------
	Serializer

	doesn't support polymorphic types
	and circular links
-----------------------------------------------------------------------------
*/
class Serializer
{
	mxMemoryStream		m_stream;
	FixUpTable			m_fixUpTable;
	TList<const void*>	m_collectedPointers;	// diagnostics

public:
	GENERATE_DUMMY_HELPER_FUNCTIONS;

	Serializer()
	{
	}

	// saves the object into contiguous memory block
	template< typename TYPE >
	inline void SaveObject( const TYPE & o, MemoryBlob &rawMem )
	{
		Assert( m_stream.Tell() == 0 );

		SObjInfo	objData;
		objData.ptr = &o;
		objData.offset = 0;

		// write raw object data
		m_stream.Pack( o );

		// serialize links
		o.CollectPointers( *this, objData );


		MemoryBlobWriter	memWriter( rawMem );

		// write relocation table
		memWriter << m_fixUpTable;

		// write object data at aligned offset

		SeekAlignedOffset( memWriter );

		memWriter.Write( m_stream.ToPtr(), m_stream.GetSize() );


		const SizeT totalSize = memWriter.Tell();
		DBGOUT("Saved %u bytes (%u bytes for fix-up table)\n",
			(UINT)totalSize,	m_fixUpTable.CalcMemoryUsage());
	}

	inline BYTE* GetBaseAddress()
	{
		return m_stream.ToPtr();
	}

	template< typename TYPE >
	inline void ProcessPointer( TYPE *const & ptr, UINT numArrayElements, const SObjInfo& containingObj )
	{
		AssertPtr(ptr);
		AssertX( !m_collectedPointers.Contains(ptr), "Circular links not supported" );
		m_collectedPointers.Add(ptr);

		// address of the pointer embedded in the structure
		const void* pointerAddress = &ptr;

		// offset of the pointer relative to the start of the containing structure
		const mxULong relativePointerOffset = mxGetByteOffset( containingObj.ptr, pointerAddress );

		// offset of the pointer relative to the start of the memory block
		const mxULong absolutePointerOffset = containingObj.offset + relativePointerOffset;

		// data should start at aligned addresses
		SeekAlignedOffset(m_stream);

		// offset of the data to which the pointer points relative to the start of the memory block
		const mxULong pointedDataOffset = m_stream.Tell();

		*(mxULong*)(this->GetBaseAddress() + absolutePointerOffset) = pointedDataOffset;

		//DBGOUT("Saving pointer [%u] (abs: %u, rel: %u) to %u (%u elements)\n",
		//	(UINT)m_fixUpTable.pointers.Num(), (UINT)absolutePointerOffset, (UINT)relativePointerOffset, (UINT)pointedDataOffset, (UINT)numArrayElements);

		{
			SPtr & newLink = m_fixUpTable.pointers.Add();
			newLink.ptrOffset = absolutePointerOffset;
			newLink.dataOffset = pointedDataOffset;
		}

		// write the data to which the pointer points
		const SizeT sizeOfElem = sizeof ptr[0];
		m_stream.Write( ptr, sizeOfElem * numArrayElements );

		for( UINT iElem = 0; iElem < numArrayElements; iElem++ )
		{
			const TYPE& rElem = ptr[ iElem ];

			SObjInfo	objData;
			objData.ptr = &rElem;
			objData.offset = pointedDataOffset + iElem * sizeOfElem;

			this->ProcessElement( rElem, objData );
		}
	}

private:

	// used for arrays of non-primitive types
	template< typename TYPE >
	inline void ProcessElement( const TYPE & o, const SObjInfo& containingObj )
	{
		o.CollectPointers( *this, containingObj );
	}
};

/*
-----------------------------------------------------------------------------
	Loader

	doesn't support polymorphic types
	and circular links
-----------------------------------------------------------------------------
*/
struct Loader
{
	InPlaceMemoryReader	m_stream;

public:
	inline Loader( void* rawMem, SizeT memSize )
		: m_stream( rawMem, memSize )
	{
	}

	// loads the object from the given memory block
	template< typename TYPE >
	inline void LoadObject( TYPE *& o )
	{
		Assert( m_stream.Tell() == 0 );

		FixUpTable	fixUpTable;
		m_stream >>	fixUpTable;

		// object data starts at aligned offset
		SeekAlignedOffset(m_stream);

		BYTE* baseAddress = m_stream.GetPtr();
		Assert(IS_ALIGNED_BY(baseAddress,DATA_ALIGNMENT));

		o = new(baseAddress) TYPE(_FinishedLoadingFlag());

		const UINT numPointers = fixUpTable.pointers.Num();
		for( UINT iLink = 0; iLink < numPointers; iLink++ )
		{
			const SPtr & theLink = fixUpTable.pointers[ iLink ];

			//DBGOUT("Restoring pointer [%u] at %u to %u\n",
			//	iLink, (UINT)theLink.ptrOffset, (UINT)theLink.dataOffset );

			*(mxULong*)(baseAddress + theLink.ptrOffset) = (mxULong)(baseAddress + theLink.dataOffset);
		}
	}
};

}//InPlace


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// parameters passed to the object after its deserialization
//struct SPostLoadContext
//{
//};

/*
-----------------------------------------------------------------------------
	SObjectPtr

	wrapper around pointer to a polymorphics object
-----------------------------------------------------------------------------
*/
struct SObjectPtr : public SBaseType
{
	TPtr< AObject >		o;

public:
	mxDECLARE_CLASS( SObjectPtr, SBaseType );
	mxDECLARE_REFLECTION;
};

/*
-----------------------------------------------------------------------------
	AObjectWriter
-----------------------------------------------------------------------------
*/
class AObjectWriter
{
public:
	template< typename KLASS >
	void SaveObject( const KLASS& o )
	{
		//o.PreSave();
		this->Serialize( &o, KLASS::StaticClass() );
	}

	virtual void Serialize( const void* o, const mxType& typeInfo ) = 0;

	virtual ~AObjectWriter() {}
};

/*
-----------------------------------------------------------------------------
	AObjectReader

	object loader
-----------------------------------------------------------------------------
*/
class AObjectReader
{
public:
	template< typename KLASS >
	void LoadObject( KLASS & o )
	{
		this->Deserialize( &o, KLASS::StaticClass() );

		o.PostLoad();
	}

	// NOTE: assumes that the memory for the object is already allocated
	virtual void Deserialize( void * o, const mxType& typeInfo ) = 0;

	virtual ~AObjectReader() {}
};

/*
=======================================================================

	Serialization of arbitrary types via reflection.

=======================================================================
*/

/*
-----------------------------------------------------------------------------
	BinaryObjectWriter

	binary object serializer
-----------------------------------------------------------------------------
*/
class BinaryObjectWriter : public AObjectWriter
{
	AStreamWriter &	m_stream;

public:
	BinaryObjectWriter( AStreamWriter& stream );
	~BinaryObjectWriter();

	//=-- AObjectWriter
	virtual void Serialize( const void* o, const mxType& typeInfo ) override;
};

/*
-----------------------------------------------------------------------------
	BinaryObjectReader

	binary object loader
-----------------------------------------------------------------------------
*/
class BinaryObjectReader : public AObjectReader
{
	 AStreamReader &	m_stream;

public:
	BinaryObjectReader( AStreamReader& stream );
	~BinaryObjectReader();

	//=-- AObjectReader
	virtual void Deserialize( void * o, const mxType& typeInfo ) override;
};

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
