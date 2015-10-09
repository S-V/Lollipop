/*
=============================================================================
	File:	SceneView.h
	Desc:	Graphics scene view.
	ToDo:	precached values should be const
			(or private, accessed via getters)
=============================================================================
*/
#pragma once

#include <Core/Entity/Components.h>
#include <Core/Object.h>
#include <Core/Serialization.h>
#include <Core/Editor.h>

#include <Renderer/Renderer.h>

//
//	rxSceneContext - is a view from which scenes are rendered.
//	contains camera view parameters and scene information
//
struct rxSceneContext
{
	rxViewFrustum	frustum;

	// precached values
	float4x4	viewMatrix;
	float4x4	projectionMatrix;
	float4x4  	viewProjectionMatrix;
	float4x4	invViewMatrix;
	float4x4	invViewProjectionMatrix;

	rxRenderWorld &	scene;		// The scene enclosing this view.

	const float1	nearZ, farZ;// Near and far clipping planes.
	const float1	fovY;		// Vertical field of view angle, in radians.
	const float1	aspectRatio;	// projection ratio (ratio between width and height of view surface)

public:
	rxSceneContext( const rxView& view, rxRenderWorld& scene );

public:
	FORCEINLINE const Vec3D& GetOrigin() const
	{
		return frustum.GetOrigin();
		//return as_matrix4( this->invViewMatrix )[3].ToVec3();
	}
	FORCEINLINE const Vec3D& GetCameraRightVector() const
	{
		return as_matrix4( this->invViewMatrix )[0].ToVec3();
	}
	FORCEINLINE const Vec3D& GetCameraUpVector() const
	{
		return as_matrix4( this->invViewMatrix )[1].ToVec3();
	}
	FORCEINLINE const Vec3D& GetLookDirection() const
	{
		return as_matrix4( this->invViewMatrix )[2].ToVec3();
	}

	void DbgCheckValid() const;
};

struct rxRayCastInput
{
	Vec3D		m_start;
	Vec3D		m_end;
};
struct rxRayCastResult
{
	//Vec3D		m_position;		// hit point position in coordinate system of the hit entity
	//Vec3D		m_normal;		// collision plane normal in coordinate system of the hit entity
	FLOAT		m_distance;		// (0.0f if didn't hit anything)
	rxRenderEntity *	m_entityHit;	// the entity that was hit (null if didn't hit anything)
	UINT		m_entitySubset;	// submesh or bone index, INDEX_NONE if undefined
};


typedef void F_EntityIterator( rxRenderEntity* pEntity, void* pData );


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
