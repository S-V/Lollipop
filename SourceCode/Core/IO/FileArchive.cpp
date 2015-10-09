/*
=============================================================================
	File:	FileArchive.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/IO/IOSystem.h>
#include "FileArchive.h"

mxNAMESPACE_BEGIN

/*
--------------------------------------------------------------
	FileArchive
--------------------------------------------------------------
*/

mxDEFINE_ABSTRACT_CLASS(FileArchive);

FileArchive::FileArchive()
{
}

FileArchive::~FileArchive()
{

}

void FileArchive::Serialize( mxArchive& archive )
{
	Super::Serialize( archive );
}

const char* FileArchive::edToChars( UINT column ) const
{
	return this->GetURI();
}

AEditable* FileArchive::edGetParent()
{
	return gCore.ioSystem;
}

#if 0
// e.g. "R:/resource.dat" -> "R/resource.dat" <= NOT SUPPORTED!
// e.g. "../Build/Debug/resource.dat" -> "Build/Debug/resource.dat"
//
void GenerateFileId( PCSTR relativePath, FileId &fileId )
{
	AssertPtr(relativePath);
	Assert(mxStrLenAnsi(relativePath) > 3);
	Assert(relativePath[1] != ':');

	const char* p = relativePath;

	// skip leading dots and slashes...

	while( *p && !mxCharIsAlpha( *p ) )
	{
		if( '.' == *p
			|| PATHSEPARATOR_CHAR == *p
			|| DRIVE_SEPARATOR_CHAR == *p )
		{
			++p;
			continue;
		}

		++p;
	}

	fileId = p;
}
#endif


mxNAMESPACE_END
