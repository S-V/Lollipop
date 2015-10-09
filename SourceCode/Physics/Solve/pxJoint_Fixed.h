/*
=============================================================================
	File:	pxJoint_Fixed.h
	Desc:	
=============================================================================
*/

#ifndef __PX_JOINT_FIXED_H__
#define __PX_JOINT_FIXED_H__

//
//	pxJoint_Fixed
//
//	A fixed joint is a constraint that restrains two rigid bodies completely from any relative motion,
//	it 'glues' them together with a constant distance between so that they behave like a single rigid body.
//	Thus, it removes all six degrees of freedom from both bodies and they must have the same angular velocity.
//
class pxJoint_Fixed : public pxJoint {
public:
	pxJoint_Fixed()
	{}
	PX_VIRTUAL ~pxJoint_Fixed()
	{}
};

#endif // !__PX_JOINT_FIXED_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
