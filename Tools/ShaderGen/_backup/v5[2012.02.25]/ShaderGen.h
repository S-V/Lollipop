/*
=============================================================================
	File:	ShaderGen.h
	Desc:	HLSL wrapper generator, public header file.
=============================================================================
*/
#ifndef __SHADER_GEN_H__
#define __SHADER_GEN_H__

#include <Base/Text/TextBuffer.h>

namespace ShaderGen
{

class Options
{
public:
	// input file names
	TList< OSPathName >	srcFiles;

	// output directory
	OSPathName		outputFolderHLSL;	// generated shader source files
	OSPathName		outputFolderCPP;	// generated C++ source files

	StackString	cppNamespace;
	OSFileName	headerFileName;	// e.g. "Main.hxx"
	OSFileName	sourceFileName;	// e.g. "Main.cxx"

	// text to insert at the start of each header file
	TextBuffer	headerFilePrefix;

	// text to insert at the start of each source file
	TextBuffer	sourceFilePrefix;

	// emit debug information
	BOOL	bDebugMode;

	BOOL	bEmitVertexFormatsDecl;	// regenerate header with vertex declarations
	BOOL	bEmitMainHeader;	// regenerate header with render targets and states
	// shader programs are always (re)generated

	// e.g.: "This ALWAYS GENERATED file contains..."
	BOOL	bEmitPreamble;

	// e.g.: C:/Dev/Renderer/GPU/script/r_render_states.fx(7,13)
	BOOL	bEmitComments;

	// e.g.: #line 63 "E:/_/Engine/Development/SourceCode/Renderer/GPU/source/p_screen_shader.fx"
	BOOL	bEmitHLSLComments;

	// aggressive optimization: find unreferenced variables and remove them from constant buffers, etc.
	bool	bStripUnusedVars;

public:
	Options()
	{
		cppNamespace = MXC("GPU");
		headerFileName = MXC("Main.hxx");
		sourceFileName = MXC("Main.cxx");

		bDebugMode = false;

		bEmitVertexFormatsDecl = true;
		bEmitMainHeader = true;

		bEmitPreamble = true;
		bEmitComments = true;
		bEmitHLSLComments = true;

		bStripUnusedVars = FALSE;
	}
	bool isOk() const
	{
		return 1
			&& !cppNamespace.IsEmpty()
			&& !headerFileName.IsEmpty()
			&& !sourceFileName.IsEmpty()

			&& !srcFiles.IsEmpty()
			&& !outputFolderHLSL.IsEmpty()
			&& !outputFolderCPP.IsEmpty()
			&& !srcFiles.IsEmpty()
			;
	}
	template< class S >
	friend S& operator & ( S & serializer, Options & o )
	{
		return serializer
			& o.srcFiles

			& o.outputFolderHLSL
			& o.outputFolderCPP

			& o.cppNamespace
			& o.headerFileName
			& o.sourceFileName

			& o.headerFilePrefix
			& o.sourceFilePrefix

			& o.bDebugMode

			& o.bEmitVertexFormatsDecl
			& o.bEmitMainHeader

			& o.bEmitPreamble
			& o.bEmitComments
			& o.bEmitHLSLComments
			;
	}
};

/*
-----------------------------------------------------------------------------
	ShaderGen
-----------------------------------------------------------------------------
*/
class Generator {
public:

	class Report
	{
	public:
		virtual void Message( const char* msg ) = 0;
		virtual void Warning( int line, int column, const char* msg ) = 0;
		virtual void Error( int line, int column, const char* msg ) = 0;
		virtual void SyntaxError( int line, int column, const char* msg ) = 0;

		virtual ~Report() {}
	};

	static Generator* Create( const Options& options );
	static void Destroy( Generator*& p );

	virtual void GenerateShaderLib() = 0;

protected:
	virtual ~Generator() = 0 {};
};

}//namespace ShaderGen





















template< typename TYPE = int >
struct TInteger 
{
	TYPE	value;

	FORCEINLINE TInteger()
	{}
	FORCEINLINE TInteger( TYPE i )
		: value( i )
	{}
	FORCEINLINE operator TYPE () const
	{
		return value;
	}
	FORCEINLINE operator TYPE& ()
	{
		return value;
	}
	FORCEINLINE void operator = ( TYPE i )
	{
		value = i;
	}
	FORCEINLINE bool operator == ( TYPE i ) const
	{
		return (value == i);
	}
	FORCEINLINE bool operator != ( TYPE i ) const
	{
		return (value != i);
	}
	// TODO: range checks. checking policy?
};

template< typename TYPE = int >
struct TInteger_InitZero : TInteger< TYPE >
{
	TInteger_InitZero()
		: TInteger( (TYPE)0 )
	{}
};







#endif // __SHADER_GEN_H__
