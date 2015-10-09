/*
=======================================================================
	File:	TextBuffer.h
	Desc:	Text buffer (for string data).
=======================================================================
*/

#ifndef __MX_TEXT_BUFFER_H__
#define __MX_TEXT_BUFFER_H__

mxNAMESPACE_BEGIN




class TextBuffer : public TList< BYTE >
{
public:
	typedef TextBuffer		THIS_TYPE;
	typedef TList< BYTE >	BASE_TYPE;

	typedef BASE_TYPE::ITEM_TYPE	ITEM_TYPE;

public:
	explicit TextBuffer( HMemory hMemoryMgr = EMemHeap::HeapTemp )
		: BASE_TYPE( hMemoryMgr )
	{
	}

	FORCEINLINE char* ToChars()
	{
		return (char*)ToPtr();
	}
	FORCEINLINE const char* ToChars() const
	{
		return (const char*)ToPtr();
	}

	FORCEINLINE UINT Length() const
	{
		return GetDataSize();
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s), UINT length )
	{
		Assert( ToChars() != s );
		this->SetNum( length + 1 );	// reserve one char for null terminator
		MemCopy( this->ToPtr(), s, length );
		this->SetLast(nil);	// null terminator
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s) )
	{
		this->SetString( s, mxStrLenAnsi(s) );
	}

private:
	mxSTATIC_ASSERT(sizeof(char) == sizeof(ITEM_TYPE));
};

template< typename TYPE, SizeT MAX_SIZE >
class FixedTextBuffer : public TStaticList< TYPE, MAX_SIZE >
{
public:
	typedef TextBuffer		THIS_TYPE;
	typedef TList< TYPE >	BASE_TYPE;
	typedef TYPE			ITEM_TYPE;

public:
	explicit FixedTextBuffer()
	{
	}

	FORCEINLINE TYPE* ToChars()
	{
		return (TYPE*)ToPtr();
	}
	FORCEINLINE const TYPE* ToChars() const
	{
		return (const TYPE*)ToPtr();
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s), UINT length )
	{
		Assert( ToChars() != s );
		this->SetNum( length + 1 );	// reserve one char for null terminator
		MemCopy( this->ToPtr(), s, length );
		this->SetLast(nil);	// null terminator
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s) )
	{
		this->SetString( s, mxStrLenAnsi(s) );
	}
};



template< SizeT MAX_SIZE >
class FixedTextBufferANSI : public FixedTextBuffer< ANSICHAR, MAX_SIZE >
{
public:
	typedef TextBuffer		THIS_TYPE;
	typedef FixedTextBuffer	BASE_TYPE;

public:
	FixedTextBufferANSI()
		: BASE_TYPE()
	{}
};



template< SizeT MAX_SIZE >
class FixedTextBufferUNICODE : public FixedTextBuffer< UNICODECHAR, MAX_SIZE >
{
public:
	typedef TextBuffer		THIS_TYPE;
	typedef FixedTextBuffer	BASE_TYPE;

public:
	FixedTextBufferUNICODE()
		: BASE_TYPE()
	{}
};

mxNAMESPACE_END

#endif /* !__MX_TEXT_BUFFER_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
