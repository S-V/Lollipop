/*
=============================================================================
	File:	SceneView.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include <Renderer/Scene/RenderWorld.h>
#include <Renderer/Core/SceneView.h>

rxSceneContext::rxSceneContext( const rxView& view, rxRenderWorld& scene )
	: nearZ( view.nearZ ), farZ( view.farZ )
	, fovY( view.fovY )
	, aspectRatio( view.aspectRatio )
	, scene( scene )
{
	Assert(view.isOk());

	as_matrix4(this->viewMatrix).BuildLookAtLH(
		view.origin,
		view.right,
		view.up,
		view.look
	);

	as_matrix4(this->projectionMatrix).BuildPerspectiveLH(
		view.fovY,
		view.aspectRatio,
		view.nearZ,
		view.farZ
	);

	this->viewProjectionMatrix = XMMatrixMultiply( this->viewMatrix, this->projectionMatrix );

	this->invViewMatrix = view.GetInverseViewMatrix();

	XMVECTOR det;
	this->invViewProjectionMatrix = XMMatrixInverse( &det, this->viewProjectionMatrix );

	this->frustum.Build( &invViewMatrix, &projectionMatrix );

	//this->view.Set( view );
}

void rxSceneContext::DbgCheckValid() const
{
	//
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
