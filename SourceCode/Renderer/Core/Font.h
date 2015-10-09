#pragma once

void DbgFont_Setup();
void DbgFont_Close();

void DbgFont_BeginRendering();

void DbgFont_DrawString(
	const UNICODECHAR* s,
	FLOAT X, FLOAT Y,
	const SColor& color,
	FLOAT fontSize,
	ETextAlignment alignment = ETextAlignment::TA_AlignLeft
);

void DbgFont_DrawString3D(
	const UNICODECHAR* s,
	const XMMATRIX& transform,
	const SColor& color,
	FLOAT fontSize,
	ETextAlignment alignment = ETextAlignment::TA_AlignLeft
);

void DbgFont_EndRendering();



class rxDrawTextStream : public mxTextStream
{
	FLOAT	m_X, m_Y;
	FColor	m_color;

public:
	rxDrawTextStream( UINT x, UINT y, const FColor& color = FColor::WHITE )
	{
		m_X = x;
		m_Y = y;
		m_color = color;

		DbgFont_BeginRendering();
	}
	~rxDrawTextStream()
	{
		DbgFont_EndRendering();
	}
	virtual void Print( const char* str, UINT length ) override
	{
		enum { DbgFontSize = 12 };
		UNICODECHAR buffer[ MAX_STRING_CHARS ];
		mxAnsiToUnicode( buffer, str, ARRAY_SIZE(buffer) );
		DbgFont_DrawString( buffer, m_X, m_Y, m_color.ToRGBA32(), DbgFontSize );
	}
};


