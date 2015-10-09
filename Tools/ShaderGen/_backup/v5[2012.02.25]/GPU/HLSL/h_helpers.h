//-------------------------------------------------------------------
//	Misc helpers & constants
//-------------------------------------------------------------------

#ifndef __HELPERS_HLSL__
#define __HELPERS_HLSL__

// maximum view-space depth
#define MAX_DEPTH		2000.0f
#define INV_MAX_DEPTH	(1/MAX_DEPTH)

//
// Colors.
//
#define	BLACK				float4( 0.0f, 0.0f, 0.0f, 1.0f )
#define	GRAY				float4( 0.5f, 0.5f, 0.5f, 1.0f )
#define	GREY				GRAY
#define	DARK_GRAY			float4( 0.25f, 0.25f, 0.25f, 1.0f )
#define	VERY_DARK_GRAY		float4( 0.11f, 0.11f, 0.11f, 1.0f )
#define	WHITE				float4( 1.0f, 1.0f, 1.0f, 1.0f )

#define	YELLOW				float4( 1.0f, 	1.0f,	0.0f,	1.0f )
#define	BEACH_SAND			float4( 1.0f, 	0.96f,	0.62f,	1.0f )
#define	GREEN				float4( 0.0f, 	1.0f,	0.0f,	1.0f )
#define	LIGHT_YELLOW_GREEN	float4( 0.48f,	0.77f, 	0.46f, 	1.0f )
#define	DARK_YELLOW_GREEN	float4( 0.1f, 	0.48f, 	0.19f, 	1.0f )
#define	BLUE				float4( 0.0f, 	0.0f, 	1.0f, 	1.0f )
#define	RED					float4( 1.0f, 	0.0f, 	0.0f, 	1.0f )
#define	CYAN				float4( 0.0f, 	1.0f, 	1.0f, 	1.0f )
#define	MAGENTA				float4( 1.0f, 	0.0f, 	1.0f, 	1.0f )
#define	DARKBROWN			float4( 0.45f,	0.39f,	0.34f,	1.0f )


#define AIR_REFRACTION_INDEX          1.0f
#define GLASS_REFRACTION_INDEX        1.61f
#define WATER_REFRACTION_INDEX        1.33f
#define DIAMOND_REFRACTION_INDEX      2.42f
#define ZIRCONIUM_REFRACTION_INDEX    2.17f

//-------------------------------------------------------------------

float3x3 InvertMatrix3x3( float3x3 mat )
{
	float det = determinant( mat );
	float3x3 T = transpose( mat );
	return float3x3(
		cross( T[1], T[2] ),
		cross( T[2], T[0] ),
		cross( T[0], T[1] )
		) / det;
}

// Helper function for converting depth information to a color ( 3x8 bits integer ).
float3 FloatToColor( in float f )
{
	float3 color;
	f *= 256.0f;
	color.x = floor( f );
	f = (f - color.x) * 256.0f;
	color.y = floor( f );
	color.z = f - color.y;
	color.xy *= 0.00390625f;	// *= 1.0 / 256
	return color;
}

float ColorToFloat( in float3 color )
{
	const float3 byteToFloat = float3( 1.0f, (1.0f / 256.0f), 1.0f / (256.0f * 256.0f) );
	return dot( color, byteToFloat );
}

float4 F32_To_ARGB32( const float f )
{
	const float4 bitSh = float4( 256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0 );
	const float4 mask = float4( 0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0 );
	float4 res = frac( f * bitSh );
	res -= res.xxyz * mask;
	return res;
}

float ARGB32_To_F32( const float4 v )
{
	const float4 bitSh = float4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );
	return dot( v, bitSh );
}

#endif // __HELPERS_HLSL__
