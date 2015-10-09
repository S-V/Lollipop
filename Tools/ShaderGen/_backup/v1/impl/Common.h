
#pragma once

//---------------------------------------------------------------------------
//	Includes
//---------------------------------------------------------------------------

#include "ShaderGen.h"
using namespace ShaderGen;

#include <Base/Text/Parse_Util.h>
#include <Base/Text/TextBuffer.h>

#include <Graphics/Graphics_DX11.h>
#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>
//#pragma comment( lib, "Graphics.lib" )

//---------------------------------------------------------------------------
//	Forward declarations
//---------------------------------------------------------------------------

class Parser;

//---------------------------------------------------------------------------
//	Memory management
//---------------------------------------------------------------------------

void* Lex_Alloc( SizeT bytes );
void Lex_Free( void* ptr );

#define LEX_NEW( typeName )	new(Lex_Alloc(sizeof(typeName))) typeName

//---------------------------------------------------------------------------
//	Parsing
//---------------------------------------------------------------------------

enum
{
	MaxFileNameLength = 64,
	MaxIdentifierLength = 48,
	MaxInformationLength = 64,
	MaxTempStringLength = 128,
};

typedef TFixedString< MaxFileNameLength >		wrFile;
typedef TFixedString< MaxIdentifierLength >		wrName;
typedef TFixedString< MaxInformationLength >	wrInfoStr;
typedef TFixedString< MaxTempStringLength >		wrBuffer;

/*
-----------------------------------------------------------------------------
	wrLocation - represents the position of a token in a source file
-----------------------------------------------------------------------------
*/
struct wrLocation
{
	String	file;
	UINT	line, column;

public:
	wrLocation()
	{}
	wrLocation( const char* fileName, UINT lineNum, UINT colNum )
		: file(fileName), line(lineNum), column(colNum)
	{}
	wrBuffer ToStr() const
	{
		wrBuffer buf;
		buf.Format("%s(%u,%u)", file.ToChars(),line,column);
		return buf;
	}
};

// base class for parsed tokens
struct wrSourceElement
{
	wrName		name;
	wrLocation	pos;	// location in the source file
	wrInfoStr	info;	// short description
};


// don't change the order, it's important for comparisons
enum ESizeMode
{
	Size_Unknown = 0,
	Size_Absolute= 1,
	Size_Relative= 2,
};

// render target size
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
	wrName	Filter;
	wrName	AddressU, AddressV, AddressW;
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

// only render target blending is supported
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


struct wrVertexElement : wrSourceElement
{
	wrName	SemanticName;
	UINT	SemanticIndex;
	wrName	Format;
};
struct wrVertexDeclaration : wrSourceElement
{
	TList< wrVertexElement >	elements;
};

enum BindFlags : U32
{
	CB_Bind_VS = BIT(0),
	CB_Bind_GS = BIT(1),
	CB_Bind_PS = BIT(2),
};

struct wrRegisterBound
{
	// Defined if the user specifies a bind point using the register keyword,
	// otherwise, INDEX_NONE
	UINT			iRegister;

	// Tells to which programmable pipeline stage (shader stages)
	// this object should be bound.
	TBits< BindFlags >		bindFlags;

	//bool	bUsed;

public:
	wrRegisterBound()
	{
		iRegister = INDEX_NONE;
		bindFlags = (BindFlags)BITS_ALL;
		//bUsed = true;
	}
	RX_OPTIMIZE("strip off unreferenced vars");
	bool isUsed() const
	{
		//return bUsed;
		return bindFlags != 0;
	}

	RX_OPTIMIZE("don't bind to slots if it's not needed");
	bool usedByVS() const
	{
		return bindFlags & CB_Bind_VS;
	}
	bool usedByGS() const
	{
		return bindFlags & CB_Bind_GS;
	}
	bool usedByPS() const
	{
		return bindFlags & CB_Bind_PS;
	}
};

// variable placed in a constant buffer
//
struct wrCBVar : wrSourceElement, wrRegisterBound
{
	wrName	typeName;

public:
	wrCBVar()
	{
	}
};

struct wrShaderConstantBuffer : wrSourceElement, wrRegisterBound
{
	TList< wrCBVar >	elements;

public:
	wrShaderConstantBuffer()
	{
	}
};

struct wrShaderSamplerState : wrSourceElement, wrRegisterBound
{
	wrName	initializer;
};

struct wrShaderResource : wrSourceElement, wrRegisterBound
{
	wrName	initializer;
};

struct wrShaderVariables
{
	TList< wrShaderConstantBuffer >	constantBuffers;
	TList< wrShaderSamplerState >	samplers;
	TList< wrShaderResource >		resources;
};

struct wrShaderInputs : wrShaderVariables
{
};

struct wrShaderCode : TextBuffer
{
	wrLocation	start;
};

struct wrShaderProgram : wrSourceElement
{
	// parsed source data

	wrLocation		startOfInputs;
	wrShaderInputs	inputs;
	wrShaderCode	code;


	wrName	vertexShader;
	wrName	pixelShader;


	// generated data

	UINT	uniqueIndex;

	// copy of original src code but it's modified for different purposes
	//TextBuffer		codeBlob;

	char* vsEntryOffset;
	UINT vsLength;

	char* psEntryOffset;
	UINT psLength;

public:
	wrShaderProgram()
	{
		uniqueIndex = -1;

		vsEntryOffset = nil;
		vsLength = 0;

		psEntryOffset = nil;
		psLength = 0;
	}

	UINT NumInstances() const
	{
		return 1;
	}

	bool isOk() const
	{
		return 1
			&& !pixelShader.IsEmpty()
			;
	}
};

void ParseCode( wrShaderCode & code, Parser * parser );

struct wrSharedSection : wrSourceElement
{
	wrShaderVariables	vars;
	wrShaderCode		code;
};

struct wrSourceFile
{
	OSPathName	name;
	OSFileName	pureFileName;	// extracted file name without extension

	// render targets
	TList< wrRenderTarget >		renderTargets;

	// render states
	TList< wrSamplerState >			samplerStates;
	TList< wrDepthStencilState >	depthStencilStates;
	TList< wrRasterizerState >		rasterizerStates;
	TList< wrBlendState >			blendStates;
	TList< wrStateBlock >			stateBlocks;

	// shared constants
	TList< wrSharedSection >		sharedSections;

	// shaders
	TList< wrShaderProgram >		shaders;

	// input layouts
	TList< wrVertexDeclaration >	vertexDeclarations;

public:

	wrShaderConstantBuffer & NewConstantBuffer( Parser* parser );
	wrShaderConstantBuffer & LastConstantBuffer();

	wrShaderSamplerState & NewSamplerState( Parser* parser );

	wrShaderResource& NewTexture( Parser* parser );

	wrVertexDeclaration& NewVertex( Parser* parser );

	template< class ELEMENT >
	ELEMENT & NewElement( Parser* parser, TList<ELEMENT> & elements )
	{
		ELEMENT& newOne = elements.Add();
		newOne.pos = parser->Location();
		return newOne;
	}
};

struct wrShaderLibrary
{
	// render targets
	TList< wrRenderTarget >		renderTargets;

	// render states
	TList< wrSamplerState >			samplerStates;
	TList< wrDepthStencilState >	depthStencilStates;
	TList< wrRasterizerState >		rasterizerStates;
	TList< wrBlendState >			blendStates;
	TList< wrStateBlock >			stateBlocks;

	// shared constants
	TList< wrSharedSection >		sharedSections;

	// shaders
	TList< wrShaderProgram >		shaders;

	// input layouts
	TList< wrVertexDeclaration >	vertexDeclarations;
};

struct ParseFileInput
{
	OSPathName	pathToSrcFile;

public:
	ParseFileInput()
	{

	}
};

struct ParseFileOutput
{
	wrSourceFile	fileData;

	UINT	numErrors;
	UINT	numWarnings;

public:
	ParseFileOutput()
	{
		numErrors = 0;
		numWarnings = 0;
	}
};

struct ParseResults
{
	TList< ParseFileOutput >	parsedFiles;
};


void Translate( const Options& config, const ParseResults& input );

struct wrShaderLibrary
{
	TList< wrSourceFile >	sourceFiles;
};


int ParseInteger(const char *p);
float ParseFloat(const char* p);
double ParseDouble(const char* p);

// finds the first occurrence of string2 in string1
char * __cdecl MyStrStr(
						const char * str1,	// string to search in
						const char * str2,	// substring to search for
						SizeT maxChar	// don't search past this number of chars
						);

//---------------------------------------------------------------------------
//	Includes
//---------------------------------------------------------------------------

#include "Parser.h"

