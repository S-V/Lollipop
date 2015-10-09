/*
=============================================================================
	File:	Math.h
	Desc:	Common math constants, tables, global math class,
			floating-point arithmetic and so on.
	NOTE:	This file should be included before other math library headers.
			The math library must be initialized prior to its use.
	
	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idMath )
=============================================================================
*/

#ifndef __GLOBAL_MATH_H__
#define __GLOBAL_MATH_H__
mxSWIPED("idSoftware");
// Set this to 1 to use Id's sqrt and trig approximations and floating-point tricks.
// Note: this can actually be slower on modern CPUs because
// they operate on the integer representation of floating point numbers
// and data transfers between integer and floating-point units are relatively slow
// (think of your CPU as two CPUs - integer and floating-point one).
// Besides, many of those functions use look-up tables which create data dependencies and lead to memory latency.
//
#define MX_MATH_USE_ID_APPROX	(0)

mxNAMESPACE_BEGIN

//
//	Math
//
class Math {
public:
	static void		Init( void );
	static void		Shutdown( void );

	static bool		IsInitialized();

public:
	static FLOAT		RSqrt( FLOAT x );			// reciprocal square root, returns huge number when x == 0.0, assumes x > 0.

	static FLOAT		InvSqrt( FLOAT x );			// inverse square root with 32 bits precision, returns huge number when x == 0.0, assumes x > 0.
	static FLOAT		InvSqrt16( FLOAT x );		// inverse square root with 16 bits precision, returns huge number when x == 0.0, assumes x > 0.
	static DOUBLE		InvSqrt64( FLOAT x );		// inverse square root with 64 bits precision, returns huge number when x == 0.0, assumes x > 0.

	static FLOAT		Sqrt( FLOAT x );			// square root with 32 bits precision
	static FLOAT		Sqrt16( FLOAT x );			// square root with 16 bits precision
	static DOUBLE		Sqrt64( FLOAT x );			// square root with 64 bits precision

	static FLOAT		Sin( FLOAT a );				// sine with 32 bits precision
	static FLOAT		Sin16( FLOAT a );			// sine with 16 bits precision, maximum absolute error is 2.3082e-09
	static DOUBLE		Sin64( FLOAT a );			// sine with 64 bits precision

	static FLOAT		Cos( FLOAT a );				// cosine with 32 bits precision
	static FLOAT		Cos16( FLOAT a );			// cosine with 16 bits precision, maximum absolute error is 2.3082e-09
	static DOUBLE		Cos64( FLOAT a );			// cosine with 64 bits precision

	static void			SinCos( FLOAT a, FLOAT &s, FLOAT &c );		// sine and cosine with 32 bits precision
	static void			SinCos16( FLOAT a, FLOAT &s, FLOAT &c );	// sine and cosine with 16 bits precision
	static void			SinCos64( FLOAT a, DOUBLE &s, DOUBLE &c );	// sine and cosine with 64 bits precision

	static FLOAT		Tan( FLOAT a );				// tangent with 32 bits precision
	static FLOAT		Tan16( FLOAT a );			// tangent with 16 bits precision, maximum absolute error is 1.8897e-08
	static DOUBLE		Tan64( FLOAT a );			// tangent with 64 bits precision

	static FLOAT		ASin( FLOAT a );			// arc sine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
	static FLOAT		ASin16( FLOAT a );			// arc sine with 16 bits precision, maximum absolute error is 6.7626e-05
	static DOUBLE		ASin64( FLOAT a );			// arc sine with 64 bits precision

	static FLOAT		ACos( FLOAT a );			// arc cosine with 32 bits precision, input is clamped to [-1, 1] to avoid a silent NaN
	static FLOAT		ACos16( FLOAT a );			// arc cosine with 16 bits precision, maximum absolute error is 6.7626e-05
	static DOUBLE		ACos64( FLOAT a );			// arc cosine with 64 bits precision

	static FLOAT		ATan( FLOAT a );			// arc tangent with 32 bits precision
	static FLOAT		ATan16( FLOAT a );			// arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
	static DOUBLE		ATan64( FLOAT a );			// arc tangent with 64 bits precision

	static FLOAT		ATan( FLOAT y, FLOAT x );	// arc tangent with 32 bits precision
	static FLOAT		ATan16( FLOAT y, FLOAT x );	// arc tangent with 16 bits precision, maximum absolute error is 1.3593e-08
	static DOUBLE		ATan64( FLOAT y, FLOAT x );	// arc tangent with 64 bits precision

	static FLOAT		Pow( FLOAT x, FLOAT y );	// x raised to the power y with 32 bits precision
	static FLOAT		Pow16( FLOAT x, FLOAT y );	// x raised to the power y with 16 bits precision
	static DOUBLE		Pow64( FLOAT x, FLOAT y );	// x raised to the power y with 64 bits precision

	static FLOAT		Exp( FLOAT f );				// e raised to the power f with 32 bits precision
	static FLOAT		Exp16( FLOAT f );			// e raised to the power f with 16 bits precision
	static DOUBLE		Exp64( FLOAT f );			// e raised to the power f with 64 bits precision

	static FLOAT		Log( FLOAT f );				// natural logarithm with 32 bits precision
	static FLOAT		Log16( FLOAT f );			// natural logarithm with 16 bits precision
	static DOUBLE		Log64( FLOAT f );			// natural logarithm with 64 bits precision

	static FLOAT		Log10( FLOAT f );

	static INT			IPow( INT x, INT y );		// integral x raised to the power y
	static INT			ILog2( FLOAT f );			// integral base-2 logarithm of the floating point value
	static INT			ILog2( INT i );				// integral base-2 logarithm of the integer value

	static INT			BitsForFloat( FLOAT f );	// minumum number of bits required to represent ceil( f )
	static INT			BitsForInteger( INT i );	// minumum number of bits required to represent i
	static INT			MaskForFloatSign( FLOAT f );// returns 0x00000000 if x >= 0.0f and returns 0xFFFFFFFF if x <= -0.0f
	static INT			MaskForIntegerSign( INT i );// returns 0x00000000 if x >= 0 and returns 0xFFFFFFFF if x < 0

	static FLOAT		Fabs( FLOAT f );			// returns the absolute value of the floating point value
	static FLOAT		Floor( FLOAT f );			// returns the largest value that is less than or equal to the given value
	static FLOAT		Ceil( FLOAT f );			// returns the smallest value that is greater than or equal to the given value
	static INT			Rint( FLOAT f );			// returns the nearest integer
	static FLOAT		IntToF( INT i );			// INT to FLOAT conversion
	static INT			Ftoi( FLOAT f );			// FLOAT to INT conversion
	static INT			FtoiFast( FLOAT f );		// fast FLOAT to INT conversion but uses current FPU round mode (default round nearest)
	static FLOAT		UByteToF( UBYTE c );		// unsigned byte to float conversion
	static UBYTE		FtoB( FLOAT f );			// float to byte conversion, the result is clamped to the range [0-255]

	static FLOAT		Reciprocal( FLOAT f );

	static FLOAT		FMod( FLOAT f, FLOAT m );

	static CHAR			ClampChar( INT i );
	static INT16		ClampShort( INT i );
	static INT			ClampInt( INT min, INT max, INT value );
	static FLOAT		ClampFloat( FLOAT min, FLOAT max, FLOAT value );

	static FLOAT		AngleNormalize360( FLOAT angle );
	static FLOAT		AngleNormalize180( FLOAT angle );
	static FLOAT		AngleDelta( FLOAT angle1, FLOAT angle2 );

	static INT			FloatToBits( FLOAT f, INT exponentBits, INT mantissaBits );
	static FLOAT		BitsToFloat( INT i, INT exponentBits, INT mantissaBits );

	static INT			FloatHash( const FLOAT *array, const INT numFloats );


	static DOUBLE		PerlinNoise( INT x, INT y, INT random );	// returns value in range [-1.0..1.0]

private:

#if MX_MATH_USE_ID_APPROX

	enum {
		LOOKUP_BITS			= 8,							
		EXP_POS				= 23,							
		EXP_BIAS			= 127,							
		LOOKUP_POS			= (EXP_POS-LOOKUP_BITS),
		SEED_POS			= (EXP_POS-8),
		SQRT_TABLE_SIZE		= (2<<LOOKUP_BITS),
		LOOKUP_MASK			= (SQRT_TABLE_SIZE-1)
	};

	static DWORD	iSqrt[ SQRT_TABLE_SIZE ];	// Inverse square root lookup table.

#endif // MX_MATH_USE_ID_APPROX

	// A lookup table for converting unsigned chars to floats.
	// Useful for avoiding LHS.
	enum { CHAR_TABLE_SIZE = 256 };
	static FLOAT	Uint8ToFloat32[ CHAR_TABLE_SIZE ];

	static bool		initialized;	// True if global Math has been successfully initialized.

private:
	// Disallow instantiating.
	PREVENT_COPY(Math);

	void ZZStaticChecks()
	{
		mxSTATIC_ASSERT( 1
					&& sizeof(INT) == sizeof(FLOAT)
					&& sizeof(INT) == sizeof(INT32)
					&& sizeof(DWORD) == sizeof(INT32)
					&& sizeof(DOUBLE) == sizeof(double)
		);
	}
};

#include "Math.inl"

mxNAMESPACE_END


// Reflection
#include <Base/Object/Reflection.h>



// Common
#include "Math.h"
#include "Random.h"
#include "Complex.h"

// Vector
#include "Vector/Vector_Common.h"
#include "Vector/Vec2D.h"
#include "Vector/Vec3D.h"
#include "Vector/Vec4D.h"

// Matrix
#include "Matrix/Matrix_Common.h"
#include "Matrix/Matrix2x2.h"
#include "Matrix/Matrix3x3.h"
#include "Matrix/Matrix4x4.h"

// Orientation / Rotation
#include "Angles.h"
#include "Quaternion.h"
#include "Rotation.h"

// Lines, planes
#include "Line.h"
#include "Plane.h"
#include "Pluecker.h"

// Bounding volumes.
#include "BoundingVolumes/Spatial.h"
#include "BoundingVolumes/Sphere.h"
#include "BoundingVolumes/AABB.h"
#include "BoundingVolumes/OOBB.h"
#include "BoundingVolumes/ViewFrustum.h"

// Math Utilities.
#include "Utils.h"


mxTextStream & operator << ( mxTextStream & rStream, const Vec2D& v );
mxTextStream & operator << ( mxTextStream & rStream, const Vec3D& v );
mxTextStream & operator << ( mxTextStream & rStream, const Vec4D& v );

mxTextStream & operator << ( mxTextStream & rStream, const Matrix2& m2x2 );
mxTextStream & operator << ( mxTextStream & rStream, const Matrix3& m3x3 );
mxTextStream & operator << ( mxTextStream & rStream, const Matrix4& m4x4 );

#endif /* !__GLOBAL_MATH_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
