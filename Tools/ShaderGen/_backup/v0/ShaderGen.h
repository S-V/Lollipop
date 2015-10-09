/*
=============================================================================
	File:	ShaderGen.h
	Desc:	HLSL wrapper generator, public header file.
=============================================================================
*/
#ifndef __SHADER_GEN_H__
#define __SHADER_GEN_H__


class Options
{
public:
	OSPathName		inputFolder;
	OSPathName		outputFolder;

	TList< OSFileName >	srcFiles;

	// PCH file included at the start of every source file, can be empty
	// should be enclosed into "" or <>
	OSFileName		precompiledHeader;

	// header files included into every source file;
	// should be enclosed into "" or <>
	TList< OSFileName >	includedHeaders;

	//TPtr< Report >	report;

	bool	bDebugMode;

public:
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

/*
-----------------------------------------------------------------------------
	ShaderGen
-----------------------------------------------------------------------------
*/
class ShaderGen {
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

	static ShaderGen* Create( const Options& options );
	static void Destroy( ShaderGen*& p );

	virtual void GenerateShaderLib() = 0;

protected:
	// haha, visual c++ 9.0 compiles this
	virtual ~ShaderGen() = 0 {};
};























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
