/*
=============================================================================
	File:	BatchRenderer.h
	Desc:	Line-batching renderer.
=============================================================================
*/

#pragma once

#include <Renderer/Core/Geometry.h>

class AHitTesting;

/*
=======================================================================

	Debug line drawing.

=======================================================================
*/

/*
-----------------------------------------------------------------------------
	Batched primitive renderer.

	designed for convenience, not for speed;
	is meant to be used for debug visualization, etc.
-----------------------------------------------------------------------------
*/
class BatchRenderer
	: SingleInstance<BatchRenderer>
{
public:

	// don't forget to call this function
	// at the end of each frame or before significant state changes
	// to flush any batched primitives
	// and prevent flickering
	//
	void Flush();

	void SetTransform( mat4_carg newTransform );
	float4x4 GetTransform() const;

	void SetState( StateBlock& newState );

	void SetHitTesting( AHitTesting* pHitTesting = nil );
	AHitTesting* IsHitTesting() const;

	// Wireframe mode

	void DrawLine3D(
		FLOAT x1, FLOAT y1, FLOAT z1,
		FLOAT x2, FLOAT y2, FLOAT z2,
		const FColor& startColor = FColor::WHITE,
		const FColor& endColor = FColor::WHITE
	);
	void DrawLine3D(
		const Vec3D& start,
		const Vec3D& end,
		const FColor& startColor = FColor::WHITE,
		const FColor& endColor = FColor::WHITE
	);
	void DrawTriangle3D(
		const Vec3D& a, const Vec3D& b, const Vec3D& c,
		const FColor& color = FColor::WHITE
	);

	void DrawDashedLine(
		const Vec3D& start,
		const Vec3D& end,
		const FLOAT dashSize,
		const FColor& startColor = FColor::WHITE,
		const FColor& endColor = FColor::WHITE
	);

	void DrawAxes( FLOAT length = 1.0f );

	// 'wireframe' arrow
	void DrawArrow( const Matrix4& arrowTransform, const FColor& color, FLOAT arrowLength = 1.f, FLOAT headSize = 0.1f );

	void DrawInfiniteGrid( const Vec3D& eyePos, FLOAT height = 0.0f, const FColor& color = FColor::WHITE );
	void DrawGrid( const Vec3D& XAxis, const Vec3D& YAxis, const Vec3D& Origin, int iXDivisions, int iYDivisions, const FColor& color );
	void DrawCube( const float4x4& worldMatrix, const FColor& color );
	void DrawAABB( const rxAABB& box, const FColor& color );
	void DrawOBB( const rxOOBB& box, const FColor& color );
	void DrawFrustum( const XNA::Frustum& frustum, const FColor& color );
	void DrawViewFrustum( const ViewFrustum& frustum, const FColor& color );

	void DrawCircle( const Vec3D& origin,const Vec3D& vX,const Vec3D& vY, const FColor& color, FLOAT radius, INT numSides = 16 );


	// Solid mode

	void DrawSolidTriangle3D(
		const Vec3D& a, const Vec3D& b, const Vec3D& c,
		const FColor& color = FColor::WHITE
	);

	void DrawSolidAABB( const rxAABB& box, const FColor& color = FColor::WHITE );

	mxUNDONE
#if 0
	void DrawRing( const XMFLOAT3& Origin, const XMFLOAT3& MajorAxis, const XMFLOAT3& MinorAxis, const FColor& Color );
	void DrawSphere( const XNA::Sphere& sphere, const FColor& Color );
	void DrawRay( const XMFLOAT3& Origin, const XMFLOAT3& Direction, BOOL bNormalize, const FColor& Color );
#endif

	// accepts sprite's coordinates in world space
	void DrawSprite(
		const Matrix4& cameraWorldMatrix,
		const Vec3D& spriteOrigin,
		const FLOAT spriteSizeX = 1.0f, const FLOAT spriteSizeY = 1.0f,
		const FColor& color = FColor::WHITE
	);

	void SetTexture( ID3D11ShaderResourceView* texture );

public_internal:	// Initialization/Destruction
	friend class rxRenderer;

	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::HeapGeneric,BatchRenderer);

	BatchRenderer();
	~BatchRenderer();

	void Initialize();
	void Shutdown();
};


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
