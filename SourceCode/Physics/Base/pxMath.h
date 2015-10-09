/*
=============================================================================
	File:	Math.h
	Desc:	.
=============================================================================
*/

#ifndef __PX_MATH_H__
#define __PX_MATH_H__

// Tests if the two values are equal (with respect to specified epsilon).
FORCEINLINE bool EqualFloats( FLOAT x, FLOAT y, FLOAT epsilon = SMALL_NUMBER )
{
	return mxFabs( x - y ) < epsilon;
}

#endif // !__PX_MATH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
