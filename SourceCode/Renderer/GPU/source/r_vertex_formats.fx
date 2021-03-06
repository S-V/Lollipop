/*
=====================================================================
	
	Standard (built-in) vertex types.

=====================================================================
*/

// used for rendering light volumes (deferred lighting)
Vertex Vertex_P3f
{
	xyz "position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
		InputSlot				0
	}
}

// used for rendering sky dome
Vertex Vertex_P3f_TEX2f
{
	xyz "position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
		InputSlot				0
	}
	uv "texture coordinates"
	{
		SemanticName			TexCoord
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32_FLOAT
		InputSlot				0
	}
}

//Vertex Vertex_P3f_TEX2f_N4Ub
//{
//	xyz "position"
//	{
//		SemanticName			Position
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32B32_FLOAT
//		InputSlot				0
//	}
//	uv "texture coordinates"
//	{
//		SemanticName			TexCoord
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32_FLOAT
//		InputSlot				0
//	}
//	N "normal (T')"
//	{
//		SemanticName			Normal
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R8G8B8A8_UINT
//		InputSlot				0
//	}
//}


// used for rendering static meshes
//
Vertex Vertex_P3f_TEX2f_N4Ub_T4Ub
{
	xyz "position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
		InputSlot				0
	}

	uv "texture coordinates"
	{
		SemanticName			TexCoord
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32_FLOAT
		InputSlot				1
	}

	N "normal (T')"
	{
		SemanticName			Normal
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UINT
		InputSlot				1
	}

	T "tangent (P')"
	{
		SemanticName			Tangent
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UINT
		InputSlot				1
	}
	/*
	B "binormal (cross(T,N))"
	{
		SemanticName			Binormal
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UINT
		InputSlot				4
	}
	*/
}


//Vertex Vertex_P4f_COL4f
//{
//	xyzw "transformed position"
//	{
//		SemanticName			Position
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32B32A32_FLOAT
//		InputSlot				0
//	}
//	rgba "vertex color"
//	{
//		SemanticName			Color
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32B32A32_FLOAT
//		InputSlot				0
//	}
//}

// used for rendering batched lines
//
Vertex Vertex_P3f_TEX2f_COL4Ub
{
	xyz "transformed position"
	{
		SemanticName			Position
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32B32_FLOAT
		InputSlot				0
	}
	uv "texture coordinates"
	{
		SemanticName			TexCoord
		SemanticIndex			0
		Format					DXGI_FORMAT_R32G32_FLOAT
		InputSlot				0
	}
	rgba "vertex color"
	{
		SemanticName			Color
		SemanticIndex			0
		Format					DXGI_FORMAT_R8G8B8A8_UNORM
		InputSlot				0
	}
}

//Vertex TextVertex
//{
//	xy "position"
//	{
//		SemanticName			Position
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32_FLOAT
//		InputSlot				0
//	}
//	uv "texture coordinates"
//	{
//		SemanticName			TexCoord
//		SemanticIndex			0
//		Format					DXGI_FORMAT_R32G32_FLOAT
//		InputSlot				0
//	}
//}

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
