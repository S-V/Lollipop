/*
=============================================================================
	File:	Log.cpp
	Desc:	Log.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

/*================================
		mxTextWriter
================================*/

mxTextWriter::mxTextWriter( AStreamWriter& stream )
	: mStream( stream )
{
}

mxTextWriter::~mxTextWriter()
{
}

void mxTextWriter::Put( const char* s, UINT length )
{
	mStream.Write( s, length );
}

mxTextWriter& mxTextWriter::operator<< (const void* p)
{
	Putf("%p",p);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (mxBool b)
{
	return (*this) << (b ? ConstCharPtr("true") : ConstCharPtr("false"));
}

mxTextWriter& mxTextWriter::operator<< (signed char c)
{
	Putf("%c",c);
	return *this;
}
/*
mxTextWriter& mxTextWriter::operator<< (unsigned char c)
{
	mStream.Write( &c, sizeof(c) );
	return *this;
}
*/
mxTextWriter& mxTextWriter::operator<< (const char* s)
{
	Putf("%s",s);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (const signed char* s)
{
	Putf("%s",s);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (const unsigned char* s)
{
	Putf("%s",s);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (short s)
{
	Putf("%d",(INT)s);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (unsigned short s)
{
	Putf("%u",(UINT)s);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (int i)
{
	Putf("%d",i);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (unsigned int u)
{
	Putf("%u",u);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (float f)
{
	Putf("%f",f);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (double d)
{
	Putf("%lf",d);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (INT64 i)
{
	Putf("%ld",i);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (UINT64 u)
{
	Putf("%lu",u);
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (const String& str)
{
	Put( str.ToChars(), str.GetDataSize() );
	return *this;
}

mxTextWriter& mxTextWriter::operator<< (const ConstCharPtr& str)
{
	Put( str.ToChars(), str.GetDataSize() );
	return *this;
}

void mxTextWriter::Putf( const char *fmt, ...)
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, fmt, length );
	Put( buffer, length );
}

/*================================
			mxLogger
================================*/



mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
