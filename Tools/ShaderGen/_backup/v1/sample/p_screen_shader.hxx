namespace GPU
{
	// Shaders

	//--------------------------------------------------------------
	//	p_fullscreen_textured_triangle_shader
	//--------------------------------------------------------------
	struct p_fullscreen_textured_triangle_shader
	{
	
	public:	// Sampler states (set manually)
		static ID3D11SamplerState* linearSampler;
	
	public:	// Shader resources (set manually)
		static ID3D11ShaderResourceView* sourceTexture;
	
	public:	// Public methods
		static inline void Set()
		{
			
			// Bind sampler states to slots
			D3DContext->PSSetSamplers(
				0,
				1,
				(ID3D11SamplerState**)&linearSampler
			);
			
			// Bind shader resources
			D3DContext->PSSetShaderResources(
				0,
				1,
				(ID3D11ShaderResourceView**)&sourceTexture
			);
			
			// Bind shader program
			SetVertexShader( shaderInstances[0].vertexShader );
			SetGeometryShader( nil );
			SetPixelShader( shaderInstances[0].pixelShader );
		}
	
	public:	// Read-only properties
		static const char* VS_EntryPoint; // vertex shader function's entry point
		static const char* PS_EntryPoint; // pixel shader function's entry point
		static const char* Name; // unique name
		static const rxStaticString Source; // original source code
		enum { UID = 1 }; // unique index
		enum { NumInstances = 1 }; // number of unique combinations
	
	public_internal:
		static void Initialize();
		static void Shutdown();
		
		static void GetLoadInfo( ShaderInfo & d );
		static void Load( const ShaderInfo& shaderInfo, ShaderManager* compiler );
	
	private:
		p_fullscreen_textured_triangle_shader() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_fullscreen_textured_triangle_shader);
	};
	
	//--------------------------------------------------------------
	//	p_fullscreen_colored_triangle_shader
	//--------------------------------------------------------------
	struct p_fullscreen_colored_triangle_shader
	{
#pragma pack (push,16)
		struct Data
		{
			float4	color;
		};
#pragma pack (pop)
	
	public:	// Constant buffers (manually updated)
		static TypedConstantBuffer< Data >	cb_Data;
	
	public:	// Public methods
		static inline void Set()
		{
			// Bind constant buffers
			cb_Data.BindTo_PS(0);
			
			// Bind shader program
			SetVertexShader( shaderInstances[0].vertexShader );
			SetGeometryShader( nil );
			SetPixelShader( shaderInstances[0].pixelShader );
		}
	
	public:	// Read-only properties
		static const char* VS_EntryPoint; // vertex shader function's entry point
		static const char* PS_EntryPoint; // pixel shader function's entry point
		static const char* Name; // unique name
		static const rxStaticString Source; // original source code
		enum { UID = 0 }; // unique index
		enum { NumInstances = 1 }; // number of unique combinations
	
	public_internal:
		static void Initialize();
		static void Shutdown();
		
		static void GetLoadInfo( ShaderInfo & d );
		static void Load( const ShaderInfo& shaderInfo, ShaderManager* compiler );
	
	private:
		p_fullscreen_colored_triangle_shader() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_fullscreen_colored_triangle_shader);
	};
	
	
}
