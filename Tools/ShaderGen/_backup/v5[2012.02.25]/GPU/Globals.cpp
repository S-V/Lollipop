/*
=============================================================================
	File:	Globals.cpp
	Desc:	Global renderer constants (GPU constants manager).
=============================================================================
*/

#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Renderer/Core/Scene.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/Globals.h>
#include <Renderer/GPU/HLSL/h_base.h>

namespace GPU
{
	static float4x4 g_viewMatrix = XMMatrixIdentity();
	static float4x4 g_viewProjMatrix = XMMatrixIdentity();

	void UpdatePerFrameConstants( const rxViewport& viewport, FLOAT globalTimeInSeconds )
	{
		GPU::Shared_Globals::PerFrame* pData = Shared_Globals::cb_PerFrame.Map(D3DContext);
		{
			const F4 viewportWidth = viewport.GetWidth();
			const F4 viewportHeight = viewport.GetHeight();

			pData->screenSize_invSize = XMVectorSet( viewportWidth, viewportHeight, 1.0f/viewportWidth, 1.0f/viewportHeight );
			pData->globalTimeInSeconds = globalTimeInSeconds;
		}
		GPU::Shared_Globals::cb_PerFrame.Unmap(D3DContext);

		GPU::Shared_Globals::pointSampler = GPU::SS_Point;
		GPU::Shared_Globals::linearSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::anisotropicSampler = GPU::SS_Aniso;
		GPU::Shared_Globals::colorMapSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::detailMapSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::normalMapSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::specularMapSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::attenuationSampler = GPU::SS_Bilinear;
		GPU::Shared_Globals::cubeMapSampler = GPU::SS_Bilinear;

		//GPU::Shared_Globals::shadowMapSampler = GPU::SS_ShadowMap;
		//GPU::Shared_Globals::shadowMapPCFSampler = GPU::SS_ShadowMapPCF;
		//GPU::Shared_Globals::shadowMapPCFSampler = GPU::SS_ShadowMapPCF_Spot;

		//GPU::Shared_Globals::Set(D3DContext);
	}

	void UpdatePerViewConstants( const rxSceneContext& sceneContext )
	{
		XMVECTOR	det;
		const float4x4	viewMatrix = sceneContext.viewMatrix;
		const float4x4	projectionMatrix = sceneContext.projectionMatrix;
		const float4x4  viewProjectionMatrix = sceneContext.viewProjectionMatrix;
		const float4x4	inverseViewMatrix = sceneContext.invViewMatrix;
		//const float4x4  invViewProjMatrix;
		//const float4x4  invViewProjTex2Clip;

		const F4 nearZ = sceneContext.nearZ;
		const F4 farZ = sceneContext.farZ;
		const F4 invNearZ = 1.0f / nearZ;
		const F4 invFarZ = 1.0f / farZ;

		g_viewMatrix = viewMatrix;
		g_viewProjMatrix = viewProjectionMatrix;


		GPU::Shared_View::PerView* pData = Shared_View::cb_PerView.Map(D3DContext);

		pData->viewMatrix = viewMatrix;
		pData->viewProjectionMatrix = viewProjectionMatrix;
		pData->inverseViewMatrix = inverseViewMatrix;
		pData->projectionMatrix = projectionMatrix;
		pData->inverseProjectionMatrix = XMMatrixInverse(&det,projectionMatrix);

		const FLOAT halfFoVy = 0.5f * sceneContext.fovY;
		const FLOAT tanHalfFoVy = mxTan( halfFoVy );
		const FLOAT tanHalfFoVx = sceneContext.aspectRatio * tanHalfFoVy;

		pData->tanHalfFoV = XMVectorSet( tanHalfFoVx, tanHalfFoVy, 1.0f, 1.0f );

		// calculate frustum corners in view-space
		{
			Vec3D	frustumCornerVS[8];
			GetViewSpaceFrustumCorners( halfFoVy, sceneContext.aspectRatio, nearZ, farZ, frustumCornerVS );

			pData->frustumCornerVS_FarTopLeft		= vec_load1( frustumCornerVS[4] );
			pData->frustumCornerVS_FarTopRight		= vec_load1( frustumCornerVS[5] );
			pData->frustumCornerVS_FarBottomRight	= vec_load1( frustumCornerVS[6] );
			pData->frustumCornerVS_FarBottomLeft	= vec_load1( frustumCornerVS[7] );
		}

		pData->depthClipPlanes = XMVectorSet( nearZ, farZ, invNearZ, invFarZ );

		GPU::Shared_View::cb_PerView.Unmap(D3DContext);

		//GPU::Shared_View::Set(D3DContext);
	}

	void UpdatePerObjectConstants( mat4_carg worldMatrix )
	{
		const float4x4	worldViewMatrix = XMMatrixMultiply(worldMatrix,g_viewMatrix);
		const float4x4	worldViewProjectionMatrix = XMMatrixMultiply(worldMatrix,g_viewProjMatrix);

		GPU::Shared_Object::PerObject* pData = Shared_Object::cb_PerObject.Map(D3DContext);
		{
			pData->worldMatrix = worldMatrix;
			pData->worldViewMatrix = worldViewMatrix;
			pData->worldViewProjectionMatrix = worldViewProjectionMatrix;
		}
		GPU::Shared_Object::cb_PerObject.Unmap(D3DContext);
		//GPU::Shared_Object::Set(D3DContext);
	}

}//namespace GPU

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
