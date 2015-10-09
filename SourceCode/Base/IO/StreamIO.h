/*
=============================================================================
	File:	StreamIO.h
	Desc:
=============================================================================
*/

#ifndef __MX_STREAM_IO_H__
#define __MX_STREAM_IO_H__

mxNAMESPACE_BEGIN


//--------------------------------------------------------------//
//	Basic I/O.
//--------------------------------------------------------------//

//
//	AStreamReader
//
class AStreamReader : public DbgNamedObject
{
public:

	// Return the size of data (length of the file if this is a file), in bytes.
	virtual SizeT GetSize() const = 0;

	// Read the given number of bytes into the buffer and return the number of bytes actually read.
	//
	virtual SizeT Read( void *pBuffer, SizeT numBytes ) = 0;

	inline void SerializeMemory( void *pDest, const SizeT numBytes )
	{
		this->Read( pDest, numBytes );
	}

	template< typename TYPE >
	inline AStreamReader & Unpack( TYPE &value )
	{
		this->Read( &value, sizeof(TYPE) );
		return *this;
	}

	template< typename TYPE >
	inline void SerializeArray( TYPE * a, UINT count )
	{
		if( TypeTrait<TYPE>::IsPlainOldDataType )
		{
			if( count ) {
				this->SerializeMemory( a, count * sizeof a[0] );
			}
		}
		else
		{
			for( UINT i = 0; i < count; i++ )
			{
				*this >> a[i];
			}
		}
	}

protected:
	inline AStreamReader() {}
	virtual ~AStreamReader();

private:
	PREVENT_COPY(AStreamReader);
};

//
//	AStreamWriter
//
class AStreamWriter : public DbgNamedObject
{
public:

	// Outputs raw data.
	// Writes to the stream and returns the number of bytes written.
	//
	virtual SizeT Write( const void* pBuffer, SizeT numBytes ) = 0;

	inline void SerializeMemory( const void *pSrc, const SizeT numBytes )
	{
		this->Write( pSrc, numBytes );
	}

	template< typename TYPE >
	inline AStreamWriter & Pack( const TYPE& value )
	{
		this->Write( &value, sizeof(TYPE) );
		return *this;
	}

	template< typename TYPE >
	inline void SerializeArray( TYPE * a, UINT count )
	{
		if( TypeTrait<TYPE>::IsPlainOldDataType )
		{
			if( count ) {
				this->SerializeMemory( a, count * sizeof a[0] );
			}
		}
		else
		{
			for( UINT i = 0; i < count; i++ )
			{
				*this << a[i];
			}
		}
	}

protected:
	inline AStreamWriter() {}
	virtual ~AStreamWriter();

private:
	PREVENT_COPY(AStreamWriter);
};

struct NullStreamWriter : public AStreamWriter
{
	virtual SizeT Write( const void* pBuffer, SizeT numBytes ) override
	{
		(void)pBuffer;
		return numBytes;
	}
};

//
//	mxStreamWriter_CountBytes
//
class mxStreamWriter_CountBytes : public AStreamWriter
{
	SizeT	m_bytesWritten;

public:
	mxStreamWriter_CountBytes();
	~mxStreamWriter_CountBytes();

	virtual SizeT Write( const void* pBuffer, SizeT numBytes );

	inline SizeT NumBytesWritten() const { return m_bytesWritten; }
};

//
//	mxStreamReader_CountBytes
//
class mxStreamReader_CountBytes : public AStreamReader
{
	AStreamReader &	m_reader;

	SizeT	m_bytesRead;

public:
	mxStreamReader_CountBytes( AStreamReader & stream );
	~mxStreamReader_CountBytes();

	virtual SizeT Read( void *pBuffer, SizeT numBytes );

	inline SizeT NumBytesRead() const { return m_bytesRead; }
};

//---------------------------------------------------------------------------
//	Helper macros
//---------------------------------------------------------------------------



//#define DECLARE_STREAMABLE( typeName )	\
//	friend AStreamWriter& operator << ( AStreamWriter& stream, const typeName& o );	\
//	friend AStreamReader& operator >> ( AStreamReader& stream, typeName& o );
//



#define mxIMPLEMENT_FUNCTION_READ_SINGLE( typeName, funcName )	\
	FORCEINLINE typeName funcName( AStreamReader & stream )	\
	{	\
		typeName value;	\
		stream.Unpack(value);	\
		return value;	\
	}

#define mxIMPLEMENT_FUNCTION_WRITE_SINGLE( typeName, funcName )	\
	FORCEINLINE void funcName( AStreamWriter & stream, const typeName& value )	\
	{	\
		stream.Pack(value);	\
	}




mxIMPLEMENT_FUNCTION_READ_SINGLE(INT8,ReadInt8);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(INT8,WriteInt8);

mxIMPLEMENT_FUNCTION_READ_SINGLE(UINT8,ReadUInt8);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(UINT8,WriteUInt8);


mxIMPLEMENT_FUNCTION_READ_SINGLE(INT16,ReadInt16);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(INT16,WriteInt16);

mxIMPLEMENT_FUNCTION_READ_SINGLE(UINT16,ReadUInt16);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(UINT16,WriteUInt16);


mxIMPLEMENT_FUNCTION_READ_SINGLE(INT32,ReadInt32);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(INT32,WriteInt32);

mxIMPLEMENT_FUNCTION_READ_SINGLE(UINT32,ReadUInt32);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(UINT32,WriteUInt32);


mxIMPLEMENT_FUNCTION_READ_SINGLE(INT64,ReadInt64);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(INT64,WriteInt64);

mxIMPLEMENT_FUNCTION_READ_SINGLE(UINT64,ReadUInt64);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(UINT64,WriteUInt64);


mxIMPLEMENT_FUNCTION_READ_SINGLE(mxFloat32,ReadF32);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(mxFloat32,WriteF32);


mxIMPLEMENT_FUNCTION_READ_SINGLE(mxFloat64,ReadF64);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(mxFloat64,WriteF64);


mxIMPLEMENT_FUNCTION_READ_SINGLE(__m128,ReadSimdQuad);
mxIMPLEMENT_FUNCTION_WRITE_SINGLE(__m128,WriteSimdQuad);






FORCEINLINE void Skip_1_byte( AStreamReader& stream )
{
	ReadUInt8( stream );
}
FORCEINLINE void Skip_4_bytes( AStreamReader& stream )
{
	ReadUInt32( stream );
}

inline void Skip_N_bytes_Internal( AStreamReader& stream, const UINT numBytes )
{
	BYTE	buffer[1024];
	const UINT num = numBytes / sizeof buffer;
	const UINT rem = numBytes - num * sizeof buffer;

	for( UINT i = 0; i < num; i++ )
	{
		stream.Read( buffer, sizeof buffer );
	}

	if( rem > 0 ) {
		stream.Read( buffer, rem );
	}
}

FORCEINLINE void Skip_N_bytes( AStreamReader& stream, const UINT numBytes )
{
	if( numBytes == 0 ) {
		return;
	}
	Skip_N_bytes_Internal( stream, numBytes );
}

template< UINT numBytes >
FORCEINLINE void Skip_N_bytes( AStreamReader& stream )
{
	mxSTATIC_ASSERT( numBytes > 0 && numBytes < 1024 );	// don't overflow stack
	char	buffer[ numBytes ];
	stream.Read( buffer, numBytes );
}



void EncodeString_Xor( const char* src, SizeT size, UINT key, char *dest );
void DecodeString_Xor( const char* src, SizeT size, UINT key, char *dest );


void WriteEncodedString( AStreamWriter & stream, const char* src, SizeT size, UINT seed );
void ReadDecodedString( AStreamReader & stream, char* dest, UINT seed );


enum EArchiveMode
{
	AM_Save,
	AM_Load,
};

// metadata
class ArchiveInfo;

// for serializing asset references
class ObjectGUID;

/*
-----------------------------------------------------------------------------
	mxArchive

	used for binary serialization
-----------------------------------------------------------------------------
*/
class mxArchive
{
public:

	// serialization of POD objects
	virtual void SerializeMemory( void* ptr, SizeT size ) = 0;

	// serialization of references to resource objects
	virtual void SerializeResourceGuid( ObjectGUID & resourceGuid )
	{Unimplemented;}


	// serialization of non-POD objects
	virtual void SerializePtr( AObject *& o )
	{Unimplemented;}

	// registers a top-level object
	virtual void InsertRootObject( AObject* o )
	{Unimplemented;}


	virtual void GetInfo( ArchiveInfo* info )
	{Unimplemented;}

public:
	virtual AStreamWriter* IsWriter() {return nil;}
	virtual AStreamReader* IsReader() {return nil;}

	inline bool IsWriting()
	{
		return this->IsWriter() != nil;
	}
	inline bool IsReading()
	{
		return this->IsReader() != nil;
	}

protected:
	virtual ~mxArchive()
	{}
};

// (de-)serializes from/to archive via << and >> stream operators
//
template< typename T >
inline mxArchive& Serialize_ArcViaStreams( mxArchive& archive, T & o )
{
	if( AStreamWriter* stream = archive.IsWriter() )
	{
		*stream << o;
	}
	if( AStreamReader* stream = archive.IsReader() )
	{
		*stream >> o;
	}
	//Unreachable;
	return archive;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< typename T >
mxArchive& TSerializeArray( mxArchive & archive, T* a, UINT num )
{
	if( TypeTrait< T >::IsPlainOldDataType )
	{
		if( num > 0 ) {
			archive.SerializeMemory( a, num * sizeof a[0] );
		}
	}
	else
	{
		for( UINT i = 0; i < num; i++ )
		{
			archive && a[i];
		}
	}
	return archive;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class S, class T, const SizeT size >
void TSerializeArray( S& s, T (&a)[size] )
{
	mxSTATIC_ASSERT( size > 0 );
	if( TypeTrait< T >::IsPlainOldDataType )
	{
		s.SerializeMemory( a, size * sizeof T );
	}
	else
	{
		for( UINT i = 0; i < size; i++ )
		{
			s & a[i];
		}
	}
	return s;
}

/*
-----------------------------------------------------------------------------
	mxTextStream
-----------------------------------------------------------------------------
*/
struct mxTextStream
{
	virtual ~mxTextStream() {}

	virtual void Print( const char* str, UINT length ) = 0;

	void Printf( const char* fmt, ... );
	mxTextStream& NewLine();

	template< SizeT N >
	mxTextStream& operator << ( const char (&str)[N] )
	{
		this->Print( str, N );
		return *this;
	}
};

mxTextStream & operator << ( mxTextStream & rStream, const INT i );
mxTextStream & operator << ( mxTextStream & rStream, const UINT i );
mxTextStream & operator << ( mxTextStream & rStream, const FLOAT f );
mxTextStream & operator << ( mxTextStream & rStream, const DOUBLE f );



/*
-----------------------------------------------------------------------------
	mxDebug

	The mxDebug class provides an output stream for debugging information.
	mxDebug is used whenever the developer needs to write out
	debugging or tracing information to a device, file, string or console.
-----------------------------------------------------------------------------
*/
struct mxDebug : mxTextStream
{
	virtual void Print( const char* str, UINT length ) override;
};

extern mxDebug	dbgout;

mxNAMESPACE_END

#endif // !__MX_STREAM_IO_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
