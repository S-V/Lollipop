#pragma once

#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>

MX_NAMESPACE_BEGIN

class VertexData;

MX_NAMESPACE_END

namespace GPU
{
	// Shader library info
	enum { TotalNumberOfShaders = 4 };
	
	UINT ShaderNameToIndex( const char* str );
	const char* ShaderIndexToName( UINT idx );
	
	// Function declarations
	void Initialize();
	void Shutdown();
	
	// Render targets
	extern RenderTarget renderTarget_RT_Diffuse_SpecPower;
	extern RenderTarget renderTarget_RT_Normal_SpecIntensity;
	extern RenderTarget renderTarget_RT_LinearDepth;
	extern RenderTarget renderTarget_RT_MotionXY;
	
	// Multiple render targets
	struct GBuffer
	{
		enum ERenderTargets
		{
			RT_Diffuse_SpecPower_index = 0,
			RT_Normal_SpecIntensity_index = 1,
			RT_LinearDepth_index = 2,
			NumRenderTargets = 3
		};
		
		dxPtr< ID3D11RenderTargetView >	pMRT[ NumRenderTargets ];
		FColor	clearColors[ NumRenderTargets ];
		
	
	public:
		FORCEINLINE GBuffer()
		{
			this->pMRT[ RT_Diffuse_SpecPower_index ] = renderTarget_RT_Diffuse_SpecPower.pRTV;
			this->pMRT[ RT_Normal_SpecIntensity_index ] = renderTarget_RT_Normal_SpecIntensity.pRTV;
			this->pMRT[ RT_LinearDepth_index ] = renderTarget_RT_LinearDepth.pRTV;
			
			this->clearColors[ RT_Diffuse_SpecPower_index ].Set( 0.000000f, 0.000000f, 0.000000f, 1.000000f );
			this->clearColors[ RT_Normal_SpecIntensity_index ].Set( 0.000000f, 0.000000f, 0.000000f, 0.000000f );
			this->clearColors[ RT_LinearDepth_index ].Set( 0.000000f, 0.000000f, 0.000000f, 0.000000f );
		}
		
		FORCEINLINE void ClearRenderTargets( ID3D11DeviceContext* theContext )
		{
			{
				ID3D11RenderTargetView* pRTV = this->pMRT[ RT_Diffuse_SpecPower_index ];
				const FLOAT* colorRGBA = this->clearColors[  RT_Diffuse_SpecPower_index ].ToFloatPtr();
				
				theContext->ClearRenderTargetView( pRTV, colorRGBA );
			}
			{
				ID3D11RenderTargetView* pRTV = this->pMRT[ RT_Normal_SpecIntensity_index ];
				const FLOAT* colorRGBA = this->clearColors[  RT_Normal_SpecIntensity_index ].ToFloatPtr();
				
				theContext->ClearRenderTargetView( pRTV, colorRGBA );
			}
			{
				ID3D11RenderTargetView* pRTV = this->pMRT[ RT_LinearDepth_index ];
				const FLOAT* colorRGBA = this->clearColors[  RT_LinearDepth_index ].ToFloatPtr();
				
				theContext->ClearRenderTargetView( pRTV, colorRGBA );
			}
		}
		
		FORCEINLINE void Bind( ID3D11DeviceContext* theContext, ID3D11DepthStencilView* pDSV = nil )
		{
			// Set render targets and depth-stencil.
			
			theContext->OMSetRenderTargets(
				NumRenderTargets,
				cast(ID3D11RenderTargetView**) this->pMRT,
				pDSV
			);
		}
		
		FORCEINLINE ID3D11RenderTargetView** GetRenderTargetsArray()
		{
			return cast(ID3D11RenderTargetView**) this->pMRT;
		}
		
		FORCEINLINE UINT Num() const
		{
			return ARRAY_SIZE(this->pMRT);
		}
	};
	
	// Sampler states
	extern SamplerState samplerState_SS_Point;
	extern SamplerState samplerState_SS_Bilinear;
	extern SamplerState samplerState_SS_Aniso;
	
	// Depth-stencil states
	extern DepthStencilState depthStencilState_DS_NormalZTestWriteNoStencil;
	extern DepthStencilState depthStencilState_DS_NoZTestWriteNoStencil;
	
	// Rasterizer states
	extern RasterizerState rasterizerState_RS_WireframeNoCullNoClip;
	extern RasterizerState rasterizerState_RS_NoCull;
	extern RasterizerState rasterizerState_RS_CullBack;
	
	// Blend states
	extern BlendState blendState_BS_NoBlending;
	
	// State blocks
	extern StateBlock renderState_Default;
	extern StateBlock renderState_Debug_NoCull;
	
	//--------------------------------------------------------------
	//	Globals
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API Shared_Globals
	{
#pragma pack (push,16)
		struct PerFrame
		{
			float1	globalTimeInSeconds;
		};
#pragma pack (pop)
	
	public:	// Constant buffers (manually updated)
		static TypedConstantBuffer< PerFrame >	cb_PerFrame;
	
	public:	// Sampler states (set manually)
		static ID3D11SamplerState* pointSampler;
		static ID3D11SamplerState* linearSampler;
		static ID3D11SamplerState* anisotropicSampler;
		static ID3D11SamplerState* colorMapSampler;
		static ID3D11SamplerState* detailMapSampler;
		static ID3D11SamplerState* normalMapSampler;
		static ID3D11SamplerState* specularMapSampler;
		static ID3D11SamplerState* attenuationSampler;
		static ID3D11SamplerState* cubeMapSampler;
	
	public:	// Public methods
		static inline void Set()
		{
			// Bind constant buffers
			cb_PerFrame.BindTo_VS(0);
			cb_PerFrame.BindTo_GS(0);
			cb_PerFrame.BindTo_PS(0);
			
			// Bind constant buffers
			// Bind sampler states to slots
			D3DContext->VSSetSamplers(
				0,
				1,
				(ID3D11SamplerState**)&pointSampler
			);
			D3DContext->GSSetSamplers(
				0,
				1,
				(ID3D11SamplerState**)&pointSampler
			);
			D3DContext->PSSetSamplers(
				0,
				1,
				(ID3D11SamplerState**)&pointSampler
			);
			D3DContext->VSSetSamplers(
				1,
				1,
				(ID3D11SamplerState**)&linearSampler
			);
			D3DContext->GSSetSamplers(
				1,
				1,
				(ID3D11SamplerState**)&linearSampler
			);
			D3DContext->PSSetSamplers(
				1,
				1,
				(ID3D11SamplerState**)&linearSampler
			);
			D3DContext->VSSetSamplers(
				2,
				1,
				(ID3D11SamplerState**)&anisotropicSampler
			);
			D3DContext->GSSetSamplers(
				2,
				1,
				(ID3D11SamplerState**)&anisotropicSampler
			);
			D3DContext->PSSetSamplers(
				2,
				1,
				(ID3D11SamplerState**)&anisotropicSampler
			);
			D3DContext->VSSetSamplers(
				3,
				1,
				(ID3D11SamplerState**)&colorMapSampler
			);
			D3DContext->GSSetSamplers(
				3,
				1,
				(ID3D11SamplerState**)&colorMapSampler
			);
			D3DContext->PSSetSamplers(
				3,
				1,
				(ID3D11SamplerState**)&colorMapSampler
			);
			D3DContext->VSSetSamplers(
				4,
				1,
				(ID3D11SamplerState**)&detailMapSampler
			);
			D3DContext->GSSetSamplers(
				4,
				1,
				(ID3D11SamplerState**)&detailMapSampler
			);
			D3DContext->PSSetSamplers(
				4,
				1,
				(ID3D11SamplerState**)&detailMapSampler
			);
			D3DContext->VSSetSamplers(
				5,
				1,
				(ID3D11SamplerState**)&normalMapSampler
			);
			D3DContext->GSSetSamplers(
				5,
				1,
				(ID3D11SamplerState**)&normalMapSampler
			);
			D3DContext->PSSetSamplers(
				5,
				1,
				(ID3D11SamplerState**)&normalMapSampler
			);
			D3DContext->VSSetSamplers(
				6,
				1,
				(ID3D11SamplerState**)&specularMapSampler
			);
			D3DContext->GSSetSamplers(
				6,
				1,
				(ID3D11SamplerState**)&specularMapSampler
			);
			D3DContext->PSSetSamplers(
				6,
				1,
				(ID3D11SamplerState**)&specularMapSampler
			);
			D3DContext->VSSetSamplers(
				7,
				1,
				(ID3D11SamplerState**)&attenuationSampler
			);
			D3DContext->GSSetSamplers(
				7,
				1,
				(ID3D11SamplerState**)&attenuationSampler
			);
			D3DContext->PSSetSamplers(
				7,
				1,
				(ID3D11SamplerState**)&attenuationSampler
			);
			D3DContext->VSSetSamplers(
				8,
				1,
				(ID3D11SamplerState**)&cubeMapSampler
			);
			D3DContext->GSSetSamplers(
				8,
				1,
				(ID3D11SamplerState**)&cubeMapSampler
			);
			D3DContext->PSSetSamplers(
				8,
				1,
				(ID3D11SamplerState**)&cubeMapSampler
			);
		}
	};
	
	//--------------------------------------------------------------
	//	View
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API Shared_View
	{
#pragma pack (push,16)
		struct PerView
		{
			float4x4	viewProjectionMatrix;
		};
#pragma pack (pop)
	
	public:	// Constant buffers (manually updated)
		static TypedConstantBuffer< PerView >	cb_PerView;
	
	public:	// Public methods
		static inline void Set()
		{
			// Bind constant buffers
			cb_PerView.BindTo_VS(1);
			cb_PerView.BindTo_GS(1);
			cb_PerView.BindTo_PS(1);
		}
	};
	
	//--------------------------------------------------------------
	//	GBuffer
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API Shared_GBuffer
	{
	
	public:	// Shader resources (set manually)
		static ID3D11ShaderResourceView* RT0;
		static ID3D11ShaderResourceView* RT1;
		static ID3D11ShaderResourceView* RT2;
		static ID3D11ShaderResourceView* RT3;
	
	public:	// Public methods
		static inline void Set()
		{
			
			// Bind shader resources
			D3DContext->VSSetShaderResources(
				0,
				1,
				(ID3D11ShaderResourceView**)&RT0
			);
			D3DContext->GSSetShaderResources(
				0,
				1,
				(ID3D11ShaderResourceView**)&RT0
			);
			D3DContext->PSSetShaderResources(
				0,
				1,
				(ID3D11ShaderResourceView**)&RT0
			);
			D3DContext->VSSetShaderResources(
				1,
				1,
				(ID3D11ShaderResourceView**)&RT1
			);
			D3DContext->GSSetShaderResources(
				1,
				1,
				(ID3D11ShaderResourceView**)&RT1
			);
			D3DContext->PSSetShaderResources(
				1,
				1,
				(ID3D11ShaderResourceView**)&RT1
			);
			D3DContext->VSSetShaderResources(
				2,
				1,
				(ID3D11ShaderResourceView**)&RT2
			);
			D3DContext->GSSetShaderResources(
				2,
				1,
				(ID3D11ShaderResourceView**)&RT2
			);
			D3DContext->PSSetShaderResources(
				2,
				1,
				(ID3D11ShaderResourceView**)&RT2
			);
			D3DContext->VSSetShaderResources(
				3,
				1,
				(ID3D11ShaderResourceView**)&RT3
			);
			D3DContext->GSSetShaderResources(
				3,
				1,
				(ID3D11ShaderResourceView**)&RT3
			);
			D3DContext->PSSetShaderResources(
				3,
				1,
				(ID3D11ShaderResourceView**)&RT3
			);
		}
	};
	
	// Shaders

	//--------------------------------------------------------------
	//	p_batched_lines
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API p_batched_lines
	{
#pragma pack (push,16)
		struct Data
		{
			float4x4	viewProjectionMatrix;
			float4	lineColor;
		};
#pragma pack (pop)
	
	public:	// Constant buffers (manually updated)
		static TypedConstantBuffer< Data >	cb_Data;
	
	public:	// Public methods
		static inline void Set()
		{
			// Bind constant buffers
			cb_Data.BindTo_VS(0);
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
		static void Load( const ShaderInfo& shaderInfo, GrShaderSystem* compiler );
	
	private:
		p_batched_lines() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_batched_lines);
	};
	
	//--------------------------------------------------------------
	//	p_fullscreen_colored_triangle_shader
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API p_fullscreen_colored_triangle_shader
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
		enum { UID = 1 }; // unique index
		enum { NumInstances = 1 }; // number of unique combinations
	
	public_internal:
		static void Initialize();
		static void Shutdown();
		
		static void GetLoadInfo( ShaderInfo & d );
		static void Load( const ShaderInfo& shaderInfo, GrShaderSystem* compiler );
	
	private:
		p_fullscreen_colored_triangle_shader() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_fullscreen_colored_triangle_shader);
	};
	
	//--------------------------------------------------------------
	//	p_fullscreen_textured_triangle_shader
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API p_fullscreen_textured_triangle_shader
	{
	
	public:	// Sampler states (set manually)
		static ID3D11SamplerState* linearSampler;
	
	public:	// Shader resources (set manually)
		static ID3D11ShaderResourceView* sourceTexture;
	
	public:	// Public methods
		static inline void Set()
		{
			
			// Bind constant buffers
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
		enum { UID = 2 }; // unique index
		enum { NumInstances = 1 }; // number of unique combinations
	
	public_internal:
		static void Initialize();
		static void Shutdown();
		
		static void GetLoadInfo( ShaderInfo & d );
		static void Load( const ShaderInfo& shaderInfo, GrShaderSystem* compiler );
	
	private:
		p_fullscreen_textured_triangle_shader() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_fullscreen_textured_triangle_shader);
	};
	
	//--------------------------------------------------------------
	//	p_test_shader
	//	uses Shared_Globals	
	//--------------------------------------------------------------
	//
	struct MX_GRAPHICS_API p_test_shader
	{
#pragma pack (push,16)
		struct Data
		{
			float4x4	wvp;
		};
#pragma pack (pop)
	
	public:	// Constant buffers (manually updated)
		static TypedConstantBuffer< Data >	cb_Data;
	
	public:	// Shader resources (set manually)
		static ID3D11ShaderResourceView* diffuseTexture;
	
	public:	// Public methods
		static inline void Set()
		{
			// Bind constant buffers
			cb_Data.BindTo_VS(1);
			
			// Bind shader resources
			D3DContext->PSSetShaderResources(
				0,
				1,
				(ID3D11ShaderResourceView**)&diffuseTexture
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
		enum { UID = 3 }; // unique index
		enum { NumInstances = 1 }; // number of unique combinations
	
	public_internal:
		static void Initialize();
		static void Shutdown();
		
		static void GetLoadInfo( ShaderInfo & d );
		static void Load( const ShaderInfo& shaderInfo, GrShaderSystem* compiler );
	
	private:
		p_test_shader() {}
		
		static ShaderInstance	shaderInstances[1];
	
	private:PREVENT_COPY(p_test_shader);
	};
	
	
#pragma pack (push,1)
	
	//--------------------------------------------------------------
	//	Vertex_P3F
	//--------------------------------------------------------------
	//
	struct Vertex_P3F
	{
		float3		xyz;	// Position ( position )		
		
		// 12 bytes
		
		enum { Stride = 12 };	// size of a single vertex, in bytes
		
		// vertex components enum
		enum Elements
		{
			xyz_index = 0,
			NumElements = 1
		};
		
		// size of each element, in bytes
		enum ElementSizes
		{
			xyz_size = 12,
		};
		
		// vertex component mask
		enum
		{
			Mask = 0
				| (1 << VEU_Position)
		};
		
		// vertex buffer streams
		enum Streams
		{
			xyz_stream = 0,
			NumStreams = 1
		};
		
		// Strides of each vertex buffer.
		// Each stride is the size (in bytes) of the elements
		// that are to be used from that vertex buffer.
		enum StreamStrides
		{
			xyz_stream_size = 12,
		};
		
		// Offsets of each element relative to the corresponding vertex buffer stream.
		enum ElementOffsetsWithinStream
		{
			xyz_stream_offset = 0,
		};
		
		typedef Vertex_P3F THIS_TYPE;
		
		enum { UID = 0 }; // unique index
		
	
	public:	// Public member functions
		static const UINT* GetElementStridesArray()
		{
			static const UINT elementStridesArray[1] = { 12 };
			return elementStridesArray;
		}
		static const UINT* GetStreamStridesArray()
		{
			static const UINT streamStridesArray[1] = { 12 };
			return streamStridesArray;
		}
		static void CreateVertexData( const IndexedMesh& src, VertexData& dest );
	
	public:	// Input layout
		static InputLayout layout;
	};
	
	//--------------------------------------------------------------
	//	Vertex_P3F_TEX2F
	//--------------------------------------------------------------
	//
	struct Vertex_P3F_TEX2F
	{
		float3		xyz;	// Position ( position )		
		float2		uv;	// TexCoord ( texture coordinates )		
		
		// 12 + 8 = 20 bytes
		
		enum { Stride = 20 };	// size of a single vertex, in bytes
		
		// vertex components enum
		enum Elements
		{
			xyz_index = 0,
			uv_index = 1,
			NumElements = 2
		};
		
		// size of each element, in bytes
		enum ElementSizes
		{
			xyz_size = 12,
			uv_size = 8,
		};
		
		// vertex component mask
		enum
		{
			Mask = 0
				| (1 << VEU_Position)
				| (1 << VEU_TexCoords)
		};
		
		// vertex buffer streams
		enum Streams
		{
			xyz_uv_stream = 0,
			NumStreams = 1
		};
		
		// Strides of each vertex buffer.
		// Each stride is the size (in bytes) of the elements
		// that are to be used from that vertex buffer.
		enum StreamStrides
		{
			xyz_uv_stream_size = 20,
		};
		
		// Offsets of each element relative to the corresponding vertex buffer stream.
		enum ElementOffsetsWithinStream
		{
			xyz_stream_offset = 0,
			uv_stream_offset = 12,
		};
		
		typedef Vertex_P3F_TEX2F THIS_TYPE;
		
		enum { UID = 1 }; // unique index
		
	
	public:	// Public member functions
		static const UINT* GetElementStridesArray()
		{
			static const UINT elementStridesArray[2] = { 12, 8 };
			return elementStridesArray;
		}
		static const UINT* GetStreamStridesArray()
		{
			static const UINT streamStridesArray[1] = { 20 };
			return streamStridesArray;
		}
		static void CreateVertexData( const IndexedMesh& src, VertexData& dest );
	
	public:	// Input layout
		static InputLayout layout;
	};
	
	//--------------------------------------------------------------
	//	Vertex_P3F_TEX2F_N4UB
	//--------------------------------------------------------------
	//
	struct Vertex_P3F_TEX2F_N4UB
	{
		float3		xyz;	// Position ( position )		
		float2		uv;	// TexCoord ( texture coordinates )		
		rxNormal4		N;	// Normal ( normal (T') )		
		
		// 12 + 8 + 4 = 24 bytes
		
		enum { Stride = 24 };	// size of a single vertex, in bytes
		
		// vertex components enum
		enum Elements
		{
			xyz_index = 0,
			uv_index = 1,
			N_index = 2,
			NumElements = 3
		};
		
		// size of each element, in bytes
		enum ElementSizes
		{
			xyz_size = 12,
			uv_size = 8,
			N_size = 4,
		};
		
		// vertex component mask
		enum
		{
			Mask = 0
				| (1 << VEU_Position)
				| (1 << VEU_TexCoords)
				| (1 << VEU_Normal)
		};
		
		// vertex buffer streams
		enum Streams
		{
			xyz_uv_N_stream = 0,
			NumStreams = 1
		};
		
		// Strides of each vertex buffer.
		// Each stride is the size (in bytes) of the elements
		// that are to be used from that vertex buffer.
		enum StreamStrides
		{
			xyz_uv_N_stream_size = 24,
		};
		
		// Offsets of each element relative to the corresponding vertex buffer stream.
		enum ElementOffsetsWithinStream
		{
			xyz_stream_offset = 0,
			uv_stream_offset = 12,
			N_stream_offset = 20,
		};
		
		typedef Vertex_P3F_TEX2F_N4UB THIS_TYPE;
		
		enum { UID = 2 }; // unique index
		
	
	public:	// Public member functions
		static const UINT* GetElementStridesArray()
		{
			static const UINT elementStridesArray[3] = { 12, 8, 4 };
			return elementStridesArray;
		}
		static const UINT* GetStreamStridesArray()
		{
			static const UINT streamStridesArray[1] = { 24 };
			return streamStridesArray;
		}
		static void CreateVertexData( const IndexedMesh& src, VertexData& dest );
	
	public:	// Input layout
		static InputLayout layout;
	};
	
	//--------------------------------------------------------------
	//	Vertex_P3F_TEX2F_N4UB_T4UB
	//--------------------------------------------------------------
	//
	struct Vertex_P3F_TEX2F_N4UB_T4UB
	{
		float3		xyz;	// Position ( position )		
		float2		uv;	// TexCoord ( texture coordinates )		
		rxNormal4		N;	// Normal ( normal (T') )		
		rxNormal4		T;	// Tangent ( tangent (P') )		
		
		// 12 + 8 + 4 + 4 = 28 bytes
		
		enum { Stride = 28 };	// size of a single vertex, in bytes
		
		// vertex components enum
		enum Elements
		{
			xyz_index = 0,
			uv_index = 1,
			N_index = 2,
			T_index = 3,
			NumElements = 4
		};
		
		// size of each element, in bytes
		enum ElementSizes
		{
			xyz_size = 12,
			uv_size = 8,
			N_size = 4,
			T_size = 4,
		};
		
		// vertex component mask
		enum
		{
			Mask = 0
				| (1 << VEU_Position)
				| (1 << VEU_TexCoords)
				| (1 << VEU_Normal)
				| (1 << VEU_Tangent)
		};
		
		// vertex buffer streams
		enum Streams
		{
			xyz_stream = 0,
			uv_N_T_stream = 1,
			NumStreams = 2
		};
		
		// Strides of each vertex buffer.
		// Each stride is the size (in bytes) of the elements
		// that are to be used from that vertex buffer.
		enum StreamStrides
		{
			xyz_stream_size = 12,
			uv_N_T_stream_size = 16,
		};
		
		// Offsets of each element relative to the corresponding vertex buffer stream.
		enum ElementOffsetsWithinStream
		{
			xyz_stream_offset = 0,
			uv_stream_offset = 0,
			N_stream_offset = 8,
			T_stream_offset = 12,
		};
		
		typedef Vertex_P3F_TEX2F_N4UB_T4UB THIS_TYPE;
		
		enum { UID = 3 }; // unique index
		
	
	public:	// Public member functions
		static const UINT* GetElementStridesArray()
		{
			static const UINT elementStridesArray[4] = { 12, 8, 4, 4 };
			return elementStridesArray;
		}
		static const UINT* GetStreamStridesArray()
		{
			static const UINT streamStridesArray[2] = { 12, 16 };
			return streamStridesArray;
		}
		static void CreateVertexData( const IndexedMesh& src, VertexData& dest );
	
	public:	// Input layout
		static InputLayout layout;
	};
	
	//--------------------------------------------------------------
	//	Vertex_P4F_COL4F
	//--------------------------------------------------------------
	//
	struct Vertex_P4F_COL4F
	{
		float4		xyzw;	// Position ( transformed position )		
		float4		rgba;	// Color ( vertex color )		
		
		// 16 + 16 = 32 bytes
		
		enum { Stride = 32 };	// size of a single vertex, in bytes
		
		// vertex components enum
		enum Elements
		{
			xyzw_index = 0,
			rgba_index = 1,
			NumElements = 2
		};
		
		// size of each element, in bytes
		enum ElementSizes
		{
			xyzw_size = 16,
			rgba_size = 16,
		};
		
		// vertex component mask
		enum
		{
			Mask = 0
				| (1 << VEU_Position)
				| (1 << VEU_Color)
		};
		
		// vertex buffer streams
		enum Streams
		{
			xyzw_rgba_stream = 0,
			NumStreams = 1
		};
		
		// Strides of each vertex buffer.
		// Each stride is the size (in bytes) of the elements
		// that are to be used from that vertex buffer.
		enum StreamStrides
		{
			xyzw_rgba_stream_size = 32,
		};
		
		// Offsets of each element relative to the corresponding vertex buffer stream.
		enum ElementOffsetsWithinStream
		{
			xyzw_stream_offset = 0,
			rgba_stream_offset = 16,
		};
		
		typedef Vertex_P4F_COL4F THIS_TYPE;
		
		enum { UID = 4 }; // unique index
		
	
	public:	// Public member functions
		static const UINT* GetElementStridesArray()
		{
			static const UINT elementStridesArray[2] = { 16, 16 };
			return elementStridesArray;
		}
		static const UINT* GetStreamStridesArray()
		{
			static const UINT streamStridesArray[1] = { 32 };
			return streamStridesArray;
		}
		static void CreateVertexData( const IndexedMesh& src, VertexData& dest );
	
	public:	// Input layout
		static InputLayout layout;
	};
#pragma pack (pop)
}
