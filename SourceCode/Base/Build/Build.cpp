/*
=============================================================================
	File:	Build.cpp
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

const char* mxGetBaseBuildTimestamp()
{
	// the string containing timestamp information
	local_ const char* g_timeStamp =
		__DATE__ " - " __TIME__

	// MVC++ has the __TIMESTAMP__ macro
	// e.g. "Fri Oct  1 17:06:21 2010"
	;

	return g_timeStamp;
}

const char easterEgg[] =
"C'mon man, get a life!"
;

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
