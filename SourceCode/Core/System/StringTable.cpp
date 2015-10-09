/*
=============================================================================
	File:	StringTable.cpp
	Desc:	
=============================================================================
*/

#include <Core_PCH.h>
#pragma hdrstop
#include <Core.h>

#include "StringTable.h"

mxNAMESPACE_BEGIN

SizeT  mxName::strnum = 0;
SizeT  mxName::strmemory = 0;
mxName::strptr  mxName::empty = { 0, 1, 0, 0, 0, 0, 0, 0 };
mxName::strptr *mxName::table[HASH_SIZE] = {0};
mxName::list_node *mxName::pointers[CACHE_SIZE] = {0};
mxName::list_node *mxName::allocs = 0;

void mxName::StaticInitialize() throw()
{
	mxName::empty.next = nil;
	mxName::empty.refcounter = 1;
	mxName::empty.length = 0;
	mxName::empty.hash = 0;
	ZERO_OUT(mxName::empty.body);
}

void mxName::StaticShutdown() throw()
{

}

AStreamWriter& operator << ( AStreamWriter& file, const mxName& o )
{
	const U4 len = mxStrLenAnsi(o.ToChars());
	file << len;
	if( len > 0 ) {
		file.Write( o.ToChars(), len );
	}
	return file;
}

AStreamReader& operator >> ( AStreamReader& file, mxName& o )
{
	U4 len;
	file >> len;

	if( len > 0 )
	{
		char buffer[ mxName::MAX_SIZE ];
		Assert(len < sizeof(buffer));
		file.Read( buffer, len );

		buffer[ len ] = 0;

		o = mxName( buffer );
	}

	return file;
}

mxNAMESPACE_END
