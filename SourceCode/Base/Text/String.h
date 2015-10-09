/*
=============================================================================
	File:	String.h
	Desc:	String class.
			Swiped from Id Software ( idStr ).
	ToDo:	replace sprintf,strcpy,etc with safer versions; make it thread-safe
=============================================================================
*/

#ifndef __MX_STRING_H__
#define __MX_STRING_H__

#include <Base/Object/Reflection.h>

mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

/*
===============================================================================

	Character string

	Stores a length and maintains a terminating null byte.

	NOTE: all instances of String use a separate memory pool
	and they can ONLY be heap objects (i.e. allocated dynamically),
	the can NOT static objects!

===============================================================================
*/

//
//	String
//
class String
{
public:
						String( void );
						String( const String & other );
						String( const String & other, INT start, INT end );
						String( const char *text );
						String( const char *text, INT start, INT end );
						explicit String( const bool b );
						explicit String( const char c );
						explicit String( const int i );
						explicit String( const unsigned u );
						explicit String( const float f );
						~String( void );

	size_t				Size( void ) const;
	const char *		c_str( void ) const;
	operator 			const char * ( void ) const;
	operator 			const char * ( void );

	char				operator [] ( INT index ) const;
	char &				operator [] ( INT index );

	void				operator = ( const String & other );
	void				operator = ( const char * text );

	friend String		operator + ( const String &a, const String &b );
	friend String		operator + ( const String &a, const char *b );
	friend String		operator + ( const char *a, const String &b );

	friend String		operator + ( const String &a, const float b );
	friend String		operator + ( const String &a, const INT b );
	friend String		operator + ( const String &a, const unsigned b );
	friend String		operator + ( const String &a, const bool b );
	friend String		operator + ( const String &a, const char b );

	String &			operator += ( const String &a );
	String &			operator += ( const char *a );
	String &			operator += ( const float a );
	String &			operator += ( const char a );
	String &			operator += ( const INT a );
	String &			operator += ( const unsigned a );
	String &			operator += ( const bool a );

						// case sensitive compare
	friend bool			operator == ( const String &a, const String &b );
	friend bool			operator == ( const String &a, const char *b );
	friend bool			operator == ( const char *a, const String &b );

						// case sensitive compare
	friend bool			operator != ( const String &a, const String &b );
	friend bool			operator != ( const String &a, const char *b );
	friend bool			operator != ( const char *a, const String &b );

						// case sensitive compare
	INT					Cmp( const char *text ) const;
	INT					Cmpn( const char *text, INT n ) const;
	INT					CmpPrefix( const char *text ) const;

						// case insensitive compare
	INT					Icmp( const char *text ) const;
	INT					Icmpn( const char *text, INT n ) const;
	INT					IcmpPrefix( const char *text ) const;

						// compares paths and makes sure folders come first
	INT					IcmpPath( const char *text ) const;
	INT					IcmpnPath( const char *text, INT n ) const;
	INT					IcmpPrefixPath( const char *text ) const;

	INT					GetLength( void ) const;		
	INT					Allocated( void ) const;
	void				Empty( void );
	bool				IsEmpty( void ) const;
	void				Clear( void );
	void				Append( const char a );
	void				Append( const String &text );
	void				Append( const char *text );
	void				Append( const char *text, INT len );
	void				Insert( const char a, INT index );
	void				Insert( const char *text, INT index );
	void				ToLower( void );
	void				ToUpper( void );
	bool				IsNumeric( void ) const;

	void	Set( const char* s ) { *this = s; }
	void	Set( const char* s, UINT len );

	INT		Length() const { return GetLength(); }
	bool	IsValid() const { return !IsEmpty(); }

	bool				HasLower( void ) const;
	bool				HasUpper( void ) const;
	INT					LengthWithoutColors( void ) const;
	void				CapLength( INT );
	void				Fill( const char ch, INT newlen );

	INT					Find( const char c, INT start = 0, INT end = -1 ) const;
	INT					Find( const char *text, bool casesensitive = true, INT start = 0, INT end = -1 ) const;
	bool				Filter( const char *filter, bool casesensitive ) const;
	INT					Last( const char c ) const;						// return the index to the last occurrence of 'c', returns -1 if not found
	const char *		Left( INT len, String &result ) const;			// store the leftmost 'len' characters in the result
	const char *		Right( INT len, String &result ) const;			// store the rightmost 'len' characters in the result
	const char *		Mid( INT start, INT len, String &result ) const;	// store 'len' characters starting at 'start' in result
	String				Left( INT len ) const;							// return the leftmost 'len' characters
	String				Right( INT len ) const;							// return the rightmost 'len' characters
	String				Mid( INT start, INT len ) const;				// return 'mLength' characters starting at 'start'
	void				StripLeading( const char c );					// strip char from front as many times as the char occurs
	void				StripLeading( const char *str );				// strip String from front as many times as the String occurs
	bool				StripLeadingOnce( const char *str );			// strip String from front just once if it occurs
	void				StripTrailing( const char c );					// strip char from end as many times as the char occurs
	void				StripTrailing( const char *str );				// strip String from end as many times as the String occurs
	bool				StripTrailingOnce( const char *str );			// strip String from end just once if it occurs
	void				Strip( const char c );							// strip char from front and end as many times as the char occurs
	void				Strip( const char *str );						// strip String from front and end as many times as the String occurs
	void				StripTrailingWhitespace( void );				// strip trailing white space characters
	String &			StripQuotes( void );							// strip quotes around String
	void				Replace( const char *old, const char *nw );
	void				ReplaceChar( char old, char nw );
	const char *		ToChars() const;
	char *				ToChars();

	char				GetLast() const { return mLength ? mData[ mLength - 1 ] : '\0'; }

	enum { FILE_HASH_SIZE =	1024 };

	// file name methods
	INT					FileNameHash( void ) const;						// hash key for the filename (skips extension)
	String &			BackSlashesToSlashes( void );					// convert slashes
	String &			SetFileExtension( const char *extension );		// set the given file extension
	String &			StripFileExtension( void );						// remove any file extension
	String &			StripAbsoluteFileExtension( void );				// remove any file extension looking from front (useful if there are multiple .'s)
	String &			DefaultFileExtension( const char *extension );	// if there's no file extension use the default
	String &			DefaultPath( const char *basepath );			// if there's no path use the default
	void				AppendPath( const char *text );					// append a partial path
	String &			StripFilename( void );							// remove the filename from a path
	String &			StripPath( void );								// remove the path from the filename
	void				ExtractFilePath( String &dest ) const;			// copy the file path to another String
	void				ExtractFileName( String &dest ) const;			// copy the filename to another String
	void				ExtractFileBase( String &dest ) const;			// copy the filename minus the extension to another String
	void				ExtractFileExtension( String &dest ) const;		// copy the file extension to another String
	bool				CheckExtension( const char *ext );

	// char * methods to replace library functions
	static INT			GetLength( const char *s );
	static char *		ToLower( char *s );
	static char *		ToUpper( char *s );
	static bool			IsNumeric( const char *s );
	static bool			HasLower( const char *s );
	static bool			HasUpper( const char *s );
	static INT			Cmp( const char *s1, const char *s2 );
	static INT			Cmpn( const char *s1, const char *s2, INT n );
	static INT			Icmp( const char *s1, const char *s2 );
	static INT			Icmpn( const char *s1, const char *s2, INT n );
	static INT			IcmpNoColor( const char *s1, const char *s2 );
	static INT			IcmpPath( const char *s1, const char *s2 );			// compares paths and makes sure folders come first
	static INT			IcmpnPath( const char *s1, const char *s2, INT n );	// compares paths and makes sure folders come first
	static void			Append( char *dest, INT size, const char *src );
	static void			Copynz( char *dest, const char *src, INT destsize );
	static INT			snPrintf( char *dest, INT size, const char *fmt, ... );
	static INT			vsnPrintf( char *dest, INT size, const char *fmt, va_list argptr );
	static INT			FindChar( const char *str, const char c, INT start = 0, INT end = -1 );
	static INT			FindText( const char *str, const char *text, bool casesensitive = true, INT start = 0, INT end = -1 );
	static bool			Filter( const char *filter, const char *name, bool casesensitive );
	static void			StripMediaName( const char *name, String &mediaName );
	static bool			CheckExtension( const char *name, const char *ext );
	static const char *	FloatArrayToString( const float *array, const INT length, const INT precision );

	// hash keys
	static INT			Hash( const char *str );
	static INT			Hash( const char *str, INT length );
	static INT			IHash( const char *str );					// case insensitive
	static INT			IHash( const char *str, INT length );		// case insensitive

	// character methods
	static char			ToLower( char c );
	static char			ToUpper( char c );
	static bool			CharIsPrintable( INT c );
	static bool			CharIsLower( INT c );
	static bool			CharIsUpper( INT c );
	static bool			CharIsAlpha( INT c );
	static bool			CharIsNumeric( INT c );
	static bool			CharIsNewLine( char c );
	static bool			CharIsTab( char c );
	static INT			ColorIndex( INT c );

	friend INT			_sprintf( String &dest, const char *fmt, ... );
	friend INT			_vsprintf( String &dest, const char *fmt, va_list ap );

	void				ReAllocate( INT amount, bool keepold );				// reallocate String mData buffer
	void				FreeData( void );									// free allocated String memory

	INT					DynamicMemoryUsed() const;

	char *				GetBufferPtr();

	void				ConvertBackslashes();	// Converts backslashes to slashes.


	void VARARGS Format( const char* fmt, ... ) mxCHECK_PRINTF_ARGS;

	void	SetBool( bool value );
	void	SetInt( int value );
	void	SetFloat( float value );

	static String	FromBool( bool value );
	static String	FromInt( int value );
	static String	FromFloat( float value );

	// Returns a pointer to the last directory separator.
	char *	GetLastSlash() const;

	friend AStreamWriter& operator << ( AStreamWriter& file, const String& obj );
	friend AStreamReader& operator >> ( AStreamReader& file, String& obj );
	friend mxArchive& operator && ( mxArchive& archive, String& o );

	// Returns the total size of stored elements, in bytes.
	SizeT GetDataSize() const
	{
		return mLength * sizeof(mData[0]);
	}

	void Reserve( INT characters )
	{
		EnsureAlloced( characters );
	}

protected:
	// ensure string mData buffer is large enough
	void EnsureAlloced( INT amount, bool keepold = true );

protected:
	enum {
		STR_ALLOC_BASE = 20,
		STR_ALLOC_GRANULARITY = 32
	};

	INT				mLength;	// length _excluding_ null terminator
	char *			mData;
	INT				mAlloced;
	char			mBaseBuffer[ STR_ALLOC_BASE ];

private:
	void	Init( void );	// initialize string using base buffer
};

mxDECLARE_BUILTIN_TYPE( String,	ETypeKind::Type_String );


typedef TList< String >	StringListType;

/*
-----------------------------------------------------------------------------
	String
-----------------------------------------------------------------------------
*/

mxIMPLEMENT_SERIALIZE_FUNCTION( String, SerializeString );

FORCEINLINE void String::EnsureAlloced( INT amount, bool keepold ) {
	if ( amount > mAlloced ) {
		ReAllocate( amount, keepold );
	}
}

FORCEINLINE void String::Init( void ) {
	mLength = 0;
	mAlloced = STR_ALLOC_BASE;
	mData = mBaseBuffer;
	mData[ 0 ] = '\0';
}

FORCEINLINE String::String( void ) {
	this->Init();
}

FORCEINLINE String::String( const String &text ) {
	INT l;

	this->Init();
	l = text.GetLength();
	this->EnsureAlloced( l + 1 );
	strcpy( mData, text.mData );
	mLength = l;
}

FORCEINLINE String::String( const String &text, INT start, INT end ) {
	INT i;
	INT l;

	Init();
	if ( end > text.GetLength() ) {
		end = text.GetLength();
	}
	if ( start > text.GetLength() ) {
		start = text.GetLength();
	} else if ( start < 0 ) {
		start = 0;
	}

	l = end - start;
	if ( l < 0 ) {
		l = 0;
	}

	this->EnsureAlloced( l + 1 );

	for ( i = 0; i < l; i++ ) {
		mData[ i ] = text[ start + i ];
	}

	mData[ l ] = '\0';
	mLength = l;
}

FORCEINLINE String::String( const char *text ) {
	INT l;

	this->Init();
	if ( text ) {
		l = strlen( text );
		this->EnsureAlloced( l + 1 );
		strcpy( mData, text );
		mLength = l;
	}
}

FORCEINLINE String::String( const char *text, INT start, INT end ) {
	INT i;
	INT l = strlen( text );

	this->Init();
	if ( end > l ) {
		end = l;
	}
	if ( start > l ) {
		start = l;
	} else if ( start < 0 ) {
		start = 0;
	}

	l = end - start;
	if ( l < 0 ) {
		l = 0;
	}

	EnsureAlloced( l + 1 );

	for ( i = 0; i < l; i++ ) {
		mData[ i ] = text[ start + i ];
	}

	mData[ l ] = '\0';
	mLength = l;
}

FORCEINLINE String::String( const bool b ) {
	this->Init();
	this->EnsureAlloced( 2 );
	mData[ 0 ] = b ? '1' : '0';
	mData[ 1 ] = '\0';
	mLength = 1;
}

FORCEINLINE String::String( const char c ) {
	this->Init();
	this->EnsureAlloced( 2 );
	mData[ 0 ] = c;
	mData[ 1 ] = '\0';
	mLength = 1;
}

FORCEINLINE String::String( const int i ) {
	char text[ 64 ];
	INT l;

	this->Init();
	l = sprintf( text, "%d", i );
	this->EnsureAlloced( l + 1 );
	strcpy( mData, text );
	mLength = l;
}

FORCEINLINE String::String( const unsigned u ) {
	char text[ 64 ];
	INT l;

	this->Init();
	l = sprintf( text, "%u", u );
	this->EnsureAlloced( l + 1 );
	strcpy( mData, text );
	mLength = l;
}

FORCEINLINE String::String( const float f ) {
	char text[ 64 ];
	INT l;

	this->Init();
	l = String::snPrintf( text, sizeof( text ), "%f", f );
	while( l > 0 && text[l-1] == '0' ) text[--l] = '\0';
	while( l > 0 && text[l-1] == '.' ) text[--l] = '\0';
	this->EnsureAlloced( l + 1 );
	strcpy( mData, text );
	mLength = l;
}

FORCEINLINE String::~String( void ) {
	this->FreeData();
	this->Init();
}

FORCEINLINE size_t String::Size( void ) const {
	return sizeof( *this ) + Allocated();
}

FORCEINLINE const char *String::c_str( void ) const {
	return mData;
}

FORCEINLINE String::operator  const char * ( void ) {
	return c_str();
}

FORCEINLINE String::operator  const char * ( void ) const {
	return c_str();
}

FORCEINLINE char String::operator [] ( INT index ) const {
	Assert( ( index >= 0 ) && ( index <= mLength ) );
	return mData[ index ];
}

FORCEINLINE char &String::operator [] ( INT index ) {
	Assert( ( index >= 0 ) && ( index <= mLength ) );
	return mData[ index ];
}

FORCEINLINE void String::operator = ( const String &text ) {
	INT l;

	l = text.GetLength();
	EnsureAlloced( l + 1, false );
	memcpy( mData, text.mData, l );
	mData[l] = '\0';
	mLength = l;
}

INLINE String operator + ( const String &a, const String &b ) {
	String result( a );
	result.Append( b );
	return result;
}

INLINE String operator + ( const String &a, const char *b ) {
	String result( a );
	result.Append( b );
	return result;
}

inline String operator + ( const char *a, const String &b ) {
	String result( a );
	result.Append( b );
	return result;
}

INLINE String operator + ( const String &a, const bool b ) {
	String result( a );
	result.Append( b ? "true" : "false" );
	return result;
}

INLINE String operator + ( const String &a, const char b ) {
	String result( a );
	result.Append( b );
	return result;
}

INLINE String operator + ( const String &a, const float b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%f", b );
	result.Append( text );

	return result;
}

INLINE String operator + ( const String &a, const INT b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%d", b );
	result.Append( text );

	return result;
}

INLINE String operator + ( const String &a, const unsigned b ) {
	char	text[ 64 ];
	String	result( a );

	sprintf( text, "%u", b );
	result.Append( text );

	return result;
}

FORCEINLINE String &String::operator += ( const float a ) {
	char text[ 64 ];

	sprintf( text, "%f", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const INT a ) {
	char text[ 64 ];

	sprintf( text, "%d", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const unsigned a ) {
	char text[ 64 ];

	sprintf_s( text, 64, "%u", a );
	Append( text );

	return *this;
}

FORCEINLINE String &String::operator += ( const String &a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const char *a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const char a ) {
	Append( a );
	return *this;
}

FORCEINLINE String &String::operator += ( const bool a ) {
	Append( a ? "true" : "false" );
	return *this;
}

FORCEINLINE bool operator == ( const String &a, const String &b ) {
	return ( !String::Cmp( a.mData, b.mData ) );
}

FORCEINLINE bool operator == ( const String &a, const char *b ) {
	Assert( b );
	return ( !String::Cmp( a.mData, b ) );
}

FORCEINLINE bool operator == ( const char *a, const String &b ) {
	Assert( a );
	return ( !String::Cmp( a, b.mData ) );
}

FORCEINLINE bool operator != ( const String &a, const String &b ) {
	return !( a == b );
}

FORCEINLINE bool operator != ( const String &a, const char *b ) {
	return !( a == b );
}

FORCEINLINE bool operator != ( const char *a, const String &b ) {
	return !( a == b );
}

FORCEINLINE INT String::Cmp( const char *text ) const {
	Assert( text );
	return String::Cmp( mData, text );
}

FORCEINLINE INT String::Cmpn( const char *text, INT n ) const {
	Assert( text );
	return String::Cmpn( mData, text, n );
}

FORCEINLINE INT String::CmpPrefix( const char *text ) const {
	Assert( text );
	return String::Cmpn( mData, text, strlen( text ) );
}

FORCEINLINE INT String::Icmp( const char *text ) const {
	Assert( text );
	return String::Icmp( mData, text );
}

FORCEINLINE INT String::Icmpn( const char *text, INT n ) const {
	Assert( text );
	return String::Icmpn( mData, text, n );
}

FORCEINLINE INT String::IcmpPrefix( const char *text ) const {
	Assert( text );
	return String::Icmpn( mData, text, strlen( text ) );
}

FORCEINLINE INT String::GetLength( void ) const {
	return mLength;
}

FORCEINLINE INT String::Allocated( void ) const {
	if ( mData != mBaseBuffer ) {
		return mAlloced;
	} else {
		return 0;
	}
}

FORCEINLINE void String::Empty( void ) {
	EnsureAlloced( 1 );
	mData[ 0 ] = '\0';
	mLength = 0;
}

FORCEINLINE bool String::IsEmpty( void ) const {
	return ( String::Cmp( mData, "" ) == 0 );
}

FORCEINLINE void String::Clear( void ) {
	FreeData();
	Init();
}

FORCEINLINE void String::Append( const char a ) {
	EnsureAlloced( mLength + 2 );
	mData[ mLength ] = a;
	mLength++;
	mData[ mLength ] = '\0';
}

FORCEINLINE void String::Append( const String &text ) {
	INT newLen;
	INT i;

	newLen = mLength + text.GetLength();
	EnsureAlloced( newLen + 1 );
	for ( i = 0; i < text.mLength; i++ ) {
		mData[ mLength + i ] = text[ i ];
	}
	mLength = newLen;
	mData[ mLength ] = '\0';
}

FORCEINLINE void String::Append( const char *text ) {
	INT newLen;
	INT i;

	if ( text ) {
		newLen = mLength + strlen( text );
		EnsureAlloced( newLen + 1 );
		for ( i = 0; text[ i ]; i++ ) {
			mData[ mLength + i ] = text[ i ];
		}
		mLength = newLen;
		mData[ mLength ] = '\0';
	}
}

FORCEINLINE void String::Append( const char *text, INT l ) {
	INT newLen;
	INT i;

	if ( text && l ) {
		newLen = mLength + l;
		EnsureAlloced( newLen + 1 );
		for ( i = 0; text[ i ] && i < l; i++ ) {
			mData[ mLength + i ] = text[ i ];
		}
		mLength = newLen;
		mData[ mLength ] = '\0';
	}
}

FORCEINLINE void String::Insert( const char a, INT index ) {
	INT i, l;

	if ( index < 0 ) {
		index = 0;
	} else if ( index > mLength ) {
		index = mLength;
	}

	l = 1;
	EnsureAlloced( mLength + l + 1 );
	for ( i = mLength; i >= index; i-- ) {
		mData[i+l] = mData[i];
	}
	mData[index] = a;
	mLength++;
}

FORCEINLINE void String::Insert( const char *text, INT index ) {
	INT i, l;

	if ( index < 0 ) {
		index = 0;
	} else if ( index > mLength ) {
		index = mLength;
	}

	l = strlen( text );
	EnsureAlloced( mLength + l + 1 );
	for ( i = mLength; i >= index; i-- ) {
		mData[i+l] = mData[i];
	}
	for ( i = 0; i < l; i++ ) {
		mData[index+i] = text[i];
	}
	mLength += l;
}

FORCEINLINE void String::ToLower( void ) {
	for (INT i = 0; mData[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsUpper( (unsigned char)mData[i] ) ) {
			mData[i] += ( 'a' - 'A' );
		}
	}
}

FORCEINLINE void String::ToUpper( void ) {
	for (INT i = 0; mData[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsLower( (unsigned char)mData[i] ) ) {
			mData[i] -= ( 'a' - 'A' );
		}
	}
}

FORCEINLINE bool String::IsNumeric( void ) const {
	return String::IsNumeric( mData );
}

FORCEINLINE bool String::HasLower( void ) const {
	return String::HasLower( mData );
}

FORCEINLINE bool String::HasUpper( void ) const {
	return String::HasUpper( mData );
}

FORCEINLINE void String::CapLength( INT newlen ) {
	if ( mLength <= newlen ) {
		return;
	}
	mData[ newlen ] = 0;
	mLength = newlen;
}

FORCEINLINE void String::Fill( const char ch, INT newlen ) {
	EnsureAlloced( newlen + 1 );
	mLength = newlen;
	memset( mData, ch, mLength );
	mData[ mLength ] = 0;
}

FORCEINLINE INT String::Find( const char c, INT start, INT end ) const {
	if ( end == -1 ) {
		end = mLength;
	}
	return String::FindChar( mData, c, start, end );
}

FORCEINLINE INT String::Find( const char *text, bool casesensitive, INT start, INT end ) const {
	if ( end == -1 ) {
		end = mLength;
	}
	return String::FindText( mData, text, casesensitive, start, end );
}

FORCEINLINE bool String::Filter( const char *filter, bool casesensitive ) const {
	return String::Filter( filter, mData, casesensitive );
}

FORCEINLINE const char *String::Left( INT len, String &result ) const {
	return Mid( 0, len, result );
}

FORCEINLINE const char *String::Right( INT len, String &result ) const {
	if ( len >= GetLength() ) {
		result = *this;
		return result;
	}
	return Mid( GetLength() - len, len, result );
}

FORCEINLINE String String::Left( INT len ) const {
	return Mid( 0, len );
}

inline String String::Right( INT len ) const {
	if ( len >= GetLength() ) {
		return *this;
	}
	return Mid( GetLength() - len, len );
}

FORCEINLINE void String::Strip( const char c ) {
	StripLeading( c );
	StripTrailing( c );
}

FORCEINLINE void String::Strip( const char *str ) {
	StripLeading( str );
	StripTrailing( str );
}

FORCEINLINE bool String::CheckExtension( const char *ext ) {
	return String::CheckExtension( mData, ext );
}

FORCEINLINE INT String::GetLength( const char *s ) {
	INT i;
	for ( i = 0; s[i]; i++ ) {}
	return i;
}

FORCEINLINE char *String::ToLower( char *s ) {
	for ( INT i = 0; s[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsUpper( (unsigned char)s[i] ) ) {
			s[i] += ( 'a' - 'A' );
		}
	}
	return s;
}

FORCEINLINE char *String::ToUpper( char *s ) {
	for ( INT i = 0; s[i]; i++ ) {
		// HUMANHEAD pdm: cast to unsigned for the sake of western european characters, which use the sign bit
		if ( CharIsLower( (unsigned char)s[i] ) ) {
			s[i] -= ( 'a' - 'A' );
		}
	}
	return s;
}

FORCEINLINE INT String::Hash( const char *str ) {
	INT i, hash = 0;
	for ( i = 0; *str != '\0'; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::Hash( const char *str, INT length ) {
	INT i, hash = 0;
	for ( i = 0; i < length; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::IHash( const char *str ) {
	INT i, hash = 0;
	for( i = 0; *str != '\0'; i++ ) {
		hash += ToLower( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE INT String::IHash( const char *str, INT length ) {
	INT i, hash = 0;
	for ( i = 0; i < length; i++ ) {
		hash += ToLower( *str++ ) * ( i + 119 );
	}
	return hash;
}

FORCEINLINE char String::ToLower( char c ) {
	if ( c <= 'Z' && c >= 'A' ) {
		return ( c + ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE char String::ToUpper( char c ) {
	if ( c >= 'a' && c <= 'z' ) {
		return ( c - ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE bool String::CharIsPrintable( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1 && c <= 0xFF );
}

FORCEINLINE bool String::CharIsLower( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c >= 'a' && c <= 'z' ) || ( c >= 0xE0 && c <= 0xFF );
}

FORCEINLINE bool String::CharIsUpper( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c <= 'Z' && c >= 'A' ) || ( c >= 0xC0 && c <= 0xDF );
}

FORCEINLINE bool String::CharIsAlpha( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
			 ( c >= 0xC0 && c <= 0xFF ) );
}

FORCEINLINE bool String::CharIsNumeric( INT c ) {
	return ( c <= '9' && c >= '0' );
}

FORCEINLINE bool String::CharIsNewLine( char c ) {
	return ( c == '\n' || c == '\r' || c == '\v' );
}

FORCEINLINE bool String::CharIsTab( char c ) {
	return ( c == '\t' );
}

FORCEINLINE INT String::ColorIndex( INT c ) {
	return ( c & 15 );
}

FORCEINLINE INT String::DynamicMemoryUsed() const {
	return ( mData == mBaseBuffer ) ? 0 : mAlloced;
}

FORCEINLINE const char* String::ToChars() const {
	return mData;
}

FORCEINLINE char* String::ToChars() {
	return mData;
}

FORCEINLINE char * String::GetBufferPtr() {
	return mData;
}

mxINLINE void String::ConvertBackslashes()
{
	ReplaceChar( '\\', '/' );
}

mxINLINE String String::FromBool( bool value )
{
	String str;
	str.SetBool( value );
	return str;
}

mxINLINE String String::FromInt( int value )
{
	String str;
	str.SetInt( value );
	return str;
}

mxINLINE String String::FromFloat( float value )
{
	String str;
	str.SetFloat( value );
	return str;
}

mxNAMESPACE_END

#endif // !__MX_STRING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
