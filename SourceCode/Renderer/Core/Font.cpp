#include "Renderer_PCH.h"
#pragma hdrstop

#define RX_USE_DEBUG_FONT	1

// Debug text rendering.

#include "Renderer.h"

#include "Font.h"


// use Erik Rufelt's font wrapper (unfortunately, it's slow)

#include <DirectX/D3D11FontWrapper/FW1FontWrapper.h>
#if MX_AUTOLINK
#pragma comment( lib, "D3D11FontWrapper.lib" )
#endif //MX_AUTOLINK

static
FW1_TEXT_FLAG ConvertAlignment( ETextAlignment alignment )
{
	if( alignment == TA_AlignLeft ) {
		return FW1_LEFT;
	}
	if( alignment == TA_AlignRight ) {
		return FW1_RIGHT;
	}
	if( alignment == TA_AlignCenter ) {
		return FW1_CENTER;
	}
	return ConvertAlignment( TA_AlignLeft );
}

static const WCHAR* FONT_FAMILY = L"Arial";

static const UINT DRAW_FLAGS =
	//FW1_CENTER | FW1_RESTORESTATE | FW1_VCENTER | FW1_NOWORDWRAP
	FW1_NOWORDWRAP
	;

//
//	D3D11DebugFont
//
class D3D11DebugFont
	: SingleInstance< D3D11DebugFont >, Initializable
{
	dxPtr< IFW1FontWrapper >	mFontWrapper;

	dxPtr< IFW1GlyphRenderStates >	mRenderStates;	// default font render states

	bool	mHasBegun;

public:

	D3D11DebugFont()
	{
		mHasBegun = false;
	}

	void Setup( ID3D11Device* pDevice )
	{
		AssertPtr(pDevice);
		OneTimeInit();

		// Create the font-wrapper.
		IFW1Factory *pFW1Factory;
		dxchk(FW1CreateFactory(FW1_VERSION, &pFW1Factory));

		/*
		FW1_FONTWRAPPERCREATEPARAMS createParams;
		ZeroMemory(&createParams, sizeof(createParams));

		// Set up the font-wrapper to use mip-mapping and anisotropic filtering for better visuals with transformed text
		createParams.SheetMipLevels = 5;
		createParams.AnisotropicFiltering = TRUE;
		createParams.DefaultFontParams.pszFontFamily = L"Arial";
		createParams.DefaultFontParams.FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		createParams.DefaultFontParams.FontStyle = DWRITE_FONT_STYLE_NORMAL;
		createParams.DefaultFontParams.FontStretch = DWRITE_FONT_STRETCH_NORMAL;

		dxchk(pFW1Factory->CreateFontWrapper( pDevice, NULL, &createParams, &mFontWrapper.Ptr ));
		*/
		
		dxchk(pFW1Factory->CreateFontWrapper( pDevice, FONT_FAMILY, &mFontWrapper.Ptr ));

		// Get the default font render states
		dxchk(mFontWrapper->GetRenderStates( &mRenderStates.Ptr ));

		pFW1Factory->Release();
	}
	void Close()
	{
		OneTimeDestroy();
		mRenderStates = nil;
		mFontWrapper = nil;
		mHasBegun = false;
	}
	void Begin()
	{
		ID3D11DeviceContext* pImmediateContext = GetD3DDeviceContext();
		// Set the default rendering states
		mRenderStates->SetStates( pImmediateContext,  0 );

		mHasBegun = true;
	}
	void End()
	{
		mHasBegun = false;
	}

	void RenderText(
			const UNICODECHAR* s,
			FLOAT X, FLOAT Y,
			const SColor& color,
			FLOAT fontSize,
			ETextAlignment alignment
		)
	{
		Assert(mHasBegun);

		mxPROFILE_SCOPE( "Render Debug Text" );

		ID3D11DeviceContext* pImmediateContext = GetD3DDeviceContext();

		U4 colorABGR = MAKEFOURCC(color.R,color.G,color.B,color.A);

		mFontWrapper->DrawString(
			pImmediateContext,
			s,
			fontSize,
			X,
			Y,
			colorABGR,
			ConvertAlignment(alignment) | FW1_STATEPREPARED | DRAW_FLAGS
		);
	}

	void RenderText3D(
			const UNICODECHAR* s,
			const XMMATRIX& transform,
			const SColor& color,
			FLOAT fontSize,
			ETextAlignment alignment
		)
	{
		Assert(mHasBegun);

		mxPROFILE_SCOPE( "Render Debug Text 3D" );

		ID3D11DeviceContext* pImmediateContext = GetD3DDeviceContext();

		U4 colorABGR = MAKEFOURCC(color.R,color.G,color.B,color.A);

		FW1_RECTF rect = { 0 };

		mFontWrapper->DrawString(
			pImmediateContext,
			s,
			FONT_FAMILY,
			fontSize,
			&rect,
			colorABGR,
			nil,	// const FW1_RECTF *pClipRect
			reinterpret_cast<const float*>(&transform),	// const FLOAT *pTransformMatrix
			ConvertAlignment(alignment) | FW1_STATEPREPARED | DRAW_FLAGS
		);
	}
};

namespace
{
	TBlob16< D3D11DebugFont >		gD3D11DebugFont;
}

void DbgFont_Setup()
{
#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Construct();
	gD3D11DebugFont.Get().Setup(D3DDevice);
#endif // RX_USE_DEBUG_FONT
}

void DbgFont_Close()
{
#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Get().Close();
	gD3D11DebugFont.Destruct();
#endif // RX_USE_DEBUG_FONT
}

void DbgFont_BeginRendering()
{
#if RX_D3D_USE_PERF_HUD
	D3DPERF_BeginEvent( D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF), L"Start Font Rendering" );
#endif // RX_D3D_USE_PERF_HUD

#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Get().Begin();
#endif // RX_USE_DEBUG_FONT
}

void DbgFont_DrawString(
	const UNICODECHAR* s,
	FLOAT X, FLOAT Y,
	const SColor& color,
	FLOAT fontSize,
	ETextAlignment alignment
	)
{
#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Get().RenderText(
		s,
		X,
		Y,
		color,
		fontSize,
		alignment
		);
#endif // RX_USE_DEBUG_FONT
}

void DbgFont_DrawString3D(
	const UNICODECHAR* s,
	const XMMATRIX& transform,
	const SColor& color,
	FLOAT fontSize,
	ETextAlignment alignment
	)
{
#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Get().RenderText3D(
		s,
		transform,
		color,
		fontSize,
		alignment
		);
#endif // RX_USE_DEBUG_FONT
}

void DbgFont_EndRendering()
{
#if RX_USE_DEBUG_FONT
	gD3D11DebugFont.Get().End();
#endif // RX_USE_DEBUG_FONT


#if RX_D3D_USE_PERF_HUD
	D3DPERF_EndEvent();
#endif // RX_D3D_USE_PERF_HUD
}
