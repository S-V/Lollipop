namespace GPU
{
#pragma pack (push,1)
	struct vtx_static
	{
		float3		xyz;	// "position"
		rxNormal4		T;	// "tangent (binormal)"
		rxNormal4		N;	// "normal"
		float2		uv;	// "texture coordinates"
		
		// 12 + 4 + 4 + 8 = 28 bytes
	
	public:	// Input layout
		static InputLayout IL;
	};
#pragma pack (pop)
}
