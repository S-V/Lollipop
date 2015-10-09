/*
=============================================================================
	File:	HLSLWrapperGenerator.cpp
	Desc:
=============================================================================
*/
#include "stdafx.h"
#pragma hdrstop
#include "ShaderGen.h"

#include "Parser.h"

/*================================
		IErrorReport
================================*/

IErrorReport::~IErrorReport() {}

IErrorReport* errorLog = nil;


/*================================
		MyErrorReport
================================*/

class MyErrorReport : public IErrorReport
{
public:
	MyErrorReport( const char* srcFileName );

	virtual void Message( const wchar_t* msg )
	{
		mxUnicodeToAnsi( mTempBuffer, msg, NUMBER_OF(mTempBuffer) );
		mxPutf("%s: %s\n",mSrcFileName.ToChars(),mTempBuffer);
	}
	virtual void SyntaxError( int line, int column, const wchar_t* msg )
	{
		mxUnicodeToAnsi( mTempBuffer, msg, NUMBER_OF(mTempBuffer) );
		mxPutf("%s(%d,%d): Syntax error: %s\n",
			mSrcFileName.ToChars(),line,column,mTempBuffer);
	}
	virtual void Error( int line, int column, const wchar_t* msg )
	{
		mxUnicodeToAnsi( mTempBuffer, msg, NUMBER_OF(mTempBuffer) );
		mxPutf("%s(%d,%d): Error: %s\n",
			mSrcFileName.ToChars(),line,column,mTempBuffer);
	}
	virtual void Warning( int line, int column, const wchar_t* msg )
	{
		mxUnicodeToAnsi( mTempBuffer, msg, NUMBER_OF(mTempBuffer) );
		mxPutf("%s(%d,%d): Warning: %s\n",
			mSrcFileName.ToChars(),line,column,mTempBuffer);
	}

private:
	OSFileName	mSrcFileName;
	char		mTempBuffer[ MAX_STRING_CHARS ];
};

MyErrorReport::MyErrorReport( const char* srcFileName )
	: mSrcFileName( _InitSlow, srcFileName )
{
}



/*================================
	HLSLWrapperGenerator
================================*/


HLSLWrapperGenerator::HLSLWrapperGenerator()
{

}
HLSLWrapperGenerator::~HLSLWrapperGenerator()
{

}

void HLSLWrapperGenerator::Setup( const Options& options )
{
	Assert(!config.isOk());

	config = options;
	FS_FixSlashes( config.inputFolder );
	FS_FixSlashes( config.outputFolder );

	Assert(config.isOk());
}

void HLSLWrapperGenerator::Clear()
{
	config.inputFolder.Clear();
	config.outputFolder.Clear();
	config.srcFiles.Clear();

	root.~wrShaderLibrary();
	new(&root) wrShaderLibrary();
}

void HLSLWrapperGenerator::GenerateShaderLib()
{
	mxPutf("Generating shader library...\n");

	const UINT numSrcFiles = config.srcFiles.Num();
	if( !numSrcFiles ) {
		mxPutf("Generating shader library...\n");
		return;
	}

	WriteDirSaver	writeDirSaver;

	IOServer::Get().SetWriteDirectory( config.outputFolder.ToChars() );

	for( UINT iSrcFile = 0; iSrcFile < numSrcFiles; iSrcFile++ )
	{
		const OSFileName & srcFileName( config.srcFiles[ iSrcFile ] );

		root.sourceFiles.Add().name = srcFileName;

		OSPathName	pathToSrcFile( config.inputFolder );
		pathToSrcFile += srcFileName.ToConstCharPtr();

		wrParseFileInput	input;
		wrParseFileOutput	output;
		Parse( input, output );
	}

	HLSLTranslator	translator;
	translator.Translate( root );
}

void HLSLWrapperGenerator::Parse( const wrParseFileInput& input, wrParseFileOutput &output )
{
	MyErrorReport	myErrorReport( input.pathToSrcFile.ToChars() );

	errorLog = &myErrorReport;

	Scanner		scanner( MX_TO_UNICODE(input.pathToSrcFile.ToChars()) );
	Parser		p( &scanner );
	parser = &p;

	p.Parse();

	output.numErrors = p.errors->count;

	mxPutf("Parsed '%s' (%d error(s)).\n",input.pathToSrcFile.ToChars(),output.numErrors);

	parser = nil;
}

/*================================
		HLSLTranslator
================================*/

HLSLTranslator::HLSLTranslator()
{}

void HLSLTranslator::Translate( const wrShaderLibrary& input )
{
	for( UINT iSrcFile = 0; iSrcFile < input.sourceFiles.Num(); iSrcFile++ )
	{
		const wrSourceFile& srcFile = input.sourceFiles[ iSrcFile ];
		Translate( srcFile );
	}
}

OSFileName GetDestFileName( const OSFileName& srcFileName, ConstCharPtr& extension )
{
	OSFileName	destFileName( srcFileName );
	destFileName.StripFileExtension();
	destFileName += extension;
	return destFileName;
}

wrName Get_DXGI_Format( const wrName& origFormat )
{
	if( origFormat.IsEmpty() ) {
		return wrName("DXGI_FORMAT_UNKNOWN");
	}
	wrName	dxgiFormat("DXGI_FORMAT_");
	dxgiFormat += origFormat;
	return dxgiFormat;
}

wrBuffer Get_RT_Size( const wrRTSize& size, ConstCharPtr& relativeToWhat = MXC("backbufferSize") )
{
	wrBuffer	sizeStr;
	if ( size.sizeMode == Size_Absolute )
	{
		sizeStr.Format( "%u", size.absoluteSize );
	}
	else if ( size.sizeMode == Size_Relative )
	{
		if( size.relativeSize == 1.0f ) {
			sizeStr = relativeToWhat;
		} else {
			sizeStr.Format( "%s * %ff", relativeToWhat.ToChars(), size.relativeSize );
		}
	} 
	else
	{
		Unimplemented;
		sizeStr = "?";
	}
	return sizeStr;
}

void emit_source_location_comment( const wrLocation& pos, mxTextWriter & tw )
{
	tw << MXC("// ") << pos.ToStr() << MXC("\n");
}
void Emit_source_element_comment( const wrSourceElement& elem, mxTextWriter & tw )
{
	tw << MXC("// ") << elem.name << MXC("\n");
	emit_source_location_comment( elem.pos, tw );
	if( !elem.info.IsEmpty() ) {
		tw << MXC("// ") << elem.info.ToChars() << MXC("\n");
	}
}

void Emit_initialize_render_target( const wrRenderTarget& renderTarget, mxTextWriter & tw )
{
	tw << MXC("\n");
	Emit_source_element_comment( renderTarget, tw );
	tw << MXC("{\n");

	wrBuffer	renderTargetFormat = Get_DXGI_Format( renderTarget.format );

	tw	<< MXC("texDesc.Format				= ") << renderTargetFormat << MXC(";\n")
		<< MXC("texDesc.Width				= ") << Get_RT_Size( renderTarget.sizeX, MXC("creationParams.windowWidth") ).ToConstCharPtr() << MXC(";\n")
		<< MXC("texDesc.Height				= ") << Get_RT_Size( renderTarget.sizeY, MXC("creationParams.windowHeight") ).ToConstCharPtr() << MXC(";\n")
		<< MXC("texDesc.MipLevels			= 1;\n")
		<< MXC("texDesc.ArraySize			= 1;\n")
		<< MXC("texDesc.SampleDesc.Count	= 1;\n")
		<< MXC("texDesc.SampleDesc.Quality	= 0;\n")
		<< MXC("texDesc.Usage				= D3D11_USAGE_DEFAULT;\n")
		<< MXC("texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;\n")
		<< MXC("texDesc.CPUAccessFlags		= 0;\n")
		<< MXC("texDesc.MiscFlags			= 0;\n")
		<< MXC("\n")
		;
	tw	<< MXC("rtvDesc.Format				= ") << renderTargetFormat << MXC(";\n")
		<< MXC("rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;\n")
		<< MXC("rtvDesc.Texture2D.MipSlice	= 0;\n")
		<< MXC("\n")
		;
	tw	<< MXC("srvDesc.Format						= ") << renderTargetFormat << MXC(";\n")
		<< MXC("srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;\n")
		<< MXC("srvDesc.Texture2D.MipLevels			= 1;\n")
		<< MXC("srvDesc.Texture2D.MostDetailedMip	= 0;\n")
		;
	tw << MXC("}\n");
}
void Emit_initialize_render_targets( const TList< wrRenderTarget >& renderTargets, mxTextWriter & tw )
{
	tw	<< MXC("void SetupRenderTargets( const SHADER_LIB_CREATE& creationParams )\n{\n")
		<< MXC("D3D11_TEXTURE2D_DESC texDesc;\n")
		<< MXC("D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;\n")
		<< MXC("D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;\n")
		<< MXC("ZERO_OUT( texDesc );\n")
		<< MXC("ZERO_OUT( rtvDesc );\n")
		<< MXC("ZERO_OUT( srvDesc );\n")
		;
	for( UINT iRenderTarget = 0;
		iRenderTarget < renderTargets.Num();
		iRenderTarget++ )
	{
		const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];
		Emit_initialize_render_target( renderTarget, tw );
	}
	tw << MXC("\n}//SetupRenderTargets\n");
}

wrBuffer ToUpper( const wrName& filter )
{
	wrBuffer	buf( filter );
	buf.ToUpper();
	return buf;
}

wrBuffer Get_Filter( const wrName& filter )
{
	if( filter.IsEmpty() ) {
		return wrBuffer("D3D11_FILTER_MIN_MAG_MIP_LINEAR");
	}
	wrBuffer	buf("D3D11_FILTER_");
	buf += ToUpper( filter );
	return buf;
}
wrBuffer GetTextureAddressMode( const wrName& addrMode )
{
	if( addrMode.IsEmpty() ) {
		return wrBuffer("D3D11_TEXTURE_ADDRESS_CLAMP");
	}
	wrBuffer	buf("D3D11_TEXTURE_ADDRESS_");
	buf += ToUpper( addrMode );
	return buf;
}
void Emit_initialize_sampler_state( const wrSamplerState& samplerState, mxTextWriter & tw ) 
{
	tw << MXC("\n");
	Emit_source_element_comment( samplerState, tw );
	tw << MXC("{\n");

	tw	<< MXC("samplerDesc.Filter			= ") << Get_Filter( samplerState.filter ) << MXC(";\n")
		<< MXC("samplerDesc.AddressU		= ") << GetTextureAddressMode( samplerState.addressU ) << MXC(";\n")
		<< MXC("samplerDesc.AddressV		= ") << GetTextureAddressMode( samplerState.addressV ) << MXC(";\n")
		<< MXC("samplerDesc.AddressW		= ") << GetTextureAddressMode( samplerState.addressW ) << MXC(";\n")
		<< MXC("samplerDesc.MipLODBias		= 0;\n")
		<< MXC("samplerDesc.MaxAnisotropy	= 1;\n")
		<< MXC("samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;\n")
		<< MXC("samplerDesc.BorderColor[0]	= 1.0f;\n")
		<< MXC("samplerDesc.BorderColor[1]	= 1.0f;\n")
		<< MXC("samplerDesc.BorderColor[2]	= 1.0f;\n")
		<< MXC("samplerDesc.BorderColor[3]	= 1.0f;\n")
		<< MXC("samplerDesc.MinLOD			= -FLT_MAX;\n")
		<< MXC("samplerDesc.MaxLOD			= +FLT_MAX;\n")
		<< MXC("\n")
		;
	tw << MXC("}\n");
}

void Emit_initialize_sampler_states( const TList< wrSamplerState >& samplerStates, mxTextWriter & tw )
{
	tw	<< MXC("void SetupSamplerStates( const SHADER_LIB_CREATE& creationParams )\n{\n")
		<< MXC("D3D11_SAMPLER_DESC samplerDesc;\n")
		<< MXC("ZERO_OUT( samplerDesc );\n")
		;
	for( UINT iSamplerState = 0;
		iSamplerState < samplerStates.Num();
		iSamplerState++ )
	{
		const wrSamplerState& samplerState = samplerStates[ iSamplerState ];
		Emit_initialize_sampler_state( samplerState, tw );
	}
	tw << MXC("\n}//SetupSamplerStates\n");
}
wrBuffer get_boolean_constant_string( const wrName& s )
{
	Assert( !s.IsEmpty() );

	wrBuffer	tmp = ToUpper( s );

	wrBuffer	buf;
	if( tmp == "0" || tmp == "FALSE" ) {
		buf = "FALSE";
	}
	else if( tmp == "1" || tmp == "TRUE" ) {
		buf = "TRUE";
	}
	else {
		Unimplemented;
	}
	return buf;
}
wrBuffer get_depth_write_mask_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_DEPTH_WRITE_MASK_ALL");
	}
	wrBuffer	buf("D3D11_DEPTH_WRITE_MASK_");
	buf += ToUpper( s );
	return buf;
}
wrBuffer get_depth_func_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_COMPARISON_LESS");
	}
	wrBuffer	buf("D3D11_COMPARISON_");
	buf += ToUpper( s );
	return buf;
}
void GenDepthStencilStateInit( const wrDepthStencilState& depthStencilState, mxTextWriter & tw )
{
	tw << MXC("\n");
	Emit_source_element_comment( depthStencilState, tw );
	tw << MXC("{\n");

	tw	<< MXC("depthStencilDesc.DepthEnable	= ") << get_boolean_constant_string( depthStencilState.DepthEnable ) << MXC(";\n")
		<< MXC("depthStencilDesc.DepthWriteMask	= ") << get_depth_write_mask_string( depthStencilState.DepthWriteMask ) << MXC(";\n")
		<< MXC("depthStencilDesc.DepthFunc		= ") << get_depth_func_string( depthStencilState.DepthFunc ) << MXC(";\n")
		
		<< MXC("depthStencilDesc.StencilEnable		= ") << get_boolean_constant_string( depthStencilState.StencilEnable ) << MXC(";\n")
		<< MXC("depthStencilDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;\n")
		<< MXC("depthStencilDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;\n")
		
		<< MXC("depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;\n")
		
		<< MXC("depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;\n")
		<< MXC("depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;\n")

		<< MXC("\n")
		;
	tw << MXC("}\n");
}
void emit_initialize_depth_stencil_states( const TList< wrDepthStencilState >& depthStencilStates, mxTextWriter & tw )
{
	tw	<< MXC("void SetupDepthStencilStates( const SHADER_LIB_CREATE& creationParams )\n{\n")
		<< MXC("D3D11_DEPTH_STENCIL_DESC depthStencilDesc;\n")
		<< MXC("ZERO_OUT( depthStencilDesc );\n")
		;
	for( UINT iDepthStencilState = 0;
		iDepthStencilState < depthStencilStates.Num();
		iDepthStencilState++ )
	{
		const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];
		GenDepthStencilStateInit( depthStencilState, tw );
	}
	tw << MXC("\n}//SetupDepthStencilStates\n");
}
void emit_line_separator_comment( mxTextWriter & tw )
{
	tw << MXC("//===========================================================================\n");
}

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

#define WR_VERSION_STRING	MXC("0.0")
#define WR_NAMESPACE		MXC("DX11")
#define WR_HEADER_FILE_EXT	MXC(".hxx")
#define WR_SOURCE_FILE_EXT	MXC(".cxx")

void emit_depth_stencil_states_declarations( const TList< wrDepthStencilState >& depthStencilStates, mxTextWriter & tw )
{
	for( UINT iDepthStencilState = 0;
		iDepthStencilState < depthStencilStates.Num();
		iDepthStencilState++ )
	{
		const wrDepthStencilState& depthStencilState = depthStencilStates[ iDepthStencilState ];
		tw	<< MXC("extern DepthStencilState depthStencilState_") << depthStencilState.name << MXC(";");
		emit_source_location_comment( depthStencilState.pos, tw );
	}
}

void emit_render_target_declarations( const TList< wrRenderTarget >& renderTargets, mxTextWriter & tw )
{
	for( UINT iRenderTarget = 0;
		iRenderTarget < renderTargets.Num();
		iRenderTarget++ )
	{
		const wrRenderTarget& renderTarget = renderTargets[ iRenderTarget ];
		tw	<< MXC("extern RenderTarget renderTarget_") << renderTarget.name << MXC(";");
		emit_source_location_comment( renderTarget.pos, tw );
	}
}
void emit_sampler_state_declarations( const TList< wrSamplerState >& samplerStates, mxTextWriter & tw )
{
	for( UINT iSamplerState = 0;
		iSamplerState < samplerStates.Num();
		iSamplerState++ )
	{
		const wrSamplerState& samplerState = samplerStates[ iSamplerState ];
		tw	<< MXC("extern SamplerState samplerState_") << samplerState.name << MXC(";");
		emit_source_location_comment( samplerState.pos, tw );
	}
}
void emit_declarations( const wrSourceFile& srcFile )
{
	OSFileName		fileName = GetDestFileName( srcFile.name, WR_HEADER_FILE_EXT );
	FileWriter		outputStream( fileName.ToChars() );
	mxTextWriter	tw( &outputStream );

	tw	<< MXC("/* This ALWAYS GENERATED file contains the definitions for the interfaces */\n")
		<< MXC("/* File created by HLSL wrapper generator version ") << WR_VERSION_STRING << MXC(" ") << GetCurrentDateTimeString() << MXC(" */\n")
		;
	tw	<< MXC("namespace ") << WR_NAMESPACE << MXC("\n{\n");

	tw	<< MXC("// Render targets\n");
	emit_render_target_declarations( srcFile.renderTargets, tw );

	tw	<< MXC("// Sampler states\n");
	emit_sampler_state_declarations( srcFile.samplerStates, tw );

	tw	<< MXC("// Depth-stencil states\n");
	emit_depth_stencil_states_declarations( srcFile.depthStencilStates, tw );

	tw << MXC("\n}//namespace ") << WR_NAMESPACE << MXC("\n");
}
void emit_definitions( const wrSourceFile& srcFile )
{
	OSFileName		fileName = GetDestFileName( srcFile.name, WR_SOURCE_FILE_EXT );
	FileWriter		outputStream( fileName.ToChars() );
	mxTextWriter	tw( &outputStream );

	tw	<< MXC("/* This ALWAYS GENERATED file contains the implementation for the interfaces */\n")
		<< MXC("/* File created by HLSL wrapper generator version ") << WR_VERSION_STRING << MXC(" ") << GetCurrentDateTimeString() << MXC(" */\n")
		;
	tw	<< MXC("namespace ") << WR_NAMESPACE << MXC("\n{\n");

	// Initialization order:
	// 1) Render targets
	// 2) State objects
	// 3) Shaders
	// 4) Input layouts
	// 5) Everything else

/*
	TList< const wrRenderTarget* >	sortedRTs;
	for( UINT iRenderTarget = 0;
		iRenderTarget < srcFile.renderTargets.Num();
		iRenderTarget++ )
	{
		sortedRTs.Add( &srcFile.renderTargets[ iRenderTarget ] );
	}

	// Sort render targets by size.
	InsertionSort< const wrRenderTarget*, wrRenderTarget >( sortedRTs.ToPtr(), 0, sortedRTs.Num()-1 );
*/
	Emit_initialize_render_targets( srcFile.renderTargets, tw );
	emit_line_separator_comment( tw );
	Emit_initialize_sampler_states( srcFile.samplerStates, tw );
	emit_line_separator_comment( tw );
	emit_initialize_depth_stencil_states( srcFile.depthStencilStates, tw );


	tw	<< MXC("void SetupGPUResources( const SHADER_LIB_CREATE& creationParams )\n{\n")
		;
	tw	<< MXC("\n}//SetupGPUResources\n")
		<< MXC("SetupRenderTargets( creationParams );\n")
		<< MXC("SetupSamplerStates( creationParams );\n")
		<< MXC("SetupDepthStencilStates( creationParams );\n")
		<< MXC("SetupRasterizerStates( creationParams );\n")
		<< MXC("SetupBlendStates( creationParams );\n")
		;
	tw	<< MXC("\n}//SetupGPUResources\n")
		;

	tw	<< MXC("\n}//namespace ") << WR_NAMESPACE << MXC("\n");
}


void HLSLTranslator::Translate( const wrSourceFile& srcFile )
{
	emit_declarations( srcFile );
	emit_definitions( srcFile );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
