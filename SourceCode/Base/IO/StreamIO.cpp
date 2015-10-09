/*
=============================================================================
	File:	StreamIO.cpp
	Desc:	
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

/*================================
	AStreamReader
================================*/

AStreamReader::~AStreamReader()
{
#if MX_DEBUG
	DBGOUT("~ Stream Reader: '%s'\n",this->DbgGetName());
#endif // MX_DEBUG
}

/*================================
	AStreamWriter
================================*/

AStreamWriter::~AStreamWriter()
{
#if MX_DEBUG
	DBGOUT("~ Stream Writer: '%s'\n",this->DbgGetName());
#endif // MX_DEBUG
}

/*================================
	mxStreamWriter_CountBytes
================================*/

mxStreamWriter_CountBytes::mxStreamWriter_CountBytes()
{
	this->DbgSetName( "mxStreamWriter_CountBytes" );
	m_bytesWritten = 0;
}

mxStreamWriter_CountBytes::~mxStreamWriter_CountBytes()
{

}

SizeT mxStreamWriter_CountBytes::Write( const void* pBuffer, SizeT numBytes )
{
	m_bytesWritten += numBytes;
	return numBytes;
}

/*================================
	mxStreamReader_CountBytes
================================*/

mxStreamReader_CountBytes::mxStreamReader_CountBytes( AStreamReader & stream )
	: m_reader( stream )
{
	this->DbgSetName( "mxStreamReader_CountBytes" );
	m_bytesRead = 0;
}

mxStreamReader_CountBytes::~mxStreamReader_CountBytes()
{

}

SizeT mxStreamReader_CountBytes::Read( void *pBuffer, SizeT numBytes )
{
	const SizeT bytesRead = m_reader.Read( pBuffer, numBytes );
	m_bytesRead += bytesRead;
	return bytesRead;
}



/*
-----------------------------------------------------------------------------
	mxTextStream
-----------------------------------------------------------------------------
*/
void mxTextStream::Printf( const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	this->Print( buffer, mxStrLenAnsi(buffer) );
}
mxTextStream& mxTextStream::NewLine()
{
	static ConstCharPtr	NEW_LINE("\n");
	this->Print( NEW_LINE.ToChars(), NEW_LINE.Length() );
	return *this;
}

mxTextStream & operator << ( mxTextStream & rStream, const INT i )
{
	rStream.Printf("%d",i);
	return rStream;
}
mxTextStream & operator << ( mxTextStream & rStream, const UINT i )
{
	rStream.Printf("%u",i);
	return rStream;
}
mxTextStream & operator << ( mxTextStream & rStream, const FLOAT f )
{
	rStream.Printf("%f",f);
	return rStream;
}
mxTextStream & operator << ( mxTextStream & rStream, const DOUBLE f )
{
	rStream.Printf("%lf",f);
	return rStream;
}




/*================================
	mxDebug
================================*/

mxDebug	dbgout;

void mxDebug::Print( const char* str, UINT length )
{
#if MX_DEBUG
	//::OutputDebugStringA( buffer );
	GetGlobalLogger().Log( LL_Info, str, length );
	::printf("%s",str);
#endif // MX_DEBUG
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
