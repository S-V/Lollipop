/*
=============================================================================
	File:	Light.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include <Core/Editor/EditableProperties.h>

#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/Util/ScreenQuad.h>

/*
-----------------------------------------------------------------------------
	ELightType
-----------------------------------------------------------------------------
*/
mxBEGIN_ENUM(LightType)
	mxREFLECT_ENUM(Light_Directional)
	mxREFLECT_ENUM(Light_Point)
	mxREFLECT_ENUM(Light_Spot)
mxEND_ENUM

/*
-----------------------------------------------------------------------------
	ELightFlags
-----------------------------------------------------------------------------
*/
mxBEGIN_FLAGS(LightFlags)
	mxREFLECT_BIT(LF_Enabled)
	mxREFLECT_BIT(LF_Shadows)
mxEND_FLAGS

/*
-----------------------------------------------------------------------------
	rxLight
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(rxLight);

/*
-----------------------------------------------------------------------------
	rxGlobalLight
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(rxGlobalLight);

rxGlobalLight::rxGlobalLight()
{

}

/*
-----------------------------------------------------------------------------
	rxParallelLight
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(rxParallelLight);
mxBEGIN_REFLECTION(rxParallelLight)
	mxMEMBER_FIELD( m_lightDirWS )
	mxMEMBER_FIELD( m_diffuseColor )
	mxMEMBER_FIELD( m_specularColor )
	mxMEMBER_FIELD( m_backColor )
	mxMEMBER_FIELD( m_ambientColor )
	mxMEMBER_FIELD( m_backLightOffset )

	mxMEMBER_FIELD( m_flags )

	mxMEMBER_FIELD( m_shadowDepthBias )
	mxMEMBER_FIELD( m_shadowFadeDistance )
mxEND_REFLECTION

rxParallelLight::rxParallelLight()
{
	m_lightDirWS = g_XMNegIdentityR1;
	m_diffuseColor = g_XMOne;
	m_specularColor = g_XMZero;
	m_backColor = g_XMZero;
	m_ambientColor = g_XMZero;
	m_backLightOffset = 0.0f;

	m_flags = LF_All;

	m_shadowDepthBias = 0.0f;
	m_shadowFadeDistance = 500.0f;
}

void rxParallelLight::SetDiffuseColor( const ColorRGB& newDiffuseColor )
{
	m_diffuseColor = vec_load1( newDiffuseColor.AsVec3D() );
}
const ColorRGB& rxParallelLight::GetDiffuseColor() const
{
	return as_vec3(m_diffuseColor);
}
void rxParallelLight::SetSpecularColor( const ColorRGB& newSpecularColor )
{
	m_specularColor = vec_load1( newSpecularColor.AsVec3D() );
}
void rxParallelLight::SetDirection( const Vec3D& newDirection )
{
	m_lightDirWS = vec_load0( newDirection.GetNormalized() );
}
Vec3D rxParallelLight::GetDirection() const
{
	return as_vec3(m_lightDirWS);
}

bool rxParallelLight::DoesCastShadows() const
{
	return m_flags & LF_Shadows;
}

void rxParallelLight::SetCastShadows( bool bShadows )
{
	setbit_cond( m_flags.m_bitmask, bShadows, LF_Shadows );
}

//---------------------------------------------------------------------------

static
FORCEINLINE
FASTBOOL Point_Light_Encloses_Eye( const rxLocalLight& light, const rxSceneContext& view )
{
	mxOPTIMIZE("vectorize:");
#if 0
	FLOAT distanceSqr = (light.GetOrigin() - view.GetOrigin()).LengthSqr();
	return distanceSqr < squaref( light.GetRadius() + view.nearZ + 0.15f );
#else
	//FLOAT distance = (light.GetOrigin() - view.GetOrigin()).LengthFast();
	//return distance < light.GetRadius() + view.nearZ;
	float4	lightVec = XMVectorSubtract( light.m_position, view.invViewMatrix.r[3] );
	float4	lightVecSq = XMVector3LengthSq( lightVec );
	float	distanceSq = XMVectorGetX( lightVecSq );
	return distanceSq < squaref( light.GetRadius() + view.nearZ );
#endif
}

static
FORCEINLINE
FASTBOOL Spot_Light_EnclosesView( const rxLocalLight& light, const rxSceneContext& view )
{
#if 0

	Vec3D  origin( this->GetOrigin() );	// cone origin

	Vec3D  dir( GetDirection() );	// direction of cone axis
	//Assert( dir.IsNormalized() );

	FLOAT t = dir * ( point - origin );

	if ( t < 0 || t > GetRange() ) {
		return FALSE;
	}

	FLOAT r = t * ( bottomRadius * GetInvRange() );	// cone radius at closest point

	// squared distance from the point to the cone axis
	FLOAT distSq = (( origin + dir * t ) - point).LengthSqr();

	return ( distSq < squaref(r) );

#else

	Vec3D  origin( light.GetOrigin() );

	Vec3D  d( view.GetOrigin() - origin );

	FLOAT sqrLength = d.LengthSqr();

	d *= mxInvSqrt( sqrLength );

	FLOAT x = d * light.GetDirection();

	FLOAT cosPhi = as_vec4(light.m_spotAngles).y;

	return ( x > 0.0f )
		&& ( x >= cosPhi )
		&& ( sqrLength < squaref(light.GetRadius()) );

#endif
}

/*
-----------------------------------------------------------------------------
	rxLocalLight
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(rxLocalLight);
mxBEGIN_REFLECTION(rxLocalLight)
	mxMEMBER_FIELD( m_position )
	mxMEMBER_FIELD( m_diffuseColor )
	mxMEMBER_FIELD( m_radiusInvRadius )
	mxMEMBER_FIELD( m_specularColor )

	mxMEMBER_FIELD( m_spotDirection )
	mxMEMBER_FIELD( m_spotAngles )

	mxMEMBER_FIELD( m_shadowDepthBias )

	mxMEMBER_FIELD( m_flags )
	mxMEMBER_FIELD( m_lightType )

	mxMEMBER_FIELD( m_billboard )
	mxMEMBER_FIELD( m_billboardSize )

	mxMEMBER_FIELD2( m_projector,	Projector_Texture,	Field_NoDefaultInit )
	mxMEMBER_FIELD( m_projectorIntensity )

	mxMEMBER_FIELD( m_bUseSSS )
mxEND_REFLECTION


rxLocalLight::rxLocalLight()
{
	m_position = g_XMIdentityR3;
	m_diffuseColor = XMVectorSet( 1.0f, 1.0f, 1.0f, 1.0f );
	m_radiusInvRadius = XMVectorSet( 1.0f, 1.0f, 0.0f, 0.0f );
	m_specularColor = g_XMZero;

	m_spotDirection = XMVectorSet( 0.0f, -1.0f, 0.0f, 0.0f );
	this->SetOuterConeAngle(90.0f);
	this->SetInnerConeAngle(45.0f);

	//m_shadowDepthBias = -0.0005f;
	m_shadowDepthBias = -0.0001f;	//<- for lights with large radius

	m_flags = LF_All;
	m_lightType = ELightType::Light_Point;

	m_billboardSize = 1.0f;
	m_projectorIntensity = 0.5f;

	m_bUseSSS = 0;
}

bool rxLocalLight::EnclosesView( const rxSceneContext& sceneContext )
{
	switch( m_lightType )
	{
	case Light_Point :
		return Point_Light_Encloses_Eye( *this, sceneContext );

	case Light_Spot :
		return Spot_Light_EnclosesView( *this, sceneContext );

	default:
		Unreachable;
	}
	return false;
}

const Vec3D& rxLocalLight::GetOrigin() const
{
	return as_vec3(m_position);
}

void rxLocalLight::SetOrigin( const Vec3D& newPos )
{
	m_position = vec_load1(newPos);
}

FLOAT rxLocalLight::GetRadius() const
{
	return XMVectorGetX(m_radiusInvRadius);
}

void rxLocalLight::SetRadius( FLOAT newRadius )
{
	Assert(newRadius > 0.0f);
	VRET_IF_NOT(newRadius > 0.0f);
	const FLOAT invRadius = 1.0f / newRadius;
	m_radiusInvRadius = XMVectorSet( newRadius, invRadius, 0.0f, 0.0f );
}

void rxLocalLight::SetDirection( const Vec3D& newDirection )
{
	m_spotDirection = vec_load0(newDirection);
}

const Vec3D& rxLocalLight::GetDirection() const
{
	return as_vec3(m_spotDirection);
}

void rxLocalLight::SetInnerConeAngle( FLOAT theta )
{
	theta = DEG2RAD(theta);
	AssertX( 0.0f < theta && theta < this->GetOuterConeAngle(), "spotlight inner angle must be in range [0..outerAngle]" );
	FLOAT cosTheta = mxCos( theta * 0.5f );
	as_vec4(m_spotAngles).x = cosTheta;
}

FLOAT rxLocalLight::GetInnerConeAngle() const
{
	FLOAT cosTheta = as_vec4( m_spotAngles ).x;
	FLOAT innerAngle = mxACos(cosTheta) * 2.0f;
	innerAngle = RAD2DEG(innerAngle);
	return innerAngle;
}

void rxLocalLight::SetOuterConeAngle( FLOAT phi )
{
	phi = DEG2RAD(phi);
	AssertX( 0.0f < phi && phi < MX_PI, "spotlight outer angle must be in range [0..PI]" );
	FLOAT cosPhi = mxCos( phi * 0.5f );
	as_vec4(m_spotAngles).y = cosPhi;

	FLOAT	innerAngle = this->GetInnerConeAngle();
	innerAngle = minf(innerAngle,RAD2DEG(phi-0.001f));
	this->SetInnerConeAngle(innerAngle);
}

FLOAT rxLocalLight::GetOuterConeAngle() const
{
	FLOAT cosTheta = as_vec4( m_spotAngles ).y;
	FLOAT outerAngle = 2.0f * mxACos( cosTheta );
	outerAngle = RAD2DEG(outerAngle);
	return outerAngle;
}

FLOAT rxLocalLight::CalcBottomRadius() const
{
	FLOAT cosPhi = as_vec4( m_spotAngles ).y;
	FLOAT halfOuterAngle = mxACos( cosPhi );
	FLOAT bottomRadius = this->GetRadius() * mxTan( halfOuterAngle );
	return bottomRadius;
}

void rxLocalLight::SetProjectorIntensity( FLOAT factor )
{
	m_projectorIntensity = factor;
}

FLOAT rxLocalLight::GetProjectorIntensity() const
{
	return m_projectorIntensity;
}

void rxLocalLight::SetShadowDepthBias( FLOAT f )
{
	m_shadowDepthBias = f;
}

FLOAT rxLocalLight::GetShadowDepthBias() const
{
	return m_shadowDepthBias;
}

bool rxLocalLight::DoesCastShadows() const
{
	return m_flags & LF_Shadows;
}

void rxLocalLight::SetCastShadows( bool bShadows )
{
	setbit_cond( m_flags.m_bitmask, bShadows, LF_Shadows );
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
