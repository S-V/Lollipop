/*
=============================================================================
	File:	Spatial.h
	Desc:
=============================================================================
*/

#ifndef __MX_SPATIAL_H__
#define __MX_SPATIAL_H__

mxNAMESPACE_BEGIN

//
//	ESpatialRelation (or result of some intersection test)
//
enum ESpatialRelation
{
	Outside		= 0,	// <= Must be equal to zero.
	Intersects	= 1,
	Inside		= 2,

	ESpatialRelation_FORCE_DWORD = 0xFFFFFFFF
};

mxNAMESPACE_END

#endif // ! __MX_SPATIAL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
