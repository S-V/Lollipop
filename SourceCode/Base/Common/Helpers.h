/*
=============================================================================
	File:	Helpers.h
	Desc:	Platform-agnostic macros
			and various miscellaneous helpers
			used throughout the engine.
	Note:	Some of these tricks won't be needed when C++0x becomes widespread.
=============================================================================
*/

#ifndef __MX_HANDY_HELPERS_H__
#define __MX_HANDY_HELPERS_H__

//
//	ARRAY_SIZE
// Return the number of elements in a statically sized array.
//   DWORD Buffer[100];
//   RTL_NUMBER_OF(Buffer) == 100
// This is also popularly known as: NUMBER_OF, ARRSIZE, _countof, NELEM, etc.
// But it can lead to the following error:
// void Test(int C[3])
// {
//   int A[3];
//   int *B = Foo();
//   size_t x = count_of(A); // Ok
//   x = count_of(B); // Error
//   x = count_of(C); // Error
// }
//#define ARRAY_SIZE( pArray )		( sizeof( pArray ) / sizeof( pArray[0] ) )
#define ARRAY_SIZE( pArray )		NUMBER_OF( pArray )

// From "WinNT.h" :
//
// TCountOf is a function that takes a reference to an array of N Ts.
//
// typedef T array_of_T[N];
// typedef array_of_T &reference_to_array_of_T;
//
// TCountOf returns a pointer to an array of N chars.
// We could return a reference instead of a pointer but older compilers do not accept that.
//
// typedef char array_of_char[N];
// typedef array_of_char *pointer_to_array_of_char;
//
// sizeof(array_of_char) == N
// sizeof(*pointer_to_array_of_char) == N
//
// pointer_to_array_of_char TCountOf(reference_to_array_of_T);
//
// We never even call TCountOf, we just take the size of dereferencing its return type.
// We do not even implement TCountOf, we just declare it.
//
// Attempts to pass pointers instead of arrays to this macro result in compile time errors.
// That is the point.
//
extern "C++" // templates cannot be declared to have 'C' linkage
template< typename T, size_t N >	// accepts a reference to an array of N T's 
char (*ArraySizeHelper( /*UNALIGNED*/ T (&)[N] ))[N];	// returns a pointer to an array of N chars

#define NUMBER_OF(A)	(sizeof(*ArraySizeHelper(A)))

//
// This does not work with:
//
// void Foo()
// {
//    struct { int x; } y[2];
//    NUMBER_OF(y); // illegal use of anonymous local type in template instantiation
// }
//
// You must instead do:
//
// struct Foo1 { int x; };
//
// void Foo()
// {
//    Foo1 y[2];
//    NUMBER_OF(y); // ok
// }
//
// OR
//
// void Foo()
// {
//    struct { int x; } y[2];
//    ARRAY_SIZE(y); // ok
// }
//
// OR
//
// void Foo()
// {
//    struct { int x; } y[2];
//    NUMBER_OF(y); // ok
// }
//

/*
template< typename T, size_t N >
size_t CountOf( const T (&) [N] )	{ return N; }
*/

/*
	This comes from Havok:

	struct hkCountOfBadArgCheck
	{
		class ArgIsNotAnArray;
		template<typename T> static ArgIsNotAnArray isArrayType(const T*, const T* const*);
		static int isArrayType(const void*, const void*);
	};

		/// Returns the number of elements in the C array.
	#define HK_COUNT_OF(x) ( \
		0 * sizeof( reinterpret_cast<const ::hkCountOfBadArgCheck*>(x) ) + \
		0 * sizeof( ::hkCountOfBadArgCheck::isArrayType((x), &(x)) ) + \
		sizeof(x) / sizeof((x)[0]) ) 
*/

/*
====================================================================================
These are commonly used to define multi-statement (multi-line) macros in C/C++
(that are going to be used in function-scope).

e.g.

Suppose, we #define the following:

#define FUNC(X) Something1; \
				Something2

If it's used like this...

	if( a == 0 ) 
		FUNC(X);

...then we have a bug:

	'Something1' will be executed only if the condition is true,
	but 'Something2' will ALWAYS be run!

To protect against these errors such can be used:

#define FUNC(X)\
do{\
statement;\
...\
statement;\
}while(0)

====================================================================================
*/
#define mxMACRO_BEGIN	do{
#define mxMACRO_END	}while(0)

//
//	SIZE_OF_THIS
//
#define SIZE_OF_THIS	sizeof(*this)

//
//	OFFSET_OF( _struct, member ) - The byte offset of a field in a structure.
//
#define OFFSET_OF( _struct, member )		((long)(long*)&((_struct*)0)->member)

// A check for whether the offset of a member within a structure is as expected
#define OFFSET_EQUALS(CLASS,MEMBER,OFFSET) (OFFSET_OF(CLASS,MEMBER)==OFFSET)


//
//	FIELD_SIZE( _struct, member ) - The size of a field in a structure.
//
#define FIELD_SIZE( _struct, member )		sizeof( ((_struct*)0)->member )



//===========================================================================
//
// GET_OUTER( OuterType, OuterMember )
//
// A platform-independent way for a contained class to get a pointer to its
// owner. If you know a class is exclusively used in the context of some
// "outer" class, this is a much more space efficient way to get at the outer
// class than having the inner class store a pointer to it.
//
//	This comes from Source Engine, Valve.
//
//	class COuter
//	{
//		class CInner // Note: this does not need to be a nested class to work
//		{
//			void PrintAddressOfOuter()
//			{
//				printf( "Outer is at 0x%x\n", GET_OUTER( COuter, m_Inner ) );
//			}
//		};
//		
//		CInner m_Inner;
//		friend class CInner;
//	};

#define GET_OUTER( OuterType, OuterMember ) \
	( ( OuterType * ) ( (byte*)this - OFFSET_OF( OuterType, OuterMember ) ) )

//-------------------------------------------------------------
/*
//
// Bit - single-bit bitmask.
//
template< SizeT N >
class Bit {
	enum { mask = (1 << N) };
};
*/
#ifndef BIT
#define BIT(n)		(1 << (n))
#endif

#define BITS_NONE	(0)
#define BITS_ALL	(-1)

#define TEST_BIT( x, bit )		((x) & (bit))
#define SET_BITS( x, bits )		((x) = (x) | (bits))
#define CLEAR_BITS( x, bits )	((x) = (x) & ~(bits))
#define FLIP_BITS( x, bits )	((x) = (x) ^ (bits))

#define BITS_IN_BYTE		(8)
#define BYTES_TO_BITS(x)	((x)<<3)
#define BITS_TO_BYTES(x)	(((x)+7)>>3)

//-------------------------------------------------------------

mxNAMESPACE_BEGIN

//
//	TPow2 - Template to compute 2 to the Nth power at compile-time.
//
template< unsigned N >
struct TPow2
{
	//enum { value = 2 * TPow2<N-1>::value };
	//changed enums to longs because of MVC++ warning: C4307: '*' : integral constant overflow.
	static const unsigned long long value = 2 * TPow2<N-1>::value;
};
template<>
struct TPow2<0>
{
	//enum { value = 1 };
	static const unsigned long long  value = 1;
};

//
//	TIsPowerOfTwo
//
template< unsigned long x >
struct TIsPowerOfTwo
{
	enum { value = (((x) & ((x) - 1)) == 0) };
};

//
//	TLog2
//
template< unsigned long NUM >
struct TLog2
{
	enum
	{
		Value = TLog2<NUM / 2>::value + 1
	};
	typedef char NUM_MustBePowerOfTwo[((NUM & (NUM - 1)) == 0 ? 1 : -1)];
};
template<> struct TLog2< 1 >
{
	enum { value = 0 };
};

mxNAMESPACE_END

//-------------------------------------------------------------

#define mxARGB(a,r,g,b)			((UINT)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define mxRGBA(r,g,b,a)			mxARGB(a,r,g,b)
#define mxXRGB(r,g,b)			mxARGB(0xff,r,g,b)
#define mxCOLORVALUE(r,g,b,a)	mxRGBA((UINT)((r)*255.f),(UINT)((g)*255.f),(UINT)((b)*255.f),(UINT)((a)*255.f))


#define MCHAR2( a, b )			( a | (b << 8) )
#define MCHAR4( a, b, c, d )	( a | (b << 8) | (c << 16) | (d << 24) )


#define GET_VERSION_MAJOR(version) (version >> 8)
#define GET_VERSION_MINOR(version) (version & 0xFF)
#define MAKE_VERSION(major, minor) (((major)<<8) + (minor))
#define GET_VERSION(version, major, minor) ( (major) = GET_VERSION_MAJOR(version); (minor) = GET_VERSION_MINOR(version); )


//! Evaluates to the high byte of x. x is expected to be of type U16.
#define HI_BYTE(x) (BYTE)((x) >> 8)

//! Evaluates to the low byte of x. x is expected to be of type U16.
#define LO_BYTE(x) (BYTE)((x) & 0xff)

//! Evaluates to the 16 bit value specified by x and y in little endian order (low, high).
#define LO_HI(x,y) (UINT16)((y) << 8 | (x))


#if defined(smallest) || defined(largest)
#error Shouldn't happen
#endif

#undef smallest
#define smallest(a,b)            (((a) < (b)) ? (a) : (b))

#undef largest
#define largest(a,b)            (((a) > (b)) ? (a) : (b))

//-------------------------------------------------------------

//#define OVERRIDES( BaseClassName )	virtual
//#define OVERRIDEN					virtual

//// Microsoft Visual C++ compiler has a special keyword for this purpose - '__super'.
//#define DECLARE_CLASS( className, baseClassName )	\
//	typedef className THIS_TYPE;	\
//	typedef baseClassName BASE_TYPE;	\
//	typedef baseClassName Super;


// it says: 'unsure about whether this function should be inlined or not'
#define mxINLINE	inline

//-------------------------------------------------------------
//	Usage:
//		NO_COPY_CONSTRUCTOR( ClassName );
//		NO_ASSIGNMENT( ClassName );
//		NO_COMPARES( ClassName );
//

#define NO_DEFAULT_CONSTRUCTOR( ClassName )\
	protected: ClassName() {}

#define NO_COPY_CONSTRUCTOR( ClassName );	\
	private: ClassName( const ClassName & );	// Do NOT implement!

#define NO_ASSIGNMENT( ClassName );			\
	private: ClassName & operator = ( const ClassName & );	// Do NOT implement!

#define NO_COMPARES( ClassName );			\
	private: ClassName & operator == ( const ClassName & );	// Do NOT implement!

#define PREVENT_COPY( ClassName );			\
	NO_COPY_CONSTRUCTOR( ClassName );		\
	NO_ASSIGNMENT( ClassName );				\
	NO_COMPARES( ClassName );


//-------------------------------------------------------------

//
//	Utility class, you can inherit from this class
//	to disallow the assignment operator and copy construction.
//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.
//
struct NonCopyable {
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:  // emphasize the following members are private
	NonCopyable( const NonCopyable& );
	const NonCopyable& operator=( const NonCopyable& );
};


//---------------------------------------------------------------------------
// Quick const-manipulation macros

// Declare a const and variable version of a function simultaneously.
#define CONST_VAR_FUNCTION(head, body) \
	inline head body \
	inline const head const body

template<class T> inline
T& make_non_const(const T& t)
	{ return const_cast<T&>(t); }

#define using_type(super, type) \
	typedef typename super::type type;


//-------------------------------------------------------------
//	Preprocessor library.
//-------------------------------------------------------------


// Convert the expanded result of a macro to a char string.
// Expands the expression before stringifying it. See:
// http://c-faq.com/ansi/stringize.html
#define TO_STR2(X)		#X

// Creates a string from the given expression ('Stringize','Stringify').
// stringizes even macros
//
#define TO_STR( x )		TO_STR2( x )


#define STRINGIZE_INDIRECT( F, X )	F(X)
#define __LINESTR__					STRINGIZE_INDIRECT(TO_STR, __LINE__)
#define __FILELINEFUNC__			(__FILE__ " " __LINESTR__ " " __FUNCTION__)

//#define __FUNCLINE__				( __FUNCTION__ " " __LINESTR__ )
#define __FUNCLINE__				( __FUNCTION__ " (" __LINESTR__ ")" )



// Join two preprocessor tokens, even when a token is itself a macro.
#define PP_JOIN_TOKEN(A,B) PP_JOIN_TOKEN2(A,B)
#define PP_JOIN_TOKEN2(A,B) PP_JOIN_TOKEN3(A,B)
// PREPROCESSOR: concatenates two strings, even when the strings are macros themselves
#define PP_JOIN_TOKEN3(A,B) A##B

/*
// Counts the number of arguments in the given variadic macro.
// The C Standard specifies that at least one argument must be passed to the ellipsis,
// to ensure that the macro does not resolve to an expression with a trailing comma.
// Note: ##__VA_ARGS__ eats comma in case of empty arg list
// works for [1 - 16], doesn't work if num args is zero
//
#define VA_NUM_ARGS(...)			VA_NUM_ARGS_IMPL_((__VA_ARGS__, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))
#define VA_NUM_ARGS_IMPL_(tuple)	VA_NUM_ARGS_IMPL tuple
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16, N, ... )	N

// how it works:
//	VA_NUM_ARGS(...)
//	e.g.
//	VA_NUM_ARGS(x1,x2,x3) -> VA_NUM_ARGS_IMPL_( x1,x2,x3, 5,4,3,2,1 )
//	-> VA_NUM_ARGS_IMPL x1,x2,x3, 5,4,3,2,1	// _1,_2,_3,_4,_5, stand for x1,x2,x3, 5,4, => N is 3
//	-> 3

// it could be done simply as
#define MX_VA_NUM_ARGS(...)                        MX_VA_NUM_ARGS_HELPER(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define MX_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
*/

// Unfortunately, even Visual Studio 2010 has this bug
// which treats a __VA_ARGS__ argument as being one single parameter.
// Hence, we have to work around this bug using the following piece of macro magic:

// MX_VA_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro
// unfortunately, VS 2010 still has this compiler bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
#    define MX_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#    define MX_VA_NUM_ARGS_REVERSE_SEQUENCE            10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#    define MX_LEFT_PARENTHESIS (
#    define MX_RIGHT_PARENTHESIS )
#    define MX_VA_NUM_ARGS(...)                        MX_VA_NUM_ARGS_HELPER MX_LEFT_PARENTHESIS __VA_ARGS__, MX_VA_NUM_ARGS_REVERSE_SEQUENCE MX_RIGHT_PARENTHESIS
#else
#    define MX_VA_NUM_ARGS(...)                        MX_VA_NUM_ARGS_HELPER(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#    define MX_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#endif

// MX_PASS_VA passes __VA_ARGS__ as multiple parameters to another macro, working around the above-mentioned bug
#if _MSC_VER >= 1400
#    define MX_PASS_VA(...)                            MX_LEFT_PARENTHESIS __VA_ARGS__ MX_RIGHT_PARENTHESIS
#else
#    define MX_PASS_VA(...)                            (__VA_ARGS__)
#endif

/* example usage:
int c1 = MX_VA_NUM_ARGS(x);	// 1
int c3 = MX_VA_NUM_ARGS(x,y,z);	// 3
int c10 = MX_VA_NUM_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10);	// 10
*/





#define mxGETTER(name, type, var)					\
	type Get##name() const { return var; }

#define mxSETTER(name, type, var)					\
	void Set##name(type v) { var = v; }

#define mxACCESSOR(name, type, var)				\
	mxGETTER(name, type, var)						\
	mxSETTER(name, type, var)


//-------------------------------------------------------------
//	Control statements.
//-------------------------------------------------------------

// should be used in switch statements to indicate fall-through
#define mxCASE_FALLTHROUGH		;

// In cases where no default is present or appropriate, this causes MSVC to generate
// as little code as possible, and throw an assertion in debug.
//
#define mxNO_SWITCH_DEFAULT	default: Unreachable;

#define mxSTATE_CASE_STRING(x)      case x: return #x;

// if the given expression returned 'false' then return 'false'
#define VRET_FALSE_IF_NOT(expr)		if(!(expr))	return false;
#define VRET_FALSE_IF_NIL(expr)		if( (expr) == nil )	return false;

#define VRET_IF_NOT(expr)			if(!(expr))	return;
#define VRET_IF_NIL(expr)			if( (expr) == nil )	return;

#define VRET_X_IF_NOT(expr, retVal)		if(!(expr))	return (retVal);
#define VRET_X_IF_NIL(expr, retVal)		if( (expr) == nil )	return (retVal);

#define VRET_NIL_IF_NOT(expr)		if(!(expr))	return nil;
#define VRET_NIL_IF_NIL(expr)		if( (expr) == nil )	return nil;


#define CHK_VRET_FALSE_IF_NOT(expr)\
	{\
		const bool bOk = (expr);\
		AssertX(bOk, #expr);\
		if( !bOk )	return false;\
	}

#define CHK_VRET_FALSE_IF_NIL(expr)\
	{\
		const bool bOk = ((expr) != nil);\
		AssertX(bOk, #expr);\
		if( !bOk )	return false;\
	}


#define CHK_VRET_IF_NOT(expr)\
	{\
		const bool bOk = (expr);\
		AssertX(bOk, #expr);\
		if( !bOk )	return;\
	}

#define CHK_VRET_IF_NIL(expr)\
	{\
		const bool bOk = ((expr) != nil);\
		AssertX(bOk, #expr);\
		if( !bOk )	return;\
	}



#define CHK_VRET_NIL_IF_NOT(expr)\
	{\
		const bool bOk = (expr);\
		AssertX(bOk, #expr);\
		if( !bOk )	return nil;\
	}

#define CHK_VRET_NIL_IF_NIL(expr)\
	{\
		const bool bOk = ((expr) != nil);\
		AssertX(bOk, #expr);\
		if( !bOk )	return nil;\
	}


#define CHK_VRET_X_IF_NOT(expr, retVal)\
	{\
		const bool bOk = (expr);\
		AssertX(bOk, #expr);\
		if( !bOk )	return (retVal);\
	}


#define CHK_VRET_X_IF_NIL(expr, retVal)\
	{\
		const bool bOk = ((expr) != nil);\
		AssertX(bOk, #expr);\
		if( !bOk )	return (retVal);\
	}


//-------------------------------------------------------------
//	Evil hacks.
//-------------------------------------------------------------


#if 0//MX_DEVELOPER && MX_DEBUG

// This can be used to access private and protected variables.

#define private		public
#define protected	public

// messes up with std::
//#define class		struct

//#define const_cast		"const_cast<> is prohibited"

#endif


//-------------------------------------------------------------
//	Virtual member functions.
//-------------------------------------------------------------

// abstract overridable method
#define PURE_VIRTUAL		=0

//
// ...because i'm always eager to instantiate my classes and test them;
// it's easy to replace these macros with " = 0" (or with "" in derived classes)
// which would hopefully be done once i got a very stable code base
//

// for abstract methods that return 'void'
#define _PURE_METHOD_STUB			{ Unimplemented; }

// for abstract methods that should return a pointer
// (i.e. this should be "#define _PURE_METHOD_STUB_RET_NULL  = 0;")
#define _PURE_METHOD_STUB_RET_NULL	{ Unimplemented; return nil; }


//-------------------------------------------------------------
//	Loops.
//-------------------------------------------------------------


#define mxINT_LOOP( var, num )		for( INT var = 0; var < (INT)num; var++ )
#define mxINT_LOOP_i( num )		mxINT_LOOP( i, num )
#define mxINT_LOOP_j( num )		mxINT_LOOP( j, num )
#define mxINT_LOOP_k( num )		mxINT_LOOP( k, num )

// NOTE: potential bug - if (INT)num is -1 then (UINT)num is UINT_MAX (0xFFFF...)
#define MX_UINT_LOOP( var, num )	for( UINT var = 0; var < (UINT)num; var++ )
#define MX_UINT_LOOP_i( num )		MX_UINT_LOOP( i, num )
#define MX_UINT_LOOP_j( num )		MX_UINT_LOOP( j, num )
#define MX_UINT_LOOP_k( num )		MX_UINT_LOOP( k, num )


// use for(;;) instead of while(true)
// because true can be #defined to false, etc.
//
#define mxLOOP_FOREVER		for(;;)


// TODO: for_each ?


//===========================================================================
/*
	macros for initializing arrays statically.

	e.g.
	enum { MAX_SCOPE_DEPTH = 32 };
	const char MyTextWriter::tabs[ MAX_SCOPE_DEPTH ] = { VAL_32X };

	you should define VAL_1X first.
	don't forget to call #undef for preprocessor clean-up.

	initialize left-overs manually if needed.

	Be careful when array size changes!

	Or use something like this:
	const char MyTextWriter::tabs[] = { VAL_32X };
*/

// You should #define VAL_1X yourself!

#define VAL_2X     VAL_1X,  VAL_1X
#define VAL_4X     VAL_2X,  VAL_2X
#define VAL_8X     VAL_4X,  VAL_4X
#define VAL_16X    VAL_8X,  VAL_8X
#define VAL_32X    VAL_16X, VAL_16X
#define VAL_64X    VAL_32X, VAL_32X
#define VAL_128X   VAL_64X, VAL_64X
#define VAL_256X   VAL_128X, VAL_128X
#define VAL_512X   VAL_256X, VAL_256X
#define VAL_1024X  VAL_512X, VAL_512X
#define VAL_2048X  VAL_1024X, VAL_1024X

//===========================================================================

enum HEX_NUMBERS
{
	HEX_0000 = 0x0,
	HEX_0001 = 0x1,
	HEX_0010 = 0x2,
	HEX_0011 = 0x3,
	HEX_0100 = 0x4,
	HEX_0101 = 0x5,
	HEX_0110 = 0x6,
	HEX_0111 = 0x7,
	HEX_1000 = 0x8,
	HEX_1001 = 0x9,
	HEX_1010 = 0xA,
	HEX_1011 = 0xB,
	HEX_1100 = 0xC,
	HEX_1101 = 0xD,
	HEX_1110 = 0xE,
	HEX_1111 = 0xF,
};


/*
	Some hexadecimal constants,
	can be be used as magic numbers
	(search for Hexspeak):

	0x00C0FFEE
	0xBABE
	0xCAFEBABE
	0xD15EA5E
	0xDEADBABE
	0xDEADFA11
	0xDEADFACE
	0xBEEF
	0x0BAD
	0x0BADCODE
	0x0BADF00D
	0xFACEFEED
	0xFEE1DEAD
	0xDEFECA7E

	0x00FACADE
	0xBABE5A55
	0x5ADCA5E5
*/

// used to refer to integer constants by names instead of (obscure) numbers
#define mxKIBIBYTE	(1<<10)
#define mxMEBIBYTE	(1<<20)
#define mxGIBIBYTE	(1<<30)

// these are used for historical reasons
#define mxKILOBYTE	(1<<10)
#define mxMEGABYTE	(1<<20)
#define mxGIGABYTE	(1<<30)

//===========================================================================
//
//	Code markers.
//

// Macro to avoid unused parameter warnings (inhibits warnings about unreferenced variables,
// makes sure the specified variable "is used" in some way so that the compiler does not give a "unused parameter" or similar warning).
// This macro may be useful whenever you want to declare a variable, but (by intention) never use it. For example for documentation purposes.
//
#define mxUNUSED( x )		((void)( &x ))
/*
// For preventing compiler warnings "unreferenced formal parameters".
template< typename T >
void UnusedParameter( const T& param )
{ (void)param; }
*/


// so that all ugly casts can be found with a text editor
#define c_cast(x)		(x)


template< typename FROM, typename TO >
inline TO implicit_cast( FROM const &x )
{
	return x;
}


/*
#define and		&& 
#define and_eq	&=
#define bitor	| 
#define or		||
#define or_eq	|=
#define xor_eq	^= 
#define xor		^ 
#define not		!
#define compl	~ 
#define not_eq	!= 
#define bitand	& 
*/

//	Global variables.
#define global_

//	Static variables (in function or file scope), this is usually used to mark dirty hacks.
#define local_		static

//	Static members of a class shared by all instanced of the class.
#define shared_		static

// access qualifiers of class members
#define public_internal		public
#define protected_internal	protected
#define private_internal	private

#define public_readonly		public
#define protected_readonly	protected
#define private_readonly	private

// remember: 'const' is a "virus".
#define const_

#define in_	
#define out_
#define inout_

// thread safety
#define mt_safe
#define mt_unsafe

// Use volatile correctness on methods to indicate the thread safety
// of their calls. This is not the same as the pointless effort of 
// declaring data volatile. Here, a class pointer should be declared
// volatile to disable non-volatile method calls the same way declaring
// a class pointer const disables all methods not labelled const. On
// methods, volatile doesn't have any assembly-level memory access 
// fencing implications - it's just a flag that hopefully can move some
// runtime errors of calling unsafe methods to compile time.
#ifndef threadsafe
#define threadsafe volatile
#endif

// Create a different type of cast so that when inspecting usages the 
// user can see when it is intended that the code is casting away thread-
// safety.
template<typename T, typename U> inline T thread_cast(const U& threadsafeObject) { return const_cast<T>(threadsafeObject); }


// used to mark optional pointers (which can be null)
#define OptPtr( typeName )		typeName*

// replace with 'me'?
/*
#ifndef self
#define self	(*this)
#else
#error	'self' shoudn't be defined prior to this point!
#endif
*/

#define sealed

//-------------------------------------------------------------
//	Markers (notes to the developer).
//-------------------------------------------------------------
//
//	Pros: bookmarks are not needed;
//		  easy to "Find All References" and no comments needed;
//		  you can #define them to what you like (e.g. #pragma message).
//	Cons: changes to the source file require recompiling;
//		  time & date have to be inserted manually.
//

// Means there's more to do here, don't forget.
#define mxTODO( message_string )
#define mxUNDONE

#define mxREFACTOR( message_string )
#define mxREMOVE_OLD_CODE

// the code is new (usually, to try out a 'genious' idea), it's not well tested and prone to bugs
#define mxEXPERIMENTAL

//means there's a known bug here, explain it and optionally give a bug ID.
#define mxBUG( message_string )
#define mxFIXME( message_string )

//KLUDGE - When you've done something ugly say so and explain how you would do it differently next time if you had more time.
#define mxHACK( message_string )
#define mxREMOVE_THIS
#define mxUGLY
//deprecated
#define mxOBSOLETE

// Tells somebody that the following code is very tricky so don't go changing it without thinking.
#define mxNOTE( message_string )

#define mxOPTIMIZE( message_string )

// used to mark code that has been stolen from somewhere (usually, i nicked it for self-education)
#define mxSWIPED( message_string )

// Beware of something.
#define mxWARNING( message_string )

// unsafe code (amenable to buffer overruns, etc)
#define mxUNSAFE

// Sometimes you need to work around a compiler problem. Document it. The problem may go away eventually.
#define mxCOMPILER_PROBLEM( message_string )

// e.g.: fresh and buggy drivers for graphics card, bugs in new DirectX 11 debug layer
#define mxPLATFORM_PROBLEM( message_string )

// used to mark code that is not thread-safe
#define mxMT_UNSAFE

// code should be cleaned up
#define mxTIDY_UP_FILE

// quick hacks that should be removed
#define mxTEMP

// long-term hacks
#define mxPERM( message_string )

// 'to be documented'
#define mxTBD

// used to mark code that works and i don't know yet how it works;
// right from the book:
// Enjoy as you do so, as one of the few things more rewarding than pro-
// gramming and seeing a correctly animated, simulated, and rendered scene on
// a screen is the confidence of understanding how and why everything worked.
// When something in a 3D system goes wrong (and it always does), the best pro-
// grammers are never satisfied with "I fixed it, but I'm not sure how"; without
// understanding, there can be no confidence in the solution, and nothing new
// is learned. Such programmers are driven by the desire to understand what
// went wrong, how to fix it, and learning from the experience.
#define mxWHY( message_string )

//-------------------------------------------------------------

#endif // !__MX_HANDY_HELPERS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
