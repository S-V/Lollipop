/*
=============================================================================
	File:	pxPhysicsSystem.h
	Desc:	Physics system.
=============================================================================
*/

#ifndef __PX_PHYSICS_SYSTEM_H__
#define __PX_PHYSICS_SYSTEM_H__

struct pxLimits
{
	pxUInt	maxNumRigidBodies;

public:
	pxLimits()
	{
		setDefaults();
	}
	void setDefaults()
	{
		maxNumRigidBodies = 1024;
	}
	bool isOk()
	{
		return true;
	}
};


#endif // !__PX_PHYSICS_SYSTEM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
