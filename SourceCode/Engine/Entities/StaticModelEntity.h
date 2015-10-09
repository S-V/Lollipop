/*
=============================================================================
	File:	StaticModelEntity.h
	Desc:	
=============================================================================
*/
#pragma once

#include <Renderer/Core/Mesh.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/Model.h>

#include <Engine/Entity.h>


/*
-----------------------------------------------------------------------------
	StaticModelEntity

	is a renderable object that represents a static model.
	contains transform, geometry and batches which reference materials
-----------------------------------------------------------------------------
*/
class StaticModelEntity : public AEntity, public rxRenderEntity
{
	rxModel			m_model;		// graphics model for rendering
	rxTransform		m_transform;	// local-to-world transform from which the world matrix is calculated
	rxAABB			m_worldAABB;	// world-space bounds for view frustum culling (changes only when local bounds or transform changes)

public:
	mxDECLARE_CLASS( StaticModelEntity, AEntity );
	mxDECLARE_REFLECTION;

	StaticModelEntity();
	StaticModelEntity(_FinishedLoadingFlag);
	virtual ~StaticModelEntity();

public:	//=-- AEntity

	virtual void Initialize( const SEntitySpawnContext& initContext );

	// called after entity is removed from world
	virtual void Shutdown();

	// real-time updates and thinking
	virtual void Tick( const SEntityUpdateContext& updateContext ) {}

	virtual rxRenderEntity* GetVisuals();

public:	//=-- rxRenderEntity

	virtual void rfRenderShadowDepth( const rxShadowRenderContext& context ) override;

	// generates drawable surfaces, etc.
	virtual void rfSubmitBatches( const rxEntityViewContext& context ) override;

public:
	friend class Static_Model_Editor;

	// recalculates precached data
	void UpdateDerivedValues();
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
