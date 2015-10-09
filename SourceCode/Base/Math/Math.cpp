/*
=============================================================================
	File:	Math.cpp
	Desc:	Common math constants, tables, global math class,
			floating-point arithmetic and so on.

			Originally written by Id Software.
			Copyright (C) 2004 Id Software, Inc. ( idMath )
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

bool	Math::initialized		= false;

#if MX_MATH_USE_ID_APPROX

	DWORD	Math::iSqrt[ SQRT_TABLE_SIZE ];			// inverse square root lookup table

#endif // MX_MATH_USE_ID_APPROX

FLOAT	Math::Uint8ToFloat32[ CHAR_TABLE_SIZE ];	// 'unsigned char' to 'float' conversion table

/*
===============
Math::Init
===============
*/
void Math::Init( void )
{
	Assert( ! IsInitialized() );

	// Check that floating-point types on this machine conform to the IEEE 754 standard.
	// Run unit-tests ( e.g. if memzero is valid for zeroing out floats, etc. ).
	mxTODO( "floating-point data type validation" );

	//===========================================================================

	// TODO: check SSE support, create a SIMD processor, set FPU states, run unit-tests, etc.

	//===========================================================================

	//
	//	Initialize look-up tables.
	//

	//********************************************************

#if MX_MATH_USE_ID_APPROX
    union _flint fi, fo;

    for ( UINT i = 0; i < SQRT_TABLE_SIZE; i++ )
	{
        fi.i	 = ((EXP_BIAS-1) << EXP_POS) | (i << LOOKUP_POS);
        fo.f	 = (FLOAT)( 1.0 / sqrt( fi.f ) );
        iSqrt[i] = ((DWORD)(((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF))<<SEED_POS;
    }
    
	iSqrt[ SQRT_TABLE_SIZE / 2 ] = ((DWORD)(0xFF))<<(SEED_POS); 
#endif // MX_MATH_USE_ID_APPROX

	//********************************************************
	for ( UINT i = 0; i < CHAR_TABLE_SIZE; i++ )
	{
		Uint8ToFloat32[ i ] = (FLOAT) ((unsigned char) i );
	}
	//********************************************************

	initialized = true;
}

/*
===============
Math::Shutdown
===============
*/
void Math::Shutdown( void )
{
	Assert( IsInitialized() );

	// Let it be initialized...
}

/*
===============
Math::IsInitialized
===============
*/
bool Math::IsInitialized( void )
{
	return initialized;
}

/*
================
Math::FloatToBits
================
*/
INT Math::FloatToBits( FLOAT f, INT exponentBits, INT mantissaBits )
{
	INT i, sign, exponent, mantissa, value;

	Assert( exponentBits >= 2 && exponentBits <= 8 );
	Assert( mantissaBits >= 2 && mantissaBits <= 23 );

	INT maxBits = ( ( ( 1 << ( exponentBits - 1 ) ) - 1 ) << mantissaBits ) | ( ( 1 << mantissaBits ) - 1 );
	INT minBits = ( ( ( 1 <<   exponentBits       ) - 2 ) << mantissaBits ) | 1;

	FLOAT max = BitsToFloat( maxBits, exponentBits, mantissaBits );
	FLOAT min = BitsToFloat( minBits, exponentBits, mantissaBits );

	if ( f >= 0.0f ) {
		if ( f >= max ) {
			return maxBits;
		} else if ( f <= min ) {
			return minBits;
		}
	} else {
		if ( f <= -max ) {
			return ( maxBits | ( 1 << ( exponentBits + mantissaBits ) ) );
		} else if ( f >= -min ) {
			return ( minBits | ( 1 << ( exponentBits + mantissaBits ) ) );
		}
	}

	exponentBits--;
	i = *reinterpret_cast<INT *>(&f);
	sign = ( i >> IEEE_FLT_SIGN_BIT ) & 1;
	exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
	mantissa = i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 );
	value = sign << ( 1 + exponentBits + mantissaBits );
	value |= ( ( INT32_SIGN_BIT_SET( exponent ) << exponentBits ) | ( abs( exponent ) & ( ( 1 << exponentBits ) - 1 ) ) ) << mantissaBits;
	value |= mantissa >> ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
	return value;
}

/*
================
Math::BitsToFloat
================
*/
FLOAT Math::BitsToFloat( INT i, INT exponentBits, INT mantissaBits )
{
	static INT exponentSign[2] = { 1, -1 };
	INT sign, exponent, mantissa, value;

	Assert( exponentBits >= 2 && exponentBits <= 8 );
	Assert( mantissaBits >= 2 && mantissaBits <= 23 );

	exponentBits--;
	sign = i >> ( 1 + exponentBits + mantissaBits );
	exponent = ( ( i >> mantissaBits ) & ( ( 1 << exponentBits ) - 1 ) ) * exponentSign[( i >> ( exponentBits + mantissaBits ) ) & 1];
	mantissa = ( i & ( ( 1 << mantissaBits ) - 1 ) ) << ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
	value = sign << IEEE_FLT_SIGN_BIT | ( exponent + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS | mantissa;
	return *reinterpret_cast<FLOAT *>(&value);
}


mxTextStream & operator << ( mxTextStream & rStream, const Vec4D& v )
{
	rStream.Printf("(%.3f, %.3f, %.3f, %.3f)",v.x,v.y,v.z,v.w);
	return rStream;
}

mxTextStream & operator << ( mxTextStream & rStream, const Matrix4& m4x4 )
{
	rStream << m4x4[0] << "\n";
	rStream << m4x4[1] << "\n";
	rStream << m4x4[2] << "\n";
	rStream << m4x4[3] << "\n";
	return rStream;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
