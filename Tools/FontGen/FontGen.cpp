#include "pch.h"

#include <Base/Util/LogUtil.h>
#include <Graphics/DX11/DX11Private.h>

#include "BitmapFontGenerator.h"







class FontGenApp : public Window
{
	SetupGraphics		setupGraphics;

	rxViewport	m_vp;

public:
	FontGenApp( UINT width, UINT height )
		: Window(TEXT("FontGenApp"),width,height)
	{
		DXGI_SWAP_CHAIN_DESC	swapChainDesc;
		D3D_DefaultSwapChainDesc(this->getWindowHandle(),swapChainDesc);

		D3D_InitializeViewport(swapChainDesc,m_vp);
	}
	~FontGenApp()
	{
	}
	virtual void onKeyPressed( EKeyCode key )
	{
		if( key == Key_F11 )
		{
			//TakeScreenShot(IFF_JPG);
		}
		Window::onKeyPressed(key);
	}
	void Tick()
	{
	}
	void GenerateFont()
	{
		BitmapFontGenerator	fontGen;
		fontGen.Initialize();

		GenerateFontInput	input;
		GenerateFontOutput	output;

		fontGen.NewFont( input, output );

		mxPutf("Generated font texture (%dx%d, %d bytes).\n"
			,output.texWidth,output.texHeight,output.imageBytes.GetDataSize()
			);

		// Create a D3D texture, initialized with the bitmap data
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = output.texWidth;
		texDesc.Height = output.texHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = output.imageBytes.ToPtr();
		data.SysMemPitch = output.texWidth * (sizeof R8G8B8A8);
		data.SysMemSlicePitch = 0;

		ID3D11Texture2DPtr texture;
		dxchk(D3DDevice->CreateTexture2D(&texDesc, &data, &texture.Ptr));


/**/
		D3DX11SaveTextureToFileA(
			D3DContext,
			texture,
			D3DX11_IFF_DDS,
			"smalldevfont.dds"
			);

		FileWriter	file("smalldevfont.font");
		DataSaver	saver(file);
		saver & output;
/**/

		// Create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZERO_OUT(srDesc);
		srDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MipLevels = 1;
		srDesc.Texture2D.MostDetailedMip = 0;

		//ID3D11ShaderResourceViewPtr srView;
		//dxchk(D3DDevice->CreateShaderResourceView(texture, &srDesc, &srView.Ptr));

		fontGen.Shutdown();
	}
};

int mxAppMain()
{
	FileLogUtil	fileLog;

	EngineDriver	driver;

	CConsole	console("FontGen");

	FontGenApp	app(800,600);
	app.bringToFront();
	app.setTopLeft(0,0);

	app.GenerateFont();

	//while (app.isOpen())
	//{
	//	app.Tick();

	//	mxSleepMilliseconds(10);
	//}

	return 0;
}

MX_APPLICATION_ENTRY_POINT
