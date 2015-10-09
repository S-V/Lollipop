/*
=============================================================================
	File:	ScreenQuad.h
	Desc:	Utilities for drawing screen-aligned quads.
=============================================================================
*/

#pragma once

// draws a full-screen triangle - needs a proper vertex shader to work
// doesn't use any vertex/index buffers
static inline void RenderFullScreenTriangleOnly( ID3D11DeviceContext* context )
{
	context->IASetInputLayout(nil);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3,0);
}
static inline void RenderFullScreenQuadOnly( ID3D11DeviceContext* context )
{
	context->IASetInputLayout(nil);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4,0);
}

void DrawFullScreenTriangle( ID3D11DeviceContext* context, const ID3D11ShaderResourceViewPtr& texture );
void DrawFullScreenTriangle( ID3D11DeviceContext* context, const FColor& color );

//
//	ScreenVertex - used for drawing screen-aligned quads.
//
class ScreenVertex {
public:
	Vec4D	xyzw;
	Vec2D	uv;

public:
	ScreenVertex( const Vec4D& position, const Vec2D& texCoords )
		: xyzw( position ), uv( texCoords )
	{}
};

#if 0
//
//	rxScreenTriangle - is actually a screen-space triangle, needs 'ScreenVertex' input layout.
//
class rxScreenTriangle {
public:
	rxScreenTriangle();
	~rxScreenTriangle();

	void Initialize();
	void Shutdown();

	// renders the triangle with current shader
	void Draw();	// sets the necessary states and issues draw calls.

	void Begin();
	void DrawQuad();	// issues draw call only
	void End();

private:
	enum { VERTEX_COUNT = 3 };
	dxPtr< ID3D11InputLayout >	vertexLayout;
	TPtr< GrVertexData >		vertexBuffer;
};

//
//	rxScreenQuad - is a screen-space quad, needs 'ScreenVertex' input layout.
//
class rxScreenQuad {
public:
	rxScreenQuad();
	~rxScreenQuad();

	void Initialize();
	void Shutdown();

	// renders the triangle with current shader
	void Draw();	// sets the necessary states and issues draw calls.

	void Begin();
	void DrawQuad();	// issues draw call only
	void End();

private:
	enum { VERTEX_COUNT = 4 };
	dxPtr< ID3D11InputLayout >	vertexLayout;
	TPtr< GrVertexData >		vertexBuffer;
};
#endif



//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
