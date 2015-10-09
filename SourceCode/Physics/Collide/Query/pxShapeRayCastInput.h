/*
=============================================================================
	File:	pxShapeRayCastInput.h
	Desc:	
=============================================================================
*/

#ifndef __PX_SHAPE_RAYCAST_INPUT_H__
#define __PX_SHAPE_RAYCAST_INPUT_H__

//
//	pxShapeRayCastInput
//
struct pxShapeRayCastInput
{
	// The start position of the ray in local space
	pxVec3	m_from;

	// The end position of the ray in local space
	pxVec3	m_to;

public:
	pxShapeRayCastInput()
	{}

private: PREVENT_COPY(pxShapeRayCastInput);
};

#endif // !__PX_SHAPE_RAYCAST_INPUT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
