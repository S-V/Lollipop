/*
=============================================================================
	File:	FileId.h
=============================================================================
*/

#pragma once

#include <Core/System/StringTable.h>

mxNAMESPACE_BEGIN

#if 0
/*
-----------------------------------------------------------------------------
	FileId - is a unique identifier used for sharing and locating files.
-----------------------------------------------------------------------------
*/
typedef mxName	FileId;

//
// e.g. "../Build/Debug/resource.dat" -> "Build/Debug/resource.dat"
//
void GenerateFileId( PCSTR relativePath, FileId &fileId );
#endif

mxNAMESPACE_END
