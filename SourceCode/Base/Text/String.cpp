/*
=============================================================================
	File:	String.cpp
	Desc:	String class.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN


#define MX_USE_STRING_POOL	(1)

namespace
{
	enum { MAX_CHARS_IN_A_STRING = MAX_STRING_CHARS };

#if MX_USE_STRING_POOL

	local_ char* AllocateStringData( SizeT numChars )
	{
		return (char*) F_HeapAlloc( EMemHeap::HeapString, numChars * sizeof(char) );
	}
	local_ void FreeStringData( void* ptr )
	{
		F_HeapFree( EMemHeap::HeapString, ptr );
	}

#else

	local_ char* AllocateStringData( SizeT numChars )
	{
		return new char[ numChars ];
	}
	local_ void FreeStringData( void* ptr )
	{
		delete [] ptr;
	}

#endif //!MX_USE_STRING_POOL

}//namespace

/*
============
String::ReAllocate
============
*/
void String::ReAllocate( INT amount, bool keepold ) {
	char	*newbuffer;
	INT		newsize;
	INT		mod;

	AssertPtr( mData );
	Assert( amount > 0 );
	Assert( amount < MAX_CHARS_IN_A_STRING );

	mod = amount % STR_ALLOC_GRANULARITY;
	if ( ! mod ) {
		newsize = amount;
	}
	else {
		newsize = amount + STR_ALLOC_GRANULARITY - mod;
	}
	mAlloced = newsize;

	newbuffer = AllocateStringData( mAlloced );

	if ( keepold && mData ) {
		mData[ mLength ] = '\0';
		strcpy( newbuffer, mData );
	}

	if ( mData && mData != mBaseBuffer )
	{
		FreeStringData( mData );
	}

	mData = newbuffer;
}

/*
============
String::FreeData
============
*/
void String::FreeData( void )
{
	if ( mData && mData != mBaseBuffer )
	{
		FreeStringData( mData );
		mData = mBaseBuffer;
	}
}

/*
============
String::operator=
============
*/
void String::operator=( const char *text ) {
	INT l = strlen( text );
	this->Set( text, l );
}

void String::Set( const char* text, UINT len )
{
	INT diff;
	INT i;

	if ( !text ) {
		// safe behaviour if NULL
		EnsureAlloced( 1, false );
		mData[ 0 ] = '\0';
		mLength = 0;
		return;
	}

	if ( text == mData ) {
		return; // copying same thing
	}

	// check if we're aliasing
	if ( text >= mData && text <= mData + mLength ) {
		diff = text - mData;

		Assert( strlen( text ) < (unsigned)mLength );

		for ( i = 0; text[ i ]; i++ ) {
			mData[ i ] = text[ i ];
		}

		mData[ i ] = '\0';

		mLength -= diff;

		return;
	}


	EnsureAlloced( len + 1, false );
	strcpy( mData, text );
	mLength = len;
}

/*
============
String::FindChar

returns -1 if not found otherwise the index of the char
============
*/
INT String::FindChar( const char *str, const char c, INT start, INT end ) {
	INT i;

	if ( end == -1 ) {
		end = strlen( str ) - 1;
	}
	for ( i = start; i <= end; i++ ) {
		if ( str[i] == c ) {
			return i;
		}
	}
	return -1;
}

/*
============
String::FindText

returns -1 if not found otherwise the index of the text
============
*/
INT String::FindText( const char *str, const char *text, bool casesensitive, INT start, INT end ) {
	INT l, i, j;

	if ( end == -1 ) {
		end = strlen( str );
	}
	l = end - strlen( text );
	for ( i = start; i <= l; i++ ) {
		if ( casesensitive ) {
			for ( j = 0; text[j]; j++ ) {
				if ( str[i+j] != text[j] ) {
					break;
				}
			}
		} else {
			for ( j = 0; text[j]; j++ ) {
				if ( ::toupper( str[i+j] ) != ::toupper( text[j] ) ) {
					break;
				}
			}
		}
		if ( !text[j] ) {
			return i;
		}
	}
	return -1;
}

/*
============
String::Filter

Returns true if the String conforms the given filter.
Several metacharacter may be used in the filter.

*          match any String of zero or more characters
?          match any single character
[lol...]   match any of the enclosed characters; a hyphen can
           be used to specify a range (e.g. a-z, A-Z, 0-9)

============
*/
bool String::Filter( const char *filter, const char *name, bool casesensitive ) {
	String buf;
	INT i, found, index;

	while(*filter) {
		if (*filter == '*') {
			filter++;
			buf.Empty();
			for (i = 0; *filter; i++) {
				if ( *filter == '*' || *filter == '?' || (*filter == '[' && *(filter+1) != '[') ) {
					break;
				}
				buf += *filter;
				if ( *filter == '[' ) {
					filter++;
				}
				filter++;
			}
			if ( buf.GetLength() ) {
				index = String(name).Find( buf.c_str(), casesensitive );
				if ( index == -1 ) {
					return false;
				}
				name += index + strlen(buf);
			}
		}
		else if (*filter == '?') {
			filter++;
			name++;
		}
		else if (*filter == '[') {
			if ( *(filter+1) == '[' ) {
				if ( *name != '[' ) {
					return false;
				}
				filter += 2;
				name++;
			}
			else {
				filter++;
				found = false;
				while(*filter && !found) {
					if (*filter == ']' && *(filter+1) != ']') {
						break;
					}
					if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']')) {
						if (casesensitive) {
							if (*name >= *filter && *name <= *(filter+2)) {
								found = true;
							}
						}
						else {
							if ( ::toupper(*name) >= ::toupper(*filter) && ::toupper(*name) <= ::toupper(*(filter+2)) ) {
								found = true;
							}
						}
						filter += 3;
					}
					else {
						if (casesensitive) {
							if (*filter == *name) {
								found = true;
							}
						}
						else {
							if ( ::toupper(*filter) == ::toupper(*name) ) {
								found = true;
							}
						}
						filter++;
					}
				}
				if (!found) {
					return false;
				}
				while(*filter) {
					if ( *filter == ']' && *(filter+1) != ']' ) {
						break;
					}
					filter++;
				}
				filter++;
				name++;
			}
		}
		else {
			if (casesensitive) {
				if (*filter != *name) {
					return false;
				}
			}
			else {
				if ( ::toupper(*filter) != ::toupper(*name) ) {
					return false;
				}
			}
			filter++;
			name++;
		}
	}
	return true;
}

/*
=============
String::StripMediaName

  makes the String lower case, replaces backslashes with forward slashes, and removes extension
=============
*/
void String::StripMediaName( const char *name, String &mediaName ) {
	char c;

	mediaName.Empty();

	for ( c = *name; c; c = *(++name) ) {
		// truncate at an extension
		if ( c == '.' ) {
			break;
		}
		// convert backslashes to forward slashes
		if ( c == '\\' ) {
			mediaName.Append( '/' );
		} else {
			mediaName.Append( String::ToLower( c ) );
		}
	}
}

/*
=============
String::CheckExtension
=============
*/
bool String::CheckExtension( const char *name, const char *ext ) {
	const char *s1 = name + GetLength( name ) - 1;
	const char *s2 = ext + GetLength( ext ) - 1;
	INT c1, c2, d;

	do {
		c1 = *s1--;
		c2 = *s2--;

		d = c1 - c2;
		while( d ) {
			if ( c1 <= 'Z' && c1 >= 'A' ) {
				d += ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c2 <= 'Z' && c2 >= 'A' ) {
				d -= ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			return false;
		}
	} while( s1 > name && s2 > ext );

	return ( s1 >= name );
}

/*
=============
String::FloatArrayToString
=============
*/
mxMT_UNSAFE
const char *String::FloatArrayToString( const float *array, const INT length, const INT precision ) {
	static INT index = 0;
	static char str[4][16384];	// in case called by nested functions
	INT i, n;
	char format[16], *s;

	// use an array of String so that multiple calls won't collide
	s = str[ index ];
	index = (index + 1) & 3;

	String::snPrintf( format, sizeof( format ), "%%.%df", precision );
	n = String::snPrintf( s, sizeof( str[0] ), format, array[0] );
	if ( precision > 0 ) {
		while( n > 0 && s[n-1] == '0' ) s[--n] = '\0';
		while( n > 0 && s[n-1] == '.' ) s[--n] = '\0';
	}
	String::snPrintf( format, sizeof( format ), " %%.%df", precision );
	for ( i = 1; i < length; i++ ) {
		n += String::snPrintf( s + n, sizeof( str[0] ) - n, format, array[i] );
		if ( precision > 0 ) {
			while( n > 0 && s[n-1] == '0' ) s[--n] = '\0';
			while( n > 0 && s[n-1] == '.' ) s[--n] = '\0';
		}
	}
	return s;
}

/*
============
String::Last

returns -1 if not found otherwise the index of the char
============
*/
INT String::Last( const char c ) const {
	INT i;
	
	for( i = GetLength(); i > 0; i-- ) {
		if ( mData[ i - 1 ] == c ) {
			return i - 1;
		}
	}

	return -1;
}

/*
============
String::StripLeading
============
*/
void String::StripLeading( const char c ) {
	while( mData[ 0 ] == c ) {
		memmove( &mData[ 0 ], &mData[ 1 ], mLength );
		mLength--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripLeading( const char * str ) {
	INT l;

	l = strlen( str );
	if ( l > 0 ) {
		while ( !Cmpn( str, l ) ) {
			memmove( mData, mData + l, mLength - l + 1 );
			mLength -= l;
		}
	}
}

/*
============
String::StripLeadingOnce
============
*/
bool String::StripLeadingOnce( const char * str ) {
	INT l;

	l = strlen( str );
	if ( ( l > 0 ) && !Cmpn( str, l ) ) {
		memmove( mData, mData + l, mLength - l + 1 );
		mLength -= l;
		return true;
	}
	return false;
}

/*
============
String::StripTrailing
============
*/
void String::StripTrailing( const char c ) {
	INT i;
	
	for( i = GetLength(); i > 0 && mData[ i - 1 ] == c; i-- ) {
		mData[ i - 1 ] = '\0';
		mLength--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripTrailing( const char *str ) {
	INT l;

	l = strlen( str );
	if ( l > 0 ) {
		while ( ( mLength >= l ) && !Cmpn( str, mData + mLength - l, l ) ) {
			mLength -= l;
			mData[mLength] = '\0';
		}
	}
}

/*
============
String::StripTrailingOnce
============
*/
bool String::StripTrailingOnce( const char *str ) {
	INT l;

	l = strlen( str );
	if ( ( l > 0 ) && ( mLength >= l ) && !Cmpn( str, mData + mLength - l, l ) ) {
		mLength -= l;
		mData[mLength] = '\0';
		return true;
	}
	return false;
}

/*
============
String::Replace
============
*/
void String::Replace( const char *old, const char *nw ) {
	INT		oldLen, newLen, i, j, count;
	String	oldString( mData );

	oldLen = strlen( old );
	newLen = strlen( nw );

	// Work out how big the new String will be
	count = 0;
	for( i = 0; i < oldString.GetLength(); i++ ) {
		if( !String::Cmpn( &oldString[i], old, oldLen ) ) {
			count++;
			i += oldLen - 1;
		}
	}

	if( count ) {
		EnsureAlloced( mLength + ( ( newLen - oldLen ) * count ) + 2, false );

		// Replace the old mData with the new mData
		for( i = 0, j = 0; i < oldString.GetLength(); i++ ) {
			if( !String::Cmpn( &oldString[i], old, oldLen ) ) {
				memcpy( mData + j, nw, newLen );
				i += oldLen - 1;
				j += newLen;
			} else {
				mData[j] = oldString[i];
				j++;
			}
		}
		mData[j] = 0;
		mLength = strlen( mData );
	}
}

/*
============
String::ReplaceChar
============
*/
void String::ReplaceChar( char old, char nw )
{
	for( UINT i = 0; i < GetLength(); i++ )
	{
		if( mData[i] == old ) {
			mData[i] = nw;
		}
	}
}

/*
============
String::Mid
============
*/
const char *String::Mid( INT start, INT len, String &result ) const {
	INT i;

	result.Empty();

	i = GetLength();
	if ( i == 0 || len <= 0 || start >= i ) {
		return NULL;
	}

	if ( start + len >= i ) {
		len = i - start;
	}

	result.Append( &mData[ start ], len );
	return result;
}

/*
============
String::Mid
============
*/
String String::Mid( INT start, INT len ) const {
	INT i;
	String result;

	i = GetLength();
	if ( i == 0 || len <= 0 || start >= i ) {
		return result;
	}

	if ( start + len >= i ) {
		len = i - start;
	}

	result.Append( &mData[ start ], len );
	return result;
}

/*
============
String::StripTrailingWhitespace
============
*/
void String::StripTrailingWhitespace( void ) {
	INT i;
	
	// cast to unsigned char to prevent stripping off high-ASCII characters
	for( i = GetLength(); i > 0 && (unsigned char)(mData[ i - 1 ]) <= ' '; i-- ) {
		mData[ i - 1 ] = '\0';
		mLength--;
	}
}

/*
============
String::StripQuotes

Removes the quotes from the beginning and end of the String
============
*/
String& String::StripQuotes ( void )
{
	if ( mData[0] != '\"' )
	{
		return *this;
	}
	
	// Remove the trailing quote first
	if ( mData[mLength-1] == '\"' )
	{
		mData[mLength-1] = '\0';
		mLength--;
	}

	// Strip the leading quote now
	mLength--;	
	memmove( &mData[ 0 ], &mData[ 1 ], mLength );
	mData[mLength] = '\0';
	
	return *this;
}

/*
=====================================================================

  filename methods

=====================================================================
*/

/*
============
String::FileNameHash
============
*/
INT String::FileNameHash( void ) const {
	INT		i;
	long	hash;
	char	letter;

	hash = 0;
	i = 0;
	while( mData[i] != '\0' ) {
		letter = String::ToLower( mData[i] );
		if ( letter == '.' ) {
			break;				// don't include extension
		}
		if ( letter =='\\' ) {
			letter = '/';
		}
		hash += (long)(letter)*(i+119);
		i++;
	}
	hash &= (FILE_HASH_SIZE-1);
	return hash;
}

/*
============
String::BackSlashesToSlashes
============
*/
String &String::BackSlashesToSlashes( void ) {
	INT i;

	for ( i = 0; i < mLength; i++ ) {
		if ( mData[ i ] == '\\' ) {
			mData[ i ] = '/';
		}
	}
	return *this;
}

/*
============
String::SetFileExtension
============
*/
String &String::SetFileExtension( const char *extension ) {
	StripFileExtension();
	if ( *extension != '.' ) {
		Append( '.' );
	}
	Append( extension );
	return *this;
}

/*
============
String::StripFileExtension
============
*/
String &String::StripFileExtension( void ) {
	INT i;

	for ( i = mLength-1; i >= 0; i-- ) {
		if ( mData[i] == '.' ) {
			mData[i] = '\0';
			mLength = i;
			break;
		}
	}
	return *this;
}

/*
============
String::StripAbsoluteFileExtension
============
*/
String &String::StripAbsoluteFileExtension( void ) {
	INT i;

	for ( i = 0; i < mLength; i++ ) {
		if ( mData[i] == '.' ) {
			mData[i] = '\0';
			mLength = i;
			break;
		}
	}

	return *this;
}

/*
==================
String::DefaultFileExtension
==================
*/
String &String::DefaultFileExtension( const char *extension ) {
	INT i;

	// do nothing if the String already has an extension
	for ( i = mLength-1; i >= 0; i-- ) {
		if ( mData[i] == '.' ) {
			return *this;
		}
	}
	if ( *extension != '.' ) {
		Append( '.' );
	}
	Append( extension );
	return *this;
}

/*
==================
String::DefaultPath
==================
*/
String &String::DefaultPath( const char *basepath ) {
	if ( ( ( *this )[ 0 ] == '/' ) || ( ( *this )[ 0 ] == '\\' ) ) {
		// absolute path location
		return *this;
	}

	*this = basepath + *this;
	return *this;
}

/*
====================
String::AppendPath
====================
*/
void String::AppendPath( const char *text ) {
	INT pos;
	INT i = 0;

	if ( text && text[i] ) {
		pos = mLength;
		EnsureAlloced( mLength + strlen( text ) + 2 );

		if ( pos ) {
			if ( mData[ pos-1 ] != '/' ) {
				mData[ pos++ ] = '/';
			}
		}
		if ( text[i] == '/' ) {
			i++;
		}

		for ( ; text[ i ]; i++ ) {
			if ( text[ i ] == '\\' ) {
				mData[ pos++ ] = '/';
			} else {
				mData[ pos++ ] = text[ i ];
			}
		}
		mLength = pos;
		mData[ pos ] = '\0';
	}
}

/*
==================
String::StripFilename
==================
*/
String &String::StripFilename( void ) {
	INT pos;

	pos = GetLength() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos ] != '/' ) && ( ( *this )[ pos ] != '\\' ) ) {
		pos--;
	}

	if ( pos < 0 ) {
		pos = 0;
	}

	CapLength( pos );
	return *this;
}

/*
==================
String::StripPath
==================
*/
String &String::StripPath( void ) {
	INT pos;

	pos = GetLength();
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) ) {
		pos--;
	}

	*this = Right( GetLength() - pos );
	return *this;
}

/*
====================
String::ExtractFilePath
====================
*/
void String::ExtractFilePath( String &dest ) const {
	INT pos;

	//
	// back up until a \ or the start
	//
	pos = GetLength();
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) ) {
		pos--;
	}

	Left( pos, dest );
}

/*
====================
String::ExtractFileName
====================
*/
void String::ExtractFileName( String &dest ) const {
	INT pos;

	//
	// back up until a \ or the start
	//
	pos = GetLength() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) ) {
		pos--;
	}

	Right( GetLength() - pos, dest );
}

/*
====================
String::ExtractFileBase
====================
*/
void String::ExtractFileBase( String &dest ) const {
	INT pos;
	INT start;

	//
	// back up until a \ or the start
	//
	pos = GetLength() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) ) {
		pos--;
	}

	start = pos;
	while( ( pos < GetLength() ) && ( ( *this )[ pos ] != '.' ) ) {
		pos++;
	}

	Mid( start, pos - start, dest );
}

/*
====================
String::ExtractFileExtension
====================
*/
void String::ExtractFileExtension( String &dest ) const {
	INT pos;

	//
	// back up until a . or the start
	//
	pos = GetLength() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '.' ) ) {
		pos--;
	}

	if ( !pos ) {
		// no extension
		dest.Empty();
	} else {
		Right( GetLength() - pos, dest );
	}
}


/*
=====================================================================

  char * methods to replace library functions

=====================================================================
*/

/*
============
String::IsNumeric

Checks a String to see if it contains only numerical values.
============
*/
bool String::IsNumeric( const char *s ) {
	INT		i;
	bool	dot;

	if ( *s == '-' ) {
		s++;
	}

	dot = false;
	for ( i = 0; s[i]; i++ ) {
		if ( !mxAnsiCharIsDigit( s[i] ) ) {
			if ( ( s[ i ] == '.' ) && !dot ) {
				dot = true;
				continue;
			}
			return false;
		}
	}

	return true;
}

/*
============
String::HasLower

Checks if a String has any lowercase chars
============
*/
bool String::HasLower( const char *s ) {
	if ( !s ) {
		return false;
	}
	
	while ( *s ) {
		if ( CharIsLower( *s ) ) {
			return true;
		}
		s++;
	}
	
	return false;
}

/*
============
String::HasUpper
	
Checks if a String has any uppercase chars
============
*/
bool String::HasUpper( const char *s ) {
	if ( !s ) {
		return false;
	}
	
	while ( *s ) {
		if ( CharIsUpper( *s ) ) {
			return true;
		}
		s++;
	}
	
	return false;
}

/*
================
String::Cmp
================
*/
INT String::Cmp( const char *s1, const char *s2 ) {
	INT c1, c2, d;

	do {
		c1 = *s1++;
		c2 = *s2++;

		d = c1 - c2;
		if ( d ) {
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;		// strings are equal
}

/*
================
String::Cmpn
================
*/
INT String::Cmpn( const char *s1, const char *s2, INT n ) {
	INT c1, c2, d;

	Assert( n >= 0 );

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;		// strings are equal until end point
		}

		d = c1 - c2;
		if ( d ) {
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;		// strings are equal
}

/*
================
String::Icmp
================
*/
INT String::Icmp( const char *s1, const char *s2 ) {
	INT c1, c2, d;

	do {
		c1 = *s1++;
		c2 = *s2++;

		d = c1 - c2;
		while( d ) {
			if ( c1 <= 'Z' && c1 >= 'A' ) {
				d += ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c2 <= 'Z' && c2 >= 'A' ) {
				d -= ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;		// strings are equal
}

/*
================
String::Icmpn
================
*/
INT String::Icmpn( const char *s1, const char *s2, INT n ) {
	INT c1, c2, d;

	Assert( n >= 0 );

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;		// strings are equal until end point
		}

		d = c1 - c2;
		while( d ) {
			if ( c1 <= 'Z' && c1 >= 'A' ) {
				d += ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c2 <= 'Z' && c2 >= 'A' ) {
				d -= ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;		// strings are equal
}

/*
================
String::IcmpPath
================
*/
INT String::IcmpPath( const char *s1, const char *s2 ) {
	INT c1, c2, d;

	do {
		c1 = *s1++;
		c2 = *s2++;

		d = c1 - c2;
		while( d ) {
			if ( c1 <= 'Z' && c1 >= 'A' ) {
				d += ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c1 == '\\' ) {
				d += ('/' - '\\');
				if ( !d ) {
					break;
				}
			}
			if ( c2 <= 'Z' && c2 >= 'A' ) {
				d -= ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c2 == '\\' ) {
				d -= ('/' - '\\');
				if ( !d ) {
					break;
				}
			}
			// make sure folders come first
			while( c1 ) {
				if ( c1 == '/' || c1 == '\\' ) {
					break;
				}
				c1 = *s1++;
			}
			while( c2 ) {
				if ( c2 == '/' || c2 == '\\' ) {
					break;
				}
				c2 = *s2++;
			}
			if ( c1 && !c2 ) {
				return -1;
			} else if ( !c1 && c2 ) {
				return 1;
			}
			// same folder depth so use the regular compare
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;
}

/*
================
String::IcmpnPath
================
*/
INT String::IcmpnPath( const char *s1, const char *s2, INT n ) {
	INT c1, c2, d;

	Assert( n >= 0 );

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;		// strings are equal until end point
		}

		d = c1 - c2;
		while( d ) {
			if ( c1 <= 'Z' && c1 >= 'A' ) {
				d += ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c1 == '\\' ) {
				d += ('/' - '\\');
				if ( !d ) {
					break;
				}
			}
			if ( c2 <= 'Z' && c2 >= 'A' ) {
				d -= ('a' - 'A');
				if ( !d ) {
					break;
				}
			}
			if ( c2 == '\\' ) {
				d -= ('/' - '\\');
				if ( !d ) {
					break;
				}
			}
			// make sure folders come first
			while( c1 ) {
				if ( c1 == '/' || c1 == '\\' ) {
					break;
				}
				c1 = *s1++;
			}
			while( c2 ) {
				if ( c2 == '/' || c2 == '\\' ) {
					break;
				}
				c2 = *s2++;
			}
			if ( c1 && !c2 ) {
				return -1;
			} else if ( !c1 && c2 ) {
				return 1;
			}
			// same folder depth so use the regular compare
			return ( INT32_SIGN_BIT_NOT_SET( d ) << 1 ) - 1;
		}
	} while( c1 );

	return 0;
}

/*
=============
String::Copynz
 
Safe strncpy that ensures a trailing zero
=============
*/
void String::Copynz( char *dest, const char *src, INT destsize ) {
	if ( !src ) {
		mxWarn( "String::Copynz: NULL src" );
		return;
	}
	if ( destsize < 1 ) {
		mxWarn( "String::Copynz: destsize < 1" ); 
		return;
	}

	strncpy( dest, src, destsize-1 );
    dest[ destsize-1 ] = 0;
}

/*
================
String::Append

  never goes past bounds or leaves without a terminating 0
================
*/
void String::Append( char *dest, INT size, const char *src ) {
	INT		l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		mxErr( "String::Append: already overflowed" );
	}
	String::Copynz( dest + l1, src, size - l1 );
}

#define STRING_MAX_STACK_BUF_SIZE		4096*2	// decreased from STRING_MAX_STACK_BUF_SIZE

/*
================
String::snPrintf
================
*/
INT String::snPrintf( char *dest, INT size, const char *fmt, ...) {
	INT len;
	va_list argptr;
	char buffer[STRING_MAX_STACK_BUF_SIZE];	// big, but small enough to fit in PPC stack

	va_start( argptr, fmt );
	len = vsprintf( buffer, fmt, argptr );
	va_end( argptr );
	if ( len >= sizeof( buffer ) ) {
		mxErr( "String::snPrintf: overflowed buffer" );
	}
	if ( len >= size ) {
		mxWarnf( "String::snPrintf: overflow of %i in %i\n", len, size );
		len = size;
	}
	String::Copynz( dest, buffer, size );
	return len;
}

/*
============
String::vsnPrintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final String if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count, then count 
characters are written and -1 is returned and no trailing '\0' is added.

String::vsnPrintf: always appends a trailing '\0', returns number of characters written (not including terminal \0)
or returns -1 on failure or if the buffer would be overflowed.
============
*/
INT String::vsnPrintf( char *dest, INT size, const char *fmt, va_list argptr ) {
	INT ret;
	mxBUG("this caused stack overflow for unknown reasons:");
#ifdef _WIN32
//#undef _vsnprintf
	ret = _vsnprintf( dest, size-1, fmt, argptr );
//#define _vsnprintf	use_idStr_vsnPrintf
#else
//#undef vsnprintf
	ret = vsnprintf( dest, size, fmt, argptr );
//#define vsnprintf	use_idStr_vsnPrintf
#endif
	dest[size-1] = '\0';
	if ( ret < 0 || ret >= size ) {
		return -1;
	}
	return ret;
}

/*
============
sprintf

Sets the value of the String using a printf interface.
============
*/
INT _sprintf( String &str, const char *fmt, ... ) {
	INT l;
	va_list argptr;
	char buffer[STRING_MAX_STACK_BUF_SIZE];
	
	va_start( argptr, fmt );
	l = String::vsnPrintf( buffer, sizeof(buffer)-1, fmt, argptr );
	va_end( argptr );
	buffer[sizeof(buffer)-1] = '\0';

	str = buffer;
	return l;
}

/*
============
vsprintf

Sets the value of the String using a vprintf interface.
============
*/
INT _vsprintf( String & str, const char *fmt, va_list argptr ) {
	INT l;
	char buffer[STRING_MAX_STACK_BUF_SIZE];
	
	l = String::vsnPrintf( buffer, sizeof(buffer)-1, fmt, argptr );
	buffer[ sizeof(buffer)-1 ] = '\0';
	
	str = buffer;
	return l;
}

void VARARGS String::Format( const char* fmt, ... ) mxCHECK_PRINTF_ARGS
{
	va_list argList;
	va_start( argList, fmt );
	char buf[ MAX_STRING_CHARS ];

	FormatArgListAnsi( buf, NUMBER_OF(buf), fmt, argList );

	*this = buf;
	va_end( argList );
}

void String::SetBool( bool value )
{
	*this = value ? "true" : "false";
}

void String::SetInt( int value )
{
	this->Format( "%d", value );
}

void String::SetFloat( float value )
{
	this->Format( "%f", value );
}

char * String::GetLastSlash() const
{
	const char* s = this->ToChars();
	const char* lastSlash = ::strrchr( s, '/' );
	if( 0 == lastSlash ) lastSlash = ::strrchr( s, '\\' );
	if( 0 == lastSlash ) lastSlash = ::strrchr( s, ':' );
	return const_cast< char* >( lastSlash );
}

AStreamWriter& operator << ( AStreamWriter& file, const String& obj )
{
	const S4 len = obj.mLength;
	file << len;
	if( len > 0 ) {
		file.Write( obj.mData, len * sizeof(obj.mData[0]) );
	}
	//DBGOUT("saved string: %s (len=%d)\n",obj.mData,(int)len);
	return file;
}
AStreamReader& operator >> ( AStreamReader& file, String& obj )
{
	S4 len;
	file >> len;

	obj.EnsureAlloced( len + 1 );
	if( len > 0 )
	{
		file.Read( obj.mData, len * sizeof(obj.mData[0]) );
	}
	obj.mLength = len;
	obj.mData[len] = '\0';
	//DBGOUT("read string: %s (len=%d)\n",obj.mData,(int)len);
	return file;
}
//mxArchive& operator && ( mxArchive& archive, String& o )
//{
//	return Serialize_ArcViaStreams( archive, o );
//}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
