
#pragma once

#include <Base/Base.h>
#include <Graphics/Graphics.h>

MX_NAMESPACE_BEGIN

struct GenerateFontInput
{
	LPCWSTR			fontName;
	FLOAT			fontSize;
	EFontStyle		fontStyle;
	bool			antiAliased;

public:
	GenerateFontInput()
	{
		fontName = L"Arial";
		fontSize = 18;
		fontStyle = EFontStyle::Regular;
		antiAliased = true;
	}
};


struct GenerateFontOutput : FontInfo
{
	TList< BYTE >	imageBytes;	// font data, in R8G8B8A8 format

public:
	GenerateFontOutput( HMemory hMemoryMgr = EMemHeap::DefaultHeap )
		: imageBytes( hMemoryMgr )
	{

	}
};

MX_SWIPED("MJP (Matt Pettineo)");
class BitmapFontGenerator
{
public:
	BitmapFontGenerator();
	~BitmapFontGenerator();

	void Initialize();
	void Shutdown();

	void NewFont( const GenerateFontInput& input, GenerateFontOutput &output );

private:
	ULONG_PTR	m_gdiPlusToken;
};

MX_NAMESPACE_END
