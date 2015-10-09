/*
=============================================================================
	File:	Tokenizer.h
	Desc:	
	Note:	UNDONE
=============================================================================
*/

#ifndef __MX_TOKENIZER_H__
#define __MX_TOKENIZER_H__

MX_NAMESPACE_BEGIN


#if 0
enum { MAX_TOKEN_LENGTH = 32 };

struct Token
{
	char	text[ MAX_TOKEN_LENGTH ];	// token string
	char *	offset;	// file offset
	UINT	line;	// source line
	union {
		int		intValue;
		float	floatValue;
	};

public:
	Token(ENoInit)
	{}
	Token()
	{
		ZERO_OUT(*this);
	}
	BOOL	operator == ( char c ) const;
	BOOL	operator == ( const char* str ) const;
};


#else

struct Token : public idToken
{
	FORCEINLINE
	Token()
	{

	}

	FORCEINLINE
	BOOL operator == ( char c ) const
	{
		return String::ToChars()[0] == c;
	}

	FORCEINLINE
	BOOL operator == ( const char* str ) const
	{
		return !String::Icmp( str );
	}

	FORCEINLINE
	BOOL equals( const char* str, UINT length ) const
	{
		return !String::Icmpn( str, length );
	}
};

#endif

//
enum CharFlags
{
	BLANK		= BIT(0),
	NEWLINE		= BIT(1),
	LETTER		= BIT(2),	// valid letter of the English alphabet

	BIN_DIGIT	= BIT(3),	// valid digit of a binary number
	OCT_DIGIT	= BIT(4),	// valid digit of an octal number
	DEC_DIGIT	= BIT(5),	// valid digit of a decimal number
	HEX_DIGIT	= BIT(6),	// valid digit of a hexadecimal number

	PUNCTUATION	= BIT(7),

	BAD			= BIT(8),
};

struct ParseUtil
{
	enum { NUM_CHARS = 256 };
	static const UINT charMap[ NUM_CHARS ];
};

struct TokenReader
{
};

/*
-----------------------------------------------------------------------------
	Lexer
-----------------------------------------------------------------------------
*/
//template< class TOKENIZER = TokenReader >
class Lexer {
public:
			Lexer();
			Lexer( const char *ptr, UINT length, const char *filename );
			~Lexer();

	BOOL	eof() const;

	BOOL	next( Token & token );

	// expect a certain token, reads the token when available
	BOOL	expect( char c );
	BOOL	expect( const char* str );
	BOOL	expect( const char* str, UINT length );

			
			// read a signed integer
	INT	expectInt( INT & value, INT min = -MAX_INT32, INT max = +MAX_INT32 );

			// read an unsigned integer
	UINT	expectUInt( UINT & value, UINT min = 0, UINT max = +MAX_UINT32 );

			// read a floating point number
	FLOAT	expectFloat( FLOAT & value, FLOAT min = -BIG_NUMBER, FLOAT max = +BIG_NUMBER );

	BOOL	expectBool( bool & value );

			// expect a token
	BOOL	expectAny( Token & token );

			// skip the rest of the current line
	BOOL	skipRestOfLine();

private:
	idLexer		lex;
};

MX_NAMESPACE_END

#endif // !__MX_MISC_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
