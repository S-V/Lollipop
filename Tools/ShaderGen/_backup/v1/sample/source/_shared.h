/*
=============================================================================
	File:	shared.h
	Desc:	This is a shared header file
			included by both host application (C++) code and shader code (HLSL).
=============================================================================
*/

#ifdef __cplusplus

#	define row_major
#	define cbuffer	struct
#	define DECLARE_CB( cbName, slot )	enum { cbName##_Index = slot }; __declspec(align(16)) struct cbName

#	define CPP_CODE( code )		code
#	define HLSL_CODE( code )

#else // if HLSL

#	define DECLARE_CB( cbName, slot )	cbuffer cbName : register( b##slot )

#	define CPP_CODE( code )
#	define HLSL_CODE( code )		code

#endif

#define PACK( byteOffset )			HLSL_CODE( : packoffset( byteOffset ) )

#ifndef __cplusplus
#	define CB_USES_SLOT( cbName, slot )	cbuffer cbName : register( b##slot )
#endif

/*===============================================
		Global shader constants.
===============================================*/

// index of the first constant buffer slot (reserved for internal use)
#define CB_StartSlot	1

// these constants are set upon application start up and change only during engine resets
DECLARE_CB( cbNeverChanges, 1 )
{
//	float screenSizeX	: packoffset( c0.x );
//	float screenSizeY	: packoffset( c0.y );
	float4	g_vScreenSize;	// backbuffer dimensions
};

// per-frame constants
MX_OPTIMIZE("the constants 'g_fTanHalfFoVx' and 'g_fTanHalfFoVy' could be #defined to improve perf");

DECLARE_CB( cbPerFrame, 2 )
{
	float4	ambientLightColor;
	float1	globalTimeInSeconds;	// Time parameter, in seconds. This keeps increasing.
};

// per-view constants
DECLARE_CB( cbPerView, 3 )
{
	float4x4	viewMatrix;
	float4x4	viewProjectionMatrix;
	float4x4	inverseViewMatrix;
	float4x4	projectionMatrix;
	float4x4	inverseProjectionMatrix;
	float4x4	inverseViewProjectionTexToClipMatrix;	// used for restoring world-space vectors

	float4		tanHalfFoV;	// (tan( 0.5 * horizFOV), tan( 0.5 * vertFOV ), 0, 0 ) - used for restoring view-space position

	float1		inverseFarPlaneZ;
	float1		inverseNearPlaneZ;
};

HLSL_CODE(
	// world-space camera position
	float3 EyePosition() { return inverseViewMatrix[3].xyz; }
	// normalized look direction vector	
	float3 CameraLook() { return inverseViewMatrix[2].xyz; }
	float3 CameraRight() { return inverseViewMatrix[0].xyz; }
	float3 CameraUp() { return inverseViewMatrix[1].xyz; }
)

// frequently updated constants

DECLARE_CB( cbPerObject, 4 )
{
	float4x4 worldMatrix;			// object-to-world 
	float4x4 worldViewMatrix;
	float4x4 worldViewProjectionMatrix;
};

HLSL_CODE(
	// world-space camera position
	float3 ObjectPosition() { return worldMatrix[3].xyz; }
)





// max. number of bones per vertex for GPU skinning
#define MAX_BONES_PER_VERTEX	4

#define MAX_BONES_PER_MODEL		128

DECLARE_CB( cbPerSkinned, 5 )
{
	float4x4	bones[ MAX_BONES_PER_MODEL ];
};


// define if transformation matrices should only contain uniform scaling
#define RX_ALLOW_ONLY_UNIFORM_SCALING




/*
-----------------------------------------------------------------------------
	Deferred pass
-----------------------------------------------------------------------------
*/

DECLARE_CB( cbPerMaterial, 6 )
{
	float4		materialDiffuseColor;
	float4		materialSpecularColor;
	float4		materialEmissiveColor;

	// material data for deferred pass
	float1		materialSpecularIntensity;
	float1		materialSpecularPower;	// normalized to range [0..1]
};



/*
-----------------------------------------------------------------------------
	Forward pass
-----------------------------------------------------------------------------
*/

#define MAX_LOCAL_LIGHTS	4

struct LightData
{
	float4		position;	// xyz - light origin, y - inverse light range
	float4		direction;
	float4		ambientColor;
	float4		diffuseColor;
	float4		specularColor;
};

DECLARE_CB( cbForwardPass, 7 )
{
//	float4		materialEmissiveColor;
//	LightData	lightData[ MAX_LOCAL_LIGHTS ];
};

// number of constant buffers reserved for special use by the engine
#define CB_Count		8

#define CB_FirstUserSlot	8
#define CB_LastUserSlot		14
#define CB_NumUserSlots		(CB_LastUserSlot - CB_FirstUserSlot)



/*===============================================
		Samplers
===============================================*/

#ifndef __cplusplus
#	define SAMPLER_USES_SLOT( samplerName, slot )	SamplerState samplerName : register( s##slot )
#else
#	define SAMPLER_USES_SLOT( samplerName, slot )	enum { samplerName##_Index = slot };
#endif

SAMPLER_USES_SLOT( pointSampler, 0 );
SAMPLER_USES_SLOT( linearSampler, 1 );
SAMPLER_USES_SLOT( anisotropicSampler, 2 );

SAMPLER_USES_SLOT( colorMapSampler, 3 );	// for sampling diffuse texture
SAMPLER_USES_SLOT( detailMapSampler, 4 );
SAMPLER_USES_SLOT( normalMapSampler, 5 );
SAMPLER_USES_SLOT( specularMapSampler, 6 );
SAMPLER_USES_SLOT( attenuationSampler, 7 );
SAMPLER_USES_SLOT( cubeMapSampler, 8 );

SAMPLER_USES_SLOT( pointClampSampler, 9 );
SAMPLER_USES_SLOT( linearClampSampler, 10 );
SAMPLER_USES_SLOT( shadowMapSampler, 11 );

// used for hardware accelerated shadow map filtering (2x2 PCF)
#ifndef __cplusplus
SamplerComparisonState	shadowMapHWFilter : register(s12);
#endif
#define shadowMapHWFilter_Index 12

#define RX_NUM_SAMPLERS	13

//---------------------------------------------------------------
//		Shader resources.
//---------------------------------------------------------------

#define NUM_GBUFFER_RENDER_TARGETS	3

#define GBUFFER_RENDER_TARGET0	0
#define GBUFFER_RENDER_TARGET1	1
#define GBUFFER_RENDER_TARGET2	2
#define GBUFFER_RENDER_TARGET3	3

#define SHADOW_MAP_INDEX	4

// start slot for setting such shader resources as diffuse/normal maps, etc
#define MATERIAL_TEXTURE_INDEX	5

// rgb - albedo, a - [optional] alpha
#define TL_DIFFUSE			0
// rgb - tangent-space normal, a - [optional] height value
#define TL_NORMALS			1
// rgb - specular color, a - specular exponent
#define TL_SPECULAR			2

// rgb - cubemap for environment mapping
#define TL_ENVMAP			3

#define TL_REFRACTION		4

// detail map / terrain blend map
#define TL_DETAIL			5

#define TL_SHADOW_MAP		6

#define TL_USER_TEXTURE0	9
#define TL_USER_TEXTURE1	10
#define TL_USER_TEXTURE2	11
#define TL_USER_TEXTURE3	12
#define TL_USER_TEXTURE4	13
#define TL_USER_TEXTURE5	14
#define TL_USER_TEXTURE6	15

#define MAX_TEXTURE_LAYERS	16



#ifndef __cplusplus

	Texture2D	shadowTexture : register(t4); //SHADOW_MAP_INDEX

	Texture2D< float4 >	materialTextures[ MAX_TEXTURE_LAYERS ] : register(t5); //MATERIAL_TEXTURE_INDEX

	#define	DiffuseMap	materialTextures[ TL_DIFFUSE ]
	#define	NormalMap	materialTextures[ TL_NORMALS ]


	float4 SampleDiffuseMap( const float2 texCoords )
	{
		return DiffuseMap.Sample( colorMapSampler, texCoords );
	}
	float4 SampleNormalMap( const float2 texCoords )
	{
		return NormalMap.Sample( normalMapSampler, texCoords );
	}
	float4 SampleDetailMap( const float2 texCoords )
	{
		return materialTextures[ TL_DETAIL ].Sample( detailMapSampler, texCoords );
	}
	float4 SampleSpecularMap( const float2 texCoords )
	{
		return materialTextures[ TL_SPECULAR ].Sample( specularMapSampler, texCoords );
	}
#endif // if HLSL

//---------------------------------------------------------------
//		Common stuff for rendering light sources.
//---------------------------------------------------------------

#define cbPerLight_Index	5

#define LIGHT_TEXTURE_INDEX	21

#define TL_LIGHT_PROJECTOR	0
#define TL_LIGHT_SHADOW_MAP	1
#define LIGHT_TEXTURE_COUNT	2


#ifndef __cplusplus

	Texture2D< float4 >	lightTextures[ LIGHT_TEXTURE_COUNT ] : register(t21);//LIGHT_TEXTURE_INDEX

	#define	ProjectiveTexture	lightTextures[ TL_LIGHT_PROJECTOR ]
	#define	ShadowDepthTexture	lightTextures[ TL_LIGHT_SHADOW_MAP ]

#endif // if HLSL


// 1 - use hardware accelerated bilinear 2x2 percentage closer shadow texture filtering for cheap shadow mapping
#define MX_HARDWARE_PCF		(1)


//
//	cbParallelLight
//
struct cbParallelLight
{
	float4	lightVector;	// normalized lightVector in view space ( light vector = -1 * direction )
	float4	diffuseColor;
	float4	specularColor;

	// transforms to light projection space; used for shadow mapping / projective texturing
	float4x4	eyeToLightProj;	// premultiplied with inverse view matrix, i.e. (invView * lightView * lightProj)

	float4x4	lightShapeTransform;	// for drawing light shape
//	float		maxZ;
//	float		invMaxZ;
	float		shadowFadeDistance;
	float		invShadowFadeDistance;
};

//
//	cbPointLight
//
struct cbPointLight
{
	float3		diffuseColor;
	float1		invRadius;		// inverse of range of influence
	float3		origin;			// in view space
	float1		radius;
	float3		specularColor;
	float1		_pad0;
	float4x4	lightShapeTransform;	// for transforming light volume (unit light shape)
	// 76 bytes
};

//
//	cbSpotLight
//
struct cbSpotLight
{
	float3		diffuseColor;
	float1		invRange;		// inverse of range of influence
	float3		origin;			// cone apex in view space
	float1		cosTheta;		// cosine of half inner cone angle
	
	float3		direction;		// normalized axis direction in view space
	float1		cosPhi;			// cosine of half outer cone angle
	float3		specularColor;
	float1		range;

	float4x4	lightShapeTransform;	// for transforming light volume (unit light shape)

	// transforms to light projection space; needed for projective texturing and shadows
	float4x4	eyeToLightProj;	// premultiplied with inverse view matrix, i.e. (invView * lightView * lightProj)

	float		projectorIntensity;
	//float		shadowDepthBias;
};








// Constant buffer for ugly hacks.
DECLARE_CB( cbAppData, 8 )//CB_FirstUserSlot
{
	cbParallelLight		globalLight;
	cbSpotLight			flashLight;
};




// Preprocessor clean-up.

#ifdef __cplusplus
//#	undef row_major
//#	undef cbuffer
//#	undef DECLARE_CB
#endif









#ifndef __cplusplus

/*===============================================
		Vertex types
===============================================*/

//appToVS
struct vtx_static
{
	float3 xyz : Position;
	uint4 tangent : Tangent;
	uint4 normal : Normal;
	float2 uv : TexCoord;
};

float3 UnpackVertexNormal( in float3 packedNormal )
{
	return packedNormal / 127.5 - 1;
}

#endif // !__cplusplus

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
