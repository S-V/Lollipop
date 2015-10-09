#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/Serialization.h>
#include <Core/Serialization/PackageFile.h>

enum { MX_PACKAGE_VERSION = 0 };
enum { PACK_FILE_SIGNATURE = MAKEFOURCC('P','A','C','K') };

// 32-bit integers will suffice
static const SizeT MX_MAX_PACKAGE_SIZE	= MAX_UINT32;

static const SizeT MX_MAX_RESOURCE_SIZE	= (mxGIBIBYTE);

/*
-----------------------------------------------------------------------------
	PakFileHeader
-----------------------------------------------------------------------------
*/
PakFileHeader::PakFileHeader( U4 fourCC )
{
	session.fourCC = fourCC;

	whenCreated = FileTime::CurrentTime();
	lastModified = FileTime::CurrentTime();

	totalSize = 0;
	crc32 = 0;
	md5 = 0;
}

bool PakFileHeader::Matches( const PakFileHeader& other ) const
{
	return mxSessionInfo::AreCompatible( session, other.session );
}

/*
-----------------------------------------------------------------------------
	OptimizedPakFile
-----------------------------------------------------------------------------
*/
OptimizedPakFile::OptimizedPakFile()
	: m_fileReader( _NoInit )
{

}

OptimizedPakFile::OptimizedPakFile( const char* fileName )
	: m_fileReader( fileName )
{
	m_fileReader >> m_header;
	m_fileReader >> m_entries;
}

OptimizedPakFile::~OptimizedPakFile()
{
	if( this->IsOpen() )
	{
		this->Close();
	}
}

bool OptimizedPakFile::IsOpen() const
{
	return m_fileReader.IsOpen();
}

void OptimizedPakFile::Close()
{
	return m_fileReader.Close();
}

PakFileHandle OptimizedPakFile::OpenFile( ObjectGUIDArg fileGuid )
{
	if( m_entries.IsValidIndex( fileGuid.v ) ) {
		return fileGuid.v;
	}
	mxDBG_UNREACHABLE;
	return BadPakFileHandle;
}

void OptimizedPakFile::CloseFile( PakFileHandle file )
{
	Assert( m_entries.IsValidIndex( file ) );
	mxUNUSED(file);
}

UINT OptimizedPakFile::GetFileSize( PakFileHandle file )
{
	CHK_VRET_X_IF_NOT( m_entries.IsValidIndex( file ), 0 );
	return m_entries[ file ].uncompressedSize;
}

SizeT OptimizedPakFile::ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead )
{
	CHK_VRET_X_IF_NOT( m_entries.IsValidIndex( file ), 0 );

	PakFileEntry& entry = m_entries[ file ];

	const UINT offset = Pak_GetFileEntryOffset( entry );
	const SizeT dataSize = entry.uncompressedSize;
	Assert(bytesToRead <= dataSize);

	m_fileReader.Seek( offset );
	m_fileReader.Read( buffer, dataSize );

	return dataSize;
}




/*
-----------------------------------------------------------------------------
	HashedPakFile
-----------------------------------------------------------------------------
*/
HashedPakFile::HashedPakFile()
	: m_fileReader( _NoInit )
{

}

HashedPakFile::HashedPakFile( const char* fileName )
	: m_fileReader( _NoInit )
{
	this->Open( fileName );
}

HashedPakFile::~HashedPakFile()
{
	if( this->IsOpen() )
	{
		this->Close();
	}
}

bool HashedPakFile::Open( const char* fileName )
{
	CHK_VRET_FALSE_IF_NOT(m_fileReader.Open( fileName ));
	m_fileReader >> m_header;
	m_fileReader >> m_entries;

	{
		const TMap< ObjectGUID, PakFileEntry >::PairsArray& pairs = m_entries.GetPairs();
		for( UINT i=0; i < pairs.Num(); i++ )
		{
			char	tmp[32];
			pairs[i].key.ToChars(tmp);
			DBGOUT( "GUID[%u] = %s\n", i, tmp );
		}
	}

	return true;
}

bool HashedPakFile::IsOpen() const
{
	return m_fileReader.IsOpen();
}

void HashedPakFile::Close()
{
	return m_fileReader.Close();
}

PakFileHandle HashedPakFile::OpenFile( ObjectGUIDArg fileGuid )
{
	const UINT index = m_entries.FindKeyIndex( fileGuid );
	if( index != INDEX_NONE ) {
		return index;
	}
	mxDBG_UNREACHABLE;
	return BadPakFileHandle;
}

void HashedPakFile::CloseFile( PakFileHandle file )
{
	mxUNUSED(file);
}

UINT HashedPakFile::GetFileSize( PakFileHandle file )
{
	const PakFileEntry* pEntry = this->FindEntryByIndex( file );
	CHK_VRET_X_IF_NIL( pEntry, 0 );
	return pEntry->uncompressedSize;
}

SizeT HashedPakFile::ReadFile( PakFileHandle file, UINT startOffset, void *buffer, UINT bytesToRead )
{
	const PakFileEntry* pEntry = this->FindEntryByIndex( file );
	CHK_VRET_X_IF_NIL( pEntry, 0 );

	const UINT offset = Pak_GetFileEntryOffset( *pEntry );
	const SizeT dataSize = pEntry->uncompressedSize;
	Assert(bytesToRead <= dataSize);

	m_fileReader.Seek( offset );
	m_fileReader.Read( buffer, dataSize );

	return dataSize;
}

const PakFileEntry* HashedPakFile::FindEntryByIndex( PakFileHandle index ) const
{
	const TMap< ObjectGUID, PakFileEntry >::PairsArray& pairs = m_entries.GetPairs();
	if( pairs.IsValidIndex( index ) ) {
		return &pairs[ index ].value;
	}
	return nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
