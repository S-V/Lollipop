/*
=============================================================================
	File:	StackString.h
	Desc:	Fixed-size string class.
=============================================================================
*/

#ifndef __MX_STACK_STRING_H__
#define __MX_STACK_STRING_H__

mxNAMESPACE_BEGIN

struct TFixedStringBase
{
	enum
	{
		// settings common to all TStackString's
		MIN_SIZE = 4,	// minimum allowed size of TStackString
		MAX_SIZE = 256,	// maximum allowed size of TStackString
	};
};

//
// Fixed-size string.
// Stores a length and maintains a terminating null byte.
// Storing the string length as byte limits the maximum string length to 255.
//
template< UINT SIZE >	// <= size of string in bytes
class TStackString : public TFixedStringBase
{
	char	mData[ SIZE ];	// first byte contains the length, last byte can never be non-null

public:

	typedef TStackString< SIZE > THIS_TYPE;

	// maximum capacity of this string
	// (NUMBER_OF(data) - 2), 2 items reserved for length and null terminator.
	enum { CAPACITY = SIZE - 2 };

	typedef char CHARS_ARRAY[ CAPACITY ];

public:

	FORCEINLINE
	TStackString()
	{
		this->Clear();
	}

	FORCEINLINE
	explicit TStackString( ENoInit )
	{}

	FORCEINLINE
	explicit TStackString( EInitZero )
	{
		this->ZeroOut();
	}

	FORCEINLINE
	TStackString( const TStackString& other )
	{
		*this = other;
	}

	template< UINT N >
	FORCEINLINE
	TStackString( const char (&s)[N] )
	{
		mxSTATIC_ASSERT( N <= CAPACITY );
		this->Set( s, N-1 );
	}

	FORCEINLINE
	TStackString( EInitSlow, const char* s )
	{
		this->Set( s, mxStrLenAnsi(s) );
	}

	FORCEINLINE
	TStackString( const char* s, UINT length )
	{
		this->Set( s, length );
	}

	FORCEINLINE
	TStackString( const ConstCharPtr& s )
	{
		this->Set( s.ToChars(), s.Length() );
	}

	FORCEINLINE
	explicit TStackString( const char c )
	{
		mData[0] = 1;
		mData[1] = c;
	}

	FORCEINLINE
	operator const ConstCharPtr () const
	{
		return this->ToConstCharPtr();
	}

	FORCEINLINE
	const ConstCharPtr ToConstCharPtr() const
	{
		return ConstCharPtr( this->ToChars(), this->Length() );
	}

/*
	FORCEINLINE
	operator char* ()
	{
		return ToChars();
	}
	FORCEINLINE
	operator const char* () const
	{
		return ToChars();
	}
*/

	FORCEINLINE
	char* ToChars()
	{
		return mData+1;
	}
	FORCEINLINE
	const char* ToChars() const
	{
		return mData+1;
	}

	FORCEINLINE
	const char * c_str() const { return ToChars(); }

	FORCEINLINE
	char* Ptr() { return this->ToChars(); }

	FORCEINLINE
	const char* Ptr() const { return ToChars(); }


	FORCEINLINE
	char& GetLast()
	{
		return ToChars()[ Length() - 1 ];
	}
	FORCEINLINE
	char GetLast() const
	{
		return ToChars()[ Length() - 1 ];
	}

	FORCEINLINE
	char operator [] ( UINT index ) const
	{
		Assert( ( index >= 0 ) && ( index <= Length() ) );
		return ToChars()[ index ];
	}

	FORCEINLINE
	char & operator [] ( UINT index )
	{
		Assert( ( index >= 0 ) && ( index <= Length() ) );
		return ToChars()[ index ];
	}


	FORCEINLINE
	TStackString& operator = ( const TStackString& other )
	{
		this->Set( other.ToChars(), other.Length() );
		return *this;
	}


/* -- intentionally disabled. Use Set() or templated version below.
	FORCEINLINE
	TStackString& operator = ( const char* s )
	{
		Set( s, mxStrLenAnsi(s) );
		return *this;
	}
*/

	// If the source buffer is bigger than this string's capacity, it won't compile at all.
	template< UINT N >
	FORCEINLINE
	TStackString& operator = ( const char (&s)[N] )
	{
		mxSTATIC_ASSERT( N <= CAPACITY );
		this->Set( s, N-1 );
		return *this;
	}

	template< typename SomeType >
	FORCEINLINE
	TStackString& operator = ( const SomeType& rhs )
	{
		this->Set( rhs.ToChars(), rhs.Length() );
		return *this;
	}

	FORCEINLINE
	TStackString& operator += ( char c )
	{
		this->Append( c );
		return *this;
	}

	// This is slow but convenient.
	FORCEINLINE
	TStackString& operator += ( const char* s )
	{
		this->Append( s );
		return *this;
	}

	FORCEINLINE
	TStackString& operator += ( const ConstCharPtr& s )
	{
		this->Append( s );
		return *this;
	}

	FORCEINLINE
	TStackString& operator += ( const TStackString& other )
	{
		this->Append( other );
		return *this;
	}


	// case sensitive comparison
	bool operator == ( const TStackString& other ) const
	{
		return Equals( other.ToChars(), other.Length() );
	}

	bool operator != ( const TStackString& other ) const { return !(*this == other); }


	template< UINT N >
	FORCEINLINE
	bool operator == ( const char (&s)[N] ) const
	{
		UINT numChars = Min( this->GetLength(), N );
		return this->Equals( s, numChars );
	}

	template< UINT N >
	FORCEINLINE
	bool operator != ( const char (&s)[N] ) const { return !(*this == s); }

	FORCEINLINE CHARS_ARRAY& AsArray() { return *c_cast(CHARS_ARRAY*)(mData+1); }

	FORCEINLINE
	void Set( const ConstCharPtr& s )
	{
		this->Set( s.ToChars(), s.Length() );
	}

	FORCEINLINE
	void Set( const char* RESTRICT_PTR(s), UINT length )
	{
		Assert( ToChars() != s );
	#if MX_BOUNDS_CHECKS
		AlwaysAssertX( length <= GetCapacity(), "Buffer overflow" );
	#endif
		UINT bytesToCopy = Min( GetCapacity(), length );
		MemCopy( ToChars(), s, bytesToCopy );
		mData[ 0 ] = bytesToCopy;
		this->ToChars()[ bytesToCopy ] = 0;
	}

	FORCEINLINE
	void SetString( const char* s )
	{
		Assert( ToChars() != s );
		this->Set( s, mxStrLenAnsi(s) );
	}


	FORCEINLINE
	void Append( char c )
	{
		UINT prevLength = Length();
	#if MX_BOUNDS_CHECKS
		AlwaysAssertX( prevLength + 1 <= GetCapacity(), "Buffer overflow" );
	#endif
		UINT offset = (prevLength + 1 <= GetCapacity()) ? 1 : 0;
		mData[ 0 ] = prevLength + offset;
		mData[ Length() ] = c;
		this->ToChars()[ prevLength + offset ] = '\0';
	}

	FORCEINLINE
	void Append( const char* RESTRICT_PTR(s), UINT length )
	{
		Assert( ToChars() != s );
		UINT prevLength = Length();
	#if MX_BOUNDS_CHECKS
		AlwaysAssertX( length + prevLength <= GetCapacity(), "Buffer overflow" );
	#endif
		UINT bytesToCopy = Min( GetCapacity() - prevLength, length );

		MemCopy( ToChars() + prevLength, s, bytesToCopy );
		mData[ 0 ] = prevLength + bytesToCopy;
		this->ToChars()[ prevLength + bytesToCopy ] = '\0';
	}

	FORCEINLINE
	void Append( const char* RESTRICT_PTR(s) )
	{
		this->Append( s, mxStrLenAnsi(s) );
	}

	FORCEINLINE
	void Append( const ConstCharPtr& s )
	{
		this->Append( s.ToChars(), s.Length() );
	}

	FORCEINLINE
	void Append( const TStackString& other )
	{
		this->Append( other.ToChars(), other.Length() );
	}

	FORCEINLINE
	void ReplaceChar( char old, char nw )
	{
		mxReplaceCharANSI( ToChars(), Length(), old, nw );
	}

	// case-sensitive comparison

	FORCEINLINE
	bool Equals( const char* s ) const
	{
		return mxEqualANSIStrings( ToChars(), Length(), s, mxStrLenAnsi(s) );
	}

	FORCEINLINE
	bool Equals( const char* s, UINT length ) const
	{
		return mxEqualANSIStrings( ToChars(), Length(), s, length );
	}

	template< UINT N >
	FORCEINLINE
	bool Equals( const char (&s)[N] ) const
	{
		return mxEqualANSIStrings( ToChars(), Length(), s, N );
	}

	void VARARGS Format( const char* fmt, ... ) mxCHECK_PRINTF_ARGS
	{
		char * dest = ToChars();
		UINT maxChars = GetCapacity() + 1;

		va_list argList;
		va_start( argList, fmt );
		UINT newLength = mxSafeGetVarArgsANSI( dest, maxChars, fmt, argList );
		va_end( argList );

		mData[0] = newLength;
	//	dest[ GetLength() ] = 0;	// mxSafeGetVarArgsANSI() appends NULL
	}

	FORCEINLINE
	void SetBool( bool value )
	{
		*this = value ? "true" : "false";
	}

	FORCEINLINE
	void SetInt( int value )
	{
		this->Format( "%d", value );
	}

	FORCEINLINE
	void SetFloat( float value )
	{
		this->Format( "%f", value );
	}

	FORCEINLINE
	void StripFileExtension()
	{
		UINT newLength = mxStripFileExtensionANSI( this->ToChars(), this->Length() );
		mData[0] = newLength;
	}

	FORCEINLINE
	void StripLeading( const char* s, UINT l )
	{
		UINT newLength = mxStripLeadingANSI( this->ToChars(), this->Length(), s, l );
		mData[0] = newLength;
	}

	FORCEINLINE
	void StripLeadingCaseless( const char* s, UINT l )
	{
		UINT newLength = mxStripLeadingCaselessANSI( this->ToChars(), this->Length(), s, l );
		mData[0] = newLength;
	}

	FORCEINLINE
	void StripTrailing( const char* s, UINT l )
	{
		UINT newLength = mxStripTrailingANSI( this->ToChars(), this->Length(), s, l );
		mData[0] = newLength;
	}

	FORCEINLINE
	void StripTrailingCaseless( const char* s, UINT l )
	{
		UINT newLength = mxStripTrailingCaselessANSI( this->ToChars(), this->Length(), s, l );
		mData[0] = newLength;
	}

	FORCEINLINE
	void StripTrailing( const char what )
	{
		UINT newLength = mxStripTrailingANSI( this->ToChars(), this->Length(), what );
		mData[0] = newLength;
	}

	void ToUpper()
	{
		const UINT length = this->Length();
		char* chars = this->ToChars();
		for( UINT i = 0; i < length; i++ )
		{
			chars[i] = mxToUpper( chars[i] );
		}
	}

	FORCEINLINE
	bool IsEmpty() const
	{
		return !this->Length();
	}

	FORCEINLINE
	void Clear()
	{
		mData[0] = 0;
		mData[1] = 0;
	}

	FORCEINLINE
	void Empty() { Clear(); }

	FORCEINLINE
	void ZeroOut()
	{
		ZERO_OUT(mData);
	}


	FORCEINLINE
	SizeT GetCapacity() const
	{
		return CAPACITY;
	}

	// Returns the length of this string excluding the null terminator.
	// The returned value is always <= GetCapacity().
	FORCEINLINE
	UINT Length() const
	{
		return mData[0];
	}

	FORCEINLINE
	UINT GetLength() const { return Length(); }

	// Returns the size of one element, in bytes.
	FORCEINLINE
	SizeT GetItemSize() const
	{
		return sizeof(mData[0]);
	}

	// Returns the total size of stored elements, in bytes.
	FORCEINLINE
	SizeT GetDataSize() const
	{
		return Length() * GetItemSize();
	}

	// Returns the amount of reserved memory in bytes (memory allocated for storing the elements).
	FORCEINLINE
	SizeT GetAllocatedMemory() const
	{
		return sizeof(mData);
	}

	// Returns the total amount of occupied memory in bytes.
	FORCEINLINE
	SizeT GetMemoryUsed() const
	{
		return sizeof(mData);
	}

	friend AStreamWriter& operator << ( AStreamWriter& file, const THIS_TYPE& o )
	{
		const U4 length = o.Length();
		file << length;
		if( length > 0 )
		{
			file.Write( o.ToChars(), o.GetDataSize() );
		}
		return file;
	}
	friend AStreamReader& operator >> ( AStreamReader& file, THIS_TYPE& o )
	{
		U4 length;
		file >> length;

		Assert( length < o.GetCapacity() );

		UINT bytesToCopy = Min( o.GetCapacity(), length );
		if( length > 0 )
		{
			file.Read( o.ToChars(), bytesToCopy * o.GetItemSize() );
		}
		o.mData[ 0 ] = bytesToCopy;
		o.ToChars()[ length ] = 0;

		return file;
	}
	friend mxArchive& operator && ( mxArchive & serializer, THIS_TYPE & o )\
	{
		if( AStreamWriter* saver = serializer.IsWriter() )
		{
			*saver << o;
		}
		else
		{
			AStreamReader* loader = serializer.IsReader();
			*loader >> o;
		}
		return serializer;
	}

public:

	// Text formatted data writer.
	class OStream : public AStreamWriter, public mxTextWriter
	{
		THIS_TYPE &	mString;

	public:
		OStream( THIS_TYPE & dest )
			: mxTextWriter( this ), mString( dest )
		{}

		virtual SizeT Write( const void* pBuffer, SizeT numBytes ) override
		{
			const UINT oldLength = mString.Length();
			const char* pStrData = static_cast< const char* >( pBuffer );
			mString.Append( pStrData, numBytes );
			return mString.Length() - oldLength;
		}
	};

	OStream GetWriter()
	{
		return OStream( *this );
	}

private:
	// ideally, this should be put into every method...
	void StaticChecks()
	{
		{mxSTATIC_ASSERT( sizeof(mData[0]) == 1 );}
		{mxSTATIC_ASSERT( sizeof(THIS_TYPE) == SIZE);}
		{mxSTATIC_ASSERT( SIZE >= MIN_SIZE );}
		{mxSTATIC_ASSERT( SIZE <= MAX_SIZE );}
	}
};

template<>
class TStackString<0>
{
private:
	PREVENT_COPY(TStackString<0>);
};
template<>
class TStackString<1>
{
private:
	PREVENT_COPY(TStackString<1>);
};
template<>
class TStackString<2>
{
private:
	PREVENT_COPY(TStackString<2>);
};
template<>
class TStackString<3>
{
private:
	PREVENT_COPY(TStackString<3>);
};

// Pascal-like string (max length - 254 characters)
typedef TStackString< TFixedStringBase::MAX_SIZE >	StackString;

mxNAMESPACE_END

#endif // !__MX_STACK_STRING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
