/*
===========================================================
	Sampler states
===========================================================
*/

SamplerState SS_Point
{
	Filter		Min_Mag_Mip_Point
	AddressU	Clamp
	AddressV	Clamp
}
SamplerState SS_Bilinear
{
	Filter		Min_Mag_Linear_Mip_Point
	AddressU	Wrap
	AddressV	Wrap
}

SamplerState SS_Trilinear
{
	Filter		Min_Mag_Mip_Linear
	AddressU	Wrap
	AddressV	Wrap
}

SamplerState SS_Aniso
{
	Filter		Anisotropic
	AddressU	Clamp
	AddressV	Clamp
	MaxAnisotropy	8//16
}

// used for post-processing
SamplerState SS_PointClamp
{
	Filter		Min_Mag_Mip_Point
	AddressU	Clamp
	AddressV	Clamp
}
SamplerState SS_LinearClamp
{
	Filter		Min_Mag_Linear_Mip_Point
	AddressU	Clamp
	AddressV	Clamp
}


SamplerState SS_ShadowMap
{
	Filter			Comparison_Min_Mag_Mip_Point
	AddressU		Clamp
	AddressV		Clamp
	AddressW		Clamp
	//MipLODBias		0.0f
	//MaxAnisotropy	1
	ComparisonFunc	Less_Equal
	BorderColor		RGBA( 0.0f 0.0f 0.0f 0.0f )
	//MinLOD			0.0f
	//MaxLOD			3.402823466e+38f	// D3D11_FLOAT32_MAX
}
// shadowMapHWFilter - used for hardware accelerated shadow map filtering (2x2 PCF)
SamplerState SS_ShadowMapPCF
{
	Filter			Comparison_Min_Mag_Mip_Point
	AddressU		Clamp
	AddressV		Clamp
	AddressW		Clamp
	//MipLODBias		0.0f
	//MaxAnisotropy	1
	ComparisonFunc	Less_Equal
	BorderColor		RGBA( 0.0f 0.0f 0.0f 0.0f )
	//MinLOD			0.0f
	//MaxLOD			3.402823466e+38f	// D3D11_FLOAT32_MAX
}

SamplerState SS_ShadowMapPCF_Bilinear
{
	Filter			Comparison_Min_Mag_Linear_Mip_Point
	// Texture coordinates outside the range [0.0, 1.0] are set to the border color.
	AddressU		Border
	AddressV		Border
	AddressW		Border
	//MipLODBias		0.0f
	//MaxAnisotropy	1
	ComparisonFunc	Less
	BorderColor		RGBA( 0.0f 0.0f 0.0f 0.0f )
	//MinLOD			0.0f
	//MaxLOD			0.0f
}

/*
===========================================================
	Depth-Stencil states
===========================================================
*/

DepthStencilState DS_NormalZTestWriteNoStencil
{
	DepthEnable 	1
	DepthWriteMask 	All
	DepthFunc 		Less
	StencilEnable 	0
}
DepthStencilState DS_NormalZTestNoWriteNoStencil
{
	DepthEnable 	1
	DepthWriteMask 	0
	DepthFunc 		Less
	StencilEnable 	False
}
DepthStencilState DS_NoZTestWriteNoStencil
{
	DepthEnable 	False
	DepthWriteMask 	Zero
	DepthFunc 		Always
	StencilEnable 	False
}
DepthStencilState DS_ZTestLessEqual_ZWrite_NoStencil
{
	DepthEnable 	True
	DepthWriteMask 	All
	DepthFunc 		Less_Equal
	StencilEnable 	False
}

DepthStencilState DS_NormalZTestWrite_StencilWrite
{
	DepthEnable 			True
	DepthWriteMask 			All
	DepthFunc 				Less_Equal

	StencilEnable 			True
	FrontFaceStencilFunc	Always
	FrontFaceStencilPass	Replace
}
// used for separable subsurface scattering post-processing pass.
DepthStencilState DS_SeparableSSS
{
	DepthEnable 			False
	DepthWriteMask 			Zero
	DepthFunc 				Always

	StencilEnable 			True
	FrontFaceStencilFunc	Equal
	FrontFaceStencilPass	Keep
}

// used for rendering light volumes;
// optimization for deferred lighting/shading:
// full screen tri and skies output z=1 so the sky/background won't be shaded
//
DepthStencilState DS_ZTestGreaterNoZWriteNoStencil
{
	DepthEnable    	True
	DepthWriteMask 	Zero
	DepthFunc 		Greater
	StencilEnable 	False
}

// used for rendering sky dome
// and also for rendering hit proxies in the editor
//
DepthStencilState DS_ZTestLessEqualNoZWriteNoStencil
{
	DepthEnable    	True
	DepthWriteMask 	Zero
	DepthFunc 		Less_Equal
	StencilEnable 	False
}

/*
===========================================================
	Rasterizer states
===========================================================
*/
RasterizerState RS_SolidNoCull
{
	FillMode			Solid
	CullMode			None
	DepthClipEnable		true
	MultisampleEnable	false
}
RasterizerState RS_SolidCullBack
{
	FillMode			Solid
	CullMode			Back
	DepthClipEnable		true
	MultisampleEnable	false
}
RasterizerState RS_SolidCullFront
{
	FillMode			Solid
	CullMode			Front
	DepthClipEnable		true
	MultisampleEnable	false
}
RasterizerState RS_WireframeCullBack
{
	FillMode			Wireframe
	CullMode			Back
	DepthClipEnable		true
	MultisampleEnable	false
}

RasterizerState RS_SolidNoCullNoClip
{
	FillMode	Solid
	CullMode	None	// so that we don't need to draw both sides of editor gizmos
	DepthClipEnable	false
	MultisampleEnable	false
}
RasterizerState RS_WireframeNoCullNoClip
{
	FillMode	Wireframe
	CullMode	None
	DepthClipEnable	false
	MultisampleEnable	false
}

/*
===========================================================
	Blend states
===========================================================
*/

BlendState BS_NoBlending
{
	BlendEnable        		false
	AlphaToCoverageEnable	false
	RenderTargetWriteMask	0x0F
}

BlendState BS_AdditiveBlending
{
	BlendEnable				true

	SrcBlend 	 			One
	DestBlend	 			One
	BlendOp		 			Add

	RenderTargetWriteMask	color_write_enable_all
}

BlendState BS_AlphaBlending
{
	BlendEnable				true
	AlphaToCoverageEnable	false

	SrcBlend				src_alpha
	DestBlend				inv_src_alpha
	BlendOp					add

	RenderTargetWriteMask	color_write_enable_all
}

BlendState BS_NoBlendingNoColorWrites
{
	BlendEnable        		false
	AlphaToCoverageEnable	false
	RenderTargetWriteMask	0
}

/*
===========================================================
	State blocks
===========================================================
*/

StateBlock Default
{
	RasterizerState		 RS_SolidCullBack

	DepthStencilState	 DS_NormalZTestWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}

//StateBlock Opaque = Default;


// used for debug visualization and rendering editor gizmos
StateBlock DebugPrimitives
{
	RasterizerState		 RS_SolidNoCullNoClip

	DepthStencilState	 DS_ZTestLessEqualNoZWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}

StateBlock HitProxy_ZTestLessEqualCullBack
{
	RasterizerState		 RS_SolidCullBack

	DepthStencilState	 DS_ZTestLessEqualNoZWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}
//StateBlock HitProxy_NoZTestNoCull
//{
//	RasterizerState		 RS_SolidNoCullNoClip
//
//	DepthStencilState	 DS_NoZTestWriteNoStencil
//	StencilRef			 0
//
//	BlendState 			 BS_NoBlending
//	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
//	SampleMask			 0xFFFFFFFF
//}
StateBlock SolidNoCullNoZTestNoClipNoBlend
{
	RasterizerState		 RS_SolidNoCullNoClip

	DepthStencilState	 DS_NoZTestWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}

//StateBlock Debug_NoCull
//{
//	RasterizerState		 RS_SolidNoCull
//
//	DepthStencilState	 DS_NoZTestWriteNoStencil
//	StencilRef			 0
//
//	BlendState 			 BS_NoBlending
//	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
//	SampleMask			 0xFFFFFFFF
//}


StateBlock TranslucentCullBack
{
	RasterizerState		RS_SolidCullBack

	DepthStencilState	DS_NormalZTestNoWriteNoStencil
	StencilRef			0

	BlendState 			BS_AlphaBlending
	BlendFactor			RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			0xFFFFFFFF
}



StateBlock SkyLast
{
	RasterizerState		 RS_SolidCullBack

	DepthStencilState	 DS_ZTestLessEqualNoZWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}


//-----------------------------------------------------------------
// render states for deferred lighting
//-----------------------------------------------------------------

// e.g. full-screen pass for directional lighting
StateBlock Deferred_Light_FullScreen_Additive
{
	RasterizerState		RS_SolidNoCull

	DepthStencilState	DS_ZTestGreaterNoZWriteNoStencil	// full screen tri and skies output z=1 so the sky/background won't be shaded
	StencilRef			0

	BlendState 			BS_AdditiveBlending
	BlendFactor			RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			0xFFFFFFFF
}

//
//	Render states for drawing light shapes during deferred lighting stage.
//
// TODO: is it possible not to switch depth test direction mid-frame and use reverse culling only?
//

RasterizerState RS_EyeOutsideLightVolume
{
	FillMode				Solid
	CullMode				Back
	DepthClipEnable			false	// to hide geometry clipping artifacts
	MultisampleEnable		false
};

StateBlock Deferred_Light_ConvexMesh_Additive
{
	RasterizerState		RS_EyeOutsideLightVolume

	DepthStencilState	DS_NormalZTestNoWriteNoStencil
	StencilRef			0

	BlendState 			BS_AdditiveBlending
	BlendFactor			RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			0xFFFFFFFF
}

//-----------------------------------------------------------------
// Shadow mapping
//-----------------------------------------------------------------

RasterizerState RS_BuildShadowMap
{
	FillMode				Solid
	CullMode				Back
	SlopeScaledDepthBias	0.7f
	DepthClipEnable			true
	MultisampleEnable		false
}

StateBlock Build_Shadow_Map
{
	RasterizerState		RS_BuildShadowMap

	DepthStencilState	DS_NormalZTestWriteNoStencil
	StencilRef			0

	BlendState 			BS_NoBlending
	BlendFactor			RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			0xFFFFFFFF
}

