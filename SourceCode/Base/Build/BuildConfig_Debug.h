/*
=============================================================================
	File:	BuildConfig_Debug.h
	Desc:	Build options, preprocessor settings for compiling debug versions.
=============================================================================
*/

#ifndef __MX_BUILD_SETTINGS_DEBUG_H__
#define __MX_BUILD_SETTINGS_DEBUG_H__

mxNAMESPACE_BEGIN

enum EAppBuildMode {
	g_bDebugMode = true,
	g_bReleaseMode = false
};

mxNAMESPACE_END


#define MX_BUILD_MODE_STRING	"DEBUG"

// run-time stats
#define	MX_STATS(code)			code

// enable/disable bounds checking
#define MX_BOUNDS_CHECKS		(1)

// enable/disable string length checking (e.g. when copying from one text buffer to a smaller buffer)
#define MX_STRLEN_CHECKS		(1)

// slows down a lot, but helps catch bugs
#define MX_MATH_CHECKS			(0)

#define MX_ENABLE_UNIT_TESTS	(1)


#endif // !__MX_BUILD_SETTINGS_DEBUG_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
