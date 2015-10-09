/*
=============================================================================
	File:	Debug.h
	Desc:	Code for debug utils, assertions and other very useful things.
	Note:	Some debug utils depend heavily on the platform.
=============================================================================
*/

#ifndef __MX_DEBUG_UTILS_H__
#define __MX_DEBUG_UTILS_H__

/*
=============================================================================
	
		Run-time checks and assertions.

=============================================================================
*/

mxNAMESPACE_BEGIN

//
//	DBGOUT - The same as Print(), but works in debug mode only.
//
#if MX_DEBUG
	extern void VARARGS DBGOUT( const char* format, ... );
	extern void DBGNEWLINE();	// prints a long line "========================"
#else
	inline void VARARGS DBGOUT( const char* format, ... ) {}
	inline void DBGNEWLINE () {}
#endif


#if MX_DEVELOPER
	extern void VARARGS DEVOUT( const char* format, ... );
	extern void DEVNEWLINE();	// prints a long line "========================"
#else
	inline void VARARGS DEVOUT( const char* format, ... ) {}
	inline void DEVNEWLINE () {}
#endif

//
//	EAssertBehaviour - used to specify actions taken on assert failures.
//
enum EAssertBehaviour
{
	AB_Continue,	// continue execution
	AB_Ignore,		// continue and don't report this assert from now on
	AB_Break,		// trigger a breakpoint
};

// should return 0 to have the program simply return from the assertion failure
// NOTE: ignore is an optional parameter, can be null
typedef int (*UserAssertCallback)( const char* expr, const char* file, const char* func, unsigned int line, bool* ignore );

//! Set a user-defined assertion-failure callback.
/** This function registers a user-defined routine to call in the event of an
* assertion failure.
* \param cb A callback routine to call that is of type UserAssertCallback.
* \return Returns the previous callback, if one was registered.
* \note The user-defined callback should have a prototype:
*    int callback( const char * message );
*
*    and should return 0 to have the program simply return from the assertion failure
*    and not process it any further, nonzero to continue with assertion processing 
*    for the host OS.
*/
extern UserAssertCallback SetUserAssertCallback( UserAssertCallback cb );



extern void OnAssertionFailed(
	const char* expression,
	const char* filename, const char* function, int line, bool* ignore = nil );

extern void OnAssertionFailedX(
	const char* expression, const char* message,
	const char* filename, const char* function, int line, bool* ignore = nil );

mxNAMESPACE_END

/*
=============================================================================
		Assertions.


  Assert that the statement x is true, and continue processing.

  If the expression is true, continue processing.

  If the expression is false, trigger a debug breakpoint.

  These asserts are only present in DEBUG builds.


	Examples of usage:
		Assert( a > b );
		AssertX( x == y, "X must be equal to Y!" );

	NOTE: Asserts should not have any side effects.

	In debug mode the compiler will generate code for asserts.
	When using MVC++, asserts trigger a break point interrupt inside a macro.
	This way the program brakes on the line of the assert.
=============================================================================
*/

#if 1//MX_DEVELOPER

		#if MX_DEBUG

			#define Assert( expr )\
				mxMACRO_BEGIN\
					static bool bIgnoreAlways = 0;\
					if(!bIgnoreAlways)\
						if(!(expr))\
							{mxDEBUG_BREAK;OnAssertionFailed( #expr, __FILE__, __FUNCTION__, __LINE__, &bIgnoreAlways );}\
				mxMACRO_END


			#define AssertX( expr, message )\
				mxMACRO_BEGIN\
					static bool bIgnoreAlways = 0;\
					if(!bIgnoreAlways)\
						if(!(expr))\
							{mxDEBUG_BREAK;OnAssertionFailedX( #expr, message, __FILE__, __FUNCTION__, __LINE__, &bIgnoreAlways );}\
				mxMACRO_END

		#else

			#define Assert( expr )				mxNOOP;(void)(expr)
			#define AssertX( expr, message )	mxNOOP;(void)(expr)

		#endif // ! MX_DEBUG

#else

		#if MX_DEBUG
			#define Assert( expr )				mxBREAK_IF( !(expr) )
			#define AssertX( expr, message )	mxBREAK_IF( !(expr) )
		#else
			#define Assert( expr )				mxNOOP;(void)(expr)
			#define AssertX( expr, message )	mxNOOP;(void)(expr)
		#endif // ! MX_DEBUG

#endif // MX_DEVELOPER


#if MX_DEBUG
	#define AssertPtr( ptr )			Assert( mxIsValidHeapPointer((ptr)) )
	#define AssertPtr8( ptr )			Assert( mxIsValidHeapPointer((ptr)) && IS_8_BYTE_ALIGNED((ptr)) )
	#define AssertPtr16( ptr )			Assert( mxIsValidHeapPointer((ptr)) && IS_16_BYTE_ALIGNED((ptr)) )
	#define Assert_GZ(x)				Assert((x)>0)
	#define Assert_GE(x)				Assert((x)>=0)
	#define Assert_LZ(x)				Assert((x)<0)
	#define Assert_LE(x)				Assert((x)<0)
	#define Assert_ZERO(x)				Assert((x)==0)
	#define Assert_NZ(x)				Assert((x)!=0)
#else
	#define AssertPtr( ptr )			(void)(ptr)
	#define AssertPtr8( ptr )			(void)(ptr)
	#define AssertPtr16( ptr )			(void)(ptr)
	#define Assert_GZ(x)				(void)(x)
	#define Assert_GE(x)				(void)(x)
	#define Assert_LZ(x)				(void)(x)
	#define Assert_LE(x)				(void)(x)
	#define Assert_ZERO(x)				(void)(x)
	#define Assert_NZ(x)				(void)(x)
#endif // ! MX_DEBUG

/*
=============================================================================
	CHK(x) is always evaluated, returns the (boolean) value of the passed (logical) expression.

	e.g.:
		bool	b;

		b = CHK(5>3);
		b = CHK(5>1);
		b = CHK(5>1) && CHK(4<1);	// <- will break on this line (4<1)
		b = CHK(5>9);				// <- and here

	Real-life example:
		bool Mesh::isOk() const
		{
			return CHK(mesh != null)
				&& CHK(numBatches > 0)
				&& CHK(batches != null)
				;
		}
=============================================================================
*/
#if MX_DEBUG
	#define CHK( expr )		( (expr) ? true : (mxALWAYS_BREAK, false) )
#else
	#define CHK( expr )		(expr)
#endif // ! MX_DEBUG

//
// FullAsserts are never removed in release builds, they slow down a lot.
//
#define AlwaysAssert( expr )		\
	mxMACRO_BEGIN					\
	static bool bIgnoreAlways = 0;	\
	if(!bIgnoreAlways)				\
		if(!(expr))					\
			OnAssertionFailed( #expr, __FILE__, __FUNCTION__, __LINE__, &bIgnoreAlways );	\
	mxMACRO_END


#define AlwaysAssertX( expr, message )	\
	mxMACRO_BEGIN						\
	static bool bIgnoreAlways = 0;		\
	if(!bIgnoreAlways)					\
		if(!(expr))						\
			OnAssertionFailedX( #expr, message, __FILE__, __FUNCTION__, __LINE__, &bIgnoreAlways );	\
	mxMACRO_END



/*
another version:
#define AlwaysAssert( expr )	\
	do { if(!(expr)) OnAssertionFailed( #expr, __FILE__, __FUNCTION__, __LINE__ ); } while(0)

#define AlwaysAssertX( expr, message )	\
	do { if(!(expr)) OnAssertionFailed( message, #expr, __FILE__, __FUNCTION__, __LINE__ ); } while(0)
*/

/* old code:
#define AlwaysAssert( expr )	\
	(void)( (expr) ? 1 : (OnAssertionFailed( #expr, __FILE__, __FUNCTION__, __LINE__ ) ))

#define AlwaysAssertX( expr, message )	\
	(void)( (expr) ? 1 : (OnAssertionFailed( message, #expr, __FILE__, __FUNCTION__, __LINE__ ) ))
*/

#define mxASSUME( x )		{ Assert((x)); OPT_HINT(x); }


/*
The verify(x) macro just returns true or false in release mode, but breaks
in debug mode.  That way the code can take a non-fatal path in release mode
if something that's not supposed to happen happens.

if ( !verify(game) ) {
	// This should never happen!
	return;
}

(this is taken from Quake 4 SDK)
*/

#if MX_DEBUG
	#define mxVERIFY( x )		( (x) ? true : (OnAssertionFailed( #x, __FILE__, __FUNCTION__, __LINE__ ), false) )
#else
	#define mxVERIFY( x )		( (x) ? true : false )
#endif


//
// Causes a breakpoint exception in debug builds, causes fatal error in release builds.
//
#if MX_DEBUG
	#define mxENSURE( expr )	do{ mxBREAK_IF( !(expr) ) }while(0)
#else
	#define mxENSURE( expr )	if( !(expr) ) {mxErr("Fatal error!");}
#endif // ! MX_DEBUG

/*
=============================================================================
		Exception handling.
=============================================================================
*/
#if MX_EXCEPTIONS_ENABLED
	#define mxTHROW(x)		throw(x)
	#define mxCATCH(x)		catch(x)
	#define mxCATCH_ALL		catch(...)
#else
	#define mxTHROW(x)		mxFatal("Unhandled exception occured. The program will now exit.\n")
	#define mxCATCH(x)		if(0)
	#define mxCATCH_ALL	if(0)
#endif //MX_EXCEPTIONS_ENABLED

/*
=============================================================================
		Debug macros.
=============================================================================
*/

// Everything inside the parentheses will be discarded in release builds.
#if MX_DEBUG
	#define DEBUG_CODE( code )		code
#else
	#define DEBUG_CODE( code )		mxNOOP
#endif // ! MX_DEBUG

/*
=============================================================================
	IF_DEBUG is a C/C++ comment-out macro.

	The preprocessor must support the '//' character sequence
	as a single line comment.

	Example:
	IF_DEBUG outputDevice << "Generating a random world " << world.Name();
=============================================================================
*/
#if MX_DEBUG
#	define IF_DEBUG		// Debug version - enable code on line.
#else
#	define _PT_SLASH( c )	/##c
#	define IF_DEBUG			_PT_SLASH(/)	// Release version - comment out line.
#endif // ! MX_DEBUG

/*
=============================================================================

	checked_cast< TypeTo, TypeFrom >

	Works as a static_cast, except that it would use dynamic_cast 
	to catch programming errors in debug mode.
	Uses fast static_cast in Release build, but checks cast with an Assert() in Debug.

	Example:

	(Both 'rxPointLight' and 'rxSpotLight' inherit 'rxLocalLight'.)

	rxPointLight * newLight = new_one(rxPointLight());

	rxSpotLight * pSpotLight = checked_cast< rxSpotLight*>( newLight );	// <= will break here

=============================================================================
*/

#if MX_DEBUG && MX_CPP_RTTI_ENABLED

	template < class TypeTo, class TypeFrom >
	FORCEINLINE TypeTo checked_cast( TypeFrom ptr )
	{
		if ( ptr )
		{
			TypeTo dtto = dynamic_cast< TypeTo >( ptr );
			Assert( dtto != NULL );
			TypeTo stto = static_cast< TypeTo >( ptr );
			Assert( dtto == stto );
			return stto;
		}
		return NULL;
	}

#else // if !defined( MX_DEBUG )

	template < class TypeTo, class TypeFrom >
	FORCEINLINE TypeTo checked_cast( TypeFrom ptr )
	{
		return static_cast< TypeTo >( ptr );
	}

#endif // ! MX_DEBUG

/*

Other versions:

template <class TypeTo, class TypeFrom>
TypeTo checked_cast(TypeFrom p)
{
	Assert(dynamic_cast<TypeTo>(p));
	return static_cast<TypeTo>(p);
}

// perform a static_cast asserted by a dynamic_cast
template <class Type, class SourceType>
Type static_cast_checked(SourceType item)
{
	Assert(!item || dynamic_cast<Type>(item));
	return static_cast<Type>(item);
}

#if MX_DEBUG
#	define checked_cast    dynamic_cast
#else // if !defined( MX_DEBUG )
#	define checked_cast    static_cast
#endif // ! MX_DEBUG


=== For using on references:

/// perform a static_cast asserted by a dynamic_cast
template <class Type, class SourceType>
Type* static_cast_checked(SourceType *item)
{
	Assert(!item || dynamic_cast<Type*>(item));
	return static_cast<Type*>(item);
}

/// overload for reference
template <class Type, class SourceType>
Type &static_cast_checked(SourceType &item)
{
	Assert(dynamic_cast<Type *>(&item));
	return static_cast<Type&>(item);
}

=== Use it like this:
	Derived d;
	Base* pbase = static_cast_checked<Base>(&d);
	Base& rbase = static_cast_checked<Base>(d);
*/

//----------------------------------------------------------------------------
//	Use this to mark unreachable locations 
//	( like an unreachable default case of a switch )
//	Unreachables are removed in release builds.
//----------------------------------------------------------------------------

#if MX_DEBUG

/*
Old version (leaves unwanted strings in .exe):
#define  Unreachable	\
	FatalError( "Unreachable code in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ );

#define  UnreachableX( message )	\
	FatalError( "'%s': unreachable code in %s, at '%s', line %d", message, __FUNCTION__, __FILE__, __LINE__ );
*/

#	define  Unreachable				mxALWAYS_BREAK
#	define  UnreachableX( message )	mxALWAYS_BREAK

#else

#	define  Unreachable				OPT_HINT( 0 )
#	define  UnreachableX( message )	OPT_HINT( 0 )

#endif // ! MX_DEBUG



#if MX_DEBUG

#	define  mxDBG_UNREACHABLE				mxDEBUG_BREAK;DBGOUT("Unreachable location in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__)
#	define  mxDBG_UNREACHABLE_X(...)		mxDEBUG_BREAK;DBGOUT(__VA_ARGS__)

#else

#	define  mxDBG_UNREACHABLE
#	define  mxDBG_UNREACHABLE_X(...)

#endif // ! MX_DEBUG


//----------------------------------------------------------------------------
//	This can be used to mark code paths that should not be executed
//	(e.g. overriden member function Write() of some class named ReadOnlyFile).
//----------------------------------------------------------------------------

#if MX_DEBUG
#	define  InvalidCall		mxALWAYS_BREAK
#else
//#	define  InvalidCall		FatalError( "Invalid call in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ )
#	define  InvalidCall		mxALWAYS_BREAK
#endif // ! MX_DEBUG


//----------------------------------------------------------------------------
//	Use this to mark unimplemented features
//	which are not supported yet and will cause a crash.
//  They are not removed in release builds.
//----------------------------------------------------------------------------
#if MX_DEBUG

	#define  Unimplemented				mxALWAYS_BREAK
	#define  UnimplementedX( message )	mxALWAYS_BREAK

#else

/*
	Old version (leaves unwanted strings in .exe):
	#define  Unimplemented				FatalError( "Unimplemented feature in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ );
	#define  UnimplementedX( message )	FatalError( "Unimplemented feature '%s' in %s, at '%s', line %d", message, __FUNCTION__, __FILE__, __LINE__ );
*/

	#define  Unimplemented				mxALWAYS_BREAK
	#define  UnimplementedX( message )	mxALWAYS_BREAK

#endif // ! MX_DEBUG



// break in debug builds, but don't crash in release builds
#if MX_DEBUG
	#if MX_DEVELOPER
		//#define mxDBG_UNIMPLEMENTED		mxWarnf("Unimplemented feature in function '%s'\n",__FUNCTION__)
		#define mxDBG_UNIMPLEMENTED		{/*MX_DEBUG_BREAK;*/mxWarnf("Unimplemented feature in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ );}
	#else
		#define mxDBG_UNIMPLEMENTED		Unimplemented
	#endif // MX_DEVELOPER
#else
	#define mxDBG_UNIMPLEMENTED
#endif // MX_DEBUG



#define  UNDONE		mxALWAYS_BREAK

/*
=============================================================================
	
		Compile-time checks and assertions.

		NOTE: expressions to be checked
		must be compile-time constants!
=============================================================================
*/

/*
============================================================================
	StaticAssert( expression )
	STATIC_ASSERT2( expression, message )

	Fires at compile-time !

	Usage:
		StaticAssert( sizeof(*void) == 4, size_of_void_must_be_4_bytes );
============================================================================
*/

#if CPP_0X

	#define mxSTATIC_ASSERT( expression )			static_assert((expression))

#else

	#define mxSTATIC_ASSERT( expression )		\
	struct PP_JOIN_TOKEN(CHECK_,__LINE__) {	\
		int m_error[ (expression) ? 1 : -1 ];	\
	};

#endif // CPP_0X


#define mxSTATIC_ASSERT2( expression, message )	\
struct ERROR_##message {						\
	ERROR_##message() {							\
		int _##message[ (expression) ? 1 : -1 ];\
	}											\
};

// This macro has no runtime side affects as it just defines an enum
// whose name depends on the current line,
// and whose value will give a divide by zero error at compile time if the assertion is false.
//
#define mxSTATIC_ASSERT_X( expression )\
	enum { PP_JOIN_TOKEN( assert_line_, __LINE__ ) = 1/(!!(e)) }\

/*
============================================================================
	STATIC_CHECK( expression )
	STATIC_CHECK2( expression, message )

	Fires at link-time !
	( unresolved external symbol void CompileTimeChecker<0>(void); )

	Usage:
		STATIC_CHECK2( sizeof(*void) == 4, size_of_void_must_be_4_bytes );

	Note: should only be used in source files, not header files.
============================================================================
*/

namespace debug
{
	// We create a specialization for true, but not for false.
	template< bool > struct CompileTimeChecker;
	template<> struct CompileTimeChecker< true > {
		CompileTimeChecker( ... );
	};
}//end of namespace debug



#define STATIC_CHECK( expr )							\
	debug::CompileTimeChecker < (expr) != false > ();


#define STATIC_CHECK2( expr, msg )						\
class ERROR_##msg {										\
	debug::CompileTimeChecker< (expr) != false > error;	\
}


/*
=================================================================
	DELAYED_ASSERT( expression );

	Valid only in function scope !

	Fires at link-time !
	( unresolved external symbol void StaticAssert<0>(void); )

	Usage example:
			void  Foo()
			{
				DELAYED_ASSERT( a == b );

				// ....
			}
=================================================================
* /
namespace debug {

	template< bool > void	StaticAssert();
	template<> inline void	StaticAssert< true >() {}

}//end of namespace debug

#define DELAYED_ASSERT( expression )					\
		::debug::StaticAssert< expression != false >();	\
*/

//--------------------------------------------------------------------------------------------------------------
//	These can be very useful compile-time assersions :
/* COMPILE_TIME_ASSERT is for enforcing boolean/integral conditions at compile time.
   Since it is purely a compile-time mechanism that generates no code, the check
   is left in even if _DEBUG is not defined. */
//--------------------------------------------------------------------------------------------------------------

// This one is valid in function scope only!
#define mxCOMPILE_TIME_ASSERT( x )				{ typedef int ZZ_compile_time_assert_failed[ (x) ? 1 : -1 ]; }

// This one is valid in function scope only!
#define mxCOMPILE_TIME_ASSERT2( x )				switch(0) { case 0: case !!(x) : ; }

// This one is valid in file and function scopes.
#define mxFILE_SCOPED_COMPILE_TIME_ASSERT( x )	extern int ZZ_compile_time_assert_failed[ (x) ? 1 : -1 ]

#define ASSERT_SIZEOF( type, size )				mxFILE_SCOPED_COMPILE_TIME_ASSERT( sizeof( type ) == size )
#define ASSERT_OFFSETOF( type, field, offset )	mxFILE_SCOPED_COMPILE_TIME_ASSERT( offsetof( type, field ) == offset )
#define ASSERT_SIZEOF_8_BYTE_MULTIPLE( type )	mxFILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 8 ) == 0 )
#define ASSERT_SIZEOF_16_BYTE_MULTIPLE( type )	mxFILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 15 ) == 0 )
#define ASSERT_SIZEOF_32_BYTE_MULTIPLE( type )	mxFILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 31 ) == 0 )

/*
============================================================================
	CHECK_STORAGE( var, required_range )

	// bad explanation, look at the example below
	var - variable (e.g. 'c', where 'c' is of type 'char')
	required_range - [0..max_alowed_value] (e.g. 256)

	Fires at compile-time !

	Usage:
		unsigned char c;
		enum { Required_Range = 257 };	// 'c' must hold up to 257 different values without overflow
		CHECK_STORAGE( c, Required_Range ); // <- error, 'c' can only hold 256 different values

	Multiple checks should be enclosed into brackets:
		{ CHECK_STORAGE( var1, range1 ); }
		{ CHECK_STORAGE( var2, range2 ); }
		etc.
============================================================================
*/

namespace debug
{
	template< unsigned BytesInVariable, unsigned long RequiredRange >
	struct TStorageChecker
	{
		enum {
			bEnoughStorage = mxNAMESPACE_PREFIX TPow2< BYTES_TO_BITS(BytesInVariable) >::value >= RequiredRange
		};
		//static const bool bEnoughStorage = ( TPow2< BYTES_TO_BITS(BytesInVariable) >::value >= RequiredRange );
	};

	template< unsigned BitsInVariable, unsigned long RequiredRange >
	struct TStorageCheckerBits
	{
		enum {
			bEnoughStorage = mxNAMESPACE_PREFIX TPow2< BitsInVariable >::value >= RequiredRange
		};
	};

}//end of namespace debug


#define CHECK_STORAGE( size_of_var, required_range )\
	class PP_JOIN_TOKEN(StorageChecker_,__LINE__) {\
		int a[ debug::TStorageChecker< (size_of_var), (required_range) >::bEnoughStorage ? 1 : -1 ];\
	}


#define CHECK_STORAGE_BITS( size_of_var_in_bits, max_value )\
	class PP_JOIN_TOKEN(StorageChecker_,__LINE__) {\
		int a[ debug::TStorageCheckerBits< (size_of_var_in_bits), (max_value) >::bEnoughStorage ? 1 : -1 ];\
	}


//--------------------------------------------------------------------------------------------------------------

//
//	COMPILE_TIME_MSG( msg )
//
//	Writes a message into the output window.
//
//	Usage:
//	#pragma COMPILE_TIME_MSG( Fix this before final release! )
//
#define mxCOMPILE_TIME_MSG( msg )		message( __FILE__ "(" TO_STR(__LINE__) "): " #msg )

//TODO: compile-time warnings
#define mxCOMPILE_TIME_WARNING( x, msg )	mxCOMPILE_TIME_ASSERT((x))

//--------------------------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------
//  RecursionGuard  -  Basic protection against function recursion
// ----------------------------------------------------------------------------------------
// Thread safety note: If used in a threaded environment, you should use a handle to a __threadlocal
// storage variable (protects aaginst race conditions and, in *most* cases, is more desirable
// behavior as well.
//
// Rationale: wxWidgets has its own wxRecursionGuard, but it has a sloppy implementation with
// entirely unnecessary assertion checks.
//
#if MX_DEBUG

mxNAMESPACE_BEGIN

	class mxRecursionGuard
	{
		int & mCounter;

	public:
		inline mxRecursionGuard(
			const char* filename,
			const char* function, int line,
			int& counter
			)
			: mCounter( counter )
		{
			++mCounter;
			if( mCounter > 1 )
			{
				// we cannot use mxErrf(), etc. here because they use recursion guard
#ifdef MX_COMPILER_MSVC
				char	buffer[ 256 ];
				MX_SPRINTF_ANSI( buffer, "Recursive function call in %s(%d): %s", filename, line, function );
				::OutputDebugStringA( buffer );
				::MessageBoxA( NULL, buffer, "Error", MB_OK );
				DebugBreak();
#else
#	error Unsupported compiler!
#endif
			}
		}
		inline ~mxRecursionGuard() throw()
		{
			--mCounter;
		}
	};

mxNAMESPACE_END

	#define RECURSION_GUARD	\
		static int __recursion_guard_call_counter__ = 0;	\
		mxRecursionGuard	_scoped_recursion_guard_( __FILE__, __FUNCTION__, __LINE__, __recursion_guard_call_counter__ );

#else // !MX_DEBUG

	#define RECURSION_GUARD

#endif // !MX_DEBUG




//
//	DbgNamedObject - named object.
//
//	NOTE: its memory layout is different in debug and release versions!
//
struct DbgNamedObject
{
	#if MX_DEBUG

			// NOTE: the name will be truncated to the size of the embedded buffer
			inline void DbgSetName( const char* str )
			{
				CHK_VRET_IF_NIL(str);
				const UINT len = smallest( mxStrLenAnsi(str), NUMBER_OF(mDebugName)-1 );
				mxStrCpyNAnsi( mDebugName, str, len );
				mDebugName[len] = '\0';
			}
			inline const char* DbgGetName() const
			{
				return mDebugName;
			}

		protected:
			inline DbgNamedObject()
			{
				mDebugName[0] = '?';
				mDebugName[1] = '\0';
			}

		private:
			enum { STR_BUF_SIZE = 64 };
			char	mDebugName[ STR_BUF_SIZE ];

	#else

			NO_INLINE void DbgSetName( const char* staticString )
			{
				(void)staticString;
			}
			NO_INLINE const char* DbgGetName() const
			{
				return "?";
			}

	#endif // MX_DEBUG

};//struct DbgNamedObject



#if MX_DEBUG
	#define mxDBG_TRACE_CALL		DBGOUT("%s (%s:%d)\n",__FUNCTION__,__FILE__,__LINE__)
#else
	#define mxDBG_TRACE_CALL
#endif // MX_DEBUG


#if MX_DEBUG

	#define DBG_DO_INTERVAL(what,milliseconds)\
	{\
		static UINT prevTime = mxGetMilliseconds();\
		const UINT currTime = mxGetMilliseconds();\
		if( currTime - prevTime > milliseconds )\
		{\
			what;\
			prevTime = currTime;\
		}\
	}

#else

	#define DBG_DO_INTERVAL(what,milliseconds)

#endif // MX_DEBUG



#endif // !__MX_DEBUG_UTILS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
