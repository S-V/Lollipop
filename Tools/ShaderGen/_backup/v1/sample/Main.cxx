#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>

#include "p_screen_shader.hxx"
#include "r_render_states.hxx"
#include "r_render_targets.hxx"
#include "r_vertex_formats.hxx"

#include "Main.hxx"

namespace GPU
{
	// sorted in ascending order
	const char* g_shaderNames[ TotalNumberOfShaders ] =
	{
		p_fullscreen_colored_triangle_shader::Name,
		p_fullscreen_textured_triangle_shader::Name,
	};
	
	UINT ShaderNameToIndex( const char* str ) {
		return BinaryStringSearch( g_shaderNames, ARRAY_SIZE(g_shaderNames), str );
	}
	
	const char* ShaderIndexToName( UINT idx ) {
		Assert( idx < ARRAY_SIZE(g_shaderNames) );
		return g_shaderNames[ idx ];
	}
	
	extern void InitializeModule_p_screen_shader();
	extern void ShutdownModule_p_screen_shader();
	
	extern void InitializeModule_r_render_states();
	extern void ShutdownModule_r_render_states();
	
	extern void InitializeModule_r_render_targets();
	extern void ShutdownModule_r_render_targets();
	
	extern void InitializeModule_r_vertex_formats();
	extern void ShutdownModule_r_vertex_formats();
	
	void InitializeGPU()
	{
		InitializeModule_p_screen_shader();
		InitializeModule_r_render_states();
		InitializeModule_r_render_targets();
		InitializeModule_r_vertex_formats();
	}
	
	void ShutdownGPU()
	{
		ShutdownModule_p_screen_shader();
		ShutdownModule_r_render_states();
		ShutdownModule_r_render_targets();
		ShutdownModule_r_vertex_formats();
	}
}
