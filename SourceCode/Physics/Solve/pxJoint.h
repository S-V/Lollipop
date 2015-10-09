/*
=============================================================================
	File:	pxJoint.h
	Desc:	
=============================================================================
*/

#ifndef __PX_JOINT_H__
#define __PX_JOINT_H__

//
//	pxJoint
//
//	A joint is a constraint that connects two rigid bodies.
//
class pxJoint : public pxConstraint {
public:
	pxJoint()
	{}
	virtual ~pxJoint()
	{}
};

#endif // !__PX_JOINT_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
