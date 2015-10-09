/*
=============================================================================
	File:	BuildDefines.h
	Desc:	Compilation options, preprocessor settings for compiling different versions.
	Note:	Always make sure this header gets included first.
=============================================================================
*/

#ifndef __MX_BUILD_DEFINES_H__
#define __MX_BUILD_DEFINES_H__

#include "Language.h"

//----------------------------------------------
//	General.
//----------------------------------------------

#if 0

#define mxNAMESPACE_NAME	lol
#define mxNAMESPACE_PREFIX	::mxNAMESPACE_NAME::
#define mxNAMESPACE_BEGIN	namespace mxNAMESPACE_NAME {
#define mxNAMESPACE_END	}
#define mxUSING_NAMESPACE	using namespace ::mxNAMESPACE_NAME;

#else

#define mxNAMESPACE_NAME
#define mxNAMESPACE_PREFIX
#define mxNAMESPACE_BEGIN
#define mxNAMESPACE_END
#define mxUSING_NAMESPACE

#endif

//----------------------------------------------
//	Debug/Release.
//----------------------------------------------

#if defined(___DEBUG) || defined(_DEBUG) || !defined(NDEBUG) || MX_FORCE_DEBUG
	#define MX_DEBUG	(1)	// Debug build.
#else
	#define MX_DEBUG	(0)
#endif


#if MX_DEBUG
	#include "BuildConfig_Debug.h"
#else
	#include "BuildConfig_Release.h"
#endif// MX_DEBUG

// 0 to force single-threaded build
#define MX_MULTITHREADED	(0)

//----------------------------------------------
//	Development.
//	Don't forget to disable some of these settings
//	in the final build of the product!
//----------------------------------------------


#ifndef MX_CHECKED
// perform advanced error checking (slows down a lot!)
#define MX_CHECKED	(MX_DEBUG)
#endif


#ifndef NO_DEVELOPER
// Emit code for statistics, testing, in-game profiling and debugging, developer console, etc.
#define MX_DEVELOPER			(1)
#else
#define MX_DEVELOPER			(0)
#endif


#ifndef NO_PROFILING
// Profile and collect data for analysis.
#define MX_ENABLE_PROFILING		(1)
#else
#define MX_ENABLE_PROFILING		(0)
#endif


#define MX_ENABLE_REFLECTION	(1)




//#define NO_EDITOR

// set when building the editor (and other tools)
// slows down and wastes memory
#ifndef NO_EDITOR
#define MX_EDITOR	(1)
#else
#define MX_EDITOR	(0)
#endif 



//----------------------------------------------
//	Misc settings.
//----------------------------------------------


#define MX_USE_ASM	(0)
#define MX_USE_SSE	(1)


//----------------------------------------------
//	Engine versioning.
//----------------------------------------------

//
// These are used for preventing file version problems, DLL hell, etc.
//
#define mxENGINE_VERSION_MINOR	0
#define mxENGINE_VERSION_MAJOR	0

#define mxENGINE_VERSION_NUMBER	0
#define mxENGINE_VERSION_STRING	"0.0"

// If this is defined, the executable will work only with demo data.
#define MX_DEMO_BUILD			(1)

// returns a string reflecting the time when the Base project was built
// in the format "Mmm dd yyyy - hh:mm:ss"
extern const char* mxGetBaseBuildTimestamp();

#endif // !__MX_BUILD_DEFINES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
