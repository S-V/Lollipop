#include "stdafx.h"

#include <EditorSupport/Util/CreateProperties.h>
#include <EditorSupport/AssetPipeline/MeshConverter.h>

#include <QtSupport/qt_common.h>

#include "static_model_editor.h"

/*
-----------------------------------------------------------------------------
	Static_Model_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Static_Model_Editor);

Static_Model_Editor::Static_Model_Editor( World_Entities_List* theParent, StaticModelEntity* theEntity )
	: Super( theParent, theEntity )
	, m_staticModelEntity( theEntity )
{

}

static
void F_Create_Render_Model_From_Mesh( rxModel & rModel, const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData )
{
	rModel.m_localToWorld = XMMatrixIdentity();

	// Load mesh.

	rModel.m_mesh.SetFromGuid( assetInfo.assetGuid );

	// Copy submeshes.

	const UINT numBatches = pStaticMeshData->batches.Num();

	rModel.m_batches.SetNum( numBatches );

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const MeshPart & src = pStaticMeshData->batches[ iBatch ];
		rxModelBatch & dst = rModel.m_batches[ iBatch ];

		dst.material.SetDefaultInstance();
		dst.indexCount = src.indexCount;
		dst.startIndex = src.startIndex;
		dst.baseVertex = src.baseVertex;
	}


	rxAABB_From_AABB( rModel.m_localAABB, pStaticMeshData->localBounds );
}

void Static_Model_Editor::Build_From_Mesh( const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData )
{
	F_Create_Render_Model_From_Mesh( m_staticModelEntity->m_model, assetInfo, pStaticMeshData );

	m_staticModelEntity->m_transform.SetIdentity();
	m_staticModelEntity->UpdateDerivedValues();

}

const char* Static_Model_Editor::edGetName() const
{
	return "Static Model";
}

void Static_Model_Editor::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);
	APlaceable::edCreateCommonProperties(properties);
	Properties::For_rxAABB( m_staticModelEntity->m_worldAABB, "WorldAABB", properties );
	Properties::For_rxModel( m_staticModelEntity->m_model, "RenderModel", properties );
}

void Static_Model_Editor::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	UNDONE;
	//QMenu	menu;

	//qtCONNECT(
	//	menu.addAction("Random Action"), SIGNAL(triggered()),
	//	this, SLOT(slot_Random_Action())
	//);

	//menu.exec( QPoint( args.x, args.y ) );
}

void Static_Model_Editor::slot_Random_Action()
{
	UNDONE;
}

const Vec3D& Static_Model_Editor::GetOrigin() const
{
	return m_staticModelEntity->m_transform.translation;
}

const Quat& Static_Model_Editor::GetOrientation() const
{
	return m_staticModelEntity->m_transform.orientation;
}

const FLOAT Static_Model_Editor::GetScale() const
{
	return m_staticModelEntity->m_transform.scaleFactor;
}

void Static_Model_Editor::SetOrigin_Impl( const Vec3D& newPos )
{
	m_staticModelEntity->m_transform.translation = newPos;
	m_staticModelEntity->UpdateDerivedValues();
}

void Static_Model_Editor::SetOrientation_Impl( const Quat& newRot )
{
	m_staticModelEntity->m_transform.orientation = newRot;
	m_staticModelEntity->UpdateDerivedValues();
}

void Static_Model_Editor::SetScale_Impl( const FLOAT newScale )
{
	m_staticModelEntity->m_transform.scaleFactor = newScale;
	m_staticModelEntity->UpdateDerivedValues();
}

void Static_Model_Editor::GetWorldAABB( AABB & bbox ) const
{
	rxAABB_To_AABB( m_staticModelEntity->m_worldAABB, bbox );
}

Static_Model_Editor* Static_Model_Editor::Create_Static_Model(
	World_Entities_List* theParent, const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData )
{
	StaticModelEntity* pStaticModelEntity = new StaticModelEntity();
	Static_Model_Editor* pStaticModelEditor = new Static_Model_Editor( theParent, pStaticModelEntity );
	pStaticModelEditor->Build_From_Mesh( assetInfo, pStaticMeshData );
	return pStaticModelEditor;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
