/*
=============================================================================
	File:	pxObjects.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

const char* pxGetObjectName( pxcObjectType type )
{
	static const char* objectTypeNames[ PX_OBJECT_TYPE_MAX ] =
	{
#define PX_DECLARE_OBJECT_TYPE(x) #x
#	include "pxObjects.inl"
#undef PX_DECLARE_OBJECT_TYPE
	};
	Assert(type>=0&&type<PX_OBJECT_TYPE_MAX);
	return objectTypeNames[ type ];
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
