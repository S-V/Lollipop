// Parser_Common.h
#pragma once

//---------------------------------------------------------------------------
//	Memory management
//---------------------------------------------------------------------------

void* Lex_Alloc( SizeT bytes );
void Lex_Free( void* ptr );

#define LEX_NEW( typeName )	new(Lex_Alloc(sizeof(typeName))) typeName

//---------------------------------------------------------------------------
//	Parsing
//---------------------------------------------------------------------------

enum
{
	MaxFileNameLength = 64,
	MaxIdentifierLength = 48,
	MaxInformationLength = 64,
	MaxTempStringLength = 128,
};

typedef TFixedString< MaxFileNameLength >		wrFile;
typedef TFixedString< MaxIdentifierLength >		wrName;
typedef TFixedString< MaxInformationLength >	wrInfoStr;
typedef TFixedString< MaxTempStringLength >		wrBuffer;

struct wrLocation
{
	String	file;
	UINT	line, column;

public:
	wrLocation()
	{}
	wrLocation( const char* fileName, UINT lineNum, UINT colNum )
		: file(fileName), line(lineNum), column(colNum)
	{}
	wrBuffer ToStr() const
	{
		wrBuffer buf;
		buf.Format("%s(%u,%u)", file.ToChars(),line,column);
		return buf;
	}
};

//---------------------------------------------------------------------------
//	Error reporting
//---------------------------------------------------------------------------

class IErrorReport
{
public:
	virtual void Message( const wchar_t* msg ) = 0;
	virtual void Warning( int line, int column, const wchar_t* msg ) = 0;
	virtual void Error( int line, int column, const wchar_t* msg ) = 0;
	virtual void SyntaxError( int line, int column, const wchar_t* msg ) = 0;

	virtual ~IErrorReport() = 0;
};

extern IErrorReport* errorLog;



U32 StringToU32( const char* s, U32 minValue = 0, U32 maxValue = U32(-1) );
F32 StringToF32( const char* s, F32 minValue = MIN_FLOAT_32, F32 maxValue = MAX_FLOAT_32 );


F32 CurrTokenToFloat( F32 minValue = MIN_FLOAT_32, F32 maxValue = MAX_FLOAT_32 );
