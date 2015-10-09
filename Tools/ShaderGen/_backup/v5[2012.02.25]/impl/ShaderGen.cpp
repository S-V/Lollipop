/*
=============================================================================
	File:	ShaderGen.cpp
	Desc:	HLSL wrapper generator, implementation.
=============================================================================
*/
#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

static OSFileName GetPureFileName( const OSPathName& path )
{
	String	tmp(path.ToChars());
	String	tmp2;
	tmp.ExtractFileName(tmp2);

	tmp2.StripFileExtension();

	OSFileName	result(_InitSlow,tmp2.ToChars());
	return result;
}
static OSFileName StripFileName( const OSPathName& path )
{
	String	tmp(path.ToChars());
	String	tmp2;
	tmp.ExtractFileName(tmp2);

	OSFileName	result(_InitSlow,tmp2.ToChars());
	return result;
}
/*
=============================================================================
	HLSLShaderGen
=============================================================================
*/
class HLSLShaderGen : public Generator
{
	Options			config;

public:
	HLSLShaderGen( const Options& options )
	{
		Assert(options.isOk());
		config = options;
		//for (int i=0; i<config.srcFiles.Num(); i++)
		//{
		//	FS_FixPathSlashes( config.srcFiles[i] );
		//}
		
		FS_FixPathSlashes( config.outputFolderHLSL );
		FS_FixPathSlashes( config.outputFolderCPP );
		Assert(config.isOk());
	}

	OVERRIDEN void GenerateShaderLib()
	{
		const UINT numSrcFiles = config.srcFiles.Num();
		if( !numSrcFiles ) {
			Msgf("No input files are specified. Quitting...");
			return;
		}

		Msgf("Generating shader library...");


		ParseResults	parseResults;

		for( UINT iSrcFile = 0; iSrcFile < numSrcFiles; iSrcFile++ )
		{
			const OSPathName & srcFileName( config.srcFiles[ iSrcFile ] );

			ParseFileInput	input;
			input.pathToSrcFile = srcFileName;

			ParseFileOutput &	output = parseResults.parsedFiles.Add();
			
			output.fileData.name = config.outputFolderHLSL;
			output.fileData.name += StripFileName(srcFileName);
			
			output.fileData.pureFileName = GetPureFileName(srcFileName);

			ParseFile( input, output );
		}

		Translate( config, parseResults );
	}

	void ParseFile( const ParseFileInput& input, ParseFileOutput &output )
	{
		Msgf("Parsing file '%s'.", input.pathToSrcFile.ToChars());
		const UINT startTimeMS = mxGetMilliseconds();

		Scanner		scanner( MX_TO_UNICODE(input.pathToSrcFile.ToChars()) );
		Parser		p( &scanner, input, output );

		p.Parse();

		output.numErrors = p.NumErrors();
		output.numWarnings = p.NumWarnings();

		mxPutf(
			"Parsed '%s' in %u msec (%u error(s), %u warning(s)).\n",
			input.pathToSrcFile.ToChars(),
			mxGetMilliseconds() - startTimeMS,
			output.numErrors, output.numWarnings
		);
	}

	void Msgf( const char* fmt, ... )
	{
		char	buffer[ MAX_STRING_CHARS ];
		MX_GET_VARARGS_ANSI( buffer, fmt );
		mxPutf("%s\n",buffer);
	}

	PREVENT_COPY(HLSLShaderGen);
};

/*
=============================================================================
	ShaderGen
=============================================================================
*/
Generator* Generator::Create( const Options& options )
{
	return new HLSLShaderGen( options );
}

void Generator::Destroy( Generator*& p )
{
	delete p;
	p = nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
