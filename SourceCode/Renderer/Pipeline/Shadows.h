/*
=============================================================================
	File:	Shadows.h
	Desc:	Shadow rendering.
=============================================================================
*/
#pragma once

#include <Renderer/Renderer.h>
#include <Renderer/Common.h>
#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/Scene/Light.h>

// build config
#include <Renderer/GPU/HLSL/BuildConfig.h>

#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/Model.h>


/*
--------------------------------------------------------------
	rxShadowCastingSet
--------------------------------------------------------------
*/
struct rxShadowCastingSet
{
	TList<rxModel*>		m_objects;

public:
	rxShadowCastingSet();

	// uses GPU::p_build_hw_shadow_map
	void Render( const rxShadowRenderContext& shadowRenderContext, mat4_carg lightViewProjection );

private:
	void UpdatePerObjectConstants( const rxModel& model, mat4_carg lightViewProjection );
};







/*
--------------------------------------------------------------
	rxShadowManager
--------------------------------------------------------------
*/
struct rxShadowManager : public TGlobal< rxShadowManager >
{
	DepthStencil	m_shadowMap;

	rxShadowCastingSet	m_shadowCasters;

	// light-view-projection matrices for each cascade
	float4x4	m_shadowMatrices[NUM_SHADOW_CASCADES];
	float		m_cascadeSplits[NUM_SHADOW_CASCADES];

public:
	rxShadowManager();

	void Initialize();

	ID3D11ShaderResourceView* Process_Directional_Light(
		const rxRenderContext& context,
		const rxParallelLight& light
	);

	ID3D11ShaderResourceView* Prepare_ShadowMap_for_SpotLight(
		const rxRenderContext& context,
		const rxLocalLight& light,
		const rxViewFrustum& lightFrustum,
		mat4_carg lightViewProjection
		);

	static UINT Static_Get_Shadow_Map_Atlas_Size();

public:
	// enable soft spot light shadows
	static bool	g_cvar_spot_light_soft_shadows;

	// enable soft directional light shadows
	static bool	g_cvar_dir_light_soft_shadows;

	// visualize PSSM/CSM splits
	static bool	g_cvar_dir_light_visualize_cascades;
};



struct rxScopedShadowMapSetter
{
	// Get the current render targets and viewports
	dxSaveRenderTargetsAndViewports		rememberRenderTargetsAndDepthStencil;

	// Get the current render states
	dxSaveRasterizerState		rememberRasterizerState;
	dxSaveBlendState			rememberBlendState;
	dxSaveDepthStencilState		rememberDepthStencilState;

public:
	rxScopedShadowMapSetter( ID3D11DeviceContext* pD3DContext, ID3D11DepthStencilView* pShadowMapDSV, UINT shadowMapSquareSize );
	~rxScopedShadowMapSetter();
};


struct OrthographicCamera
{
	float4x4	m_viewProjection;
	float4x4	m_view;
	float4x4	m_projection;

public:
	OrthographicCamera(
		vec4_carg eyePosition, vec4_carg focusPosition, vec4_carg upDirection,
		float minX, float minY,
		float maxX, float maxY,
		float nearZ, float farZ
		)
	{
		Assert(maxX > minX);
		Assert(maxY > minY);

		m_view = XMMatrixLookAtLH( eyePosition, focusPosition, upDirection );
		m_projection = XMMatrixOrthographicOffCenterLH( minX, maxX, minY, maxY, nearZ, farZ );
		m_viewProjection = XMMatrixMultiply( m_view, m_projection );
	}

	void UpdateProjectionMatrix(const XMMATRIX& newProjection)
	{
		m_projection = newProjection;
		m_viewProjection = XMMatrixMultiply( m_view, m_projection );
	}
	void UpdateViewMatrix(const XMMATRIX& newViewMatrix)
	{
		m_view = newViewMatrix;
		m_viewProjection = XMMatrixMultiply( m_view, m_projection );
	}
};


/*
--------------------------------------------------------------
	rxShadowRenderContext
--------------------------------------------------------------
*/
struct rxShadowRenderContext
{
	const rxSceneContext *	s;		// high-level scene information
	ID3D11DeviceContext *	pD3D;	// low-level immediate device context (should be finally wrapped)
};




// Finds the approximate smallest enclosing bounding sphere for a set of points. Based on
// "An Efficient Bounding Sphere", by Jack Ritter.
inline XNA::Sphere ComputeBoundingSphereFromPoints(const XMFLOAT3* points, UINT numPoints, UINT stride)
{
	XNA::Sphere sphere;

	XMASSERT(numPoints > 0);
	XMASSERT(points);

	// Find the points with minimum and maximum x, y, and z
	XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

	MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3(points);

	for(UINT i = 1; i < numPoints; i++)
	{
		XMVECTOR Point = XMLoadFloat3((XMFLOAT3*)((BYTE*)points + i * stride));

		float px = XMVectorGetX(Point);
		float py = XMVectorGetY(Point);
		float pz = XMVectorGetZ(Point);

		if(px < XMVectorGetX(MinX))
			MinX = Point;

		if(px > XMVectorGetX(MaxX))
			MaxX = Point;

		if(py < XMVectorGetY(MinY))
			MinY = Point;

		if(py > XMVectorGetY(MaxY))
			MaxY = Point;

		if(pz < XMVectorGetZ(MinZ))
			MinZ = Point;

		if(pz > XMVectorGetZ(MaxZ))
			MaxZ = Point;
	}

	// Use the min/max pair that are farthest apart to form the initial sphere.
	XMVECTOR DeltaX = XMVectorSubtract(MaxX, MinX);
	XMVECTOR DistX = XMVector3Length(DeltaX);

	XMVECTOR DeltaY = XMVectorSubtract(MaxY, MinY);
	XMVECTOR DistY = XMVector3Length(DeltaY);

	XMVECTOR DeltaZ = XMVectorSubtract(MaxZ, MinZ);
	XMVECTOR DistZ = XMVector3Length(DeltaZ);

	XMVECTOR Center;
	XMVECTOR Radius;

	if(XMVector3Greater(DistX, DistY))
	{
		if(XMVector3Greater(DistX, DistZ))
		{
			// Use min/max x.
			Center = (MaxX + MinX) * 0.5f;
			Radius = DistX * 0.5f;
		}
		else
		{
			// Use min/max z.
			Center = (MaxZ + MinZ) * 0.5f;
			Radius = DistZ * 0.5f;
		}
	}
	else // Y >= X
	{
		if(XMVector3Greater(DistY, DistZ))
		{
			// Use min/max y.
			Center = (MaxY + MinY) * 0.5f;
			Radius = DistY * 0.5f;
		}
		else
		{
			// Use min/max z.
			Center = (MaxZ + MinZ) * 0.5f;
			Radius = DistZ * 0.5f;
		}
	}

	// Add any points not inside the sphere.
	for(UINT i = 0; i < numPoints; i++)
	{
		XMVECTOR Point = XMLoadFloat3((XMFLOAT3*)((BYTE*)points + i * stride));

		XMVECTOR Delta = Point - Center;

		XMVECTOR Dist = XMVector3Length(Delta);

		if(XMVector3Greater(Dist, Radius))
		{
			// Adjust sphere to include the new point.
			Radius = (Radius + Dist) * 0.5f;
			Center += (XMVectorReplicate(1.0f) - Radius * XMVectorReciprocal(Dist)) * Delta;
		}
	}

	XMStoreFloat3(&sphere.Center, Center);
	XMStoreFloat(&sphere.Radius, Radius);

	return sphere;
}

// Represents the 6 planes of a frustum
_DECLSPEC_ALIGN_16_ struct Frustum
{
	XMVECTOR Planes[6];
};

// Calculates the furstum planes given a view * projection matrix
inline void ComputeFrustum(const XMMATRIX& viewProj, Frustum& frustum)
{
	XMVECTOR det;
	XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

	// Corners in homogeneous clip space
	XMVECTOR corners[8] =
	{                                               //                         7--------6
		XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f),      //                        /|       /|
		XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),      //     Y ^               / |      / |
		XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),      //     | _              3--------2  |
		XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),      //     | /' Z           |  |     |  |
		XMVectorSet( 1.0f, -1.0f, 1.0f, 1.0f),      //     |/               |  5-----|--4
		XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),      //     + ---> X         | /      | /
		XMVectorSet( 1.0f,  1.0f, 1.0f, 1.0f),      //                      |/       |/
		XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f),      //                      1--------0
	};

	// Convert to world space
	for(UINT i = 0; i < 8; ++i)
		corners[i] = XMVector3TransformCoord(corners[i], invViewProj);

	// Calculate the 6 planes
	frustum.Planes[0] = XMPlaneFromPoints(corners[0], corners[4], corners[2]);
	frustum.Planes[1] = XMPlaneFromPoints(corners[1], corners[3], corners[5]);
	frustum.Planes[2] = XMPlaneFromPoints(corners[3], corners[2], corners[7]);
	frustum.Planes[3] = XMPlaneFromPoints(corners[1], corners[5], corners[0]);
	frustum.Planes[4] = XMPlaneFromPoints(corners[5], corners[7], corners[4]);
	frustum.Planes[5] = XMPlaneFromPoints(corners[1], corners[0], corners[3]);
}

// Tests a frustum for intersection with a sphere
inline UINT TestFrustumSphere(const Frustum& frustum, const XNA::Sphere& sphere)
{
	XMVECTOR sphereCenter = XMLoadFloat3(&sphere.Center);

	UINT result = 1;
	for(UINT i = 0; i < 6; i++) {
		float distance = XMVectorGetX(XMPlaneDotCoord(frustum.Planes[i], sphereCenter));

		if (distance < -sphere.Radius)
			return 0;
		else if (distance < sphere.Radius)
			result =  1;
	}

	return result;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
