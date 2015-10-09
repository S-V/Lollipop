#pragma once

#include <Core/Editor/EditableProperties.h>

#include <Renderer/Core/Texture.h>

class rxSceneContext;

/*
=======================================================================

	Dynamic lights

=======================================================================
*/

//
//	ELightType - enumerates all allowed light types.
//
enum ELightType
{
	//<= Global lights start here
	Light_Directional = 0,	// e.g.: sun light

	//<= Local lights start here
	//Light_Local,
	Light_Point,	// e.g.: light from explosion
	Light_Spot,		// e.g.: flash light

	//LT_Line,		// e.g.: laser beams

	//<= User-defined light types start here
	//Light_UserType0,


	NumLightTypes,

	LIGHT_TYPE_BITS = 3
};
mxDECLARE_ENUM( ELightType, U4, LightType );

enum ELightFlags
{
	LF_Enabled	= BIT(0),
	LF_Shadows	= BIT(1),	// Does the light cast dynamic shadows? (Set if the light causes the affected geometry to cast shadows.)
	LF_All		= -1,
};
mxDECLARE_FLAGS( ELightFlags, U4, LightFlags );

/*
-----------------------------------------------------------------------------
	rxLight

	base class for all types representing light sources
-----------------------------------------------------------------------------
*/
struct rxLight : public SBaseType
{
	// empty, must be a POD type!

	mxDECLARE_CLASS(rxLight,SBaseType);
};

/*
-----------------------------------------------------------------------------
	rxGlobalLight
-----------------------------------------------------------------------------
*/
struct rxGlobalLight : public rxLight
{


public:
	mxDECLARE_CLASS(rxGlobalLight,rxLight);

	rxGlobalLight();


};


//
//	rxParallelLight - represents a directional (infinite) light source.
//
mxALIGN_16(struct rxParallelLight) : public rxGlobalLight
{
	float4		m_lightDirWS;	// normalized light direction in world space
	float4		m_diffuseColor;
	float4		m_specularColor;
	float4		m_backColor;
	float4		m_ambientColor;
	F4			m_backLightOffset;

	LightFlags	m_flags;

	//U4		m_numCascades;
	F4		m_shadowDepthBias;
	F4		m_shadowFadeDistance;

public:
	mxDECLARE_CLASS(rxParallelLight,rxGlobalLight);
	mxDECLARE_REFLECTION;

	rxParallelLight();

	void SetDiffuseColor( const ColorRGB& newDiffuseColor );
	const ColorRGB& GetDiffuseColor() const;

	void SetSpecularColor( const ColorRGB& newSpecularColor );
	const ColorRGB& GetSpecularColor() const;

	// Get/Set direction that the light is pointing in world space.
	void SetDirection( const Vec3D& newDirection );

	// Returns light direction in world space.
	Vec3D GetDirection() const;

	bool DoesCastShadows() const;
	void SetCastShadows( bool bShadows );

	// apply batched changes
	//void ApplyChanges();

	mxIMPLEMENT_COMMON_PROPERTIES(rxParallelLight);
};

/*
-----------------------------------------------------------------------------
	rxLocalLight

	represents a localized dynamic light source in a scene;

	this structure should be as small as possible
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct rxLocalLight) : public rxLight
{
	// common light data
	float4			m_position;		// light position in world-space
	float4			m_diffuseColor;	// rgb - color, a - light intensity
	float4			m_radiusInvRadius;	// x - light radius, y - inverse light range
	float4			m_specularColor;
	// spot light data
	float4			m_spotDirection;	// normalized axis direction in world space
	// x = cosTheta (cosine of half inner cone angle), y = cosPhi (cosine of half outer cone angle)
	float4			m_spotAngles;

	// nudge a bit to reduce self-shadowing (we can get shadow acne on all surfaces without this bias)
	F4				m_shadowDepthBias;

	LightFlags		m_flags;
	LightType		m_lightType;	// ELightType

	// for light flares
	rxTexture::Ref	m_billboard;
	F4				m_billboardSize;

	// spot light projector
	rxTexture::Ref	m_projector;
	F4				m_projectorIntensity;

	//@hack: for subsurface scattering
	U4				m_bUseSSS;

public:
	mxDECLARE_CLASS(rxLocalLight,rxLight);
	mxDECLARE_REFLECTION;

	rxLocalLight();

	bool EnclosesView( const rxSceneContext& sceneContext );

	const Vec3D& GetOrigin() const;
	void SetOrigin( const Vec3D& newPos );

	// range of influence
	FLOAT GetRadius() const;
	void SetRadius( FLOAT newRadius );

	// Direction that the light is pointing in world space.
	void SetDirection( const Vec3D& newDirection );
	const Vec3D& GetDirection() const;

	// Sets the apex angles for the spot light which determine the light's angles of illumination.

	// theta - angle, in radians, of a spotlight's inner cone - that is, the fully illuminated spotlight cone.
	// This value must be in the range from 0 through the value specified by Phi.
	//
	void SetInnerConeAngle( FLOAT theta );
	FLOAT GetInnerConeAngle() const;

	// phi - angle, in radians, defining the outer edge of the spotlight's outer cone. Points outside this cone are not lit by the spotlight.
	// This value must be between 0 and pi.
	//
	void SetOuterConeAngle( FLOAT phi );
	FLOAT GetOuterConeAngle() const;

	// Set projective texture blend factor ('factor' must be in range [0..1]).
	void SetProjectorIntensity( FLOAT factor = 0.5f );
	FLOAT GetProjectorIntensity() const;

	void SetShadowDepthBias( FLOAT f );
	FLOAT GetShadowDepthBias() const;

	FLOAT CalcBottomRadius() const;

	bool DoesCastShadows() const;
	void SetCastShadows( bool bShadows );

	mxIMPLEMENT_COMMON_PROPERTIES(rxLocalLight);
};


//
//	rxPointLight - is a point source.
//
//	The light has a position in space and radiates light in all directions.
//


//
//	rxSpotLight - is a spotlight source.
//
//	This light is like a point light,
//	except that the illumination is limited to a cone.
//	This light type has a direction and several other parameters
//	that determine the shape of the cone it produces.
//


/*
-----------------------------------------------------------------------------
	SDynLight

	represents a visible dynamic light source which can cast a shadow;

	this structure should be as small as possible
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct SDynLight)
{
	float4		posAndRadius;	// xyz = light position in view-space, z = light range
	float4		diffuseAndInvRadius;	// rgb = diffuse color, a = inverse range
	float4		falloff;	// x - falloff start, y - falloff width
	float4x4	viewProjection;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
