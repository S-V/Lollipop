/*
=============================================================================
	File:	Win32_MVCpp.h
	Desc:	Win32-specific code for Microsoft Visual C++ compilers.
	ToDo:	don't include "Windows.h" - it slows down compilation!
=============================================================================
*/
#pragma once
#ifndef __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__
#define __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__

// just in case...

#ifndef WIN32
	#define WIN32
#endif

#if !MX_PLATFORM_WIN32
#	define MX_PLATFORM	Platform_Win32
#endif

// #define MX_AUTOLINK to automatically include the needed libs (via #pragma lib)
#define MX_AUTOLINK		(1)

// exclude rarely used stuff from Windows headers
 // – used to reduce the size of the header files and speed up compilation.
// Excludes things like cryptography, DDE, RPC, the Windows Shell and Winsock.
// unset this option if it causes problems (e.g. GDI plus and DXUT won't compile)
#define MX_LEAN_WIN32	(1)

// turning on all compiler warnings can help find obscure bugs
#define MX_MUTE_CERTAIN_WARNINGS	(1)

//---------------------------------------------------------------

// Make sure we're compiling the correct (debug/release) version.
#if MX_DEBUG

#	ifndef DEBUG
#		define DEBUG
#	endif

#	ifndef _DEBUG
#		define _DEBUG
#	endif

#	undef NDEBUG

#else // if in release mode

#	ifdef DEBUG
#		undef DEBUG
#	endif

#	ifdef _DEBUG
#		undef _DEBUG
#	endif

#	ifndef NDEBUG
#		define NDEBUG
#	endif

#endif // !MX_DEBUG

//---------------------------------------------------------------

enum WindowsOSType
{
	OS_Unknown,
	OS_Windows9X, 
	OS_WindowsNT,
	OS_Windows2K,
	OS_WindowsXP,
	OS_WindowsVista,
	OS_Windows7
};

// Check the compiler and windows version.

#if !defined(MX_COMPILER_MSVC)
#	error Microsoft Visual C++ compiler required!
#endif

#if (_MSC_VER < 1500 )	// MVC++ 2008
#	error A version of Microsoft Visual C++ 9.0 ( 2008 ) or higher is required!
#endif

/*
	Microsoft Visual C++ 10.0 ( 2010 )	-	_MSC_VER == 1600
	Microsoft Visual C++ 9.0 ( 2008 )	-	_MSC_VER == 1500
	Microsoft Visual C++ 8.0 ( 2005 )	-	_MSC_VER == 1400
	Microsoft Visual C++ 7.1 ( 2003 )	-	_MSC_VER == 1310
	Microsoft Visual C++ 7.0 ( 2002 )	-	_MSC_VER == 1300
	Microsoft Visual C++ 6.0 ( 1998 )	-	_MSC_VER == 1200
*/

#	if ( _MSC_VER == 1200 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 6.0"
#	elif ( _MSC_VER == 1300 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 7.0"
#	elif ( _MSC_VER == 1400 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 8.0"
#	elif ( _MSC_VER == 1500 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 9.0"
#	elif ( _MSC_VER == 1600 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 10.0"
#	else
#		error Unknown compiler !
#	endif

#if defined _M_IX86
	// x86
#elif defined _M_X64
	// x64
	#error Invalid compile option: 64-bit mode
#elif defined _M_IA64
	// Itanium
	#error Invalid compile option: Itanium mode
#endif


/*
//
// Ensure proper handling of for-scope.
//
#if (_MSC_VER <= 1200)
// Limit the scope of the for statement
#	define for if(0); else for
#else
#	pragma conform( forScope, on )
#	pragma warning( disable: 4258 )	// 'variable' : definition from the for loop is ignored; the definition from the enclosing scope is used
#endif
*/

//
//	Prevent the compiler from complaining.
//
#if MX_MUTE_CERTAIN_WARNINGS
	#include "DisableWarnings.h"
#endif //MX_MUTE_CERTAIN_WARNINGS

#if ! MX_DEBUG
	// Disable MSVC STL debug + security features.
	#undef _SECURE_SCL
	#define _SECURE_SCL 0

	#undef _SECURE_SCL_THROWS
	#define _SECURE_SCL_THROWS 0

	#undef _HAS_ITERATOR_DEBUGGING
	#define _HAS_ITERATOR_DEBUGGING 0

	#undef _HAS_EXCEPTIONS
	#define _HAS_EXCEPTIONS 0

	// Inhibit certain compiler warnings.
	#define _CRT_SECURE_NO_WARNINGS
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE

#endif // !MX_DEBUG

//
//	Include windows-specific headers.
//
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif


#if MX_LEAN_WIN32
	#define NOMINMAX	// Suppress Window's global min/max macros.
	#define NOGDICAPMASKS
	#define OEMRESOURCE
	#define NOATOM

#if !MX_EDITOR
	#define NOCTLMGR
#endif // MX_EDITOR

	#define NOMEMMGR
	#define NOMETAFILE
	#define NOOPENFILE
	#define NOSERVICE
	#define NOSOUND
	#define NOCOMM
	#define NOKANJI
	#define NOHELP
	#define NOPROFILER
	#define NODEFERWINDOWPOS
	#define NOMCX
	#define NOIME
	#define NORPC
	#define NOPROXYSTUB
	#define NOIMAGE
	#define NOTAPE

	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#define WIN32_EXTRA_LEAN
	#define VC_EXTRALEAN
#endif

#ifndef STRICT
	#define STRICT					// Use strict declarations for Windows types
#endif

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

#if MX_AUTOLINK
	#pragma comment( lib, "winmm.lib" )
#endif

// CRT's memory leak detection.
#if MX_DEBUG
//	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

#include <sal.h>

//---------------------------------------------------------------
//	C Run-Time Library Header Files
//---------------------------------------------------------------

#include <memory>
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tchar.h>
#include <string.h>
#include <process.h>

#include <limits.h>
#include <float.h>
#include <math.h>

#include <strsafe.h>

//---------------------------------------------------------------
//		Fix windows macros.
//---------------------------------------------------------------

#include "Win32_Undefs.h"

//---------------------------------------------------------------
//		Defines.
//---------------------------------------------------------------

#ifdef MX_BUILD_ENGINE_DLL
	// We're building our own DLL.
	#define mxAPI	__declspec(dllexport)
#else
	#define mxAPI	__declspec(dllimport)
#endif // !MX_ENGINE_DLL

//---------------------------------------------------------------
//	Compiler-specific stuff.
//---------------------------------------------------------------

// Check current compiler settings.

//
// TypeDescriptor must be disabled in release builds !
//
#ifdef _CPPRTTI
#	define MX_CPP_RTTI_ENABLED	(1)
#	if MX_DEBUG
		//We need it for dynamic_cast ( checked_cast ).
#	else
		#error Project settings: TypeDescriptor must be disabled in release version!
#	endif // ! MX_DEBUG
#else
#	define MX_CPP_RTTI_ENABLED	(0)
#endif

#if ! MX_DEBUG
#	define dynamic_cast	"use of dynamic_cast is prohibited"
#endif //!MX_DEBUG

//
// Exception handling must be disabled in release builds !
//
#ifdef _CPPUNWIND
#	define MX_EXCEPTIONS_ENABLED	(1)
#	if MX_DEBUG
		// We use exceptions in debug mode.
		#define NO_THROW	throw()
#	else
		#error Project settings: Disable exception handling!
		#define NO_THROW
#	endif // ! MX_DEBUG
#else
#	define MX_EXCEPTIONS_ENABLED	(0)
#endif

//---------------------------------------------------------------

// don't inline
#define NO_INLINE		__declspec(noinline)

// Force a function call site not to be inlined.
#define DONT_INLINE(f)	(((int)(f)+1)?(f):(f))


// NOTE: inlined functions are harder to debug,
// you may want to turn off inlining in debug mode.

// The C++ standard defines that 'inline' doesn't enforce the function to necessarily be inline.
// It only 'hints' the compiler that it is a good candidate for inline function.
#define INLINE			inline

// always inline
#ifndef FORCEINLINE
	#define FORCEINLINE	__forceinline
#endif

#define ALWAYSINLINE	__forceinline




// '__noop' discards everything inside parentheses.
#define mxNOOP			__noop

#define mxDEPRECATED	__declspec( deprecated )

// MVC++ compiler allows non-strings in pragmas.
#define mxPRAGMA( x )	__pragma x

//
//	Function attributes extension
//	(to allow the compiler perform more error checking, inhibit unwanted warnings, perform more optimizations, etc)
//	(e.g. it will be __attribute__(x) under GCC).
//
#define mxATTRIBUTE( x )

// it will be __attribute__((format(printf,n,n+1))) on GCC
#define mxPRINTF( n )

// pure functions have no side effects; in GCC: __attribute__((pure))
#define mxPURE_FUNC

// enables printf parameter validation on GCC
// (equivalent to MX_ATTRIBUTE( (format(printf,1,2)) ) )
#define mxCHECK_PRINTF_ARGS

// enables vprintf parameter validation on GCC
#define mxCHECK_VPRINTF_ARGS

// enables printf parameter validation on GCC
#define mxCHECK_PRINTF_METHOD_ARGS

// enables vprintf parameter validation on GCC
#define mxCHECK_VPRINTF_METHOD_ARGS

/// Visual Studio’s non-standard C++ extensions:

/// allows member functions to be made abstract. uses nonstandard C++ extensions provided by MSVC
#define mxABSTRACT    abstract
 
/// marks member functions as being an override of a base class virtual function. uses nonstandard C++ extensions provided by MSVC
#//define MX_OVERRIDE    override
 
/// allows classes and member functions to be made sealed. uses nonstandard C++ extensions provided by MSVC
#define mxFINAL       sealed

//
//	Calling conventions.
//
#define VARARGS	__cdecl
#define CDECL	__cdecl
#define PASCAL	__stdcall
#define VPCALL	__fastcall

#define mxCALL	__cdecl


// tells the compiler not to emit function prologue/epilogue code

#define NAKED	__declspec( naked )

//---------------------------------------------------------------------------

// The first two pragmas tell the compiler to do inline recursion as deep as it needs to.
// You can change the depth to any number up to 255.
// The other pragma tells the compiler to inline whenever it feels like it.
// this may improve code involving template metaprogramming with recursion
//
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#if !MX_DEBUG
#pragma auto_inline( on )
#endif


// Don't initialize the pointer to the vtable in a constructor of an abstract class.
#define NO_VTABLE	__declspec( novtable )


mxSWIPED("Valve Source Engine L4D SDK");
// This can be used to declare an abstract (interface only) class.
// Classes marked abstract should not be instantiated.  If they are, and access violation will occur.
//
// Example of use:
//
// abstract_class CFoo
// {
//      ...
// }
//
// MSDN __declspec(novtable) documentation: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclang/html/_langref_novtable.asp
//
// Note: NJS: This is not enabled for regular PC, due to not knowing the implications of exporting a class with no no vtable.
//       It's probable that this shouldn't be an issue, but an experiment should be done to verify this.
//
#ifndef _X360
#define abstract_class class
#else
#define abstract_class class NO_VTABLE
#endif

//#define mxINTERFACE( ClassName )	struct NO_VTABLE ClassName
#define abstract_interface	struct NO_VTABLE


// Tell the compiler what size of a pointer to a member of a forward-declared class it should use.
// e.g.
//	class VIRTUAL_INHERITANCE Foo;
//	int Foo::*ptr;
// NOTE: You should try to use single inheritance only (for best performance).
//
#define SINGLE_INHERITANCE		__single_inheritance
#define MULTIPLE_INHERITANCE	__multiple_inheritance
#define VIRTUAL_INHERITANCE		__virtual_inheritance

// Generate safe and optimal code using best-case representation.
// The most general representation is single-inheritance, pointer to member function.
// See: http://msdn.microsoft.com/en-us/library/83cch5a6(v=vs.71).aspx
//	http://www.peousware.com/member-function-pointer-size-in-visual-c/
// Generally, in Visual Studio pointers to member functions will occupy either 4, 8 or 16 bytes
// (for single inheritance, multiple inheritance, and virtual inheritance, respectively).
// See: http://blogs.msdn.com/b/oldnewthing/archive/2004/02/09/70002.aspx
// NOTE: commented out because it messes up with fast delegates.
//#pragma pointers_to_members( best_case, single_inheritance )


// Optimization macros (they should be put right after #pragma).

#if MX_DEBUG
	#define ENABLE_OPTIMIZATION  optimize("",off)
#else
	#define ENABLE_OPTIMIZATION  optimize("",on)
#endif

	#define DISABLE_OPTIMIZATION optimize("",off)


#define RESTRICT	__restrict

// Assume that the given pointer is not aliased.
#define RESTRICT_PTR( pointer )		__restrict pointer

// tells the compiler that the return value (RV) of a function is an object that will not be aliased with any other pointers
#define RESTRICT_RV		__declspec(restrict)

// Tell the compiler that the specified condition is assumed to be true.
// 0 can be used to mark unreachable locations after that point in the program.
// (e.g. it's used in switch-statements whose default-case can never be reached, resulting in more optimal code )
// Essentially the 'Hint' is that the condition specified is assumed to be true at
// that point in the compilation.  If '0' is passed, then the compiler assumes that
// any subsequent code in the same 'basic block' is unreachable, and thus usually
// removed.
#define OPT_HINT( expr )		__assume(( expr ))

// Tell the compiler that the specified expression is unlikely to be true.
// (Hints to branch prediction system.)
//
#define IF_LIKELY( expr )		if(( expr ))
#define IF_UNLIKELY( expr )		if(( expr ))


/*
noalias means that a function call does not modify or reference visible global state 
and only modifies the memory pointed to directly by pointer parameters (first-level indirections). 
If a function is annotated as noalias, the optimizer can assume that, in addition to the parameters themselves,
only first-level indirections of pointer parameters are referenced or modified inside the function.
The visible global state is the set of all data that is not defined or referenced outside of the compilation scope,
and their address is not taken. The compilation scope is all source files (/LTCG (Link-time Code Generation) builds) or a single source file (non-/LTCG build).
*/
#define mxNO_ALIAS	__declspec( noalias )

// Function doesn't return.
#define mxNO_RETURN	__declspec( noreturn )

#define mxUNROLL

// The purpose of the following global constants is to prevent redundant 
// reloading of the constants when they are referenced by more than one
// separate inline math routine called within the same function.  Declaring
// a constant locally within a routine is sufficient to prevent redundant
// reloads of that constant when that single routine is called multiple
// times in a function, but if the constant is used (and declared) in a 
// separate math routine it would be reloaded.
//
#define mxGLOBAL_CONST		extern const __declspec(selectany)

// In MVC++ explicit storage types for enums can be specified, e.g.
// enum ETest : char
// {};
//
#define mxDECL_ENUM_STORAGE( enumName, storage )	enum enumName : storage


//---------------------------------------------------------------
//		Common.
//---------------------------------------------------------------

//#include <Base/System/common/common.h>

//---------------------------------------------------------------
//		Intrinsics.
//---------------------------------------------------------------

#include <intrin.h>

// _____________________________________________________________________________
// Memory

#pragma intrinsic( memset, memcpy, memcmp )
#pragma intrinsic( strcpy, strlen, strcat, strcmp )

// _____________________________________________________________________________
// Bit scanning

unsigned char _BitScanForward(unsigned long* Index, unsigned long Mask);
unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask);
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)

#ifdef _M_X64
	unsigned char _BitScanForward64(unsigned long* Index, unsigned __int64 Mask);
	unsigned char _BitScanReverse64(unsigned long* Index, unsigned __int64 Mask);
	#pragma intrinsic(_BitScanReverse64)
	#pragma intrinsic(_BitScanForward64)
#endif

// _____________________________________________________________________________
// Atomics

void _ReadBarrier();
void _ReadWriteBarrier();

long _InterlockedIncrement(long volatile *Addend);
long _InterlockedDecrement(long volatile *Addend);
long _InterlockedExchange(long volatile *Target, long Value);
long _InterlockedCompareExchange(long volatile *Destination, long ExChange, long Comperand);
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedCompareExchange)

#ifndef _WIN64
	inline void* _InterlockedCompareExchangePointer(void* volatile *Destination, void* Exchange, void* Comperand) { return (void*)_InterlockedCompareExchange((long volatile*)Destination, (long)(long*)Exchange, (long)(long*)Comperand); }
	inline void* _InterlockedExchangePointer(void* volatile *Target, void* Value) { return (void*)_InterlockedExchange((long volatile*)Target, (long)Value); }
#endif

#ifdef _M_X64
	void* _InterlockedCompareExchangePointer(void* volatile *Destination, void* Exchange, void* Comperand);
	void* _InterlockedExchangePointer(void* volatile *Target, void* Value);
	long long _InterlockedIncrement64(long long volatile *Addend);
	long long _InterlockedDecrement64(long long volatile *Addend);
	long long _InterlockedExchange64(long long volatile *Target, long long Value);
	long long _InterlockedCompareExchange64(long long volatile *Destination, long long ExChange, long long Comperand);
	#pragma intrinsic(_InterlockedExchangePointer)
	#pragma intrinsic(_InterlockedCompareExchangePointer)
	#pragma intrinsic(_InterlockedIncrement64)
	#pragma intrinsic(_InterlockedDecrement64)
	#pragma intrinsic(_InterlockedExchange64)
	#pragma intrinsic(_InterlockedCompareExchange64)
#endif

// _____________________________________________________________________________
// Math

#pragma intrinsic( _lrotl, _lrotr, _rotl, _rotr )
#pragma intrinsic( abs, fabs, labs )
#pragma intrinsic( sin, cos, tan, atan, atan2 )
#pragma intrinsic( exp, log, log10, sqrt )

//---------------------------------------------------------------
//	Utilities for Debugging.
//---------------------------------------------------------------

#include "Win32_Debug.h"

//---------------------------------------------------------------
//		Types.
//---------------------------------------------------------------

#include "Win32_Types.h"

//---------------------------------------------------------------
//	Asm & Math routines.
//---------------------------------------------------------------

#include "Win32_Asm.h"
#include "Win32_Math.h"

//---------------------------------------------------------------
//	Data alignment and various memory macros.
//---------------------------------------------------------------

#include "Win32_Memory.h"

//---------------------------------------------------------------
//	Threading.
//---------------------------------------------------------------

#include "Win32_Threading.h"

//---------------------------------------------------------------
//	Input/Output.
//---------------------------------------------------------------

#include "Win32_FileIO.h"

//---------------------------------------------------------------
//	Miscellaneous.
//---------------------------------------------------------------

// __PRETTY_FUNCTION__ is not part of the standard (and it's not available in MVC++)
#define __PRETTY_FUNCTION__		__FUNCTION__

//---------------------------------------------------------------
//	Utilities.
//---------------------------------------------------------------

#include "Win32_Timer.h"
#include "Profiler.h"
#include "Win32_Utils.h"

//---------------------------------------------------------------
//	External libraries.
//---------------------------------------------------------------

//#include "Win32_Ext.h"

//---------------------------------------------------------------
//	Platform-independent application entry point.
//	Put the mxAPPLICATION_ENTRY_POINT macro
//	into one of your source files.
//---------------------------------------------------------------

#ifndef _CONSOLE

	#define mxAPPLICATION_ENTRY_POINT	\
		int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )	\
		{																									\
			UNREFERENCED_PARAMETER( hPrevInstance );														\
			UNREFERENCED_PARAMETER( lpCmdLine );															\
			mxENSURE(mxInitializeBase());																		\
			int ret = mxAppMain();																			\
			mxENSURE(mxShutdownBase());																		\
			return ret;																						\
		}

#else

	#define mxAPPLICATION_ENTRY_POINT	\
		int _tmain( int argc, _TCHAR* argv[] )																\
		{																									\
			UNREFERENCED_PARAMETER( argc );																	\
			UNREFERENCED_PARAMETER( argv );																	\
			mxENSURE(mxInitializeBase());																		\
			int ret = mxAppMain();																			\
			mxENSURE(mxShutdownBase());																		\
			return ret;																						\
		}

#endif //_CONSOLE

//---------------------------------------------------------------

#endif // ! __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
