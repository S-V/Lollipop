/*
=============================================================================
	File:	BatchRenderer.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/DX11/DX11Helpers.h>

#include <Renderer/Common.h>
#include <Renderer/Core/Font.h>
#include <Renderer/Core/Geometry.h>
#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>
#include <Renderer/Util/BatchRenderer.h>


FORCEINLINE R8G8B8A8 FColor_to_RGBA32( const FColor& theColor )
{
	R8G8B8A8	rgba;
	rgba.asU32 	= theColor.ToRGBA32();
	return rgba;
}

namespace
{
	typedef GPU::Vertex_P3f_TEX2f_COL4Ub	Vertex;
	typedef U4	Index;

	struct BatchRendererData
	{
		TList< Vertex >	batchedVertices;
		TList< Index >	batchedIndices;

		TDynamicVertexBuffer< Vertex >	dynamicVB;
		TDynamicIndexBuffer< Index >	dynamicIB;

		float4x4	currentTransform;

		D3D11_PRIMITIVE_TOPOLOGY	currentTopology;
		TPtr<StateBlock>			currentState;
		ID3D11ShaderResourceViewPtr	currentTexture;

		TPtr<AHitTesting>	pHitTesting;

	public:
		BatchRendererData()
		{
			currentTransform = XMMatrixIdentity();
			currentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
			currentState = &GPU::DebugPrimitives;
			pHitTesting = nil;
		}
	};

	TPtr< BatchRendererData >	self;

	void SetBatchState( BatchRenderer* theRenderer, D3D11_PRIMITIVE_TOPOLOGY newPrimType )
	{
		const bool	bStateChanged = (self->currentTopology != newPrimType)
			;
		if( bStateChanged )
		{
			theRenderer->Flush();

			self->currentTopology = newPrimType;
		}
	}

}//namespace

/*
-----------------------------------------------------------------------------
	BatchRenderer
-----------------------------------------------------------------------------
*/
BatchRenderer::BatchRenderer()
{

}

BatchRenderer::~BatchRenderer()
{

}

void BatchRenderer::Initialize()
{
	mxSTATIC_IN_PLACE_CTOR_X( self, BatchRendererData );

	const SizeT maxBufferSize = (1<<16);	// 64 KiB
	const UINT maxVertices = maxBufferSize / sizeof Vertex;
	const UINT maxIndices = maxBufferSize / sizeof Index;

	self->batchedVertices.Reserve( maxVertices );
	self->batchedIndices.Reserve( maxIndices );

	self->dynamicVB.Setup( maxVertices );
	self->dynamicIB.Setup( maxIndices );
}

void BatchRenderer::Shutdown()
{
	self->dynamicVB.Close();
	self->dynamicIB.Close();

	self->batchedVertices.Empty();
	self->batchedIndices.Empty();

	self.Destruct();
}

void BatchRenderer::SetTransform( mat4_carg newTransform )
{
	if( !MatricesAreEqual( self->currentTransform, newTransform ) )
	{
		this->Flush();
		self->currentTransform = newTransform;
	}
}

float4x4 BatchRenderer::GetTransform() const
{
	return self->currentTransform;
}

void BatchRenderer::SetState( StateBlock& newState )
{
	AssertPtr(&newState);
	if( self->currentState != &newState )
	{
		this->Flush();
		self->currentState = &newState;
	}
}

void BatchRenderer::SetHitTesting( AHitTesting* pHitTesting )
{
	if( self->pHitTesting != pHitTesting )
	{
		this->Flush();
		self->pHitTesting = pHitTesting;
	}
}

AHitTesting* BatchRenderer::IsHitTesting() const
{
	return self->pHitTesting.Ptr;
}

void BatchRenderer::DrawLine3D(
	FLOAT x1, FLOAT y1, FLOAT z1,
	FLOAT x2, FLOAT y2, FLOAT z2,
	const FColor& startColor,
	const FColor& endColor
)
{
	SetBatchState( this, D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	const UINT iBaseVertex = self->batchedVertices.Num();
	Vertex & v0 = self->batchedVertices.Add();
	Vertex & v1 = self->batchedVertices.Add();

	v0.xyz.x 	= x1;
	v0.xyz.y 	= y1;
	v0.xyz.z 	= z1;
	v0.rgba 	= FColor_to_RGBA32( startColor );

	v1.xyz.x 	= x2;
	v1.xyz.y 	= y2;
	v1.xyz.z 	= z2;
	v1.rgba 	= FColor_to_RGBA32( endColor );

	self->batchedIndices.Add( iBaseVertex + 0 );
	self->batchedIndices.Add( iBaseVertex + 1 );
}

void BatchRenderer::DrawLine3D(
	const Vec3D& start,
	const Vec3D& end,
	const FColor& startColor,
	const FColor& endColor
)
{
	this->DrawLine3D(
		start.x, start.y, start.z,
		end.x, end.y, end.z,
		startColor,
		endColor
	);
}

void BatchRenderer::DrawTriangle3D(
	const Vec3D& a, const Vec3D& b, const Vec3D& c,
	const FColor& color
)
{
#if 0
	DrawLine3D( a, b, color, color );
	DrawLine3D( b, c, color, color );
	DrawLine3D( c, a, color, color );
#else
	SetBatchState( this, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );

	Vertex & v0 = self->batchedVertices.Add();
	Vertex & v1 = self->batchedVertices.Add();
	Vertex & v2 = self->batchedVertices.Add();
	Vertex & v3 = self->batchedVertices.Add();

	v0.xyz.x 	= a.x;
	v0.xyz.y 	= a.y;
	v0.xyz.z 	= a.z;
	v0.rgba.asU32 	= color.ToRGBA32();

	v1.xyz.x 	= b.x;
	v1.xyz.y 	= b.y;
	v1.xyz.z 	= b.z;
	v1.rgba.asU32 	= color.ToRGBA32();

	v2.xyz.x 	= c.x;
	v2.xyz.y 	= c.y;
	v2.xyz.z 	= c.z;
	v2.rgba.asU32 	= color.ToRGBA32();

	v3.xyz.x 	= a.x;
	v3.xyz.y 	= a.y;
	v3.xyz.z 	= a.z;
	v3.rgba.asU32 	= color.ToRGBA32();
#endif
}

void BatchRenderer::DrawDashedLine(
	const Vec3D& start,
	const Vec3D& end,
	const FLOAT dashSize,
	const FColor& startColor,
	const FColor& endColor
)
{
	mxSWIPED("Unreal Engine 3");

	Vec3D lineDir = end - start;
	FLOAT lineLeft = (end - start).LengthFast();
	lineDir /= lineLeft;

	while(lineLeft > 0.f)
	{
		Vec3D DrawStart = end - ( lineLeft * lineDir );
		Vec3D DrawEnd = DrawStart + ( minf(dashSize, lineLeft) * lineDir );

		this->DrawLine3D( DrawStart, DrawEnd, startColor, endColor );

		lineLeft -= 2*dashSize;
	}
}

void BatchRenderer::DrawAxes( FLOAT l )
{
#if 0
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		l*1.0f,0.0f,0.0f,
		FColor::WHITE,
		FColor::RED
	);
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		0.0f,l*1.0f,0.0f,
		FColor::WHITE,
		FColor::GREEN
	);
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		0.0f,0.0f,l*1.0f,
		FColor::WHITE,
		FColor::BLUE
	);
#elif 0
	// Draw axes.

	// Draw the x-axis in red
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		l*1.0f,0.0f,0.0f,
		FColor::RED,
		FColor::RED
	);
	// Draw the y-axis in green
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		0.0f,l*1.0f,0.0f,
		FColor::GREEN,
		FColor::GREEN
	);
	// Draw the z-axis in blue
	this->DrawLine3D(
		0.0f,0.0f,0.0f,
		0.0f,0.0f,l*1.0f,
		FColor::BLUE,
		FColor::BLUE
	);
#else
	this->DrawArrow(
		MatrixFromAxes(
			Vec3D::vec3_unit_x,
			Vec3D::vec3_unit_y,
			Vec3D::vec3_unit_z
		),
		FColor::RED, l*1.0f, l*0.1f
	);
	this->DrawArrow(
		MatrixFromAxes(
			Vec3D::vec3_unit_y,
			Vec3D::vec3_unit_z,
			Vec3D::vec3_unit_x
		),
		FColor::GREEN, l*1.0f, l*0.1f
	);
	this->DrawArrow(
		MatrixFromAxes(
			Vec3D::vec3_unit_z,
			Vec3D::vec3_unit_x,
			Vec3D::vec3_unit_y
		),
		FColor::BLUE, l*1.0f, l*0.1f
	);
#endif
}

void BatchRenderer::DrawArrow( const Matrix4& arrowTransform, const FColor& color, FLOAT arrowLength, FLOAT headSize )
{
	mxSWIPED("Unreal Engine 3");

	const Vec3D	origin = arrowTransform.GetTranslation();
	const Vec3D endPoint = arrowTransform.TransformVector(Vec3D( arrowLength, 0.0f, 0.0f ));
	const FLOAT headScale = 0.5f;

	const FColor& startColor = FColor::WHITE;
	const FColor& endColor = color;

	this->DrawLine3D( origin, endPoint, startColor, endColor );
	this->DrawLine3D( endPoint, arrowTransform.TransformVector(Vec3D( arrowLength-headSize, +headSize*headScale, +headSize*headScale )), startColor, endColor );
	this->DrawLine3D( endPoint, arrowTransform.TransformVector(Vec3D( arrowLength-headSize, +headSize*headScale, -headSize*headScale )), startColor, endColor );
	this->DrawLine3D( endPoint, arrowTransform.TransformVector(Vec3D( arrowLength-headSize, -headSize*headScale, +headSize*headScale )), startColor, endColor );
	this->DrawLine3D( endPoint, arrowTransform.TransformVector(Vec3D( arrowLength-headSize, -headSize*headScale, -headSize*headScale )), startColor, endColor );
}

void BatchRenderer::DrawInfiniteGrid( const Vec3D& eyePos, FLOAT height, const FColor& color )
{
	const FLOAT gridHalfSizeX = 100.0f, gridHalfSizeZ = 100.0f;
	const UINT numGridCells = 50;
	const FLOAT invNumGridCells = 1.0f / numGridCells;
	const FLOAT dx = gridHalfSizeX * 2 * invNumGridCells;
	const FLOAT dz = gridHalfSizeZ * 2 * invNumGridCells;

	//Vec3D gridCenter(
	//	mxSnapFloat( eyePos.x, invNumGridCells ),
	//	height,
	//	mxSnapFloat( eyePos.z, invNumGridCells )
	//);

	// Set start position.

	FLOAT x = -gridHalfSizeX;
	FLOAT z = -gridHalfSizeZ;

	for( int i = 0; i < numGridCells; i++ )
	{
		DrawLine3D(
			x, height, -gridHalfSizeZ,
			x, height,  gridHalfSizeZ,
			color, color
		);
		x += dx;
	}
	for( int i = 0; i < numGridCells; i++ )
	{
		DrawLine3D(
			-gridHalfSizeX, height,  z,
			+gridHalfSizeX, height,  z,
			color, color
		);
		z += dz;
	}
}

mxSWIPED("DXSDK, June 2010, XNA collision detection sample");
void BatchRenderer::DrawGrid( const Vec3D& XAxis, const Vec3D& YAxis, const Vec3D& Origin, int iXDivisions, int iYDivisions, const FColor& color )
{
//	HRESULT hr;

	iXDivisions = Max( 1, iXDivisions );
	iYDivisions = Max( 1, iYDivisions );

	// build grid geometry
//	INT iLineCount = iXDivisions + iYDivisions + 2;
	//assert( (2*iLineCount) <= MAX_VERTS );

	XMVECTOR vX = XMLoadFloat3( &as_float3(XAxis) );
	XMVECTOR vY = XMLoadFloat3( &as_float3(YAxis) );
	XMVECTOR vOrigin = XMLoadFloat3( &as_float3(Origin) );

	for( INT i = 0; i <= iXDivisions; i++ )
	{
		FLOAT fPercent = ( FLOAT )i / ( FLOAT )iXDivisions;
		fPercent = ( fPercent * 2.0f ) - 1.0f;
		XMVECTOR vScale = XMVectorScale( vX, fPercent );
		vScale = XMVectorAdd( vScale, vOrigin );

		XMVECTOR vA, vB;
		vA = XMVectorSubtract( vScale, vY );
		vB = XMVectorAdd( vScale, vY );

		DrawLine3D( as_vec4(vA).ToVec3(), as_vec4(vB).ToVec3(), color, color );
	}

//	INT iStartIndex = ( iXDivisions + 1 ) * 2;
	for( INT i = 0; i <= iYDivisions; i++ )
	{
		FLOAT fPercent = ( FLOAT )i / ( FLOAT )iYDivisions;
		fPercent = ( fPercent * 2.0f ) - 1.0f;
		XMVECTOR vScale = XMVectorScale( vY, fPercent );
		vScale = XMVectorAdd( vScale, vOrigin );
		
		XMVECTOR vA, vB;
		vA = XMVectorSubtract( vScale, vX );
		vB = XMVectorAdd( vScale, vX );

		DrawLine3D( as_vec4(vA).ToVec3(), as_vec4(vB).ToVec3(), color, color );
	}
}

void BatchRenderer::DrawCube( const float4x4& worldMatrix, const FColor& color )
{
	static const XMVECTOR verts[8] =
    {
        { -1, -1, -1, 0 },
        { 1, -1, -1, 0 },
        { 1, -1, 1, 0 },
        { -1, -1, 1, 0 },
        { -1, 1, -1, 0 },
        { 1, 1, -1, 0 },
        { 1, 1, 1, 0 },
        { -1, 1, 1, 0 }
    };
	const UINT* edgeIndices = AABB::GetEdges();

    // copy to vertex buffer
    //assert( 8 <= MAX_VERTS );

	XMFLOAT3 transformedVerts[8];
	for( int i=0; i < 8; ++i )
	{
		XMVECTOR v = XMVector3Transform( verts[i], worldMatrix );
		XMStoreFloat3( &transformedVerts[i], v );
	}

	for( int iEdge = 0; iEdge < AABB::NUM_EDGES; iEdge++ )
	{
		XMFLOAT3 & start = transformedVerts[ edgeIndices[iEdge*2] ];
		XMFLOAT3 & end = transformedVerts[ edgeIndices[iEdge*2 + 1] ];
		DrawLine3D( as_vec3(start), as_vec3(end), color, color );
	}
}

void BatchRenderer::DrawAABB( const rxAABB& box, const FColor& color )
{
	XMMATRIX matWorld = XMMatrixScaling( box.Extents.x, box.Extents.y, box.Extents.z );
	XMVECTOR position = XMLoadFloat3( &box.Center );
	matWorld.r[3] = XMVectorSelect( matWorld.r[3], position, XMVectorSelectControl( 1, 1, 1, 0 ) );

	DrawCube( matWorld, color );
}

void BatchRenderer::DrawOBB( const rxOOBB& box, const FColor& color )
{
	XMMATRIX matWorld = XMMatrixRotationQuaternion( XMLoadFloat4( &box.Orientation ) );
	XMMATRIX matScale = XMMatrixScaling( box.Extents.x, box.Extents.y, box.Extents.z );
	matWorld = XMMatrixMultiply( matScale, matWorld );
	XMVECTOR position = XMLoadFloat3( &box.Center );
	matWorld.r[3] = XMVectorSelect( matWorld.r[3], position, XMVectorSelectControl( 1, 1, 1, 0 ) );

	DrawCube( matWorld, color );
}

void BatchRenderer::DrawFrustum( const XNA::Frustum& frustum, const FColor& color )
{
	// compute corner points
	XMVECTOR Origin = XMVectorSet( frustum.Origin.x, frustum.Origin.y, frustum.Origin.z, 0 );

	FLOAT Near = frustum.Near;
	FLOAT Far = frustum.Far;
	FLOAT RightSlope = frustum.RightSlope;
	FLOAT LeftSlope = frustum.LeftSlope;
	FLOAT TopSlope = frustum.TopSlope;
	FLOAT BottomSlope = frustum.BottomSlope;

	XMFLOAT3 CornerPoints[8];
	CornerPoints[0] = XMFLOAT3( RightSlope * Near, TopSlope * Near, Near );
	CornerPoints[1] = XMFLOAT3( LeftSlope * Near, TopSlope * Near, Near );
	CornerPoints[2] = XMFLOAT3( LeftSlope * Near, BottomSlope * Near, Near );
	CornerPoints[3] = XMFLOAT3( RightSlope * Near, BottomSlope * Near, Near );

	CornerPoints[4] = XMFLOAT3( RightSlope * Far, TopSlope * Far, Far );
	CornerPoints[5] = XMFLOAT3( LeftSlope * Far, TopSlope * Far, Far );
	CornerPoints[6] = XMFLOAT3( LeftSlope * Far, BottomSlope * Far, Far );
	CornerPoints[7] = XMFLOAT3( RightSlope * Far, BottomSlope * Far, Far );

	XMVECTOR Orientation = XMLoadFloat4( &frustum.Orientation );
	XMMATRIX Mat = XMMatrixRotationQuaternion( Orientation );
	for( UINT i = 0; i < 8; i++ )
	{
		XMVECTOR Result = XMVector3Transform( XMLoadFloat3( &CornerPoints[i] ), Mat );
		Result = XMVectorAdd( Result, Origin );
		XMStoreFloat3( &CornerPoints[i], Result );
	}

	// copy to vertex buffer
	//Assert( (12 * 2) <= MAX_VERTS );

#define DBG_DRAW_LINE(pointA,pointB)	\
	DrawLine3D( pointA.x,pointA.y,pointA.z, pointB.x,pointB.y,pointB.z,	\
	color, color );

	DBG_DRAW_LINE(CornerPoints[0],CornerPoints[1]);
	DBG_DRAW_LINE(CornerPoints[1],CornerPoints[2]);
	DBG_DRAW_LINE(CornerPoints[2],CornerPoints[3]);
	DBG_DRAW_LINE(CornerPoints[3],CornerPoints[0]);

	DBG_DRAW_LINE(CornerPoints[0],CornerPoints[4]);
	DBG_DRAW_LINE(CornerPoints[1],CornerPoints[5]);
	DBG_DRAW_LINE(CornerPoints[2],CornerPoints[6]);
	DBG_DRAW_LINE(CornerPoints[3],CornerPoints[7]);

	DBG_DRAW_LINE(CornerPoints[4],CornerPoints[5]);
	DBG_DRAW_LINE(CornerPoints[5],CornerPoints[6]);
	DBG_DRAW_LINE(CornerPoints[6],CornerPoints[7]);
	DBG_DRAW_LINE(CornerPoints[7],CornerPoints[4]);

#undef DBG_DRAW_LINE

}

void BatchRenderer::DrawViewFrustum( const ViewFrustum& frustum, const FColor& color )
{
	Vec3D	frustumCorners[8];
	frustum.CalculateCornerPoints( frustumCorners );

#if 1

#define DBG_DRAW_LINE(pointA,pointB)	\
	DrawLine3D( pointA, pointB, color, color );

	DBG_DRAW_LINE(frustumCorners[0],frustumCorners[1]);
	DBG_DRAW_LINE(frustumCorners[2],frustumCorners[3]);
	DBG_DRAW_LINE(frustumCorners[4],frustumCorners[5]);
	DBG_DRAW_LINE(frustumCorners[6],frustumCorners[7]);


	// far plane
	DBG_DRAW_LINE(frustumCorners[0],frustumCorners[4]);
	DBG_DRAW_LINE(frustumCorners[0],frustumCorners[2]);
	DBG_DRAW_LINE(frustumCorners[2],frustumCorners[6]);
	DBG_DRAW_LINE(frustumCorners[4],frustumCorners[6]);

	// near plane
	DBG_DRAW_LINE(frustumCorners[1],frustumCorners[5]);
	DBG_DRAW_LINE(frustumCorners[3],frustumCorners[7]);
	DBG_DRAW_LINE(frustumCorners[3],frustumCorners[1]);
	DBG_DRAW_LINE(frustumCorners[7],frustumCorners[5]);

#undef DBG_DRAW_LINE

#else

	rxOOBB	obb;
	XNA::ComputeBoundingOrientedBoxFromPoints(
		&obb,
		8,
		&as_float3(frustumCorners[0]),
		sizeof(frustumCorners[0])
	);
	mxSTATIC_ASSERT(sizeof(frustumCorners[0]) == sizeof(XMFLOAT3));

	DrawOBB(obb,color);
#endif
}

void BatchRenderer::DrawCircle( const Vec3D& origin,const Vec3D& vX,const Vec3D& vY, const FColor& color, FLOAT radius, INT numSides )
{
	FLOAT	fAngleDelta = MX_TWO_PI / (FLOAT)numSides;
	Vec3D	vPrevVertex = origin + vX * radius;

	for( INT iSide = 0; iSide < numSides; iSide++ )
	{
		Vec3D	v = origin +
			(vX * mxCos(fAngleDelta * (iSide + 1)) + vY * mxSin(fAngleDelta * (iSide + 1))) * radius;

		DrawLine3D(vPrevVertex,v,color,color);

		vPrevVertex = v;
	}
}

void BatchRenderer::DrawSolidTriangle3D(
	const Vec3D& a, const Vec3D& b, const Vec3D& c,
	const FColor& color
	)
{
	const U4 rgbaColor = color.ToRGBA32();

	SetBatchState( this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	const UINT iBaseVertex = self->batchedVertices.Num();
	Vertex & v0 = self->batchedVertices.Add();
	Vertex & v1 = self->batchedVertices.Add();
	Vertex & v2 = self->batchedVertices.Add();

	v0.xyz.x 	= a.x;
	v0.xyz.y 	= a.y;
	v0.xyz.z 	= a.z;
	v0.rgba.asU32 	= rgbaColor;

	v1.xyz.x 	= b.x;
	v1.xyz.y 	= b.y;
	v1.xyz.z 	= b.z;
	v1.rgba.asU32 	= rgbaColor;

	v2.xyz.x 	= c.x;
	v2.xyz.y 	= c.y;
	v2.xyz.z 	= c.z;
	v2.rgba.asU32 	= rgbaColor;

	self->batchedIndices.Add( iBaseVertex + 0 );
	self->batchedIndices.Add( iBaseVertex + 1 );
	self->batchedIndices.Add( iBaseVertex + 2 );
}

void BatchRenderer::DrawSolidAABB( const rxAABB& box, const FColor& color )
{
	SetBatchState( this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
Unimplemented;
#if 0
	enum { NUM_CORNERS = 8 };
	enum { NUM_INDICES = 6*2*3 };

	static const XMVECTOR verts[NUM_CORNERS] =
	{
		{ -1, -1, -1, 0 },
		{ 1, -1, -1, 0 },
		{ 1, -1, 1, 0 },
		{ -1, -1, 1, 0 },
		{ -1, 1, -1, 0 },
		{ 1, 1, -1, 0 },
		{ 1, 1, 1, 0 },
		{ -1, 1, 1, 0 }
	};
	static const Index indices[NUM_INDICES]=
	{
		//
	};

	enum { NUM_VERTICES = 24 };
	enum { NUM_INDICES = 36 };

	// Create vertex buffer.

	mxVertex vertices[ NUM_VERTICES ];

	// Fill in the front face vertex data.
	vertices[0] = mxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[1] = mxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),	Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[2] = mxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),	Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[3] = mxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	);

	// Fill in the back face vertex data.					
	vertices[4] = mxVertex( Vec3D( -0.5f, -0.5f, 0.5f ), 	Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[5] = mxVertex( Vec3D(  0.5f, -0.5f, 0.5f ), 	Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[6] = mxVertex( Vec3D(  0.5f,  0.5f, 0.5f ), 	Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[7] = mxVertex( Vec3D( -0.5f,  0.5f, 0.5f ), 	Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	);

	// Fill in the top face vertex data.					
	vertices[8]  = mxVertex( Vec3D( -0.5f, 0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[9]  = mxVertex( Vec3D( -0.5f, 0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[10] = mxVertex( Vec3D(  0.5f, 0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	);
	vertices[11] = mxVertex( Vec3D(  0.5f, 0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	);

	// Fill in the bottom face vertex data.					
	vertices[12] = mxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[13] = mxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[14] = mxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	);
	vertices[15] = mxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	);

	// Fill in the left face vertex data.					
	vertices[16] = mxVertex( Vec3D( -0.5f, -0.5f,  0.5f ),	Vec2D( 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	);
	vertices[17] = mxVertex( Vec3D( -0.5f,  0.5f,  0.5f ),	Vec2D( 0.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	);
	vertices[18] = mxVertex( Vec3D( -0.5f,  0.5f, -0.5f ),	Vec2D( 1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	);
	vertices[19] = mxVertex( Vec3D( -0.5f, -0.5f, -0.5f ),	Vec2D( 1.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	);

	// Fill in the right face vertex data.					
	vertices[20] = mxVertex( Vec3D(  0.5f, -0.5f, -0.5f ),	Vec2D( 0.0f, 1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	);
	vertices[21] = mxVertex( Vec3D(  0.5f,  0.5f, -0.5f ),	Vec2D( 0.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	);
	vertices[22] = mxVertex( Vec3D(  0.5f,  0.5f,  0.5f ),	Vec2D( 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	);
	vertices[23] = mxVertex( Vec3D(  0.5f, -0.5f,  0.5f ),	Vec2D( 1.0f, 1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	);

	// Scale the box.
	{
		const Vec3D  vScale( length, height, depth );
		Matrix4  scaleMat( Matrix4::CreateScale( vScale ) );
		Matrix4  invTranspose( scaleMat.Inverse().Transpose() );

		for(UINT i = 0; i < NUM_VERTICES; ++i)
		{
			vertices[i].xyz = scaleMat.TransformVector( vertices[i].xyz );

			vertices[i].N = invTranspose.TransformNormal( vertices[i].N );
			vertices[i].T = invTranspose.TransformNormal( vertices[i].T );
		}
	}

	// Create the index buffer.

	rxIndex indices[ NUM_INDICES ];

	// Fill in the front face index data
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8; indices[13] =  9; indices[14] = 10;
	indices[15] = 8; indices[16] = 10; indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;


	newMesh->vertices.SetNum( NUM_VERTICES );
	MemCopy( newMesh->vertices.ToPtr(), vertices, sizeof(vertices) );

	newMesh->indices.SetNum( NUM_INDICES );
	MemCopy( newMesh->indices.ToPtr(), indices, sizeof(indices) );
#endif
}

mxUNDONE
#if 0
void BatchRenderer::DrawRing( const XMFLOAT3& Origin, const XMFLOAT3& MajorAxis, const XMFLOAT3& MinorAxis, const FColor& Color )
{
   static const DWORD dwRingSegments = 32;

    XMFLOAT3 verts[ dwRingSegments + 1 ];

    XMVECTOR vOrigin = XMLoadFloat3( &Origin );
    XMVECTOR vMajor = XMLoadFloat3( &MajorAxis );
    XMVECTOR vMinor = XMLoadFloat3( &MinorAxis );

    FLOAT fAngleDelta = XM_2PI / ( float )dwRingSegments;
    // Instead of calling cos/sin for each segment we calculate
    // the sign of the angle delta and then incrementally calculate sin
    // and cosine from then on.
    XMVECTOR cosDelta = XMVectorReplicate( cosf( fAngleDelta ) );
    XMVECTOR sinDelta = XMVectorReplicate( sinf( fAngleDelta ) );
    XMVECTOR incrementalSin = XMVectorZero();
    static const XMVECTOR initialCos =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };
    XMVECTOR incrementalCos = initialCos;
    for( DWORD i = 0; i < dwRingSegments; i++ )
    {
        XMVECTOR Pos;
        Pos = XMVectorMultiplyAdd( vMajor, incrementalCos, vOrigin );
        Pos = XMVectorMultiplyAdd( vMinor, incrementalSin, Pos );
        XMStoreFloat3( ( XMFLOAT3* )&verts[i], Pos );
        // Standard formula to rotate a vector.
        XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
        XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
        incrementalCos = newCos;
        incrementalSin = newSin;
    }
    verts[ dwRingSegments ] = verts[0];

    // Copy to vertex buffer
    assert( (dwRingSegments+1) <= MAX_VERTS );

    XMFLOAT3* pVerts = NULL;
    HRESULT hr;
    V( g_pVB->Lock( 0, 0, (void**)&pVerts, D3DLOCK_DISCARD ) )
    memcpy( pVerts, verts, sizeof(verts) );
    V( g_pVB->Unlock() )

    // Draw ring
    D3DXCOLOR clr = Color;
    g_pEffect9->SetFloatArray( g_Color, clr, 4 );
    g_pEffect9->CommitChanges();
    pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, dwRingSegments );
}
void BatchRenderer::DrawSphere( const XNA::Sphere& sphere, const FColor& Color )
{
	const XMFLOAT3 Origin = sphere.Center;
	const float fRadius = sphere.Radius;

	DrawRing( pd3dDevice, Origin, XMFLOAT3( fRadius, 0, 0 ), XMFLOAT3( 0, 0, fRadius ), Color );
	DrawRing( pd3dDevice, Origin, XMFLOAT3( fRadius, 0, 0 ), XMFLOAT3( 0, fRadius, 0 ), Color );
	DrawRing( pd3dDevice, Origin, XMFLOAT3( 0, fRadius, 0 ), XMFLOAT3( 0, 0, fRadius ), Color );
}
void BatchRenderer::DrawRay( const XMFLOAT3& Origin, const XMFLOAT3& Direction, BOOL bNormalize, const FColor& Color )
{
    XMFLOAT3 verts[3];
    memcpy( &verts[0], &Origin, 3 * sizeof( FLOAT ) );

    XMVECTOR RayOrigin = XMLoadFloat3( &Origin );
    XMVECTOR RayDirection = XMLoadFloat3( &Direction );
    XMVECTOR NormDirection = XMVector3Normalize( RayDirection );
    if( bNormalize )
        RayDirection = NormDirection;

    XMVECTOR PerpVector;
    XMVECTOR CrossVector = XMVectorSet( 0, 1, 0, 0 );
    PerpVector = XMVector3Cross( NormDirection, CrossVector );

    if( XMVector3Equal( XMVector3LengthSq( PerpVector ), XMVectorSet( 0, 0, 0, 0 ) ) )
    {
        CrossVector = XMVectorSet( 0, 0, 1, 0 );
        PerpVector = XMVector3Cross( NormDirection, CrossVector );
    }
    PerpVector = XMVector3Normalize( PerpVector );

    XMStoreFloat3( ( XMFLOAT3* )&verts[1], XMVectorAdd( RayDirection, RayOrigin ) );
    PerpVector = XMVectorScale( PerpVector, 0.0625f );
    NormDirection = XMVectorScale( NormDirection, -0.25f );
    RayDirection = XMVectorAdd( PerpVector, RayDirection );
    RayDirection = XMVectorAdd( NormDirection, RayDirection );
    XMStoreFloat3( ( XMFLOAT3* )&verts[2], XMVectorAdd( RayDirection, RayOrigin ) );
    
    // Copy to vertex buffer
    assert( 3 <= MAX_VERTS );
    XMFLOAT3* pVerts = NULL;
    HRESULT hr;
    V( g_pVB->Lock( 0, 0, (void**)&pVerts, D3DLOCK_DISCARD ) )
    memcpy( pVerts, verts, sizeof(verts) );
    V( g_pVB->Unlock() )

    // Draw ray
    D3DXCOLOR clr = Color;
    g_pEffect9->SetFloatArray( g_Color, clr, 4 );
    g_pEffect9->CommitChanges();
    pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, 2 );
}
#endif


void BatchRenderer::DrawSprite(
	const Matrix4& cameraWorldMatrix,
	const Vec3D& spriteOrigin,
	const FLOAT spriteSizeX, const FLOAT spriteSizeY,
	const FColor& color
)
{
	const Vec3D spriteX = cameraWorldMatrix[0].ToVec3() * spriteSizeX;
	const Vec3D spriteY = cameraWorldMatrix[1].ToVec3() * spriteSizeY;

	const U4 rgbaColor = color.ToRGBA32();

	SetBatchState( this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	const UINT iBaseVertex = self->batchedVertices.Num();

	Vertex & v0 = self->batchedVertices.Add();
	Vertex & v1 = self->batchedVertices.Add();
	Vertex & v2 = self->batchedVertices.Add();
	Vertex & v3 = self->batchedVertices.Add();

	
	v0.xyz		= spriteOrigin - spriteX - spriteY;	// bottom left
	v0.uv.x 	= 0.0f;
	v0.uv.y 	= 1.0f;
	v0.rgba.asU32 	= rgbaColor;

	v1.xyz		= spriteOrigin - spriteX + spriteY;	// top left
	v1.uv.x 	= 0.0f;
	v1.uv.y 	= 0.0f;
	v1.rgba.asU32 	= rgbaColor;

	v2.xyz		= spriteOrigin + spriteX + spriteY;	// top right
	v2.uv.x 	= 1.0f;
	v2.uv.y 	= 0.0f;
	v2.rgba.asU32 	= rgbaColor;

	v3.xyz		= spriteOrigin + spriteX - spriteY;	// bottom right
	v3.uv.x 	= 1.0f;
	v3.uv.y 	= 1.0f;
	v3.rgba.asU32 	= rgbaColor;

	// indices:
	// 0,	1,	2,
	// 0,	2,	3,

	self->batchedIndices.Add( iBaseVertex + 0 );
	self->batchedIndices.Add( iBaseVertex + 1 );
	self->batchedIndices.Add( iBaseVertex + 2 );

	self->batchedIndices.Add( iBaseVertex + 0 );
	self->batchedIndices.Add( iBaseVertex + 2 );
	self->batchedIndices.Add( iBaseVertex + 3 );
}

void BatchRenderer::SetTexture( ID3D11ShaderResourceView* texture )
{
	if( self->currentTexture != texture )
	{
		this->Flush();
		self->currentTexture = texture;
	}
}

void BatchRenderer::Flush()
{
	const UINT numVertices = self->batchedVertices.Num();
	const UINT numIndices = self->batchedIndices.Num();

	if( numVertices == 0 )
	{
		return;
	}

	ID3D11DeviceContext* pD3DContext = D3DContext;

	{
		dxScopedLock	lockForWriting( pD3DContext, self->dynamicVB.mVB, D3D11_MAP_WRITE_DISCARD );
		MemCopy( lockForWriting.ToVoidPtr(), self->batchedVertices.ToPtr(), self->batchedVertices.GetDataSize() );
	}
	{
		dxScopedLock	lockForWriting( pD3DContext, self->dynamicIB.mIB.pD3DBuffer, D3D11_MAP_WRITE_DISCARD );
		MemCopy( lockForWriting.ToVoidPtr(), self->batchedIndices.ToPtr(), self->batchedIndices.GetDataSize() );
	}

	self->batchedVertices.Empty();
	self->batchedIndices.Empty();

	//dxSavePreviouslyBoundMesh	savePrevMesh;
	//dxSaveRenderStates			saveRenderStates;
	//dxSaveShaderResources		saveShaderResources;

	self->currentState->Set( pD3DContext );

	// If we're not currently rendering hit proxies...
	if( !self->pHitTesting )
	{
		// ... then render with our own shader.

		typedef GPU::p_batched_lines ShaderType;

		// Update shader constants.
		ShaderType::Data* pData = ShaderType::cb_Data.Map(pD3DContext);
		{
			pData->transform = self->currentTransform;
		}
		ShaderType::cb_Data.Unmap(pD3DContext);

		ShaderType::textureMap = self->currentTexture;

		// Select correct shader variation.
		rxShaderInstanceId	shaderInstanceId = ShaderType::DefaultInstanceId;

		if( self->currentTexture.IsValid() )
		{
			shaderInstanceId |= ShaderType::bTextureMap;
		}

		ShaderType::Set( pD3DContext, shaderInstanceId );
	}

	// otherwise, render with hit proxy shader.
#if MX_EDITOR
	else
	{
		self->pHitTesting->SetTransform( self->currentTransform );
	}
#endif // MX_EDITOR

	self->dynamicVB.Bind( pD3DContext );
	self->dynamicIB.Bind( pD3DContext );

	pD3DContext->IASetInputLayout( Vertex::layout );
	pD3DContext->IASetPrimitiveTopology( self->currentTopology );

	if( numIndices > 0 )
	{
		pD3DContext->DrawIndexed( numIndices, 0, 0 );
	}
	else
	{
		pD3DContext->Draw( numVertices, 0 );
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
