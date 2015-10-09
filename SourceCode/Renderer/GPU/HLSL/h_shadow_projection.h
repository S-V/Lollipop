// Contains functions for applying shadow maps.

#ifndef H_SHADOW_PROJECTION_HLSL
#define H_SHADOW_PROJECTION_HLSL

/*
=======================================================================

	Percentage Closer Soft Shadows (PCSS)

=======================================================================
*/

/*
// code taken from NVidia whitepaper
// "Integrating Realistic Soft Shadows into Your Game Engine"
// Kevin Myers 
// Randima (Randy) Fernando 
// Louis Bavoil
// [2008]

#define BLOCKER_SEARCH_NUM_SAMPLES 16 
#define PCF_NUM_SAMPLES 16 
#define NEAR_PLANE 9.5 
#define LIGHT_WORLD_SIZE .5 
#define LIGHT_FRUSTUM_WIDTH 3.75 

// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT 
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH) 

static const float2 poissonDisk[16] =
{ 
	float2( -0.94201624, -0.39906216 ), 
	float2( 0.94558609, -0.76890725 ), 
	float2( -0.094184101, -0.92938870 ), 
	float2( 0.34495938, 0.29387760 ), 
	float2( -0.91588581, 0.45771432 ), 
	float2( -0.81544232, -0.87912464 ), 
	float2( -0.38277543, 0.27676845 ), 
	float2( 0.97484398, 0.75648379 ), 
	float2( 0.44323325, -0.97511554 ), 
	float2( 0.53742981, -0.47373420 ), 
	float2( -0.26496911, -0.41893023 ), 
	float2( 0.79197514, 0.19090188 ), 
	float2( -0.24188840, 0.99706507 ), 
	float2( -0.81409955, 0.91437590 ), 
	float2( 0.19984126, 0.78641367 ), 
	float2( 0.14383161, -0.14100790 ) 
}; 

float PenumbraSize( float zReceiver, float zBlocker ) //Parallel plane estimation 
{ 
	return (zReceiver - zBlocker) / zBlocker; 
} 

void FindBlocker(out float avgBlockerDepth,  
				 out float numBlockers, 
				 float2 uv, float zReceiver ) 
{ 
	//This uses similar triangles to compute what  
	//area of the shadow map we should search 
	float searchWidth = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver; 

	float blockerSum = 0; 
	numBlockers = 0; 

	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i ) 
	{ 
		float shadowMapDepth = shadowDepthMap.SampleLevel( 
			pointClampSampler,  
			uv + poissonDisk[i] * searchWidth, 
			0
			).x;

		if ( shadowMapDepth < zReceiver ) { 
			blockerSum += shadowMapDepth; 
			numBlockers++; 
		} 
	} 

	avgBlockerDepth = blockerSum / numBlockers; 
} 

float PCF_Filter( float2 uv, float zReceiver, float filterRadiusUV ) 
{ 
	float sum = 0.0f; 
	for ( int i = 0; i < PCF_NUM_SAMPLES; ++i ) 
	{ 
		float2 offset = poissonDisk[i] * filterRadiusUV; 
		sum += shadowDepthMap.SampleCmpLevelZero(shadowMapPCFSampler, uv + offset, zReceiver); 
	} 
	return sum / PCF_NUM_SAMPLES; 
} 

// coords.xy contains the u-v coordinates to look up 
// coords.z is the depth value to compare against 
//
float PCSS ( Texture2D shadowMapTex, float4 coords  ) 
{ 
	float2 uv = coords.xy; 
	float zReceiver = coords.z; // Assumed to be eye-space z in this code 
	// STEP 1: blocker search 
	float avgBlockerDepth = 0; 
	float numBlockers = 0; 
	FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver ); 

	if( numBlockers < 1 )   
		//There are no occluders so early out (this saves filtering) 
		return 1.0f; 

	// STEP 2: penumbra size 
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);     
	float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / coords.z; 

	// STEP 3: filtering 
	return PCF_Filter( uv, zReceiver, filterRadiusUV ); 
} 

*/





static const float2 Poisson25[25] = {
{-0.978698, -0.0884121},
{-0.841121, 0.521165},
{-0.71746, -0.50322},
{-0.702933, 0.903134},
{-0.663198, 0.15482},
{-0.495102, -0.232887},
{-0.364238, -0.961791},
{-0.345866, -0.564379},
{-0.325663, 0.64037},
{-0.182714, 0.321329},
{-0.142613, -0.0227363},
{-0.0564287, -0.36729},
{-0.0185858, 0.918882},
{0.0381787, -0.728996},
{0.16599, 0.093112},
{0.253639, 0.719535},
{0.369549, -0.655019},
{0.423627, 0.429975},
{0.530747, -0.364971},
{0.566027, -0.940489},
{0.639332, 0.0284127},
{0.652089, 0.669668},
{0.773797, 0.345012},
{0.968871, 0.840449},
{0.991882, -0.657338},
};

static const float2 Poisson32[32] = {
{-0.975402, -0.0711386},
{-0.920347, -0.41142},
{-0.883908, 0.217872},
{-0.884518, 0.568041},
{-0.811945, 0.90521},
{-0.792474, -0.779962},
{-0.614856, 0.386578},
{-0.580859, -0.208777},
{-0.53795, 0.716666},
{-0.515427, 0.0899991},
{-0.454634, -0.707938},
{-0.420942, 0.991272},
{-0.261147, 0.588488},
{-0.211219, 0.114841},
{-0.146336, -0.259194},
{-0.139439, -0.888668},
{0.0116886, 0.326395},
{0.0380566, 0.625477},
{0.0625935, -0.50853},
{0.125584, 0.0469069},
{0.169469, -0.997253},
{0.320597, 0.291055},
{0.359172, -0.633717},
{0.435713, -0.250832},
{0.507797, -0.916562},
{0.545763, 0.730216},
{0.56859, 0.11655},
{0.743156, -0.505173},
{0.736442, -0.189734},
{0.843562, 0.357036},
{0.865413, 0.763726},
{0.872005, -0.927},
};

static const float2 Poisson64[64] = {
{-0.934812, 0.366741},
{-0.918943, -0.0941496},
{-0.873226, 0.62389},
{-0.8352, 0.937803},
{-0.822138, -0.281655},
{-0.812983, 0.10416},
{-0.786126, -0.767632},
{-0.739494, -0.535813},
{-0.681692, 0.284707},
{-0.61742, -0.234535},
{-0.601184, 0.562426},
{-0.607105, 0.847591},
{-0.581835, -0.00485244},
{-0.554247, -0.771111},
{-0.483383, -0.976928},
{-0.476669, -0.395672},
{-0.439802, 0.362407},
{-0.409772, -0.175695},
{-0.367534, 0.102451},
{-0.35313, 0.58153},
{-0.341594, -0.737541},
{-0.275979, 0.981567},
{-0.230811, 0.305094},
{-0.221656, 0.751152},
{-0.214393, -0.0592364},
{-0.204932, -0.483566},
{-0.183569, -0.266274},
{-0.123936, -0.754448},
{-0.0859096, 0.118625},
{-0.0610675, 0.460555},
{-0.0234687, -0.962523},
{-0.00485244, -0.373394},
{0.0213324, 0.760247},
{0.0359813, -0.0834071},
{0.0877407, -0.730766},
{0.14597, 0.281045},
{0.18186, -0.529649},
{0.188208, -0.289529},
{0.212928, 0.063509},
{0.23661, 0.566027},
{0.266579, 0.867061},
{0.320597, -0.883358},
{0.353557, 0.322733},
{0.404157, -0.651479},
{0.410443, -0.413068},
{0.413556, 0.123325},
{0.46556, -0.176183},
{0.49266, 0.55388},
{0.506333, 0.876888},
{0.535875, -0.885556},
{0.615894, 0.0703452},
{0.637135, -0.637623},
{0.677236, -0.174291},
{0.67626, 0.7116},
{0.686331, -0.389935},
{0.691031, 0.330729},
{0.715629, 0.999939},
{0.8493, -0.0485549},
{0.863582, -0.85229},
{0.890622, 0.850581},
{0.898068, 0.633778},
{0.92053, -0.355693},
{0.933348, -0.62981},
{0.95294, 0.156896},
};

//#define PRESET 2
//
//#if PRESET == 0
//
//#define USE_POISSON
//#define SEARCH_POISSON_COUNT 25
//#define SEARCH_POISSON Poisson25
//#define PCF_POISSON_COUNT 25
//#define PCF_POISSON Poisson25
//
//#elif PRESET == 1
//
//#define USE_POISSON
//#define SEARCH_POISSON_COUNT 32
//#define SEARCH_POISSON Poisson32
//#define PCF_POISSON_COUNT 64
//#define PCF_POISSON Poisson64
//
//#else
//
//#define BLOCKER_SEARCH_STEP_COUNT 3
//#define PCF_FILTER_STEP_COUNT 7
//
//#endif

#if 0

#ifndef SHADOW_EPSILON
#define SHADOW_EPSILON 0.00015f
#endif

#define BLOCKER_SEARCH_STEP_COUNT 3
#define PCF_FILTER_STEP_COUNT 3

static const float g_LightZNear = 0.1f;


static const float2 g_LightRadiusUV = float2(1.0f / 1200.0f * 960.0f * 2.0f,1.0f / 800.0f * 960.0f* 2.0f);

// Using similar triangles from the surface point to the area light
float2 SearchRegionRadiusUV(float zWorld)
{
	return g_LightRadiusUV * (zWorld - g_LightZNear) / zWorld;
}

// Using similar triangles between the area light, the blocking plane and the surface point
float2 PenumbraRadiusUV(float zReceiver, float zBlocker)
{
	return g_LightRadiusUV * (zReceiver - zBlocker) / zBlocker;
}

// Project UV size to the near plane of the light
float2 ProjectToLightUV(float2 sizeUV, float zWorld)
{
	return sizeUV * g_LightZNear / zWorld;
}

// Derivatives of light-space depth with respect to texture coordinates
float2 DepthGradient(float2 uv, float z)
{
	float2 dz_duv = 0;

	float3 duvdist_dx = ddx(float3(uv,z));
	float3 duvdist_dy = ddy(float3(uv,z));

	dz_duv.x = duvdist_dy.y * duvdist_dx.z;
	dz_duv.x -= duvdist_dx.y * duvdist_dy.z;

	dz_duv.y = duvdist_dx.x * duvdist_dy.z;
	dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

	float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
	dz_duv /= det;

	return dz_duv;
}

float BiasedZ(float z0, float2 dz_duv, float2 offset)
{
	return z0 + dot(dz_duv, offset);
	//return z0;
}

#define lightRange	(lightRadiusInvRadius.x)


float ZClipToZEye(float zClip)
{
	return lightRange*g_LightZNear / (lightRange - zClip*(lightRange-g_LightZNear));   
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void FindBlocker(out float avgBlockerDepth, 
				 out float numBlockers,
				 float2 uv,
				 float z0,
				 float2 dz_duv,
				 float2 searchRegionRadiusUV)
{
	float blockerSum = 0;
	numBlockers = 0;

	float2 stepUV = searchRegionRadiusUV / BLOCKER_SEARCH_STEP_COUNT;
	for( float x = -BLOCKER_SEARCH_STEP_COUNT; x <= BLOCKER_SEARCH_STEP_COUNT; ++x )
		for( float y = -BLOCKER_SEARCH_STEP_COUNT; y <= BLOCKER_SEARCH_STEP_COUNT; ++y )
		{
			float2 offset = float2( x, y ) * stepUV;
			float shadowMapDepth = shadowDepthMap.SampleLevel(pointSampler, uv + offset, 0).x;
			//float z = BiasedZ(z0, dz_duv, offset);
			float z = z0 - SHADOW_EPSILON;
			if ( (shadowMapDepth) < z )
			{
				blockerSum += shadowMapDepth;
				numBlockers++;
			}
		}
		avgBlockerDepth = blockerSum / numBlockers;
}

// Performs PCF filtering on the shadow map using multiple taps in the filter region.
float PCF_Filter( float2 uv, float z0, float2 dz_duv, float2 filterRadiusUV )
{
	float sum = 0;

	float2 stepUV = filterRadiusUV / PCF_FILTER_STEP_COUNT;
	for( float x = -PCF_FILTER_STEP_COUNT; x <= PCF_FILTER_STEP_COUNT; ++x )
		for( float y = -PCF_FILTER_STEP_COUNT; y <= PCF_FILTER_STEP_COUNT; ++y )
		{
			float2 offset = float2( x, y ) * stepUV;
			// float z = BiasedZ(z0, dz_duv, offset);
			float z = z0 - SHADOW_EPSILON;
			sum += shadowDepthMap.SampleCmpLevelZero(shadowMapPCFSampler, uv + offset, z).x;
			//sum += shadowDepthMap.SampleLevel(pointSampler, uv + offset,  0).x < z ? 0.0f : 1.0f;
		}
		float numSamples = (PCF_FILTER_STEP_COUNT*2+1);
		return sum / (numSamples*numSamples);
}

float PCSS_Shadow(float2 uv, float z, float2 dz_duv, float zEye)
{
	// ------------------------
	// STEP 1: blocker search
	// ------------------------
	float avgBlockerDepth = 0;
	float numBlockers = 0;
	float2 searchRegionRadiusUV = SearchRegionRadiusUV(zEye);
	FindBlocker( avgBlockerDepth, numBlockers, uv, z, dz_duv, searchRegionRadiusUV );


	// Early out if no blocker found
	if (numBlockers == 0) return 1.0;

	// ------------------------
	// STEP 2: penumbra size
	// ------------------------
	float avgBlockerDepthWorld = ZClipToZEye(avgBlockerDepth);

	float2 penumbraRadiusUV = PenumbraRadiusUV(zEye, avgBlockerDepthWorld);
	float2 filterRadiusUV = ProjectToLightUV(penumbraRadiusUV / 4.0f, zEye);

	// ------------------------
	// STEP 3: filtering
	// ------------------------
	return PCF_Filter(uv, z, dz_duv, filterRadiusUV);
}
#endif


#if 0
	// Transform from view space to light projection space.
	float4 lightSpacePosH = mul( float4(positionVS, 1.0f), eyeToLightProjection );
	lightSpacePosH.xyz /= lightSpacePosH.w;

	float3 projTexCoords;
    projTexCoords[0] = (lightSpacePosH.x / lightSpacePosH.w) * 0.5f + 0.5f;
    projTexCoords[1] = (-lightSpacePosH.y / lightSpacePosH.w) * 0.5f + 0.5f;
    projTexCoords[2] = lightSpacePosH.z / lightSpacePosH.w;

	float2 uv = projTexCoords.xy;
    float z = projTexCoords.z;	// normalized depth from the point of view of the light

// Compute gradient using ddx/ddy before any branching
    float2 dz_duv = DepthGradient(uv, z);

    // Eye-space z from the light's point of view
    float zEye = mul(float4(position, 1.0f), lightView).z;


	float shadow = PCF_Filter( uv, z, uv, float2(1.0f / 512.0f * 1.0f,  1.0f / 512.0f * 1.0f) );

	//float4 coords = float4( projTexCoords.xy, depth, 1.0 );
	//outputColor = (float3) PCSS( shadowDepthMap, coords );

	outputColor = shadow;

#endif // bSpotLight_ProjectsTexture || bSpotLight_CastShadows



#endif // !H_SHADOW_PROJECTION_HLSL



