/*
=============================================================================
	File:	pxSupportMap.h
	Desc:	Support maps.
=============================================================================
*/

#ifndef __PX_SUPPORT_MAP_H__
#define __PX_SUPPORT_MAP_H__

//
//	pxSupportMap2D
//
class pxSupportMap2D {
public:
	virtual ~pxSupportMap2D() {}
	virtual pxVec2 GetSupportPoint( const pxVec2& direction ) = 0;
};

//
//	pxSupportMap3D
//
class pxSupportMap3D {
public:
	virtual ~pxSupportMap3D() {}
	virtual pxVec3 GetSupportPoint( const pxVec3& direction ) = 0;
};

#endif // !__PX_SUPPORT_MAP_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
