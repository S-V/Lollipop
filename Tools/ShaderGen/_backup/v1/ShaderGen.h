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
	OSPathName		outputFolder;


	// text to insert at the start of each header file
	TextBuffer	headerFilePrefix;

	// text to insert at the start of each source file
	TextBuffer	sourceFilePrefix;

	// emit debug information
	bool	bDebugMode;

	// e.g.: "This ALWAYS GENERATED file contains..."
	bool	bEmitPreamble;

	// e.g.: C:/Dev/Renderer/GPU/script/r_render_states.fx(7,13)
	bool	bEmitComments;

public:
	Options()
	{
		bDebugMode = false;
		bEmitPreamble = false;
		bEmitComments = false;
	}
	bool isOk() const
	{
		return !srcFiles.IsEmpty()
			&& !outputFolder.IsEmpty()
			&& !srcFiles.IsEmpty()
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
