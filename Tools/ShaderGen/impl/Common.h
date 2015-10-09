
#pragma once

//---------------------------------------------------------------------------
//	Includes
//---------------------------------------------------------------------------

#include "ShaderGen.h"
using namespace ShaderGen;

#include <Base/Templates/Containers/BitSet/BitSet32.h>

#include <Base/Text/TextUtils.h>
#include "Base/Text/Parse_Util.h"
#include <Base/Text/TextBuffer.h>
#include <Base/Util/Sorting.h>

#define MAX_SAMPLER_STATE_SLOTS 16
#define MAX_SHADER_RESOURCE_SLOTS 16
#define MAX_CONSTANT_BUFFER_SLOTS 14

#include <Graphics/Graphics_DX11.h>
#include <Graphics/DX11/DX11Private.h>
//#include <Graphics/DX11/DX11Helpers.h>
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
	MaxIdentifierLength = 64,
	MaxInformationLength = 64,
	MaxTempStringLength = 128,
};

typedef TStackString< MaxFileNameLength >		wrFile;
typedef TStackString< MaxIdentifierLength >		wrName;
typedef TStackString< MaxInformationLength >	wrInfoStr;
typedef TStackString< MaxTempStringLength >		wrBuffer;


//typedef String		wrFile;
//typedef String		wrName;
//typedef String		wrInfoStr;
//typedef String		wrBuffer;

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

	UINT uniqueId;	// read-only

public:
	wrSourceElement()
		: uniqueId(ms_counter++)
	{
	}

	wrName GetUniqueName() const;

private:
	static UINT ms_counter;
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
		F4	relativeSize;
		U4	absoluteSize;
		U4	sizeValue;
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
	void SetRelativeSize( FLOAT relativeSize )
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
	FColor		clearColor;

	wrRenderTarget()
	{
		clearColor = FColor::BLACK;
	}
	bool DependsOnBackbufferSize() const
	{
		return sizeX.sizeMode == Size_Relative
			|| sizeY.sizeMode == Size_Relative
			;
	}
	static bool Compare( const wrRenderTarget& a, const wrRenderTarget& b )
	{
		return a.sizeX < b.sizeX
			&& a.sizeY < b.sizeY;
	}
};


struct wrMultiRenderTarget : wrSourceElement
{
	TList<wrRenderTarget>	renderTargets;
};

struct wrSamplerState : wrSourceElement
{
	wrName	Filter;

	wrName	AddressU, AddressV, AddressW;

	FLOAT	MipLODBias;
	UINT	MaxAnisotropy;
	wrName	ComparisonFunc;

	FColor	BorderColor;

	FLOAT	MinLOD;
	FLOAT	MaxLOD;

	// should we emit 'SamplerComparisonState' instead of 'SamplerState' in shader code?
	bool	bIsSamplerCmpState;

public:
	wrSamplerState()
	{
		Filter = MXC("MIN_MAG_MIP_LINEAR");

		AddressU = MXC("Clamp");
		AddressV = MXC("Clamp");
		AddressW = MXC("Clamp");

		MipLODBias = 0.0f;
		MaxAnisotropy = 1;
		ComparisonFunc = MXC("NEVER");

		BorderColor.SetAll(1.0f);

		MinLOD = -3.402823466e+38F; // -FLT_MAX
		MaxLOD = 3.402823466e+38F; // FLT_MAX

		bIsSamplerCmpState = false;
	}
};

struct wrDepthStencilState : wrSourceElement
{
	wrName DepthEnable;
	wrName DepthWriteMask;
	wrName DepthFunc;
	wrName StencilEnable;

	wrName FrontFaceStencilFunc;
	wrName FrontFaceStencilPass;

public:
	wrDepthStencilState()
	{
		DepthEnable = "True";
		DepthWriteMask = "All";
		DepthFunc = "Less";
		StencilEnable = "False";

		FrontFaceStencilFunc = "Always";
		FrontFaceStencilPass = "Keep";
	}
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
	UINT	InputSlot;

	UINT	sizeInBytes;

	wrVertexElement()
	{
		SemanticIndex = -1;
		InputSlot = -1;
		sizeInBytes = 7777;
	}
};

struct wrStream
{
	TList< wrVertexElement >	elems;
};

struct wrVertexDeclaration : wrSourceElement
{
	TList< wrVertexElement >	elements;

	// names in C++ code
	TList< wrName >	elementNames;	// size == num elements
	TList< wrName >	streamNames;	// size == num streams
	TList< UINT >	streamSizes;	// size == num streams

	TList< wrStream >	streams;

	UINT	uniqueIndex;

public:
	wrVertexDeclaration()
	{
		uniqueIndex = INDEX_NONE;
	}

	UINT CalcNumStreams() const
	{
		UINT numStreams = 0;

		//BitSet32	streams(0);

		for( UINT iVertexElement = 0;
			iVertexElement < elements.Num();
			iVertexElement++ )
		{
			const wrVertexElement& elem = elements[ iVertexElement ];

			const UINT streamIndex = elem.InputSlot;

			//Assert(streams.get(streamIndex) == 0);

			//streams.set(streamIndex);

			numStreams = Max(numStreams,streamIndex+1);
		}
		return numStreams;
	}
};

enum BindFlags : U4
{
	CB_Bind_HS = BIT(0),
	CB_Bind_DS = BIT(1),
	CB_Bind_TS = BIT(2),
	CB_Bind_VS = BIT(3),
	CB_Bind_GS = BIT(4),
	CB_Bind_PS = BIT(5),
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

template< class TYPE >	// where TYPE : wrRegisterBound
bool Check_If_Vars_Are_Used_By_VS( const TList<TYPE>& elems )
{
	for(UINT i=0; i<elems.Num(); i++)
	{
		if(elems[i].usedByVS()) {
			return true;
		}
	}
	return false;
}
template< class TYPE >	// where TYPE : wrRegisterBound
bool Check_If_Vars_Are_Used_By_PS( const TList<TYPE>& elems )
{
	for(UINT i=0; i<elems.Num(); i++)
	{
		if(elems[i].usedByPS()) {
			return true;
		}
	}
	return false;
}
template< class TYPE >	// where TYPE : wrRegisterBound
bool Check_If_Vars_Are_Used_By_GS( const TList<TYPE>& elems )
{
	for(UINT i=0; i<elems.Num(); i++)
	{
		if(elems[i].usedByGS()) {
			return true;
		}
	}
	return false;
}

template< class TYPE >	// where TYPE : wrRegisterBound
void Calc_Min_Max_Registers_Used( const TList<TYPE>& elems, UINT &iMinRegister, UINT &iMaxRegister )
{
	iMinRegister = (UINT)-1;
	iMaxRegister = 0;

	for(UINT i=0; i<elems.Num(); i++)
	{
		iMinRegister = Min( iMinRegister, elems[i].iRegister );
		iMaxRegister = Max( iMaxRegister, elems[i].iRegister );
	}
}


// variable placed in a constant buffer
//
struct wrCBVar : wrSourceElement, wrRegisterBound
{
	wrName	typeName;

	wrName	arrayDim;	// e.g.: typeName varName[ arrayDim ];

public:
	wrCBVar()
	{
	}

	// e.g. "float4	lightDiffuseColor;"
	StackString	GenDeclCodeString() const;
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

	bool bIsSamplerCmpState;

	wrShaderSamplerState()
	{
		bIsSamplerCmpState = false;
	}
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

	BitSet32	constantBuffersSlots;
	BitSet32	samplerStatesSlots;
	BitSet32	shaderResourcesSlots;

public:
	wrShaderVariables()
		: constantBuffersSlots(0)
		, samplerStatesSlots(0)
		, shaderResourcesSlots(0)
	{
	}

	void generate_hlsl_code_for_binding( TextBuffer& code, const Options& config ) const;

	bool are_empty() const
	{
		return constantBuffers.IsEmpty()
			&& samplers.IsEmpty()
			&& resources.IsEmpty()
			;
	}
};

typedef TList< wrBuffer >	SharedSectionsList;

struct wrShaderInputs : wrSourceElement, wrShaderVariables
{
	SharedSectionsList	sharedSections;
};

struct wrShaderDefine : wrSourceElement
{
	UINT	defaultValue;
};

struct wrShaderCode : TextBuffer
{
	wrLocation	start;
};

struct wrShaderProgram : wrSourceElement
{
	// parsed source data

	TList<wrShaderDefine>	defines;
	wrShaderInputs	inputs;
	wrShaderCode	code;


	wrName	vertexShader;
	wrName	pixelShader;


	// generated data

	UINT	uniqueIndex;

	TextBuffer	generatedCode;

	char* vsEntryOffset;
	UINT vsLength;

	char* psEntryOffset;
	UINT psLength;

public:
	wrShaderProgram()
	{
		uniqueIndex = INDEX_NONE;

		vsEntryOffset = nil;
		vsLength = 0;

		psEntryOffset = nil;
		psLength = 0;
	}

	UINT CalcNumInstances() const
	{
		return (1<<defines.Num());
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
	// parsed data

	wrShaderVariables	vars;
	wrShaderCode		code;	// appended to shader source code

	// generated data

	TextBuffer	bindCode;	// appended to shader source code
};

struct wrSourceFile
{
	OSPathName	name;
	OSFileName	pureFileName;	// extracted file name without extension

	// render targets
	TList< wrRenderTarget >		renderTargets;

	TList< wrMultiRenderTarget >	multiRenderTargets;

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
	// render targets
	TList< wrRenderTarget >			renderTargets;	// sorted by size in descending order
	TList< wrMultiRenderTarget >	multiRenderTargets;

	// render states
	TList< wrSamplerState >			samplerStates;
	TList< wrDepthStencilState >	depthStencilStates;
	TList< wrRasterizerState >		rasterizerStates;
	TList< wrBlendState >			blendStates;
	TList< wrStateBlock >			stateBlocks;

	// shared constants
	TList< wrSharedSection >		sharedSections;

	// shaders
	TList< wrShaderProgram >		shaders;	// sorted by name in ascending order

	// input layouts
	TList< wrVertexDeclaration >	vertexDeclarations;

public:
	wrShaderLibrary(const ParseResults& results);

	bool Resolve(const Options& config);

	wrSharedSection* FindSharedSectionByName( const char* sharedSectionName );

	UINT CalcNumUniqueShaderProgramCombinations() const;
};







void Warning( const wrSourceElement& element, const char* fmt, ... );

//---------------------------------------------------------------------------
//	Includes
//---------------------------------------------------------------------------

#include "Parser.h"

