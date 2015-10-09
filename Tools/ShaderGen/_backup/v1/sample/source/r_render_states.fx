/*
===========================================================
	Sampler states
===========================================================
*/

SamplerState SS_Point
{
	Filter		MIN_MAG_MIP_POINT
	AddressU	Clamp
	AddressV	Clamp
}
SamplerState SS_Bilinear
{
	Filter		MIN_MAG_LINEAR_MIP_POINT
	AddressU	Wrap
	AddressV	Wrap
}
/*
SamplerState SS_Trilinear
{
	Filter		MIN_MAG_MIP_LINEAR
	AddressU	Wrap
	AddressV	Wrap
}
*/
SamplerState SS_Aniso
{
	Filter		ANISOTROPIC
	AddressU	Wrap
	AddressV	Wrap
}

/*
//typedef SS_Trilinear SS_Linear

// used for post-processing
SamplerState SS_PointClamp
{
	Filter		MIN_MAG_MIP_POINT
	AddressU	Clamp
	AddressV	Clamp
}
SamplerState SS_LinearClamp
{
	Filter		MIN_MAG_LINEAR_MIP_POINT
	AddressU	Clamp
	AddressV	Clamp
}
*/




/*
===========================================================
	Depth-Stencil states
===========================================================
*/

DepthStencilState DS_NormalZTestWriteNoStencil
{
	DepthEnable 	1
	DepthWriteMask 	all
	DepthFunc 		less
	StencilEnable 	0
}
DepthStencilState DS_NoZTestWriteNoStencil
{
	DepthEnable 	0
	DepthWriteMask 	0
	DepthFunc 		less
	StencilEnable 	0
}


/*
===========================================================
	Rasterizer states
===========================================================
*/
RasterizerState RS_NoCull
{
	FillMode	Solid
	CullMode	None
	DepthClipEnable	true
	MultisampleEnable	false
}
RasterizerState RS_CullBack
{
	FillMode	Solid
	CullMode	Back
	DepthClipEnable	true
	MultisampleEnable	false
}
/*
RasterizerState RS_CullFront
{
	FillMode	Solid
	CullMode	Front
	DepthClipEnable	true
	MultisampleEnable	false
}


RasterizerState RS_Wireframe
{
	FillMode			wireframe
	CullMode			back
	DepthClipEnable		true
	MultisampleEnable	false
}
*/
//typedef RS_CullBack			RS_Normal

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
/*
BlendState BS_AdditiveBlending
{
	BlendEnable				true

	SrcBlend 	 			One
	DestBlend	 			One
	BlendOp		 			Add

	RenderTargetWriteMask	0x0F
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
*/
//typedef BS_AlphaBlending	BS_BlendTranslucent


/*
===========================================================
	State blocks
===========================================================
*/

StateBlock Default
{
	RasterizerState		 RS_CullBack

	DepthStencilState	 DS_NormalZTestWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}

//StateBlock Opaque = Default;

StateBlock Debug_NoCull
{
	RasterizerState		 RS_NoCull

	DepthStencilState	 DS_NoZTestWriteNoStencil
	StencilRef			 0

	BlendState 			 BS_NoBlending
	BlendFactor			 RGBA( 0.0f 0.0f 0.0f 1.0f )
	SampleMask			 0xFFFFFFFF
}


