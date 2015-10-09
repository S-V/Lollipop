/*
=============================================================================
	File:	IOSystem.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include <Core/IO/FileArchive.h>

#include "IOSystem.h"

mxNAMESPACE_BEGIN

/*
--------------------------------------------------------------
	IOSystem
--------------------------------------------------------------
*/
IOSystem::IOSystem()
{
}

void IOSystem::Mount( FileArchive* fileSystem )
{
	DBGOUT("Mounting '%s'\n",fileSystem->GetURI());

	(*this).Add(fileSystem);
}

void IOSystem::Unmount( FileArchive* fileSystem )
{
	DBGOUT("Unmounting '%s'\n",fileSystem->GetURI());

	(*this).Remove(fileSystem);
}

mxDataStream* IOSystem::OpenReader( const char* filePath )
{
	for( UINT iArchive = 0; iArchive < (*this).Num(); iArchive++ )
	{
		mxDataStream* newStream = (*this)[ iArchive ]->OpenReader( filePath );
		if( newStream )
		{
			return newStream;
		}
	}
	return nil;
}

mxDataStream* IOSystem::OpenWriter( const char* filePath )
{
	for( UINT iArchive = 0; iArchive < (*this).Num(); iArchive++ )
	{
		mxDataStream* newStream = (*this)[ iArchive ]->OpenWriter( filePath );
		if( newStream )
		{
			return newStream;
		}
	}
	return nil;
}

FileArchive* IOSystem::FindArchive( const char* pURL )
{
	for(UINT iArchive = 0;
		iArchive < (*this).Num();
		iArchive++)
	{
		FileArchive* archive = (*this)[ iArchive ];

		if( mxStrEquAnsi(archive->GetURI(), pURL) )
		{
			return archive;
		}
	}
	return nil;
}

void IOSystem::Serialize( mxArchive& archive )
{
	TRefCountedObjectList::Serialize( archive );
}

const char* IOSystem::edToChars( UINT column ) const
{
	return "Global File System";
}

AEditable* IOSystem::edGetParent()
{
	return nil;
}

mxNAMESPACE_END
