/*
=============================================================================
	File:	Win32_Undefs.h
	Desc:	Removes some Windows-specific macros that might cause name clashes.
=============================================================================
*/
#pragma once

#undef BYTE
#undef BOOL
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef DOUBLE
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT

#undef CDECL

// defined in shlobj.h
#undef NUM_POINTS

//#ifdef interface
//#undef interface
//#endif

//#ifdef CreateEvent
//#undef CreateEvent
//#endif

//#ifdef CreateFile
//#undef CreateFile
//#endif

//#ifdef CopyFile
//#undef CopyFile
//#endif
//
//#ifdef GetObject
//#undef GetObject
//#endif
//
//#ifdef GetClassName
//#undef GetClassName
//#endif
//
//#ifdef RegisterClass
//#undef RegisterClass
//#endif

//#ifdef SendMessage
//#undef SendMessage
//#endif

//#ifdef DrawText
//#undef DrawText
//#endif

//#ifdef GetCurrentTime
//#undef GetCurrentTime
//#endif
//
//#ifdef Yield
//#undef Yield
//#endif

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
