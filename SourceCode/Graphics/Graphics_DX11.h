#pragma once

#include <Core/Editor.h>
#include <Graphics/Graphics.h>


//---------------------------------------------------------------
//	Include headers for DirectX 11 programming.
//---------------------------------------------------------------

#define MX_COMPILE_WITH_D3D9	1	// needed for profiling with PIX
#define MX_COMPILE_WITH_D3D10	0
#define MX_COMPILE_WITH_D3D11	1


#if (MX_COMPILE_WITH_D3D9 || MX_COMPILE_WITH_D3D10 || MX_COMPILE_WITH_D3D11)

	// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
	// This makes D3D objects work well in the debugger watch window, but slows down 
	// performance slightly.

	#if MX_DEBUG && (!defined( D3D_DEBUG_INFO ))
	#	define D3D_DEBUG_INFO
	#endif

	// DirectX error codes.

	#include <DxErr.h>
	#if MX_AUTOLINK
		#pragma comment( lib, "DxErr.lib" )
	#endif


	#if MX_COMPILE_WITH_D3D9
		#include <d3d9.h>
		#include <d3dx9.h>

		#if MX_AUTOLINK
			#pragma comment( lib, "d3d9.lib" )

			#if MX_DEBUG
				#pragma comment( lib, "d3dx9d.lib" )
			#else
				#pragma comment( lib, "d3dx9.lib" )
			#endif
		#endif // MX_AUTOLINK
	#endif // MX_COMPILE_WITH_D3D9


	#if MX_COMPILE_WITH_D3D10
		#include <D3D10.h>
		#include <D3DX10.h>

		#if MX_AUTOLINK
			#pragma comment( lib, "d3d10.lib" )

			#if MX_DEBUG
				#pragma comment( lib, "d3dx10d.lib" )
			#else
				#pragma comment( lib, "d3dx10.lib" )
			#endif
		#endif // MX_AUTOLINK

	#endif // MX_COMPILE_WITH_D3D10


	#if MX_COMPILE_WITH_D3D11

		#include <D3Dcommon.h>
		#include <D3Dcompiler.h>
		#include <D3D11.h>
		#include <D3DX11.h>

		#if MX_AUTOLINK
			#pragma comment( lib, "d3d11.lib" )

			#if MX_DEBUG
				#pragma comment( lib, "d3dx11d.lib" )
			#else
				#pragma comment( lib, "d3dx11.lib" )
			#endif
		#endif // MX_AUTOLINK

	#endif // MX_COMPILE_WITH_D3D11

	#if MX_COMPILE_WITH_D3D10 || MX_COMPILE_WITH_D3D11
		#include <DXGI.h>
	#endif

	#if MX_AUTOLINK
		#pragma comment (lib, "dxgi.lib")
		#pragma comment( lib, "dxguid.lib" )
		#pragma comment( lib, "d3dcompiler.lib" )
		#pragma comment( lib, "winmm.lib" )
		#pragma comment( lib, "comctl32.lib" )
	#endif // MX_AUTOLINK

#endif // D3D


//------------------------------------------------------------------------
//	Useful macros
//------------------------------------------------------------------------

#include <Base/Util/DirectX_Helpers.h>


//------------------------------------------------------------------------
//	Device context
//------------------------------------------------------------------------

#include <Graphics/DX11/D3D11DeviceContext.h>


//------------------------------------------------------------------------
//	Debugging and run-time checks - they slow down a lot!
//------------------------------------------------------------------------

#if RX_DEBUG_RENDERER

	#define RX_ASSERT( x )		Assert( x )
	#define dxchk( expr )		for( HRESULT hr = (expr); ; )\
								{\
									if( SUCCEEDED( hr ) )\
									{\
									}\
									else\
									{\
										extern void dxError( HRESULT errorCode );\
										dxError( hr );\
										mxDEBUG_BREAK;\
									}\
									break;\
								}

#else

	#define RX_ASSERT( x )
	#define dxchk( expr )		expr

#endif // RX_DEBUG_RENDERER

//
// Returns a string corresponding to the given error code.
//
const char* D3D_GetErrorCodeString( HRESULT hErrorCode );


/*
=======================================================================

	Render state objects

=======================================================================
*/

mxDECLARE_COM_PTR(ID3D11RasterizerState);
mxDECLARE_COM_PTR(ID3D11DepthStencilState);
mxDECLARE_COM_PTR(ID3D11BlendState);
mxDECLARE_COM_PTR(ID3D11SamplerState);

struct StateBlock
{
	// read-only data:

	ID3D11RasterizerStatePtr	rasterizer;

	ID3D11DepthStencilStatePtr	depthStencil;
	UINT						stencilRef;

	ID3D11BlendStatePtr	blend;
	FLOAT				blendFactorRGBA[4];
	UINT				sampleMask;

public:
	FORCEINLINE void Set( ID3D11DeviceContext* theContext ) const
	{
		theContext->RSSetState( this->rasterizer );

		theContext->OMSetDepthStencilState( this->depthStencil, this->stencilRef );

		theContext->OMSetBlendState( this->blend, this->blendFactorRGBA, this->sampleMask );
	}

public:
	StateBlock();
	NO_ASSIGNMENT(StateBlock);
};

/*
=======================================================================

	Render targets and depth-stencil surfaces

=======================================================================
*/

struct RenderTargetInfo
{
	D3D11_TEXTURE2D_DESC			Texture2D;
	D3D11_RENDER_TARGET_VIEW_DESC	RTV;
	D3D11_SHADER_RESOURCE_VIEW_DESC	SRV;
};

struct RenderTarget
{
	dxPtr< ID3D11Texture2D >			pTexture;
	dxPtr< ID3D11RenderTargetView >		pRTV;
	dxPtr< ID3D11ShaderResourceView >	pSRV;
	//U2	cachedWidth, cachedHeight;

public:
	RenderTarget();

	void Create( D3D11_TEXTURE2D_DESC * pTexDesc );

	void Release();

	bool IsNull() const
	{
		return pTexture.IsNull()
			&& pRTV.IsNull()
			&& pSRV.IsNull();
	}

	void CalcWidthHeight( UINT &OutWidth, UINT &OutHeight ) const;

	UINT CalcSizeInBytes() const;

private:
	PREVENT_COPY(RenderTarget);
};

// Code for multiple render targets is auto-generated by the tools.
//struct MultiRenderTarget
//{
//};

struct DepthStencilInfo
{
	D3D11_TEXTURE2D_DESC			Texture2D;
	D3D11_DEPTH_STENCIL_VIEW_DESC	DSV;
	D3D11_SHADER_RESOURCE_VIEW_DESC	SRV;
};

// Depth-Stencil surface
//
struct DepthStencil
{
	dxPtr< ID3D11Texture2D >			pTexture;
	dxPtr< ID3D11DepthStencilView >		pDSV;
	dxPtr< ID3D11ShaderResourceView >	pSRV;

public:
	DepthStencil();

	void Create(
		UINT nWidth,
		UINT nHeight,
		DXGI_FORMAT eFormat,
		bool bSampleDepthStencil,
		UINT nSampleCount = 1,
		UINT nSampleQuality = 0
	);

	void Release()
	{
		pTexture = nil;
		pDSV = nil;
		pSRV = nil;
	}
	bool IsNull() const
	{
		return pTexture.IsNull()
			&& pDSV.IsNull()
			&& pSRV.IsNull();
	}

	void CalcWidthHeight( UINT &OutWidth, UINT &OutHeight ) const;

	UINT CalcSizeInBytes() const;

private:
	PREVENT_COPY(DepthStencil);
};

inline
DXGI_FORMAT D3D_GetDepthStencilSRVFormat( DXGI_FORMAT format )
{
	if( format == DXGI_FORMAT_D16_UNORM ) {
		return DXGI_FORMAT_R16_TYPELESS;
	}
	else if( format == DXGI_FORMAT_D24_UNORM_S8_UINT ){
		return DXGI_FORMAT_R24G8_TYPELESS;
	}
	else {
		return DXGI_FORMAT_R32_TYPELESS;
	}
}

/*
=======================================================================

	GPU programs

=======================================================================
*/

mxDECLARE_COM_PTR(ID3DBlob);

mxDECLARE_COM_PTR(ID3D11VertexShader);
mxDECLARE_COM_PTR(ID3D11HullShader);
mxDECLARE_COM_PTR(ID3D11DomainShader);
mxDECLARE_COM_PTR(ID3D11GeometryShader);
mxDECLARE_COM_PTR(ID3D11PixelShader);

mxDECLARE_COM_PTR(ID3D11ComputeShader);

// shader source code (text string)
//
struct ShaderSource 
{
	const char* code;	// string *without* null terminator
	SizeT codeLength;	// number of characters in the string

public:
	ShaderSource()
	{
		code = nil;
		codeLength = 0;
	}
	bool isOk() const
	{
		return (code != nil)
			&& (codeLength > 0)
			;
	}
};

// shader #define
//
struct dxShaderMacro : public D3D_SHADER_MACRO
{
	FORCEINLINE dxShaderMacro()
	{
		Name = nil;
		Definition = nil;
	}
	FORCEINLINE dxShaderMacro( LPCSTR name, LPCSTR definition )
	{
		this->Set( name, definition );
	}
	FORCEINLINE void Set( LPCSTR name, LPCSTR definition )
	{
		Name = name;
		Definition = definition;
	}
	FORCEINLINE bool operator == ( const dxShaderMacro& other ) const
	{
		return Name == other.Name
			&& Definition == other.Definition;
	}
	FORCEINLINE bool operator != ( const dxShaderMacro& other ) const
	{
		return !(*this == other);
	}

private:
	void StaticChecks()
	{
		mxSTATIC_ASSERT(sizeof(*this)==sizeof(D3D_SHADER_MACRO));
	}
};

// Maximum allowed number of '#define's in a single effect file.
// NOTE: it must be a power of two!
//
enum { RX_MAX_SHADER_DEFINES = 32 };

/*
struct ShaderMacrosArray
{
	UINT			num;
	ShaderMacro		macros[ MAX_SHADER_MACROS_IN_FILE ];

	FORCEINLINE ShaderMacrosArray()
		: num(0)
	{}

	FORCEINLINE UINT Num() const
	{
		return num;
	}

	FORCEINLINE void Add( const ShaderMacro& newMacro )
	{
		Assert( num < MAX_SHADER_MACROS_IN_FILE-1 );
#if RX_DEBUG_SHADERS
		Assert( FindIndex( newMacro ) == INDEX_NONE );
#endif
		// instead of checking for overflow, we just mask the index so it wraps around
		UINT index = num & (MAX_SHADER_MACROS_IN_FILE-1);
		macros[ index ] = newMacro;
		++num;
	}

	FORCEINLINE const ShaderMacro & operator [] ( UINT index ) const
	{
		Assert( index < MAX_SHADER_MACROS_IN_FILE );
		return macros[index];
	}
	FORCEINLINE ShaderMacro & operator [] ( UINT index )
	{
		Assert( index < MAX_SHADER_MACROS_IN_FILE );
		return macros[index];
	}

	FORCEINLINE const ShaderMacro* Ptr() const
	{
		return macros;
	}
	FORCEINLINE ShaderMacro* Ptr()
	{
		return macros;
	}

	FORCEINLINE UINT FindIndex( const ShaderMacro& macro )
	{
		for( UINT iMacro = 0; iMacro < num; iMacro++ )
		{
			if( macro == macros[iMacro] ) {
				return iMacro;
			}
		}
		return INDEX_NONE;
	}

	FORCEINLINE UINT MemoryUsed() const
	{
		return sizeof(num) + sizeof(macros);
	}

	FORCEINLINE UINT GetHashCode() const
	{
		const UINT length = sizeof(num) + num * sizeof(macros[0]);
		return CRC32_BlockChecksum( this, length );
	}

	FORCEINLINE void Clear( bool bZeroOut = false )
	{
		num = 0;
		if( bZeroOut ) {
			MemZero( macros, sizeof(macros) );
		}
	}

	FORCEINLINE void SetNum( UINT newNum )
	{
		Assert( num <= MAX_SHADER_MACROS_IN_FILE );
		num = newNum;
	}

private:
	static void _checks()
	{
		StaticAssert(TIsPowerOfTwo<MAX_SHADER_MACROS_IN_FILE>::value);
	}
};
*/

typedef TStaticList_InitZeroed< dxShaderMacro, RX_MAX_SHADER_DEFINES >	dxShaderMacros;



typedef void F_GetShaderMacros( UINT iShaderInstance, dxShaderMacros &OutDefines );

/*
-----------------------------------------------------------------------------
	ShaderInstance - is a compiled shader instance,
	represents a specific shader variation.
	This is actually used for rendering.

	NOTE: there can be thousands of them,
	so keep the size of this structure small (and aligned to a power of two)
-----------------------------------------------------------------------------
*/
struct ShaderInstance
{
	ID3D11VertexShaderPtr	vertexShader;

#if RX_HW_D3D11
	ID3D11HullShaderPtr		hullShader;
	ID3D11DomainShaderPtr	domainShader;
	ID3D11GeometryShaderPtr	geometryShader;	// GS is available in D3D10 too, but it's used infrequently
#endif // RX_HW_D3D11

	ID3D11PixelShaderPtr	pixelShader;
};

// used to identify variations (permutations) of some shader program
// (it's actually a bit mask constructed from #define's)
typedef U4 rxShaderInstanceId;

// used for serialization;
//@todo: use 2-byte code offsets? we rarely have more than 2^16 combinations?
struct ShaderInstanceData
{
#pragma pack (push,1)

	U4	VS_CodeIndex;

#if RX_HW_D3D11
	U4	HS_CodeIndex;
	U4	DS_CodeIndex;
	U4	GS_CodeIndex;
#endif // RX_HW_D3D11

	U4	PS_CodeIndex;

#pragma pack (pop)

public:
	ShaderInstanceData()
	{
		MemSet( this, INDEX_NONE, sizeof *this );
	}
	bool isOk() const
	{
		return VS_CodeIndex != INDEX_NONE
			//&& PS_CodeIndex != INDEX_NONE	//<- PS can be null when building shadow depth map
			;
	}
};
mxDECLARE_POD_TYPE(ShaderInstanceData);

//=========================================================
//	Shader programs
//=========================================================

struct GrShaderData;
class GrShaderSystem;

// unique shader program id - used to identify shader source programs
typedef UINT ShaderId;

// this structure contains all the necessary info
// needed to manipulate the shader;
// used for creating shader instances among other things.
// NOTE: its memory address mustn't change,
// i.e. it cannot be a stack variable, cannot be moved in memory,
// instances of GrShaderData are statically allocated
// and usually linked with their corresponding C++ classes.
//
struct GrShaderData
{
	// unique index
	ShaderId	uniqueId;

	// pointer to static string - unique shader name (for development)
	const char *	name;

	// pointer to static string - source file name (for development)
	const char *	file;

	// pointers to static strings
	const char *	VS_EntryPoint;	// vertex shader entry
	const char *	HS_EntryPoint;	// hull shader entry
	const char *	DS_EntryPoint;	// domain shader entry
	const char *	GS_EntryPoint;	// geometry shader entry
	const char *	PS_EntryPoint;	// pixel shader entry

	UINT			numInstances;	// number of unique combinations
	ShaderInstance*	instances;	// pointer to array of instances

	F_GetShaderMacros*	getDefines;	// can be null

//-- BEGIN INTERNAL USE
	// pointer to static array; used for mainly serialization
	ShaderInstanceData*	instancesData;

//-- END INTERNAL USE

public:
	GrShaderData();

	bool isOk() const;

private:
	PREVENT_COPY(GrShaderData);
};

/*
=======================================================================

	Shader resources

=======================================================================
*/

mxDECLARE_COM_PTR(ID3D11Resource);
mxDECLARE_COM_PTR(ID3D11Texture1D);
mxDECLARE_COM_PTR(ID3D11Texture2D);
mxDECLARE_COM_PTR(ID3D11Texture3D);

mxDECLARE_COM_PTR(ID3D11ShaderResourceView);

/*
=======================================================================

	Buffers

=======================================================================
*/

mxDECLARE_COM_PTR(ID3D11Buffer);

inline void* D3D_LockBuffer( ID3D11DeviceContext* context, ID3D11Buffer* buffer )
{
	D3D11_MAPPED_SUBRESOURCE	mappedResource;

	dxchk(context->Map(
		buffer,	// ID3D11Resource *pResource
		0,	// UINT Subresource
		D3D11_MAP_WRITE_DISCARD,	// D3D11_MAP MapType
		0,	// UINT MapFlags
		&mappedResource	// D3D11_MAPPED_SUBRESOURCE *pMappedResource
	));

	return mappedResource.pData;
}

inline void D3D_UnlockBuffer( ID3D11DeviceContext* context, ID3D11Buffer* buffer )
{
	context->Unmap(
		buffer,	// ID3D11Resource *pResource
		0	// UINT Subresource
	);
}

class dxScopedLock
{
	D3D11_MAPPED_SUBRESOURCE	m_mappedData;
	dxPtr<ID3D11DeviceContext>	m_pD3DContext;
	dxPtr<ID3D11Resource>		m_pDstResource;
	UINT						m_iSubResource;

public:
	FORCEINLINE dxScopedLock( ID3D11DeviceContext* pD3DContext, ID3D11Resource* pDstResource, D3D11_MAP eMapType, UINT iSubResource = 0, UINT nMapFlags = 0 )
	{
		m_pD3DContext = pD3DContext;
		m_pDstResource = pDstResource;
		m_iSubResource = iSubResource;
		dxchk(pD3DContext->Map( pDstResource, iSubResource, eMapType, nMapFlags, &m_mappedData ));
	}
	FORCEINLINE ~dxScopedLock()
	{
		m_pD3DContext->Unmap( m_pDstResource, m_iSubResource );
	}
	template< typename TYPE >
	FORCEINLINE TYPE As()
	{
		return static_cast< TYPE >( m_mappedData.pData );
	}
	FORCEINLINE void* ToVoidPtr()
	{
		return m_mappedData.pData;
	}
	FORCEINLINE D3D11_MAPPED_SUBRESOURCE& GetMappedData()
	{
		return m_mappedData;
	}
};

/*
=======================================================================

	Geometry

=======================================================================
*/

// Input layout describes the input-buffer data for the input-assembler stage.
//

mxDECLARE_COM_PTR(ID3D11InputLayout);

/*
-----------------------------------------------------------------------------
	rxHardwareBuffer
-----------------------------------------------------------------------------
*/
struct rxHardwareBuffer
{
	dxPtr< ID3D11Buffer >	pD3DBuffer;

public:
	void Clear()
	{
		this->pD3DBuffer = nil;
	}

	bool IsValid() const
	{
		return this->pD3DBuffer != nil;
	}
	bool IsNull() const
	{
		return !IsValid();
	}

	void* Lock();
	void Unlock();
};


/*
-----------------------------------------------------------------------------
	GrIndexBuffer
-----------------------------------------------------------------------------
*/
class GrIndexBuffer : public rxHardwareBuffer
{
	DXGI_FORMAT		format;	// this can only be DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT

	friend class D3D11ResourceSystem;

public:
	inline GrIndexBuffer()
	{
		this->Clear();
	}

	void Create( const void* data, const UINT size, const UINT stride );

	void Clear()
	{
		rxHardwareBuffer::Clear();
		this->format = DXGI_FORMAT_UNKNOWN;
	}

	FORCEINLINE void Bind( ID3D11DeviceContext* theContext ) const
	{
		theContext->IASetIndexBuffer(
			this->pD3DBuffer,
			this->format,
			0	// Offset
		);
	}
};

template< typename INDEX_TYPE >
struct GrIndexBufferTemplate : public GrIndexBuffer
{
	inline GrIndexBufferTemplate()
	{}

	void Create( const INDEX_TYPE* srcData, const UINT dataSize )
	{
		GrIndexBuffer::Create( srcData, dataSize, sizeof srcData[0] );
	}
};

typedef GrIndexBufferTemplate< U2 >	GrIndexBuffer16;
typedef GrIndexBufferTemplate< U4 >	GrIndexBuffer32;


/*
-----------------------------------------------------------------------------
	GrVertexData
-----------------------------------------------------------------------------
*/
mxALIGN_16(struct GrVertexData)
{
	// vertex buffers for each stream
	ID3D11BufferPtr			m_streams[ RX_MAX_INPUT_SLOTS ];

	UINT	m_streamStrides[ RX_MAX_INPUT_SLOTS ];
	UINT	m_streamOffsets[ RX_MAX_INPUT_SLOTS ];
	UINT	m_numStreams;	// number of vertex buffers

	// vertex declaration in D3D9 parlance
	TPtr< ID3D11InputLayout >	m_inputLayout;

public:
	inline GrVertexData()
	{
		ZERO_OUT(m_streamStrides);
		ZERO_OUT(m_streamOffsets);
		m_numStreams = 0;
	}

	FORCEINLINE void Bind( ID3D11DeviceContext* pDeviceContext ) const
	{
		pDeviceContext->IASetInputLayout( m_inputLayout.Ptr );

		pDeviceContext->IASetVertexBuffers(
			0,	// Start slot
			m_numStreams,
			c_cast(ID3D11Buffer**) m_streams,
			m_streamStrides,
			m_streamOffsets
		);
	}
};





/*
--------------------------------------------------------------
	GrResourceSystem
	low-level graphics resources manager
--------------------------------------------------------------
*/
class GrResourceSystem : SingleInstance<GrResourceSystem>, public AEditable
{
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::HeapGraphics,GrResourceSystem);

public:

	// render states

	virtual void Create_SamplerState(
		ID3D11SamplerStatePtr &r,
		const D3D11_SAMPLER_DESC& desc
	) = 0;

	virtual void Destroy_SamplerState(
		ID3D11SamplerStatePtr &r
	) = 0;


	virtual void Create_DepthStencilState(
		ID3D11DepthStencilStatePtr &r,
		const D3D11_DEPTH_STENCIL_DESC& desc
	) = 0;

	virtual void Destroy_DepthStencilState(
		ID3D11DepthStencilStatePtr &r
	) = 0;

	virtual void Create_BlendState(
		ID3D11BlendStatePtr &r,
		const D3D11_BLEND_DESC& desc
	) = 0;

	virtual void Destroy_BlendState(
		ID3D11BlendStatePtr &r
	) = 0;


	virtual void Create_RasterizerState(
		ID3D11RasterizerStatePtr &r,
		const D3D11_RASTERIZER_DESC& desc
	) = 0;

	virtual void Destroy_RasterizerState(
		ID3D11RasterizerStatePtr &r
	) = 0;

	// render targets

	virtual void Create_RenderTarget(
		RenderTarget &r,
		const D3D11_TEXTURE2D_DESC& tex2DDesc,
		const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc,
		const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc
	) = 0;

	virtual void Destroy_RenderTarget(
		RenderTarget &r
	) = 0;

	// input layouts

	virtual void Create_InputLayout(
		ID3D11InputLayoutPtr &r,
		const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
		UINT NumElements
	) = 0;

	virtual void Destroy_InputLayout(
		ID3D11InputLayoutPtr &r
	) = 0;


	// vertex buffers

	virtual void Create_VertexBuffer(
		ID3D11BufferPtr & r,
		UINT size,
		UINT stride,
		const void* data = nil,
		bool dynamic = false
	) = 0;

	virtual void Destroy_VertexBuffer(
		ID3D11BufferPtr &r
	) = 0;


	virtual void Create_IndexBuffer(
		GrIndexBuffer & r,
		UINT size,
		UINT stride,
		const void* data = nil,
		bool dynamic = false
	) = 0;

	virtual void Destroy_IndexBuffer(
		GrIndexBuffer &r
	) = 0;

public:
	virtual ~GrResourceSystem()
	{}
protected:
	GrResourceSystem()
	{}
};

/*
--------------------------------------------------------------
	FileIncludeInterface
	used for #including files when compiling shaders
--------------------------------------------------------------
*/
class FileIncludeInterface
{
public:
	virtual ~FileIncludeInterface() {}

	// A user-implemented method for opening a shader #include file.
	virtual bool Open(
		const ANSICHAR* fileName,
		const void**fileData,
		UINT *fileSize
	) = 0;

	// A user-implemented method for closing a shader #include file.
	virtual bool Close(
		const void* fileData
	) = 0;
};

// null-terminated text buffer
//
struct GrCharBuffer : public TList< ANSICHAR >
{
public:
	typedef GrCharBuffer		THIS_TYPE;
	typedef TList< ANSICHAR >	BASE_TYPE;

	typedef BASE_TYPE::ITEM_TYPE	ITEM_TYPE;

public:
	explicit GrCharBuffer( HMemory hMemoryMgr = EMemHeap::HeapTemp )
		: BASE_TYPE( hMemoryMgr )
	{}

	FORCEINLINE ANSICHAR* ToChars()
	{
		return ToPtr();
	}
	FORCEINLINE const ANSICHAR* ToChars() const
	{
		return ToPtr();
	}

	FORCEINLINE UINT Length() const
	{
		return GetDataSize();
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s), UINT length )
	{
		Assert( ToChars() != s );
		this->SetNum( length + 1 );	// reserve one char for null terminator
		MemCopy( this->ToPtr(), s, length );
		this->SetLast(nil);	// null terminator
	}

	FORCEINLINE
	void SetString( const ANSICHAR* RESTRICT_PTR(s) )
	{
		this->SetString( s, mxStrLenAnsi(s) );
	}
};

// marker
#define GR_TEXT( ansiChars )	ansiChars


struct GrShaderCacheInfo
{
	PCSTR pathToShaderSource;
	PCSTR pathToShaderCache;
	// if the shader source files have changed then the shader cache should be rebuilt
	FileTime timeStamp;	// shader source time stamp
};

/*
--------------------------------------------------------------
	GrShaderSystem
	High-level shader system
--------------------------------------------------------------
*/
class GrShaderSystem
	: public AEditable, SingleInstance<GrShaderSystem>
{
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::HeapGraphics,GrShaderSystem);

public:

	virtual void Create_ConstantBuffer(
		ID3D11BufferPtr &r,
		UINT size,	// Size of the constant buffer, in bytes. Must be aligned on 16-byte boundary
		const void* data = nil	// Pointer to initial data. [can be nil]
	) = 0;

	virtual void Destroy_ConstantBuffer(
		ID3D11BufferPtr &r
	) = 0;


	virtual void RegisterShader(
		GrShaderData * shaderInfo
	) = 0;

	virtual void GetShaderSource(
		const ANSICHAR* fileName,
		const ANSICHAR*&fileData,
		UINT &fileSize
	) = 0;

	virtual void ReloadShader(
		GrShaderData * shaderInfo,
		const ANSICHAR* source,
		const UINT sourceLength
	) {};




	// Compiles all shaders.
	virtual void CompileShaders(
		const GrShaderCacheInfo& settings
	) = 0;

	virtual void SaveCompiledShaders(
		PCSTR pathToShaderCache
	) = 0;

	virtual const char* GetCompilationErrors()
	{
		return nil;
	}

public:
	virtual ~GrShaderSystem()
	{}
protected:
	GrShaderSystem()
	{}
};


#if 0
//
//	TShadowCBuffer<T> - is a typed constant buffer and it can only be updated as a whole.
//	(i.e. it cannot be mapped, but can only be updated using a copy operation.)
//	It is not possible to use this to partially update a shader-constant buffer.
//
//	NOTE: it keeps a shadow copy in system memory.
//
//	NOTE: Be careful NOT to use any class with a virtual function table,
//	or appropriately pad your shader side constant buffer...
//
template< typename STRUCT >
class TShadowCBuffer : public STRUCT
{
public:
	TShadowCBuffer()
	{}
	~TShadowCBuffer()
	{}

	void Create( GrShaderSystem* shaderSystem )
	{
		UINT bufferSize = ALIGN16( sizeof STRUCT );
		shaderSystem->Create_ConstantBuffer( mD3DBuffer, bufferSize );
	}
	void Destroy( GrShaderSystem* shaderSystem )
	{
		shaderSystem->Destroy_ConstantBuffer( mD3DBuffer );
	}

	FORCEINLINE void Update( ID3D11DeviceContext* theContext )
	{
		theContext->UpdateSubresource(
			mD3DBuffer,	// ID3D11Resource *pDstResource
			0,			// UINT DstSubresource
			nil,		// const D3D11_BOX *pDstBox
			this,		// const void *pSrcData
			0,			// UINT SrcRowPitch
			0			// UINT SrcDepthPitch
		);
	}

	// 'slot' must be in range [0..D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) !

	FORCEINLINE void BindTo_VS( ID3D11DeviceContext* theContext, UINT slot )
	{
		theContext->VSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_GS( ID3D11DeviceContext* theContext, UINT slot )
	{
		theContext->GSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS( ID3D11DeviceContext* theContext, UINT slot )
	{
		theContext->PSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}

private:
	ID3D11BufferPtr		mD3DBuffer;
};
#endif


//
//	TConstantBuffer
//
template< typename STRUCT >
class TConstantBuffer
{
	dxPtr< ID3D11Buffer >	mD3DBuffer;

	// TConstantBuffer must not contain any other members except a pointer to ID3D11Buffer,
	// (arrays of TConstantBuffer they are treated like (ID3D11Buffer**) in generated code)
	//
	mxSTATIC_ASSERT( sizeof TConstantBuffer<STRUCT> == sizeof(ID3D11Buffer*) );

public:
	enum { BUFFER_SIZE = sizeof STRUCT };

	TConstantBuffer()
	{}
	~TConstantBuffer()
	{}

	void Create( GrShaderSystem* shaderSystem )
	{
		const SizeT bufferSize = ALIGN16( sizeof STRUCT );
		mD3DBuffer = D3D_CreateMappableConstantBuffer( bufferSize );
	}

	template< typename U >
	void CopyFromExisting( TConstantBuffer<U>& other )
	{
		Assert( other.IsValid() );

		mxSTATIC_ASSERT( sizeof U >= sizeof STRUCT );

		mD3DBuffer = other.mD3DBuffer;
	}

	void Destroy( GrShaderSystem* shaderSystem )
	{
		mD3DBuffer = nil;
	}

	FORCEINLINE STRUCT* Map( ID3D11DeviceContext* d3dContext )
	{
		return (STRUCT*)D3D_LockBuffer( d3dContext, mD3DBuffer );
	}

	FORCEINLINE void Unmap( ID3D11DeviceContext* d3dContext )
	{
		D3D_UnlockBuffer( d3dContext, mD3DBuffer );
	}

	template< typename TYPE >
	FORCEINLINE TYPE* MapAs()
	{
		return (TYPE*) this->Map( sizeof(TYPE) );
	}

	template< typename TYPE >
	FORCEINLINE void Update( const TYPE& srcData )
	{
		void* pData = this->Map( sizeof(TYPE) );
		MemCopy( pData, &srcData, sizeof(TYPE) );
		this->Unmap();
	}

	// 'slot' must be in range [0..D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) !

	FORCEINLINE void BindTo_VS( ID3D11DeviceContext* d3dContext, UINT slot )
	{
		d3dContext->VSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_GS( ID3D11DeviceContext* d3dContext, UINT slot )
	{
		d3dContext->GSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}
	FORCEINLINE void BindTo_PS( ID3D11DeviceContext* d3dContext, UINT slot )
	{
		d3dContext->PSSetConstantBuffers( slot, 1, &mD3DBuffer.Ptr );
	}

	FORCEINLINE bool IsValid() const
	{
		return mD3DBuffer != nil;
	}
};




/*
-----------------------------------------------------------------------------
	TDynamicVertexBuffer< VERTEX >

	Templated single-stream vertex buffer
-----------------------------------------------------------------------------
*/
template< class VERTEX >
struct TDynamicVertexBuffer
{
	dxPtr< ID3D11Buffer >	mVB;

public:
	TDynamicVertexBuffer()
	{
		//StaticAssert(VERTEX::NumStreams == 1);
	}

	void Setup( UINT numElements, const VERTEX* initialData = nil )
	{
		graphics.resources->Create_VertexBuffer(
			mVB,
			numElements * sizeof(VERTEX),
			sizeof VERTEX,
			initialData,
			true
		);
	}

	void Close()
	{
		mVB = nil;
	}

	FORCEINLINE void Bind( ID3D11DeviceContext* context, UINT slot = 0 )
	{
		//@todo: sort meshes by input layout

		// Set the vertex layout.

		context->IASetInputLayout( VERTEX::layout );

		// Set the vertex buffers.

		const UINT	stride = sizeof VERTEX;
		const UINT	offset = 0;

		context->IASetVertexBuffers(
			slot,
			1,
			&mVB.Ptr,
			&stride,
			&offset
		);
	}

	FORCEINLINE void* Lock()
	{
		return D3D_LockBuffer(D3DContext,mVB);
	}
	FORCEINLINE void Unlock()
	{
		return D3D_UnlockBuffer(D3DContext,mVB);
	}
};

/*
-----------------------------------------------------------------------------
	TDynamicIndexBuffer< INDEX >
-----------------------------------------------------------------------------
*/
template< class INDEX >
struct TDynamicIndexBuffer
{
	GrIndexBuffer	mIB;

public:
	TDynamicIndexBuffer()
	{
	}

	void Setup( UINT maxIndices, const INDEX* initialData = nil )
	{
		graphics.resources->Create_IndexBuffer(
			mIB,
			maxIndices * sizeof(INDEX),
			sizeof INDEX,
			initialData,
			true
		);
	}

	void Close()
	{
		mIB.Clear();
	}

	FORCEINLINE void Bind( ID3D11DeviceContext* context )
	{
		mIB.Bind( context );
	}

	FORCEINLINE void* Lock()
	{
		return mIB.Lock();
	}
	FORCEINLINE void Unlock()
	{
		return mIB.Unlock();
	}
};




//--------------------------------------------------------------------------------------
// Profiling/instrumentation support
//--------------------------------------------------------------------------------------

//
// back-end renderer stats - used only in profiling mode.
//
struct rxBackendStats
{
	UINT	lastFrameRenderTime;	// total time spent on rendering the last frame, in microseconds

	// TODO: DIP and DP?
	UINT	numDrawCalls;	// approximate number of draw calls issued in the last frame
	UINT	numTriangles;	// approximate number of triangles rendered in the last frame
	UINT	numVertices;	// approximate number of vertices rendered in the last frame

	UINT	numInputLayoutChanges;	// number of vertex declarations switches in the last frame
	UINT	numPrimTopologyChanges;	// number of primitive topology changes in the last frame

	UINT	numVBChanges;	// number of vertex buffer switches in the last frame
	UINT	numIBChanges;	// number of index buffer switches in the last frame

	UINT	numMapCalls;	// number of Map() / Unmap() calls

	// render state changes

	UINT	numDSStateChanges;	// number of depth-stencil state changes in the last frame
	UINT	numBSStateChanges;	// number of blend state changes in the last frame
	UINT	numRSStateChanges;	// number of rasterizer state changes in the last frame

	UINT	numVSChanges;	// number of vertex shader switches in the last frame
	UINT	numGSChanges;	// number of geometry shader switches in the last frame
	UINT	numPSChanges;	// number of pixel shader switches in the last frame

	UINT	numVSCBChanges;	// number of vertex shader constant buffer switches in the last frame
	UINT	numGSCBChanges;	// number of geometry shader constant buffer switches in the last frame
	UINT	numPSCBChanges;	// number of pixel shader constant buffer switches in the last frame

	UINT	numVSCBUpdates;	// number of vertex shader constant buffer updates in the last frame
	UINT	numGSCBUpdates;	// number of geometry shader constant buffer updates in the last frame
	UINT	numPSCBUpdates;	// number of pixel shader constant buffer updates in the last frame

	// these variables are persistent across frames
	UINT	frameCount;		// total number of rendered frames; incremented every frame
	UINT	minFrameRenderTime, maxFrameRenderTime;

public:
	rxBackendStats()
	{
		// Initialize performance counters for the first time.
		ZERO_OUT(*this);

		minFrameRenderTime = MAX_UINT32;
		maxFrameRenderTime = 0;
	}
	void Reset()
	{
		numDrawCalls = 0;
		numTriangles = 0;
		numVertices = 0;

		numInputLayoutChanges = 0;
		numPrimTopologyChanges = 0;

		numVBChanges = 0;
		numIBChanges = 0;

		numMapCalls = 0;

		numDSStateChanges = 0;
		numBSStateChanges = 0;
		numRSStateChanges = 0;

		numVSChanges = 0;
		numGSChanges = 0;
		numPSChanges = 0;

		numVSCBChanges = 0;
		numGSCBChanges = 0;
		numPSCBChanges = 0;

		numVSCBUpdates = 0;
		numGSCBUpdates = 0;
		numPSCBUpdates = 0;
	}
};

// valid only if RX_PROFILE is enabled
//
extern rxBackendStats gfxBEStats;





// Use dxDbgSetName() to attach names to D3D objects for use by 
// SDKDebugLayer, PIX's object table, etc.

#if 0//RX_D3D_USE_PERF_HUD

	FORCEINLINE void dxDbgSetName( IDXGIObject* pObj, const char* pstrName )
	{
		if( pObj != nil ) {
			pObj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName );
		}
	}
	FORCEINLINE void dxDbgSetName( ID3D11Device* pObj, const char* pstrName )
	{
		if( pObj != nil ) {
			pObj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName );
		}
	}
	FORCEINLINE void dxDbgSetName( ID3D11DeviceChild* pObj, const char* pstrName )
	{
		if( pObj != nil ) {
			pObj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName );
		}
	}

#else

	inline void dxDbgSetName( void* p, const char* pstrName ) {}

#endif // RX_D3D_USE_PERF_HUD






//--------------------------------------------------------------------------------------
// PIXEvent is a helper class that makes it easy to attach begin and end
// events to a block of code.  Simply define a CDXUTPerfEventGenerator variable anywhere 
// in a block of code, and the class's constructor will call DXUT_BeginPerfEvent when 
// the block of code begins, and the class's destructor will call DXUT_EndPerfEvent when 
// the block ends.
//--------------------------------------------------------------------------------------

#if RX_D3D_USE_PERF_HUD

	struct PIXEvent
	{
		inline PIXEvent( const TCHAR* markerName, D3DCOLOR color = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF) )
		{
			D3DPERF_BeginEvent( color, markerName );
		}
		inline ~PIXEvent()
		{
			D3DPERF_EndEvent();
		}
	};

	#define rxGPU_MARKER( markerName )		PIXEvent	pixEvent##_markerName( L#markerName );

#else

	#define rxGPU_MARKER( markerName )

#endif // RX_D3D_USE_PERF_HUD




/*
=======================================================================

	Rendering

=======================================================================
*/

// low-level graphics viewport
struct GrViewport
{
	D3D11_VIEWPORT	d;	// dimensions

	RenderTarget	mainRT;
	DepthStencil	mainDS;

	dxPtr< IDXGISwapChain >	swapChain;	// DirectX-specific

public:
	GrViewport();
	~GrViewport();

	void Release();
	bool IsNull() const;
	bool IsValid() const;
	bool CheckIsOk() const;

	FORCEINLINE FLOAT GetWidth() const
	{
		return d.Width;
	}
	FORCEINLINE FLOAT GetHeight() const
	{
		return d.Height;
	}
	FORCEINLINE FLOAT GetAspectRatio() const
	{
		//TSetMax<FLOAT>( d.Height, 1 );	// prevent division by zero
		FLOAT aspectRatio = d.Width / d.Height;
		return aspectRatio;
	}
	FORCEINLINE void GetCenter( FLOAT &x, FLOAT &y ) const
	{
		x = d.Width * 0.5f;
		y = d.Height * 0.5f;
	}
	BOOL IsFullScreen() const;

	// Bind the render target and the depth-stencil surface.
	void Set( ID3D11DeviceContext* theContext ) const;
};

bool D3D_SupportsPrimitiveTopology( EPrimitiveType::Code topology );

D3D11_PRIMITIVE_TOPOLOGY D3D_GetPrimitiveTopology( EPrimitiveType::Code topology );
EPrimitiveType::Code D3D_GetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology );

FORCEINLINE void rxSetTopology( ID3D11DeviceContext* theContext, rxTopology topology )
{
	theContext->IASetPrimitiveTopology( D3D_GetPrimitiveTopology( topology ) );
}

/*
=======================================================================

	Initialization and destruction

=======================================================================
*/

class GraphicsDevice : SingleInstance<GraphicsDevice>
{
public:
	GraphicsDevice();
	~GraphicsDevice();

	bool Initialize();
	void Shutdown();

public_internal:
	// immediate context
	dxPtr< ID3D11DeviceContext >	m_pD3DDeviceContext;
	dxPtr< ID3D11Device >	m_pD3DDevice;
	dxPtr< IDXGIFactory >	m_pDXGIFactory;
	dxPtr< IDXGIOutput >	m_pDXGIOutput;
};

// Low-level graphics system interface.
// Global variables specific to DirectX 11 graphics system.
// Note:	most of these are read-only!

class GraphicsSystem
	: SingleInstance<GraphicsSystem>, Initializable, NonCopyable
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	bool Initialize();
	void Shutdown();

public:	// Immediate-mode rendering
	void SetState( const StateBlock& newState );

public_internal:
	// device state
	GraphicsDevice	device;

	// graphics resource management

	TPtr< GrResourceSystem >	resources;

	TPtr< GrShaderSystem >	shaders;
};

extern GraphicsSystem graphics;


//---------------------------------------------------------------
//	Nasty global functions.
//---------------------------------------------------------------

FORCEINLINE ID3D11Device* GetD3DDevice()
{
	return graphics.device.m_pD3DDevice;
}
FORCEINLINE ID3D11DeviceContext* GetD3DDeviceContext()
{
	return graphics.device.m_pD3DDeviceContext;
}
FORCEINLINE IDXGIFactory* GetDXGIFactory()
{
	return graphics.device.m_pDXGIFactory;
}
FORCEINLINE IDXGIOutput* GetDXGIOutput()
{
	return graphics.device.m_pDXGIOutput;
}

//---------------------------------------------------------------
//	Nasty macros.
//---------------------------------------------------------------

#define D3DDevice		GetD3DDevice()
#define D3DContext		GetD3DDeviceContext()

struct SetupGraphics
{
	SetupGraphics()
	{
		graphics.Initialize();
	}
	~SetupGraphics()
	{
		graphics.Shutdown();
	}
};


//---------------------------------------------------------------
//	Internal public.
//---------------------------------------------------------------

// used when initializing shader library
struct GrShaderLibraryContext
{
	// for creating render targets
	UINT	backBufferWidth;
	UINT	backBufferHeight;

public:
	GrShaderLibraryContext();
};


/*
=======================================================================

	Utilities

=======================================================================
*/

UINT DXGIFormat_GetElementSize( DXGI_FORMAT format );
UINT DXGIFormat_GetElementCount( DXGI_FORMAT format );
bool DXGIFormat_HasAlphaChannel( DXGI_FORMAT format );
const char* DXGIFormat_ToString( DXGI_FORMAT format );
DXGI_FORMAT String_ToDXGIFormat( const char* str );

// Helper functions to create SRGB formats from typeless formats and vice versa.
DXGI_FORMAT DXGIFormat_MAKE_SRGB( DXGI_FORMAT format );
DXGI_FORMAT DXGIFormat_MAKE_TYPELESS( DXGI_FORMAT format );

const char* DXGIFormat_BaseTypeString( DXGI_FORMAT format );
const char* DXGIFormat_TypeString( DXGI_FORMAT format );

// returns a string describing the engine's type corresponding to the given DXGI format
const char* DXGIFormat_ToEngineTypeString( DXGI_FORMAT format, EVertexElementUsage semantic );


const char* D3D_FeatureLevelToStr( D3D_FEATURE_LEVEL featureLevel );

const char* DXGI_ScanlineOrder_ToStr( DXGI_MODE_SCANLINE_ORDER scanlineOrder );
const char* DXGI_ScalingMode_ToStr( DXGI_MODE_SCALING scaling );

D3D11_INPUT_CLASSIFICATION D3D_GetInputSlotClass( const char* str );

D3D11_FILTER D3D_TextureFilteringModeFromString( const char* str );

D3D11_TEXTURE_ADDRESS_MODE D3D_TextureAddressingModeFromString( const char* str );

D3D11_COMPARISON_FUNC D3D_ComparisonFunctionFromString( const char* str );

D3D11_DEPTH_WRITE_MASK D3D_DepthWriteMaskFromString( const char* str );
D3D11_BLEND D3D_BlendOptionFromString( const char* str );
D3D11_BLEND_OP D3D_BlendOperationFromString( const char* str );
D3D11_FILL_MODE D3D_FillModeFromString( const char* str );
D3D11_CULL_MODE D3D_CullModeFromString( const char* str );

D3DX11_IMAGE_FILE_FORMAT D3D_GetImageFormat( EImageFileFormat format );

EDeviceVendor D3D_GetDeviceVendor();

const char* D3D11_USAGE_To_Chars( const D3D11_USAGE usage );
const char* D3D11_SRV_DIMENSION_To_Chars( const D3D11_SRV_DIMENSION e );

//------------------------------------------------------------------------
//	Shader profiles
//------------------------------------------------------------------------

D3D_FEATURE_LEVEL D3D_GetSelectedFeatureLevel();

const char* D3D_GetShaderModel();
const char* D3D_GetVertexShaderProfile();
const char* D3D_GetGeometryShaderProfile();
const char* D3D_GetPixelShaderProfile();
const char* D3D_GetComputeShaderProfile();
const char* D3D_GetDomainShaderProfile();
const char* D3D_GetHullShaderProfile();




inline
void D3D_CalcTexture2DSize( ID3D11Texture2D* pTexture, UINT &OutWidth, UINT &OutHeight )
{
	D3D11_TEXTURE2D_DESC	texDesc;
	pTexture->GetDesc( &texDesc );
	OutWidth = texDesc.Width;
	OutHeight = texDesc.Height;
}

inline
bool D3D_EqualDimensions( ID3D11Texture2D* pTextureA, ID3D11Texture2D* pTextureB )
{
	UINT	aW, aH;
	UINT	bW, bH;
	D3D_CalcTexture2DSize( pTextureA, aW, aH );
	D3D_CalcTexture2DSize( pTextureB, bW, bH );

	return ( aW == bW ) && ( aH == bH );
}

inline
void D3D_GetTexture2D_ShaderResourceView_Desc( const DXGI_FORMAT eTextureFormat,
											  D3D11_SHADER_RESOURCE_VIEW_DESC &OutDesc )
{
	//ZERO_OUT(OutDesc);
	OutDesc.Format = eTextureFormat;
	OutDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	OutDesc.Texture2D.MostDetailedMip = 0;
	OutDesc.Texture2D.MipLevels = 1;
}


typedef void F_EnumerateRenderTargets( RenderTarget* p, void* userData );
typedef void F_EnumerateSamplerStates( ID3D11SamplerState* p, void* userData );
typedef void F_EnumerateDepthStencilStates( ID3D11DepthStencilState* p, void* userData );
typedef void F_EnumerateRasterizerStates( ID3D11RasterizerState* p, void* userData );
typedef void F_EnumerateBlendStates( ID3D11BlendState* p, void* userData );


