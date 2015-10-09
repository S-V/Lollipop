#pragma once

#include "editor_common.h"
#include "basic_entity_editor.h"
#include "light_editor.h"
#include "sky_editor.h"
#include "render/hit_testing.h"

class rxModelBatch;
class StaticTriangleMeshData;

/*
-----------------------------------------------------------------------------
	ModelBatchHitProxy
-----------------------------------------------------------------------------
*/
struct ModelBatchHitProxy : public QObject, public AHitProxy
{
	Q_OBJECT

public:

	mxDECLARE_CLASS( ModelBatchHitProxy, AHitProxy );

	TPtr<Graphics_Model_Editor>		pModel;
	UINT							iBatch;

public:
	ModelBatchHitProxy();

	rxModelBatch& GetModelBatch() const;

	virtual APlaceable* IsPlaceable() override;
	virtual bool edOnItemDropped( AEditable* pDroppedItem ) override;
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;
	virtual const char* edGetName() const override;
	virtual void edRemoveSelf() override;

public slots:
	void slot_ShowProperties();
};

/*
-----------------------------------------------------------------------------
	Graphics_Model_Editor
-----------------------------------------------------------------------------
*/
struct Graphics_Model_Editor : public QObject, public AObjectEditor, public APlaceable
{
	Q_OBJECT

public:

	Graphics_Models_List &	m_parent;
	rxRenderWorld &	m_scene;

	// name (initially assigned upon creation from mesh)
	String	m_name;

	// index of rxModel
	UINT	m_modelIndex;

	rxTransform		m_cachedTransform;

	TFixedArray<ModelBatchHitProxy>	m_hitProxies;

private:
	void UpdateCachedValues();

public:
	mxDECLARE_CLASS(Graphics_Model_Editor,AObjectEditor);

	Graphics_Model_Editor( Graphics_Models_List& theParent, rxModel& theModel );
	~Graphics_Model_Editor();

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;
	virtual bool edCanBeRenamed() const override;
	virtual bool edSetName( const char* newName ) override;
	virtual void edRemoveSelf() override;

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

	virtual APlaceable* IsPlaceable() override;
//	virtual AEditable* GetEditorObject() override;

	rxModel& GetModel() const;

	virtual const Vec3D& GetOrigin() const override;
	virtual const Quat& GetOrientation() const override;
	virtual const FLOAT GetScale() const override;
	virtual void SetOrigin_Impl( const Vec3D& newPos ) override;
	virtual void SetOrientation_Impl( const Quat& newRot ) override;
	virtual void SetScale_Impl( const FLOAT newScale ) override;
	virtual void GetWorldAABB( AABB & bbox ) const override;

	virtual void Placeable_Draw_Editor_Stuff( const EdDrawContext& drawContext ) override;

	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );

private slots:
	void slot_Random_Action();

public:
	static Graphics_Model_Editor* Static_Create(
		Graphics_Models_List& parent
		, rxModel& model
		);

	static Graphics_Model_Editor* Static_Create_From_Mesh(
		World_Editor& rWorld
		, const SAssetInfo& rAssetInfo
		, const StaticTriangleMeshData* pStaticMeshData
		);
};

/*
-----------------------------------------------------------------------------
	Graphics_Models_List
-----------------------------------------------------------------------------
*/
struct Graphics_Models_List : public TRefCountedObjectList<Graphics_Model_Editor>
{
	TValidPtr< Graphics_Scene_Editor >	m_parent;

public:
	typedef TRefCountedObjectList Super;

	Graphics_Models_List( Graphics_Scene_Editor* theParent );
	~Graphics_Models_List();

	virtual void PostLoad() override;

	virtual AEditable* edGetParent() override;
	virtual const char* edGetName() const override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

	void Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
	void Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext );
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
