
/*
//-----------------------------------------------
//	G-buffer
//-----------------------------------------------

Texture2D< float4 >		RT0 = RT_Diffuse_SpecPower;
Texture2D< float4 >		RT1 = RT_Normal_SpecIntensity;
Texture2D< float1 >		RT2 = RT_LinearDepth;
Texture2D< float2 >		RT3 = RT_MotionXY;
*/

//
//	PS_Out_GBuffer
//
struct PS_Out_GBuffer
{
	float4	rt0  : SV_Target0;
	float4	rt1  : SV_Target1;
	float1	rt2  : SV_Target2;
};


//-------------------------------------------------------------------

#if 0
/**
 * Samples view-space pixel depth.
 */
float ReadLinearDepth( in float2 texCoord )
{
	return g_RT2.SampleLevel( pointSampler, texCoord, 0 ).r;
}

/**
 * Restores view-space pixel position from linear view-space depth.
 * Accepts clip-space pixel position.
 */
float3 RestorePositionVS( in float depthVS, in float2 screenXY )
{
	float3 posVS;
	posVS.x = screenXY.x * (depthVS * tanHalfFoV.x);
	posVS.y = screenXY.y * (depthVS * tanHalfFoV.y);
	posVS.z = depthVS;
	return posVS;
}

/**
 * Returns view-space pixel position.
 * Accepts clip-space pixel position and texture coordinates.
 */
float3 ReadPosition_VS( in float2 screenXY, in float2 texCoord )
{
	float depthVS = ReadLinearDepth( texCoord ); // sampled linear view-space depth
	return RestorePositionVS( depthVS, screenXY );
}

/**
 * Returns view-space pixel position.
 */
float3 GetPosition_VS( in float2 texCoord )
{
	float2 screenXY = TexCoordsToClipPos( texCoord );
	return ReadPosition_VS( screenXY, texCoord );
}

/**
 * Restores world-space pixel position.
 * /
 Look at inverseViewProjectionTexToClipMatrix
float3 RestorePosition_WS( in float2 texCoord )
{
	float3 posVS = GetPosition_VS( texCoord );
	return mul( float4(posVS, 1.0f), g_mInvViewMatrix ).xyz;
}*/

/**
 * Samples diffuse reflectance.
 */
float3 ReadDiffuse( in float2 texCoord )
{
	float3 diffuseColor = g_RT0.SampleLevel( pointSampler, texCoord, 0 ).rgb;
	return diffuseColor;
}

//
//	SurfaceData
//
struct SurfaceData
{
	float3	position;	// view-space position
	float3	normal;		// view-space normal
	float	specularIntensity;
	float3	diffuseColor;
	float	specularPower;	// normalized, in range [0..1]
};

void PackSurfaceData( in SurfaceData s, inout PS_Out_GBuffer o )
{
	// write albedo
	o.rt0.rgb = s.diffuseColor;

	// write specular exponent
	o.rt0.a = s.specularPower;	// power should already have been normalized to range [0..1]
	
	// write normal
	o.rt1.rg = PackNormal( s.normal );

	// write specular brightness
	o.rt1.b = s.specularIntensity;
	
	// write depth
	o.rt2.r = s.position.z;
}

void ReadSurfaceData( in float2 texCoord, out SurfaceData s )
{
	float4 G0 = g_RT0.SampleLevel( pointSampler, texCoord, 0 );
	float3 G1 = g_RT1.SampleLevel( pointSampler, texCoord, 0 );
	float1 G2 = g_RT2.SampleLevel( pointSampler, texCoord, 0 );

	float2 screenXY = TexCoordsToClipPos( texCoord );
	s.position = RestorePositionVS( G2.r, screenXY );
	s.normal = UnpackNormal( G1.rg );
	s.specularIntensity = G1.b;
	s.diffuseColor = G0.rgb;
	s.specularPower = G0.a * 254.0f + 1.0f;	// [0..1] -> [1..255]
}

#endif
