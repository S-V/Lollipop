/*
=============================================================================
	File:	pxShapeRayCastOutput.h
	Desc:	
=============================================================================
*/

#ifndef __PX_SHAPE_RAYCAST_OUTPUT_H__
#define __PX_SHAPE_RAYCAST_OUTPUT_H__

//
//	pxShapeRayCastOutput
//
struct pxShapeRayCastOutput
{
	// The normalized normal of the surface hit by the ray. This normal is in the space of the ray.
	// It's only valid if m_hitFraction < 1.0.
	pxVec3	m_normal;

	// This value will be in the range of [0..1]. It determines where along the ray the hit occurred, where 1 is the end position
	// of the ray and 0 is the start position of the ray. It is also used as an "early out" value and has to be initialized with 1.0f.
	pxReal	m_hitFraction;

	// Pad to a multiple of 16 bytes.
	pxS4 m_pad[3];

public:
	inline pxShapeRayCastOutput() {
		this->Reset();
	}

	// Resets this structure if you want to reuse it for another raycast, by setting the hitFraction to 1.
	inline void Reset() {
		m_hitFraction = 1.0f;
	}

	inline bool HasHit() const {
		return m_hitFraction < 1.0f;
	}
};

#endif // !__PX_SHAPE_RAYCAST_OUTPUT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
