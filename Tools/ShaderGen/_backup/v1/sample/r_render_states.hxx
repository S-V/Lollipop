namespace GPU
{
	// Sampler states
	extern SamplerState samplerState_SS_Point;
	extern SamplerState samplerState_SS_Bilinear;
	
	// Depth-stencil states
	extern DepthStencilState depthStencilState_DS_NormalZTestWriteNoStencil;
	extern DepthStencilState depthStencilState_DS_NoZTestWriteNoStencil;
	
	// Rasterizer states
	extern RasterizerState rasterizerState_RS_NoCull;
	extern RasterizerState rasterizerState_RS_CullBack;
	
	// Blend states
	extern BlendState blendState_BS_NoBlending;
	
	// State blocks
	extern StateBlock renderState_Default;
	extern StateBlock renderState_Debug_NoCull;
	
}
