#pragma once

#include <EditorSupport/AssetPipeline/MeshConverter.h>

#include <Engine/Entities/StaticModelEntity.h>

#include "editor_common.h"
#include "basic_entity_editor.h"

/*
-----------------------------------------------------------------------------
	Static_Model_Editor
-----------------------------------------------------------------------------
*/
class Static_Model_Editor : public QObject, public AEntityEditor, public APlaceable
{
	Q_OBJECT

private:

	TValidPtr< StaticModelEntity >	m_staticModelEntity;

public:
	mxDECLARE_CLASS(Static_Model_Editor,AEntityEditor);

	Static_Model_Editor( World_Entities_List* theParent, StaticModelEntity* theEntity );

	void Build_From_Mesh( const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData );

	virtual const char* edGetName() const override;

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

	virtual void edShowContextMenu( const EdShowContextMenuArgs& args ) override;

	virtual APlaceable* IsPlaceable() {return this;}

	virtual const Vec3D& GetOrigin() const override;
	virtual const Quat& GetOrientation() const override;
	virtual const FLOAT GetScale() const override;
	virtual void SetOrigin_Impl( const Vec3D& newPos ) override;
	virtual void SetOrientation_Impl( const Quat& newRot ) override;
	virtual void SetScale_Impl( const FLOAT newScale ) override;
	virtual void GetWorldAABB( AABB & bbox ) const override;

private slots:
	void slot_Random_Action();

public:
	static Static_Model_Editor* Create_Static_Model(
		World_Entities_List* theParent, const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData );
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
