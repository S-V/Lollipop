/*
===========================================================
	Render targets

	ScaleX, ScaleY - Viewport ratio
	SizeX, SizeY - Absolute size
===========================================================
*/

// R8G8B8 - world-space normal, A8 - specular intensity
RenderTarget RT_Normal_SpecIntensity
{
	ScaleX		1.0
	ScaleY		1.0
	Format		R8G8B8A8_UNORM
	Info		"Normals & Specular intensity"
}

// R8G8B8 - albedo, A8 - specular power
RenderTarget RT_Diffuse_SpecPower
{
	ScaleX		1.0
	ScaleY		1.0
	Format		R8G8B8A8_UNORM
	Info		"Diffuse & Specular power"
}

// 16 or 32-bit linear depth (for restoring pixel positions and shadow mapping)
RenderTarget RT_LinearDepth
{
	ScaleX		1.0
	ScaleY		1.0
	Format		R32_FLOAT	// is packing into R8G8B8A8 faster ?
	Info		"Depth"
}

// screen-space motion vectors (XY) for motion blur
//RenderTarget RT_MotionXY
//{
//	ScaleX		1.0
//	ScaleY		1.0
//	Format		R16G16_FLOAT	// is packing into R8G8B8A8 faster ?
//	Info		"MotionXY"
//}





// MRTs are packed and optimized for efficiency
MultiRenderTarget GBuffer
{
	RenderTarget RT_Normal_SpecIntensity
	{
		ClearColor RGBA( 0.0f 0.0f 0.0f 0.0f )
	}
	RenderTarget RT_Diffuse_SpecPower
	{
		ClearColor RGBA( 0.0f 0.0f 0.0f 1.0f )
	}
	RenderTarget RT_LinearDepth
	{
		ClearColor RGBA( 0.0f 0.0f 0.0f 0.0f )
	}
	//DepthStencil $Main

	Info	"Geometry buffer"
}


//@todo: compare R11G11B10 with R16G16B16A16
RenderTarget RT_HDR_SceneColor
{
	ScaleX		1.0
	ScaleY		1.0
	
	Format		R16G16B16A16_FLOAT
	//Format		R11G11B10_FLOAT

	Info		"HDR Light Accum"
}

/**/
// used for subsurface scattering post-processing pass
RenderTarget RT_Ping
{
	ScaleX		1.0
	ScaleY		1.0
	Format		R16G16B16A16_FLOAT
	Info		"Temporary storage"
}
RenderTarget RT_Pong
{
	ScaleX		1.0
	ScaleY		1.0
	Format		R16G16B16A16_FLOAT
	Info		"Temporary storage"
}
/** /
RenderTarget RT_Downscaled_2x2
{
	ScaleX		0.5
	ScaleY		0.5
	Format		R16G16B16A16_FLOAT
	Info		"Temporary storage"
}
/**/
RenderTarget RT_Downscaled_4x4
{
	ScaleX		0.25f
	ScaleY		0.25f
	Format		R8G8B8A8_UNORM
	Info		"Downscaled map for bloom"
}

RenderTarget RT_Downscaled_4x4_B
{
	ScaleX		0.25f
	ScaleY		0.25f
	Format		R8G8B8A8_UNORM
	Info		"Downscaled map for bloom (B)"
}
/*
RenderTarget RT_HBlur_Map
{
	ScaleX		0.25f
	ScaleY		0.25f
	Format		R8G8B8A8_UNORM
	Info		"HBlur map for bloom"
}
RenderTarget RT_FinalBlur_Map
{
	ScaleX		0.25f
	ScaleY		0.25f
	Format		R8G8B8A8_UNORM
	Info		"Final blur map for bloom"
}
*/

// Render targets for GPU based exposure level computation

// textures to hold the intermediate results of the luminance calculation
/*
RenderTarget RT_Lum256
{
	SizeX		256
	SizeY		256
	Format		R16_FLOAT
	Info		"Lum256"
}
RenderTarget RT_Lum64
{
	SizeX		64
	SizeY		64
	Format		R16_FLOAT
	Info		"Lum64"
}
RenderTarget RT_Lum16
{
	SizeX		16
	SizeY		16
	Format		R16_FLOAT
	Info		"Lum16"
}
RenderTarget RT_Lum4
{
	SizeX		4
	SizeY		4
	Format		R16_FLOAT
	Info		"Lum4"
}
RenderTarget RT_AvgLum
{
	SizeX		1
	SizeY		1
	Format		R16_FLOAT
	Info		"AverageLuminance"
}
*/
/*
RenderTarget AdaptedLuminance
{
	SizeX		1
	SizeY		1
	Format		R16_FLOAT
}
*/
