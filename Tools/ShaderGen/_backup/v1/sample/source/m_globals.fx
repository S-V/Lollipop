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

		// these constants are set upon application start up
		// and change only during engine resets
		cbuffer NeverChanges
		{
			float4	screenSize;	// xy - backbuffer dimensions
		};

		// per-frame constants
		cbuffer PerFrame
		{
			float1	globalTimeInSeconds;	// Time parameter, in seconds. This keeps increasing.
		};

		/*===============================================
				Samplers
		===============================================*/

		SamplerState pointSampler = SS_Bilinear;
		SamplerState linearSampler = SS_Bilinear;
		SamplerState anisotropicSampler = SS_Aniso;
	}
	Code
	{
		// shared code here
	}
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
		//	float4x4	viewMatrix;
			float4x4	viewProjectionMatrix;
		//	float4x4	inverseViewMatrix;
		//	float4x4	projectionMatrix;
		//	float4x4	inverseProjectionMatrix;
		//	float4x4	inverseViewProjectionTexToClipMatrix;	// used for restoring world-space vectors

		//	float4		tanHalfFoV;	// (tan( 0.5 * horizFOV), tan( 0.5 * vertFOV ), 0, 0 ) - used for restoring view-space position

		//	float1		inverseFarPlaneZ;
		//	float1		inverseNearPlaneZ;
		};
	}
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
		Texture2D< float4 >		RT0 = RT_Diffuse_SpecPower;
		Texture2D< float4 >		RT1 = RT_Normal_SpecIntensity;
		Texture2D< float1 >		RT2 = RT_LinearDepth;
		Texture2D< float2 >		RT3 = RT_MotionXY;
	}
}
