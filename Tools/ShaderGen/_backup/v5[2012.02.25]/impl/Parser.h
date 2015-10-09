

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
public_internal://private:
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
		_register=15,
		_short=16,
		_signed=17,
		_static=18,
		_struct=19,
		_typedef=20,
		_union=21,
		_unsigned=22,
		_void=23,
		_volatile=24,
		_comma=25,
		_semicolon=26,
		_colon=27,
		_star=28,
		_lpar=29,
		_rpar=30,
		_lbrack=31,
		_rbrack=32,
		_lbrace=33,
		_rbrace=34,
		_ellipsis=35,
		_BIND_HS=36,
		_BIND_DS=37,
		_BIND_TS=38,
		_BIND_VS=39,
		_BIND_GS=40,
		_BIND_PS=41,
		_ppDefine=110,
		_ppUndef=111,
		_ppIf=112,
		_ppElif=113,
		_ppElse=114,
		_ppEndif=115,
		_ppInclude=116
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

public_internal:

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


	U4 TokenToU32( U4 minValue = 0, U4 maxValue = U4(-1) )
	{
		U4 v = ParseInteger( TokenStr() );
		if( (v < minValue) || (v > maxValue) ) {
			Error("expected an integer in range [%d..%d]",minValue,maxValue);
			return 0;
		}
		return v;
	}
	F8 TokenToF32( F8 minValue = 1e-40, F8 maxValue = 1e+40 )
	{
		F8 v = ParseFloat( TokenStr() );
		if( (v < minValue) || (v > maxValue) ) {
			Error("expected a floating-point value in range [%f..%f]",(F4)minValue,(F4)maxValue);
			return 0.0f;
		}
		return v;
	}

	inline void SetPos( wrSourceElement & x ) { x.pos = this->Location(); }

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
	void multi_render_target_declaration();
	void sampler_state_declaration();
	void depth_stencil_state_declaration();
	void rasterizer_state_declaration();
	void blend_state_declaration();
	void state_block_declaration();
	void shader_program_declaration();
	void vertex_declaration();
	void shared_section_declaration();
	void multi_render_target_list();
	void rgba_color(FColor & rgba );
	void boolean_constant();
	void bit_mask();
	void shader_defines(wrShaderProgram & shader );
	void shader_inputs();
	void shader_define(wrShaderProgram & shader );
	void using_shared_sections_decls_list(SharedSectionsList& sharedSections );
	void shader_variables_list_decl(wrShaderVariables & vars );
	void shader_variable_decl(wrShaderVariables & vars );
	void constant_buffer_decl(wrShaderVariables & vars );
	void sampler_state_decl(wrShaderVariables & vars );
	void shader_resource_decl(wrShaderVariables & vars );
	void using_shared_sections_decl(SharedSectionsList& sharedSections );
	void shader_stage_bind_flags(wrRegisterBound & o );
	void constant_buffer_register(wrShaderConstantBuffer & cb );
	void constant_buffer_element(wrShaderConstantBuffer & cb );
	void type_name();
	void vertex_element();
	void shared_variables_declaration(wrSharedSection & sharedSection );
	void shared_code_declaration(wrSharedSection & sharedSection );

	void Parse();

}; // end Parser



#endif // !defined(COCO_PARSER_H__)

