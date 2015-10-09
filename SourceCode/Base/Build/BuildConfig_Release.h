/*
=============================================================================
	File:	BuildConfig_Release.h
	Desc:	Build options, preprocessor settings for compiling release versions.
=============================================================================
*/

#ifndef __MX_BUILD_SETTINGS_RELEASE_H__
#define __MX_BUILD_SETTINGS_RELEASE_H__

mxNAMESPACE_BEGIN

enum EAppBuildMode {
	g_bDebugMode = false,
	g_bReleaseMode = true
};

mxNAMESPACE_END


#define MX_BUILD_MODE_STRING	"RELEASE"

// run-time stats
#define	MX_STATS(code)

// enable/disable bounds checking
#define MX_BOUNDS_CHECKS		(0)

// enable/disable string length checking (e.g. when copying from one text buffer to a smaller buffer)
#define MX_STRLEN_CHECKS		(0)

#define MX_ENABLE_UNIT_TESTS	(0)

#define MX_MATH_CHECKS			(0)

#endif // !__MX_BUILD_SETTINGS_RELEASE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
