//-------------------------------------------------------------------
//	Normal vectors encoding/decoding
//-------------------------------------------------------------------

// Compresses a normalized vector.
// [-1..1] -> [0..1]
float3 CompressNormal( float3 v )
{
	return v * 0.5f + 0.5f;
}

// Expands a range-compressed vector (used with normal maps).
// [0..1] -> [-1..1]
float3 ExpandNormal( float3 v )
{
	return v * 2.0f - 1.0f;
}


/**
 * Encodes normal into spherical coordinates.
 * Can be used for normal vectors in any space (not only view space).
 * Quite ALU-heavy.
 */
float2 ToSpherical( float3 N )
{
	return float2( atan2(N.y, N.x) * M_INV_PI, N.z ) * 0.5f + 0.5f;
}

/**
 * Decodes normal from spherical coordinates.
 * Quite ALU-heavy.
 */
float3 FromSpherical( float2 spherical )
{
	spherical = spherical * 2.0f - 1.0f;

	float2 theta;
	sincos( spherical.x * M_PI, theta.y, theta.x );

	float len = sqrt( 1.0f - spherical.y * spherical.y );

	return float3( theta.xy * len, spherical.y );
}



// Converts a normalized cartesian direction vector
// to spherical coordinates.
float2 CartesianToSpherical(float3 cartesian)
{
	float2 spherical;

	spherical.x = atan2(cartesian.y, cartesian.x) * M_INV_PI;
	spherical.y = cartesian.z;

	return spherical * 0.5f + 0.5f;
}

// Converts a spherical coordinate to a normalized
// cartesian direction vector.
float3 SphericalToCartesian(float2 spherical)
{
	float2 sinCosTheta, sinCosPhi;

	spherical = spherical * 2.0f - 1.0f;
	sincos(spherical.x * M_PI, sinCosTheta.x, sinCosTheta.y);
	sinCosPhi = float2(sqrt(1.0 - spherical.y * spherical.y), spherical.y);

	return float3(sinCosTheta.y * sinCosPhi.x, sinCosTheta.x * sinCosPhi.x, sinCosPhi.y);    
}


/**
 * Constants for Stereographic Projection method.
 */
#define ST_PROJ_SCALE		1.7777f
#define ST_PROJ_INV_SCALE	0.562525f
#define ST_PROJ_TWO_SCALE	3.5554f

float2 EncodeNormal_SP( float3 N )
{
	float2 Nenc;	// encoded normal
	Nenc = N.xy / (N.z + 1.0f);
	Nenc *= ST_PROJ_INV_SCALE;
	Nenc = Nenc * 0.5f + 0.5f;
	return Nenc;
}
float3 DecodeNormal_SP( float2 Nenc )
{
	float3 N;
	float3 nn;
	nn.xy = Nenc * ST_PROJ_TWO_SCALE - ST_PROJ_SCALE;
	nn.z = 1.0f;
	float g = 2.0f / dot(nn.xyz,nn.xyz);
	N.xy = g * nn.xy;
	N.z = g - 1.0f;
	return N;
}



float2 PackNormal( float3 N )
{
	float2 Nenc;	// encoded normal
/*
	These have been successfully used to pack *view-space* normal (into R16_G16_FLOAT):
*/
/* * /
	// From S.T.A.L.K.E.R : Clear Sky:
	Nenc = N.xy * 0.5f + 0.5f;
	Nenc.x *= (N.z < 0.0f) ? -1.0f : 1.0f;
/* */
/* * /
	// From CryEngine 3:	(problems with lookDirWS(0,-1,0))
	Nenc = normalize(N.xy) * sqrt( (-N.z * 0.5f) + 0.5f );
	Nenc = Nenc * 0.5f + 0.5f;
/* */
/* * /
	// Spheremap Transform:
	// (works great, but had issues with env. mapping)
	float f = N.z*2+1;
	float g = dot(N,N);
	float p = sqrt(g+f);
	Nenc = N.xy/p * 0.5 + 0.5;
/* */
/* * /
	// Lambert Azimuthal Equal-Area Projection:
	// (works great, but had issues with env. mapping and world-space vectors (1;0;0))
	float f = sqrt(8*N.z+8);
	Nenc = N.xy / f + 0.5;
/* */
/*
*/	// Stereographic Projection method:
	// (works great for me)
/* * /
	Nenc = EncodeNormal_SP(N);
/* */
/* */
	// Spherical Coordinates:
	Nenc = ToSpherical( N );//works with R11G11B10_FLOAT
/* */
	return Nenc;
}

float3 UnpackNormal( in float2 Nenc )
{
	float3 N;
/*
	These have been successfully used to unpack *view-space* normal (from R16_G16_FLOAT):
*/
/* * /
	// From S.T.A.L.K.E.R : Clear Sky:
	N.xy = 2.0f * abs(Nenc.xy) - 1.0f;
	N.z = ((Nenc.x < 0.0f) ? -1.0f : 1.0f)
		* sqrt(abs( 1.0f - dot(N.xy, N.xy) ));
/* */
/* * /
	// From CryEngine 3:	(problems with lookDirWS(0,-1,0))
	float2 fenc = Nenc.xy * 2.0f - 1.0f;
	N.z = -( dot(fenc,fenc) * 2.0f - 1.0f );
	N.xy = normalize(fenc) * sqrt(1.0f - N.z * N.z );
/* */
/* * /
	// Spheremap Transform:
	// (works great, but had issues with env. mapping)
	float2 tmp = -Nenc*Nenc+Nenc;
	float f = tmp.x+tmp.y;
	float m = sqrt(4*f-1);
	N.xy = (Nenc*4-2) * m;
	N.z = 8*f-3;
/* */
/* * /
	// Lambert Azimuthal Equal-Area Projection:
	// (works great, but had issues with env. mapping)
	float2 fenc = Nenc*4-2;
	float f = dot(fenc,fenc);
	float g = sqrt(1-f/4);
	N.xy = fenc*g;
	N.z = 1-f/2;
/* */
/*
*/	// Stereographic Projection method:
	// (works great for me)(but had small issues with lookDirWS(0,-1,0))
/* * /
	N = DecodeNormal_SP( Nenc );
/* */
/* */
	// Spherical Coordinates:
	N = FromSpherical( Nenc );//works with R11G11B10_FLOAT
/* */
	return N;
}
