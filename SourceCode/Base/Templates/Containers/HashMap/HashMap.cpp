/*
=============================================================================
	File:	HashMap.cpp
	Desc:
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN


namespace HashMapUtil
{
	void* AllocateMemory( UINT bytes )
	{
		return mxAlloc( bytes );
	}

	void ReleaseMemory( void* ptr )
	{
		mxFree( ptr );
	}

}//namespace Array_Util

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
