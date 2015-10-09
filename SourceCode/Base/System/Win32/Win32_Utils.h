/*
=============================================================================
	File:	Win32_Utils.h
	Desc:
=============================================================================
*/
#pragma once

mxPERM("we should have our own optimized printf(); also, don't use CRT functions");



mxNAMESPACE_BEGIN

// Maximum number of characters a temporary stack-allocated text buffer can hold.
// If this value is too large, it will cause stack overflow.
//enum { MAX_STRING_CHARS = 4096 };	// a 4 Kb buffer
enum { MAX_STRING_CHARS = 1024 };

//------------------------------------------------------------------------
//	String comparisons
//------------------------------------------------------------------------

FORCEINLINE INT mxStrCmpAnsi( const ANSICHAR* str1, const ANSICHAR* str2 )
{
	return strcmp( str1, str2 );
}
FORCEINLINE INT mxStrCmpUnicode( const UNICODECHAR* str1, const UNICODECHAR* str2 )
{
	return wcscmp( str1, str2 );
}
FORCEINLINE INT mxStrCmp( const mxChar* str1, const mxChar* str2 )
{
	return _tcscmp( str1, str2 );
}

// case-insensitive compare
FORCEINLINE INT mxStrCmpI( const mxChar* str1, const mxChar* str2 )
{
	return _tcsicmp( str1, str2 );
}

FORCEINLINE INT mxStrNCmp( const mxChar* str1, const mxChar* str2, SizeT maxCount )
{
	return _tcsncmp( str1, str2, maxCount );
}

FORCEINLINE INT mxStrNCmpI( const mxChar* str1, const mxChar* str2, SizeT maxCount )
{
	return _tcsnicmp( str1, str2, maxCount );
}

FORCEINLINE INT mxStrNCmpAnsi( const ANSICHAR* str1, const ANSICHAR* str2, SizeT maxCount )
{
	return strncmp( str1, str2, maxCount );
}
FORCEINLINE INT mxStrNCmpIAnsi( const ANSICHAR* str1, const ANSICHAR* str2, SizeT maxCount )
{
	return strnicmp( str1, str2, maxCount );
}


FORCEINLINE bool mxStrEquAnsi( const ANSICHAR* str1, const ANSICHAR* str2 )
{
	return (0 == mxStrCmpAnsi( str1, str2 ));
}
FORCEINLINE bool mxStrEquAnsi( const ANSICHAR* str1, const ANSICHAR* str2, SizeT maxCount )
{
	return (0 == mxStrNCmpAnsi( str1, str2, maxCount ));
}

// case sensitive comparison
FORCEINLINE bool mxEqualANSIStrings( const ANSICHAR* s1, UINT len1, const ANSICHAR* s2, UINT len2 )
{
	// 1 - first compares string lengths before doing full string comparisons;
	// may or may not be faster. YMMV.
#define MX_USE_SHORT_CIRCUITING_TFIXED_STRING_COMPARES	(1)

#if MX_USE_SHORT_CIRCUITING_TFIXED_STRING_COMPARES
	if( len1 != len2 ) {
		return false;
	}
#endif

#define mymin(x,y)	(((x)>(y))?(x):(y))

	UINT numChars = mymin( len1, len2 );
	return mxStrEquAnsi( s1, s2, numChars );

#undef MX_USE_SHORT_CIRCUITING_TFIXED_STRING_COMPARES
#undef mymin
}

FORCEINLINE bool mxStrEquUnicode( const mxChar* str1, const mxChar* str2 )
{
	return (0 == mxStrCmpUnicode( str1, str2 ));
}
FORCEINLINE bool mxStrEqu( const mxChar* str1, const mxChar* str2 )
{
	return (0 == mxStrCmp( str1, str2 ));
}

//------------------------------------------------------------------------
//	String length calculation
//------------------------------------------------------------------------

FORCEINLINE INT GetStrLengthUnicode( const UNICODECHAR* str )
{
	return ::WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
}
FORCEINLINE INT GetStrLengthAnsi( const ANSICHAR* str )
{
	return ::MultiByteToWideChar( CP_ACP, 0, str, -1, NULL, 0 );
}

SizeT mxStrLenAnsi( const ANSICHAR* str );

SizeT mxStrLenUnicode( const UNICODECHAR* str );

FORCEINLINE SizeT mxStrNLenAnsi( const ANSICHAR* str, SizeT maxCount )
{
	return strnlen( str, maxCount );
}
FORCEINLINE SizeT mxStrLen( const mxChar* str )
{
	return _tcslen( str );
}

//------------------------------------------------------------------------
//	String concatenation
//------------------------------------------------------------------------

mxChar* mxStrCat( mxChar *dest, const mxChar* src );

ANSICHAR* mxStrCatAnsi( ANSICHAR *dest, const ANSICHAR* src );

/*==============================================================================
	mxStrAppend

	Consumes:
		char[DestSize]	pDest - points to the destination buffer
		SizeT			DestLength - holds the length of the string in the destination buffer
		char[SrcSize]	pSrc - points to the source buffer

	Returns:
		The number of copied characters (excluding NULL terminator).
		So if retval < SrcSize then truncation has occured.

	Notes:
		Always NUL terminates the destination buffer (unless dest_size == 0).
		Make sure the destination buffer is initialized with zero(s) before using this function!

e.g.:
	char	buf[1];
	mxStrAppend( buf, "MyString" );
==============================================================================*/

template< SizeT DestSize, SizeT SrcSize >
SizeT mxStrAppend(
	char (&pDest)[DestSize],
	SizeT DestLength,
	const char (&pSrc)[SrcSize] )
{
	mxASSERT( DestLength < DestSize );

	// determine the max amount of chars we can copy (1 - for NULL terminator)
	SizeT n = Min( DestSize - DestLength, SrcSize ) - 1;

	char * d = pDest + DestLength;	// start copying with this position
	const char * s = pSrc;

	while( n && *s )
	{
		*d++ = *s++;
		n--;
	}
	*d = '\0';
	return s - pSrc;
}

//------------------------------------------------------------------------
//	String copying
//------------------------------------------------------------------------

ANSICHAR* mxStrCpyAnsi( ANSICHAR* dest, const ANSICHAR* src );

ANSICHAR* mxStrCpyNAnsi( ANSICHAR* dest, const ANSICHAR* src, SizeT maxCount );

UNICODECHAR* mxStrCpyUnicode( UNICODECHAR* dest, const UNICODECHAR* src );


FORCEINLINE void mxStrCpy( mxChar * dest, const mxChar* src )
{
	wcscpy( dest, src );
}

//------------------------------------------------------------------------
//	String searching
//------------------------------------------------------------------------


const mxChar* mxStrStr( const mxChar *str, const mxChar* substr );

// finds the first occurrence of string2 in string1
//
FORCEINLINE const ANSICHAR* __cdecl mxStrStrAnsi(
	const ANSICHAR* str1,	// string to search in
	const ANSICHAR* str2	// string to search for
){
	return ::strstr( str1, str2 );
}

// finds the first occurrence of string2 in string1
//
const ANSICHAR* __cdecl mxStrStrNAnsi(
	const ANSICHAR* str1,	// string to search in
	const ANSICHAR* str2,	// string to search for
	const SizeT maxChars	// don't search past this number of chars
);

const mxChar* mxStrChr( const mxChar *str, const INT ch );
const ANSICHAR* mxStrChrAnsi( const ANSICHAR *str, const INT c );

inline const char* mxStrGetExt( const char *filepath, const SizeT len )
{
	const char *str = filepath;
	for (const char* p = str + len-1; p >= str; --p)
	{
		switch(*p)
		{
		case ':':
		case '/':
		case '\\':
			// we've reached a path separator - it means there's no extension in this name
			return "";
		case '.':
			// there's an extension in this file name
			return p+1;
		}
	}
	return "";
}

// converts backslashes into forward slashes
inline void ConvertBackSlashesAnsi( ANSICHAR* str )
{
	ANSICHAR* p = str;
	while( *p ) {
		if( *p == '\\' ) {
			*p = '/';
		}
		++p;
	}
}
inline void ConvertBackSlashesUnicode( UNICODECHAR* str )
{
	UNICODECHAR* p = str;
	while( *p ) {
		if( *p == L'\\' ) {
			*p = L'/';
		}
		++p;
	}
}
INLINE void ConvertBackSlashes( mxChar* str )
{
#if UNICODE
	ConvertBackSlashesUnicode( str );
#else
	ConvertBackSlashesAnsi( str );
#endif
}

// Converts a path to the uniform form (Unix-style) in place.
template< typename PATH_TYPE >
inline void F_NormalizePath( PATH_TYPE & pathName )
{
	if( !pathName.IsEmpty() )
	{
		ConvertBackSlashesAnsi( pathName.ToChars() );
		if( pathName.GetLast() != '/' )
		{
			pathName.Append( '/' );
		}
	}
}

// e.g. "Data/meshes/world.x" => "/meshes/world.x"
//
inline PCSTR StripPackageName( PCSTR filePath )
{
	const char* p = filePath;

	while( *p )
	{
		if( PATHSEPARATOR_CHAR == *p )
		{
			break;
		}
		++p;
	}

	return p;
}

FORCEINLINE char mxToLower( char c ) {
	if ( c <= 'Z' && c >= 'A' ) {
		return ( c + ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE char mxToUpper( char c ) {
	if ( c >= 'a' && c <= 'z' ) {
		return ( c - ( 'a' - 'A' ) );
	}
	return c;
}

FORCEINLINE bool mxCharIsPrintable( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1 && c <= 0xFF );
}

FORCEINLINE bool mxCharIsLower( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c >= 'a' && c <= 'z' ) || ( c >= 0xE0 && c <= 0xFF );
}

FORCEINLINE bool mxCharIsUpper( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( c <= 'Z' && c >= 'A' ) || ( c >= 0xC0 && c <= 0xDF );
}

FORCEINLINE bool mxCharIsAlpha( INT c ) {
	// test for regular ASCII and western European high-ascii chars
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
		( c >= 0xC0 && c <= 0xFF ) );
}

FORCEINLINE bool mxCharIsNumeric( INT c ) {
	return ( c <= '9' && c >= '0' );
}

FORCEINLINE bool mxCharIsNewLine( char c ) {
	return ( c == '\n' || c == '\r' || c == '\v' );
}

FORCEINLINE bool mxCharIsTab( char c ) {
	return ( c == '\t' );
}

FORCEINLINE bool mxAnsiCharIsDigit( char c )
{
	return c >= '0' && c <= '9';
}

FORCEINLINE bool mxAnsiCharIsSpace( char c )
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

FORCEINLINE bool mxAnsiCharIsDelimiter( char c )
{
	return c==' ' || c=='\t' || c==';' || c==',' || c=='=' || c==':' || c=='\n';
}

FORCEINLINE bool mxAnsiCharIsUpper( char c )
{
	return c >= 'A' && c <= 'Z';
}

FORCEINLINE char mxAnsiCharToLower( char c ) {
	return ( c <= 'Z' && c >= 'A' )
		?
		( c + ( 'a' - 'A' ) )
		:
		( c );
}

FORCEINLINE char mxAnsiCharToUpper( char c ) {
	return ( c >= 'a' && c <= 'z' )
		?
		( c + ( 'a' - 'A' ) )
		:
		( c );
}


FORCEINLINE bool mxCharIsPathSeparator( char c ) {
	return c == PATHSEPARATOR_CHAR
		|| c == PATHSEPARATOR_CHAR2
		;
}

//------------------------------------------------------------------------
//	String manipulation
//------------------------------------------------------------------------

void mxReplaceCharANSI( ANSICHAR* str, UINT length, char old, char nw );

// reverses the string in place
void mxReverseStringANSI( ANSICHAR* str, UINT length );

// removes file extension (looking from end)
// returns length of new string
//
UINT mxStripFileExtensionANSI( char* s, UINT len );

UINT mxStripTrailingANSI(
	char* s, UINT len,
	char what );

UINT mxStripLeadingANSI(
	char* s, UINT len,
	const char* what, UINT l );

UINT mxStripLeadingCaselessANSI(
	char* s, UINT len,
	const char* what, UINT l );

UINT mxStripTrailingANSI(
	char* s, UINT len,
	const char* what, UINT l );

UINT mxStripTrailingCaselessANSI(
	char* s, UINT len,
	const char* what, UINT l );

//------------------------------------------------------------------------
//	String formatting functions
//------------------------------------------------------------------------

//
//	FormatArgListAnsi
//
inline void VARARGS FormatArgListAnsi( ANSICHAR *outputBuffer, SizeT characterCount, const ANSICHAR* formatString, va_list argList )
{
	MX_ASSERT_PTR( outputBuffer );
	mxASSERT( characterCount > 0 );
	MX_ASSERT_PTR( formatString );
	MX_ASSERT_PTR( argList );

#if MX_BOUNDS_CHECKS
	const HRESULT hResult =
#endif
	// use non-CRT thread safe function under Win32
	::StringCchVPrintfA( outputBuffer, characterCount, formatString, argList );

#if MX_BOUNDS_CHECKS
	if( FAILED(hResult) || HRESULT_CODE(hResult) == ERROR_INSUFFICIENT_BUFFER )
	{
		::DebugBreak();
	}
#endif
}

//
//	FormatArgListUnicode
//
inline void VARARGS FormatArgListUnicode( UNICODECHAR *outputBuffer, SizeT characterCount, const UNICODECHAR* formatString, va_list argList )
{
	MX_ASSERT_PTR( outputBuffer );
	mxASSERT( characterCount > 0 );
	MX_ASSERT_PTR( formatString );
	MX_ASSERT_PTR( argList );

#if MX_BOUNDS_CHECKS
	const HRESULT hResult =
#endif
	// safe, appends NUL
	::StringCchVPrintfW( outputBuffer, characterCount, formatString, argList );

#if MX_BOUNDS_CHECKS
	if( FAILED(hResult) || HRESULT_CODE(hResult) == ERROR_INSUFFICIENT_BUFFER )
	{
		::DebugBreak();
	}
#endif
}

//
//	MX_GET_VARARGS_ANSI
//
#define MX_GET_VARARGS_ANSI( buffer, fmt )	\
	{	\
		va_list	 argPtr;	\
		va_start( argPtr, fmt );	\
		FormatArgListAnsi( buffer, NUMBER_OF(buffer), fmt, argPtr );	\
		va_end( argPtr );	\
	}

//
//	MX_GET_VARARGS_UNICODE
//
#define MX_GET_VARARGS_UNICODE( buffer, fmt )	\
	{	\
		va_list	 argPtr;	\
		va_start( argPtr, fmt );	\
		FormatArgListUnicode( buffer, NUMBER_OF(buffer), fmt, argPtr );	\
		va_end( argPtr );	\
	}

#if UNICODE
#	define MX_GET_VARARGS	MX_GET_VARARGS_UNICODE
#else
#	define MX_GET_VARARGS	MX_GET_VARARGS_ANSI
#endif

INLINE void VARARGS mxSPrintfAnsi( ANSICHAR* buffer, SizeT numChars, const char* fmt, ... )
{
	va_list	 argPtr;
	va_start( argPtr, fmt );
	FormatArgListAnsi( buffer, numChars, fmt, argPtr );
	va_end( argPtr );
}

INLINE void VARARGS mxSPrintfUnicode( UNICODECHAR* buffer, SizeT numChars, const UNICODECHAR* fmt, ... )
{
	va_list	 argPtr;
	va_start( argPtr, fmt );
	_vsnwprintf( buffer, numChars, fmt, argPtr );
	va_end( argPtr );
}

#define MX_SPRINTF_ANSI( buf, fmt, ... )		mxSPrintfAnsi( buf, NUMBER_OF(buf), fmt, __VA_ARGS__ )
#define MX_SPRINTF_UNICODE( buf, fmt, ... )		mxSPrintfUnicode( buf, NUMBER_OF(buf), fmt, __VA_ARGS__ )

#if UNICODE
#	define MX_SPRINTF( buf, fmt, ... )	MX_SPRINTF_UNICODE( buf, fmt, __VA_ARGS__ )
#else
#	define MX_SPRINTF( buf, fmt, ... )	MX_SPRINTF_ANSI( buf, fmt, __VA_ARGS__ )
#endif


#if MX_DEBUG

	inline
	UINT mxSafeGetVarArgsANSI( ANSICHAR *outputBuffer, SizeT maxChars, const ANSICHAR* formatString, va_list argList )
	{
		FormatArgListAnsi( outputBuffer, maxChars, formatString, argList );
		return mxStrLenAnsi( outputBuffer );
	}

#else

	//
	//	mxSafeGetVarArgsANSI - returns the number of characters written or 0 if truncated.
	//	appends NULL to the output buffer.
	//
	inline
	UINT mxSafeGetVarArgsANSI( ANSICHAR *outputBuffer, SizeT maxChars, const ANSICHAR* formatString, va_list argList )
	{
		MX_ASSERT_PTR( outputBuffer );
		mxASSERT( maxChars > 0 );
		MX_ASSERT_PTR( formatString );
		MX_ASSERT_PTR( argList );

		// returns the number of characters written, not including the terminating null, or a negative value if an output error occurs.
		// causes a crash in case of buffer overflow
		INT result = _vsnprintf_s(
			outputBuffer,
			maxChars * sizeof(outputBuffer[0]),
			maxChars,
			formatString,
			argList
			);

	#if MX_BOUNDS_CHECKS
		if( result < 0 ) {
			::DebugBreak();
			return 0;
		}
	#endif

		return c_cast(UINT)result;
	}

#endif // !MX_DEBUG

	//
	//	MX_GET_VARARGS_ANSI_X
	//
	//	UINT 'newLength' - the number of characters written or 0 if truncated
	//
	#define MX_GET_VARARGS_ANSI_X( buffer, fmt, newLength )	\
	{	\
		va_list	 argPtr;	\
		va_start( argPtr, fmt );	\
		newLength = mxSafeGetVarArgsANSI( buffer, NUMBER_OF(buffer), fmt, argPtr );	\
		va_end( argPtr );	\
	}


//---------------------------------------------------------------

#define mxPrintfAnsi	::printf
#define mxPrintfUnicode	::wprintf

#if UNICODE
#	define mxPrintf		mxPrintfUnicode
#else
#	define mxPrintf		mxPrintfAnsi
#endif


//---------------------------------------------------------------

	inline bool BOOLToBool(BOOL value)
	{ return (value != FALSE); }

	inline BOOL BoolToBOOL(bool value)
	{ return (value ? TRUE: FALSE); }

/*
#ifdef _WIN32
	inline bool LRESULTToBool(LRESULT value)
	{ return (value != FALSE); }
#endif

	inline VARIANT_BOOL BoolToVARIANT_BOOL(bool value)
	{ return (value ? VARIANT_TRUE: VARIANT_FALSE); }

	inline bool VARIANT_BOOLToBool(VARIANT_BOOL value)
	{ return (value != VARIANT_FALSE); }
*/

//---------------------------------------------------------------

// Wrapper for Win32 HANDLEs that can/must be CloseHandle()d.
//
class Win32_AutoHandle {
public:
	inline Win32_AutoHandle(HANDLE h_=NULL)
		: mHandle(h_)
	{}
	inline ~Win32_AutoHandle()
	{
		this->Close();
	}
	inline operator HANDLE()
	{
		return mHandle;
	}
	inline bool Close()
	{
		if (mHandle == NULL)
			return true;
		if (!::CloseHandle(mHandle))
			return false;
		mHandle = NULL;
		return true;
	}

protected:
	HANDLE mHandle;
};

class Win32_FileMapping: public Win32_AutoHandle
{
public:
	bool Create(HANDLE file, LPSECURITY_ATTRIBUTES attributes,
		DWORD protect, UINT64 maximumSize, LPCTSTR name)
	{
		mHandle = ::CreateFileMapping(file, attributes,
			protect, DWORD(maximumSize >> 32), DWORD(maximumSize), name);
		return (mHandle != NULL);
	}

	bool Open(DWORD desiredAccess, bool inheritHandle, LPCTSTR name)
	{
		mHandle = ::OpenFileMapping(desiredAccess, BoolToBOOL(inheritHandle), name);
		return (mHandle != NULL);
	}

	LPVOID MapViewOfFile(DWORD desiredAccess, UINT64 fileOffset,
		SIZE_T numberOfBytesToMap)
	{
		return ::MapViewOfFile(mHandle, desiredAccess,
			DWORD(fileOffset >> 32), DWORD(fileOffset), numberOfBytesToMap);
	}

	LPVOID MapViewOfFileEx(DWORD desiredAccess, UINT64 fileOffset,
		SIZE_T numberOfBytesToMap, LPVOID baseAddress)
	{
		return ::MapViewOfFileEx(mHandle, desiredAccess,
			DWORD(fileOffset >> 32), DWORD(fileOffset),
			numberOfBytesToMap, baseAddress);
	}
};


mxSWIPED("Nebula3");
/*
Provides read-access to environment variables. Useful for tools.
NOTE: using this class restricts your code to the Win32 platform.
*/
class Win32Environment
{
public:
    /// return true if env-variable exists
    static bool Exists( const char* envVarName );
    /// get value of existing env-variable
    static void Read( const char* envVarName, char * outValue, UINT numChars );
};

void mxGetLastErrorString( char* buffer, UINT numChars );

void Win_GetWindowSize( HWND windowHandle, UINT &width, UINT &height );



mxNAMESPACE_END



//--------------------------------------------------------------//

mxSWIPED("OOOII, file: oString.h");

#define oTRACE(...)


// Use "-1" with unsigned values to indicate a bad/uninitialized state. Be 
// careful with size_t values because oINVALID will result in 0x00000000ffffffff
// on 64-bit systems. Use oINVALID_SIZE_T to ensure all bits are set.
#define oINVALID (~0u)

#ifdef _WIN64
	#define oINVALID_SIZE_T (~0ull)
#else
	#define oINVALID_SIZE_T (~0u)
#endif

#define oNEWLINE "\r\n"
#define oWHITESPACE " \t\v\f" oNEWLINE

#define oSAFESTR(str) ((str) ? (str) : "")
#define oSAFESTRN(str) ((str) ? (str) : "(null)")
#define oSTRNEXISTS(str) (str && str[0] != '\0')

#define oKB(n) (n * 1024LL)
#define oMB(n) (oKB(n) * 1024LL)
#define oGB(n) (oMB(n) * 1024LL)
#define oTB(n) (oGB(n) * 1024LL)

// Extensible pattern for uniform conversion of C++ objects (especially enums) to string. 
// All the user needs to do is define a function of the templated type to add a new type.
template<typename T> const char* oAsString(const T& _Object);

// Constants used throughout the code for asynchronous/time-based operations. Look to 
// comments on an API to understand when it is appropriate to use these.
const static unsigned int oINFINITE_WAIT = oINVALID;
const static unsigned int oTIMED_OUT = oINVALID;

#if CPP_0X

// #define all the colors out of this so that when it's no longer in the tr1 
// namespace, or if the implementation changes to boost, then we're ready here.
#define oFUNCTION std::tr1::function
#define oBIND std::tr1::bind
template<typename T> std::tr1::reference_wrapper<T> oBINDREF__(T& _Value) { return std::tr1::reference_wrapper<T>(_Value); }
#define oBINDREF oBINDREF__ // #defined so it's the same color as other oBIND elements for Visual Assist, et al.
#define oBIND1 std::tr1::placeholders::_1
#define oBIND2 std::tr1::placeholders::_2
#define oBIND3 std::tr1::placeholders::_3
#define oBIND4 std::tr1::placeholders::_4
#define oBIND5 std::tr1::placeholders::_5
#define oBIND6 std::tr1::placeholders::_6
#define oBIND7 std::tr1::placeholders::_7
#define oBIND8 std::tr1::placeholders::_8
#define oBIND9 std::tr1::placeholders::_9


// In several utility functions it is useful to see if a file exists (such as 
// going through a search path looking for a file, so define a common signature 
// here.
typedef oFUNCTION<bool(const char* _Path)> oPATH_EXISTS_FUNCTION;

// In several utility function it is useful to load an entire small file into
// a buffer, so define a common signature here.
typedef oFUNCTION<bool(void* _pDestination, size_t _SizeofDestination, const char* _Path)> oLOAD_BUFFER_FUNCTION;

#endif // CPP_0X


// 128 bit blob
struct uint128_t
{
	unsigned long long data[2];

	bool operator == (const uint128_t& other ) const
	{
		if( other.data[0] != data[0] )
			return false;

		if( other.data[1] != data[1] )
			return false;

		return true;
	}

	bool operator != (const uint128_t& other ) const
	{
		return !((*this) == other);
	}
};


struct oByteSwizzle16
{
	union
	{
		short AsShort;
		unsigned short AsUnsignedShort;
		char AsChar[2];
		unsigned char AsUnsignedChar[2];
	};
};

struct oByteSwizzle32
{
	union
	{
		float AsFloat;
		int AsInt;
		unsigned int AsUnsignedInt;
		short AsShort[2];
		unsigned short AsUnsignedShort[2];
		char AsChar[4];
		unsigned char AsUnsignedChar[4];
	};
};

struct oByteSwizzle64
{
	union
	{
		double AsDouble;
		long long AsLongLong;
		unsigned long long AsUnsignedLongLong;
		float AsFloat[2];
		int AsInt[2];
		unsigned int AsUnsignedInt[2];
		short AsShort[4];
		unsigned short AsUnsignedShort[4];
		char AsChar[8];
		unsigned char AsUnsignedChar[8];
	};
};

// _____________________________________________________________________________
// String cleanup

// Convert all characters of the specified string to lowercase (in-place)
void oToLower(char* _String);

// Convert all characters of the specified string to uppercase (in-place)
void oToUpper(char* _String);

// Convert \n -> \r\n
char* oNewlinesToDos(char* _StrDestination, size_t _SizeofStrDestination, const char* _StrSource);

// Remove all chars found in _ToTrim from the beginning of the string. _Trimmed 
// can be the same as _StrSource. Returns _Trimmed.
char* oTrimLeft(char* _Trimmed, size_t _SizeofTrimmed, const char* _StrSource, const char* _ToTrim = oWHITESPACE);

// Remove all chars found in _ToTrim form the end of the string. strDestination can be the same as strSource. Returns dst.
char* oTrimRight(char* _Trimmed, size_t _SizeofTrimmed, const char* _StrSource, const char* _ToTrim = oWHITESPACE);

// Trims both the left and right side of a string
inline char* oTrim(char* _Trimmed, size_t _SizeofTrimmed, const char* _StrSource, const char* _ToTrim = oWHITESPACE) { return oTrimRight(_Trimmed, _SizeofTrimmed, oTrimLeft(_Trimmed, _SizeofTrimmed, _StrSource, _ToTrim), _ToTrim); }

// Replaces any run of whitespace with a single ' ' character. Returns _StrDestination
char* oPruneWhitespace(char* _StrDestination, size_t _SizeofStrDestination, const char* _StrSource, char _Replacement = ' ', const char* _ToPrune = oWHITESPACE);

// Overwrites the specified buffer's last 4 bytes of capacity with "...\0" so if
// some strcpy were truncated, this would add a bit more visual sugar that the
// truncation took place.
char* oAddTruncationElipse(char* _StrDestination, size_t _SizeofStrDestination);

// replace all occurrences of strFind in strSource with strReplace and copy the result to strDestination
errno_t oReplace(char* _StrResult, size_t _SizeofStrResult, const char* _StrSource, const char* _StrFind, const char* _StrReplace);

// Search from the back of the string to find the specified substring
const char* oStrStrReverse(const char* _Str, const char* _SubStr);
char* oStrStrReverse(char* _Str, const char* _SubStr);

// Insert one string into another in-place. _InsertionPoint must point into 
// _StrSource If _ReplacementLength is non-zero, then that number of characters 
// from _InsertionPoint on will be overwritten by the _Insertion.
errno_t oInsert(char* _StrSource, size_t _SizeofStrResult, char* _InsertionPoint, size_t _ReplacementLength, const char* _Insertion);

// Essentially a variadic strcat_s
errno_t oVStrAppend(char* _StrDestination, size_t _SizeofStrDestination, const char* _Format, va_list _Args);
inline errno_t oStrAppend(char* _StrDestination, size_t _SizeofStrDestination, const char* _Format, ...) { va_list args; va_start(args, _Format); return oVStrAppend(_StrDestination, _SizeofStrDestination, _Format, args); }

// Returns the appropriate suffix [st nd rd th] for a number
const char* oOrdinal(int _Number);

// Fills the specified buffer with a size in either bytes, KB, MB, GB, or TB 
// depending on the number of bytes specified.
errno_t oFormatMemorySize(char* _StrDestination, size_t _SizeofStrDestination, unsigned long long _NumBytes, size_t _NumPrecisionDigits);

// Fills the specified buffer with a size in days hours minutes seconds
errno_t oFormatTimeSize(char* _StrDestination, size_t _SizeofStrDestination, double _TimeInSeconds);

// For numbers, this inserts commas where they ought to be (every 3 numbers)
errno_t oFormatCommas(char* _StrDestination, size_t _SizeofStrDestination, int _Number);

// Returns the nul-terminated string version of a fourcc code
char* oConvertFourcc(char* _StrDestination, size_t _SizeofStrDestination, int _Fourcc);

// _____________________________________________________________________________
// C++ Parsing (might be useful for other languages too)

// returns true for [A-Za-z0-9_]
inline bool oIsCppID(unsigned char c) { return isalnum(c) || c == '_'; }

// Move to the next id character, or one of the stop chars, whichever is first
const char* oMoveToNextID(const char* _pCurrent, const char* _Stop = "");
char* oMoveToNextID(char* _pCurrent, const char* _Stop = "");

// Returns the pointer into _TypeinfoName that represents just the name of the 
// user type, thus skipping any prefix [enum|class|struct|union]. This does not
// behave well for built-in types.
const char* oGetTypeName(const char* _TypeinfoName);

// first param is assumed to be pointing to the open brace. From there this will 
// find the  brace at the same level of recursion - internal pairs are ignored.
const char* oGetNextMatchingBrace(const char* _pPointingAtOpenBrace, char _CloseBrace);

// Same as above, but multi-char delimiters
const char* oGetNextMatchingBrace(const char* _pPointingAtOpenBrace, const char* _OpenBrace, const char* _CloseBrace);

struct oIFDEF_BLOCK
{
	enum TYPE
	{
		UNKNOWN,
		IFDEF,
		IFNDEF,
		IF,
		ELIF,
		ELSE,
		ENDIF,
	};

	TYPE Type;
	const char* ExpressionStart; // what comes after one of the opening TYPES. NULL for ELSE and ENDIF
	const char* ExpressionEnd;
	const char* BlockStart; // The data within the block
	const char* BlockEnd;
};

// Where *_ppStrSourceCode is pointing into a C++ style source code string, this
// will find the next #ifdef or #ifndef block (currently anything with #if #elif 
// blocks will break this code) and fill the specified array of blocks with where
// the internal strings begin and end. This does so in a way that recursive #if*
// statements are skipped and the list consists only of those at the same level
// as the original #if*. This stops searching beyond _StrSourceCodeEnd, or if
// NULL then to the end of the string. Up to the _MaxNumBlocks is filled in.
// Iterating through the result can be done until a Type is ENDIF, or using an
// index up to *_pNumValidBlocks. The ENDIF node's BlockEnd points immediately 
// after the #endif statement.
bool oGetNextMatchingIfdefBlocks(oIFDEF_BLOCK* _pBlocks, size_t _MaxNumBlocks, size_t* _pNumValidBlocks, const char* _StrSourceCodeBegin, const char* _StrSourceCodeEnd);

// Zeros-out the entire section delimited by the open and close braces, useful
// for getting rid of block comments or #if 0/#endif blocks
char* oZeroSection(char* _pPointingAtOpenBrace, const char* _OpenBrace, const char* _CloseBrace, char _Replacement = ' ');

// Like this very C++ comment! This function replaces the comment with the 
// specified char.
char* oZeroLineComments(char* _String, const char* _CommentPrefix, char _Replacement = ' ');

struct oMACRO
{
	const char* Symbol;
	const char* Value;
};

// This function uses the specified macros to go through and evaluate #if*'s 
// statements using oGetNextMatchingIfdefBlocks (at the time of this writing #if 
// and #elif are not supported) and zero out undefined code.
// The final macro should be { 0, 0 } as a nul terminator.
// Returns _StrSourceCode or NULL if there's a failure (check oGetLastError()).
char* oZeroIfdefs(char* _StrSourceCode, const oMACRO* _pMacros, char _Replacement = ' ');

// Walks through from start counting lines up until the specified line.
size_t oGetLineNumber(const char* _Start, const char* _Line);

// some compilers return the full name (struct foo, or class bar). This function
// returns simply the identifier from that.
const char* oGetSimpleTypename(const char* _TypeinfoName);

// Given the string that is returned from typeid(someStdVec).name(), return the 
// string that represents the typename held in the vector. Returns dst
char* oGetStdVectorType(char* _StrDestination, size_t _SizeofStrDestination, const char* _TypeinfoName);

// Fills strDestination with the file name of the next found include path
// context should be the address of the pointer to a string of C++ source
// code, and it will be updated to point just after the found header. This 
// returns false, when there are no more matches.
bool oGetNextInclude(char* _StrDestination, size_t _SizeofStrDestination, const char** _ppContext);

#if 0
// Given a buffer of source that uses #include statements, replace those 
// statements with the contents of the specified include files by using
// a user callback. The buffer must be large enough to accommodate all 
// merged includes. This can return EIO, meaning there was a problem in the 
// specified Load function or EINVAL if the specified buffer is too small to 
// hold the results of the merge.
errno_t oMergeIncludes(char* _StrSourceCode, size_t _SizeofStrSourceCode, const char* _SourceFullPath, oLOAD_BUFFER_FUNCTION _Load);
#endif
// Convert a buffer into a C++ array. This is useful when you want to embed data 
// in code itself. This fills the destination string with a declaration of the 
// form:
// const <specifiedType> <specifiedName>[] = { <buffer data> };
// This also defines a function of the form:
// void GetDesc<bufferName>(const char** ppBufferName, const void** ppBuffer, size_t* pSize)
// that can be externed and used to access the buffer. Any extension '.' in the
// specified bufferName will be replaced with '_', so GetDescMyFile_txt(...)

size_t oCodifyData(char* _StrDestination, size_t _SizeofStrDestination, const char* _BufferName, const void* _pBuffer, size_t _SizeofBuffer, size_t _WordSize);

bool oGetKeyValuePair(char* _KeyDestination, size_t _SizeofKeyDestination, char* _ValueDestination, size_t _SizeofValueDestination, char _KeyValueSeparator, const char* _KeyValuePairSeparators, const char* _SourceString, const char** _ppLeftOff = 0);

#if 0
// Walk through a C++ style source file and check all #include statements for 
// their date compared to the source file itself. _SourceFullPath is a semi-colon
// delimited list of paths.

// GetModifiedDate() should return the timestamp at which the file at the 
// specified full path was modified, or 0 if the file could not be found.

// NOTE: This function uses the specified _HeaderSearchPath for all headers 
// recursively which may include system headers, so ensure system paths are 
// specified in the search path as well, or optionally special-case certain
// filenames in the functions. For example, you could test for windows.h in each
// function and return true that it exists, return a modifed data of 0x1 so that
// it's a very very old/unchanged file, and LoadHeaderFile loads an empty file so
// that the algorithm doesn't have to recurse. NOTE: The array of macros must
// be NUL-terminated, meaning a value of {0,0} must be the last entry in the 
// oMACRO array.
bool oHeadersAreUpToDate(const char* _StrSourceCode, const char* _SourceFullPath, const oMACRO* _pMacros, oPATH_EXISTS_FUNCTION _PathExists, oFUNCTION<time_t(const char* _Path)> _GetModifiedDate, oLOAD_BUFFER_FUNCTION _LoadHeaderFile, const char* _HeaderSearchPath);
#endif
// _____________________________________________________________________________
// String tokenization

// Like strtok_s, except you can additionally specify open and close scope chars 
// to ignore. For example:
// char* ctx;
// const char* s = "myparams1(0, 1), myparams2(2, 3), myparams3(4, 5)"
// const char* token = oStrTok(s, ",", &ctx, "(", ")");
// while (token)
// {
//		token = oStrTok(0, ",", &ctx, "(", ")");
//		printf("%s\n", token);
// }
// Yields:
// myparams1(0, 1)
// myparams2(2, 3)
// myparams3(4, 5)
char* oStrTok(const char* _Token, const char* _Delimiter, char** _ppContext, const char* _OpenScopeChars = "", const char* _CloseScopeChars = "");

// Use this to clean up a oStrTok iteration if iterating through all tokens is 
// unnecessary. This is automatically called when oStrTok returns 0.
void oCloseStrTok(char** _ppContext);

// Open and close pairings might be mismatched, in which case oStrTok will 
// return 0 early, call oCloseStrTok automatically, but leave the context in a 
// state that can be queried with this function. The context is not really valid 
// (i.e. all memory has been freed).
bool oStrTokFinishedSuccessfully(char** _ppContext);

// _____________________________________________________________________________
// Command-line parsing

struct oOption
{
	const char* LongName;
	char ShortName;
	const char* ArgumentName;
	const char* Description;
};

// Similar to strtok, first call should specify argv and argc from main(), and
// subsequent calls should pass 0 to those values.
// This searches through the specified command line arguments and matches either 
// "--LongName" or "-ShortName" and fills *value with the value for the arg (or 
// if ArgumentName is NULL, meaning there is no arg, then *value gets the option 
// itself, or NULL if the options does not exist.)
// returns:
// if there is a match, the ShortName of the match
// 0 for no more matches
// ' ' for regular arguments (non-option values)
// '?' for an unrecognized option
// ':' if there was a missing option value
// to move through an entire argv list, each iteration should do argv++, argc--;
// options specified in an array somewhere must be terminated with an extra
// CMDLINE_OPTION entry that is all NULL/0 values.
char oOptTok(const char** _ppValue, int _Argc, const char* _Argv[], const oOption* _pOptions);

// Prints documentation for the specified options to the specified buffer.
char* oOptDoc(char* _StrDestination, size_t _SizeofStrDestination, const char* _AppName, const oOption* _pOptions);

// _____________________________________________________________________________
// Type to string and string to type functions. The following types are supported:

// Currently the following are supported. If more is desired, create new
// typed template<> implementations of this declaration.
//
// C++ types, including:
// bool
// (unsigned) char
// (unsigned) short
// (unsigned) long
// (unsigned) int
// (unsigned) long long
// float, double
//
// and also 
// half, oColor
// The user can define additional types by defining typed templates.

template<typename T> errno_t oToString(char* _StrDestination, size_t _SizeofStrDestination, const T& _Value);
template<typename T> errno_t oFromString(T* _pValue, const char* _StrSource);

#if 0
// Helper code assuming comma delineated vector of objects
template<typename T>
errno_t oFromString( std::vector<T>* _pVec, const char* _StrSource)
{
	char* ctx = NULL;
	const char* token = oStrTok(_StrSource, ",", &ctx);
	while (token)
	{
		T obj;
		errno_t res = oFromString(&obj, token );
		if( 0 != res )
		{
			oCloseStrTok(&ctx);
			_pVec->clear(); // Incomplete so clear
			return res;
		}

		_pVec->push_back(obj);
		token = oStrTok(0, ",", &ctx);
	}
	return 0;
}
#endif


// _____________________________________________________________________________
// Templated-on-size versions of the above API

template<size_t size> inline char* oNewlinesToDos(char (&_StrDestination)[size], const char* _StrSource) { return oNewlinesToDos(_StrDestination, size, _StrSource); }
template<size_t size> inline char* oTrimLeft(char (&_Trimmed)[size], const char* _StrSource, const char* _ToTrim = oWHITESPACE) { return oTrimLeft(_Trimmed, size, _StrSource, _ToTrim); }
template<size_t size> inline char* oTrimRight(char (&_Trimmed)[size], const char* _StrSource, const char* _ToTrim = oWHITESPACE) { return oTrimRight(_Trimmed, size, _StrSource, _ToTrim); }
template<size_t size> inline char* oTrim(char (&_Trimmed)[size], const char* _StrSource, const char* _ToTrim = oWHITESPACE) { return oTrim(_Trimmed, size, _StrSource, _ToTrim); }
template<size_t size> inline char* oPruneWhitespace(char (&_StrDestination)[size], const char* _StrSource, char _Replacement = ' ', const char* _ToPrune = oWHITESPACE) { return oPruneWhitespace(_StrDestination, size, _StrSource, _Replacement, _ToPrune); }
template<size_t size> inline char* oAddTruncationElipse(char (&_StrDestination)[size]) { return oAddTruncationElipse(_StrDestination, size); }
template<size_t size> inline size_t oStrConvert(char (&_StrDestination)[size], const wchar_t* _StrSource) { return oStrConvert(_StrDestination, size, _StrSource); }
template<size_t size> inline size_t oStrConvert(wchar_t (&_StrDestination)[size], const char* _StrSource) { return oStrConvert(_StrDestination, size, _StrSource); }
template<size_t size> inline errno_t oReplace(char (&_StrResult)[size], const char* _StrSource, const char* _StrFind, const char* _StrReplace) { return oReplace(_StrResult, size, _StrSource, _StrFind, _StrReplace); }
template<size_t size> inline errno_t oInsert(char (&_StrSource)[size], char* _InsertionPoint, size_t _ReplacementLength, const char* _Insertion) { return oInsert(_StrSource, size, _InsertionPoint, _ReplacementLength, _Insertion); }
template<size_t size> inline errno_t oVStrAppend(char (&_StrDestination)[size], const char* _Format, va_list _Args) { return oVStrAppend(_StrDestination, size, _Format, _Args); }
template<size_t size> inline errno_t oStrAppend(char (&_StrDestination)[size], const char* _Format, ...) { va_list args; va_start(args, _Format); return oVStrAppend(_StrDestination, size, _Format, args); }
template<size_t size> inline errno_t oFormatMemorySize(char (&_StrDestination)[size], unsigned long long _NumBytes, size_t _NumPrecisionDigits) { return oFormatMemorySize(_StrDestination, size, _NumBytes, _NumPrecisionDigits); }
template<size_t size> inline errno_t oFormatTimeSize(char (&_StrDestination)[size], double _TimeInSeconds) { return oFormatTimeSize(_StrDestination, size, _TimeInSeconds); }
template<size_t size> inline errno_t oFormatCommas(char (&_StrDestination)[size], int _Number) { return oFormatCommas(_StrDestination, size, _Number); }
template<size_t size> inline char* oConvertFourcc(char (&_StrDestination)[size], int _Fourcc) { return oConvertFourcc(_StrDestination, size, _Fourcc); }
template<size_t size> inline char* oGetStdVectorType(char (&_StrDestination)[size], const char* _TypeinfoName) { return oGetStdVectorType(_StrDestination, size, _TypeinfoName); }
template<size_t size> inline bool oGetNextInclude(char (&_StrDestination)[size], const char** _ppContext) { return oGetNextInclude(_StrDestination, size, _ppContext); }
template<size_t size> inline bool oGetNextMatchingIfdefBlocks(oIFDEF_BLOCK (&_pBlocks)[size], size_t* _pNumValidBlocks, const char* _StrSourceCodeBegin, const char* _StrSourceCodeEnd) { return oGetNextMatchingIfdefBlocks(_pBlocks, size, _pNumValidBlocks, _StrSourceCodeBegin, _StrSourceCodeEnd); }
#if 0
template<size_t size> inline errno_t oMergeIncludes(char (&_StrSourceCode)[size], const char* _SourceFullPath, oLOAD_BUFFER_FUNCTION _Load, char* _StrErrorMessage, size_t _SizeofStrErrorMessage) { return oMergeIncludes(_StrSourceCode, size, _SourceFullPath, Load, _StrErrorMessage, _SizeofStrErrorMessage); }
template<size_t size> inline errno_t oMergeIncludes(char* _StrSourceCode, size_t _SizeofStrSourceCode, const char* _SourceFullPath, oLOAD_BUFFER_FUNCTION Load, char (&_StrErrorMessage)[size]) { return oMergeIncludes(_StrSourceCode, size, _SourceFullPath, Load, _StrErrorMessage, size); }
template<size_t size, size_t errSize> inline errno_t oMergeIncludes(char (&_StrSourceCode)[size], const char* _SourceFullPath, oLOAD_BUFFER_FUNCTION Load, char (&_StrErrorMessage)[errSize]) { return oMergeIncludes(_StrSourceCode, size, _SourceFullPath, Load, _StrErrorMessage, errSize); }
#endif
template<size_t size> inline size_t oCodifyData(char (&_StrDestination)[size], const char* _BufferName, const void* _pBuffer, size_t _SizeofBuffer, size_t _WordSize) { return oCodifyData(_StrDestination, size, _BufferName, pBuffer, _SizeofBuffer, _WordSize); }
template<size_t size> char* oOptDoc(char (&_StrDestination)[size], const char* _AppName, const oOption* _pOptions) { return oOptDoc(_StrDestination, size, _AppName, _pOptions); }
template<size_t size, typename T> errno_t oToString(char (&_StrDestination)[size], const T& _Value) { return oToString(_StrDestination, size, _Value); }
template<size_t size> inline bool oGetKeyValuePair(char (&_KeyDestination)[size], char* _ValueDestination, size_t _SizeofValueDestination, char _KeyValueSeparator, const char* _KeyValuePairSeparators, const char* _SourceString, const char** _ppLeftOff = 0) { return oGetKeyValuePair(_KeyDestination, size, _ValueDestination, _SizeofValueDestination, _KeyValueSeparator, _KeyValuePairSeparators, _SourceString, _ppLeftOff); }
template<size_t size> inline bool oGetKeyValuePair(char* _KeyDestination, size_t _SizeofKeyDestination, char (&_ValueDestination)[size], char _KeyValueSeparator, const char* _KeyValuePairSeparators, const char* _SourceString, const char** _ppLeftOff = 0) { return oGetKeyValuePair(_KeyDestination, _SizeofKeyDestination, _ValueDestination, size, _KeyValueSeparator, _KeyValuePairSeparators, _SourceString, _ppLeftOff); }
template<size_t key_size, size_t value_size> inline bool oGetKeyValuePair(char (&_KeyDestination)[key_size], char (&_ValueDestination)[value_size], const char* _KeyValueSeparator, const char* _KeyValuePairSeparators, const char* _SourceString, const char** _ppLeftOff = 0) { return oGetKeyValuePair(_KeyDestination, key_size, _ValueDestination, value_size, _KeyValueSeparator, _KeyValuePairSeparators, _SourceString, _ppLeftOff); }


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
