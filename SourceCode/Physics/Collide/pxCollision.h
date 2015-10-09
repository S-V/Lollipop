/*
=============================================================================
	File:	Collision.h
	Desc:	Collision detection, common stuff.
=============================================================================
*/

#ifndef __PX_COLLISION_H__
#define __PX_COLLISION_H__

#define PX_USE_CONTACT_CACHING	(1)
#define PX_USE_CONTINUOUS_COLLISION_DETECTION	(0)

//------------------------------------------------------------------------
// PX_COLLISION_TOLERANCE - used by broad-phase collision detection.
//
// From Havok SDK:
// Each time the shortest distance between a pair of objects overlapping in the broadphase becomes less than PX_COLLISION_TOLERANCE,
// the narrowphase collision agent for those objects creates a contact point or points for them (some agents create more than
// one contact point at a time).
// Having a non-zero collision tolerance can greatly help with two performance-related issues. Firstly, it
// can sometimes be useful for the system to pick up potential collisions just before they actually happen
// - when the distance between the objects is still greater than zero, as in our example. For fast-moving
// objects, this allows the collision solver to prevent interpenetration as early as possible.
// It can also be useful for the system to maintain collision information even when the objects are slightly
// separated. For instance, when an object is sliding across or settling on another object, this allows the
// system to maintain the same manifold rather than having to create new contact points in each simulation
// step.
//
MX_GLOBAL_CONST pxReal PX_COLLISION_TOLERANCE = REAL(0.25);

//------------------------------------------------------------------------
// PX_COLLISION_MARGIN - used by near-phase collision detection.
//
// From Havok SDK:
// Adding a radius to a shape can improve performance. The core convex-convex
// collision detection algorithm is fast when shapes are not interpenetrating, and slower when they are.
// Adding a radius makes it less likely that the shapes themselves will interpenetrate, thus reducing the
// likelihood of the slower algorithm being used. The shell is thus faster in situations where there is a risk
// of shapes interpenetrating - for instance, when an object is settling or sliding on a surface, when there is
// a stack of objects, or when many objects are jostling together.
//
MX_GLOBAL_CONST pxReal PX_COLLISION_MARGIN = REAL(0.05);

//------------------------------------------------------------------------
// PX_CONTACT_THRESHOLD - used by near-phase collision detection.
//
// if separating distance (distance between two closest points) is larger
// than this value, the contact won't be processed.
//
//MX_GLOBAL_CONST pxReal PX_CONTACT_THRESHOLD = REAL(0.02);

//------------------------------------------------------------------------
// PX_COLLISION_SKIN_WIDTH - used by near-phase collision detection.
//MX_GLOBAL_CONST pxReal PX_COLLISION_SKIN_WIDTH = pxReal(0.1);

//------------------------------------------------------------------------
// Testing & Debugging.
//------------------------------------------------------------------------

#if PX_DEBUG

// system blows up when penetration depth is too big
MX_GLOBAL_CONST pxReal PX_MAX_PENETRATION_DEPTH = REAL(0.1);

#define PX_DBG_CHECK_PENETRATION(x)	Assert(mxFabs(x)<PX_MAX_PENETRATION_DEPTH)

#else

#endif

#endif // !__PX_COLLISION_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
