#ifndef __SHADE_LIB_HLSL__
#define __SHADE_LIB_HLSL__

/*
	accepts dot product of light vector and surface normal
*/
float3 HemisphereLighting( float3 diffuseColor, float NdotL )
{
	return diffuseColor * squaref( 0.5 + 0.5 * NdotL );
}

mxSWIPED("CryEngine")
// - Blinn has good properties for plastic and some metallic surfaces. 
// - Good for general use. Very cheap.
// N - surface normal
// V - view vector (from surface towards the eye)
// L - light vector
// Exp - specular exponent
float Blinn( float3 N, float3 V, float3 L, float Exp )
{
	// compute half-vector
	float3 H = normalize( V + L );			// 4 alu
	return pow( saturate( dot(N, H) ), Exp );	// 4 alu
	// 8 ALU
}

/*
// Compute fresnel term, assumes NdotI comes clamped
half Fresnel( in half NdotI, in half bias, in half power)
{
  half facing = (1.0 - NdotI);
  return bias + (1.0-bias)*pow(facing, power);  
}
*/

float Fresnel( in float3 half, in float3 view, in float f0 )
{
	float base = 1.0 - dot(view, half);
	float exponential = pow(base, 5.0);
	return exponential + f0 * (1.0 - exponential);
}


/*
//
//	SParallelLight
//
struct SParallelLight
{
	float4	lightVector;	// normalized lightVector ( light vector = -1 * direction )
	float4	diffuseColor;
	float4	specularColor;
};

float3 DirectionalLight( in SurfaceData surface, in SParallelLight light )
{
	float3 outputColor = 0.0f;

	// diffuse
	float3 lightVec = light.lightVector.xyz;
	float NdotL = saturate(dot( surface.normal, lightVec ));

	outputColor += surface.diffuseColor.rgb * light.diffuseColor.rgb * NdotL;

	// compute surface specular term

	// Blinn-Phong
	const float3 halfVector = normalize(normalize( -surface.position ) + lightVec);

	float specularFactor = pow( abs(dot( surface.normal, halfVector )), surface.specularPower );
	float3 specularColor = (surface.specularIntensity * light.specularColor.rgb) * specularFactor;

	outputColor += specularColor;

	return outputColor;
}
*/



/*
// from Serious Engine (2011):
// calculates omni-light intensity
//
float CalcPointLightIntensity( in float3 pixelPos, in float3 lightPos, in float lightRange )
{
  // distance attenuation
  float3 vDist   = pixelPos -  lightPos;
  float  fDistSqr  = dot( vDist, vDist );
  float  fDist = rsqrt( fDistSqr );
  float  fFactor = 1- saturate( 1 / (lightRange * fDist));
  fFactor /= fDistSqr;
 return fFactor;
}
*/

// linear falloff instead of quadratic gives a softer look
//
float CalcPointLightAttenuation(
	in float3 pixelPos, in float3 N,
	in float3 lightPos, in float lightRange, in float invLightRange )
{
	float3 L = lightPos.xyz - pixelPos.xyz;
	float distance = length(L);
	L /= distance;

	float NdotL = saturate(dot(N,L));
	float attenuation = saturate( 1.0f - distance * invLightRange ) * NdotL;

	return attenuation;
}


#endif // __SHADE_LIB_HLSL__

