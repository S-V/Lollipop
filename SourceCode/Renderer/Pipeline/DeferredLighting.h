#pragma once


#include <Renderer/Common.h>
//#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>
#include <Renderer/Core/SceneView.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/Scene/Light.h>

/*
=============================================================================
	Light shape meshes (for deferred lighting).
=============================================================================
*/

// vertex type used for drawing light shapes (for rendering local lights).
struct LSVertex : public GPU::Vertex_P3f
{
	FORCEINLINE LSVertex()
	{}
	FORCEINLINE LSVertex( const Vec3D& pos )
	{
		this->xyz = pos;
	}
	FORCEINLINE LSVertex( FLOAT x, FLOAT y, FLOAT z )
	{
		this->xyz.Set( x,y,z );
	}
};

enum { MAX_LIGHT_SHAPE_VERTICES = 1024*4 };
enum { MAX_LIGHT_SHAPE_INDICES = 4096*4 };

typedef TStaticList
<
	LSVertex,
	MAX_LIGHT_SHAPE_VERTICES
>
LSVertexList;

typedef TStaticList
<
	U2,
	MAX_LIGHT_SHAPE_INDICES
>
LSIndexList;

//
//	rxLightShape - uses triangle lists and 16-bit indices for rendering.
//
class rxLightShape {
public:
	rxLightShape();
	~rxLightShape();

	void Clear();

	UINT GetVertexCount() const { return m_numVertices; }
	UINT GetIndexCount() const { return m_numIndices; }

	FORCEINLINE void Bind( ID3D11DeviceContext* context )
	{
		m_vertices.Bind( context );
		m_indices.Bind( context );
		rxSetTopology( context, EPrimitiveType::PT_TriangleList );
	}

	FORCEINLINE void Draw( ID3D11DeviceContext* context )
	{
		context->DrawIndexed( m_numIndices, 0, 0 );
	}

	FORCEINLINE void SetAndDraw( ID3D11DeviceContext* context )
	{
		this->Bind( context );
		this->Draw( context );
	}

protected_internal:
	void CreateBuffers( const LSVertexList& vertices, const LSIndexList& indices );

protected:
	TVertexSource< LSVertex >	m_vertices;
	GrIndexBuffer		m_indices;	// triangle list

	UINT 	m_numVertices;
	UINT 	m_numIndices;
};

//
//	rxLightShape_Sphere
//
class rxLightShape_Sphere : public rxLightShape {
public:
	rxLightShape_Sphere();
	~rxLightShape_Sphere();

	void Build( FLOAT radius, UINT numSlices, UINT numStacks );

private:
	void BuildStacks( LSVertexList & vertices, LSIndexList & indices );

private:
	FLOAT	m_radius;

	UINT	m_numSlices;
	UINT	m_numStacks;
};

//
//	rxLightShape_Cylinder
//
class rxLightShape_Cylinder : public rxLightShape {
public:
	rxLightShape_Cylinder();
	~rxLightShape_Cylinder();

	// NOTE: untransformed cone is centered around the origin
	// and its axis points along (-1.0f * Y)

	void Build( FLOAT topRadius, FLOAT bottomRadius, FLOAT height,
		UINT numSlices, UINT numStacks,
		const Matrix4* shapeTransform = nil );

private:
	void BuildStacks( LSVertexList & vertices, LSIndexList & indices );
	void BuildBottomCap( LSVertexList & vertices, LSIndexList & indices );
	void BuildTopCap( LSVertexList & vertices, LSIndexList & indices );

private:
	FLOAT	m_topRadius;
	FLOAT	m_bottomRadius;
	FLOAT	m_height;

	UINT	m_numSlices;
	UINT	m_numStacks;
};

//
//	rxLightShape_UnitCube
//
class rxLightShape_UnitCube : public rxLightShape {
public:
	rxLightShape_UnitCube();
	~rxLightShape_UnitCube();

	void Build();
};


// Light shapes for drawing local lights.
struct LightShapes
{
	rxLightShape_Sphere		unitPointLightShape;	// unit sphere
	rxLightShape_Cylinder	unitSpotLightShape;	// apex - in Y-direction (top radius close to 0), bottom radius = 1, height = 1
	rxLightShape_UnitCube	unitCubeLightShape;

public:
	void CreateMeshes();
};



/*
=======================================================================
	computes part of local-to-world matrix for drawing spot light shape
=======================================================================
*/
static inline
float4x4 CalcSpotLightScale( const rxLocalLight* pLight )
{
	const FLOAT bottomRadius = pLight->CalcBottomRadius();
	const FLOAT lightRadius = pLight->GetRadius();
	return XMMatrixSet(
		bottomRadius,	0.0f,			0.0f,			0.0f,
		0.0f,			bottomRadius,	0.0f,			0.0f,
		0.0f,			0.0f,			lightRadius,	0.0f,
		0.0f,			0.0f,			0.0f,			1.0f
	);
}

static inline
float4x4 CalcSpotLightWorldMatrix( const rxLocalLight* pLight )
{
#if 0
	const float4	WORLD_UP = g_XMIdentityR1;
	const float4	WORLD_RIGHT = g_XMIdentityR0;

	float4	lightDir = pLight->m_spotDirection;
	float4	rightDir = XMVector3Cross( WORLD_UP, lightDir );

	float	rightDirInvLen = XMVectorGetX( XMVector3ReciprocalLengthEst( rightDir ) );

	// prevent singularities when light direction is nearly parallel to 'up' direction
	if( rightDirInvLen >= (1.0f / 1e-3f) ) {
		rightDir = WORLD_RIGHT;
	} else {
		rightDir = XMVectorScale( rightDir, rightDirInvLen );
	}

	const float4	upDir = XMVector3Normalize( XMVector3Cross( lightDir, rightDir ) );

	return XMMATRIX(
		XMVectorSetW( rightDir,	0.0f ),
		XMVectorSetW( upDir,	0.0f ),
		XMVectorSetW( lightDir,	0.0f ),
		pLight->m_position
		);
#else

	return BuildBasisVectors( pLight->m_position, pLight->m_spotDirection );

#endif
}


static inline
float4x4 CalcSpotLightProjectionMatrix( const rxLocalLight* pLight )
{
	FLOAT fovY = pLight->GetOuterConeAngle();
	FLOAT aspect = 1.0f;
	FLOAT farZ = pLight->GetRadius();
	FLOAT nearZ = minf( 0.1f, farZ * 0.5f );

	//XMMatrixPerspectiveFovLH(
	//	)
	float4x4	projectionMatrix;
	as_matrix4( projectionMatrix ).BuildPerspectiveLH( fovY, aspect, nearZ, farZ );
	return projectionMatrix;
}



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
