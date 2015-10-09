/*
=============================================================================
	Desc:	Shader constant registers that are reserved by the engine.
=============================================================================
*/

/*
-----------------------------------------------------------------------------
	Globals
-----------------------------------------------------------------------------
*/
Shared Globals
{
Variables
{
/*===============================================
		Global shader constants.
===============================================*/
/*
// these constants are set upon application start up
// and change only during engine resets
cbuffer NeverChanges
{
	float4	screenSize;	// xy - backbuffer dimensions
};
*/
// per-frame constants
cbuffer PerFrame
{
	float4	screenSize_invSize;	// xy - backbuffer dimensions, zw - inverse viewport size
	float1	globalTimeInSeconds;	// Time parameter, in seconds. This keeps increasing.
};

/*===============================================
		Samplers
===============================================*/

SamplerState pointSampler = SS_Bilinear;
SamplerState linearSampler = SS_Bilinear;
SamplerState anisotropicSampler = SS_Aniso;

SamplerState pointClampSampler = SS_PointClamp;
SamplerState linearClampSampler = SS_LinearClamp;

SamplerState colorMapSampler = SS_Bilinear;
SamplerState detailMapSampler = SS_Bilinear;
SamplerState normalMapSampler = SS_Bilinear;
SamplerState specularMapSampler = SS_Bilinear;
SamplerState attenuationSampler = SS_Bilinear;
SamplerState cubeMapSampler = SS_Bilinear;

SamplerComparisonState	shadowMapSampler = SS_ShadowMap;
SamplerComparisonState	shadowMapPCFSampler = SS_ShadowMapPCF;
SamplerComparisonState	shadowMapPCFBilinearSampler = SS_ShadowMapPCF_Bilinear;

}//Inputs
//---------------------------------------------------------------------------
Code
{
#include "h_base.h"

// returns render target size (backbuffer dimensions)
float2 GetViewportSize() { return screenSize_invSize.xy; }
// returns texel size
float2 GetInvViewportSize() { return screenSize_invSize.zw; }
}//Code
}

/*
-----------------------------------------------------------------------------
	View
-----------------------------------------------------------------------------
*/
Shared View
{
Variables
{
// per-view constants
cbuffer PerView
{
	float4x4	viewMatrix;
	float4x4	viewProjectionMatrix;
	float4x4	inverseViewMatrix;
	float4x4	projectionMatrix;
	float4x4	inverseProjectionMatrix;
//	float4x4	inverseViewProjectionTexToClipMatrix;	// used for restoring world-space vectors

	// x = tan( 0.5 * horizFOV), y = tan( 0.5 * vertFOV ), z = 1, w = 1
	float4		tanHalfFoV;	// used for restoring view-space position

	// x = nearZ, y = farZ, z = invNearZ, w = invFarZ
	float4		depthClipPlanes;

	// positions of frustum corners in view-space
	float4		frustumCornerVS_FarTopLeft;
	float4		frustumCornerVS_FarTopRight;
	float4		frustumCornerVS_FarBottomLeft;
	float4		frustumCornerVS_FarBottomRight;
};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#define PER_VIEW_CONSTANTS_DEFINED
#include "h_base.h"

float3 GetCameraRightVector() { return inverseViewMatrix[0].rgb; }
float3 GetCameraUpVector() { return inverseViewMatrix[1].rgb; }
float3 GetLookDirection() { return inverseViewMatrix[2].rgb; }
// returns camera position in world space
float3 GetEyePositionWS() { return inverseViewMatrix[3].rgb; }

float GetNearZ() { return depthClipPlanes.x; }
float GetFarZ() { return depthClipPlanes.y; }
float GetInverseNearZ() { return depthClipPlanes.z; }
float GetInverseFarZ() { return depthClipPlanes.w; }
}//Code
}

/*
-----------------------------------------------------------------------------
	Object
-----------------------------------------------------------------------------
*/
Shared Object
{
Variables
{
// per-object constants
cbuffer PerObject
{
	float4x4 worldMatrix;	// object-to-world transformation matrix
	float4x4 worldViewMatrix;
	float4x4 worldViewProjectionMatrix;

	//float4x4	worldIT;		// transpose of the inverse of the world matrix
	//float4x4	worldViewIT;	// transpose of the inverse of the world-view matrix
};
}//Inputs
//---------------------------------------------------------------------------
Code
{
#define PER_OBJECT_CONSTANTS_DEFINED
#include "h_transform.h"
}//Code
}





/*
-----------------------------------------------------------------------------
	GBuffer
-----------------------------------------------------------------------------
*/
Shared GBuffer
{
Variables
{
	//-----------------------------------------------
	//	G-buffer
	//-----------------------------------------------

	// normal in world-space, spec intensity - 0..1
	Texture2D< float4 >		RT_normal_specMul = RT_Normal_SpecIntensity;
	// albedo, spec power normalized to 0..1
	Texture2D< float4 >		RT_diffuse_specExp = RT_Diffuse_SpecPower;
	// depth - normalized to nearZ/farZ..1
	Texture2D< float1 >		RT_linearDepth = RT_LinearDepth;
	//Texture2D< float2 >		motionXY = RT_MotionXY;
}//Inputs
//---------------------------------------------------------------------------
Code
{
#define RX_GBUFFER_READS_ENABLED
#include "h_geometry_buffer.h"
}//Code
}




/*
-----------------------------------------------------------------------------
	LocalLightData
	used for deferred lighting
-----------------------------------------------------------------------------
*/
Shared LocalLightData
{
Variables
{
cbuffer PerLocalLight
{
	// point light data
	float4		lightDiffuseColor;
	float4		lightPosition;			// in view space
	float4		lightRadiusInvRadius;	// x - light radius, y - inverse light range
	float4		lightSpecularColor;	

	// spot light data
	float4		spotLightDirection;		// normalized axis direction in view space
	// x = cosTheta (cosine of half inner cone angle), y = cosPhi (cosine of half outer cone angle)
	float4		spotLightAngles;

	float4x4	lightShapeTransform;	// for transforming light volume meshes

	float4x4	eyeToLightProjection;	// for projective texture mapping

	float		lightProjectorIntensity;
	// nudge a bit to reduce self-shadowing (we'll get shadow acne on all surfaces without this bias)
	float		lightShadowDepthBias;
};
}//Inputs
//---------------------------------------------------------------------------
Code
{

}//Code
}


