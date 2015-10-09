

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__



#include "Scanner.h"



class Parser;

class Errors {
public:
	int count;			// number of errors detected
	Parser* parser;

	Errors( Parser* p );
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_identifier=1,
		_float_constant=2,
		_integer_constant=3,
		_char_constant=4,
		_string_constant=5,
		_auto=6,
		_case=7,
		_cbuffer=8,
		_char=9,
		_const=10,
		_default=11,
		_double=12,
		_enum=13,
		_extern=14,
		_float=15,
		_int=16,
		_long=17,
		_register=18,
		_short=19,
		_signed=20,
		_static=21,
		_struct=22,
		_typedef=23,
		_union=24,
		_unsigned=25,
		_void=26,
		_volatile=27,
		_comma=28,
		_semicolon=29,
		_colon=30,
		_star=31,
		_lpar=32,
		_rpar=33,
		_lbrack=34,
		_rbrack=35,
		_lbrace=36,
		_rbrace=37,
		_ellipsis=38,
		_ppDefine=90,
		_ppUndef=91,
		_ppIf=92,
		_ppElif=93,
		_ppElse=94,
		_ppEndif=95,
		_ppInclude=96
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();	// reads the next token (and updates lookahead token)
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token
	
private:
	const ParseFileInput &	input;
	ParseFileOutput &		output;
	UINT	numWarnings;
	UINT	numErrors;

public:
	wrSourceFile& ParsedFile() { return output.fileData; }

	wrLocation Location() const { return scanner->CurrLocation(); }
	const wrBuffer& TokenBuf() const { return t->text; }
	const char* TokenStr() const { return t->text.ToChars(); }

	UINT Line() const { return scanner->CurrLocation().line; }
	UINT Column() const { return scanner->CurrLocation().column; }

	const char* SourceFileName() const { return input.pathToSrcFile.ToChars(); }

	void Message( const char* msg )
	{
		mxPutf("%s: %s\n",SourceFileName(),msg);
	}
	void Warning( const char* fmt, ... )
	{
		++numWarnings;
		char	buffer[ MAX_STRING_CHARS ];
		MX_GET_VARARGS_ANSI( buffer, fmt );
		Warning( Line(), Column(), buffer );
	}
	void Warning( int line, int column, const char* msg )
	{
		++numWarnings;
		mxPutf("%s(%d,%d): Warning: %s\n",
			SourceFileName(),line,column,msg);
	}
	void Error( const char* fmt, ... )
	{
		++numErrors;
		char	buffer[ MAX_STRING_CHARS ];
		MX_GET_VARARGS_ANSI( buffer, fmt );
		Error( Line(), Column(), buffer );
		mxBeep(200);
	}
	void Error( int line, int column, const char* msg )
	{
		++numErrors;
		mxPutf("%s(%d,%d): Error: %s\n",
			SourceFileName(),line,column,msg);
		mxBeep(200);
	}
	void SyntaxError( int line, int column, const char* msg )
	{
		++numErrors;
		mxPutf("%s(%d,%d): Syntax error: %s\n",
			SourceFileName(),line,column,msg);
		mxBeep(200);
	}

	UINT NumWarnings() const { return numWarnings; }
	UINT NumErrors() const { return numErrors; }


	U32 TokenToU32( U32 minValue = 0, U32 maxValue = U32(-1) )
	{
		U32 v = ParseInteger( TokenStr() );
		if( (v < minValue) || (v > maxValue) ) {
			Error("expected an integer in range [%d..%d]",minValue,maxValue);
			return 0;
		}
		return v;
	}
	F32 TokenToF32( F32 minValue = MIN_FLOAT_32, F32 maxValue = MAX_FLOAT_32 )
	{
		F32 v = ParseFloat( TokenStr() );
		if( (v < minValue) || (v > maxValue) ) {
			Error("expected a floating-point value in range [%f..%f]",minValue,maxValue);
			return 0.0f;
		}
		return v;
	}

#define SETPOS(x)	(x).pos = this->Location()

//---------------------------------------------------------------------------
//	Scanner specification
//---------------------------------------------------------------------------

// By default, Coco/R generates scanners that are case sensitive.
// If this is not desired, one has to write IGNORECASE at the beginning of the scanner specification.
//
//IGNORECASE




	Parser( Scanner *scanner, const ParseFileInput& input, ParseFileOutput &output );
	~Parser();
	void SemErr(const wchar_t* msg);

	void HLSL();
	void declaration();
	void render_target_declaration();
	void sampler_state_declaration();
	void depth_stencil_state_declaration();
	void rasterizer_state_declaration();
	void blend_state_declaration();
	void state_block_declaration();
	void shader_program_declaration();
	void boolean_constant();
	void bit_mask();
	void rgba_color(FColor & rgba );
	void shader_inputs();
	void shader_variable_decl();
	void constant_buffer_decl();
	void sampler_state_decl();
	void shader_resource_decl();
	void constant_buffer_register();
	void constant_buffer_element();
	void type_name();

	void Parse();

}; // end Parser



#endif // !defined(COCO_PARSER_H__)

