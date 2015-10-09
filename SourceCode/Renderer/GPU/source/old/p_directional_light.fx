/*
-----------------------------------------------------------------------------
*/
Shader p_directional_light
//[
//	CAUSES_SHADOWS : 1 = 0
//]
{
Inputs : Globals View
{
	cbuffer LightData
	{
		float4 m_lightVector;
		float4 m_diffuseColor;
		float4 m_backColor;
		float4 m_ambientColor;
		//float m_backLightOffset;
	};
}
Code
{
#include "h_base.h"
#include "h_geometry_buffer.h"
#include "h_screen_shader.h"
#include "h_shadelib.hlsl"

float4 PS_Main( VS_ScreenOutput input ) : SV_Target
{
	float3 outputColor = 0.0f;
/*
	SurfaceData  s;
	ReadSurfaceData( input.texCoord, s );
m_lightVector
	outputColor = DirectionalLight( s, lightData );
*/


#if 0 //CAUSES_SHADOWS
	float3 posVS = s.position;

	//float distToEye = length( posVS );

	//if( distToEye < lightData.shadowFadeDistance )
	if( posVS.z < lightData.shadowFadeDistance )
	{
		// Transform from view space to light projection space.
		float4 lightSpacePosH = mul( float4(posVS, 1.0f), lightData.eyeToLightProj );
		lightSpacePosH.xyz /= lightSpacePosH.w;

	//	if( lightSpacePosH.x < -1.0f || lightSpacePosH.x > 1.0f || lightSpacePosH.y < -1.0f || lightSpacePosH.y > 1.0f )
	//		return float4( outputColor, 1.0f );

		// texture coordinates for sampling shadow map/projective texture
		float2 projTexCoords = ClipPosToTexCoords( lightSpacePosH.xy );

	//	projTexCoords.x = saturate(projTexCoords.x);
	//	projTexCoords.y = saturate(projTexCoords.y);

		static const float shadowDepthBias = -0.003f;

		float depth = lightSpacePosH.z;	// normalized depth from the point of view of the light

		float comparisonValue = depth + shadowDepthBias;
		float shadowFactor = ShadowDepthTexture.SampleCmpLevelZero( shadowMapHWFilter, projTexCoords, comparisonValue );
		// zero shadow factor means 'the pixel is completely in shadow'

		//shadowFactor *= saturate( shadowFactor * (lightData.shadowFadeDistance - distToEye) );
		//shadowFactor *= saturate( shadowFactor ) * saturate( lightData.shadowFadeDistance - distToEye );
		//shadowFactor *= ((shadowFactor < 0.9f) ? 1.0f : 0.0f) * saturate( lightData.shadowFadeDistance - distToEye );
/** /		if( shadowFactor < 0.9f )
		{
			shadowFactor *= saturate( distToEye - lightData.shadowFadeDistance );
			//shadowFactor *= 1.0f - saturate( distToEye * lightData.invShadowFadeDistance );
		}
*/

		outputColor *= shadowFactor;
	}

#endif//CAUSES_SHADOWS

	return float4( outputColor, 1.0f );
}

}//Code

}={
	VertexShader	FullScreenTriangle_VS
	PixelShader		PS_Main
}
