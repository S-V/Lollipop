/*
=============================================================================
	File:	common.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#if PX_COLLECT_STATISTICS
	
	pxGlobalStats	gPhysStats;

#endif//PX_COLLECT_STATISTICS

void* pxNew( SizeT numBytes )
{
	return mxAllocX( EMemHeap::HeapPhysics, numBytes );
}
void pxFree( void* ptr )
{
	mxFreeX( EMemHeap::HeapPhysics, ptr );
}

/*
-----------------------------------------------------------------------------
	pxMaterial
-----------------------------------------------------------------------------
*/
pxMaterial::pxMaterial()
{
	friction = 0.6f;
	restitution = 0.1f;
	userData = nil;
}

pxMaterial* pxMaterial::Static_GetPointerByHandle( Handle handle )
{
	return &Physics::GetMaterial( handle );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
