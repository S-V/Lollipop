/*
=============================================================================
	File:	Constants.h
	Desc:	Universal constants.
=============================================================================
*/

#ifndef __MX_UNIVERSAL_CONSTANTS_H__
#define __MX_UNIVERSAL_CONSTANTS_H__

//--------------------------------------------------
//	Definitions of useful mathematical constants.
//--------------------------------------------------

mxNAMESPACE_BEGIN

//
// pi
//
#define		MX_PI			FLOAT( 3.1415926535897932384626433832795028841971693993751 )
#define		MX_PI_f64		DOUBLE( 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798 )
#define		MX_TWO_PI		FLOAT( 6.28318530717958647692528676655901 )	// pi * 2
#define		MX_HALF_PI		FLOAT( 1.5707963267948966192313216916395 )	// pi / 2
#define		MX_ONEFOURTH_PI	FLOAT( 0.78539816339744830961566084581975 )	// pi / 4
#define		MX_INV_PI		FLOAT( 0.31830988618379067153776752674508 )	// 1 / pi
#define		MX_INV_TWOPI	FLOAT( 0.15915494309189533576888376337254 )	// 1 / (pi * 2)
#define		MX_2_DIV_PI		FLOAT( 0.63661977236758134307553505349016 )	// 2 / pi
#define		MX_INV_SQRT_PI	FLOAT( 0.564189583547756286948f )			// 1 / sqrt(pi)
#define		MX_SQRT_PI		FLOAT( 1.772453850905516027298167483341 )	// sqrt(pi)
#define		MX_2_SQRT_PI	FLOAT( 1.1283791670955125738961589031216 )	// 2 / sqrt(pi)
#define		MX_PI_SQUARED	FLOAT( 9.869604401089358618834490999873 )	// pi * pi

#define MX_TANGENT_30		FLOAT( 0.57735026918962576450914878050196f )// tan(30)
#define MX_2_TANGENT_30		FLOAT( (MX_TANGENT_30 * 2.0f)	// 2*tan(30)

//
// Golden ratio ( phi = FLOAT( 0.5 * (Sqrt(5) + 1) ).
//
#define		MX_PHI			FLOAT( 1.61803398874989484820f )
#define		MX_PHI_f64		DOUBLE( 1.6180339887498948482045868343656 )

//
// e
//
#define		MX_E			FLOAT( 2.71828182845904523536f )
#define		MX_E_f64		DOUBLE( 2.7182818284590452353602874713526624977572470936999 )
#define		MX_LOG2E		FLOAT( 1.44269504088896340736f )	// log2(e)
#define		MX_LOG10E		FLOAT( 0.434294481903251827651f )	// log10(e)

#define		MX_LN10			FLOAT( 2.30258509299404568402f )	// ln(10)

#define		MX_INV_LOG2		FLOAT( 3.32192809488736234787f )	// 1.0 / log10(2)
#define		MX_LN2			FLOAT( 0.69314718055994530941723212145818f )	// ln(2)
#define		MX_INV_LN2		FLOAT( 1.44269504089f )				// 1.0f / ln(2)

#define		MX_INV_3		FLOAT( 0.33333333333333333333f )	// 1/3
#define		MX_INV_6		FLOAT( 0.16666666666666666666f )	// 1/6
#define		MX_INV_7		FLOAT( 0.14285714285714285714f )	// 1/7
#define		MX_INV_9		FLOAT( 0.11111111111111111111f )	// 1/9
#define		MX_INV_255		FLOAT( 0.00392156862745098039f )	// 1/255

#define		MX_SQRT_2		FLOAT( 1.4142135623730950488016887242097f )	// sqrt(2)
#define		MX_INV_SQRT_2	FLOAT( 0.7071067811865475244008443621048490f )	// 1 / sqrt(2)

#define		MX_SQRT_3		FLOAT( 1.7320508075688772935274463415059f )	// sqrt(3)
#define		MX_INV_SQRT_3	FLOAT( 0.57735026918962576450f )	// 1 / sqrt(3)

#define		MX_SQRT5		FLOAT( 2.236067977499789696409173668731f )

// Euler's constant
#define		MX_EULER		FLOAT( 0.5772156649015328606065 )

// degrees to radians multiplier
#define		MX_DEG2RAD		FLOAT( 0.0174532925199432957692369076848861 )	// MX_PI / 180.0f

// radians to degrees multiplier
#define		MX_RAD2DEG		FLOAT( 57.2957795130823208767981548141052 )	// 180.0f / MX_PI

// seconds to milliseconds multiplier
#define		MX_SEC2MS		1000

// milliseconds to seconds multiplier
#define		MX_MS2SEC		FLOAT( 0.001f )

//
// Atomic physics constants.
//
namespace KernPhysik
{
	// Unified atomic mass.
	const FLOAT	u  = 1.66053873e-27f;
	
	// Electron mass.
	const FLOAT	me = 9.109373815276e-31f;
	
	// Electron charge.
	const FLOAT	e  = 1.602176462e-16f;
	
	// Proton mass.
	const FLOAT	mp = 1.67262158e-27f;
	
	// Neutron mass.
	const FLOAT	mn = 1.67492716e-27f;

};//End of KernPhysik

//
// Bulk physics constants.
//
namespace Physik
{
	// Standard gravity acceleration.
	const FLOAT	EARTH_GRAVITY	= 9.81f;

	// Boltzmann constant
	const FLOAT	k		= 1.3806503e-23f;
	
	// Electric field constant / permittivity of free space
	const FLOAT	e		= 8.854187817e-12f;
	
	// Gravitational Constant
	const FLOAT	G		= 6.67259f;
	
	// Impedance of free space
	const FLOAT	Z		= 376.731f;
	
	// Speed of light
	const FLOAT	c		= 2.99792458e8f;
	
	// Magnetic field constant / Permeability of a vacuum
	const FLOAT	mu		= 1.2566370614f;
	
	// Planck constant
	const FLOAT	h		= 6.62606876e-34f;

	// Stefan-Boltzmann constant
	const FLOAT	sigma	= 5.670400e-8f;

	// Astronomical unit
	const FLOAT	AU		= 149.59787e11f;

};//End of Physik

//
//	Conversions.
//

#if 1

	#define DEG2RAD( a )		( (a) * MX_DEG2RAD )
	#define RAD2DEG( a )		( (a) * MX_RAD2DEG )

	#define SEC2MS( t )			( mxFloatToInt( (t) * MX_SEC2MS ) )
	#define MS2SEC( t )			( mxIntToFloat( (t) ) * MX_MS2SEC )

	#define ANGLE2SHORT( x )	( mxFloatToInt( (x) * 65536.0f / 360.0f ) & 65535 )
	#define SHORT2ANGLE( x )	( (x) * ( 360.0f / 65536.0f ) ); }

	#define ANGLE2BYTE( x )		( mxFloatToInt( (x) * 256.0f / 360.0f ) & 255 )
	#define BYTE2ANGLE( x )		( (x) * ( 360.0f / 256.0f ) )

#else

	FORCEINLINE FLOAT 	DEG2RAD( FLOAT a )		{ return ( (a) * MX_DEG2RAD ); }
	FORCEINLINE FLOAT 	RAD2DEG( FLOAT a )		{ return ( (a) * MX_RAD2DEG ); }

	FORCEINLINE INT		SEC2MS( FLOAT t )		{ return ( mxFloatToInt( (t) * MX_SEC2MS ) ); }
	FORCEINLINE FLOAT	MS2SEC( INT t )			{ return ( mxIntToFloat( t ) * MX_MS2SEC ); }

	FORCEINLINE UINT16	ANGLE2SHORT( FLOAT x )	{ return ( mxFloatToInt( (x) * 65536.0f / 360.0f ) & 65535 ); }
	FORCEINLINE FLOAT	SHORT2ANGLE( UINT16 x )	{ return ( (x) * ( 360.0f / 65536.0f ) ); }

	FORCEINLINE BYTE	ANGLE2BYTE( FLOAT x )	{ return ( mxFloatToInt( (x) * 256.0f / 360.0f ) & 255 ); }
	FORCEINLINE FLOAT	BYTE2ANGLE( BYTE x )	{ return ( (x) * ( 360.0f / 256.0f ) ); }

#endif

//--------------------------------------------------------------//

// maximum world size (numbers taken from idTech 4)
// 128 * 1024 = 131072
#define MAX_WORLD_COORD			( 128 * 1024 )
#define MIN_WORLD_COORD			( -128 * 1024 )
#define MAX_WORLD_SIZE			( MAX_WORLD_COORD - MIN_WORLD_COORD )
#define MAX_WORLD_HALF_SIZE	(MAX_WORLD_SIZE/2)

#define BIG_NUMBER			999999.0f
#define SMALL_NUMBER		1.e-6f


//--------------------------------------------------------------//
//	String constants
//--------------------------------------------------------------//

// string denoting unknown error
extern const char* MX_STRING_UNKNOWN_ERROR;

mxNAMESPACE_END

#endif // !__MX_UNIVERSAL_CONSTANTS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
