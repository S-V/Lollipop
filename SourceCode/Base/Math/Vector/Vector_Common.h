/*
=============================================================================
	File:	Vector_Common.h
	Desc:	Common stuff used by vector classes.
=============================================================================
*/

#ifndef __MATH_VECTOR_COMMON_H__
#define __MATH_VECTOR_COMMON_H__
mxSWIPED("idSoftware");
mxNAMESPACE_BEGIN

// This value controls vector normalization, vector comparison routines, etc.
const FLOAT VECTOR_EPSILON = 0.001f;

//
//	EVectorComponent - For referring to the components of a vector by name rather than by an integer.
//
enum EVectorComponent
{
	Vector_X = 0,
	Vector_Y,
	Vector_Z,
	Vector_W,

	VC_FORCE_DWORD	= MAX_SDWORD
};

enum EAxisType
{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
};

//
//	EViewAxes
//
enum EViewAxes
{
	VA_Right	= Vector_X,
	VA_Up		= Vector_Y,	// 'upward'
	VA_LookAt	= Vector_Z,	// Look direction vector ('forward').
};

mxNAMESPACE_END

/*
===============================================================================

	Old 3D vector macros, should no longer be used.

===============================================================================
*/

#define DotProduct( a, b)			((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VectorSubtract( a, b, c )	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd( a, b, c )		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define	VectorScale( v, s, o )		((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA( v, s, b, o )		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorCopy( a, b )			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])

#define	DEFAULT_LOOK_VECTOR		Vec3D( 0.0f, 0.0f, 1.0f )
#define	WORLD_RIGHT_VECTOR		Vec3D( 1.0f, 0.0f, 0.0f )
#define	WORLD_UP_VECTOR			Vec3D( 0.0f, 1.0f, 0.0f )

#define	UNTRANSFORMED_NORMAL	DEFAULT_LOOK_VECTOR

#endif /* !__MATH_VECTOR_COMMON_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
