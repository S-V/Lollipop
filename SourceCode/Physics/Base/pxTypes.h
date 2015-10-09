/*
=============================================================================
	File:	pxTypes.h
	Desc:	Base types used by the physics library.
=============================================================================
*/

#ifndef __PX_TYPES_H__
#define __PX_TYPES_H__


#include <Core/VectorMath.h>
#include <Physics/Base/pxBuildConfig.h>


typedef unsigned char	pxByte;

typedef int				pxInt;
typedef unsigned int	pxUInt;

typedef long			pxLong;
typedef unsigned long	pxULong;

typedef int				pxBool;

typedef mxInt16			pxS1;
typedef mxUInt16		pxU2;
typedef mxInt16			pxS2;
typedef mxUInt16		pxU2;
typedef mxInt32			pxS4;
typedef mxUInt32		pxU4;
typedef mxInt64			pxS8;
typedef mxUInt64		pxU8;

typedef mxFloat16		pxF2;
typedef float			pxF4;
typedef double			pxF8;
typedef pxF4			pxReal;

#define REAL(x)			(x##f)

//#define SMALL_NUMBER	1.0e-04f
#define PX_BIG_NUMBER	1.0e+04f

#define PX_EPSILON		1.192092896e-07f
#define PX_INFINITY		1e30f

// NOTE: keep (PX_LARGE_FLOAT * PX_LARGE_FLOAT) < FLT_MAX
#define PX_LARGE_FLOAT		1e18f
#define PX_TINY_NUMBER		1e-4f

#define PX_POSITIVE_INFINITY	1e18f
#define PX_NEGATIVE_INFINITY	-1e18f

#define ANGULAR_MOTION_THRESHOLD MX_ONEFOURTH_PI


typedef Vec2D pxVec2;

// all matrices are row major
typedef Matrix2			pxMat2x2;
typedef Matrix4			pxMat4x4;

#define TArray		TList

//
//	pxFloatBuffer
//
typedef TList< pxReal >	pxFloatBuffer;


inline void px_static_checks_types()
{
	COMPILE_TIME_ASSERT(sizeof(void*)==sizeof(pxS4));
	COMPILE_TIME_ASSERT(sizeof(pxReal)==sizeof(float));
}









#endif // !__PX_TYPES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
