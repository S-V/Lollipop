#pragma once

#include <Renderer/Scene/Light.h>

#include "editor_common.h"
#include "render/hit_testing.h"

enum ELocalLightType
{
	LocalLight_Point = ELightType::Light_Point,
	LocalLight_Spot = ELightType::Light_Spot,
};
mxDECLARE_ENUM( ELocalLightType, U4, LocalLightType );

/*
-----------------------------------------------------------------------------
	LocalLightHitProxy
-----------------------------------------------------------------------------
*/
struct LocalLightHitProxy : public AHitProxy
{
	mxDECLARE_CLASS( LocalLightHitProxy, AHitProxy );

	Local_Light_Editor &	rEditor;

public:
	LocalLightHitProxy( Local_Light_Editor& editor );

	virtual APlaceable* IsPlaceable() override;
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
	virtual void edRemoveSelf() override;
};


/*
-----------------------------------------------------------------------------
	Local_Light_Editor
-----------------------------------------------------------------------------
*/
struct Local_Light_Editor : public AObjectEditor, public APlaceable
{
	Scene_Editor_Local_Lights &	m_parent;
	rxRenderWorld &	m_scene;

	String	m_name;

	// index of rxLocalLight
	UINT	m_editedObjectIndex;

	LocalLightHitProxy	m_hitProxy;

public:
	mxDECLARE_CLASS(Local_Light_Editor,AObjectEditor);

	Local_Light_Editor( Scene_Editor_Local_Lights& theParent, rxLocalLight& theLight );
	~Local_Light_Editor();

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
	virtual bool edCanBeRenamed() const override;
	virtual bool edSetName( const char* newName ) override;
	virtual void edRemoveSelf() override;

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

	virtual APlaceable* IsPlaceable() override;
//	virtual AEditable* GetEditorObject() override;

	virtual const Vec3D& GetOrigin() const override;
	virtual const Quat& GetOrientation() const override;
	virtual const FLOAT GetScale() const override;
	virtual void SetOrigin_Impl( const Vec3D& newPos ) override;
	virtual void SetOrientation_Impl( const Quat& newRot ) override;
	virtual void SetScale_Impl( const FLOAT newScale ) override;
	virtual void GetWorldAABB( AABB & bbox ) const override;
	virtual Matrix4 GetWorldTransform() const override;

	virtual void Placeable_Draw_Editor_Stuff( const EdDrawContext& drawContext ) override;

	void Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );


	rxLocalLight& GetLight() const;


	UINT GetLightType() const;
	void SetLightType( UINT lightType );

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

	bool IsSSSLight() const;
	void SetSSSLight( bool bSSSLight );

	bool DoesCastShadows() const;
	void SetCastShadows( bool bShadows );
};

/*
-----------------------------------------------------------------------------
	Scene_Editor_Local_Lights
-----------------------------------------------------------------------------
*/
struct Scene_Editor_Local_Lights : public QObject, public TRefCountedObjectList<Local_Light_Editor>
{
	Q_OBJECT

public:

	Graphics_Scene_Editor &	m_parent;
	rxRenderWorld &	m_scene;

	ID3D11ShaderResourceViewPtr	m_lightBulbTextureSRV;

public:
	typedef TRefCountedObjectList Super;

	Scene_Editor_Local_Lights( Graphics_Scene_Editor& theParent, rxRenderWorld& theScene );
	~Scene_Editor_Local_Lights();

	virtual void PostLoad() override;

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

	void Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );

public slots:
	void Create_New_Local_Light();
};


/*
-----------------------------------------------------------------------------
	Dir_Light_Editor
-----------------------------------------------------------------------------
*/
struct Dir_Light_Editor : public AObjectEditor
{
	Scene_Editor_Global_Lights &	m_parent;
	rxRenderWorld &	m_scene;

	String	m_name;

	// index of rxParallelLight
	UINT	m_editedObjectIndex;

public:
	mxDECLARE_CLASS(Dir_Light_Editor,AObjectEditor);

	Dir_Light_Editor( Scene_Editor_Global_Lights& theParent, rxParallelLight& theLight );
	~Dir_Light_Editor();

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
	virtual bool edCanBeRenamed() const override;
	virtual bool edSetName( const char* newName ) override;
	virtual void edRemoveSelf() override;

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

	rxParallelLight& GetLight() const;

	// Direction that the light is pointing in world space.
	void SetDirection( const Vec3D& newDirection );
	const Vec3D& GetDirection() const;

	bool DoesCastShadows() const;
	void SetCastShadows( bool bShadows );
};
/*
-----------------------------------------------------------------------------
	Scene_Editor_Global_Lights
-----------------------------------------------------------------------------
*/
class Scene_Editor_Global_Lights : public QObject, public TRefCountedObjectList<Dir_Light_Editor>
{
	Q_OBJECT

public:

	Graphics_Scene_Editor &	m_parent;
	rxRenderWorld &	m_scene;

public:
	typedef TRefCountedObjectList Super;

	Scene_Editor_Global_Lights( Graphics_Scene_Editor& theParent, rxRenderWorld& theScene );
	~Scene_Editor_Global_Lights();

	virtual void PostLoad() override;

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

public slots:
	void Create_New_Dir_Light();
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
