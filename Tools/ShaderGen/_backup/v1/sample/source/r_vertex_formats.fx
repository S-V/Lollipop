/*
=====================================================================
	
	Standard (built-in) vertex types.

=====================================================================
*/

// used for rendering static meshes
//
Vertex vtx_static
{
	xyz "position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
	}

	T "tangent (binormal)"
	{
		SemanticName			Tangent
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UINT
	}

	N "normal"
	{
		SemanticName			Normal
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UINT
	}

	uv "texture coordinates"
	{
		SemanticName			TexCoord
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32_FLOAT
	}
}



//
//	vtx_debug - used for debug drawing (shapes,arrows,overlays,etc)
//
/*
Vertex vtx_debug
{
	xyz "position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
	}

	T "padding"
	{
		SemanticName			Unknown
		SemanticIndex			0
		Format					DXGI_FORMAT_R32_FLOAT
	}

	uv "Color"
	{
		SemanticName			Color
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32A32_FLOAT
	}
}
*/
