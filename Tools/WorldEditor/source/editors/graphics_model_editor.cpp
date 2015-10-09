#include "stdafx.h"

#include <Renderer/Util/RenderMesh.h>
#include <Engine/Worlds.h>

#include <EditorSupport/Util/CreateProperties.h>

#include "editors/graphics_model_editor.h"
#include "editors/graphics_world_editor.h"
#include "editors/world_editor.h"
#include "util/graphics_model_util.h"
#include "property_grid.h"
#include "render/viewports.h"

/*
-----------------------------------------------------------------------------
	ModelBatchHitProxy
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( ModelBatchHitProxy );

ModelBatchHitProxy::ModelBatchHitProxy()
{
	iBatch = INDEX_NONE;
}

rxModelBatch& ModelBatchHitProxy::GetModelBatch() const
{
	rxModelBatch & modelBatch = this->pModel->GetModel().m_batches[ this->iBatch ];
	return modelBatch;
}

APlaceable* ModelBatchHitProxy::IsPlaceable()
{
	return pModel;
}

bool ModelBatchHitProxy::edOnItemDropped( AEditable* pDroppedItem )
{
	EdAssetInfo* pAssetInfo = SafeCast<EdAssetInfo>( pDroppedItem );
	if( pAssetInfo != nil )
	{
		if( pAssetInfo->d.assetType == Asset_Graphics_Material )
		{
			rxModelBatch & modelBatch = this->GetModelBatch();
			modelBatch.material.SetFromGuid( pAssetInfo->d.assetGuid );
			return true;
		}
	}
	return false;
}

void ModelBatchHitProxy::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	rxModelBatch & modelBatch = this->GetModelBatch();
	Properties::For_rxModel_Batch( modelBatch, "Batch", properties );

	pModel->edCreateProperties(properties,false);
}

void ModelBatchHitProxy::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	//pModel->edShowContextMenu(args);

	qtCONNECT(
		args.parent->addAction("Model Batch Properties"), SIGNAL(triggered()),
		this, SLOT(slot_ShowProperties())
	);
}

const char* ModelBatchHitProxy::edGetName() const
{
	return pModel->edGetName();
}

void ModelBatchHitProxy::edRemoveSelf()
{
	return pModel->edRemoveSelf();
}

void ModelBatchHitProxy::slot_ShowProperties()
{
	Util_NewPropertyEditorDockWidget( this, EditorApp::GetMainFrame() );
}

/*
-----------------------------------------------------------------------------
	Graphics_Model_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR(Graphics_Model_Editor);

Graphics_Model_Editor::Graphics_Model_Editor( Graphics_Models_List& theParent, rxModel& theModel )
	: m_parent( theParent )
	, m_scene( theParent.m_parent->m_scene )
{
	m_modelIndex = m_scene.m_models.GetItemIndex( &theModel );

	rxModel& model = this->GetModel();

	m_name = model.m_mesh.GetPath();

	mxENSURE(m_cachedTransform.FromMat4( model.m_localToWorld ));

	// Create hit proxies.

	const UINT numBatches = model.m_batches.Num();

	m_hitProxies.SetNum( numBatches );

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const rxModelBatch & src = model.m_batches[ iBatch ];
		ModelBatchHitProxy & dst = m_hitProxies[ iBatch ];

		dst.pModel = this;
		dst.iBatch = iBatch;
	}
}

Graphics_Model_Editor::~Graphics_Model_Editor()
{
}

AEditable* Graphics_Model_Editor::edGetParent()
{
	return &m_parent;
}

const char* Graphics_Model_Editor::edGetName() const
{
	return m_name;
}

bool Graphics_Model_Editor::edCanBeRenamed() const
{
	return true;
}

bool Graphics_Model_Editor::edSetName( const char* newName )
{
	m_name = newName;
	return true;
}

static
Graphics_Model_Editor* F_Get_Item_By_Index(
	Graphics_Model_Editor** itemsArray
	, UINT numItems
	, UINT objectIndex
	)
{
	for( UINT i=0; i < numItems; i++ )
	{
		Graphics_Model_Editor* pItem = itemsArray[i];
		if( pItem->m_modelIndex == objectIndex )
		{
			return pItem;
		}
	}
	mxDBG_UNREACHABLE;
	return nil;
}

void Graphics_Model_Editor::edRemoveSelf()
{
	// swap-with-last trick

	Graphics_Model_Editor* pLastItem = F_Get_Item_By_Index(
		m_parent.ToPtr(), m_parent.Num(), m_scene.m_models.Num()-1 );

	m_scene.m_models.RemoveAt_Fast( m_modelIndex );
	pLastItem->m_modelIndex = m_modelIndex;

	m_modelIndex = INDEX_NONE;

	m_parent.Remove( this );
}

void Graphics_Model_Editor::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	APlaceable::edCreateCommonProperties(properties);

	rxModel& model = this->GetModel();

	Properties::For_rxModel( model, "RenderModel", properties );

	{
		EdPropertyList* pBatchListProperties = new EdPropertyList( "Batches", properties );
		pBatchListProperties->SetPropertyFlags(PF_ReadOnly);

		const UINT numBatches = model.m_batches.Num();

		for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
		{
			rxModelBatch & modelBatch = model.m_batches[ iBatch ];

			Properties::For_rxModel_Batch( modelBatch, "Submesh", pBatchListProperties );
		}
	}
}

void Graphics_Model_Editor::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	qtCONNECT(
		args.parent->addAction("Random Action"), SIGNAL(triggered()),
		this, SLOT(slot_Random_Action())
	);
}

void Graphics_Model_Editor::slot_Random_Action()
{
	//
}

void Graphics_Model_Editor::UpdateCachedValues()
{
	rxModel& model = this->GetModel();
	model.m_localToWorld = m_cachedTransform.ToMat4();

	rxAABB_Transform( &model.m_worldAABB, model.m_localAABB, m_cachedTransform );
}

rxModel& Graphics_Model_Editor::GetModel() const
{
	return m_scene.m_models[ m_modelIndex ];
}

const Vec3D& Graphics_Model_Editor::GetOrigin() const
{
	return m_cachedTransform.translation;
}

const Quat& Graphics_Model_Editor::GetOrientation() const
{
	return m_cachedTransform.orientation;
}

const FLOAT Graphics_Model_Editor::GetScale() const
{
	return m_cachedTransform.scaleFactor;
}

void Graphics_Model_Editor::SetOrigin_Impl( const Vec3D& newPos )
{
	m_cachedTransform.translation = newPos;
	this->UpdateCachedValues();
}

void Graphics_Model_Editor::SetOrientation_Impl( const Quat& newRot )
{
	m_cachedTransform.orientation = newRot;
	this->UpdateCachedValues();
}

void Graphics_Model_Editor::SetScale_Impl( const FLOAT newScale )
{
	m_cachedTransform.scaleFactor = newScale;
	this->UpdateCachedValues();
}

void Graphics_Model_Editor::GetWorldAABB( AABB & bbox ) const
{
	rxModel& model = this->GetModel();

	//rxAABB	worldAabb;
	//rxAABB_Transform( &worldAabb, model.m_localAABB, m_cachedTransform );
	rxAABB_To_AABB( model.m_worldAABB, bbox );
}

APlaceable* Graphics_Model_Editor::IsPlaceable()
{
	return this;
}

//AEditable* Graphics_Model_Editor::GetEditorObject()
//{
//	return this;
//}

void Graphics_Model_Editor::Placeable_Draw_Editor_Stuff( const EdDrawContext& drawContext )
{
	//const rxSceneContext &	sceneContext = drawContext.sceneContext;
	//BatchRenderer &	renderer = drawContext.renderer;

	//const rxModel& model = this->GetModel();
	//const float4x4 wvp = model.m_localToWorld * sceneContext.viewProjectionMatrix;
	//renderer.SetTransform( wvp );
}

void Graphics_Model_Editor::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	rxModel& model = this->GetModel();

	rxMesh* mesh = model.m_mesh.ToPtr();
	F_Bind_Mesh_Positions_Only( *mesh, D3DContext );

	HitTesting & hitTesting = GetHitTesting();

	hitTesting.SetTransform( XMMatrixMultiply( model.m_localToWorld, sceneContext.viewProjectionMatrix ) );

	const UINT numBatches = m_hitProxies.Num();

	for( UINT iBatch = 0; iBatch < numBatches; iBatch++ )
	{
		const rxModelBatch & batch = model.m_batches[ iBatch ];
		ModelBatchHitProxy & hitProxy = m_hitProxies[ iBatch ];

		hitTesting.BeginHitProxy( &hitProxy );

		D3DContext->DrawIndexed( batch.indexCount, batch.startIndex, batch.baseVertex );

		hitTesting.EndHitProxy();
	}
}

Graphics_Model_Editor* Graphics_Model_Editor::Static_Create(
	Graphics_Models_List& parent
	, rxModel& model )
{
	Graphics_Model_Editor* pGraphics_Model_Editor =
		new Graphics_Model_Editor( parent, model );

	return pGraphics_Model_Editor;
}

Graphics_Model_Editor* Graphics_Model_Editor::Static_Create_From_Mesh(
	World_Editor& rWorld
	, const SAssetInfo& rAssetInfo
	, const StaticTriangleMeshData* pStaticMeshData
	)
{
	rxModel& rModel = rWorld.m_world->GetRenderWorld().CreateModel();

	GraphicsUtil::F_Create_Render_Model_From_Mesh( rModel, rAssetInfo, pStaticMeshData );

	Graphics_Model_Editor* pGraphics_Model_Editor =
		new Graphics_Model_Editor( rWorld.m_sceneEditor.m_models, rModel );

	gCore.editor->Notify_SelectionChanged( pGraphics_Model_Editor );

	return pGraphics_Model_Editor;
}

/*
-----------------------------------------------------------------------------
	Graphics_Models_List
-----------------------------------------------------------------------------
*/
Graphics_Models_List::Graphics_Models_List( Graphics_Scene_Editor* theParent )
	: m_parent( theParent )
{
}

Graphics_Models_List::~Graphics_Models_List()
{
}

void Graphics_Models_List::PostLoad()
{
	rxRenderWorld& renderWorld = m_parent->m_scene;

	const UINT numModels = renderWorld.m_models.Num();

	this->Reserve( numModels );

	for( UINT iModel = 0; iModel < numModels; iModel++ )
	{
		rxModel& model = renderWorld.m_models[ iModel ];

		Graphics_Model_Editor* pEditor = Graphics_Model_Editor::Static_Create( *this, model );

		Super::Add( pEditor );
	}
}

AEditable* Graphics_Models_List::edGetParent()
{
	return m_parent;
}

const char* Graphics_Models_List::edGetName() const
{
	return "Models";
}

void Graphics_Models_List::edShowContextMenu( const EdShowContextMenuArgs& args )
{
	//Super::edShowContextMenu(args);
}

void Graphics_Models_List::Draw_Editor_Stuff( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	mxUNUSED(viewport);
	mxUNUSED(sceneContext);
}

void Graphics_Models_List::Draw_Hit_Proxies( const EdSceneViewport& viewport, const rxSceneContext& sceneContext )
{
	const UINT numModels = this->Num();

	for( UINT iModel = 0; iModel < numModels; iModel++ )
	{
		Graphics_Model_Editor* pEditor = (*this)[ iModel ];

		pEditor->Draw_Hit_Proxies( viewport, sceneContext );
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
