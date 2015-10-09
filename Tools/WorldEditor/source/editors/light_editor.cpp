#include "stdafx.h"

#include <Renderer/Scene/RenderWorld.h>

#include <QtSupport/qt_common.h>

#include "light_editor.h"
#include "graphics_world_editor.h"
#include "render/viewports.h"
#include "util/load_texture_from_file_util.h"

mxBEGIN_ENUM(LocalLightType)
	mxREFLECT_ENUM(LocalLight_Point)
	mxREFLECT_ENUM(LocalLight_Spot)
mxEND_ENUM


/*
-----------------------------------------------------------------------------
	LocalLightHitProxy
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(LocalLightHitProxy);

LocalLightHitProxy::LocalLightHitProxy( Local_Light_Editor& editor )
	: rEditor( editor )
{
}

APlaceable* LocalLightHitProxy::IsPlaceable()
{
	return &rEditor;
}

void LocalLightHitProxy::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	rEditor.edCreateProperties(properties,false);
}

void LocalLightHitProxy::edRemoveSelf()
{
	rEditor.edRemoveSelf();
}

/*
-----------------------------------------------------------------------------
	Local_Light_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Local_Light_Editor);

Local_Light_Editor::Local_Light_Editor( Scene_Editor_Local_Lights& theParent, rxLocalLight& theLight )
	: m_parent( theParent )
	, m_scene( theParent.m_scene )
	, m_hitProxy( *this )
{
	m_editedObjectIndex = m_scene.m_localLights.GetItemIndex( &theLight );

	m_name = "Local Light ";
	m_name.Append(String(theParent.Num()));
}

Local_Light_Editor::~Local_Light_Editor()
{

}

AEditable* Local_Light_Editor::edGetParent()
{
	return &m_parent;
}

const char* Local_Light_Editor::edGetName() const
{
	return m_name;
}

bool Local_Light_Editor::edCanBeRenamed() const
{
	return true;
}

bool Local_Light_Editor::edSetName( const char* newName )
{
	m_name = newName;
	return true;
}

void Local_Light_Editor::edRemoveSelf()
{
	F_Remove_Editor_Template( this, m_parent, m_scene.m_localLights );
}

void Local_Light_Editor::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	APlaceable::edCreateCommonProperties(properties);

	rxLocalLight& rLight = this->GetLight();

	new EdProperty_ColorRGBA( properties, "diffuseColor", float4_as_FColor(rLight.m_diffuseColor) );
	new EdProperty_ColorRGBA( properties, "specularColor", float4_as_FColor(rLight.m_specularColor) );

	const mxEnumType& enumType = T_DeduceTypeInfo<LocalLightType>().UpCast<mxEnumType>();
	MxEnumPropertyX( Local_Light_Editor, enumType, GetLightType, SetLightType );

	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "radius", GetRadius, SetRadius );

	MxProperty( properties, Local_Light_Editor, EdProperty_Vector3D, "Spot Light Direction", GetDirection, SetDirection );
	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "Spot Light Inner Cone Angle", GetInnerConeAngle, SetInnerConeAngle );
	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "Spot Light Outer Cone Angle", GetOuterConeAngle, SetOuterConeAngle );

	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "shadowDepthBias", GetShadowDepthBias, SetShadowDepthBias );

	new EdProperty_AssetReference( rLight.m_projector, "Projector", properties );
	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "projectorIntensity", GetProjectorIntensity, SetProjectorIntensity );

	MxProperty( properties, Local_Light_Editor, EdProperty_Bool, "Shadows", DoesCastShadows, SetCastShadows );

	MxProperty( properties, Local_Light_Editor, EdProperty_Bool, "isSSSLight", IsSSSLight, SetSSSLight );

	MxProperty( properties, Local_Light_Editor, EdProperty_Float, "shadowDepthBias", GetShadowDepthBias, SetShadowDepthBias );
}

bool Local_Light_Editor::IsSSSLight() const
{
	return this->GetLight().m_bUseSSS;
}

void Local_Light_Editor::SetSSSLight( bool bSSSLight )
{
	this->GetLight().m_bUseSSS = bSSSLight;
}

bool Local_Light_Editor::DoesCastShadows() const
{
	return this->GetLight().DoesCastShadows();
}

void Local_Light_Editor::SetCastShadows( bool bShadows )
{
	this->GetLight().SetCastShadows( bShadows );
}

APlaceable* Local_Light_Editor::IsPlaceable()
{
	return this;
}

//AEditable* Local_Light_Editor::GetEditorObject()
//{
//	return this;
//}

const Vec3D& Local_Light_Editor::GetOrigin() const
{
	return this->GetLight().GetOrigin();
}

const Quat& Local_Light_Editor::GetOrientation() const
{
	return Quat::quat_identity;
	//return Matrix3::CreateRotation( Vec3D::vec3_unit_z, this->GetDirection() ).ToQuat();
}

const FLOAT Local_Light_Editor::GetScale() const
{
	return this->GetRadius();
}

void Local_Light_Editor::SetOrigin_Impl( const Vec3D& newPos )
{
	this->GetLight().SetOrigin(newPos);
}

void Local_Light_Editor::SetOrientation_Impl( const Quat& newRot )
{
	//mxUNUSED(newRot);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion( newRot.quad );
	this->GetLight().m_spotDirection = XMVector3TransformNormal(
		this->GetLight().m_spotDirection,
		rotationMatrix
		);
	this->GetLight().m_spotDirection = XMVector3Normalize( this->GetLight().m_spotDirection );
}

void Local_Light_Editor::SetScale_Impl( const FLOAT newScale )
{
	this->SetRadius(newScale);
}

void Local_Light_Editor::GetWorldAABB( AABB & bbox ) const
{
	Sphere	sphereBounds( this->GetOrigin(), this->GetRadius() );
	bbox.FromSphere( sphereBounds );
}

Matrix4 Local_Light_Editor::GetWorldTransform() const
{
	//const Quat q = this->GetOrientation();
	//const Vec3D p = this->GetOrigin();
	//Matrix4 xform;
	//xform.BuildTransform( p, q );
	//return xform;
	return as_matrix4(XMMatrixTranslationFromVector( this->GetLight().m_position ));
}

void Local_Light_Editor::Placeable_Draw_Editor_Stuff( const EdDrawContext& drawContext )
{
	const rxSceneContext &	sceneContext = drawContext.sceneContext;
	BatchRenderer &	renderer = drawContext.renderer;

	renderer.SetTransform(sceneContext.viewProjectionMatrix);

	const rxLocalLight& rLight = this->GetLight();
	const ELightType eLightType = rLight.m_lightType;

	const FColor lightColor(as_vec4(rLight.m_diffuseColor));

	const Vec3D center = rLight.GetOrigin();
	const FLOAT radius = rLight.GetRadius();

	if( eLightType == Light_Point )
	{
		const UINT numCircleSegments = 32;

		const Vec3D axisX( 1.0f, 0.0f, 0.0f );
		const Vec3D axisY( 0.0f, 1.0f, 0.0f );
		const Vec3D axisZ( 0.0f, 0.0f, 1.0f );

		renderer.DrawCircle(
			center,
			axisY,
			axisZ,
			lightColor,
			radius,
			numCircleSegments
		);
		renderer.DrawCircle(
			center,
			axisX,
			axisZ,
			lightColor,
			radius,
			numCircleSegments
		);
		renderer.DrawCircle(
			center,
			axisX,
			axisY,
			lightColor,
			radius,
			numCircleSegments
		);
	}
	else if( eLightType == Light_Spot )
	{
		// Draw the axis arrow cone

		// Calculate the vertices for the base of the cone

		const UINT nAXIS_ARROW_SEGMENTS = 8;
		const FLOAT fAXIS_ARROW_SEGMENTS = (FLOAT)nAXIS_ARROW_SEGMENTS;

		Vec3D	arrowVertices[ nAXIS_ARROW_SEGMENTS + 1 ];

		const FLOAT	bottomRadius = rLight.CalcBottomRadius();
		const Vec3D	bottomCenter = center + rLight.GetDirection() * radius;

		Matrix4 coneTransform(
			Matrix4::CreateTranslation( Vec3D( 0.0f, 0.0f, radius ) )
			* Matrix3::CreateRotation( Vec3D::vec3_unit_z, rLight.GetDirection() ).ToMat4()
			* Matrix4::CreateTranslation( Vec3D( center.x, center.y, center.z ) )
			);

		for( UINT iSegment = 0 ; iSegment <= nAXIS_ARROW_SEGMENTS ; iSegment++ )
		{
			const FLOAT theta = iSegment * (MX_TWO_PI / fAXIS_ARROW_SEGMENTS);	// in radians

			FLOAT	s, c;
			mxSinCos( theta, s, c );

			Vec3D	arrowVertex( bottomRadius * s, bottomRadius * c, 0.0f );

			arrowVertices[ iSegment ] = coneTransform.TransformVector( arrowVertex );
		}

		for( UINT iSegment = 0 ; iSegment < nAXIS_ARROW_SEGMENTS ; iSegment++ )
		{
			const Vec3D& p0 = arrowVertices[ iSegment ];
			const Vec3D& p1 = arrowVertices[ iSegment + 1 ];

			renderer.DrawLine3D(
				p0,
				p1,
				lightColor,
				lightColor
			);
			renderer.DrawLine3D(
				p0,
				center,
				lightColor,
				lightColor
			);
			renderer.DrawLine3D(
				p1,
				center,
				lightColor,
				lightColor
			);
		}
	}
	else {
		mxDBG_UNREACHABLE;
	}
}

void Local_Light_Editor::Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	BatchRenderer &	renderer = gRenderer.GetDrawHelper();

	const rxLocalLight& rLight = this->GetLight();

	renderer.SetTexture( m_parent.m_lightBulbTextureSRV );
	{
		const FLOAT spriteSize = 0.1f;
		const FLOAT spriteSizeX = spriteSize;
		const FLOAT spriteSizeY = spriteSize;
		const FColor spriteColor(as_vec4(rLight.m_diffuseColor));

		renderer.DrawSprite(
			as_matrix4(sceneContext.invViewMatrix),
			rLight.GetOrigin(),
			spriteSizeX, spriteSizeY,
			spriteColor
		);
	}
	renderer.SetTexture( nil );
}

void Local_Light_Editor::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	HitTesting & hitTesting = GetHitTesting();

	hitTesting.BeginHitProxy( &m_hitProxy );

	this->Draw_Editor_Stuff( viewport, sceneContext );

	hitTesting.EndHitProxy();
}

rxLocalLight& Local_Light_Editor::GetLight() const
{
	return m_scene.m_localLights[ m_editedObjectIndex ];
}

UINT Local_Light_Editor::GetLightType() const
{
	return this->GetLight().m_lightType.AsInt();
}

void Local_Light_Editor::SetLightType( UINT lightType )
{
	this->GetLight().m_lightType = static_cast< ELightType >( lightType );
}

FLOAT Local_Light_Editor::GetRadius() const
{
	return this->GetLight().GetRadius();
}

void Local_Light_Editor::SetRadius( FLOAT newRadius )
{
	this->GetLight().SetRadius(newRadius);
}

void Local_Light_Editor::SetDirection( const Vec3D& newDirection )
{
	this->GetLight().SetDirection(newDirection);
}

const Vec3D& Local_Light_Editor::GetDirection() const
{
	return this->GetLight().GetDirection();
}

void Local_Light_Editor::SetInnerConeAngle( FLOAT theta )
{
	this->GetLight().SetInnerConeAngle(theta);
}

FLOAT Local_Light_Editor::GetInnerConeAngle() const
{
	return this->GetLight().GetInnerConeAngle();
}

void Local_Light_Editor::SetOuterConeAngle( FLOAT phi )
{
	this->GetLight().SetOuterConeAngle(phi);
}

FLOAT Local_Light_Editor::GetOuterConeAngle() const
{
	return this->GetLight().GetOuterConeAngle();
}

void Local_Light_Editor::SetProjectorIntensity( FLOAT factor )
{
	this->GetLight().SetProjectorIntensity(factor);
}

FLOAT Local_Light_Editor::GetProjectorIntensity() const
{
	return this->GetLight().GetProjectorIntensity();
}

void Local_Light_Editor::SetShadowDepthBias( FLOAT f )
{
	this->GetLight().SetShadowDepthBias(f);
}

FLOAT Local_Light_Editor::GetShadowDepthBias() const
{
	return this->GetLight().GetShadowDepthBias();
}

/*
-----------------------------------------------------------------------------
	Scene_Editor_Local_Lights
-----------------------------------------------------------------------------
*/
Scene_Editor_Local_Lights::Scene_Editor_Local_Lights( Graphics_Scene_Editor& theParent, rxRenderWorld& theScene )
	: m_parent( theParent )
	, m_scene( theScene )
{
	m_lightBulbTextureSRV = GraphicsUtil::F_Load_Texture_From_File("EditorResources/light.png");
}

Scene_Editor_Local_Lights::~Scene_Editor_Local_Lights()
{

}

void Scene_Editor_Local_Lights::PostLoad()
{
	rxRenderWorld& renderWorld = m_scene;

	const UINT numLights = renderWorld.m_localLights.Num();

	this->Reserve( numLights );

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		rxLocalLight& rLight = renderWorld.m_localLights[ iLight ];

		Local_Light_Editor* pEditor = new Local_Light_Editor( *this, rLight );

		Super::Add( pEditor );
	}
}

AEditable* Scene_Editor_Local_Lights::edGetParent()
{
	return &m_parent;
}

const char* Scene_Editor_Local_Lights::edGetName() const
{
	return "Local Lights";
}

void Scene_Editor_Local_Lights::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	qtCONNECT(
		args.parent->addAction("Create New Light"), SIGNAL(triggered()),
		this, SLOT(Create_New_Local_Light())
	);
}

void Scene_Editor_Local_Lights::Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	const UINT numLights = this->Num();

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		Local_Light_Editor* pEditor = (*this)[ iLight ];

		pEditor->Draw_Editor_Stuff( viewport, sceneContext );
	}
}

void Scene_Editor_Local_Lights::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	const UINT numLights = this->Num();

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		Local_Light_Editor* pEditor = (*this)[ iLight ];

		pEditor->Draw_Hit_Proxies( viewport, sceneContext );
	}
}

void Scene_Editor_Local_Lights::Create_New_Local_Light()
{
	rxLocalLight& rNewLight = m_scene.CreateLocalLight();

	Local_Light_Editor* pNewEditor = new Local_Light_Editor( *this, rNewLight );

	this->Add( pNewEditor );

	gCore.editor->Notify_SelectionChanged( pNewEditor );
}


/*
-----------------------------------------------------------------------------
	Dir_Light_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Dir_Light_Editor);

Dir_Light_Editor::Dir_Light_Editor( Scene_Editor_Global_Lights& theParent, rxParallelLight& theLight )
	: m_parent( theParent )
	, m_scene( theParent.m_scene )
{
	m_editedObjectIndex = m_scene.m_dirLights.GetItemIndex( &theLight );

	m_name = "Directional Light ";
	m_name.Append(String(theParent.Num()));
}

Dir_Light_Editor::~Dir_Light_Editor()
{

}

AEditable* Dir_Light_Editor::edGetParent()
{
	return &m_parent;
}

const char* Dir_Light_Editor::edGetName() const
{
	return m_name;
}

bool Dir_Light_Editor::edCanBeRenamed() const
{
	return true;
}

bool Dir_Light_Editor::edSetName( const char* newName )
{
	m_name = newName;
	return true;
}

void Dir_Light_Editor::edRemoveSelf()
{
	F_Remove_Editor_Template( this, m_parent, m_scene.m_dirLights );
}

void Dir_Light_Editor::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	rxParallelLight& rLight = this->GetLight();

	new EdProperty_ColorRGBA( properties, "diffuseColor", float4_as_FColor(rLight.m_diffuseColor) );
	new EdProperty_ColorRGBA( properties, "specularColor", float4_as_FColor(rLight.m_specularColor) );
	new EdProperty_ColorRGBA( properties, "backColor", float4_as_FColor(rLight.m_backColor) );
	new EdProperty_ColorRGBA( properties, "ambientColor", float4_as_FColor(rLight.m_ambientColor) );
	MxProperty( properties, Dir_Light_Editor, EdProperty_Vector3D, "Direction", GetDirection, SetDirection );
	MxProperty( properties, Dir_Light_Editor, EdProperty_Bool, "Shadows", DoesCastShadows, SetCastShadows );
}

rxParallelLight& Dir_Light_Editor::GetLight() const
{
	return m_scene.m_dirLights[ m_editedObjectIndex ];
}

void Dir_Light_Editor::SetDirection( const Vec3D& newDirection )
{
	this->GetLight().SetDirection(newDirection);
}

const Vec3D& Dir_Light_Editor::GetDirection() const
{
	return this->GetLight().GetDirection();
}

bool Dir_Light_Editor::DoesCastShadows() const
{
	return this->GetLight().DoesCastShadows();
}

void Dir_Light_Editor::SetCastShadows( bool bShadows )
{
	this->GetLight().SetCastShadows( bShadows );
}

/*
-----------------------------------------------------------------------------
	Scene_Editor_Global_Lights
-----------------------------------------------------------------------------
*/
Scene_Editor_Global_Lights::Scene_Editor_Global_Lights( Graphics_Scene_Editor& theParent, rxRenderWorld& theScene )
	: m_parent( theParent )
	, m_scene( theScene )
{

}

Scene_Editor_Global_Lights::~Scene_Editor_Global_Lights()
{

}

void Scene_Editor_Global_Lights::PostLoad()
{
	rxRenderWorld& renderWorld = m_scene;

	const UINT numLights = renderWorld.m_dirLights.Num();

	this->Reserve( numLights );

	for( UINT iLight = 0; iLight < numLights; iLight++ )
	{
		rxParallelLight& rLight = renderWorld.m_dirLights[ iLight ];

		Dir_Light_Editor* pEditor = new Dir_Light_Editor( *this, rLight );

		Super::Add( pEditor );
	}
}

AEditable* Scene_Editor_Global_Lights::edGetParent()
{
	return &m_parent;
}

const char* Scene_Editor_Global_Lights::edGetName() const
{
	return "Global Lights";
}

void Scene_Editor_Global_Lights::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	qtCONNECT(
		args.parent->addAction("Create New Directional Light"), SIGNAL(triggered()),
		this, SLOT(Create_New_Dir_Light())
	);
}

void Scene_Editor_Global_Lights::Create_New_Dir_Light()
{
	rxParallelLight& rNewDirLight = m_scene.CreateDirectionalLight();

	Dir_Light_Editor* pNewEditor = new Dir_Light_Editor( *this, rNewDirLight );

	this->Add( pNewEditor );

	gCore.editor->Notify_SelectionChanged( pNewEditor );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
