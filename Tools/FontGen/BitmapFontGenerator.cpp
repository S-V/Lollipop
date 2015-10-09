// BitmapFontGenerator.cpp
// no pch

// Windows Header Files:
#include <windows.h>

// Native C++ compiler COM support
#include <comip.h>
#include <comdef.h>

// GDI+
#include <gdiplus.h>

#include "BitmapFontGenerator.h"

#if 1//AUTOLINK
#pragma comment( lib, "gdi32.lib" )
#pragma comment (lib, "Gdiplus.lib")
#endif

namespace
{

// Throws a GdiPlusException on failing Status value
static inline void GdiPlusCall( Gdiplus::Status status )
{
	if( status != Gdiplus::Ok )
	{
		DEBUG_BREAK;
	}
}

}//namespace

MX_NAMESPACE_BEGIN

BitmapFontGenerator::BitmapFontGenerator()
{
	m_gdiPlusToken = nil;
}

BitmapFontGenerator::~BitmapFontGenerator()
{}

void BitmapFontGenerator::Initialize()
{
	// Init GDI+
	Assert( nil == m_gdiPlusToken );
	Gdiplus::GdiplusStartupInput startupInput (NULL, TRUE, TRUE);
	Gdiplus::GdiplusStartupOutput startupOutput;
	GdiPlusCall(Gdiplus::GdiplusStartup( &m_gdiPlusToken, &startupInput, &startupOutput ));
}

void BitmapFontGenerator::Shutdown()
{
	// Shutdown GDI+
	Gdiplus::GdiplusShutdown( m_gdiPlusToken );
}

void BitmapFontGenerator::NewFont( const GenerateFontInput& input, GenerateFontOutput &output )
{
	Assert(m_gdiPlusToken);

	output.size = input.fontSize;

	Gdiplus::TextRenderingHint hint = input.antiAliased ? Gdiplus::TextRenderingHintAntiAliasGridFit : Gdiplus::TextRenderingHintSystemDefault;

	// Create the font
	Gdiplus::Font	font( input.fontName, input.fontSize, input.fontStyle, Gdiplus::UnitPixel, NULL );

	// Check for error during construction
	GdiPlusCall(font.GetLastStatus());

	// Create a temporary Bitmap and Graphics for figuring out the rough size required
	// for drawing all of the characters
	const int size = static_cast<int>( input.fontSize * FontGen::NumChars * 2 ) + 1;
	Gdiplus::Bitmap sizeBitmap( size, size, PixelFormat32bppARGB );
	GdiPlusCall(sizeBitmap.GetLastStatus());

	Gdiplus::Graphics sizeGraphics( &sizeBitmap );
	GdiPlusCall(sizeGraphics.GetLastStatus());
	GdiPlusCall(sizeGraphics.SetTextRenderingHint( hint ));

	output.charHeight = font.GetHeight( &sizeGraphics ) * 1.5f;

	WCHAR allChars[ FontGen::NumChars + 1 ];
	for( WCHAR i = 0; i < FontGen::NumChars; ++i ) {
		allChars[ i ] = i + FontGen::StartChar;
	}
	allChars[ FontGen::NumChars ] = 0;

	Gdiplus::RectF sizeRect;
	GdiPlusCall(sizeGraphics.MeasureString( allChars, FontGen::NumChars, &font, Gdiplus::PointF(0, 0), &sizeRect ));

#if 0
	const int numRows = static_cast<int>( sizeRect.Width / FontGen::TexWidth ) + 1;
#else
	FontGen::TexWidth = CeilPowerOfTwo( static_cast<int>( sizeRect.Width ) );
	const int numRows = 1;
#endif

	const int texHeight = static_cast<int>( numRows * output.charHeight ) + 1;



	output.texWidth = FontGen::TexWidth;
	output.texHeight = texHeight;

	// Create a temporary Bitmap and Graphics for drawing the characters one by one
	const int tempSize = static_cast<int>( input.fontSize * 2 );
	Gdiplus::Bitmap drawBitmap( tempSize, tempSize, PixelFormat32bppARGB );
	GdiPlusCall(drawBitmap.GetLastStatus());

	Gdiplus::Graphics drawGraphics(&drawBitmap);
	GdiPlusCall(drawGraphics.GetLastStatus());
	GdiPlusCall(drawGraphics.SetTextRenderingHint(hint));

	// Create a temporary Bitmap + Graphics for creating a full character set
	Gdiplus::Bitmap textBitmap (FontGen::TexWidth, texHeight, PixelFormat32bppARGB);
	GdiPlusCall(textBitmap.GetLastStatus());

	Gdiplus::Graphics textGraphics (&textBitmap);
	GdiPlusCall(textGraphics.GetLastStatus());
	GdiPlusCall(textGraphics.Clear(Gdiplus::Color(0, 255, 255, 255)));
	GdiPlusCall(textGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy));

	// Solid brush for text rendering
	Gdiplus::SolidBrush brush (Gdiplus::Color(255, 255, 255, 255));
	GdiPlusCall(brush.GetLastStatus());

	// Draw all of the characters, and copy them to the full character set
	WCHAR charString [2];
	charString[1] = 0;
	int currentX = 0;
	int currentY = 0;
	for( SizeT i = 0; i < FontGen::NumChars; ++i )
	{
		charString[0] = static_cast<WCHAR>(i + FontGen::StartChar);

		// Draw the character
		GdiPlusCall(drawGraphics.Clear( Gdiplus::Color( 0, 255, 255, 255 ) ));
		GdiPlusCall(drawGraphics.DrawString( charString, 1, &font, Gdiplus::PointF( 0, 0 ), &brush ));

		// Figure out the amount of blank space before the character
		int minX = 0;
		for( int x = 0; x < tempSize; ++x )
		{
			for( int y = 0; y < tempSize; ++y )
			{
				Gdiplus::Color color;
				GdiPlusCall(drawBitmap.GetPixel( x, y, &color ));
				if( color.GetAlpha() > 0 )
				{
					minX = x;
					x = tempSize;
					break;
				}
			}
		}

		// Figure out the amount of blank space after the character
		int maxX = tempSize - 1;
		for( int x = tempSize - 1; x >= 0; --x )
		{
			for( int y = 0; y < tempSize; ++y )
			{
				Gdiplus::Color color;
				GdiPlusCall(drawBitmap.GetPixel( x, y, &color ));
				if( color.GetAlpha() > 0 )
				{
					maxX = x;
					x = -1;
					break;
				}
			}
		}

		int charWidth = maxX - minX + 1;

		// Figure out if we need to move to the next row
		if( currentX + charWidth >= FontGen::TexWidth )
		{
			currentX = 0;
			currentY += static_cast<int>(output.charHeight) + 1;
		}

		// Fill out the structure describing the character position
		output.charDescs[i].X = static_cast<float>(currentX);
		output.charDescs[i].Y = static_cast<float>(currentY);
		output.charDescs[i].Width = static_cast<float>(charWidth);
		output.charDescs[i].Height = static_cast<float>(output.charHeight);

		// Copy the character over
		int height = static_cast<int>( output.charHeight + 1 );
		GdiPlusCall(textGraphics.DrawImage( &drawBitmap, currentX, currentY, minX, 0, charWidth, height, Gdiplus::UnitPixel ));

		currentX += charWidth + 1;
	}

	// Figure out the width of a space character
	charString[0] = ' ';
	charString[1] = 0;
	GdiPlusCall(drawGraphics.MeasureString( charString, 1, &font, Gdiplus::PointF(0, 0), &sizeRect));
	output.spaceWidth = sizeRect.Width;

	// Lock the bitmap for direct memory access
	Gdiplus::BitmapData bmData;
	Gdiplus::Rect		lockRect( 0, 0, FontGen::TexWidth, texHeight );
	GdiPlusCall(textBitmap.LockBits( &lockRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData ));

	struct R8G8B8A8
	{
		U32	RGBA;
	};

	// bmData.Scan0 points at font data
	// FontGen::TexWidth is the width of the texture
	// texHeight  is the height of the texture

	const SizeT bytesOfData = FontGen::TexWidth * texHeight * sizeof(R8G8B8A8);
	output.imageBytes.SetNum( bytesOfData );

	MemCopy( output.imageBytes.ToPtr(), bmData.Scan0, bytesOfData );

	GdiPlusCall(textBitmap.UnlockBits( &bmData ));
}

MX_NAMESPACE_END

