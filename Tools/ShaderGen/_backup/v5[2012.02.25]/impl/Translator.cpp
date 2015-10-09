#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

#include "DX11_Util.h"

//marker
#define SHADER_GEN_HADRCODED

//------------------------------------------------------------------------
//	Configuration
//------------------------------------------------------------------------

#define WR_VERSION_STRING	MXC("0.5")

#define WR_HEADER_FILE_EXT	MXC(".hxx")
#define WR_SOURCE_FILE_EXT	MXC(".cxx")

//#define WR_MAIN_HEADER_FILE_NAME	MXC("Main.hxx")
//#define WR_MAIN_SOURCE_FILE_NAME	MXC("Main.cxx")

#if 0
#define WR_RenderTarget_PREFIX			"renderTarget_"
#define WR_SamplerState_PREFIX			"samplerState_"
#define WR_RasterizerState_PREFIX		"rasterizerState_"
#define WR_BlendState_PREFIX			"blendState_"
#define WR_DepthStencilState_PREFIX		"depthStencilState_"
#define WR_StateBlock_PREFIX			"renderState_"
#else
#define WR_RenderTarget_PREFIX			""
#define WR_SamplerState_PREFIX			""
#define WR_RasterizerState_PREFIX		""
#define WR_BlendState_PREFIX			""
#define WR_DepthStencilState_PREFIX		""
#define WR_StateBlock_PREFIX			""
#endif

#define WR_Shader_PREFIX	""

#define WR_Shader_ConstantBuffer_PREFIX	"cb_"

#define WR_Shader_Sampler_PREFIX	""
#define WR_Shader_Texture_PREFIX	""

#define WR_Shader_ConstantBuffer_Alignment	UINT(16)

static bool g_bEmitComments = false;


//===========================================================================

OSFileName GetHeaderFileName( const Options& config, const wrSourceFile& srcFile )
{
	OSFileName	tmp(srcFile.pureFileName);
	tmp += WR_HEADER_FILE_EXT;
	return tmp;
	//return GetDestFileName( srcFile.pureFileName, WR_HEADER_FILE_EXT );
}
//===========================================================================
OSFileName GetSourceFileName( const Options& config, const wrSourceFile& srcFile )
{
	OSFileName	tmp(srcFile.pureFileName);
	tmp += WR_SOURCE_FILE_EXT;
	return tmp;
	//return GetDestFileName( srcFile.pureFileName, WR_SOURCE_FILE_EXT );
}
static
OSFileName GetDestFileName( const OSFileName& srcFileName, ConstCharPtr& extension )
{
	OSFileName	destFileName( srcFileName );
	destFileName.StripFileExtension();
	destFileName += extension;
	return destFileName;
}
const char* Get_Shared_Section_Name( const char* originalName )
{
	static StackString	tempBuffer;
	tempBuffer.Format( "%s%s",
		"Shared_", originalName );
	return tempBuffer.ToChars();
}
static
wrBuffer GetCurrentDateTimeString()
{
	wrBuffer	currentDateTime;

	CalendarTime	localTime( CalendarTime::GetCurrentLocalTime() );

	StackString	timeOfDay;
	GetTimeOfDayString( timeOfDay, localTime.hour, localTime.minute, localTime.second );

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
	explicit MyTextWriter( mxStreamWriter& stream )
		: mxTextWriter( stream ), scopeDepth( 0 )
	{
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
	void LeaveNamedScope( const char* text, ELeaveScopeAction action = ELeaveScopeAction::Scope_NoAction)
	{
		Assert( scopeDepth > 0 );
		--scopeDepth;
		switch(action)
		{
		case ELeaveScopeAction::Scope_NoAction :
			this->Putf( "}%s\n", text );
			break;

		case ELeaveScopeAction::Scope_Semicolon :
			this->Putf( "};%s\n", text );
			break;

		default:
			Unreachable;
		}
	}
	// writes string with indentation
	OVERRIDEN void Put( const char* s, UINT length )
	{
		if( scopeDepth > 0 ) {
			mStream.Write( tabs, scopeDepth );
		}
		mStream.Write( s, length );
	}
	void NewLine()
	{
		Put("\n",1);
	}
	// writes string only, without indentation, doesn't write tabs
	MyTextWriter& Emit( const char* s, UINT length )
	{
		mStream.Write( s, length );
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
		mStream.Write( "\n", 1 );
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
	String scopeName;
public:
	wrScope( MyTextWriter& tw, ELeaveScopeAction action = ELeaveScopeAction::Scope_NoAction )
		: writer( tw ), onLeaveScopeAction(action)
	{
		writer.EnterScope();
	}
	~wrScope()
	{
		if( !scopeName.IsEmpty() ) {
			writer.LeaveNamedScope(scopeName,onLeaveScopeAction);
		} else {
			writer.LeaveScope(onLeaveScopeAction);
		}
	}
	void SetName( const char* name )
	{
		scopeName = name;
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
	if(!g_bEmitComments) {return;}

	tw.Emitf( "// %s\n", pos.ToStr().ToChars() );

}
void Emit_Comment_Source_Element_Info( const wrSourceElement& elem, MyTextWriter & tw )
{
	if(!g_bEmitComments) {return;}

	tw.Emitf( "// %s\n", elem.name.ToChars() );
	if( !elem.info.IsEmpty() ) {
		tw.Emitf( "// %s\n", elem.info.ToChars() );
	}
	Emit_Comment_Source_Location( elem.pos, tw );
}

/*
=============================================================================
	HLSLTranslator
=============================================================================
*/
class HLSLTranslator
{
	Options	m_options;

public:
	HLSLTranslator()
	{
	}

	void Translate( const Options& config, ParseResults& input )
	{
		m_options = config;

		wrShaderLibrary	shaderLib(input);

		shaderLib.Resolve(config);

		WriteCode(config,shaderLib);
	}
//===========================================================================
	void WriteCode(const Options& config, const wrShaderLibrary& shaderLib)
	{
		Emit_Main(config,shaderLib);
		Generate_HLSL_File_With_Vertex_Formats(config,shaderLib);
	}
	//===========================================================================
	void Generate_HLSL_File_With_Vertex_Formats(const Options& config, const wrShaderLibrary& shaderLib)
	{
		if( shaderLib.vertexDeclarations.Num() )
		{
			StackString	hlslFileName;
			hlslFileName.Format("%sh_vertex_lib_AUTO.h",
				m_options.outputFolderHLSL.ToChars());

			FileWriter		fileWriter(hlslFileName.ToChars(),0);
			MyTextWriter	tw(fileWriter);


			const StackString	includeGuardStr(MXC("H_VERTEX_LIB_AUTO"));
			tw.Putf("#ifndef %s\n",includeGuardStr.ToChars());
			tw.Putf("#define %s\n",includeGuardStr.ToChars());
			tw.NewLine();

			for( UINT iVtx = 0;
				iVtx < shaderLib.vertexDeclarations.Num();
				iVtx++ )
			{
				const wrVertexDeclaration& vtx = shaderLib.vertexDeclarations[ iVtx ];

				Write_HLSL_Code_For_Vertex_Format(config,vtx,tw);

				tw.NewLine();
			}

			tw.Putf("#endif // !%s\n",includeGuardStr.ToChars());
			tw.NewLine();
		}
	}
		/*
	=============================================================================
	*/
	void Write_HLSL_Code_For_Vertex_Format(const Options& config, const wrVertexDeclaration& vtx,MyTextWriter & tw )
	{
		if(config.bEmitHLSLComments)
		{
			Emit_Comment_Source_Location(vtx.pos,tw);
		}

		tw.Putf("struct %s\n",vtx.name.ToChars());
		wrScope	sc(tw,Scope_Semicolon);

		for(UINT iVertexElement = 0;
			iVertexElement < vtx.elements.Num();
			iVertexElement++)
		{
			const wrVertexElement & elemDesc = vtx.elements[ iVertexElement ];

			const EVertexElementUsage semantic = rxUtil_GetVertexElementSemanticEnum(elemDesc.SemanticName.ToChars());

			const DXGI_FORMAT dxgi_format = String_ToDXGIFormat(elemDesc.Format.ToChars());

			StackString		nameInShader;

			rxUtil_GenerateVertexElementName(
				semantic,
				elemDesc.SemanticName.ToChars(),
				elemDesc.SemanticIndex,
				nameInShader
			);

			tw.Putf("%s	%s : %s; // %s\n"
				,DXGIFormat_TypeString(dxgi_format)
				,nameInShader.ToChars()
				,elemDesc.SemanticName.ToChars()
				,elemDesc.info.ToChars()
				);
		}
	}
	/*
	=============================================================================
	*/
	StackString GetModuleInitFunctionName( const wrSourceFile& srcFile )
	{
		StackString	tmp;
		tmp.Format("InitializeModule_%s",srcFile.pureFileName.ToChars());
		return tmp;
	}
	/*
	=============================================================================
	*/
	StackString GetModuleShutdownFunctionName( const wrSourceFile& srcFile )
	{
		StackString	tmp;
		tmp.Format("ShutdownModule_%s",srcFile.pureFileName.ToChars());
		return tmp;
	}
	//===========================================================================
	void Emit_Include_Header_Files( const Options& config, const wrSourceFile& srcFile, MyTextWriter & tw )
	{
		tw.NewLine();

		OSFileName	headerFileName = GetHeaderFileName( config, srcFile );
		tw.Putf( "#include \"%s\"\n", headerFileName.ToChars() );

		tw.NewLine();
	}
	/*
	=============================================================================
	*/
	void Emit_Main( const Options& config, const wrShaderLibrary& shaderLib )
	{
		if( config.bEmitMainHeader ) {
			Emit_Main_Header_File( config, shaderLib );
		}

		Emit_Shader_Programs_Declarations_Header_File( config, shaderLib );

		if( config.bEmitVertexFormatsDecl ) {
			Emit_Vertex_Formats_Declarations_Header_File( config, shaderLib );
		}

		Emit_Main_Source_File( config, shaderLib );
	}
	//===========================================================================

	void Emit_Preamble( const Options& config, MyTextWriter & tw )
	{
		if( config.bEmitPreamble )
		{
			tw	//<< MXC("/* This ALWAYS GENERATED file contains the definitions for the interfaces */\n")
				<< MXC("/* Do NOT modify this file! It has been autogenerated by the tools.*/\n")
				<< MXC("/* File created by HLSL wrapper generator version ") << WR_VERSION_STRING << MXC(" ") << GetCurrentDateTimeString() << MXC(" */\n")
				<< MXC("/* Copyright © 2012 ShaderGen.*/\n")
				<< MXC("\n")
				;
		}
	}

	//===========================================================================

	void Emit_End_Of_File( MyTextWriter & tw )
	{
		tw.NewLine();
		tw << MXC("//--------------------------------------------------------------//\n");
		tw << MXC("//				End Of File.									//\n");
		tw << MXC("//--------------------------------------------------------------//\n\n");
	}

	//===========================================================================

	void Emit_Pragma_Once( MyTextWriter & tw )
	{
		tw << MXC("#pragma once\n");
		tw.NewLine();
	}

	//===========================================================================
	void Emit_Vertex_Formats_Declarations_Header_File( const Options& config, const wrShaderLibrary& shaderLib )
	{
		OSPathName		fullPath(config.outputFolderCPP);
		fullPath.Append( MXC("VertexFormats.hxx") );

		FileWriter		outputStream( fullPath.ToChars() );
		MyTextWriter	tw( outputStream );

		Emit_Preamble( config, tw );
		Emit_Pragma_Once( tw );

		tw	<< MXC("// Vertex formats\n");
		tw.NewLine();

		if( config.bEmitComments ) {
			tw.Putf("// %u input layouts\n\n", (UINT)shaderLib.vertexDeclarations.Num());
		}

		tw	<< MXC("class VertexData;\n");
		tw.NewLine();

		tw.Putf("namespace %s\n",config.cppNamespace.ToChars());
		{
			wrScope	sc(tw);

			String	scopeName;
			scopeName.Format(" // namespace %s", config.cppNamespace.ToChars());
			sc.SetName(scopeName.ToChars());

			Emit_Vertex_Declarations( shaderLib.vertexDeclarations, tw );
		}
		Emit_End_Of_File( tw );
	}

	//===========================================================================
	void Emit_Shader_Programs_Declarations_Header_File( const Options& config, const wrShaderLibrary& shaderLib )
	{
		OSPathName		fullPath(config.outputFolderCPP);
		fullPath.Append( MXC("ShaderPrograms.hxx") );

		FileWriter		outputStream( fullPath.ToChars() );
		MyTextWriter	tw( outputStream );

		Emit_Preamble( config, tw );
		Emit_Pragma_Once( tw );

		tw	<< MXC("// Shaders\n\n");

		if( config.bEmitComments ) {
			tw.Putf("// %u shader programs (%u unique combinations)\n\n",
				(UINT)shaderLib.shaders.Num(), (UINT)shaderLib.CalcNumUniqueShaderProgramCombinations()
				);
		}

		SHADER_GEN_HADRCODED
		tw	<< MXC("#include <Renderer/GPU/HLSL/BuildConfig.h>\n");
		tw.NewLine();

		tw.Putf("namespace %s\n",config.cppNamespace.ToChars());
		{
			wrScope	sc(tw);

			String	scopeName;
			scopeName.Format(" // namespace %s", config.cppNamespace.ToChars());
			sc.SetName(scopeName.ToChars());

			Emit_Shader_Declarations( shaderLib.shaders, tw );
		}
		Emit_End_Of_File( tw );
	}
	//===========================================================================
	void Emit_Main_Header_File( const Options& config, const wrShaderLibrary& shaderLib )
	{
		OSPathName		fullPath(config.outputFolderCPP);
		fullPath.Append(config.headerFileName.ToChars());

		FileWriter		outputStream( fullPath.ToChars() );
		MyTextWriter	tw( outputStream );

		Emit_Preamble( config, tw );
		Emit_Pragma_Once( tw );

		tw	<< MXC("// Declarations for interfacing with the shader library\n\n");


		if( config.bEmitComments )
		{
			const UINT numShaderProgramCombinations = shaderLib.CalcNumUniqueShaderProgramCombinations();

			tw.Putf(
				"// %u render targets\n"
				"// %u sampler states\n"
				"// %u depth stencil states\n"
				"// %u rasterizer states\n"
				"// %u blend states\n"
				"// %u shader programs (%u unique combinations)\n"
				"// %u input layouts\n"
				"\n"
				,
				(UINT)shaderLib.renderTargets.Num(),
				(UINT)shaderLib.samplerStates.Num(),
				(UINT)shaderLib.depthStencilStates.Num(),
				(UINT)shaderLib.rasterizerStates.Num(),
				(UINT)shaderLib.blendStates.Num(),
				(UINT)shaderLib.shaders.Num(), numShaderProgramCombinations,
				(UINT)shaderLib.vertexDeclarations.Num()
				);
		}


		if(!config.headerFilePrefix.IsEmpty() && config.headerFilePrefix[0] != 0)
		{
			//tw.Put(config.headerFilePrefix.ToChars(),config.headerFilePrefix.Num());
			tw.Emit(config.headerFilePrefix.ToChars());
		}


		tw.Putf("namespace %s\n",config.cppNamespace.ToChars());
		{
			wrScope	sc(tw);

			String	scopeName;
			scopeName.Format(" // namespace %s", config.cppNamespace.ToChars());
			sc.SetName(scopeName.ToChars());

			tw	<< MXC("// Shader library info\n");
			tw.Putf("enum { TotalNumberOfShaders = %u };\n",shaderLib.shaders.Num());

			tw.NewLine();

			tw	<< MXC("UINT ShaderNameToIndex( const char* str );\n");
			tw	<< MXC("const char* ShaderIndexToName( UINT idx );\n");

			tw.NewLine();

			tw	<< MXC("extern const FileTime TIME_STAMP;\n");

			tw.NewLine();

			tw	<< MXC("/*\n");
			tw	<< MXC("=============================================================================\n");
			tw	<< MXC("	Function declarations\n");
			tw	<< MXC("=============================================================================\n");
			tw	<< MXC("*/\n");
			tw.NewLine();

			tw.Putf("void Initialize( const GrShaderLibraryContext& context );\n");
			tw.NewLine();
			tw.Putf("void Shutdown();\n");
			tw.NewLine();
			tw	<< MXC("// releases render targets that depend on backbuffer size; should be called before resizing viewports\n");
			tw.Putf("void ReleaseRenderTargets();\n");
			tw.NewLine();
			tw	<< MXC("// recreates render targets that depend on backbuffer size; should be called after resizing viewports\n");
			tw.Putf("void ReallocRenderTargets( UINT sizeX, UINT sizeY );\n");

			tw.NewLine();
			tw	<< MXC("// Reflection/Introspection\n");
			tw.NewLine();
			tw.Putf("void EnumerateRenderTargets     ( F_EnumerateRenderTargets* callback, void* userData );\n");
			tw.Putf("void EnumerateSamplerStates     ( F_EnumerateSamplerStates* callback, void* userData );\n");
			tw.Putf("void EnumerateDepthStencilStates( F_EnumerateDepthStencilStates* callback, void* userData );\n");
			tw.Putf("void EnumerateRasterizerStates  ( F_EnumerateRasterizerStates* callback, void* userData );\n");
			tw.Putf("void EnumerateBlendStates       ( F_EnumerateBlendStates* callback, void* userData );\n");


			tw.NewLine();


			Emit_RenderTarget_Declarations( shaderLib.renderTargets, tw );

			Emit_MultiRenderTarget_Declarations( shaderLib.multiRenderTargets, tw );

			Emit_SamplerState_Declarations( shaderLib.samplerStates, tw );

			Emit_DepthStencilStates_Declarations( shaderLib.depthStencilStates, tw );

			Emit_RasterizerStates_Declarations( shaderLib.rasterizerStates, tw );

			Emit_BlendStates_Declarations( shaderLib.blendStates, tw );

			Emit_StateBlocks_Declarations( shaderLib.stateBlocks, tw );

			Emit_Shared_Sections_Declarations( shaderLib.sharedSections, tw );
		}
		Emit_End_Of_File( tw );
	}
	//===========================================================================
	void Emit_Main_Source_File( const Options& config, const wrShaderLibrary& shaderLib )
	{
		OSPathName		fullPath(config.outputFolderCPP);
		fullPath.Append(config.sourceFileName.ToChars());

		FileWriter		outputStream( fullPath.ToChars() );
		MyTextWriter	tw( outputStream );


		Emit_Preamble( config, tw );

		tw << MXC("/* This ALWAYS GENERATED file contains the definitions for the interfaces */\n\n");

		if(!config.sourceFilePrefix.IsEmpty() && config.sourceFilePrefix[0] != 0)
		{
			//tw.Put(config.sourceFilePrefix.ToChars(),config.sourceFilePrefix.Num());
			tw.Emit(config.sourceFilePrefix.ToChars());
		}

		

		// include header files

		tw.NewLine();
		tw.Putf( "#include \"%s\"\n", config.headerFileName.ToChars() );
		tw << MXC("#include \"ShaderPrograms.hxx\"\n");
		tw << MXC("#include \"VertexFormats.hxx\"\n");
		tw.NewLine();

		





		// open namespace

		tw.Putf("namespace %s\n",config.cppNamespace.ToChars());
		wrScope	sc(tw);


		const FileTime currTime = FileTime::CurrentTime();

		char	szTimeStamp[128];	// timeLastModified
		FS_Win32_FileTimeStampToDateTimeString( currTime.time, szTimeStamp, ARRAY_SIZE(szTimeStamp));
		tw.Putf( "// %s\n", szTimeStamp );

		tw.Putf( "const FileTime TIME_STAMP( %u, %u );\n", currTime.time.dwLowDateTime, currTime.time.dwHighDateTime );

		tw.NewLine();


		// string table (maps shader indices to their names)
		{
			tw << "// sorted in ascending order\n";
			tw << "const char* g_shaderNames[ TotalNumberOfShaders ] =\n";
			wrScope	sc22(tw,Scope_Semicolon);

			for( UINT iShader = 0;
				iShader < shaderLib.shaders.Num();
				iShader++ )
			{
				const wrShaderProgram& shader = shaderLib.shaders[ iShader ];
				tw.Putf( "\"%s\",\n", Get_ShaderClass_Name(shader) );
			}
		}

		tw.NewLine();

		tw << "UINT ShaderNameToIndex( const char* str ) {\n";
		tw << "	return BinaryStringSearch( g_shaderNames, ARRAY_SIZE(g_shaderNames), str );\n";
		tw << "}\n";

		tw.NewLine();

		tw << "const char* ShaderIndexToName( UINT idx ) {\n";
		tw << "	Assert( idx < ARRAY_SIZE(g_shaderNames) );\n";
		tw << "	return g_shaderNames[ idx ];\n";
		tw << "}\n";

		tw.NewLine();






		Emit_Create_RenderTargets( shaderLib.renderTargets, tw );
		Emit_Create_SamplerStates( shaderLib.samplerStates, tw );
		Emit_Create_DepthStencilStates( shaderLib.depthStencilStates, tw );
		Emit_Create_RasterizerStates( shaderLib.rasterizerStates, tw );
		Emit_Create_BlendStates( shaderLib.blendStates, tw );
		Emit_Create_StateBlocks( shaderLib.stateBlocks, tw );

		Emit_Shared_Sections_Definitions( shaderLib.sharedSections, tw );
		Emit_Shader_Definitions( shaderLib.shaders, tw );

		Emit_Create_InputLayouts( shaderLib.vertexDeclarations, tw );

		Emit_Vertex_Struct_Definitions(shaderLib.vertexDeclarations,tw);




		tw.NewLine();
		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Creation / Destruction\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();


		Emit_Main_Initialize(config,shaderLib,tw);


		tw.NewLine();

		//Shutdown
		tw.Putf("void Shutdown()\n");
		{
			wrScope	sc(tw);

			Emit_Release_Resources( shaderLib, tw );
		}

	}
	//=============================================================================
	void Emit_Main_Initialize(const Options& config, const wrShaderLibrary& shaderLib, MyTextWriter & tw)
	{
		//Initialize
		tw.Putf("void Initialize( const GrShaderLibraryContext& context )\n");
		wrScope	sc(tw);

		// Initialization order:
		// 1) Render targets
		// 2) State objects
		// 3) Shaders
		// 4) Input layouts
		// 5) Everything else

		if( shaderLib.renderTargets.Num() ) {
			tw	<< MXC("// create render targets first so that they're placed in the fastest portion of VRAM\n");
			tw	<< MXC("SetupRenderTargets( context.backBufferWidth, context.backBufferHeight );\n");
		}
		if( shaderLib.samplerStates.Num() ) {
			tw	<< MXC("SetupSamplerStates();\n");
		}
		if( shaderLib.depthStencilStates.Num() ) {
			tw	<< MXC("SetupDepthStencilStates();\n");
		}
		if( shaderLib.rasterizerStates.Num() ) {
			tw	<< MXC("SetupRasterizerStates();\n");
		}
		if( shaderLib.blendStates.Num() ) {
			tw	<< MXC("SetupBlendStates();\n");
		}
		if( shaderLib.stateBlocks.Num() ) {
			tw	<< MXC("SetupStateBlocks();\n");
		}
		if( shaderLib.vertexDeclarations.Num() ) {
			tw	<< MXC("CreateInputLayouts();\n");
		}
		if( shaderLib.sharedSections.Num() ) {
			tw	<< MXC("SetupSharedSections();\n");
		}

		tw.NewLine();
		tw	<< MXC("// create shader programs\n");

		for( UINT iShader = 0;
			iShader < shaderLib.shaders.Num();
			iShader++ )
		{
			const wrShaderProgram& shader = shaderLib.shaders[ iShader ];

			tw.Putf( "%s::Initialize();\n", Get_ShaderClass_Name(shader) );
		}
	}
	/*
	=============================================================================
	*/
	void Emit_RenderTarget_Declarations( const TList< wrRenderTarget >& renderTargets, MyTextWriter & tw )
	{
		const UINT numRenderTargets = renderTargets.Num();

		if( !numRenderTargets ) {
			return;
		}

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Render targets\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		for( UINT iRenderTarget = 0;
			iRenderTarget < numRenderTargets;
			iRenderTarget++ )
		{
			const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

			Emit_Comment_Source_Location( renderTarget.pos, tw );

			tw.Putf("extern RenderTarget %s;\n",Get_RenderTarget_Name(renderTarget));
		}

		tw.NewLine();
	}
	//=============================================================================
	void Emit_MultiRenderTarget_Declarations( const TList< wrMultiRenderTarget >& multiRenderTargets, MyTextWriter & tw )
	{
		const UINT numRenderTargets = multiRenderTargets.Num();

		if( !numRenderTargets ) {
			return;
		}

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Multiple render targets\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		for( UINT iRenderTarget = 0;
			iRenderTarget < numRenderTargets;
			iRenderTarget++ )
		{
			const wrMultiRenderTarget& renderTarget = multiRenderTargets[ iRenderTarget ];

			Emit_Comment_Source_Location( renderTarget.pos, tw );

			Emit_MultipleRenderTarget_Struct(renderTarget,tw);
		}

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

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Sampler states\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		StackString	buffer;

		for( UINT iSamplerState = 0;
			iSamplerState < numSamplerStates;
			iSamplerState++ )
		{
			const wrSamplerState& samplerState = samplerStates[ iSamplerState ];

			Emit_Comment_Source_Location( samplerState.pos, tw );
			buffer.Format( "extern ID3D11SamplerStatePtr %s%s;\n",
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

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Depth-stencil states\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		StackString	buffer;

		for( UINT iDepthStencilState = 0;
			iDepthStencilState < numDepthStencilStates;
			iDepthStencilState++ )
		{
			const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];
			Emit_Comment_Source_Location( depthStencilState.pos, tw );
			buffer.Format( "extern ID3D11DepthStencilStatePtr %s%s;\n",
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
	
		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Rasterizer states\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		StackString	buffer;

		for( UINT iRasterizerState = 0;
			iRasterizerState < numRasterizerStates;
			iRasterizerState++ )
		{
			const wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];
			Emit_Comment_Source_Location( rasterizerState.pos, tw );
			buffer.Format( "extern ID3D11RasterizerStatePtr %s%s;\n",
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

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	Blend states\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");


		StackString	buffer;

		for( UINT iBlendState = 0;
			iBlendState < numBlendStates;
			iBlendState++ )
		{
			const wrBlendState& blendState = blendStates[ iBlendState ];
			Emit_Comment_Source_Location( blendState.pos, tw );
			buffer.Format( "extern ID3D11BlendStatePtr %s%s;\n",
				WR_BlendState_PREFIX, blendState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_StateBlocks_Declarations( const TList< wrStateBlock >& stateBlocks, MyTextWriter & tw )
	{
		const UINT numStateBlocks = stateBlocks.Num();

		if( !numStateBlocks ) {
			return;
		}

		tw	<< MXC("/*\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("	State blocks\n");
		tw	<< MXC("=============================================================================\n");
		tw	<< MXC("*/\n\n");

		StackString	buffer;

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
	void Emit_Create_RenderTargets( const TList< wrRenderTarget >& renderTargets, MyTextWriter & tw )
	{
		const UINT numRenderTargets = renderTargets.Num();

		if( !numRenderTargets ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Render targets\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		for( UINT iRenderTarget = 0;
			iRenderTarget < numRenderTargets;
			iRenderTarget++ )
		{
			const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

			Emit_Comment_Source_Location( renderTarget.pos, tw );
			tw.Putf( "RenderTarget %s;\n",
				Get_RenderTarget_Name(renderTarget));
		}

		tw.NewLine();


		tw	<< MXC("static void SetupRenderTargets( const UINT sizeX, const UINT sizeY )\n");
		{
			wrScope	funcScope(tw);

			tw	<< MXC("D3D11_TEXTURE2D_DESC texDesc;\n")
				<< MXC("D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;\n")
				<< MXC("D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;\n")
				//<< MXC("ZERO_OUT( texDesc );\n")
				//<< MXC("ZERO_OUT( rtvDesc );\n")
				//<< MXC("ZERO_OUT( srvDesc );\n")
				;
			tw.NewLine();

			for( UINT iRenderTarget = 0;
				iRenderTarget < numRenderTargets;
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];


				Emit_Create_RenderTarget( renderTarget, tw );
			}
		}


		tw.NewLine();
		tw	<< MXC("// releases render targets that depend on backbuffer size; should be called before resizing viewports\n");
		tw	<< MXC("void ReleaseRenderTargets()\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRenderTarget = 0;
				iRenderTarget < numRenderTargets;
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];
				if( renderTarget.DependsOnBackbufferSize() )
				{
					tw	.Putf("%s%s.Release();\n",
						WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
				}
			}
		}

		tw.NewLine();
		tw	<< MXC("// recreates render targets that depend on backbuffer size; should be called after resizing viewports\n");
		tw	<< MXC("void ReallocRenderTargets( UINT sizeX, UINT sizeY )\n");
		{
			wrScope	funcScope(tw);

			tw	<< MXC("D3D11_TEXTURE2D_DESC texDesc;\n")
				<< MXC("D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;\n")
				<< MXC("D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;\n")
				//<< MXC("ZERO_OUT( texDesc );\n")
				//<< MXC("ZERO_OUT( rtvDesc );\n")
				//<< MXC("ZERO_OUT( srvDesc );\n")
				;

			for( UINT iRenderTarget = 0;
				iRenderTarget < numRenderTargets;
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

				if( renderTarget.DependsOnBackbufferSize() )
				{
					tw.NewLine();
					tw.Putf("Assert( %s%s.IsNull() );\n",
						WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );

					Emit_Create_RenderTarget( renderTarget, tw );
				}
			}
		}

		tw.NewLine();
		tw	<< MXC("void EnumerateRenderTargets( F_EnumerateRenderTargets* callback, void* userData )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRenderTarget = 0;
				iRenderTarget < numRenderTargets;
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];

				tw.Putf("(*callback)( &%s%s, userData );\n",
						WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
			}
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_Create_RenderTarget( const wrRenderTarget& renderTarget, MyTextWriter & tw )
	{
		Emit_Comment_Source_Element_Info( renderTarget, tw );
		//tw.NewLine();
		tw.Putf("Assert( %s%s.IsNull() );\n",
			WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );

		wrScope	sc(tw);

		wrBuffer	renderTargetFormat = DXGI_Format_From_Script_Format( renderTarget.format );

		tw	.Putf( "texDesc.Format				= %s;\n", renderTargetFormat.ToChars() );
		tw	.Putf( "texDesc.Width				= %s;\n", Get_RT_Size( renderTarget.sizeX, MXC("sizeX") ).ToChars() );
		tw	.Putf( "texDesc.Height				= %s;\n", Get_RT_Size( renderTarget.sizeY, MXC("sizeY") ).ToChars() );
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

		tw	.Putf("graphics.resources->Create_RenderTarget( %s%s, texDesc, rtvDesc, srvDesc );\n",
			WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );

		//tw.Emit( "#if MX_EDITOR\n" );
		//tw.Putf( "%s%s.debugName = \"%s\";\n", WR_RenderTarget_PREFIX, renderTarget.name.ToChars(), renderTarget.name.ToChars() );
		//tw.Emit( "#endif // MX_EDITOR\n" );

		tw.NewLine();
		Emit_DbgSetName(renderTarget,tw);
	}

	//===========================================================================
	void Emit_DbgSetName( const char* prefix, const wrSourceElement& element, MyTextWriter & tw )
	{
		tw	.Putf("dxDbgSetName( %s%s, \"%s\" );\n",
			prefix, element.name.ToChars(), element.name.ToChars() );
	}
	//===========================================================================
	void Emit_DbgSetName( const wrRenderTarget& element, MyTextWriter & tw )
	{
		tw.Putf("dxDbgSetName( %s%s.pTexture, \"%s_T2D\" );\n",
			WR_RenderTarget_PREFIX, element.name.ToChars(), element.name.ToChars() );

		tw.Putf("dxDbgSetName( %s%s.pRTV, \"%s_RTV\" );\n",
			WR_RenderTarget_PREFIX, element.name.ToChars(), element.name.ToChars() );

		tw.Putf("dxDbgSetName( %s%s.pSRV, \"%s_SRV\" );\n",
			WR_RenderTarget_PREFIX, element.name.ToChars(), element.name.ToChars() );
	}
	//===========================================================================
	void Emit_DbgSetName( const wrVertexDeclaration& element, MyTextWriter & tw )
	{
		tw	.Putf("dxDbgSetName( %s::layout, \"%s\" );\n",
			element.name.ToChars(), element.name.ToChars() );
	}
	//===========================================================================
	void Emit_Create_SamplerStates( const TList< wrSamplerState >& samplerStates, MyTextWriter & tw )
	{
		const UINT numSamplerStates = samplerStates.Num();

		if( !numSamplerStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Sampler states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();


		for( UINT iSamplerState = 0;
			iSamplerState < numSamplerStates;
			iSamplerState++ )
		{
			const wrSamplerState& samplerState = samplerStates[ iSamplerState ];

			Emit_Comment_Source_Location( samplerState.pos, tw );

			tw.Putf( "ID3D11SamplerStatePtr %s;\n",
				Get_SamplerState_Name(samplerState) );
		}

		tw.NewLine();


		tw	<< MXC("static void SetupSamplerStates()\n");
		{
			wrScope	sc(tw);

			tw	<< MXC("D3D11_SAMPLER_DESC samplerDesc;\n")
				//<< MXC("ZERO_OUT( samplerDesc );\n")
				;

			for( UINT iSamplerState = 0;
				iSamplerState < numSamplerStates;
				iSamplerState++ )
			{
				const wrSamplerState& samplerState = samplerStates[ iSamplerState ];

				Emit_Create_SamplerState( samplerState, tw );
			}
		}



		tw.NewLine();
		tw	<< MXC("void EnumerateSamplerStates( F_EnumerateSamplerStates* callback, void* userData )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iSamplerState = 0;
				iSamplerState < numSamplerStates;
				iSamplerState++ )
			{
				const wrSamplerState& samplerState = samplerStates[ iSamplerState ];

				tw.Putf("(*callback)( %s%s, userData );\n",
					WR_SamplerState_PREFIX, samplerState.name.ToChars() );
			}
		}


		tw.NewLine();
	}
	//===========================================================================
	void Emit_Create_SamplerState( const wrSamplerState& samplerState, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( samplerState, tw );
		wrScope	sc(tw);

		tw	.Putf("samplerDesc.Filter	= %s;\n", GetTextureFilter( samplerState.Filter ).ToChars() );
		tw	.Putf("samplerDesc.AddressU	= %s;\n", GetTextureAddressMode( samplerState.AddressU ).ToChars() );
		tw	.Putf("samplerDesc.AddressV	= %s;\n", GetTextureAddressMode( samplerState.AddressV ).ToChars() );
		tw	.Putf("samplerDesc.AddressW	= %s;\n", GetTextureAddressMode( samplerState.AddressW ).ToChars() );

		StackString	ComparisonFunc(_InitSlow,samplerState.ComparisonFunc.ToChars());
		ComparisonFunc.ToUpper();

		tw	.Putf("samplerDesc.MipLODBias		= %ff;\n",samplerState.MipLODBias);
		tw	.Putf("samplerDesc.MaxAnisotropy	= %u;\n",samplerState.MaxAnisotropy);
		tw	.Putf("samplerDesc.ComparisonFunc	= D3D11_COMPARISON_%s;\n",ComparisonFunc.ToChars());
		tw	.Putf("samplerDesc.BorderColor[0]	= %ff;\n",samplerState.BorderColor.R);
		tw	.Putf("samplerDesc.BorderColor[1]	= %ff;\n",samplerState.BorderColor.G);
		tw	.Putf("samplerDesc.BorderColor[2]	= %ff;\n",samplerState.BorderColor.B);
		tw	.Putf("samplerDesc.BorderColor[3]	= %ff;\n",samplerState.BorderColor.A);
		//tw	.Putf("samplerDesc.MinLOD			= %ff;\n",samplerState.MinLOD);
		//tw	.Putf("samplerDesc.MaxLOD			= %ff;\n",samplerState.MaxLOD);
		tw	.Putf("samplerDesc.MinLOD			= 0.0f;\n");
		tw	.Putf("samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;\n");

#if 0
		tw	.Putf("samplerDesc.MipLODBias		= %f;\n",samplerState.MipLODBias);
		tw	.Putf("samplerDesc.MaxAnisotropy	= %u;\n",samplerState.MaxAnisotropy);
		tw	.Putf("samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;\n");
		tw	.Putf("samplerDesc.BorderColor[0]	= 1.0f;\n");
		tw	.Putf("samplerDesc.BorderColor[1]	= 1.0f;\n");
		tw	.Putf("samplerDesc.BorderColor[2]	= 1.0f;\n");
		tw	.Putf("samplerDesc.BorderColor[3]	= 1.0f;\n");
		tw	.Putf("samplerDesc.MinLOD			= -FLT_MAX;\n");
		tw	.Putf("samplerDesc.MaxLOD			= +FLT_MAX;\n");
#endif
		;

		tw.NewLine();

		tw	.Putf("graphics.resources->Create_SamplerState( %s%s, samplerDesc );\n",
			WR_SamplerState_PREFIX, samplerState.name.ToChars() );

		Emit_DbgSetName(WR_SamplerState_PREFIX, samplerState,tw);
	}
	//===========================================================================
	void Emit_Create_DepthStencilStates( const TList< wrDepthStencilState >& depthStencilStates, MyTextWriter & tw )
	{
		const UINT numDepthStencilStates = depthStencilStates.Num();

		if( !numDepthStencilStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Depth-Stencil states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		StackString	buffer;

		for( UINT iDepthStencilState = 0;
			iDepthStencilState < numDepthStencilStates;
			iDepthStencilState++ )
		{
			const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];

			Emit_Comment_Source_Location( depthStencilState.pos, tw );
			buffer.Format( "ID3D11DepthStencilStatePtr %s%s;\n",
				WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("static void SetupDepthStencilStates()\n");
		{
			wrScope	sc(tw);

			tw	<< MXC("D3D11_DEPTH_STENCIL_DESC depthStencilDesc;\n")
				//<< MXC("ZERO_OUT( depthStencilDesc );\n")
				;

			for( UINT iDepthStencilState = 0;
				iDepthStencilState < numDepthStencilStates;
				iDepthStencilState++ )
			{
				const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];

				Emit_Create_DepthStencilState( depthStencilState, tw );
			}
		}


		tw.NewLine();
		tw	<< MXC("void EnumerateDepthStencilStates( F_EnumerateDepthStencilStates* callback, void* userData )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iDepthStencilState = 0;
				iDepthStencilState < numDepthStencilStates;
				iDepthStencilState++ )
			{
				const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];

				tw.Putf("(*callback)( %s%s, userData );\n",
					WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );
			}
		}


		tw.NewLine();
	}
	//===========================================================================
	void Emit_Create_DepthStencilState( const wrDepthStencilState& depthStencilState, MyTextWriter & tw )
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

		tw	.Putf("graphics.resources->Create_DepthStencilState( %s%s, depthStencilDesc );\n",
			WR_DepthStencilState_PREFIX, depthStencilState.name.ToChars() );

		Emit_DbgSetName(WR_DepthStencilState_PREFIX, depthStencilState,tw);
	}
	//===========================================================================
	void Emit_Create_RasterizerStates( const TList< wrRasterizerState >& rasterizerStates, MyTextWriter & tw )
	{
		const UINT numRasterizerStates = rasterizerStates.Num();

		if( !numRasterizerStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Rasterizer states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		StackString	buffer;

		for( UINT iRasterizerState = 0;
			iRasterizerState < numRasterizerStates;
			iRasterizerState++ )
		{
			const wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];

			Emit_Comment_Source_Location( rasterizerState.pos, tw );
			buffer.Format( "ID3D11RasterizerStatePtr %s%s;\n",
				WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("static void SetupRasterizerStates()\n");
		{
			wrScope	sc(tw);

			tw	<< MXC("D3D11_RASTERIZER_DESC rasterizerDesc;\n")
				//<< MXC("ZERO_OUT( rasterizerDesc );\n")
				;

			for( UINT iRasterizerState = 0;
				iRasterizerState < numRasterizerStates;
				iRasterizerState++ )
			{
				const wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];

				Emit_Create_RasterizerState( rasterizerState, tw );
			}
		}


		tw.NewLine();
		tw	<< MXC("void EnumerateRasterizerStates( F_EnumerateRasterizerStates* callback, void* userData )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRasterizerState = 0;
				iRasterizerState < numRasterizerStates;
				iRasterizerState++ )
			{
				const wrRasterizerState& rasterizerState = rasterizerStates[ iRasterizerState ];

				tw.Putf("(*callback)( %s%s, userData );\n",
					WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );
			}
		}


		tw.NewLine();

	}
	//===========================================================================
	void Emit_Create_RasterizerState( const wrRasterizerState& rasterizerState, MyTextWriter & tw )
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

		tw	.Putf("graphics.resources->Create_RasterizerState( %s%s, rasterizerDesc );\n",
			WR_RasterizerState_PREFIX, rasterizerState.name.ToChars() );

		Emit_DbgSetName(WR_RasterizerState_PREFIX, rasterizerState,tw);
	}
	//===========================================================================
	void Emit_Create_BlendStates( const TList< wrBlendState >& blendStates, MyTextWriter & tw )
	{
		const UINT numBlendStates = blendStates.Num();

		if( !numBlendStates ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	Blend states\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		StackString	buffer;

		for( UINT iBlendState = 0;
			iBlendState < numBlendStates;
			iBlendState++ )
		{
			const wrBlendState& blendState = blendStates[ iBlendState ];

			Emit_Comment_Source_Location( blendState.pos, tw );
			buffer.Format( "ID3D11BlendStatePtr %s%s;\n",
				WR_BlendState_PREFIX, blendState.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("static void SetupBlendStates()\n");
		{
			wrScope	sc(tw);

			tw	<< MXC("D3D11_BLEND_DESC blendDesc;\n")
				<< MXC("ZERO_OUT( blendDesc );\n")
				;

			for( UINT iBlendState = 0;
				iBlendState < numBlendStates;
				iBlendState++ )
			{
				const wrBlendState& blendState = blendStates[ iBlendState ];

				Emit_Create_BlendState( blendState, tw );
			}
		}


		tw.NewLine();
		tw	<< MXC("void EnumerateBlendStates( F_EnumerateBlendStates* callback, void* userData )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iBlendState = 0;
				iBlendState < numBlendStates;
				iBlendState++ )
			{
				const wrBlendState& blendState = blendStates[ iBlendState ];

				tw.Putf("(*callback)( %s%s, userData );\n",
					WR_BlendState_PREFIX, blendState.name.ToChars() );
			}
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_Create_BlendState( const wrBlendState& blendState, MyTextWriter & tw )
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

		tw	.Putf("graphics.resources->Create_BlendState( %s%s, blendDesc );\n",
			WR_BlendState_PREFIX, blendState.name.ToChars() );

		Emit_DbgSetName(WR_BlendState_PREFIX, blendState,tw);
	}
	//===========================================================================
	void Emit_Create_StateBlocks( const TList< wrStateBlock >& stateBlocks, MyTextWriter & tw )
	{
		const UINT numStateBlocks = stateBlocks.Num();

		if( !numStateBlocks ) {
			return;
		}

		tw << MXC("//===========================================================================\n");
		tw << MXC("//	State blocks\n");
		tw << MXC("//===========================================================================\n");
		tw.NewLine();

		StackString	buffer;

		for( UINT iStateBlock = 0;
			iStateBlock < numStateBlocks;
			iStateBlock++ )
		{
			const wrStateBlock& stateBlock = stateBlocks[ iStateBlock ];

			Emit_Comment_Source_Location( stateBlock.pos, tw );
			buffer.Format( "StateBlock %s%s;\n",
				WR_StateBlock_PREFIX, stateBlock.name.ToChars() );
			tw	<< buffer;
		}

		tw.NewLine();


		tw	<< MXC("static void SetupStateBlocks()\n");
		wrScope	sc(tw);

		for( UINT iStateBlock = 0;
			iStateBlock < numStateBlocks;
			iStateBlock++ )
		{
			const wrStateBlock& stateBlock = stateBlocks[ iStateBlock ];

			Emit_Initialize_StateBlock( stateBlock, tw );
		}
	}
	//===========================================================================
	void Emit_Initialize_StateBlock( const wrStateBlock& stateBlock, MyTextWriter & tw )
	{
		tw.NewLine();
		Emit_Comment_Source_Element_Info( stateBlock, tw );
		wrScope	sc(tw);

		StackString	stateBlockName(WR_StateBlock_PREFIX);
		stateBlockName += stateBlock.name;

		StackString	rasterizerStateName(WR_RasterizerState_PREFIX);
		rasterizerStateName += stateBlock.rasterizerState;
		
		StackString	depthStencilStateName(WR_DepthStencilState_PREFIX);
		depthStencilStateName += stateBlock.depthStencilState;
		
		StackString	blendStateName(WR_BlendState_PREFIX);
		blendStateName += stateBlock.blendState;

		tw	.Putf("%s.rasterizer   = %s;\n",		stateBlockName.ToChars(), rasterizerStateName.ToChars() );
		tw.NewLine();
		tw	.Putf("%s.depthStencil = %s;\n",		stateBlockName.ToChars(), depthStencilStateName.ToChars() );
		tw	.Putf("%s.stencilRef   = %s;\n",		stateBlockName.ToChars(), stateBlock.stencilRef.ToChars() );
		tw.NewLine();
		tw	.Putf("%s.blend           = %s;\n",	stateBlockName.ToChars(), blendStateName.ToChars() );
		
		
		//tw	.Putf("%s.blendFactorRGBA = %s;\n",		stateBlockName.ToChars(), get_rgba_color_string( stateBlock.blendFactorRGBA ).ToChars() );
		tw	.Putf("%s.blendFactorRGBA[0] = %f;\n",		stateBlockName.ToChars(), stateBlock.blendFactorRGBA.R );
		tw	.Putf("%s.blendFactorRGBA[1] = %f;\n",		stateBlockName.ToChars(), stateBlock.blendFactorRGBA.G );
		tw	.Putf("%s.blendFactorRGBA[2] = %f;\n",		stateBlockName.ToChars(), stateBlock.blendFactorRGBA.B );
		tw	.Putf("%s.blendFactorRGBA[3] = %f;\n",		stateBlockName.ToChars(), stateBlock.blendFactorRGBA.A );
		


		tw	.Putf("%s.sampleMask      = %s;\n",		stateBlockName.ToChars(), stateBlock.sampleMask.ToChars() );
	}
	//===========================================================================
	void Emit_Release_Resources( const wrShaderLibrary& shaderLib, MyTextWriter & tw )
	{
		if( shaderLib.shaders.Num() )
		{
			for( UINT iShader = 0;
				iShader < shaderLib.shaders.Num();
				iShader++ )
			{
				const wrShaderProgram& shader = shaderLib.shaders[ iShader ];

				tw.Putf( "%s::Shutdown();\n", Get_ShaderClass_Name(shader) );
			}
		}

		if( shaderLib.samplerStates.Num() )
		{
			tw.NewLine();
			for( UINT iSamplerState = 0;
				iSamplerState < shaderLib.samplerStates.Num();
				iSamplerState++ )
			{
				const wrSamplerState& samplerState = shaderLib.samplerStates[ iSamplerState ];
				tw.Putf("graphics.resources->Destroy_SamplerState( %s );\n", Get_SamplerState_Name(samplerState) );
			}
			tw.NewLine();
		}

		if( shaderLib.depthStencilStates.Num() )
		{
			for( UINT iDepthStencilState = 0;
				iDepthStencilState < shaderLib.depthStencilStates.Num();
				iDepthStencilState++ )
			{
				const wrDepthStencilState& depthStencilState = shaderLib.depthStencilStates[ iDepthStencilState ];
				tw.Putf("graphics.resources->Destroy_DepthStencilState( %s );\n", Get_DepthStencilState_Name(depthStencilState) );
			}
			tw.NewLine();
		}
		
		if( shaderLib.rasterizerStates.Num() )
		{
			for( UINT iRasterizerState = 0;
				iRasterizerState < shaderLib.rasterizerStates.Num();
				iRasterizerState++ )
			{
				const wrRasterizerState& rasterizerState = shaderLib.rasterizerStates[ iRasterizerState ];
				tw.Putf("graphics.resources->Destroy_RasterizerState( %s );\n", Get_RasterizerState_Name(rasterizerState) );
			}
			tw.NewLine();
		}
		
		if( shaderLib.blendStates.Num() )
		{
			for( UINT iBlendState = 0;
				iBlendState < shaderLib.blendStates.Num();
				iBlendState++ )
			{
				const wrBlendState& blendState = shaderLib.blendStates[ iBlendState ];
				tw.Putf("graphics.resources->Destroy_BlendState( %s );\n", Get_BlendState_Name(blendState) );
			}
			tw.NewLine();
		}
		
		if( shaderLib.renderTargets.Num() )
		{
			for( UINT iRenderTarget = 0;
				iRenderTarget < shaderLib.renderTargets.Num();
				iRenderTarget++ )
			{
				const wrRenderTarget& renderTarget = shaderLib.renderTargets[ iRenderTarget ];
				tw.Putf("graphics.resources->Destroy_RenderTarget( %s );\n", Get_RenderTarget_Name(renderTarget) );
			}
			tw.NewLine();
		}
		
		if( shaderLib.stateBlocks.Num() )
		{
			for( UINT iStateBlock = 0;
				iStateBlock < shaderLib.stateBlocks.Num();
				iStateBlock++ )
			{
				const wrStateBlock& stateBlock = shaderLib.stateBlocks[ iStateBlock ];
				tw.Putf("ZERO_OUT( %s );\n", Get_StateBlock_Name(stateBlock) );
			}
		}

		if( shaderLib.vertexDeclarations.Num() )
		{
			for( UINT iVtx = 0;
				iVtx < shaderLib.vertexDeclarations.Num();
				iVtx++ )
			{
				const wrVertexDeclaration& vtx = shaderLib.vertexDeclarations[ iVtx ];

				tw.Putf("graphics.resources->Destroy_InputLayout( %s::layout );\n", vtx.name.ToChars() );
			}
		}


		for( UINT iSharedSection = 0;
			iSharedSection < shaderLib.sharedSections.Num();
			iSharedSection++ )
		{
			const wrSharedSection & sharedSection = shaderLib.sharedSections[ iSharedSection ];
			
			for( UINT iCB = 0;
				iCB < sharedSection.vars.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = sharedSection.vars.constantBuffers[ iCB ];

				tw.Putf("%s::%s.Destroy( graphics.shaders );\n"
					,Get_Shared_Section_Name(sharedSection.name.ToChars())
					,Get_ShaderConstantBuffer_Name(cb)
					);
			}
		}
	}
	//===========================================================================
	const char* Get_SamplerState_Name( const wrSourceElement& o )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_SamplerState_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_SamplerState_Name( const char* str )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_SamplerState_PREFIX, str );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_DepthStencilState_Name( const wrDepthStencilState& samplerState )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_DepthStencilState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_BlendState_Name( const wrBlendState& samplerState )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_BlendState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_RasterizerState_Name( const wrRasterizerState& samplerState )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_RasterizerState_PREFIX, samplerState.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_RenderTarget_Name( const wrRenderTarget& renderTarget )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_RenderTarget_PREFIX, renderTarget.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_StateBlock_Name( const wrStateBlock& stateBlock )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_StateBlock_PREFIX, stateBlock.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderConstantBuffer_Name( const wrShaderConstantBuffer& cb )
	{
		static StackString	tempBuffer;
		//ZERO_OUT(tempBuffer);
		tempBuffer.Format( "%s%s",
			WR_Shader_ConstantBuffer_PREFIX, cb.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderSampler_Name( const wrShaderSamplerState& o )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_Sampler_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderShaderResource_Name( const wrShaderResource& o )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_Texture_PREFIX, o.name.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	const char* Get_ShaderClass_Name( const wrShaderProgram& o )
	{
		return Get_ShaderClass_Name(o.name);
	}
	//===========================================================================
	const char* Get_ShaderClass_Name( const wrName& originalName )
	{
		static StackString	tempBuffer;
		tempBuffer.Format( "%s%s",
			WR_Shader_PREFIX, originalName.ToChars() );
		return tempBuffer.ToChars();
	}
	//===========================================================================
	void Emit_Shader_Declarations( const TList< wrShaderProgram >& shaders, MyTextWriter & tw )
	{
		const UINT numShaders = shaders.Num();

		if( !numShaders ) {
			return;
		}

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
	void Emit_Struct( const char* structName, MyTextWriter & tw )
	{
		tw.Putf( "struct MX_GRAPHICS_API %s\n", structName );
	}
	//===========================================================================
	void Emit_Shader_Variables_As_Struct_Members_Declarations( const wrShaderVariables& vars, MyTextWriter & tw )
	{
		// Constant buffers
		if( vars.constantBuffers.Num() )
		{
			{
				wrSetScopeLevel	d(tw,0);
				tw.Putf("#pragma pack (push,%u)\n", WR_Shader_ConstantBuffer_Alignment);
			}

			for( UINT iCB = 0;
				iCB < vars.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = vars.constantBuffers[ iCB ];

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
				iCB < vars.constantBuffers.Num();
				iCB++ )
			{
				const wrShaderConstantBuffer& cb = vars.constantBuffers[ iCB ];

				tw.Putf("static TConstantBuffer< %s >	%s;	//%s\n"
					,cb.name.ToChars(),Get_ShaderConstantBuffer_Name(cb)
					,GetString_UsedByShaderStages(cb).ToChars()
					);
			}
		}//Constant buffers



		// Sampler states
		if( vars.samplers.Num() )
		{
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Sampler states (set manually)\n";
			}

			for( UINT iSamplerState = 0;
				iSamplerState < vars.samplers.Num();
				iSamplerState++ )
			{
				const wrShaderSamplerState& ss = vars.samplers[ iSamplerState ];

				tw.Putf("static ID3D11SamplerState* %s;	//%s\n"
					,ss.name.ToChars()
					,GetString_UsedByShaderStages(ss).ToChars()
					);
			}
		}//Sampler states


		// Shader resources

		if( vars.resources.Num() )
		{
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Shader resources (set manually)\n";
			}

			for( UINT iShaderResource = 0;
				iShaderResource < vars.resources.Num();
				iShaderResource++ )
			{
				const wrShaderResource& tex = vars.resources[ iShaderResource ];

				tw.Putf("static ID3D11ShaderResourceView* %s;	//%s\n"
					,tex.name.ToChars()
					,GetString_UsedByShaderStages(tex).ToChars()
					);

				tw.Putf("\t\t\t\t\t\tenum { %s_Slot_%s = %u };\n",
					"SR", tex.name.ToChars(), tex.iRegister );
				//enum { SR_Slot_shadowDepthMap = 3 };
			}

		}//Shader resources



	}

	//===========================================================================

	StackString GetString_UsedByShaderStages( const wrRegisterBound& o )
	{
		StackString	result(_InitSlow,"");
		if( o.usedByVS() )
		{
			result.Append(MXC(" VS"));
		}
		if( o.usedByGS() )
		{
			result.Append(MXC(" GS"));
		}
		if( o.usedByPS() )
		{
			result.Append(MXC(" PS"));
		}
		return result;
	}


	//===========================================================================
	void Emit_Shader_Declaration( const wrShaderProgram & shader, MyTextWriter & tw )
	{
		//tw << MXC("/*\n");
		tw << MXC("//==============================================================================================================================================\n");
		tw << MXC("//\n");
		tw.Putf( "//	%s\n", shader.name.ToChars() );
	
		if(shader.inputs.sharedSections.Num())
		{
			tw << MXC("//\n");
			tw.Putf( "//	uses" );

			for( UINT iSharedSection = 0;
				iSharedSection < shader.inputs.sharedSections.Num();
				iSharedSection++ )
			{
				const wrBuffer & sharedSection = shader.inputs.sharedSections[ iSharedSection ];
				//tw.Emitf(" %s", sharedSection.ToChars());
				tw.Emitf(" %s",
					Get_Shared_Section_Name(sharedSection.ToChars())
					);
				if(iSharedSection < shader.inputs.sharedSections.Num()-1)
				{
					tw.Emit(",");
				}
			}

			tw.NewLine();
		}
		
		tw << MXC("//\n");
		tw << MXC("//==============================================================================================================================================\n");
		tw << MXC("//\n");
		//tw << MXC("*/\n");


		Emit_Comment_Source_Location( shader.pos, tw );

		Emit_Struct( Get_ShaderClass_Name(shader), tw );
		{
			wrScope	classScope(tw,ELeaveScopeAction::Scope_Semicolon);

			Emit_Shader_Variables_As_Struct_Members_Declarations(shader.inputs,tw);




			// Static public fields (attributes)
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Read-only properties\n";
			}

			tw.Putf("enum { UID = %u }; // unique index (within this shader library)\n",shader.uniqueIndex);
			tw.Putf("enum { NumInstances = %u }; // number of unique combinations\n",shader.CalcNumInstances());






			if(shader.defines.Num())
			{
				tw << "// Integer bit masks for identifying unique shader program combinations:\n";
				tw << "enum Variations\n";
				wrScope	enumScope(tw,ELeaveScopeAction::Scope_Semicolon);

				UINT defaultMask = 0;

				for( UINT iDefine = 0;
					iDefine < shader.defines.Num();
					iDefine++ )
				{
					const wrShaderDefine & define = shader.defines[ iDefine ];

					Assert(define.defaultValue==0 || define.defaultValue==1);

					tw.Putf( "%s = BIT(%u), //default=%u\n",
						define.name.ToChars(),iDefine, (UINT)define.defaultValue
						);

					defaultMask |= define.defaultValue;
				}

				tw.Putf( "DefaultInstanceId = %u\n", defaultMask );
			}
			else
			{
				//
			}


			// Static Public methods
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				//tw << "public:	// Public methods\n";
				tw << "public:	// Public member functions\n";
			}
			Emit_Shader_Public_Methods_Declarations(shader,tw);



			// Public internal
			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public_internal:\n";
			}

			tw << "static void Initialize();\n";
			tw << "static void Shutdown();\n";

			//if(shader.defines.Num())
			{
				tw.NewLine();
				tw << "static void GetShaderDefines( rxShaderInstanceId instanceId, dxShaderMacros &OutDefines );\n";
			}
			

			//tw.NewLine();
			//tw << "static SizeT GetSourceCodeLength();\n";
			//tw << "static void GetSourceCodeBytes( char *buffer );\n";
			//tw.NewLine();
			//tw << "static void GetShaderInfo( GrShaderData & d );\n";
			//tw << "static void Load( const GrShaderData& shaderInfo, GrShaderSystem* compiler );\n";

			//tw << "static const char* VS_EntryPoint; // vertex shader function's entry point\n";
			//tw << "static const char* PS_EntryPoint; // pixel shader function's entry point\n";
			//tw << "static const char* Name; // unique name\n";
			//tw << "static const rxStaticString Source; // original source code\n";





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

				tw.Putf("static ShaderInstance	shaderInstances[ %s::NumInstances ];\n",shader.name.ToChars());
			}



			// PREVENT_COPY( className )
			if(0)
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

				tw.Putf("%s\n",
					elem.GenDeclCodeString().ToChars()
					);
			}
		}
	}
	//===========================================================================
	void Emit_Bind_Shader_Variables_To_Pipeline_As_Struct_Member_Function(
		const wrShaderVariables& vars, MyTextWriter & tw
		)
	{
		RX_OPTIMIZE("bind to slots intelligently (bind the whole range at once)");

		if( vars.constantBuffers.Num() )
		{
			Emit_Bind_Constant_Buffers(vars.constantBuffers,tw);

		}// Bind constant buffers


		if(vars.samplers.Num())
		{
			tw.NewLine();

			Emit_Bind_Sampler_States(vars.samplers,tw);
			
		}//Bind sampler states


		if(vars.resources.Num())
		{
			tw.NewLine();

			Emit_Bind_Shader_Resources(vars.resources,tw);
		}//Bind Texture resources
	}
	
	//===========================================================================

	void Emit_Shader_Public_Methods_Declarations( const wrShaderProgram& shader, MyTextWriter & tw )
	{
		if(shader.defines.Num())
		{
			tw << "static FORCEINLINE BOOL IsValidInstanceId( rxShaderInstanceId instanceId )\n";
			{
				wrScope	functionScope(tw);
				tw.Putf("return ( instanceId < %s::NumInstances );\n",shader.name.ToChars());
			}
			tw << "static FORCEINLINE ShaderInstance& GetInstance( rxShaderInstanceId instanceId = DefaultInstanceId )\n";
			{
				wrScope	functionScope(tw);
				tw.Putf("Assert( IsValidInstanceId( instanceId ) );\n",shader.name.ToChars());
				tw << "return shaderInstances[ instanceId ];\n";
			}
		}
		else
		{
			tw << "static FORCEINLINE ShaderInstance& GetInstance()\n";
			{
				wrScope	functionScope(tw);
				tw << "return shaderInstances[ 0 ];\n";
			}
		}


		if(shader.defines.Num())
		{
			tw << "static FORCEINLINE void Set( ID3D11DeviceContext* theContext, rxShaderInstanceId instanceId = DefaultInstanceId )\n";
		}
		else
		{
			tw << "static FORCEINLINE void Set( ID3D11DeviceContext* theContext )\n";
		}
		{
			wrScope	functionScope(tw);

			Emit_Bind_Shader_Variables_To_Pipeline_As_Struct_Member_Function(shader.inputs,tw);
			if( !shader.inputs.are_empty() )
			{
				tw.NewLine();
			}

			tw << "// Bind shader program\n";

			if(shader.defines.Num())
			{
				tw << "ShaderInstance & program = GetInstance( instanceId );\n";
			}
			else
			{
				tw << "ShaderInstance & program = shaderInstances[ 0 ];\n";
			}

			tw.NewLine();

			tw << "theContext->VSSetShader(\n";
			tw << "	program.vertexShader,\n";
			tw << "	nil,	// ID3D11ClassInstance *const *ppClassInstances\n";
			tw << "	0	// NumClassInstances\n";
			tw << ");\n";

			if( shader.pixelShader != "nil" )
			{
				tw << "theContext->PSSetShader(\n";
				tw << "	program.pixelShader,\n";
				tw << "	nil,	// ID3D11ClassInstance *const *ppClassInstances\n";
				tw << "	0	// NumClassInstances\n";
				tw << ");\n";
			}
			else
			{
				tw << "theContext->PSSetShader(\n";
				tw << "	nil,\n";
				tw << "	nil,	// ID3D11ClassInstance *const *ppClassInstances\n";
				tw << "	0	// NumClassInstances\n";
				tw << ");\n";
			}
		}


		// Scoped Set():
		{
			tw.NewLine();
			tw << "// NOTE: it doesn't not restore the previous shader program\n";
			tw.Putf("class SetScoped\n");
			wrScope	classScope(tw,ELeaveScopeAction::Scope_Semicolon);

			tw << "ID3D11DeviceContext* pD3DContext;\n";

			const wrShaderVariables& vars = shader.inputs;

			TList<String>	cppCode_SaveStates;
			TList<String>	cppCode_restoreStates;

			if( vars.constantBuffers.Num() )
			{
				UINT	iMinCBSlot,iMaxCBSlot;
				Calc_Min_Max_Registers_Used(vars.constantBuffers,iMinCBSlot,iMaxCBSlot);

				const bool bUsedByVS = Check_If_Vars_Are_Used_By_VS(vars.constantBuffers);
				if( bUsedByVS )
				{
					tw.Putf("ID3D11Buffer* old_VS_CBs[ %u ];\n",vars.constantBuffers.Num());

					{String	strCppCode_Save_VS_CBs;
					strCppCode_Save_VS_CBs.Format(
						"pD3DContext->VSGetConstantBuffers( %u, %u, old_VS_CBs );\n",
						iMinCBSlot,vars.constantBuffers.Num()
					);
					cppCode_SaveStates.Add(strCppCode_Save_VS_CBs);}


					{String	strCppCode_Restore_VS_CBs;
					strCppCode_Restore_VS_CBs.Format(
						"pD3DContext->VSSetConstantBuffers( %u, %u, old_VS_CBs );	D3D_SafeReleaseArray( old_VS_CBs );\n",
						iMinCBSlot,vars.constantBuffers.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_VS_CBs);}
				}

				const bool bUsedByPS = Check_If_Vars_Are_Used_By_PS(vars.constantBuffers);
				if( bUsedByPS )
				{
					tw.Putf("ID3D11Buffer* old_PS_CBs[ %u ];\n",vars.constantBuffers.Num());

					{String	strCppCode_Save_PS_CBs;
					strCppCode_Save_PS_CBs.Format(
						"pD3DContext->PSGetConstantBuffers( %u, %u, old_PS_CBs );\n",
						iMinCBSlot,vars.constantBuffers.Num()
						);
					cppCode_SaveStates.Add(strCppCode_Save_PS_CBs);}


					{String	strCppCode_Restore_PS_CBs;
					strCppCode_Restore_PS_CBs.Format(
						"pD3DContext->PSSetConstantBuffers( %u, %u, old_PS_CBs );	D3D_SafeReleaseArray( old_PS_CBs );\n",
						iMinCBSlot,vars.constantBuffers.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_PS_CBs);}
				}
			}// Bind constant buffers


			if(vars.samplers.Num())
			{
				UINT	iMinSSSlot,iMaxSSSlot;
				Calc_Min_Max_Registers_Used(vars.samplers,iMinSSSlot,iMaxSSSlot);

				const bool bUsedByVS = Check_If_Vars_Are_Used_By_VS(vars.samplers);
				if( bUsedByVS ) {
					tw.Putf("ID3D11SamplerState* old_VS_SSs[ %u ];\n",vars.samplers.Num());

					{String	strCppCode_Save_VS_SS;
					strCppCode_Save_VS_SS.Format(
						"pD3DContext->VSGetSamplers( %u, %u, old_VS_SS );\n",
						iMinSSSlot,vars.samplers.Num()
						);
					cppCode_SaveStates.Add(strCppCode_Save_VS_SS);}


					{String	strCppCode_Restore_VS_SS;
					strCppCode_Restore_VS_SS.Format(
						"pD3DContext->VSSetSamplers( %u, %u, old_VS_SS );	D3D_SafeReleaseArray( old_VS_SS );\n",
						iMinSSSlot,vars.samplers.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_VS_SS);}
				}

				const bool bUsedByPS = Check_If_Vars_Are_Used_By_PS(vars.samplers);
				if( bUsedByPS ) {
					tw.Putf("ID3D11SamplerState* old_PS_SS[ %u ];\n",vars.samplers.Num());

					{String	strCppCode_Save_PS_SS;
					strCppCode_Save_PS_SS.Format(
						"pD3DContext->PSGetSamplers( %u, %u, old_PS_SS );\n",
						iMinSSSlot,vars.samplers.Num()
						);
					cppCode_SaveStates.Add(strCppCode_Save_PS_SS);}


					{String	strCppCode_Restore_PS_SS;
					strCppCode_Restore_PS_SS.Format(
						"pD3DContext->PSSetSamplers( %u, %u, old_PS_SS );	D3D_SafeReleaseArray( old_PS_SS );\n",
						iMinSSSlot,vars.samplers.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_PS_SS);}
				}
			}//Bind sampler states


			if(vars.resources.Num())
			{
				UINT	iMinSRVSlot,iMaxSRVSlot;
				Calc_Min_Max_Registers_Used(vars.resources,iMinSRVSlot,iMaxSRVSlot);

				const bool bUsedByVS = Check_If_Vars_Are_Used_By_VS(vars.resources);
				if( bUsedByVS )
				{
					tw.Putf("ID3D11ShaderResourceView* old_VS_SRVs[ %u ];\n",vars.resources.Num());

					{String	strCppCode_Save_VS_SRVs;
					strCppCode_Save_VS_SRVs.Format(
						"pD3DContext->VSGetShaderResources( %u, %u, old_VS_SRVs );\n",
						iMinSRVSlot,vars.resources.Num()
						);
					cppCode_SaveStates.Add(strCppCode_Save_VS_SRVs);}

					{String	strCppCode_Restore_VS_SRVs;
					strCppCode_Restore_VS_SRVs.Format(
						"pD3DContext->VSSetShaderResources( %u, %u, old_VS_SRVs );	D3D_SafeReleaseArray( old_VS_SRVs );\n",
						iMinSRVSlot,vars.resources.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_VS_SRVs);}
				}

				const bool bUsedByPS = Check_If_Vars_Are_Used_By_PS(vars.resources);
				if( bUsedByPS )
				{
					tw.Putf("ID3D11ShaderResourceView* old_PS_SRVs[ %u ];\n",vars.resources.Num());

					{String	strCppCode_Save_PS_SRVs;
					strCppCode_Save_PS_SRVs.Format(
						"pD3DContext->PSGetShaderResources( %u, %u, old_PS_SRVs );\n",
						iMinSRVSlot,vars.resources.Num()
						);
					cppCode_SaveStates.Add(strCppCode_Save_PS_SRVs);}

					{String	strCppCode_Restore_PS_SRVs;
					strCppCode_Restore_PS_SRVs.Format(
						"pD3DContext->PSSetShaderResources( %u, %u, old_PS_SRVs );	D3D_SafeReleaseArray( old_PS_SRVs );\n",
						iMinSRVSlot,vars.resources.Num()
						);
					cppCode_restoreStates.Add(strCppCode_Restore_PS_SRVs);}
				}
			}//Bind Texture resources

			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Public member functions\n";
			}

			// Ctor

			if(shader.defines.Num())
			{
				tw.Putf("FORCEINLINE SetScoped( ID3D11DeviceContext* theContext, rxShaderInstanceId instanceId = DefaultInstanceId )\n");
			}
			else
			{
				tw.Putf("FORCEINLINE SetScoped( ID3D11DeviceContext* theContext )\n");
			}
			{
				wrScope	ctorScope(tw);

				tw.Putf("pD3DContext = theContext;\n");

				// Save states
				for( UINT i=0; i <cppCode_SaveStates.Num(); i++ )
				{
					tw.Putf("%s",cppCode_SaveStates[i].ToChars());
				}

				if(shader.defines.Num())
				{
					tw.Putf("%s::Set( pD3DContext, instanceId );\n",shader.name.ToChars());
				}
				else
				{
					tw.Putf("%s::Set( pD3DContext );\n",shader.name.ToChars());
				}
			}

			// Dtor
			tw.Putf("FORCEINLINE ~SetScoped()\n");
			{
				wrScope	dtorScope(tw);

				// Save states
				for( UINT i=0; i <cppCode_restoreStates.Num(); i++ )
				{
					tw.Putf("%s",cppCode_restoreStates[i].ToChars());
				}
			}
		}
	}
	//===========================================================================
	void Emit_Shader_Definitions( const TList< wrShaderProgram >& shaders, MyTextWriter & tw )
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
			const wrShaderProgram& shader = shaders[ iShader ];

			Emit_Shader_Definition( shader, tw );
		}
	}
	//===========================================================================
	void Emit_Create_Constant_Buffers( const char* className, const TList< wrShaderConstantBuffer >& constantBuffers, MyTextWriter & tw )
	{
		//Constant buffers
		for( UINT iCB = 0;
			iCB < constantBuffers.Num();
			iCB++ )
		{
			const wrShaderConstantBuffer& cb = constantBuffers[ iCB ];

			//tw << "//[static]\n";
			tw.Putf("%s::%s.Create( graphics.shaders );\n"
				,className
				,Get_ShaderConstantBuffer_Name(cb)
				);

			//Emit_DbgSetName(WR_Shader_ConstantBuffer_PREFIX, cb, tw);

		}//Constant buffers
	}
	//===========================================================================
	void Emit_Shader_Variables_As_Struct_Members_Definitions( const wrShaderVariables& vars, MyTextWriter & tw, const char* className )
	{
		tw.NewLine();
		tw.NewLine();

		//Constant buffers
		for( UINT iCB = 0;
			iCB < vars.constantBuffers.Num();
			iCB++ )
		{
			const wrShaderConstantBuffer& cb = vars.constantBuffers[ iCB ];

			//tw << "//[static]\n";
			tw.Putf("TConstantBuffer< %s::%s >	%s::%s;\n"
				,className,cb.name.ToChars()
				,className,Get_ShaderConstantBuffer_Name(cb)
				);
		}//Constant buffers


		// Sampler states
		for( UINT iSamplerState = 0;
			iSamplerState < vars.samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = vars.samplers[ iSamplerState ];

			//tw << "//[static]\n";
			tw.Putf("ID3D11SamplerState* %s::%s = nil;\n"
				,className,Get_ShaderSampler_Name(ss)
				);
		}// Sampler states


		// Shader resources

		for( UINT iShaderResource = 0;
			iShaderResource < vars.resources.Num();
			iShaderResource++ )
		{
			const wrShaderResource& res = vars.resources[ iShaderResource ];

			//tw << "//[static]\n";
			tw.Putf("ID3D11ShaderResourceView* %s::%s = nil;\n"
				,className,Get_ShaderShaderResource_Name(res)
				);
		}//Shader resources
	}
	//===========================================================================
	void Emit_Initialize_SamplerStates( const char* className, const TList< wrShaderSamplerState >& samplers, MyTextWriter & tw )
	{
		// Sampler states
		for( UINT iSamplerState = 0;
			iSamplerState < samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = samplers[ iSamplerState ];

			const char* samplerStateName = Get_SamplerState_Name(ss.initializer.ToChars());

			StackString	samplerInitializer;

			if( ss.initializer.IsEmpty() )
			{
				samplerInitializer = "nil";
			}
			else
			{
				//samplerInitializer.Format("%s.p.Ptr",samplerStateName);
				samplerInitializer.Format("%s",samplerStateName);
			}

			tw.Putf("%s::%s = %s;\n"
				,className,Get_ShaderSampler_Name(ss)
				,samplerInitializer.ToChars()
				);

		}// Sampler states
	}
	//===========================================================================
	void Emit_Initialize_Shared_ShaderResourcesViews( const char* className, const TList< wrShaderResource >& pSRVs, MyTextWriter & tw )
	{
		// Sampler states
		for( UINT iSRV = 0;
			iSRV < pSRVs.Num();
			iSRV++ )
		{
			const wrShaderResource& sr = pSRVs[ iSRV ];

			//const char* shaderResStateName = Get_ShaderShaderResource_Name(sr);

			StackString	shaderResInitializer;

			if( sr.initializer.IsEmpty() )
			{
				shaderResInitializer = "nil";
			}
			else
			{
				shaderResInitializer.Format("%s%s.pSRV",
					WR_RenderTarget_PREFIX,sr.initializer.ToChars());
			}

			tw.Putf("%s::%s = %s;\n"
				,className,Get_ShaderShaderResource_Name(sr)
				,shaderResInitializer.ToChars()
				);

		}// ShaderResourcesViews
	}
	//===========================================================================

	void Emit_Shader_Definition( const wrShaderProgram& shader, MyTextWriter & tw )
	{
		const char* shaderClassName = Get_ShaderClass_Name(shader);
		const UINT numShaderInstances = shader.CalcNumInstances();
		(void)numShaderInstances;

		Emit_Shader_Variables_As_Struct_Members_Definitions(shader.inputs,tw,Get_ShaderClass_Name(shader));



		// shader source code

		// generate external source file
		StackString	extShaderSourceFileName;

		extShaderSourceFileName.Format("%s_AUTO.hxx",
			shader.name.ToChars());

		{
			//const SizeT srcCodeLength = shader.generatedCode.Num();

			//const char* srcCodeBytes = (const char*)shader.generatedCode.ToPtr();
			//StaticAssert( sizeof srcCodeBytes[0] == sizeof shader.generatedCode.ToPtr()[0] );

			//(void)srcCodeLength;


			// generate external source file
			{
				StackString	fullExtSourceFileName;
				fullExtSourceFileName.Format("%s%s",
					m_options.outputFolderHLSL.ToChars(),
					extShaderSourceFileName.ToChars());

				FileWriter	fileWriter(fullExtSourceFileName.ToChars(),0);
				mxTextWriter	tw2(fileWriter);

				if(m_options.bEmitComments)
				{
					tw2.Putf(
						"// %s\n"
						"// derived from '%s' (%u)\n",
						extShaderSourceFileName.ToChars(),
						shader.pos.file.ToChars(),
						shader.pos.line
					);

					if(shader.defines.Num())
					{
						tw2 << "// Defines:\n";
						for( UINT iDefine = 0;
							iDefine < shader.defines.Num();
							iDefine++ )
						{
							const wrShaderDefine & define = shader.defines[ iDefine ];

							tw2.Putf( "//	%s = %u\n", define.name.ToChars(),define.defaultValue );
						}
					}
				}

				//tw2.Putf(
				//	"#line 1 \"%s%s\"\n",
				//	m_options.outputFolderHLSL.ToChars(),
				//	extShaderSourceFileName.ToChars()
				//);

				//fileWriter.Write(
				tw2.Put(shader.generatedCode.ToChars(),shader.generatedCode.GetDataSize());
			}

		}// shader source code


		// Read-only properties
		// Shader instances

		MX_OPTIMIZE("shader instances should be grouped together (CPU cache)");

		//tw << "//[static]\n";
		tw.Putf("ShaderInstance %s::shaderInstances[%u];\n"
			,Get_ShaderClass_Name(shader),shader.CalcNumInstances()
			);

		tw.NewLine();

		
		Emit_Extern_Declarations(shader,tw);



		//-------------------
		// Functions.
		//-------------------

		//Initialize
		{
			tw.Putf("void %s::Initialize()\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);

			Emit_Cpp_code_for_assertions(Get_ShaderClass_Name
				(shader),
				shader.inputs,
				tw
				);

			//Constant buffers
			if(shader.inputs.constantBuffers.Num())
			{
				Emit_Create_Constant_Buffers(Get_ShaderClass_Name(shader),shader.inputs.constantBuffers,tw);
				tw.NewLine();
			}
			//samplers
			if(shader.inputs.samplers.Num())
			{
				Emit_Initialize_SamplerStates(Get_ShaderClass_Name(shader),shader.inputs.samplers,tw);
				tw.NewLine();
			}

			tw.Putf("static GrShaderData d;\n");

			tw.Putf("d.uniqueId = %s::UID;\n",shaderClassName);
			tw.Putf("d.name = GR_TEXT(\"%s\");\n",shaderClassName);
			tw.Putf("d.file = GR_TEXT(\"%s\");\n",extShaderSourceFileName.ToChars());

			tw.Putf("d.VS_EntryPoint = GR_TEXT(\"%s\");\n",shader.vertexShader.ToChars());

			if( shader.pixelShader != "nil" ) {
				tw.Putf("d.PS_EntryPoint = GR_TEXT(\"%s\");\n",shader.pixelShader.ToChars());
			} else {
				tw.Putf("d.PS_EntryPoint = nil;	// null pixel shader\n");
			}

			tw.Putf("d.numInstances = %s::NumInstances;\n",shaderClassName);
			tw.Putf("d.instances = %s::shaderInstances;\n",shaderClassName);
			tw.Putf("d.getDefines = %s::GetShaderDefines;\n",shaderClassName);

			tw.Putf("static ShaderInstanceData instancesData[ %s::NumInstances ];\n",shaderClassName);
			tw.Putf("d.instancesData = instancesData;\n");

			tw.NewLine();

			tw.Putf("graphics.shaders->RegisterShader(&d);\n");

		}//Initialize

		tw.NewLine();


		//GetShaderDefines
		//if(shader.defines.Num())
		{
			tw.Putf("void %s::GetShaderDefines( rxShaderInstanceId instanceId, dxShaderMacros &OutDefines )\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);

			tw << "OutDefines.Empty();\n";

			for( UINT iDefine = 0;
				iDefine < shader.defines.Num();
				iDefine++ )
			{
				const wrShaderDefine & define = shader.defines[ iDefine ];

				StackString		definition;
				definition.Format("%u",define.defaultValue);

				tw.Putf( "OutDefines.Add().Set( \"%s\", (instanceId & %s) ? \"1\" : \"0\" );\n", define.name.ToChars(), define.name.ToChars() );
			}

			tw.Putf( "OutDefines.Add().Set( nil, nil );	// append null terminator\n" );
		}//GetShaderDefines


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


#if 0
		tw.NewLine();


		//void GetShaderInfo( GrShaderData & d )
		{
			tw.Putf("void %s::GetShaderInfo( GrShaderData & d )\n"
				,shaderClassName);
			wrScope	sc(tw);

			tw.Putf("outInfo.source.code = %s::Source.data;\n",shaderClassName);
			tw.Putf("outInfo.source.codeLength = %s::Source.size;\n",shaderClassName);

			tw.Putf("outInfo.uniqueId = %s::UID;\n",shaderClassName);
			tw.Putf("outInfo.name = %s::Name;\n",shaderClassName);

			tw.Putf("outInfo.VS_EntryPoint = %s::VS_EntryPoint;\n",shaderClassName);
			tw.Putf("outInfo.PS_EntryPoint = %s::PS_EntryPoint;\n",shaderClassName);

			tw.Putf("outInfo.numInstances = %s::NumInstances;\n",shaderClassName);
			tw.Putf("outInfo.instances = %s::shaderInstances;\n",shaderClassName);

		}//void GetShaderInfo( GrShaderData & d )
#endif



#if 0
		//Load()
		{
			tw.Putf("void %s::Load( const GrShaderData& shaderInfo, GrShaderSystem* compiler )\n"
				,Get_ShaderClass_Name(shader));
			wrScope	sc(tw);


			Emit_Initialize_SamplerStates(Get_ShaderClass_Name(shader),shader.inputs.samplers,tw);


			tw << "compiler->NewShaderInstance( shaderInfo, shaderInstances[0] );\n";

			//tw << "Create_VertexShader( d.source, d.VS_EntryPoint, shaderInstances[0].vertexShader );\n";
			//tw << "Create_PixelShader( d.source, d.PS_EntryPoint, shaderInstances[0].pixelShader );\n";

			//tw << "d.instances = shaderInstances;\n";
			//tw << "d.numInstances = 1;\n";

		}//void Load( GrShaderData & d )
#endif



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
				tw.Putf("%s::%s.Destroy( graphics.shaders );\n"
					, Get_ShaderClass_Name(shader)
					,Get_ShaderConstantBuffer_Name(cb)
					);
			}//Constant buffers



			//tw.Putf("graphics.shaders->Destroy_ShaderInstances( %u, %s::shaderInstances );\n"
			//	,shader.NumInstances(), Get_ShaderClass_Name(shader));

		}//Shutdown
	}
	
	//===========================================================================
	void Emit_Shared_Sections_Declarations( const TList< wrSharedSection > & sharedSections, MyTextWriter & tw )
	{
		for( UINT iSharedSection = 0;
			iSharedSection < sharedSections.Num();
			iSharedSection++ )
		{
			const wrSharedSection & sharedSection = sharedSections[ iSharedSection ];
			Emit_Shared_Section_Declaration(sharedSection,tw);
		}
	}
//===========================================================================
	void Emit_Shared_Section_Declaration( const wrSharedSection& sharedSection, MyTextWriter & tw )
	{
		tw << MXC("//--------------------------------------------------------------\n");
		tw.Putf( "//	%s\n", sharedSection.name.ToChars() );
		tw << MXC("//--------------------------------------------------------------\n");
		tw << MXC("//\n");

		Emit_Struct( Get_Shared_Section_Name(sharedSection.name.ToChars()), tw );
		{
			wrScope	classScope(tw,ELeaveScopeAction::Scope_Semicolon);

			Emit_Shader_Variables_As_Struct_Members_Declarations(sharedSection.vars,tw);

			Emit_Shared_Section_Methods_Declarations(sharedSection,tw);
		}

		tw.NewLine();
	}
	//===========================================================================

	void Emit_Shared_Section_Methods_Declarations( const wrSharedSection& sharedSection, MyTextWriter & tw )
	{
		{
			wrDecreaseScopeLevel	d(tw,1);
			tw.NewLine();
			tw << "public:	// Public methods\n";
		}


		tw << "static inline void Set( ID3D11DeviceContext* theContext )\n";
		{
			wrScope	functionScope(tw);

			Emit_Bind_Shader_Variables_To_Pipeline_As_Struct_Member_Function(sharedSection.vars,tw);
		}
	}
	//===========================================================================
	void Emit_Shared_Sections_Definitions( const TList< wrSharedSection > & sharedSections, MyTextWriter & tw )
	{
		if( !sharedSections.Num() )return;

		for( UINT iSharedSection = 0;
			iSharedSection < sharedSections.Num();
			iSharedSection++ )
		{
			const wrSharedSection & sharedSection = sharedSections[ iSharedSection ];
			Emit_Shared_Section_Definition(sharedSection,tw);
		}

		tw.NewLine();

		tw << "static void SetupSharedSections()\n";
		{
			wrScope	sc(tw);

			for( UINT iSharedSection = 0;
				iSharedSection < sharedSections.Num();
				iSharedSection++ )
			{
				const wrSharedSection & sharedSection = sharedSections[ iSharedSection ];

				Emit_Cpp_code_for_assertions(Get_Shared_Section_Name(sharedSection.name.ToChars()),
					sharedSection.vars,
					tw);

				Emit_Create_Constant_Buffers(
					Get_Shared_Section_Name(sharedSection.name.ToChars()),
					sharedSection.vars.constantBuffers,
					tw);

				Emit_Initialize_SamplerStates(
					Get_Shared_Section_Name(sharedSection.name.ToChars()),
					sharedSection.vars.samplers,
					tw);

				Emit_Initialize_Shared_ShaderResourcesViews(
					Get_Shared_Section_Name(sharedSection.name.ToChars()),
					sharedSection.vars.resources,
					tw);
			}
		}

		tw.NewLine();
	}
	//===========================================================================
	void Emit_Shared_Section_Definition( const wrSharedSection& sharedSection, MyTextWriter & tw )
	{
		Emit_Shader_Variables_As_Struct_Members_Definitions(sharedSection.vars,tw,
			Get_Shared_Section_Name(sharedSection.name.ToChars() ));
	}
	//===========================================================================

	void Emit_Extern_Declarations( const wrShaderProgram& shader, MyTextWriter & tw )
	{
		// Sampler states
		for( UINT iSamplerState = 0;
			iSamplerState < shader.inputs.samplers.Num();
			iSamplerState++ )
		{
			const wrShaderSamplerState& ss = shader.inputs.samplers[ iSamplerState ];

			const char* samplerStateName = Get_SamplerState_Name(ss.initializer.ToChars());

			if( !ss.initializer.IsEmpty() )
			{
				tw.Putf("extern ID3D11SamplerStatePtr %s;\n",samplerStateName);
			}

		}// Sampler states

		tw.NewLine();
	}
	//===========================================================================
	void Emit_Vertex_Declarations( const TList< wrVertexDeclaration >& vertexDecls, MyTextWriter & tw )
	{
		if( vertexDecls.Num() )
		{
			{
				wrSetScopeLevel	d(tw,0);
				tw.Putf("#pragma pack (push,1)\n");
			}

			for( UINT iVtx = 0;
				iVtx < vertexDecls.Num();
				iVtx++ )
			{
				const wrVertexDeclaration& vtx = vertexDecls[ iVtx ];

				Emit_Vertex_Struct_Declaration(vtx,tw);
			}

			{
				wrSetScopeLevel	d(tw,0);
				tw.Putf("#pragma pack (pop)\n");
			}

		}//Constant buffers
	}
	//===========================================================================
	void Emit_Vertex_Struct_Declaration(const wrVertexDeclaration& vtx,MyTextWriter & tw)
	{
		{
			tw.NewLine();
			tw << MXC("//==============================================================================================================================================\n");
			tw << MXC("//\n");
			tw.Putf("//	%s\n",vtx.name.ToChars());
			tw << MXC("//\n");
			tw << MXC("//==============================================================================================================================================\n");
			tw << MXC("//\n");

			Emit_Comment_Source_Location(vtx.pos,tw);

			tw.Putf("struct %s\n",vtx.name.ToChars());
			wrScope	sc(tw,Scope_Semicolon);

			for( UINT iVertexElement = 0; iVertexElement < vtx.elements.Num(); iVertexElement++ )
			{
				const wrVertexElement& elem = vtx.elements[ iVertexElement ];

				//tw.Putf(
				//	"%s %s : %s;\n"
				//	,DXGIFormat_ToEngineTypeString(
				//		String_ToDXGIFormat(elem.Format.ToChars())
				//		,rxUtil_GetVertexElementSemanticEnum(elem.SemanticName.ToChars())
				//		)
				//	,elem.name.ToChars()
				//	,elem.SemanticName.ToChars()
				//);

				tw.Putf(
					"%s		%s;	// %s"
					,DXGIFormat_ToEngineTypeString(
					String_ToDXGIFormat(elem.Format.ToChars())
					,rxUtil_GetVertexElementSemanticEnum(elem.SemanticName.ToChars())
					)
					,elem.name.ToChars()
					,elem.SemanticName.ToChars()
					);

				if(!elem.info.IsEmpty())
				{
					mxReplaceCharANSI((char*)elem.info.ToChars(),elem.info.Length(),'\"',' ');
					tw.Emitf(" (%s)",elem.info.ToChars());
				}
				tw.NewLine();
			}

			tw.NewLine();
			tw.Putf("// ");

			Assert(vtx.elements.Num()>0);

			UINT totalSize = 0;



			for( UINT iVertexElement = 0;
				iVertexElement < vtx.elements.Num();
				iVertexElement++ )
			{
				const wrVertexElement& elem = vtx.elements[ iVertexElement ];

				DXGI_FORMAT format = String_ToDXGIFormat(elem.Format.ToChars());

				const UINT elemSize = DXGIFormat_GetElementSize(format);

				const_cast<wrVertexElement&>(elem).sizeInBytes = elemSize;



				totalSize += elemSize;



				if( vtx.elements.Num() > 1 )
				{
					tw.Emitf("%u", elemSize);

					if(iVertexElement < vtx.elements.Num()-1)
					{
						tw.Emit(" + ");
					}
				}
			}

			if( vtx.elements.Num() > 1 )
			{
				tw.Emitf(" = %u bytes\n",totalSize);
			}
			else if( vtx.elements.Num() == 1 )
			{
				tw.Emitf("%u bytes\n",totalSize);
			}
			else {
				Unreachable;
			}


			tw.NewLine();



			tw.Putf("enum { Stride = %u };	// size of a single vertex, in bytes\n",totalSize);
			tw.NewLine();



			const_cast<wrVertexDeclaration&>(vtx).elementNames.
				SetNum(vtx.elements.Num());

			tw.Putf("// vertex components enum\n");
			tw.Putf("enum Elements\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);

				for( UINT iVertexElement = 0;
					iVertexElement < vtx.elements.Num();
					iVertexElement++ )
				{
					const wrVertexElement& elem = vtx.elements[ iVertexElement ];

					const_cast<wrVertexDeclaration&>(vtx).
						elementNames[iVertexElement].Format("%s_index",elem.name.ToChars());

					tw.Putf("%s = %u,\n",
						vtx.elementNames[iVertexElement].ToChars(),iVertexElement);
				}

				tw.Putf("NumElements = %u\n",vtx.elements.Num());
			}

			tw.NewLine();


			tw.Putf("// size of each element, in bytes\n");
			tw.Putf("enum ElementSizes\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);

				for( UINT iVertexElement = 0;
					iVertexElement < vtx.elements.Num();
					iVertexElement++ )
				{
					const wrVertexElement& elem = vtx.elements[ iVertexElement ];

					const UINT elemSize = elem.sizeInBytes;

					tw.Putf("%s_size = %u,\n",elem.name.ToChars(),elemSize);
				}
			}
			tw.NewLine();




			tw.Putf("// vertex component mask\n");
			tw.Putf("enum\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);
				tw.Putf("Mask = 0\n");

				for( UINT iVertexElement = 0;
					iVertexElement < vtx.elements.Num();
					iVertexElement++ )
				{
					const wrVertexElement& elem = vtx.elements[ iVertexElement ];

					EVertexElementUsage sem = rxUtil_GetVertexElementSemanticEnum(elem.SemanticName.ToChars());
					const char* semStr = rxUtil_GetVertexElementSemanticStr(sem);

					tw.Putf("\t| (1 << %s)\n",semStr);
				}
			}
			tw.NewLine();








			tw.Putf("// vertex buffer streams\n");
			tw.Putf("enum Streams\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);

				const UINT numStreams = vtx.CalcNumStreams();

				TList< wrStream >	streams;
				streams.SetNum(numStreams);

				const_cast<wrVertexDeclaration&>(vtx).streamNames.SetNum(numStreams);
				const_cast<wrVertexDeclaration&>(vtx).streamSizes.SetNum(numStreams);

				for( UINT iVertexElement = 0;
					iVertexElement < vtx.elements.Num();
					iVertexElement++ )
				{
					const wrVertexElement& elem = vtx.elements[ iVertexElement ];

					const UINT streamIndex = elem.InputSlot;
					streams[ streamIndex ].elems.Add(elem);
				}



				const_cast<wrVertexDeclaration&>(vtx).streams = streams;


				for( UINT iVertexStream = 0;
					iVertexStream < streams.Num();
					iVertexStream++ )
				{
					const wrStream& vtxStream = streams[ iVertexStream ];

					String	streamName;
					UINT streamSize = 0;

					for( UINT iStreamElement = 0;
						iStreamElement < vtxStream.elems.Num();
						iStreamElement++ )
					{
						const wrVertexElement& elem = vtxStream.elems[ iStreamElement ];
						streamName.Append(elem.name.ToChars());

						if(iStreamElement < vtxStream.elems.Num()-1){
							streamName.Append("_");
						}

						streamSize += elem.sizeInBytes;
					}
					streamName.Append("_stream");


					const_cast<wrVertexDeclaration&>(vtx)
						.streamNames[iVertexStream].SetString( streamName );

					const_cast<wrVertexDeclaration&>(vtx)
						.streamSizes[iVertexStream] = streamSize;


					tw.Putf("%s = %u,\n",streamName.ToChars(),iVertexStream);
				}

				tw.Putf("NumStreams = %u\n",numStreams);
			}

			tw.NewLine();




			tw.Putf("// Strides of each vertex buffer.\n");
			tw.Putf("// Each stride is the size (in bytes) of the elements\n");
			tw.Putf("// that are to be used from that vertex buffer.\n");
			tw.Putf("enum StreamStrides\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);

				for( UINT iVertexStream = 0;
					iVertexStream < vtx.streamSizes.Num();
					iVertexStream++ )
				{
					const wrName& streamName = vtx.streamNames[ iVertexStream ];
					const UINT streamSize = vtx.streamSizes[ iVertexStream ];

					tw.Putf("%s_size = %u,\n",streamName.ToChars(),streamSize);
				}
			}
			tw.NewLine();


			tw.Putf("// Offsets of each element relative to the corresponding vertex buffer stream.\n"
				);
			tw.Putf("enum ElementOffsetsWithinStream\n");
			{
				wrScope	enumScope(tw,Scope_Semicolon);


				for( UINT iVertexStream = 0;
					iVertexStream < vtx.streams.Num();
					iVertexStream++ )
				{
					const wrStream& vtxStream = vtx.streams[ iVertexStream ];

					UINT accumOffset = 0;

					for( UINT iStreamElement = 0;
						iStreamElement < vtxStream.elems.Num();
						iStreamElement++ )
					{
						const wrVertexElement& elem = vtxStream.elems[ iStreamElement ];

						tw.Putf("%s_stream_offset = %u,\n",elem.name.ToChars(),accumOffset);

						accumOffset += elem.sizeInBytes;
					}
				}
			}
			tw.NewLine();





			tw.Putf("typedef %s THIS_TYPE;\n",vtx.name.ToChars());

			tw.NewLine();



			Assert(vtx.uniqueIndex != INDEX_NONE);
			{
				tw.Putf("enum { UID = %u }; // unique index (within this shader library)\n",vtx.uniqueIndex);
			}


			{
				UINT vertexFmtHash = FNV32_StringHash(vtx.name.ToChars());
				tw.Putf("enum { GUID = %u }; // unique vertex format hash\n",vertexFmtHash);
			}


			tw.NewLine();


			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Public member functions\n";
			}
			tw.Putf("static const UINT* GetElementStridesArray()\n");
			{
				wrScope	funcScope(tw);

				StackString	strides;
				for( UINT iVertexElement = 0;
					iVertexElement < vtx.elements.Num();
					iVertexElement++ )
				{
					const wrVertexElement& elem = vtx.elements[ iVertexElement ];

					const UINT elemSize = elem.sizeInBytes;

					StackString	tmp;
					tmp.Format("%u",elemSize);

					strides += tmp;

					if(iVertexElement < vtx.elements.Num()-1)
					{
						strides += MXC(", ");
					}
				}

				tw.Putf("static const UINT elementStridesArray[%u] = { %s };\n"
					,vtx.elements.Num()
					,strides.ToChars());
				tw.Putf("return elementStridesArray;\n");
			}



			tw.Putf("static const UINT* GetStreamStridesArray()\n");
			{
				wrScope	funcScope(tw);

				StackString	strides;
				for( UINT iVertexStream = 0;
					iVertexStream < vtx.streamSizes.Num();
					iVertexStream++ )
				{
					const UINT streamSize = vtx.streamSizes[ iVertexStream ];

					StackString	tmp;
					tmp.Format("%u",streamSize);

					strides += tmp;

					if(iVertexStream < vtx.streamSizes.Num()-1)
					{
						strides += MXC(", ");
					}
				}

				tw.Putf("static const UINT streamStridesArray[%u] = { %s };\n"
					,vtx.streamSizes.Num()
					,strides.ToChars());
				tw.Putf("return streamStridesArray;\n");
			}


			tw.Putf("static void AssembleVertexData( const IndexedMesh& src, VertexData& dest );\n");

			{
				wrDecreaseScopeLevel	d(tw,1);
				tw.NewLine();
				tw << "public:	// Input layout\n";
			}
			tw.Putf("static ID3D11InputLayoutPtr layout;\n");
		}
		
		tw.Putf("MX_DECLARE_POD_TYPE( %s );\n",vtx.name.ToChars());
		tw.NewLine();

	}
	//===========================================================================
	void Emit_Vertex_Struct_Definitions( const TList< wrVertexDeclaration >& vertexDecls, MyTextWriter & tw )
	{
		if( !vertexDecls.Num() )
		{
			return;
		}

		for( UINT iVtx = 0;
			iVtx < vertexDecls.Num();
			iVtx++ )
		{
			const wrVertexDeclaration& vtx = vertexDecls[ iVtx ];
			Emit_Vertex_Struct_Definition(vtx,tw);
		}

		tw.NewLine();

	}
	//===========================================================================
	void Emit_Vertex_Struct_Definition(const wrVertexDeclaration& vtx,MyTextWriter & tw)
	{
		tw.Putf("void %s::AssembleVertexData( const IndexedMesh& src, VertexData& dest )\n"
			,vtx.name.ToChars()
			);
		{
			wrScope	funcScope(tw);


			tw.Putf("dest.streams.SetNum( NumStreams );\n");
			tw.NewLine();
			tw.Putf("const SizeT vertexCount = src.numVertices;\n");
	

			Assert(vtx.streamNames.Num() > 0);


			for( UINT iVertexStream = 0;
				iVertexStream < vtx.streamNames.Num();
				iVertexStream++ )
			{
				const wrName& streamName = vtx.streamNames[ iVertexStream ];

				tw.NewLine();

				//tw.Putf("%s = %u;\n",
				//	streamName.ToChars(),vtx.streamSizes[ iVertexStream ]);





				tw.Putf("dest.streams[ %s ].stride = %s_size;\n", streamName.ToChars(),streamName.ToChars() );
				tw.Putf("dest.streams[ %s ].SetNum( vertexCount );\n", streamName.ToChars(),streamName.ToChars() );

				tw.NewLine();

				const wrStream& stream = vtx.streams[iVertexStream];

				for( UINT iStreamElement = 0;
					iStreamElement < stream.elems.Num();
					iStreamElement++ )
				{
					const wrVertexElement& elem = stream.elems[ iStreamElement ];

					const char* elemType = DXGIFormat_ToEngineTypeString(
						String_ToDXGIFormat(elem.Format.ToChars())
						,rxUtil_GetVertexElementSemanticEnum(elem.SemanticName.ToChars())
						);
					const char* streamNameBySem = IndexedMesh::GetStreamNameBySemantic(
						rxUtil_GetVertexElementSemanticEnum(elem.SemanticName.ToChars())
						);

					tw.Putf("TCopyVertices( c_cast(%s*)dest.streams[ %s ].ToPtr(), src.%s,\n",
						elemType, streamName.ToChars(), streamNameBySem
						);
					tw.Putf("	vertexCount, %s_size, %s_stream_offset );\n",
						streamName.ToChars(), elem.name.ToChars()
						);

					tw.NewLine();
				}//iStreamElement
			}//iVertexStream
		}
		tw.NewLine();
	}
	//===========================================================================
	void Emit_Create_InputLayouts( const TList< wrVertexDeclaration >& vertexDecls, MyTextWriter & tw )
	{
		if(!vertexDecls.Num()) {
			return;
		}


		for( UINT iVtx = 0;
			iVtx < vertexDecls.Num();
			iVtx++ )
		{
			const wrVertexDeclaration& vtx = vertexDecls[ iVtx ];
			tw.Putf("ID3D11InputLayoutPtr %s::layout;\n",vtx.name.ToChars());
		}


		tw.NewLine();


		tw.Putf("static void CreateInputLayouts()\n");
		wrScope	sc(tw);

		for( UINT iVtx = 0;
			iVtx < vertexDecls.Num();
			iVtx++ )
		{
			const wrVertexDeclaration& vtx = vertexDecls[ iVtx ];

			Emit_Create_InputLayout(vtx,tw);

			tw.NewLine();
		}

	}
	//===========================================================================
	void Emit_Create_InputLayout(const wrVertexDeclaration& vtx, MyTextWriter & tw)
	{
		wrScope	sc0(tw);

		tw.Putf("dxVertexFormat	vtxFormat;\n");
		//tw.Putf("vtxFormat.Clear();\n");

		for( UINT iVertexElement = 0; iVertexElement < vtx.elements.Num(); iVertexElement++ )
		{
			const wrVertexElement& elem = vtx.elements[ iVertexElement ];

			wrScope	sc(tw);

			tw.Putf("D3D11_INPUT_ELEMENT_DESC & elemDesc = vtxFormat.elements.Add();\n");

			tw.Putf("elemDesc.SemanticName = \"%s\";\n",elem.SemanticName.ToChars());
			tw.Putf("elemDesc.SemanticIndex = %u;\n",elem.SemanticIndex);
			tw.Putf("elemDesc.Format = %s;\n",elem.Format.ToChars());
			tw.Putf("elemDesc.InputSlot = %u;\n",elem.InputSlot);

			//tw.Putf("elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;\n");
			tw.Putf("elemDesc.AlignedByteOffset = %s::%s_stream_offset;\n",
				vtx.name.ToChars(),elem.name.ToChars());

			tw.Putf("elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;\n");
			tw.Putf("elemDesc.InstanceDataStepRate = 0;\n");
		}

		tw.Putf("graphics.resources->Create_InputLayout( %s::layout, vtxFormat.elements.ToPtr(), vtxFormat.elements.Num() );\n", vtx.name.ToChars());

		Emit_DbgSetName(vtx,tw);
	}

	//===========================================================================

	void Emit_Bind_Constant_Buffers( const TList< wrShaderConstantBuffer >& constantBuffers, MyTextWriter & tw )
	{
		if( !constantBuffers.Num() )
		{
			return;
		}

		tw << "// Bind constant buffers\n";
		
		for( UINT iCB = 0;
			iCB < constantBuffers.Num();
			iCB++ )
		{
			const wrShaderConstantBuffer& cb = constantBuffers[ iCB ];

			if( !cb.isUsed() ) {
				continue;
			}

			Assert( cb.iRegister != INDEX_NONE );

			if( cb.usedByVS() )
			{
				tw.Putf("%s.BindTo_VS( theContext, %u );\n"
					,Get_ShaderConstantBuffer_Name(cb)
					,cb.iRegister
					);
			}
			//if( cb.usedByGS() )
			//{
			//	tw.Putf("%s.BindTo_GS( theContext, %u );\n"
			//		,Get_ShaderConstantBuffer_Name(cb)
			//		,cb.iRegister
			//		);
			//}
			if( cb.usedByPS() )
			{
				tw.Putf("%s.BindTo_PS( theContext, %u );\n"
					,Get_ShaderConstantBuffer_Name(cb)
					,cb.iRegister
					);
			}
		}
	}

	//===========================================================================

	struct Range
	{
		UINT	firstElem;
		UINT	startSlot;
		UINT	numSlots;

		Range()
		{
			firstElem = INDEX_NONE;
			startSlot = INDEX_NONE;
			numSlots = INDEX_NONE;
		}
	};

	template< class TYPE >	// where TYPE : wrRegisterBound
	void OptimizeForRangedSet( const TList<TYPE>& srcList, TList<Range> &ranges )
	{
		ranges.Empty();

		const UINT numElems = srcList.Num();

		Assert(numElems > 0);

		const TYPE & p = srcList[ 0 ];

		Assert( p.isUsed() );
		Assert( p.iRegister != INDEX_NONE );


		Range& range = ranges.Add();

		if( numElems == 1 )
		{
			range.firstElem = 0;
			range.startSlot = p.iRegister;
			range.numSlots = 1;
		}
		else
		{
			// NOTE: we cannot sort the srcList because it keeps the original order
			// (how they were declared in a source file),
			// how they are laid out in memory

			Range& prevRange = range;

			Assert( prevRange.isUsed() );
			Assert( prevRange.iRegister != INDEX_NONE );

			for( UINT i = 0;
				i < srcList.Num();
				i++ )
			{
				const TYPE & p = srcList[i];


				if( p.isUsed() )
				{
					Assert( p.iRegister != INDEX_NONE );

					range.startSlot = p.iRegister;
					range.numSlots;
				}
				else
				{

				}
			}
		}
	}


	template< class T >	// where T : wrRegisterBound
	//void Emit_Set_Objects_To_Device_Context_Template(
	bool Are_elements_Contiguous(
		const TList< T > & elements,
		MyTextWriter & tw
		)
	{
		Assert(elements.Num()>0);

		bool elementsAreContiguous = true;

		UINT prevRegister = elements[ 0 ].iRegister;
		Assert( elements[ 0 ].iRegister != INDEX_NONE );

		for( UINT i = 1; i < elements.Num(); i++ )
		{
			const T & elem = elements[ i ];

			Assert( elem.isUsed() );
			Assert( elem.iRegister != INDEX_NONE );

			if( elem.iRegister != prevRegister+1 )
			{
				elementsAreContiguous = false;
				break;
			}

			prevRegister = elem.iRegister;
		}

		Assert(elementsAreContiguous);
		return elementsAreContiguous;
	}

	void Emit_Bind_Sampler_States( const TList< wrShaderSamplerState >& samplers, MyTextWriter & tw )
	{
		if( !samplers.Num() )
		{
			return;
		}

		tw << "// Bind sampler states to slots\n";

		const bool bStupidNoOptimizations = false;

		if( bStupidNoOptimizations )
		{
			for( UINT iSamplerState = 0;
				iSamplerState < samplers.Num();
				iSamplerState++ )
			{
				const wrShaderSamplerState& ss = samplers[ iSamplerState ];

				if( !ss.isUsed() ) {
					continue;
				}

				Assert( ss.iRegister != INDEX_NONE );

				if( ss.usedByVS() )
				{
					tw << "theContext->VSSetSamplers(\n";
					tw.Putf("\t%u,\n",ss.iRegister);
					tw.Putf("\t1,\n");
					tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
					tw << ");\n";
				}
				//if( ss.usedByGS() )
				//{
				//	tw << "theContext->GSSetSamplers(\n";
				//	tw.Putf("\t%u,\n",ss.iRegister);
				//	tw.Putf("\t1,\n");
				//	tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
				//	tw << ");\n";
				//}
				if( ss.usedByPS() )
				{
					tw << "theContext->PSSetSamplers(\n";
					tw.Putf("\t%u,\n",ss.iRegister);
					tw.Putf("\t1,\n");
					tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ss));
					tw << ");\n";
				}
			}

		}
		else // !bStupidNoOptimizations
		{
			//Emit_Set_Objects_To_Device_Context_Template( samplers, tw );
			if(Are_elements_Contiguous( samplers, tw ))
			{
				const wrShaderSamplerState& ssFirst = samplers.GetFirst();
				//const wrShaderSamplerState& ssLast = samplers.GetLast();

				if( AnyUsedBy(samplers,(CB_Bind_VS)) )
				{
					tw << "theContext->VSSetSamplers(\n";
					tw.Putf("\t%u,\n",ssFirst.iRegister);
					tw.Putf("\t%u,\n",samplers.Num());
					tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ssFirst));
					tw << ");\n";
				}
				if( AnyUsedBy(samplers,(CB_Bind_PS)) )
				{
					tw << "theContext->PSSetSamplers(\n";
					tw.Putf("\t%u,\n",ssFirst.iRegister);
					tw.Putf("\t%u,\n",samplers.Num());
					tw.Putf("\t(ID3D11SamplerState**)&%s\n",Get_ShaderSampler_Name(ssFirst));
					tw << ");\n";
				}
			}
		}
	}

	template< class T >
	bool AnyUsedBy( const TList< T >& rList, UINT flags )
	{
		for( UINT i = 0; i < rList.Num(); i++ )
		{
			const T& o = rList[i];
			if( UINT(o.bindFlags) & flags ) {
				return true;
			}
		}
		return false;
	}


	//===========================================================================

	void Emit_Bind_Shader_Resources( const TList< wrShaderResource >& resources, MyTextWriter & tw )
	{
		if( !resources.Num() )
		{
			return;
		}

		tw << "// Bind shader resources\n";


#if 0
		for( UINT iShaderResource = 0;
			iShaderResource < resources.Num();
			iShaderResource++ )
		{
			const wrShaderResource& tex = resources[ iShaderResource ];

			if( !tex.isUsed() ) {
				continue;
			}

			Assert( tex.iRegister != INDEX_NONE );

			if( tex.usedByVS() )
			{
				tw << "theContext->VSSetShaderResources(\n";
				tw.Putf("\t%u,\n",tex.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
				tw << ");\n";
			}
			//if( tex.usedByGS() )
			//{
			//	tw << "theContext->GSSetShaderResources(\n";
			//	tw.Putf("\t%u,\n",tex.iRegister);
			//	tw.Putf("\t1,\n");
			//	tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
			//	tw << ");\n";
			//}
			if( tex.usedByPS() )
			{
				tw << "theContext->PSSetShaderResources(\n";
				tw.Putf("\t%u,\n",tex.iRegister);
				tw.Putf("\t1,\n");
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(tex));
				tw << ");\n";
			}
		}
#else
		if(Are_elements_Contiguous( resources, tw ))
		{
			const wrShaderResource& rFirst = resources.GetFirst();
			//const wrShaderResource& rLast = resources.GetLast();

			if( AnyUsedBy(resources,(CB_Bind_VS)) )
			{
				tw << "theContext->VSSetShaderResources(\n";
				tw.Putf("\t%u,\n",rFirst.iRegister);
				tw.Putf("\t%u,\n",resources.Num());
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(rFirst));
				tw << ");\n";
			}

			if( AnyUsedBy(resources,(CB_Bind_PS)) )
			{
				tw << "theContext->PSSetShaderResources(\n";
				tw.Putf("\t%u,\n",rFirst.iRegister);
				tw.Putf("\t%u,\n",resources.Num());
				tw.Putf("\t(ID3D11ShaderResourceView**)&%s\n",Get_ShaderShaderResource_Name(rFirst));
				tw << ");\n";
			}
		}
#endif
	}

	//=============================================================================

	void Emit_MultipleRenderTarget_Struct(const wrMultiRenderTarget& mrt, MyTextWriter & tw)
	{
		const char* mrtName = mrt.name.ToChars();

		const UINT numRenderTargets = mrt.renderTargets.Num();

		tw.Putf("struct %s\n",mrtName);
		wrScope	classScope(tw,Scope_Semicolon);

		tw.Putf("enum ERenderTargets\n");
		{
			wrScope	enumScope(tw,Scope_Semicolon);

			for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
			{
				const wrRenderTarget& rt = mrt.renderTargets[iRenderTarget];

				Assert(!rt.name.IsEmpty());

				tw.Putf("%s_index = %u,\n", rt.name.ToChars(),iRenderTarget);
			}

			tw.Putf("NumRenderTargets = %u\n", numRenderTargets);
		}

		tw.NewLine();

		// data

		tw << "// render target views in SoA layout:\n";
		tw.Putf("dxPtr< ID3D11RenderTargetView >	pMRT[ NumRenderTargets ];\n");
		tw << "// render targets are cleared with these colors:\n";
		tw.Putf("FColor	clearColors[ NumRenderTargets ];\n");

		{
			wrDecreaseScopeLevel	d(tw,1);
			tw.NewLine();
			tw << "public:\n";
		}

		// ctor

		tw.Putf("FORCEINLINE %s()\n",mrtName);
		{
			wrScope	ctorScope(tw);

			tw.Putf("Initialize();\n");
		}

		// Init function
		tw.Putf("void Initialize()\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
			{
				const wrRenderTarget& rt = mrt.renderTargets[iRenderTarget];

				tw.Putf("this->pMRT[ %s_index ] = %s.pRTV;\n",
					rt.name.ToChars(), Get_RenderTarget_Name(rt) );
			}
			
			tw.NewLine();

			for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
			{
				const wrRenderTarget& rt = mrt.renderTargets[iRenderTarget];

				const FColor& clearColor = rt.clearColor;

				tw.Putf("this->clearColors[ %s_index ].Set( %ff, %ff, %ff, %ff );\n",
					rt.name.ToChars(),
					clearColor.R, clearColor.G, clearColor.B, clearColor.A
				);
			}
			
		}

		tw.Putf("void ReleaseRenderTargets()\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
			{
				const wrRenderTarget& rt = mrt.renderTargets[iRenderTarget];

				tw.Putf("this->pMRT[ %s_index ] = nil;\n",
					rt.name.ToChars());
			}
		}

		tw.Putf("FORCEINLINE void ClearRenderTargets( ID3D11DeviceContext* theContext )\n");
		{
			wrScope	funcScope(tw);

			for( UINT iRenderTarget = 0; iRenderTarget < numRenderTargets; iRenderTarget++ )
			{
				const wrRenderTarget& rt = mrt.renderTargets[iRenderTarget];

				{
					wrScope	sc(tw);
					
					tw.Putf("ID3D11RenderTargetView* pRTV = this->pMRT[ %s_index ];\n",rt.name.ToChars());
					tw.Putf("const FLOAT* colorRGBA = this->clearColors[  %s_index ].ToFloatPtr();\n",rt.name.ToChars());
					tw.NewLine();
					tw.Putf("theContext->ClearRenderTargetView( pRTV, colorRGBA );\n");
				}
			}
		}

		tw.NewLine();

		tw.Putf("FORCEINLINE void Bind( ID3D11DeviceContext* theContext, ID3D11DepthStencilView* pDSV = nil )\n");
		{
			wrScope	funcScope(tw);
			tw.Putf("// Set render targets and depth-stencil.\n");
			tw.NewLine();
			tw.Putf("theContext->OMSetRenderTargets(\n");
			tw.Putf("	NumRenderTargets,\n");
			tw.Putf("	c_cast(ID3D11RenderTargetView**) this->pMRT,\n");
			tw.Putf("	pDSV\n");
			tw.Putf(");\n");
		}

		tw.NewLine();

		tw.Putf("FORCEINLINE ID3D11RenderTargetView** GetRenderTargetsArray()\n");
		{
			wrScope	funcScope(tw);
			tw.Putf("return c_cast(ID3D11RenderTargetView**) this->pMRT;\n");
		}

		tw.NewLine();

		tw.Putf("FORCEINLINE UINT Num() const\n");
		{
			wrScope	funcScope(tw);
			tw.Putf("return ARRAY_SIZE(this->pMRT);\n");
		}
	}

	//===========================================================================
	void Emit_Cpp_code_for_assertions( const char* structName, const wrShaderVariables& vars, MyTextWriter& tw )
	{
		bool bHadAny = false;

		//Constant buffers
		//for( UINT iCB = 0;
		//	iCB < vars.constantBuffers.Num();
		//	iCB++ )
		//{
		//	const wrShaderConstantBuffer& cb = vars.constantBuffers[ iCB ];
		//	Assert(cb.iRegister!=INDEX_NONE);

		//	tw.Putf("cbuffer %s : register(b%u)\n"
		//		,cb.name.ToChars()
		//		,cb.iRegister
		//		);
		//}


		// Sampler states
		if( vars.samplers.Num() > 1 )
		{
			if(!bHadAny) {
			tw.NewLine();
			}

			tw.Putf("// Make sure that sampler states are laid out contiguously in memory (so we can Set() them in one call)\n");
			wrScope	cbScope(tw);

			tw.Putf("ID3D11SamplerState** samplerStatesArray = c_cast(ID3D11SamplerState**) &%s::%s;\n",
				structName,
				vars.samplers.GetFirst().name.ToChars());

			for( UINT iSamplerState = 0;
				iSamplerState < vars.samplers.Num();
				iSamplerState++ )
			{
				const wrShaderSamplerState& ss = vars.samplers[ iSamplerState ];

				tw.Putf("ensure( samplerStatesArray + %u == &%s::%s );\n"
					,iSamplerState
					,structName
					,ss.name.ToChars()
				);
			}

			bHadAny = true;
		}// Sampler states




		// Shader resources
		if( vars.resources.Num() > 1 )
		{
			if(!bHadAny) {
				tw.NewLine();
			}

			tw.Putf("// Make sure that shader resources are laid out contiguously in memory (so we can Set() them in one call)\n");
			wrScope	cbScope(tw);

			tw.Putf("ID3D11ShaderResourceView** shaderResourcesArray = c_cast(ID3D11ShaderResourceView**) &%s::%s;\n",
				structName,
				vars.resources.GetFirst().name.ToChars());

			for( UINT iShaderResource = 0;
				iShaderResource < vars.resources.Num();
				iShaderResource++ )
			{
				const wrShaderResource& res = vars.resources[ iShaderResource ];

				tw.Putf("ensure( shaderResourcesArray + %u == &%s::%s );\n"
					,iShaderResource
					,structName
					,res.name.ToChars()
				);
			}

			bHadAny = true;
		}//Shader resources

		if( bHadAny )
		{
			tw.NewLine();
		}		
	}

private:
	PREVENT_COPY(HLSLTranslator);
};

//===========================================================================

void emit_hlsl_line_num_info( const wrSourceElement& elem, MyTextWriter& tw)
{
	tw.Putf(
		"#line %d \"%s\"\n",
		elem.pos.line,
		elem.pos.file.ToChars()
		);
}

//NOTE: this must be done globally, after generating shader code!
// we need to process it and replace clear, understandable names
// with opaque, inscrutable ones (strip comments, etc.).
//
#define OBFUSCATE_SHADER_CODE	0

wrName GetSourceElemName( const wrSourceElement& elem )
{
#if OBFUSCATE_SHADER_CODE
	return elem.GetUniqueName();
#else
	return elem.name;
#endif
}



StackString	wrCBVar::GenDeclCodeString() const
{
	StackString	buf;

	if( arrayDim.IsEmpty() )
	{
		buf.Format("%s %s;",
			typeName.ToChars(),
			GetSourceElemName(*this).ToChars()
			);
	}
	else
	{
		buf.Format("%s %s[ %s ];",
			typeName.ToChars(),
			GetSourceElemName(*this).ToChars(),
			arrayDim.ToChars()
			);
	}
	return buf;
}


void wrShaderVariables::generate_hlsl_code_for_binding( TextBuffer& code, const Options& config ) const
{
	TextBuffer::OStream codeWriter = code.GetOStream();

	MyTextWriter	tw(codeWriter);

	//Constant buffers
	for( UINT iCB = 0;
		iCB < this->constantBuffers.Num();
		iCB++ )
	{
		const wrShaderConstantBuffer& cb = this->constantBuffers[ iCB ];
		Assert(cb.iRegister!=INDEX_NONE);

		if( config.bEmitHLSLComments )
		{
			emit_hlsl_line_num_info(cb,tw);
		}

		tw.Putf("cbuffer %s : register(b%u)\n"
			,GetSourceElemName(cb).ToChars()
			,cb.iRegister
			);

		wrScope	cbScope(tw,Scope_Semicolon);

		for( UINT iCBElement = 0;
			iCBElement < cb.elements.Num();
			iCBElement++ )
		{
			const wrCBVar& cbVar = cb.elements[iCBElement];

			tw.Putf("%s\n",
				cbVar.GenDeclCodeString().ToChars()
				);
		}
	}


	// Sampler states
	for( UINT iSamplerState = 0;
		iSamplerState < this->samplers.Num();
		iSamplerState++ )
	{
		const wrShaderSamplerState& ss = this->samplers[ iSamplerState ];
		Assert(ss.iRegister!=INDEX_NONE);

		if( config.bEmitHLSLComments )
		{
			emit_hlsl_line_num_info(ss,tw);
		}

		const char* samplerStateOrCmpState = "?";

		if(ss.bIsSamplerCmpState)
			samplerStateOrCmpState = "SamplerComparisonState";
		else
			samplerStateOrCmpState="SamplerState";

		tw.Putf("%s %s : register(s%u);\n"
			,samplerStateOrCmpState
			,GetSourceElemName(ss).ToChars()
			,ss.iRegister
			);
	}// Sampler states




	// Shader resources

	for( UINT iShaderResource = 0;
		iShaderResource < this->resources.Num();
		iShaderResource++ )
	{
		const wrShaderResource& res = this->resources[ iShaderResource ];
		Assert(res.iRegister!=INDEX_NONE);

		if( config.bEmitHLSLComments )
		{
			emit_hlsl_line_num_info(res,tw);
		}

		tw.Putf("Texture2D %s : register(t%u);\n"
			,GetSourceElemName(res).ToChars()
			,res.iRegister
			);
	}//Shader resources
}

/*
=============================================================================
*/
void Translate( const Options& config, const ParseResults& input )
{
	g_bEmitComments = config.bEmitComments;

	HLSLTranslator	translator;
	// remember, const is a virus
	ParseResults& nonConstInput = const_cast< ParseResults& >( input );
	translator.Translate( config, nonConstInput );
}

