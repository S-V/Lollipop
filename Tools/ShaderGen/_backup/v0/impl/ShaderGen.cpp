/*
=============================================================================
	File:	ShaderGen.cpp
	Desc:	HLSL wrapper generator, implementation.
=============================================================================
*/
#include "stdafx.h"
#pragma hdrstop
#include "Common.h"

/*
=============================================================================
	HLSLShaderGen
=============================================================================
*/
class HLSLShaderGen : public ShaderGen
{
	Options			config;

public:
	HLSLShaderGen( const Options& options )
	{
		Assert(options.isOk());
		config = options;
		FS_FixSlashes( config.inputFolder );
		FS_FixSlashes( config.outputFolder );
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

		WriteDirSaver	writeDirSaver;

		IOServer::Get().SetWriteDirectory( config.outputFolder.ToChars() );

		ParseResults	parseResults;

		for( UINT iSrcFile = 0; iSrcFile < numSrcFiles; iSrcFile++ )
		{
			const OSFileName & srcFileName( config.srcFiles[ iSrcFile ] );

			ParseFileInput	input;
			input.pathToSrcFile = config.inputFolder;
			input.pathToSrcFile += srcFileName.ToConstCharPtr();

			ParseFileOutput &	output = parseResults.parsedFiles.Add();
			output.fileData.name = srcFileName;
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
ShaderGen* ShaderGen::Create( const Options& options )
{
	return new HLSLShaderGen( options );
}

void ShaderGen::Destroy( ShaderGen*& p )
{
	delete p;
	p = nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
