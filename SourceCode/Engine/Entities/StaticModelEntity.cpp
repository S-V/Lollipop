/*
=============================================================================
	File:	StaticModelEntity.cpp
	Desc:	
=============================================================================
*/
#include <Engine_PCH.h>
#pragma hdrstop
#include <Engine.h>

#include <Engine/Entities/StaticModelEntity.h>

/*
-----------------------------------------------------------------------------
	StaticModelEntity
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS( StaticModelEntity );

mxBEGIN_REFLECTION(StaticModelEntity)
	mxMEMBER_FIELD( m_model )
	mxMEMBER_FIELD2( m_transform, Local_To_World_Transform )
	mxMEMBER_FIELD2( m_worldAABB, AABB_In_World_Space )
mxEND_REFLECTION

StaticModelEntity::StaticModelEntity()
{
	m_transform.SetIdentity();
	rxAABB_Infinity( m_worldAABB );
}

StaticModelEntity::StaticModelEntity(_FinishedLoadingFlag)
{
}

StaticModelEntity::~StaticModelEntity()
{
}

void StaticModelEntity::Initialize( const SEntitySpawnContext& initContext )
{
	Super::Initialize( initContext );
}

void StaticModelEntity::Shutdown()
{
	Super::Shutdown();
}

rxRenderEntity* StaticModelEntity::GetVisuals()
{
	return this;
}

void StaticModelEntity::rfRenderShadowDepth( const rxShadowRenderContext& context )
{
	UNDONE;
	//m_model.RenderShadowDepth( context );
}

void StaticModelEntity::rfSubmitBatches( const rxEntityViewContext& context )
{
	UNDONE;
	//m_model.SubmitBatches( context );
}

void StaticModelEntity::UpdateDerivedValues()
{
	m_transform.orientation.Normalize();

	m_model.m_localToWorld = m_transform.ToMat4();
	rxAABB_Transform( &m_worldAABB, m_model.m_localAABB, m_transform );
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
