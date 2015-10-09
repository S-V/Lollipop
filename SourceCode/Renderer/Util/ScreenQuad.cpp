/*
=============================================================================
	File:	ScreenQuad.cpp
	Desc:	Utilities for drawing screen-aligned quads.
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include "ScreenQuad.h"

#include <Renderer/GPU/ShaderPrograms.hxx>


void DrawFullScreenTriangle( ID3D11DeviceContext* context, const ID3D11ShaderResourceViewPtr& texture )
{
	GPU::p_fullscreen_textured_triangle_shader::sourceTexture = texture;
	GPU::p_fullscreen_textured_triangle_shader::Set(context);

	RenderFullScreenTriangleOnly(context);
}
void DrawFullScreenTriangle( ID3D11DeviceContext* context, const FColor& color )
{
	GPU::p_fullscreen_colored_triangle_shader::Data* pData = GPU::p_fullscreen_colored_triangle_shader::cb_Data.Map(context);
	pData->color = color.mSimdQuad;
	GPU::p_fullscreen_colored_triangle_shader::cb_Data.Unmap(context);
	GPU::p_fullscreen_colored_triangle_shader::Set(context);

	RenderFullScreenTriangleOnly(context);
}

#if 0
/*================================
		rxScreenTriangle
================================*/

rxScreenTriangle::rxScreenTriangle()
{
	DBG_ENSURE_ONLY_ONE_CALL;
}

rxScreenTriangle::~rxScreenTriangle()
{}

void rxScreenTriangle::Initialize()
{
	if( ! vertexLayout )
	{
		vertexLayout = RX_GET_INPUT_LAYOUT( ScreenVertex );
	}
	if( ! vertexBuffer )
	{
		// NOTE: z = 1 (the farthest possible distance) -> optimization for deferred lighting/shading engines:
		// skybox outputs z = w = 1 -> if we set depth test to less than the sky won't be shaded
		//
		const ScreenVertex vertices[VERTEX_COUNT] =
		{
			ScreenVertex( Vec4D( -1.0f, -3.0f, 1.0f, 1.0f ), Vec2D( 0.0f, 2.0f ) ),	// lower-left
			ScreenVertex( Vec4D( -1.0f,  1.0f, 1.0f, 1.0f ), Vec2D( 0.0f, 0.0f ) ),	// upper-left
			ScreenVertex( Vec4D(  3.0f,  1.0f, 1.0f, 1.0f ), Vec2D( 2.0f, 0.0f ) ),	// upper-right
		};

		vertexBuffer = rxResourceServer::Get().NewVertexBuffer(
			sizeof( vertices ),
			sizeof(vertices[0]),
			vertices
		);
	}
}

void rxScreenTriangle::Shutdown()
{
	vertexBuffer = null;
	vertexLayout = null;
}

void rxScreenTriangle::Draw()
{
	// Draw a triangle covering the whole screen.

	gfxBackEnd.rxSetTopology( EPrimitiveType::PT_TriangleList );
	gfxBackEnd.rxSetInputLayout( vertexLayout );
	gfxBackEnd.SetVertexBuffer( vertexBuffer );
	gfxBackEnd.Draw( VERTEX_COUNT, 0 /*StartVertexLocation*/ );
}

void rxScreenTriangle::Begin()
{
	gfxBackEnd.rxSetTopology( EPrimitiveType::PT_TriangleList );
	gfxBackEnd.rxSetInputLayout( vertexLayout );
	gfxBackEnd.SetVertexBuffer( vertexBuffer );
}

void rxScreenTriangle::DrawQuad()
{
	gfxBackEnd.Draw( VERTEX_COUNT, 0 );
}

void rxScreenTriangle::End()
{

}

/*================================
		rxScreenQuad
================================*/

rxScreenQuad::rxScreenQuad()
{
	DBG_ENSURE_ONLY_ONE_CALL;
}

rxScreenQuad::~rxScreenQuad()
{}

void rxScreenQuad::Initialize()
{
	if( ! vertexLayout )
	{
		vertexLayout = RX_GET_INPUT_LAYOUT( ScreenVertex );
	}
	if( ! vertexBuffer )
	{
		// NOTE: z = 1 (the farthest possible distance) -> optimization for deferred lighting/shading engines:
		// skybox outputs z = w = 1 -> if we set depth test to less than the sky won't be shaded
		//
		const ScreenVertex vertices[VERTEX_COUNT] =
		{
			ScreenVertex( Vec4D( -1.0f,  1.0f, 1.0f, 1.0f ),	Vec2D( 0.0f, 0.0f ) ),
			ScreenVertex( Vec4D(  1.0f,  1.0f, 1.0f, 1.0f ),	Vec2D( 1.0f, 0.0f ) ),
			ScreenVertex( Vec4D( -1.0f, -1.0f, 1.0f, 1.0f ),	Vec2D( 0.0f, 1.0f ) ),
			ScreenVertex( Vec4D(  1.0f, -1.0f, 1.0f, 1.0f ),	Vec2D( 1.0f, 1.0f ) ),
		};

		vertexBuffer = rxResourceServer::Get().NewVertexBuffer(
			sizeof( vertices ),
			sizeof(vertices[0]),
			vertices
		);
	}
}

void rxScreenQuad::Shutdown()
{
	vertexBuffer = null;
	vertexLayout = null;
}

void rxScreenQuad::Draw()
{
	// Draw a triangle covering the whole screen.

	gfxBackEnd.rxSetTopology( EPrimitiveType::PT_TriangleStrip );
	gfxBackEnd.rxSetInputLayout( vertexLayout );
	gfxBackEnd.SetVertexBuffer( vertexBuffer );
	gfxBackEnd.Draw( VERTEX_COUNT, 0 );
}

void rxScreenQuad::Begin()
{
	gfxBackEnd.rxSetTopology( EPrimitiveType::PT_TriangleStrip );
	gfxBackEnd.rxSetInputLayout( vertexLayout );
	gfxBackEnd.SetVertexBuffer( vertexBuffer );
}

void rxScreenQuad::DrawQuad()
{
	gfxBackEnd.Draw( VERTEX_COUNT, 0 );
}

void rxScreenQuad::End()
{

}
#endif


//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
