/*
=============================================================================
	File:	pxContactPoint.h
	Desc:	Contact processing.
=============================================================================
*/

#ifndef __PX_CONTACT_POINT_H__
#define __PX_CONTACT_POINT_H__

//
//	pxContactPoint - a simple structure class representing a contact point with a normal and a penetration depth.
//
//	NOTE: the normal always points "into" A, and "out of" B.
// The penetration depth of a pair of intersecting objects is the shortest vector
// over which one object needs to be translated in order to bring the pair in touching contact.
// NOTE: usually, A represents dynamic (moving) objects and B - static ones.
//
MX_ALIGN_16(struct) pxContactPoint
{
	// position of the contact point (in world space) on bodyB
	pxVec3		position;

	// (xyz) - contact normal vector (toward feature on body A, in world space)
	// (w) - separation distance in the direction of contact normal (penetration depth if positive (>epsilon)),
	// lies in range [ -maximum_penetration_depth, maximum_separation_distance ]
	pxVec4		normalAndDepth;

	//total:16+16=32 bytes

public:
	FORCEINLINE pxContactPoint()
	{}
	FORCEINLINE void setPosition( const pxVec3& newPosition )
	{
		position = newPosition;
	}
	FORCEINLINE const pxVec3& getPosition() const
	{
		return position;
	}
	FORCEINLINE void setNormal( const pxVec3& newNormal )
	{
		normalAndDepth.x = newNormal.x;
		normalAndDepth.y = newNormal.y;
		normalAndDepth.z = newNormal.z;
	}
	FORCEINLINE const pxVec3& getNormal() const
	{
		return normalAndDepth;
	}
	FORCEINLINE void setNormalAndDepth( const pxVec3& newNormal, pxReal depth )
	{
		normalAndDepth.x = newNormal.x;
		normalAndDepth.y = newNormal.y;
		normalAndDepth.z = newNormal.z;
		normalAndDepth.w = depth;
	}
	FORCEINLINE pxReal getDepth() const
	{
		return normalAndDepth.w;
	}
};

PX_OPT_IDEA("pack material params into chars/shorts, they are in [0..1] anyway;"
			"use look-up tables? 16-bit floats?"
			"keep only 16-bit material indices 'materialA' and 'materialB'?"
			);


// contact point ID
typedef pxU4 pxContactPointId;


#endif // !__PX_CONTACT_POINT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
