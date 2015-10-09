/*
=============================================================================
	File:	Win32_Types.h
	Desc:	Win32 platform-specific types and some handy typedefs.
	ToDo:	Remove redundant types.
=============================================================================
*/

//
//	Character types.
//

// Characters must be signed !
#ifdef _CHAR_UNSIGNED
#	error Wrong project settings: The 'char' type must be signed!
#endif

typedef char		ANSICHAR;		// ANSI character.
typedef wchar_t		UNICODECHAR;	// Unicode character.

//
//	Default character type - this can be either 8-bit wide ANSI or 16-bit wide Unicode character.
//
#if UNICODE
	typedef UNICODECHAR				mxChar;
#else
	typedef char					mxChar;
#endif

// Shortcut for wide character type.
typedef wchar_t		WChar;

// Shortcut for ANSI character type.
typedef ANSICHAR	AChar;

//---------------------------------------------------------------

// Conversions from ANSI char to wide char type.
//
#define WIDEN2(x)		L ## x
#define WIDEN(x)		WIDEN2(x)

#define __WFILE__		WIDEN(__FILE__)
#define __WFUNCTION__	WIDEN(__FUNCTION__)

#ifndef TEXT
	#if UNICODE
		#define TEXT(str)	L##str
	#else
		#define TEXT(str)	str
	#endif
#endif // !TEXT

// Automatically converts string to multi-byte or unicode
#define mxTEXT(str)	TEXT(str)

// All string constants should be enclosed in mxSTRING()
#define mxSTRING(str)	(str)

// CHAR_BIT is the number of bits per byte (normally 8).
#ifndef CHAR_BIT
	#define CHAR_BIT 8
#endif // !CHAR_BIT


/**
Checks if parameter ch is a white-space character.
For the purpose of this function, standard white-space characters are:
' '		(0x20)	space (SPC)
'\t'	(0x09)	horizontal tab (TAB)
'\n'	(0x0a)	newline (LF)
'\v'	(0x0b)	vertical tab (VT)
'\f'	(0x0c)	feed (FF)
'\r'	(0x0d)	carriage return (CR)
*/
FORCEINLINE int mxCharIsSpace( const mxChar ch )
{
#if UNICODE
	return ::iswspace( ch );
#else
	return ::isspace( ch );
#endif
}

//
//	mxUnicodeToAnsi
//
//	Converts UNICODE string to ANSI string.
//
//	pSrc [in] : Pointer to the wide character string to convert.
//	pDest [out] : Pointer to a buffer that receives the converted string.
//	destSize [in] : Size, in bytes, of the buffer indicated by pDest.
//
FORCEINLINE
ANSICHAR* mxUnicodeToAnsi( ANSICHAR* pDest, const UNICODECHAR* pSrc, INT destChars )
{
	::WideCharToMultiByte(
		CP_ACP,				// UINT     CodePage, CP_ACP - The system default Windows ANSI code page.
		0,					// DWORD    dwFlags
		pSrc,				// LPCWSTR  lpWideCharStr
		-1,					// int      cchWideChar
		pDest,				// LPSTR   lpMultiByteStr
		destChars,			// int      cbMultiByte
		NULL,				// LPCSTR   lpDefaultChar
		NULL				// LPBOOL  lpUsedDefaultChar
	);
	return pDest;
}

//
//	mxAnsiToUnicode
//
//	Converts ANSI string to UNICODE string.
//
FORCEINLINE
UNICODECHAR* mxAnsiToUnicode( UNICODECHAR* pDest, const ANSICHAR* pSrc, INT destChars )
{
	::MultiByteToWideChar(
		CP_ACP,				// UINT     CodePage, CP_ACP - The system default Windows ANSI code page.
		0,					// DWORD    dwFlags
		pSrc,				// LPCSTR   lpMultiByteStr
		-1,					// int      cbMultiByte, -1 means null-terminated source string
		pDest,				// LPWSTR  lpWideCharStr
		destChars			// int      cchWideChar
	);
	return pDest;
}

// converts ANSI string to UNICODE string
#define mxANSI_TO_UNICODE( dest, src )	mxAnsiToUnicode( dest, src, NUMBER_OF(dest) )

// converts UNICODE string to ANSI string
#define mxUNICODE_TO_ANSI( dest, src )	mxUnicodeToAnsi( dest, src, NUMBER_OF(dest) )

//
//	Returns the size (in characters) of the corresponding ANSI string.
//
FORCEINLINE
INT mxGetLengthANSI( const UNICODECHAR* pSrc )
{
	return ::WideCharToMultiByte(
		CP_ACP,	// CP_ACP - The system default Windows ANSI code page.
		0,
		pSrc,
		-1,	// null-terminated string
		NULL,
		0,
		NULL,
		NULL
	);
}

//
//	Returns the size (in characters) of the corresponding UNICODE string.
//
FORCEINLINE
INT mxGetLengthUNICODE( const ANSICHAR* pSrc )
{
	return ::MultiByteToWideChar(
		CP_ACP,	// CP_ACP - The system default Windows ANSI code page.
		0,
		pSrc,
		-1,	// null-terminated string
		NULL,
		0
	);
}

mxUNSAFE
// converts ANSI string to UNICODE string; don't use for big strings or else it can crash;
// FIXME: the passed string's length is evaluated twice
//
#define mxTO_UNICODE( pSrc )	\
	mxAnsiToUnicode(	\
		(UNICODECHAR*)mxStackAlloc(sizeof(UNICODECHAR) * mxGetLengthUNICODE(pSrc)),	\
		(const ANSICHAR*)pSrc,	\
		mxGetLengthUNICODE(pSrc) )

mxUNSAFE
// converts UNICODE string to ANSI string; don't use for big strings or else it can crash;
// FIXME: the passed string's length is evaluated twice
//
#define mxTO_ANSI( pSrc )	\
	mxUnicodeToAnsi(	\
		(ANSICHAR*)mxStackAlloc(sizeof(ANSICHAR) * mxGetLengthANSI(pSrc)),	\
		(const UNICODECHAR*)pSrc,	\
		mxGetLengthANSI(pSrc) )

// converts a string of mxChar's
#if UNICODE
	#define mxCHARS_AS_ANSI( pSrc )		mxTO_ANSI( pSrc )
	#define mxCHARS_AS_UNICODE( pSrc )		pSrc
#else
	#define mxCHARS_AS_ANSI( pSrc )		pSrc
	#define mxCHARS_AS_UNICODE( pSrc )		mxTO_UNICODE( pSrc )
#endif

//---------------------------------------------------------------

//
//	Base integer types.
//
typedef unsigned __int8		BYTE;
typedef unsigned __int8		UBYTE;
typedef signed __int8		mxInt8;		// Signed 8-bit integer [-128..127].
typedef unsigned __int8		mxUInt8;	// Unsigned 8-bit integer [0..255].

typedef signed __int16		SWORD;
typedef unsigned __int16	UWORD;
typedef signed __int16		mxInt16;	// Signed 16-bit integer [-32 768..32 767].
typedef unsigned __int16	mxUInt16;	// Unsigned 16-bit integer [0..65 535].

typedef unsigned __int32	UDWORD;
typedef signed __int32		mxInt32;	// Signed 32-bit integer [-2 147 483 648..2 147 483 647].
typedef unsigned __int32	mxUInt32;	// Unsigned 32-bit integer [0..4 294 967 295].

// 64-bit wide integer types.
typedef unsigned __int64	UQWORD;
typedef signed __int64		mxInt64;	// Signed 64-bit integer [-9,223,372,036,854,775,808 .. 9,223,372,036,854,775,807].
typedef unsigned __int64	mxUInt64;	// Unsigned 64-bit integer [0 .. 18,446,744,073,709,551,615].

/*/ 128-bit wide integer types. They are not supported on 32-bit architectures.
typedef signed __int128		mxInt128;	// Signed 128-bit integer.
typedef unsigned __int128	mxUInt128;	// Unsigned 128-bit integer.
*/

// An integer type that is guaranteed to be the same size as a pointer.
typedef signed long			mxLong;
typedef unsigned long		mxULong;

// size_t is used for counts or ranges which need to span the range of a pointer
// ( e.g.: in memory copying routines. )
// Note: these should be used instead of ints for VERY LARGE arrays as loop counters/indexes.
typedef size_t			SizeT;
typedef ptrdiff_t		PtrDiffT;


//
// The most efficient integer types on this platform.
//
typedef signed int		mxInt;	// Must be the most efficient signed integer type on this platform ( the signed integer type used by default ).
typedef unsigned int	mxUInt;	// Must be the most efficient unsigned integer type on this platform ( the unsigned integer type used by default ).

//---------------------------------------------------------------

//
//	Low-level integer types.
//

typedef signed __int16		word;
typedef signed __int32		dword;
typedef signed __int64		qword;

typedef unsigned __int8		ubyte;
typedef unsigned __int16	uword;
typedef unsigned __int32	udword;
typedef unsigned __int64	uqword;

typedef unsigned char		byte;

/*
============================================================
These types are already defined in Windows headers.

typedef mxInt		INT;
typedef mxUInt		UINT;
typedef mxLong		LONG;
typedef mxULong		ULONG;

typedef mxInt16		INT16;
typedef mxUInt16	UINT16;

typedef mxInt32		INT32;
typedef mxUInt32	UINT32;
============================================================
*/

typedef mxInt8		INT8;
typedef mxUInt8		UINT8;

typedef mxInt64		INT64;
typedef mxUInt64	UINT64;

//
// Type ranges.
//
enum EIntegerTypeRanges
{
	MAX_SBYTE	= 0x7F,			// Maximum 8-bit signed integer.
	MAX_UBYTE	= 0xFFU,		// Maximum 8-bit unsigned integer.

	MAX_SWORD	= 0x7FFF,		// Maximum 16-bit signed integer.
	MAX_UWORD	= 0xFFFFU,		// Maximum 16-bit unsigned integer.

	MAX_SDWORD	= 0x7FFFFFFF,	// Maximum 32-bit signed integer.
	MAX_UDWORD	= 0xFFFFFFFFU,	// Maximum 32-bit unsigned integer.
};

// Maximum 64-bit signed integer.
#define MAX_SQWORD	0x7FFFFFFFFFFFFFFF

// Maximum 64-bit unsigned integer.
#define MAX_UQWORD	0xFFFFFFFFFFFFFFFU



// These are provided for convenience.


// Maximum 8-bit unsigned integer.
const UINT8		MAX_UINT8	= ((UINT8)~((UINT8)0));

// Maximum 8-bit signed integer.
const INT8		MAX_INT8	= ((INT8)(MAX_UINT8 >> 1));
// Minimum 8-bit signed integer.
const UINT8		MIN_INT8	= ((UINT8)~MAX_INT8);


// Maximum 16-bit unsigned integer.
const UINT16	MAX_UINT16	= ((UINT16)~((UINT16)0));

// Maximum 16-bit signed integer.
const INT16		MAX_INT16	= ((INT16)(MAX_UINT16 >> 1));
// Minimum 16-bit signed integer.
const UINT16	MIN_INT16	= ((INT16)~MAX_INT16);


// Maximum 32-bit unsigned integer.
const UINT32	MAX_UINT32	= ((UINT32)~((UINT32)0));

// Maximum 32-bit signed integer.
const INT32		MAX_INT32	= ((INT32)(MAX_UINT32 >> 1));
// Minimum 32-bit signed integer.
const INT32		MIN_INT32	= ((INT32)~MAX_INT32);


const UINT64	MAX_UINT64	= 0xFFFFFFFFFFFFFFFU;	// Maximum 64-bit unsigned integer.
const INT64		MAX_INT64	= 0x7FFFFFFFFFFFFFFF;	// Maximum 64-bit signed integer.
const INT64		MIN_INT64	= ((INT64)~MAX_INT64);	// Minimum 64-bit signed integer.

#define MAX_DWORD	MAX_INT32
#define MAX_LONG	MAX_INT32

//
//	Base IEEE-754 floating-point types.
//
typedef float		mxFloat32;	// Used when 32-bit floats are explicitly required. Range: -1.4023x10^-45__+3.4028x10^+38
typedef double		mxFloat64;	// Used when 64-bit floats are explicitly required. Range: -4.9406x10^-324__+1.7977x10^+308

// 80-bit extended precision floating point data type.
// Note: this type is not accessible through C/C++. Use asm for that.
//typedef long double	mxFloat80;

// 128-bit wide floating point types.
typedef __m128			mxSimdReal;	// only the first (x) element contains a valid value
typedef __m128			mxSimdQuad;	// all four elements are used

union mxQuadRealUnion
{
	float   f[4];
	__m128  quad;
};

// mxFloat32 : 32 bit IEEE float ( 1 sign , 8 exponent bits , 23 fraction bits )
// mxFloat64 : 64 bit IEEE float ( 1 sign , 11 exponent bits , 52 fraction bits )

typedef mxFloat32	mxReal;	// Default floating-point type.

/*
============================================================
These types are already defined in Windows headers.

typedef mxFloat32		FLOAT;	// Single-precision floating-point type (used by default in many cases).
*/
typedef mxFloat64		DOUBLE;	// Double-precision floating-point type.



#define MAX_FLOAT_32	3.402823466e+38F
#define MIN_FLOAT_32	1.175494351e-38F

#define MAX_FLOAT_64          179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0
#define MIN_FLOAT_64          2.225073858507201383090e-308
#define FLOAT_64_EPSILON      2.220446049250313080847e-16

//===============================================================

//
//	Boolean types.
//

//
//	mxBool - guaranteed to be 1 byte in size.
//
typedef BYTE	mxBool;

//
//	mxBool32 - guaranteed to be 32 bytes in size (for performance-related reasons).
//
// Thus comparisons like bool32 == true will not work as expected.
//
typedef INT32	mxBool32;

//
//	FASTBOOL - 'false' is zero, 'true' is any non-zero value.
//
// Thus comparisons like bool32 == true will not work as expected.
//
typedef int		FASTBOOL;

// 'false' is zero, 'true' is one.
typedef unsigned	mxBool01;




//===============================================================

//
// The null pointer type.
//
#ifndef NULL
#define NULL	0
#endif

#if !CPP_0X
#define nullptr	0
#endif

//#define null	0

// this was introduced to do less typing
#define nil		nullptr


// macro for converting pointers to boolean values
//
#if CPP_0X
	#define PtrToBool(pointer_to_variable)	\
		((pointer_to_variable) != nil)
#else
	#define PtrToBool(pointer_to_variable)	\
		(pointer_to_variable)
#endif

//===============================================================

mxNAMESPACE_BEGIN

typedef HWND WindowHandle;

#define INVALID_WINDOW_HANDLE	(WindowHandle)0

//
//	mxDateTime
//
struct mxDateTime
{
	UBYTE	year;	// years since 1900
	UBYTE	month;	// months since January [0..11]
	UBYTE	day;	// day of month [0..30]
	UBYTE	hour;	// hours since midnight [0..23]
	UBYTE	minute;	// [0..59]
	UBYTE	second;	// [0..59]

public:
	mxDateTime( int year, int month, int day, int hour, int minute, int second )
	{
		this->year = year - 1900;
		this->month = month;
		this->day = day;
		this->hour = hour;
		this->minute = minute;
		this->second = second;
	}
};

mxNAMESPACE_END


//===============================================================



//--------------------------------------------------------------
//	These are probably redundant, but i'm a lazy typist.
//	Besides, shorter names allow you to align your code nicely.
//--------------------------------------------------------------

// Base integer types.
typedef signed char                         S1;
typedef signed short                        S2;
typedef signed int                          S4;
typedef signed long long                    S8;
typedef unsigned char                       U1;
typedef unsigned short                      U2;
typedef unsigned int                        U4;
typedef unsigned long long                  U8;

// Boolean types.
typedef U1	B1;
typedef U4	B4;

// Pointers to base integer types.
typedef signed char *                       PS8;
typedef signed short *                      PS16;
typedef signed int *                        PS32;
typedef signed long long *                  PS64;
typedef unsigned char *                     PU8;
typedef unsigned short *                    PU16;
typedef unsigned int *                      PU32;
typedef unsigned long long *                PU64;
typedef float *                             PF32;
typedef double *                            PF64;
//typedef void *                              PVOID;	// name clash with Windows
// Pointers to constant base integer types.
typedef const signed char *                 PCS8;
typedef const signed short *                PCS16;
typedef const signed int *                  PCS32;
typedef const signed long long *            PCS64;
typedef const unsigned char *               PCU8;
typedef const unsigned short *              PCU16;
typedef const unsigned int *                PCU32;
typedef const unsigned long long *          PCU64;
typedef const float *                       PCF32;
typedef const double *                      PCF64;
//typedef const void *                        PCVOID;	// name clash with Windows
// Constant pointers to constant base integer types.
typedef const signed char * const           CPCS8;
typedef const signed short * const          CPCS16;
typedef const signed int * const            CPCS32;
typedef const signed long long * const      CPCS64;
typedef const unsigned char * const         CPCU8;
typedef const unsigned short * const        CPCU16;
typedef const unsigned int * const          CPCU32;
typedef const unsigned long long * const    CPCU64;
typedef const float * const                 CPCF32;
typedef const double * const                CPCF64;
typedef const void * const                  CPCVOID;

//typedef void **                             PPVOID;	// name clash with Windows

/*
#ifdef PBL_X64
	typedef signed long long                  S3264;
	typedef unsigned long long                U3264;
	typedef signed long long *                PS3264;
	typedef unsigned long long *              PU3264;
	typedef const signed long long *          PCS3264;
	typedef const unsigned long long *        PCU3264;
	typedef const signed long long * const    CPCS3264;
	typedef const unsigned long long * const  CPCU3264;
#else
	typedef signed int                        S3264;
	typedef unsigned int                      U3264;
	typedef signed int *                      PS3264;
	typedef unsigned int *                    PU3264;
	typedef const signed int *                PCS3264;
	typedef const unsigned int *              PCU3264;
	typedef const signed int * const          CPCS3264;
	typedef const unsigned int * const        CPCU3264;
#endif
*/


// used for filenames, etc.
//typedef char* PCHARS;
typedef const char* PCHARS;


// Base floating-point types.
typedef mxFloat32	F4;
typedef mxFloat64	F8;


template<int> struct TIntegerForSize;
template<>    struct TIntegerForSize<1> { typedef UINT8  Unsigned; typedef INT8  Signed; };
template<>    struct TIntegerForSize<2> { typedef UINT16 Unsigned; typedef INT16 Signed; };
template<>    struct TIntegerForSize<4> { typedef UINT32 Unsigned; typedef INT32 Signed; };
template<>    struct TIntegerForSize<8> { typedef UINT64 Unsigned; typedef INT64 Signed; };
template<class T> struct TIntegerForSizeof: TIntegerForSize<sizeof(T)> { };



#include <Base/System/common/CryHalf.inl>
#include <Base/System/common/CryHalf_branchfree.inl>

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
