/*
=============================================================================
	File:	DisableWarnings.h
	Desc:	Used to prevent some MVC++ compiler warnings.
=============================================================================
*/

#ifndef _MSC_VER
#pragma error "This header is for Microsoft VC only."
#endif /* _MSC_VER */

mxSWIPED("CryEngine3");
// Disable (and enable) specific compiler warnings.
// MSVC compiler is very confusing in that some 4xxx warnings are shown even with warning level 3,
// and some 4xxx warnings are NOT shown even with warning level 4.


#pragma warning ( disable: 4345 )	// behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
#pragma warning ( disable: 4351 )	// new behavior: elements of array 'array' will be default initialized

#pragma warning ( disable: 4511 )	// copy constructor could not be generated
#pragma warning ( disable: 4512 )	// assignment operator could not be generated
#pragma warning ( disable: 4505 )	// unreferenced local function has been removed

#pragma warning ( disable: 4127 )	// conditional expression is constant
#pragma warning ( disable: 4100 )	// unreferenced formal parameter

#pragma warning ( disable: 4245 )	// 'argument': conversion from 'type1' to 'type2', signed/unsigned mismatch
#pragma warning ( disable: 4244 )	// 'argument': conversion from 'double' to 'float', possible loss of data
#pragma warning ( disable: 4267 )	// 'argument': conversion from 'size_t' to 'int', possible loss of data

#pragma warning ( disable: 4311 )	// 'variable' : pointer truncation from 'type1' to 'type2'
#pragma warning ( disable: 4312 )	// 'type cast' : conversion from 'type1' to 'type2' of greater size

#pragma warning ( disable: 4018 )	// signed/unsigned mismatch

#pragma warning ( disable: 4324 )	// structure was padded due to __declspec(align())

#pragma warning ( disable: 4172 )	// returning address of local variable or temporary

#pragma warning ( disable: 4512 )	// assignment operator could not be generated

#pragma warning ( disable: 4995 )	// name was marked as #pragma deprecated
#pragma warning ( disable: 4996 )	// Deprecation warning: this function or variable may be unsafe.

#pragma warning ( disable: 4099 )	// type name first seen using 'class' now seen using 'struct'

#pragma warning ( disable: 4786 )	// the fully-qualified name of the class you are using is too long to fit into the debug information and will be truncated to 255 characters

#pragma warning ( disable: 4355 )	// 'this' : used in base member initializer list



#pragma warning ( disable: 4163 )	// function not available as an intrinsic function

#pragma warning ( disable: 4510 )	// default constructor could not be generated
#pragma warning ( disable: 4610 )	// X can never be instantiated - user defined constructor required

#pragma warning ( disable: 4098 )	// defaultlib conflicts with use of other libs; use /NODEFAULTLIB:library

#if !MX_DEBUG
	#pragma warning ( disable: 4702 )	// unreachable code
	#pragma warning ( disable: 4711 )	// selected for automatic inline expansion
	#pragma warning ( disable: 4505 )	// unreferenced local function has been removed
#endif //!MX_DEBUG

//--------------------------------------------------------------
//	nonstandard extensions that i use
//--------------------------------------------------------------

#pragma warning ( disable: 4482 )	// Nonstandard extension: enum used in qualified name.
#pragma warning ( disable: 4201 )	// Nonstandard extension used : nameless struct/union.
#pragma warning ( disable: 4239 )	// Nonstandard extension used : conversion from X to Y.
#pragma warning ( disable: 4480 )	// nonstandard extension used: specifying underlying type for enum
#pragma warning ( disable: 4238 )	// nonstandard extension used : class rvalue used as lvalue
#pragma warning ( disable: 4200 )	// nonstandard extension used : zero-sized array in struct/union
#pragma warning ( disable: 4481 )	// nonstandard extension used: override specifier 'override'


//--------------------------------------------------------------
//	Warnings disabled in builds with C++ code analysis enabled.
//--------------------------------------------------------------

#pragma warning ( disable: 4158 )	// assuming #pragma pointers_to_members(full_generality, single_inheritance)

#if MX_CHECKED

#pragma warning ( disable: 6387 )	// 'argument X' might be 'Y': this does not adhere to the specification for the function 'F'

#pragma warning ( disable: 6011 )	// Dereferencing NULL pointer

#pragma warning ( disable: 6211 )	// Leaking memory due to an exception. Consider using a local catch block to clean up memory

#endif

//--------------------------------------------------------------
//	Make MSVC more pedantic, this is a recommended pragma list
//	from _Win32_Programming_ by Rector and Newcomer.
//--------------------------------------------------------------

#pragma warning(error:4002) /* too many actual parameters for macro */
#pragma warning(error:4003) /* not enough actual parameters for macro */
#pragma warning(1:4010)     /* single-line comment contains line-continuation character */
#pragma warning(error:4013) /* 'function' undefined; assuming extern returning int */
#pragma warning(1:4016)     /* no function return type; using int as default */
#pragma warning(error:4020) /* too many actual parameters */
#pragma warning(error:4021) /* too few actual parameters */
#pragma warning(error:4027) /* function declared without formal parameter list */
#pragma warning(error:4029) /* declared formal parameter list different from definition */
#pragma warning(error:4033) /* 'function' must return a value */
#pragma warning(error:4035) /* 'function' : no return value */
#pragma warning(error:4045) /* array bounds overflow */
#pragma warning(error:4047) /* different levels of indirection */
#pragma warning(error:4049) /* terminating line number emission */
#pragma warning(error:4053) /* An expression of type void was used as an operand */
#pragma warning(error:4071) /* no function prototype given */
#pragma warning(error:4150)//deletion of pointer to incomplete type 'x'; no destructor called


#pragma warning(disable:4244)   /* No possible loss of data warnings: Win32 */
#pragma warning(disable:4267)   /* No possible loss of data warnings: x64 */
#pragma warning(disable:4305) /* No truncation from int to char warnings */
#pragma warning(disable:4146) /* Unary minus operator applied to unsigned type, result still unsigned */




#ifndef __cplusplus
#pragma warning(disable:4018) /* No signed unsigned comparison warnings for C code, to match gcc's behavior */
#endif

#pragma warning(disable:4800) /* Forcing value to bool 'true' or 'false' warnings */


#pragma warning(disable:4251)	// 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable:4275)	// non – DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'





#pragma warning(disable: 4530)  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable: 4996)	// 'stricmp' was declared deprecated
#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#pragma warning(disable: 6255)  // _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead. (Note: _malloca requires _freea.)

// Turn on the following very useful warnings.
//#pragma warning(3: 4264)				// no override available for virtual member function from base 'class'; function is hidden
//#pragma warning(3: 4266)				// no override available for virtual member function from base 'type'; function is hidden





// NOTE: this does not work.
// Mainly because this suppresses warning 4221 for the compiler
// but the warning occurs during the linker phase so this has no effect on eliminating the warning.
// See: http://stackoverflow.com/questions/1822887/what-is-the-best-way-to-eliminate-ms-visual-c-linker-warning-warning-lnk4221
// comment out to identify empty translation units
#pragma warning ( disable: 4221 )	// no public symbols found; archive member will be inaccessible

// Solution: Use an anonymous namespace:
// namespace { char dummy; };
// Symbols within such namespace have external linkage,
// so there will be something in the export table.

// The following macro "NoEmptyFile()" can be put into a file
// in order suppress the MS Visual C++ Linker warning 4221
//
// warning LNK4221: no public symbols found; archive member will be inaccessible
//
// This warning occurs on PC and XBOX when a file compiles out completely
// has no externally visible symbols which may be dependent on configuration
// #defines and options.

#define NO_EMPTY_FILE	namespace { char NoEmptyFileDummy##__LINE__; }




//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
