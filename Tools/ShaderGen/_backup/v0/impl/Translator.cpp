#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

#include "DX11_Util.h"

#include <Base/Templates/Containers/BitSet/BitSet32.h>

//------------------------------------------------------------------------
//	Configuration
//------------------------------------------------------------------------

#define WR_VERSION_STRING	MXC("0.0")
#define WR_NAMESPACE		MXC("DX11")
#define WR_HEADER_FILE_EXT	MXC(".hxx")
#define WR_SOURCE_FILE_EXT	MXC(".cxx")

// for development only
#define WR_EMIT_COMMENTS	(0)

#define WR_EMIT_PREAMBLE	(0)

#define WR_RenderTarget_PREFIX			"renderTarget_"
#define WR_SamplerState_PREFIX			"samplerState_"
#define WR_RasterizerState_PREFIX		"rasterizerState_"
#define WR_BlendState_PREFIX			"blendState_"
#define WR_DepthStencilState_PREFIX		"depthStencilState_"
#define WR_StateBlock_PREFIX			"renderState_"

#define WR_Shader_PREFIX	""
#define WR_Shader_ConstantBuffer_PREFIX	"cb_"
#define WR_Shader_Sampler_PREFIX	""
#define WR_Shader_Texture_PREFIX	""

#define WR_Shader_ConstantBuffer_Alignment	UINT(16)

static
OSFileName GetDestFileName( const OSFileName& srcFileName, ConstCharPtr& extension )
{
	OSFileName	destFileName( srcFileName );
	destFileName.StripFileExtension();
	destFileName += extension;
	return destFileName;
}
static
wrBuffer GetCurrentDateTimeString()
{
	wrBuffer	currentDateTime;

	CalendarTime	localTime( CalendarTime::GetCurrentLocalTime() );

	String	timeOfDay;
	GetTimeOfDayString( localTime.hour, localTime.minute, localTime.second,
		timeOfDay );

	currentDateTime.Format(
		"on %s, %s %u%s, at %s",
		CalendarTime::WeekdayToStr( localTime.weekday ),
		CalendarTime::MonthToStr( localTime.month ),
		localTime.day, GetNumberExtension( localTime.day ),
		timeOfDay.ToChars()
		);

	return currentDateTime;
}


enum ELeaveScopeAction
{
	Scope_NoAction = 0,
	Scope_Semicolon
};

/*
=============================================================================
	MyTextWriter
=============================================================================
*/
class MyTextWriter : public mxTextWriter
{
	INT			scopeDepth;
	
	enum { MAX_SCOPE_DEPTH = 32 };
	static const char tabs[ MAX_SCOPE_DEPTH ];

public:
	explicit MyTextWriter( mxStreamWriter* stream )
		: mxTextWriter( stream ), scopeDepth( 0 )
	{
		AssertPtr(stream);
	}
	~MyTextWriter()
	{
		while( scopeDepth )
		{
			LeaveScope();
		}
	}
	void EnterScope()
	{
		Assert( scopeDepth < MAX_SCOPE_DEPTH );
		(*this) << MXC("{\n");
		++scopeDepth;
	}
	void LeaveScope(ELeaveScopeAction action = ELeaveScopeAction::Scope_NoAction)
	{
		Assert( scopeDepth > 0 );
		--scopeDepth;
		switch(action)
		{
		case ELeaveScopeAction::Scope_NoAction :
			(*this) << MXC("}\n");
			break;

		case ELeaveScopeAction::Scope_Semicolon :
			(*this) << MXC("};\n");
			break;

		default:
			Unreachable;
		}
	}
	// writes string with indentation
	OVERRIDEN void Put( const char* s, UINT length )
	{
		if( scopeDepth > 0 ) {
			mStream->Write( tabs, scopeDepth );
		}
		mStream->Write( s, length );
	}
	void NewLine()
	{
		Put("\n",1);
	}
	// writes string only, without indentation, doesn't write tabs
	MyTextWriter& Emit( const char* s, UINT length )
	{
		mStream->Write( s, length );
		return *this;
	}
	MyTextWriter& Emit( const char* s )
	{
		return Emit( s, mxStrLenAnsi(s) );
	}
	MyTextWriter& Emitf( const char* fmt, ... )
	{
		char	buffer[ MAX_STRING_CHARS ];
		UINT	length;
		MX_GET_VARARGS_ANSI_X( buffer, fmt, length );
		return Emit( buffer, length );
	}
	
	MyTextWriter& EmitNewLine()
	{
		mStream->Write( "\n", 1 );
		return *this;
	}

public_internal:
	INT GetDepth() const { return scopeDepth; }
	void SetDepth( INT nestingLevel )
	{
		Assert(nestingLevel >= 0);
		scopeDepth = nestingLevel;
	}
};

#define VAL_1X     '\t'

const char MyTextWriter::tabs[ MAX_SCOPE_DEPTH ] = { VAL_32X };
//StaticAssert( MyTextWriter::MAX_SCOPE_DEPTH == 32 );

#undef VAL_1X




// scope helper
class wrScope
{
	MyTextWriter& writer;
	const ELeaveScopeAction onLeaveScopeAction;
public:
	wrScope( MyTextWriter& tw, ELeaveScopeAction action = ELeaveScopeAction::Scope_NoAction )
		: writer( tw ), onLeaveScopeAction(action)
	{
		writer.EnterScope();
	}
	~wrScope()
	{
		writer.LeaveScope(onLeaveScopeAction);
	}
};

class wrSetScopeLevel
{
	MyTextWriter& writer;
	const UINT newScopeLevel;
	const INT savedScopeLevel;
public:
	wrSetScopeLevel( MyTextWriter& tw, UINT newScopeLevel = 1 )
		: writer( tw ), newScopeLevel( newScopeLevel ), savedScopeLevel( tw.GetDepth() )
	{
		writer.SetDepth(newScopeLevel);
	}
	~wrSetScopeLevel()
	{
		writer.SetDepth(savedScopeLevel);
	}
};
class wrDecreaseScopeLevel
{
	MyTextWriter& writer;
	const UINT numTabs;
	const INT savedScopeLevel;
public:
	wrDecreaseScopeLevel( MyTextWriter& tw, UINT numTabs = 1 )
		: writer( tw ), numTabs( numTabs ), savedScopeLevel( tw.GetDepth() )
	{
		UINT newScopeLevel = 
			(savedScopeLevel >= numTabs)
			?
			(savedScopeLevel - numTabs)
			:
			0
			;

		writer.SetDepth(newScopeLevel);
	}
	~wrDecreaseScopeLevel()
	{
		writer.SetDepth(savedScopeLevel);
	}
};

void Emit_Comment_Source_Location( const wrLocation& pos, MyTextWriter & tw )
{
#if WR_EMIT_COMMENTS
	tw.Emitf( "// %s\n", pos.ToStr().ToChars() );
#endif
}
void Emit_Comment_Source_Element_Info( const wrSourceElement& elem, MyTextWriter & tw )
{
#if WR_EMIT_COMMENTS
	tw.Emitf( "// %s\n", elem.name.ToChars() );
	if( !elem.info.IsEmpty() ) {
		tw.Emitf( "// %s\n", elem.info.ToChars() );
	}
	Emit_Comment_Source_Location( elem.pos, tw );
#endif
}

/*
=============================================================================
	HLSLTranslator
=============================================================================
*/
class HLSLTranslator
{
public:
	HLSLTranslator()
	{

	}

	void Translate( const Options& config, ParseResults& input )
	{
		for( UINT iSrcFile = 0;
			iSrcFile < input.parsedFiles.Num();
			iSrcFile++ )
		{
			ParseFileOutput& parsedData = input.parsedFiles[ iSrcFile ];
			wrSourceFile& srcFile = parsedData.fileData;

			Translate( config, srcFile );
		}
	}

	void Translate( const Options& config, wrSourceFile& srcFile )
	{
		Resolve( config, srcFile );

		Emit_Declarations( srcFile );
		Emit_Definitions( config, srcFile );
	}

	//===========================================================================

	OSFileName GetHeaderFileName( const wrSourceFile& srcFile )
	{
		return GetDestFileName( srcFile.name, WR_HEADER_FILE_EXT );
	}
	//===========================================================================
	OSFileName GetSourceFileName( const wrSourceFile& srcFile )
	{
		return GetDestFileName( srcFile.name, WR_SOURCE_FILE_EXT );
	}
	/*
	=============================================================================
	*/
	void Emit_Declarations( wrSourceFile& srcFile )
	{
		OSFileName		fileName = GetHeaderFileName( srcFile );
		FileWriter		outputStream( fileName.ToChars() );
		MyTextWriter	tw( &outputStream );

		if( WR_EMIT_PREAMBLE )
		{
		tw	<< MXC("/* This ALWAYS GENERATED file contains the definitions for the interfaces */\n")
			<< MXC("/* File created by HLSL wrapper generator version ") << WR_VERSION_STRING << MXC(" ") << GetCurrentDateTimeString() << MXC(" */\n")
			<< MXC("\n")
			;
		}
		tw	<< MXC("namespace ") << WR_NAMESPACE << MXC("\n");
		wrScope	sc(tw);

		Emit_RenderTarget_Declarations( srcFile.renderTargets, tw );

		Emit_SamplerState_Declarations( srcFile.samplerStates, tw );

		Emit_DepthStencilStates_Declarations( srcFile.depthStencilStates, tw );

		Emit_RasterizerStates_Declarations( srcFile.rasterizerStates, tw );

		Emit_BlendStates_Declarations( srcFile.blendStates, tw );

		Emit_StateBlocks_Declarations( srcFile.stateBlocks, tw );

		Emit_Shader_Declarations( srcFile.shaders, tw );

		tw	<< MXC("// Function declarations\n");
		tw	<< MXC("void AllocateGPUResources(void);\n");
		tw	<< MXC("void ReleaseGPUResources(void);\n");
	}
	/*
	=============================================================================
	*/
	void Emit_Definitions( const Options& config, wrSourceFile& srcFile )
	{
		OSFileName		fileName = GetSourceFileName( srcFile );
		FileWriter		outputStream( fileName.ToChars() );
		MyTextWriter	tw( &outputStream );

		if( WR_EMIT_PREAMBLE )
		{
		tw	<< MXC("/* This ALWAYS GENERATED file contains the implementation for the interfaces */\n")
			<< MXC("/* File created by HLSL wrapper generator version ") << WR_VERSION_STRING << MXC(" ") << GetCurrentDateTimeString() << MXC(" */\n")
			<< MXC("\n")
			;
		}

		Emit_Include_Header_Files( config, srcFile, tw );

		tw	<< MXC("namespace ") << WR_NAMESPACE << MXC("\n");
		wrScope	sc(tw);

		// Initialization order:
		// 1) Render targets
		// 2) State objects
		// 3) Shaders
		// 4) Input layouts
		// 5) Everything else


		Emit_Initialize_RenderTargets( srcFile.renderTargets, tw );
		Emit_Initialize_SamplerStates( srcFile.samplerStates, tw );
		Emit_Initialize_DepthStencilStates( srcFile.depthStencilStates, tw );
		Emit_Initialize_RasterizerStates( srcFile.rasterizerStates, tw );
		Emit_Initialize_BlendStates( srcFile.blendStates, tw );
		Emit_Initialize_StateBlocks( srcFile.stateBlocks, tw );

		Emit_Shader_Definitions( srcFile.shaders, tw );

		tw.NewLine();
		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Creation / Destruction function\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		tw	<< MXC("void AllocateGPUResources()\n");
		{
			wrScope	sc(tw);

			tw	<< MXC("const DeviceState& creationParams = gfxState;\n\n");

			if( srcFile.renderTargets.Num() ) {
				tw	<< MXC("SetupRenderTargets( creationParams );\n");
			}
			if( srcFile.samplerStates.Num() ) {
				tw	<< MXC("SetupSamplerStates( creationParams );\n");
			}
			if( srcFile.depthStencilStates.Num() ) {
				tw	<< MXC("SetupDepthStencilStates( creationParams );\n");
			}
			if( srcFile.rasterizerStates.Num() ) {
				tw	<< MXC("SetupRasterizerStates( creationParams );\n");
			}
			if( srcFile.blendStates.Num() ) {
				tw	<< MXC("SetupBlendStates( creationParams );\n");
			}
			if( srcFile.stateBlocks.Num() ) {
				tw	<< MXC("SetupStateBlocks( creationParams );\n");
			}



			tw.NewLine();



			for( UINT iShader = 0;
				iShader < srcFile.shaders.Num();
				iShader++ )
			{
				wrShaderProgram& shader = srcFile.shaders[ iShader ];

				tw.Putf( "%s::Initialize();\n", Get_ShaderClass_Name(shader) );
			}
		}



		Emit_Release_Resources( srcFile, tw );
	}
	//===========================================================================
	void Emit_Include_Header_Files( const Options& config, const wrSourceFile& srcFile, MyTextWriter & tw )
	{
		if( !config.precompiledHeader.IsEmpty() )
		{
			tw.Putf( "#include %s\n", config.precompiledHeader.ToChars() );
			tw.Putf( "#pragma hdrstop\n" );
		}
		for( UINT i = 0; i < config.includedHeaders.Num(); i++ )
		{
			tw.Putf( "#include %s\n", config.includedHeaders[i].ToChars() );
		}

		tw.NewLine();

		OSFileName	headerFileName = GetHeaderFileName( srcFile );
		tw.Putf( "#include \"%s\"\n", headerFileName.ToChars() );

		tw.NewLine();
	}
	/*
	=============================================================================
	*/
	void Emit_RenderTarget_Declarations( TList< wrRenderTarget >& renderTargets, MyTextWriter & tw )
	{
		const UINT numRenderTargets = renderTargets.Num();

		if( !numRenderTargets ) {
			return;
		}

		tw	<< MXC("// Render targets\n");

		struct DeclareRenderTarget
		{
			MyTextWriter & tw;
			FixedString	buffer;

			DeclareRenderTarget(MyTextWriter & tw) : tw(tw) {}

			void operator() (wrRenderTarget& renderTarget)
			{
				Emit_Comment_Source_Location( renderTarget.pos, tw );
				buffer.Format( "extern RenderTarget %s%s;\n",
					WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
				tw	<< buffer;
			}

		} decl( tw );

		renderTargets.Do_ForAll( decl );

		tw.NewLine();
	}
	/*
	=============================================================================
	*/
	void Emit_SamplerState_Declarations( const TList< wrSamplerState >& samplerStates, MyTextWriter & tw )
	{
		const UINT numSamplerStates = samplerStates.Num();

		if( !numSamplerStates ) {
			return;
		}

		tw	<< MXC("// Sampler states\n");

		FixedString	buffer;

		for( UINT iSamplerState = 0;
			iSamplerState < numSamplerStates;
			iSamplerState++ )
		{
			const wrSamplerState& samplerState = samplerStates[ iSamplerState ];

			Emit_Comment_Source_Location( samplerState.pos, tw );
			buffer.Format( "extern SamplerState %s%s;\n",
				WR_SamplerState_PREFIX, samplerState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}
	/*
	=============================================================================
	*/
	void Emit_DepthStencilStates_Declarations( const TList< wrDepthStencilState >& depthStencilStates, MyTextWriter & tw )
	{
		const UINT numDepthStencilStates = depthStencilStates.Num();

		if( !numDepthStencilStates ) {
			return;
		}

		tw	<< MXC("// Depth-stencil states\n");

		FixedString	buffer;

		for( UINT iDepthStencilState = 0;
			iDepthStencilState < numDepthStencilStates;
			iDepthStencilState++ )
		{
			const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];
			Emit_Comment_Source_Location( depthStencilState.pos, tw );
			buffer.Format( "extern DepthStencilState %s%s;\n",
				WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}

	//===========================================================================
	void Emit_RasterizerStates_Declarations( const TList< wrRasterizerState >& rasterizerStates, MyTextWriter & tw )
	{
		const UINT numRasterizerStates = rasterizerStates.Num();

		if( !numRasterizerStates ) {
			return;
		}

		tw	<< MXC("// Rasterizer states\n");

		FixedString	buffer;

		for( UINT iRasterizerState = 0;
			iRasterizerState < numRasterizerStates;
			iRasterizerState++ )
		{
			const wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];
			Emit_Comment_Source_Location( rasterizerState.pos, tw );
			buffer.Format( "extern RasterizerState %s%s;\n",
				WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}

	//===========================================================================
	void Emit_BlendStates_Declarations( const TList< wrBlendState >& blendStates, MyTextWriter & tw )
	{
		const UINT numBlendStates = blendStates.Num();

		if( !numBlendStates ) {
			return;
		}

		tw	<< MXC("// Blend states\n");

		FixedString	buffer;

		for( UINT iBlendState = 0;
			iBlendState < numBlendStates;
			iBlendState++ )
		{
			const wrBlendState& blendState = blendStates[ iBlendState ];
			Emit_Comment_Source_Location( blendState.pos, tw );
			buffer.Format( "extern BlendState %s%s;\n",
				WR_BlendState_PREFIX, blendState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_StateBlocks_Declarations( TList< wrStateBlock >& stateBlocks, MyTextWriter & tw )
	{
		const UINT numStateBlocks = stateBlocks.Num();

		if( !numStateBlocks ) {
			return;
		}

		tw	<< MXC("// State blocks\n");

		FixedString	buffer;

		for( UINT iStateBlock = 0;
			iStateBlock < numStateBlocks;
			iStateBlock++ )
		{
			const wrStateBlock& stateBlock = stateBlocks[ iStateBlock ];
			Emit_Comment_Source_Location( stateBlock.pos, tw );
			buffer.Format( "extern StateBlock %s%s;\n",
				WR_StateBlock_PREFIX, stateBlock.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}
	/*
	=============================================================================
	*/
	void Emit_Initialize_RenderTargets( TList< wrRenderTarget >& renderTargets, MyTextWriter & tw )
	{
		const UINT numRenderTargets = renderTargets.Num();

		if( !numRenderTargets ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Render targets\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iRenderTarget = 0;
			iRenderTarget < numRenderTargets;
			iRenderTarget++ )
		{
			wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

			Emit_Comment_Source_Location( renderTarget.pos, tw );
			buffer.Format( "RenderTarget %s%s;\n",
				WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupRenderTargets( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		tw	<< MXC("D3D11_TEXTURE2D_DESC texDesc;\n")
			<< MXC("D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;\n")
			<< MXC("D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;\n")
			<< MXC("ZERO_OUT( texDesc );\n")
			<< MXC("ZERO_OUT( rtvDesc );\n")
			<< MXC("ZERO_OUT( srvDesc );\n")
			;

		for( UINT iRenderTarget = 0;
			iRenderTarget < numRenderTargets;
			iRenderTarget++ )
		{
			wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

			Emit_Initialize_RenderTarget( renderTarget, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_RenderTarget( const wrRenderTarget& renderTarget, MyTextWriter & tw )
	{
		tw << MXC("\n");
		Emit_Comment_Source_Element_Info( renderTarget, tw );
		wrScope	sc(tw);

		wrBuffer	renderTargetFormat = DXGI_Format_From_Script_Format( renderTarget.format );

		tw	.Putf( "texDesc.Format				= %s;\n", renderTargetFormat.ToChars() );
		tw	.Putf( "texDesc.Width				= %s;\n", Get_RT_Size( renderTarget.sizeX, MXC("creationParams.backbufferWidth") ).ToChars() );
		tw	.Putf( "texDesc.Height				= %s;\n", Get_RT_Size( renderTarget.sizeY, MXC("creationParams.backbufferHeight") ).ToChars() );
		tw	<< MXC("texDesc.MipLevels			= 1;\n")
			<< MXC("texDesc.ArraySize			= 1;\n")
			<< MXC("texDesc.SampleDesc.Count	= 1;\n")
			<< MXC("texDesc.SampleDesc.Quality	= 0;\n")
			<< MXC("texDesc.Usage				= D3D11_USAGE_DEFAULT;\n")
			<< MXC("texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;\n")
			<< MXC("texDesc.CPUAccessFlags		= 0;\n")
			<< MXC("texDesc.MiscFlags			= 0;\n")
			<< MXC("\n")
			;
		tw	.Putf("rtvDesc.Format				= %s;\n", renderTargetFormat.ToChars() );
		tw	<< MXC("rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;\n")
			<< MXC("rtvDesc.Texture2D.MipSlice	= 0;\n")
			<< MXC("\n")
			;
		tw	.Putf("srvDesc.Format						= %s;\n", renderTargetFormat.ToChars() );
		tw	<< MXC("srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;\n")
			<< MXC("srvDesc.Texture2D.MipLevels			= 1;\n")
			<< MXC("srvDesc.Texture2D.MostDetailedMip	= 0;\n")
			;
		tw.NewLine();

		tw	.Putf("Create_RenderTarget( texDesc, rtvDesc, srvDesc, %s%s );\n",
			WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
		
	}

	//===========================================================================
	void Emit_Initialize_SamplerStates( TList< wrSamplerState >& samplerStates, MyTextWriter & tw )
	{
		const UINT numSamplerStates = samplerStates.Num();

		if( !numSamplerStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Sampler states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iSamplerState = 0;
			iSamplerState < numSamplerStates;
			iSamplerState++ )
		{
			wrSamplerState& samplerState = samplerStates[ iSamplerState ];

			Emit_Comment_Source_Location( samplerState.pos, tw );
			buffer.Format( "SamplerState %s%s;\n",
				WR_SamplerState_PREFIX, samplerState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupSamplerStates( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		tw	<< MXC("D3D11_SAMPLER_DESC samplerDesc;\n")
			<< MXC("ZERO_OUT( samplerDesc );\n")
			;

		for( UINT iSamplerState = 0;
			iSamplerState < numSamplerStates;
			iSamplerState++ )
		{
			wrSamplerState& samplerState = samplerStates[ iSamplerState ];

			Emit_Initialize_SamplerState( samplerState, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_SamplerState( wrSamplerState& samplerState, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( samplerState, tw );
		wrScope	sc(tw);

		tw	.Putf("samplerDesc.Filter	= %s;\n", GetTextureFilter( samplerState.Filter ).ToChars() );
		tw	.Putf("samplerDesc.AddressU	= %s;\n", GetTextureAddressMode( samplerState.AddressU ).ToChars() );
		tw	.Putf("samplerDesc.AddressV	= %s;\n", GetTextureAddressMode( samplerState.AddressV ).ToChars() );
		tw	.Putf("samplerDesc.AddressW	= %s;\n", GetTextureAddressMode( samplerState.AddressW ).ToChars() );

		tw	<< MXC("samplerDesc.MipLODBias		= 0;\n")
			<< MXC("samplerDesc.MaxAnisotropy	= 1;\n")
			<< MXC("samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;\n")
			<< MXC("samplerDesc.BorderColor[0]	= 1.0f;\n")
			<< MXC("samplerDesc.BorderColor[1]	= 1.0f;\n")
			<< MXC("samplerDesc.BorderColor[2]	= 1.0f;\n")
			<< MXC("samplerDesc.BorderColor[3]	= 1.0f;\n")
			<< MXC("samplerDesc.MinLOD			= -FLT_MAX;\n")
			<< MXC("samplerDesc.MaxLOD			= +FLT_MAX;\n")
			;

		tw.NewLine();

		tw	.Putf("Create_SamplerState( samplerDesc, %s%s );\n",
			WR_SamplerState_PREFIX, samplerState.name.ToChars() );
	}
	//===========================================================================
	void Emit_Initialize_DepthStencilStates( TList< wrDepthStencilState >& depthStencilStates, MyTextWriter & tw )
	{
		const UINT numDepthStencilStates = depthStencilStates.Num();

		if( !numDepthStencilStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Depth-Stencil states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iDepthStencilState = 0;
			iDepthStencilState < numDepthStencilStates;
			iDepthStencilState++ )
		{
			wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];

			Emit_Comment_Source_Location( depthStencilState.pos, tw );
			buffer.Format( "DepthStencilState %s%s;\n",
				WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupDepthStencilStates( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		tw	<< MXC("D3D11_DEPTH_STENCIL_DESC depthStencilDesc;\n")
			<< MXC("ZERO_OUT( depthStencilDesc );\n")
			;

		for( UINT iDepthStencilState = 0;
			iDepthStencilState < numDepthStencilStates;
			iDepthStencilState++ )
		{
			wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];

			Emit_Initialize_DepthStencilState( depthStencilState, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_DepthStencilState( wrDepthStencilState& depthStencilState, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( depthStencilState, tw );
		wrScope	sc(tw);

		tw	.Putf("depthStencilDesc.DepthEnable	= %s;\n", get_boolean_constant_string( depthStencilState.DepthEnable ).ToChars() );
		tw	.Putf("depthStencilDesc.DepthWriteMask	= %s;\n", get_depth_write_mask_string( depthStencilState.DepthWriteMask ).ToChars() );
		tw	.Putf("depthStencilDesc.DepthFunc	= %s;\n", get_depth_func_string( depthStencilState.DepthFunc ).ToChars() );
		
		tw	.Putf("depthStencilDesc.StencilEnable	= %s;\n", get_boolean_constant_string( depthStencilState.StencilEnable ).ToChars() );
		tw	<< MXC("depthStencilDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;\n");
		tw	<< MXC("depthStencilDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;\n");

		tw	<< MXC("depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;\n")

			<< MXC("depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;\n")
			<< MXC("depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;\n")

			;

		tw.NewLine();

		tw	.Putf("Create_DepthStencilState( depthStencilDesc, %s%s );\n",
			WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );
	}
	//===========================================================================
	void Emit_Initialize_RasterizerStates( TList< wrRasterizerState >& rasterizerStates, MyTextWriter & tw )
	{
		const UINT numRasterizerStates = rasterizerStates.Num();

		if( !numRasterizerStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Rasterizer states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iRasterizerState = 0;
			iRasterizerState < numRasterizerStates;
			iRasterizerState++ )
		{
			wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];

			Emit_Comment_Source_Location( rasterizerState.pos, tw );
			buffer.Format( "RasterizerState %s%s;\n",
				WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupRasterizerStates( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		tw	<< MXC("D3D11_RASTERIZER_DESC rasterizerDesc;\n")
			<< MXC("ZERO_OUT( rasterizerDesc );\n")
			;

		for( UINT iRasterizerState = 0;
			iRasterizerState < numRasterizerStates;
			iRasterizerState++ )
		{
			wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];

			Emit_Initialize_RasterizerState( rasterizerState, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_RasterizerState( wrRasterizerState& rasterizerState, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( rasterizerState, tw );
		wrScope	sc(tw);

		tw	.Putf("rasterizerDesc.FillMode				= %s;\n", get_fill_mode_string( rasterizerState.FillMode ).ToChars() );
		tw	.Putf("rasterizerDesc.CullMode				= %s;\n", get_cull_mode_string( rasterizerState.CullMode ).ToChars() );
		tw	.Putf("rasterizerDesc.FrontCounterClockwise	= %s;\n", get_boolean_constant_string( rasterizerState.FrontCounterClockwise ).ToChars() );
		tw	.Putf("rasterizerDesc.DepthBias				= %s;\n", get_integer_constant_string( rasterizerState.DepthBias ).ToChars() );
		tw	.Putf("rasterizerDesc.DepthBiasClamp		= %s;\n", get_float_constant_string( rasterizerState.DepthBiasClamp ).ToChars() );
		tw	.Putf("rasterizerDesc.SlopeScaledDepthBias	= %s;\n", get_float_constant_string( rasterizerState.SlopeScaledDepthBias ).ToChars() );
		tw	.Putf("rasterizerDesc.DepthClipEnable		= %s;\n", get_boolean_constant_string( rasterizerState.DepthClipEnable ).ToChars() );
		tw	.Putf("rasterizerDesc.ScissorEnable			= %s;\n", get_boolean_constant_string( rasterizerState.ScissorEnable ).ToChars() );
		tw	.Putf("rasterizerDesc.MultisampleEnable		= %s;\n", get_boolean_constant_string( rasterizerState.MultisampleEnable ).ToChars() );
		tw	.Putf("rasterizerDesc.AntialiasedLineEnable	= %s;\n", get_boolean_constant_string( rasterizerState.AntialiasedLineEnable ).ToChars() );

		tw.NewLine();

		tw	.Putf("Create_RasterizerState( rasterizerDesc, %s%s );\n",
			WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );
	}
	//===========================================================================
	void Emit_Initialize_BlendStates( TList< wrBlendState >& blendStates, MyTextWriter & tw )
	{
		const UINT numBlendStates = blendStates.Num();

		if( !numBlendStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Blend states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iBlendState = 0;
			iBlendState < numBlendStates;
			iBlendState++ )
		{
			wrBlendState& blendState = blendStates[ iBlendState ];

			Emit_Comment_Source_Location( blendState.pos, tw );
			buffer.Format( "BlendState %s%s;\n",
				WR_BlendState_PREFIX, blendState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupBlendStates( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		tw	<< MXC("D3D11_BLEND_DESC blendDesc;\n")
			<< MXC("ZERO_OUT( blendDesc );\n")
			;

		for( UINT iBlendState = 0;
			iBlendState < numBlendStates;
			iBlendState++ )
		{
			wrBlendState& blendState = blendStates[ iBlendState ];

			Emit_Initialize_BlendState( blendState, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_BlendState( wrBlendState& blendState, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( blendState, tw );
		wrScope	sc(tw);

		tw	.Putf("blendDesc.AlphaToCoverageEnable	= %s;\n", get_boolean_constant_string( blendState.AlphaToCoverageEnable ).ToChars() );
		tw	.Putf("blendDesc.IndependentBlendEnable	= FALSE;\n" );
		
		tw	.Putf("blendDesc.RenderTarget[0].BlendEnable	= %s;\n", get_boolean_constant_string( blendState.BlendEnable ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].SrcBlend		= %s;\n", get_blend_mode_string( blendState.SrcBlend ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].DestBlend		= %s;\n", get_blend_mode_string( blendState.DestBlend ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].BlendOp		= %s;\n", get_blend_op_string( blendState.BlendOp ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].SrcBlendAlpha	= %s;\n", get_blend_mode_string( blendState.SrcBlendAlpha ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].DestBlendAlpha	= %s;\n", get_blend_mode_string( blendState.DestBlendAlpha ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].BlendOpAlpha	= %s;\n", get_blend_op_string( blendState.BlendOpAlpha ).ToChars() );
		tw	.Putf("blendDesc.RenderTarget[0].RenderTargetWriteMask	= %s;\n", get_write_mask_string( blendState.RenderTargetWriteMask ).ToChars() );

		tw.NewLine();

		tw	.Putf("Create_BlendState( blendDesc, %s%s );\n",
			WR_BlendState_PREFIX, blendState.name.ToChars() );
	}
	//===========================================================================
	void Emit_Initialize_StateBlocks( TList< wrStateBlock >& stateBlocks, MyTextWriter & tw )
	{
		const UINT numStateBlocks = stateBlocks.Num();

		if( !numStateBlocks ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	State blocks\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		FixedString	buffer;

		for( UINT iStateBlock = 0;
			iStateBlock < numStateBlocks;
			iStateBlock++ )
		{
			wrStateBlock& stateBlock = stateBlocks[ iStateBlock ];

			Emit_Comment_Source_Location( stateBlock.pos, tw );
			buffer.Format( "StateBlock %s%s;\n",
				WR_StateBlock_PREFIX, stateBlock.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("void SetupStateBlocks( const DeviceState& creationParams )\n");
		wrScope	sc(tw);

		for( UINT iStateBlock = 0;
			iStateBlock < numStateBlocks;
			iStateBlock++ )
		{
			wrStateBlock& stateBlock = stateBlocks[ iStateBlock ];

			Emit_Initialize_StateBlock( stateBlock, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_StateBlock( wrStateBlock& stateBlock, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( stateBlock, tw );
		wrScope	sc(tw);

		FixedString	stateBlockName(WR_StateBlock_PREFIX);
		stateBlockName += stateBlock.name;

		FixedString	rasterizerStateName(WR_RasterizerState_PREFIX);
		rasterizerStateName += stateBlock.rasterizerState;
		
		FixedString	depthStencilStateName(WR_DepthStencilState_PREFIX);
		depthStencilStateName += stateBlock.depthStencilState;
		
		FixedString	blendStateName(WR_BlendState_PREFIX);
		blendStateName += stateBlock.blendState;

		tw	.Putf("%s.rasterizer   = &%s;\n",		stateBlockName.ToChars(), rasterizerStateName.ToChars() );
		tw.NewLine();
		tw	.Putf("%s.depthStencil = &%s;\n",		stateBlockName.ToChars(), depthStencilStateName.ToChars() );
		tw	.Putf("%s.stencilRef   = %s;\n",		stateBlockName.ToChars(), stateBlock.stencilRef.ToChars() );
		tw.NewLine();
		tw	.Putf("%s.blend           = &%s;\n",	stateBlockName.ToChars(), blendStateName.ToChars() );
		tw	.Putf("%s.blendFactorRGBA = %s;\n",		stateBlockName.ToChars(), get_rgba_color_string( stateBlock.blendFactorRGBA ).ToChars() );
		tw	.Putf("%s.sampleMask      = %s;\n",		stateBlockName.ToChars(), stateBlock.sampleMask.ToChars() );
	}
	//===========================================================================
	void Emit_Release_Resources( const wrSourceFile& srcFile, MyTextWriter & tw )
	{
		tw.NewLine();
		tw	<< MXC("void ReleaseGPUResources(void)\n");

		wrScope	sc(tw);




		

		for( UINT iShader = 0;
			iShader < srcFile.shaders.Num();
			iShader++ )
		{
			const wrShaderProgram& shader = srcFile.shaders[ iShader ];

			tw.Putf( "%s::Shutdown();\n", Get_ShaderClass_Name(shader) );
		}


		tw.NewLine();


		if( srcFile.samplerStates.Num() )
		{
			for( UINT iSamplerState = 0;
				iSamplerState < srcFile.samplerStates.Num();
				iSamplerState++ )
			{
				const wrSamplerState& samplerState = srcFile.samplerStates[ iSamplerState ];
				tw.Putf("Destroy_SamplerState( %s );\n", Get_SamplerState_Name(samplerState) );
			}
			tw.NewLine();
		}

		if( srcFile.depthStencilStates.Num() )
		{
			for( UINT iDepthStencilState = 0;
				iDepthStencilState < srcFile.depthStencilStates.Num();
				iDepthStencilState++ )
			{
				const wrDepthStencilState& depthStencilState = srcFile.depthStencilStates[ iDepthStencilState ];
				tw.Putf("Destroy_DepthStencilState( %s );\n", Get_DepthStencilState_Name(depthStencilState) );
			}
			tw.NewLine();
		}
		if( srcFile.rasterizerStates.Num() )
		{
			for( UINT iRasterizerState = 0;
				iRasterizerState < srcFile.rasterizerStates.Num();
				iRasterizerState++ )
			{
				const wrRasterizerState& rasterizerState = srcFile.rasterizerStates[ iRasterizerState ];
				tw.Putf("Destroy_RasterizerState( %s );\n", Get_RasterizerState_Name(rasterizerState) );
			}
			tw.NewLine();
		}
		if( srcFile.blendStates.Num() )
		{
			for( UINT iBlendState = 0;
				iBlendState < srcFile.blendStates.Num();
				iBlendState++ )
			{
				const wrBlendState& blendState = srcFile.blendStates[ iBlendState ];
				tw.Putf("Destroy_BlendState( %s );\n", Get_BlendState_Name(blendState) );
			}
			tw.NewLine();
		}
		if( srcFile.renderTargets.Num() )
		{
			for( UINT iRenderTarget = 0;
				iRenderTarget < srcFile.renderTargets.Num();
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = srcFile.renderTargets[ iRenderTarget ];
				tw.Putf("Destroy_RenderTarget( %s );\n", Get_RenderTarget_Name(renderTarget) );
			}
			tw.NewLine();
		}
		if( srcFile.stateBlocks.Num() )
		{
			for( UINT iStateBlock = 0;
				iStateBlock < srcFile.stateBlocks.Num();
				iStateBlock++ )
			{
				const wrStateBlock& stateBlock = srcFile.stateBlocks[ iStateBlock ];
				tw.Putf("ZERO_OUT( %s );\n", Get_StateBlock_Name(stateBlock) );
			}
		}
	}
	//===========================================================================
	const char* Get_SamplerState_Name( const wrSourceElement& o )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_SamplerState_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_SamplerState_Name( const char* str )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_SamplerState_PREFIX, str );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_DepthStencilState_Name( const wrDepthStencilState& samplerState )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_DepthStencilState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_BlendState_Name( const wrBlendState& samplerState )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_BlendState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_RasterizerState_Name( const wrRasterizerState& samplerState )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_RasterizerState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_RenderTarget_Name( const wrRenderTarget& samplerState )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_RenderTarget_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_StateBlock_Name( const wrStateBlock& stateBlock )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_StateBlock_PREFIX, stateBlock.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderConstantBuffer_Name( const wrShaderConstantBuffer& cb )
	{
		static FixedString	tempBuffer;
		//ZERO_OUT(tempBuffer);
		tempBuffer.Format( "%s%s",
			WR_Shader_ConstantBuffer_PREFIX, cb.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderSampler_Name( const wrShaderSamplerState& o )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_Sampler_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderShaderResource_Name( const wrShaderResource& o )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_Texture_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderClass_Name( const wrShaderProgram& o )
	{
		static FixedString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	void Emit_Shader_Declarations( TList< wrShaderProgram >& shaders, MyTextWriter & tw )
	{
		const UINT numShaders = shaders.Num();

		if( !numShaders ) {
			return;
		}

		tw	<< MXC("// Shaders\n\n");

		for( UINT iShader = 0;
			iShader < numShaders;
			iShader++ )
		{
			const wrShaderProgram& shader = shaders[ iShader ];

			Emit_Shader_Declaration(shader,tw);
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_Shader_Declaration( const wrShaderProgram & shader, MyTextWriter & tw )
	{
		Emit_Comment_Source_Location( shader.pos, tw );

		tw.Putf( "struct %s\n", Get_ShaderClass_Name(shader) );
		{
			wrScope	classScope(tw,ELeaveScopeAction::Scope_Semicolon);
			


			// Constant buffers
			if( shader.inputs.constantBuffers.Num() )
			{
				{
					wrSetScopeLevel	d(tw,0);
					tw.Putf("#pragma pack (push,%u)\n", WR_Shader_ConstantBuffer_Alignment);
				}

				for( UINT iCB = 0;
					iCB < shader.inputs.constantBuffers.Num();
					iCB++ )
				{
					const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

					Emit_Constant_Buffer_Declaration(cb,tw);
				}

				{
					wrSetScopeLevel	d(tw,0);
					tw.Putf("#pragma pack (pop)\n");
				}


				{
					wrDecreaseScopeLevel	d(tw,1);
					tw.NewLine();
					tw << "public:	// Constant buffers (manually updated)\n";
				}


				for( UINT iCB = 0;
					iCB < shader.inputs.constantBuffers.Num();
					iCB++ )
				{
					const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

					tw.Putf("static TypedConstantBuffer< %s >	%s;\n"
						,cb.name.ToChars(),Get_ShaderConstantBuffer_Name(cb)
						);
				}
			}//Constant buffers



			// Sampler states
			if( shader.inputs.samplers.Num() )
			{
				{
					wrDecreaseScopeLevel	d(tw,1);
					tw.NewLine();
					tw << "public:	// Sampler states (set manually)\n";
				}

				for( UINT iSamplerState = 0;
					iSamplerState < shader.inputs.samplers.Num();
					iSamplerState++ )
				{
					const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

					tw.Putf("static ID3D11SamplerState* %s;\n"
						,ss.name.ToChars()
					);
				}
			}//Sampler states


			// Shader resources

			if( shader.inputs.resources.Num() )
			{
				{
					wrDecreaseScopeLevel	d(tw,1);
					tw.NewLine();
					tw << "public:	// Shader resources (set manually)\n";
				}

				for( UINT iShaderResource = 0;
					iShaderResource < shader.inputs.resources.Num();
					iShaderResource++ )
				{
					const wrShaderResource& tex = shader.inputs.resources[ iShaderResource ];

					tw.Putf("static ID3D11ShaderResourceView* %s;\n"
						,tex.name.ToChars()
						);
				}

			}//Shader resources











			// Static public fields (attributes)
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Public methods\n";
			}
			Emit_Shader_Public_Methods_Declarations(shader,tw);







			// Static public fields (attributes)
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Read-only properties\n";
			}
			tw << "static const char* VS_EntryPoint;\n";
			tw << "static const char* PS_EntryPoint;\n";
			tw << "static const char* Name;\n";
			tw << "static const rxStaticString Source;\n";





			// Public internal
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public_internal:\n";
			}

			tw << "static void Initialize();\n";
			tw << "static void Shutdown();\n";
			tw.NewLine();
			//tw << "static SizeT GetSourceCodeLength();\n";
			//tw << "static void GetSourceCodeBytes( char *buffer );\n";
			//tw.NewLine();
			tw << "static void GetLoadInfo( ShaderInfo & d );\n";
			tw << "static void Load( const ShaderInfo& shaderInfo, rxHighLevelShaderCompiler& compiler );\n";


			// Private constructor and internal data
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "private:\n";
			}
			tw.Putf( "%s() {}\n", Get_ShaderClass_Name(shader) );

			// Shader instances
			{
				tw.NewLine();

				const UINT numShaderInstances = shader.NumInstances();
				tw.Putf("static ShaderInstance	shaderInstances[%u];\n",numShaderInstances);
			}



			// PREVENT_COPY( className )
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw.Putf( "private:PREVENT_COPY(%s);\n", Get_ShaderClass_Name(shader) );
			}
		}
		tw.NewLine();
	}
	//===========================================================================
	void Emit_Constant_Buffer_Declaration( const wrShaderConstantBuffer& cb, MyTextWriter & tw )
	{
		tw.Putf("struct %s\n",cb.name.ToChars());
		{
			wrScope	cbScope(tw,ELeaveScopeAction::Scope_Semicolon);

			for( UINT iCBElement = 0;
				iCBElement < cb.elements.Num();
				iCBElement++ )
			{
				const wrCBVar& elem = cb.elements[ iCBElement ];
				tw.Putf("%s	%s;\n",
					elem.typeName.ToChars(),
					elem.name.ToChars()
					);
			}
		}
	}
	//===========================================================================
	void Emit_Shader_Public_Methods_Declarations( const wrShaderProgram& shader, MyTextWriter & tw )
	{
		tw << "static inline void Set()\n";
		wrScope	functionScope(tw);



		RX_OPTIMIZE("bind to slots intelligently (bind the whole range at once)");


		if( shader.inputs.constantBuffers.Num() )
		{
			tw << "// Bind constant buffers\n";
			for( UINT iCB = 0;
				iCB < shader.inputs.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

				if( !cb.isUsed() ) {
					continue;
				}

				Assert( cb.iRegister != INDEX_NONE );

				if( cb.usedByVS() )
				{
					tw.Putf("%s.BindTo_VS(%u);\n"
						,Get_ShaderConstantBuffer_Name(cb)
						,cb.iRegister
						);
				}
				if( cb.usedByGS() )
				{
					tw.Putf("%s.BindTo_GS(%u);\n"
						,Get_ShaderConstantBuffer_Name(cb)
						,cb.iRegister
						);
				}
				if( cb.usedByPS() )
				{
					tw.Putf("%s.BindTo_PS(%u);\n"
						,Get_ShaderConstantBuffer_Name(cb)
						,cb.iRegister
						);
				}
			}
		}// Bind constant buffers




		if(shader.inputs.samplers.Num())
		{

			tw.NewLine();

		tw << "// Bind sampler states to slots\n";
		for( UINT iSamplerState = 0;
			iSamplerState < shader.inputs.samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

			if( !ss.isUsed() ) {
				continue;
			}

			Assert( ss.iRegister != INDEX_NONE );

#if 0
			if( ss.usedByVS() )
			{
				tw.Putf("SetSamplerVS(%u,%s);\n"
					,ss.iRegister
					,Get_ShaderSampler_Name(ss)
					
					);
			}
			if( ss.usedByGS() )
			{
				tw.Putf("SetSamplerGS(%u,%s);\n"
					,ss.iRegister
					,Get_ShaderSampler_Name(ss)
					
					);
			}
			if( ss.usedByPS() )
			{
				tw.Putf("SetSamplerPS(%u,%s);\n"
					,ss.iRegister
					,Get_ShaderSampler_Name(ss)
					
					);
			}
#else
			if( ss.usedByVS() )
			{
				tw << "D3DContext->VSSetSamplers(\n";
				tw.Putf("\t%u,\n",ss.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
				tw << ");\n";
			}
			if( ss.usedByGS() )
			{
				tw << "D3DContext->GSSetSamplers(\n";
				tw.Putf("\t%u,\n",ss.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
				tw << ");\n";
			}
			if( ss.usedByPS() )
			{
				tw << "D3DContext->PSSetSamplers(\n";
				tw.Putf("\t%u,\n",ss.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
				tw << ");\n";
			}


#endif
		}
		}//Bind sampler states


		if(shader.inputs.resources.Num())
		{
			tw.NewLine();

		tw << "// Bind shader resources\n";
		for( UINT iShaderResource = 0;
			iShaderResource < shader.inputs.resources.Num();
			iShaderResource++ )
		{
			const wrShaderResource& tex = shader.inputs.resources[ iShaderResource ];

			if( !tex.isUsed() ) {
				continue;
			}

			Assert( tex.iRegister != INDEX_NONE );

#if 0
			if( tex.usedByVS() )
			{
				tw.Putf("SetShaderResourceVS(%u,%s);\n"
					,tex.iRegister
					,Get_ShaderShaderResource_Name(tex)

					);
			}
			if( tex.usedByGS() )
			{
				tw.Putf("SetShaderResourceGS(%u,%s);\n"
					,tex.iRegister
					,Get_ShaderShaderResource_Name(tex)

					);
			}
			if( tex.usedByPS() )
			{
				tw.Putf("SetShaderResourcePS(%u,%s);\n"
					,tex.iRegister
					,Get_ShaderShaderResource_Name(tex)

					);
			}
#else
			if( tex.usedByVS() )
			{
				tw << "D3DContext->VSSetShaderResources(\n";
				tw.Putf("\t%u,\n",tex.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
				tw << ");\n";
			}
			if( tex.usedByGS() )
			{
				tw << "D3DContext->GSSetShaderResources(\n";
				tw.Putf("\t%u,\n",tex.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
				tw << ");\n";
			}
			if( tex.usedByPS() )
			{
				tw << "D3DContext->PSSetShaderResources(\n";
				tw.Putf("\t%u,\n",tex.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
				tw << ");\n";
			}
#endif
		}
		}//Bind Texture resources



		



		tw.NewLine();
		tw << "// Bind shader program\n";
		tw << "SetVertexShader( shaderInstances[0].vertexShader );\n";
		tw << "SetGeometryShader( nil );\n";
		tw << "SetPixelShader( shaderInstances[0].pixelShader );\n";

	}
	//===========================================================================
	void Emit_Shader_Definitions( TList< wrShaderProgram >& shaders, MyTextWriter & tw )
	{
		const UINT numShaders = shaders.Num();

		if( !numShaders ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Shaders\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		for( UINT iShader = 0;
			iShader < numShaders;
			iShader++ )
		{
			wrShaderProgram& shader = shaders[ iShader ];

			Emit_Shader_Definition( shader, tw );
		}
	}
	//===========================================================================
	void Emit_Shader_Definition( const wrShaderProgram& shader, MyTextWriter & tw )
	{
		const char* shaderClassName = Get_ShaderClass_Name(shader);
		const UINT numShaderInstances = shader.NumInstances();
		(void)numShaderInstances;

		//Constant buffers
		for( UINT iCB = 0;
			iCB < shader.inputs.constantBuffers.Num();
			iCB++ )
		{
			const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

			//tw << "//[static]\n";
			tw.Putf("TypedConstantBuffer< %s::%s >	%s::%s;\n"
				,Get_ShaderClass_Name(shader),cb.name.ToChars()
				,Get_ShaderClass_Name(shader),Get_ShaderConstantBuffer_Name(cb)
				);
		}//Constant buffers

		



		// Sampler states
		for( UINT iSamplerState = 0;
			iSamplerState < shader.inputs.samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

			//tw << "//[static]\n";
			tw.Putf("ID3D11SamplerState* %s::%s = nil;\n"
				,Get_ShaderClass_Name(shader),Get_ShaderSampler_Name(ss)
				);
		}// Sampler states




		// Shader resources

		for( UINT iShaderResource = 0;
			iShaderResource < shader.inputs.resources.Num();
			iShaderResource++ )
		{
			const wrShaderResource& res = shader.inputs.resources[ iShaderResource ];

			//tw << "//[static]\n";
			tw.Putf("ID3D11ShaderResourceView* %s::%s = nil;\n"
				,Get_ShaderClass_Name(shader),Get_ShaderShaderResource_Name(res)
				);
		}//Shader resources



		// Read-only properties
		{
			tw.Putf("const char* %s::VS_EntryPoint = \"%s\";\n"
				,Get_ShaderClass_Name(shader)
				,shader.vertexShader.ToChars()
				);

			tw.Putf("const char* %s::PS_EntryPoint = \"%s\";\n"
				,Get_ShaderClass_Name(shader)
				,shader.pixelShader.ToChars()
				);

			tw.Putf("const char* %s::Name = \"%s\";\n"
				,Get_ShaderClass_Name(shader)
				,shader.name.ToChars()
				);




			// shader source code
			{
				TextBuffer	finalSrcCode;

				ComposeFinalHLSLSourceCode(shader,finalSrcCode);


				const SizeT srcCodeLength = finalSrcCode.Num();

				const char* srcCodeBytes = (const char*)finalSrcCode.ToPtr();
				StaticAssert( sizeof srcCodeBytes[0] == sizeof finalSrcCode.ToPtr()[0] );

				tw.Putf("const rxStaticString %s::Source =\n"
					,Get_ShaderClass_Name(shader)
					);


				wrScope	sc(tw,Scope_Semicolon);
				tw.Putf("%u, // length\n",srcCodeLength);

				// emit shader source code

				wrScope	sc2(tw);



				//enum { MAX_COLUMNS = 60 };
				enum { MAX_COLUMNS = 300 };
				enum { NUM_CHARS_IN_COLUMN = 4 };	// e.g. "0x73"

				FixedString	buffer;

				mxStreamWriter* outStream = tw.GetStream();

				SizeT iByte = 0;

				

				while(true)
				{
					for(UINT iColumn = 0;
						iColumn < MAX_COLUMNS/NUM_CHARS_IN_COLUMN;
						iColumn++ )
					{
						if( iByte >= srcCodeLength ) {
							goto L_End;
						}
						UINT c = srcCodeBytes[ iByte++ ];

						buffer.Format("0x%x,",c);
						
						outStream->Write( buffer.ToChars(), buffer.Length() );
					}

					outStream->Write( "\n", 1 );
				}

L_End:
				tw.NewLine();

			}// shader source code


		}
		// Read-only properties





		// Shader instances

		//tw << "//[static]\n";
		tw.Putf("ShaderInstance %s::shaderInstances[%u];\n"
			,Get_ShaderClass_Name(shader),shader.NumInstances()
			);

		// Shader instances



		tw.NewLine();





		//-------------------
		// Functions.
		//-------------------

		//Initialize
		{
			tw.Putf("void %s::Initialize()\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);

			//Constant buffers
			for( UINT iCB = 0;
				iCB < shader.inputs.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

				//tw << "//[static]\n";
				tw.Putf("%s::%s.Create( GetDeviceContext() );\n"
					,Get_ShaderClass_Name(shader)
					,Get_ShaderConstantBuffer_Name(cb)
					);
			}//Constant buffers


			tw.NewLine();
			tw.Putf("ShaderInfo d;\n");
			tw.Putf("%s::GetLoadInfo( d );\n",shaderClassName);
			tw.NewLine();
			tw << "RX_DEBUG_CODE(DbgResourcesDatabase::Get().Register_Shader( d ));\n";
			tw.NewLine();
			tw << "HighLevelShaderCompiler	compiler;\n";
			tw.Putf("%s::Load( d, compiler );\n",shaderClassName);
		}//Initialize



/*
		tw.NewLine();


		//SizeT GetSourceCodeLength()
		{
			tw.Putf("SizeT %s::GetSourceCodeLength();\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);
		}//SizeT GetSourceCodeLength()
		

		tw.NewLine();


		//void GetSourceCodeBytes( char *buffer )
		{
			tw.Putf("void %s::GetSourceCodeBytes( char *buffer );\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);
		}//void GetSourceCodeBytes( char *buffer )
*/


		tw.NewLine();


		//void GetLoadInfo( ShaderInfo & d )
		{
			tw.Putf("void %s::GetLoadInfo( ShaderInfo & d )\n"
				,shaderClassName);
			wrScope	sc(tw);

			tw.Putf("d.source.code = %s::Source.data;\n",shaderClassName);
			tw.Putf("d.source.codeLength = %s::Source.size;\n",shaderClassName);

			tw.Putf("d.VS_EntryPoint = %s::VS_EntryPoint;\n",shaderClassName);
			tw.Putf("d.PS_EntryPoint = %s::PS_EntryPoint;\n",shaderClassName);

			tw.Putf("d.name = %s::Name;\n",shaderClassName);

			tw.Putf("d.load = %s::Load;\n",shaderClassName);

		}//void GetLoadInfo( ShaderInfo & d )



		tw.NewLine();



		//Load()
		{
			tw.Putf("void %s::Load( const ShaderInfo& shaderInfo, rxHighLevelShaderCompiler& compiler )\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);



			// Sampler states
			for( UINT iSamplerState = 0;
				iSamplerState < shader.inputs.samplers.Num();
				iSamplerState++ )
			{
				const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

				FixedString	samplerInitializer;

				if( ss.initializer.IsEmpty() )
				{
					samplerInitializer = "nil";
				}
				else
				{
					samplerInitializer.Format("%s.p.Ptr",
						Get_SamplerState_Name(ss.initializer.ToChars()));
				}

				tw.Putf("%s::%s = %s;\n"
					,Get_ShaderClass_Name(shader),Get_ShaderSampler_Name(ss)
					,samplerInitializer.ToChars()
					);

			}// Sampler states




			tw << "compiler.NewShaderInstance( shaderInfo, shaderInstances[0] );\n";

			//tw << "Create_VertexShader( d.source, d.VS_EntryPoint, shaderInstances[0].vertexShader );\n";
			//tw << "Create_PixelShader( d.source, d.PS_EntryPoint, shaderInstances[0].pixelShader );\n";

			//tw << "d.instances = shaderInstances;\n";
			//tw << "d.numInstances = 1;\n";

		}//void Load( ShaderInfo & d )



		tw.NewLine();



		//Shutdown
		{
			tw.Putf("void %s::Shutdown()\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);

			//Constant buffers
			for( UINT iCB = 0;
				iCB < shader.inputs.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];

				//tw << "//[static]\n";
				tw.Putf("%s::%s.Destroy();\n"
					, Get_ShaderClass_Name(shader)
					,Get_ShaderConstantBuffer_Name(cb)
					);
			}//Constant buffers


			tw.NewLine();

			tw.Putf("Destroy_ShaderInstances( %u, %s::shaderInstances );\n"
				,shader.NumInstances(), Get_ShaderClass_Name(shader));

		}//Shutdown
	}
	//===========================================================================

	void ComposeFinalHLSLSourceCode( const wrShaderProgram& shader, TextBuffer & finalSrcCode )
	{
		finalSrcCode.Reserve(shader.code.Num());
		finalSrcCode.Empty();
		finalSrcCode.ZeroOut();

		TextBuffer::OStream codeWriter = finalSrcCode.GetOStream();
		MyTextWriter	tw(&codeWriter);


		tw.Putf(
			"#line %d \"%s\"\n",
			shader.startOfInputs.line,
			shader.startOfInputs.file.ToChars()
			);

		//Constant buffers
		for( UINT iCB = 0;
			iCB < shader.inputs.constantBuffers.Num();
			iCB++ )
		{
			const wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];


			tw.Putf("cbuffer %s : register(b%u)\n"
				,cb.name.ToChars()
				,cb.iRegister
				);

			wrScope	cbScope(tw,Scope_Semicolon);

			for( UINT iCBElement = 0;
				iCBElement < cb.elements.Num();
				iCBElement++ )
			{
				const wrCBVar& cbVar = cb.elements[iCBElement];

				tw.Putf("%s %s;\n",
					cbVar.typeName.ToChars(),
					cbVar.name.ToChars());
			}
		}



		// Sampler states
		for( UINT iSamplerState = 0;
			iSamplerState < shader.inputs.samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

			tw.Putf("SamplerState %s : register(s%u);\n"
				,ss.name.ToChars()
				,ss.iRegister
				);
		}// Sampler states




		// Shader resources

		for( UINT iShaderResource = 0;
			iShaderResource < shader.inputs.resources.Num();
			iShaderResource++ )
		{
			const wrShaderResource& res = shader.inputs.resources[ iShaderResource ];

			tw.Putf("Texture2D %s : register(t%u);\n"
				,res.name.ToChars()
				,res.iRegister
				);
		}//Shader resources


		tw.NewLine();

		tw.Putf(
			"#line %d \"%s\"\n",
			shader.startOfCode.line,
			shader.startOfCode.file.ToChars()
		);


		finalSrcCode.Append( shader.code );


#if MX_DEBUG
		FixedString	dumpFileName;
		dumpFileName.Format("%s.hlsl",shader.name.ToChars());

		FileWriter	fileWriter(dumpFileName.ToChars());
		MyTextWriter	tw2(&fileWriter);
		fileWriter.Write(finalSrcCode.ToChars(),finalSrcCode.GetDataSize());
		//tw2.Emit( finalSrcCode.ToChars() );
#endif
	}
	
	//===========================================================================


	void Resolve( const Options& config, wrSourceFile& srcFile )
	{
		TList< wrShaderProgram > & shaders = srcFile.shaders;

		const UINT numShaders = shaders.Num();

		if( !numShaders ) {
			return;
		}

		for( UINT iShader = 0;
			iShader < numShaders;
			iShader++ )
		{
			wrShaderProgram& shader = shaders[ iShader ];

			Resolve_Shader_Program( shader );
		}
	}

	//===========================================================================

	UINT MeasureBracedSectionLength( const char* s )
	{
		const char* p = s;

		while( *p && *p != '{') {
			++p;
		}

		UINT braceDepth = 1;
		while(*p && braceDepth)
		{
			++p;
			if( *p == '{' ) {
				braceDepth++;
			}
			if( *p == '}' ) {
				braceDepth--;
			}
		}

		if( braceDepth > 0 ) {
			mxErrf("no matching closing brace");
		}
		return p - s;
	}

	//===========================================================================
	void Resolve_Shader_Program( wrShaderProgram & shader )
	{
		Assert(!shader.vertexShader.IsEmpty());
		Assert(!shader.pixelShader.IsEmpty());

		shader.vsEntryOffset = strstr(
			(char*)shader.codeBlob.ToChars(),
			shader.vertexShader.ToChars() );

		if( !shader.vsEntryOffset ) {
			mxErrf("Failed to find vertex shader entry point '%s' in shader '%s'"
				,shader.vertexShader.ToChars(),shader.name.ToChars());
		}

		shader.vsLength = MeasureBracedSectionLength(shader.vsEntryOffset);




		shader.psEntryOffset = strstr(
			(char*)shader.codeBlob.ToChars(),
			shader.pixelShader.ToChars() );

		if( !shader.psEntryOffset ) {
			mxErrf("Failed to find pixel shader entry point '%s' in shader '%s'"
				,shader.pixelShader.ToChars(),shader.name.ToChars());
		}

		shader.psLength = MeasureBracedSectionLength(shader.psEntryOffset);




		FindUnusedVarsInCB(shader);
		FindUnusedVars( shader.inputs.samplers, shader, 1 );
		FindUnusedVars( shader.inputs.resources, shader, 1 );






		Resolve_Shader_Constant_Buffers( shader.inputs.constantBuffers );
		Resolve_Shader_Sampler_States( shader.inputs.samplers );
		Resolve_Shader_Resources( shader.inputs.resources );
	}

	//===========================================================================

	void FindUnusedVarsInCB(wrShaderProgram & shader)
	{
		TList< wrShaderConstantBuffer >	usedConstantBuffers;

		for( UINT iCB = 0;
			iCB < shader.inputs.constantBuffers.Num();
			iCB++ )
		{
			wrShaderConstantBuffer& cb = shader.inputs.constantBuffers[ iCB ];
			
			
			cb.bindFlags = 0;
			
			FindUnusedVars( cb.elements, shader, 1 );

			UINT numUsedVars = 0;

			for( UINT iCBElement = 0;
				iCBElement < cb.elements.Num();
				iCBElement++ )
			{
				const wrCBVar& elem = cb.elements[ iCBElement ];
				if( elem.isUsed() ) {
					++numUsedVars;
				}

				cb.bindFlags |= elem.bindFlags;
			}

			if( numUsedVars )
			{
				usedConstantBuffers.Add(cb);
			}
			else
			{
				// this constant buffer is unused
				//unreferenced
				Warning(cb,"constant buffer '%s' is unused\n",cb.name.ToChars());
			}
		}

		shader.inputs.constantBuffers = usedConstantBuffers;

	}

	//===========================================================================

	template< class T, class BIT_SET >
	void AssignSlots( TList< T > & elements, BIT_SET & bitSlots, UINT maxSlot )
	{
		Assert( bitSlots.Capacity() >= maxSlot );

		bitSlots.setAll( 0 );

		for( UINT i = 0; i < elements.Num(); i++ )
		{
			T & elem = elements[ i ];

			if( elem.iRegister != INDEX_NONE )
			{
				// the bind point was specified using the register keyword

				Assert(elem.iRegister <= maxSlot);

				if( bitSlots.get( elem.iRegister ) )
				{
					Warning( elem, "The slot %u for %s is already taken\n"
						,elem.iRegister
						,elem.name.ToChars()
					);
				}

				// mark this slot as taken
				bitSlots.set( elem.iRegister );
			}
			else
			{
				// Find a free slot.

				const int freeSlot = bitSlots.firstZeroBit();

				if ( freeSlot > maxSlot )
				{
					Warning( elem,"Out of free slots for %s\n", elem.name.ToChars() );

					elem.iRegister = maxSlot;
				}
				else
				{
					elem.iRegister = freeSlot;

					// mark this slot as taken
					bitSlots.set( freeSlot );
				}
			}
		}
	}
	//===========================================================================
	template< class T >	// where T : wrRegisterBound
	void FindUnusedVars(
		TList< T > & elements, const wrShaderProgram& shader,
		bool bRemoveUnusedVars = false )
	{
		//char* srcCode = (char*) shader.code.ToChars();

		TList< T >	usedVars;

		for( UINT i = 0; i < elements.Num(); i++ )
		{
			T & elem = elements[ i ];

#if 0
			char* p = strstr( srcCode, elem.name.ToChars() );
			elem.bUsed = (p != nil);

			if( elem.bUsed ) {
				usedVars.Add(elem);
			}
			else {
				Warning(elem,"unreferenced variable: '%s'\n",elem.name.ToChars());
			}
#else
			elem.bindFlags = 0;

			const bool usedByVS = MyStrStr(
				shader.vsEntryOffset, elem.name.ToChars(),
				shader.vsLength
				) != nil;
			if( usedByVS ) {
				elem.bindFlags |= CB_Bind_VS;
			}

			const bool usedByPS = MyStrStr(
				shader.psEntryOffset, elem.name.ToChars(),
				shader.psLength
				) != nil;
			if( usedByPS ) {
				elem.bindFlags |= CB_Bind_PS;
			}


			if( elem.isUsed() ) {
				usedVars.Add(elem);
			}
			else {
				Warning(elem,"unreferenced variable: '%s'\n",elem.name.ToChars());
			}
#endif
		}

		if( bRemoveUnusedVars ) {
			if( usedVars.Num() < elements.Num() ) {
				elements = usedVars;
			}
		}
	}
	//===========================================================================
	void Resolve_Shader_Constant_Buffers( TList< wrShaderConstantBuffer > & constantBuffers )
	{
		BitSet32	bitSlots;
		//CHECK_STORAGE( sizeof(bitSlots), MAX_CONSTANT_BUFFER_SLOTS );
		AssignSlots( constantBuffers, bitSlots, MAX_CONSTANT_BUFFER_SLOTS );

		//for( UINT iCB = 0;
		//	iCB < constantBuffers.Num();
		//	iCB++ )
		//{
		//	wrShaderConstantBuffer& cb = constantBuffers[ iCB ];
		//}
	}
	//===========================================================================
	void Resolve_Shader_Sampler_States( TList< wrShaderSamplerState > & samplers )
	{
		BitSet32	bitSlots;
		AssignSlots( samplers, bitSlots, MAX_SAMPLER_STATE_SLOTS );

		//for( UINT iSamplerState = 0;
		//	iSamplerState < samplers.Num();
		//	iSamplerState++ )
		//{
		//	wrShaderSamplerState& ss = samplers[ iSamplerState ];
		//	(void)ss;
		//}
	}
	//===========================================================================
	void Resolve_Shader_Resources( TList< wrShaderResource > & resources )
	{
		BitSet32	bitSlots;
		AssignSlots( resources, bitSlots, MAX_SHADER_RESOURCE_SLOTS );

		//for( UINT iShaderResource = 0;
		//	iShaderResource < resources.Num();
		//	iShaderResource++ )
		//{
		//	wrShaderResource& tex = resources[ iShaderResource ];
		//	(void)tex;
		//}
	}
	//===========================================================================
	void Warning( const wrSourceElement& element, const char* fmt, ... )
	{
		char	temp[ MAX_STRING_CHARS ];
		MX_GET_VARARGS_ANSI( temp, fmt );

		const wrLocation & loc = element.pos;
	
		FixedString	fileLineInfo;
		FileLineNumberInfoToString(
			loc.file.ToChars(),
			loc.line, loc.column, fileLineInfo
			);

		mxPutf("%s%s",fileLineInfo.ToChars(),temp);
	}

private:
	PREVENT_COPY(HLSLTranslator);
};

/*
=============================================================================
*/
void Translate( const Options& config, const ParseResults& input )
{
	HLSLTranslator	translator;
	// remember, const is a virus
	ParseResults& nonConstInput = const_cast< ParseResults& >( input );
	translator.Translate( config, nonConstInput );
}

