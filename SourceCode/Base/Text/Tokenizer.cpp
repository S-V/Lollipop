/*
=============================================================================
	File:	Tokenizer.cpp
	Desc:	
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>
#include "Tokenizer.h"

MX_NAMESPACE_BEGIN

const UINT ParseUtil::charMap[ NUM_CHARS ] =
{
	/* 000 nul */	0,
	/* 001 soh */	0,
	/* 002 stx */	0,
	/* 003 etx */	0,
	/* 004 eot */	0,
	/* 005 enq */	0,
	/* 006 ack */	0,
	/* 007 bel */	0,
	/* 010 bs  */	0,
	/* 011 ht  */	BLANK,
	/* 012 nl  */	NEWLINE,
	/* 013 vt  */	BLANK,
	/* 014 ff  */	BLANK,
	/* 015 cr  */	0,
	/* 016 so  */	0,
	/* 017 si  */	0,
	/* 020 dle */	0,
	/* 021 dc1 */	0,
	/* 022 dc2 */	0,
	/* 023 dc3 */	0,
	/* 024 dc4 */	0,
	/* 025 nak */	0,
	/* 026 syn */	0,
	/* 027 etb */	0,
	/* 030 can */	0,
	/* 031 em  */	0,
	/* 032 sub */	0,
	/* 033 esc */	0,
	/* 034 fs  */	0,
	/* 035 gs  */	0,
	/* 036 rs  */	0,
	/* 037 us  */	0,
	/* 040 sp  */	BLANK,
	/* 041 !   */	PUNCTUATION,
	/* 042 "   */	PUNCTUATION,
	/* 043 #   */	PUNCTUATION,
	/* 044 $   */	0,
	/* 045 %   */	PUNCTUATION,
	/* 046 &   */	PUNCTUATION,
	/* 047 '   */	PUNCTUATION,
	/* 050 (   */	PUNCTUATION,
	/* 051 )   */	PUNCTUATION,
	/* 052 *   */	PUNCTUATION,
	/* 053 +   */	PUNCTUATION,
	/* 054 ,   */	PUNCTUATION,
	/* 055 -   */	PUNCTUATION,
	/* 056 .   */	PUNCTUATION,
	/* 057 /   */	PUNCTUATION,
	/* 060 0   */	BIN_DIGIT | OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 061 1   */	BIN_DIGIT | OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 062 2   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 063 3   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 064 4   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 065 5   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 066 6   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 067 7   */	OCT_DIGIT | DEC_DIGIT | HEX_DIGIT,
	/* 070 8   */	DEC_DIGIT | HEX_DIGIT,
	/* 071 9   */	DEC_DIGIT | HEX_DIGIT,
	/* 072 :   */	PUNCTUATION,
	/* 073 ;   */	PUNCTUATION,
	/* 074 <   */	PUNCTUATION,
	/* 075 =   */	PUNCTUATION,
	/* 076 >   */	PUNCTUATION,
	/* 077 ?   */	PUNCTUATION,
	/* 100 @   */	0,
	/* 101 A   */	LETTER | HEX_DIGIT,
	/* 102 B   */	LETTER | HEX_DIGIT,
	/* 103 C   */	LETTER | HEX_DIGIT,
	/* 104 D   */	LETTER | HEX_DIGIT,
	/* 105 E   */	LETTER | HEX_DIGIT,
	/* 106 F   */	LETTER | HEX_DIGIT,
	/* 107 G   */	LETTER,
	/* 110 H   */	LETTER,
	/* 111 I   */	LETTER,
	/* 112 J   */	LETTER,
	/* 113 K   */	LETTER,
	/* 114 L   */	LETTER,
	/* 115 M   */	LETTER,
	/* 116 N   */	LETTER,
	/* 117 O   */	LETTER,
	/* 120 P   */	LETTER,
	/* 121 Q   */	LETTER,
	/* 122 R   */	LETTER,
	/* 123 S   */	LETTER,
	/* 124 T   */	LETTER,
	/* 125 U   */	LETTER,
	/* 126 V   */	LETTER,
	/* 127 W   */	LETTER,
	/* 130 X   */	LETTER,
	/* 131 Y   */	LETTER,
	/* 132 Z   */	LETTER,
	/* 133 [   */	PUNCTUATION,
	/* 134 \   */	PUNCTUATION,
	/* 135 ]   */	PUNCTUATION,
	/* 136 ^   */	PUNCTUATION,
	/* 137 _   */	LETTER,
	/* 140 `   */	0,
	/* 141 a   */	LETTER | HEX_DIGIT,
	/* 142 b   */	LETTER | HEX_DIGIT,
	/* 143 c   */	LETTER | HEX_DIGIT,
	/* 144 d   */	LETTER | HEX_DIGIT,
	/* 145 e   */	LETTER | HEX_DIGIT,
	/* 146 f   */	LETTER | HEX_DIGIT,
	/* 147 g   */	LETTER,
	/* 150 h   */	LETTER,
	/* 151 i   */	LETTER,
	/* 152 j   */	LETTER,
	/* 153 k   */	LETTER,
	/* 154 l   */	LETTER,
	/* 155 m   */	LETTER,
	/* 156 n   */	LETTER,
	/* 157 o   */	LETTER,
	/* 160 p   */	LETTER,
	/* 161 q   */	LETTER,
	/* 162 r   */	LETTER,
	/* 163 s   */	LETTER,
	/* 164 t   */	LETTER,
	/* 165 u   */	LETTER,
	/* 166 v   */	LETTER,
	/* 167 w   */	LETTER,
	/* 170 x   */	LETTER,
	/* 171 y   */	LETTER,
	/* 172 z   */	LETTER,
	/* 173 {   */	PUNCTUATION,
	/* 174 |   */	PUNCTUATION,
	/* 175 }   */	PUNCTUATION,
	/* 176 ~   */	PUNCTUATION,

	BAD
};

/*================================
			Lexer
================================*/

Lexer::Lexer()
{

}

#if 0
Lexer::Lexer( const char *ptr, UINT length, const char *filename )
: lex( ptr, length, filename, (LEXFL_ALLOWPATHNAMES | LEXFL_NOSTRINGESCAPECHARS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS) )
{

}

Lexer::~Lexer()
{

}

BOOL Lexer::eof() const
{
	return lex.EndOfFile();
}

BOOL Lexer::next( Token & token )
{
	return lex.ReadToken( &token );
}

BOOL Lexer::expect( char c )
{
	Token	token;
	next(token);
	return token == c;
}

BOOL Lexer::expect( const char* str )
{
	Token	token;
	next(token);
	return token == str;
}

BOOL Lexer::expect( const char* str, UINT length )
{
	return lex.ExpectTokenString( str );
}

INT Lexer::expectInt( INT & value, INT min, INT max )
{
	Token token;
	lex.ExpectTokenType( TT_NUMBER, TT_INTEGER, &token );
	INT value = token.GetIntValue();
	if( value < min || value > max ) {
		lex.Error("expected an integer value in range [%u,%u] but got '%s'.\n"
			,min,max,token.ToChars());
	}
	return value;
}

UINT Lexer::expectUInt( UINT & value, UINT min, UINT max )
{
	Token token;
	lex.ExpectTokenType( TT_NUMBER, TT_INTEGER, &token );
	INT value = token.GetIntValue();
	if( value < 0 ) {
		dxWarnf("File '%s', line '%d': expected a positive integer value but got '%s'.\n",
			lex.GetFileName(),lex.GetLineNum(),token.ToChars());
	}
	if( value < min || value > max ) {
		lex.Error("expected a positive integer value in range [%u,%u] but got '%s'.\n"
			,min,max,token.ToChars());
	}
	return (UINT)value;
}

FLOAT Lexer::expectFloat( FLOAT & value, FLOAT min, FLOAT max )
{
	Token token;
	lex.ExpectTokenType( TT_NUMBER, TT_FLOAT, &token );
	FLOAT f = token.GetFloatValue();
	if( f < min || f > max ) {
		lex.Error("expected a floating-point value in range [%f,%f] but got '%s'.\n"
			,min,max,token.ToChars());
	}
	return f;
}

BOOL Lexer::expectBool( bool & value )
{
	Token token;
	lex.ReadToken( &token );
	if( token.equals( "TRUE" ) ) {
		return TRUE;
	}
	if( token.equals( "FALSE" ) ) {
		return FALSE;
	}
	lex.Error( "Expected boolean, but got '%s'.\n",token.ToChars());
	return FALSE;
}

BOOL Lexer::expectAny( Token & token )
{
	return lex.ExpectAnyToken( &token );
}

BOOL Lexer::skipRestOfLine()
{
	return lex.SkipRestOfLine();
}
#endif

MX_NAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
