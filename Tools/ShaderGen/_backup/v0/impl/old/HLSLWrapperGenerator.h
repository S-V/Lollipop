/*
=============================================================================
	File:	HLSLWrapperGenerator.h
	Desc:
=============================================================================
*/

#pragma once



//---------------------------------------------------------------------------
//	Shader script parser
//---------------------------------------------------------------------------

class Parser;

/*================================
		ESizeMode
================================*/

// don't change the order, it's important for comparisons
enum ESizeMode
{
	Size_Unknown = 0,
	Size_Absolute= 1,
	Size_Relative= 2,
};

/*================================
		wrRTSize
================================*/

struct wrRTSize
{
	ESizeMode	sizeMode;
	union
	{
		F32	relativeSize;
		U32	absoluteSize;
		U32	sizeValue;
	};

public:
	wrRTSize()
	{
		this->sizeMode = Size_Unknown;
		this->sizeValue = 0;
	}
	bool Check() const
	{
		Unimplemented;
		if( this->sizeMode == Size_Unknown ) {
			//
		}
		return true;
	}
	void SetAbsoluteSize( UINT absoluteSize )
	{
		Assert( this->sizeMode == Size_Unknown );
		this->sizeMode = Size_Absolute;
		this->absoluteSize = absoluteSize;
	}
	void SetRelativeSize( UINT relativeSize )
	{
		Assert( this->sizeMode == Size_Unknown );
		this->sizeMode = Size_Relative;
		this->relativeSize = relativeSize;
	}

	// for sorting render targets
	friend bool operator < ( const wrRTSize& a, const wrRTSize& b )
	{
		return ( a.sizeMode < b.sizeMode );
	}
};

struct wrSourceElement
{
	wrName		name;
	wrLocation	pos;	// location in the source file
	wrInfoStr	info;	// short description
};

struct wrRenderTarget : wrSourceElement
{
	wrName		format;
	wrRTSize	sizeX, sizeY;

	static bool Compare( const wrRenderTarget& a, const wrRenderTarget& b )
	{
		return a.sizeX < b.sizeX
			&& a.sizeY < b.sizeY;
	}
};

struct wrSamplerState : wrSourceElement
{
	wrName	filter;
	wrName	addressU, addressV, addressW;
};
struct wrDepthStencilState : wrSourceElement
{
	wrName DepthEnable;
	wrName DepthWriteMask;
	wrName DepthFunc;
	wrName StencilEnable;
};
struct wrRasterizerState : wrSourceElement
{
	wrName FillMode;
	wrName CullMode;
	wrName FrontCounterClockwise;
	wrName DepthBias;
	wrName DepthBiasClamp;
	wrName SlopeScaledDepthBias;
	wrName DepthClipEnable;
	wrName ScissorEnable;
	wrName MultisampleEnable;
	wrName AntialiasedLineEnable;
};
/*
	typedef struct D3D11_RENDER_TARGET_BLEND_DESC
	{
		BOOL BlendEnable;
		D3D11_BLEND SrcBlend;
		D3D11_BLEND DestBlend;
		D3D11_BLEND_OP BlendOp;
		D3D11_BLEND SrcBlendAlpha;
		D3D11_BLEND DestBlendAlpha;
		D3D11_BLEND_OP BlendOpAlpha;
		UINT8 RenderTargetWriteMask;
	} 	D3D11_RENDER_TARGET_BLEND_DESC;

	typedef struct D3D11_BLEND_DESC
	{
		BOOL AlphaToCoverageEnable;
		BOOL IndependentBlendEnable;
		D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[ 8 ];
	} 	D3D11_BLEND_DESC;
*/
struct wrBlendState : wrSourceElement
{
	wrName AlphaToCoverageEnable;
	wrName BlendEnable;
	wrName SrcBlend;
	wrName DestBlend;
	wrName BlendOp;
	wrName SrcBlendAlpha;
	wrName DestBlendAlpha;
	wrName BlendOpAlpha;
	wrName RenderTargetWriteMask;
};
struct wrStateBlock : wrSourceElement
{
	// Rasterizer Stage

	wrName	rasterizerState;

	// Output-Merger Stage

	wrName	depthStencilState;
	wrName	stencilRef;

	wrName	blendState;
	FColor	blendFactorRGBA;
	wrName	sampleMask;
};

struct wrSourceFile
{
	OSFileName	name;
	TList< wrRenderTarget >	renderTargets;
	TList< wrSamplerState >	samplerStates;
	TList< wrDepthStencilState >	depthStencilStates;
	TList< wrRasterizerState >	rasterizerStates;
	TList< wrBlendState >	blendStates;
	TList< wrStateBlock >	stateBlocks;
};

struct wrShaderLibrary
{
	TList< wrSourceFile >	sourceFiles;
};

struct wrParseFileInput
{
	OSPathName	pathToSrcFile;

public:
	wrParseFileInput()
	{

	}
};

struct wrParseFileOutput
{
	UINT	numErrors;

public:
	wrParseFileOutput()
	{
		numErrors = 0;
	}
};

/*================================
	HLSLWrapperGenerator
================================*/

class HLSLWrapperGenerator
{
public:
	struct Options
	{
		OSPathName		inputFolder;
		OSPathName		outputFolder;

		TList< OSFileName >	srcFiles;

		bool	bDebugMode;

		Options()
		{
			bDebugMode = false;
		}
		bool isOk() const
		{
			return !inputFolder.IsEmpty()
				&& !outputFolder.IsEmpty()
				&& !srcFiles.IsEmpty()
				;
		}
	};

	void Setup( const Options& options );
	void Clear();

	void GenerateShaderLib();

	HLSLWrapperGenerator();
	~HLSLWrapperGenerator();

private:

	void Parse( const wrParseFileInput& input, wrParseFileOutput &output );

public_internal:
	Options			config;
	wrShaderLibrary	root;
	TPtr< Parser >	parser;
};

extern HLSLWrapperGenerator	compiler;

//=========================================================

inline
wrSourceFile& CurrentSourceFile() {
	return compiler.root.sourceFiles.GetLast();
}

/*================================
		HLSLTranslator
================================*/

class HLSLTranslator
{
public:
	HLSLTranslator();

	void Translate( const wrShaderLibrary& input );

private:
	void Translate( const wrSourceFile& srcFile );
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
