#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Renderer/Core/Texture.h>

#include <EditorSupport/AssetPipeline.h>
#include "TextureConverter.h"

namespace Editor
{

TextureConverter::TextureConverter()
{
	F_RegisterAssetProcessor(".dds",this);
	F_RegisterAssetProcessor(".png",this);
	//RegisterAssetImporter(".tga",this);
	F_RegisterAssetProcessor(".bmp",this);
	F_RegisterAssetProcessor(".jpg",this);
	F_RegisterAssetProcessor(".tiff",this);
	F_RegisterAssetProcessor(".gif",this);
	F_RegisterAssetProcessor(".wmp",this);
}

TextureConverter::~TextureConverter()
{

}

void TextureConverter::ProcessFile(
	const ProcessFileInput& input,
	ProcessFileOutput &output
	)
{
	output.assetType = Asset_Texture2D;

	const char* filePath = input.filePath.ToChars();

	JsonBuildConfig	buildConfig;
	const bool bHasBuildConfig = JsonBuildConfig::F_Get_Asset_Build_Config( filePath, buildConfig );


	String	fileExtension;
	input.filePath.ExtractFileExtension( fileExtension );

	if( fileExtension == "dds")
	{
		if( bHasBuildConfig ) {
			mxWarnf("TextureConverter::ProcessFile: build config for %s is ignored\n", input.filePath.ToChars());
		}
		//FileReader	srcFile( input.filePath );
		//CHK_VRET_IF_NOT( srcFile.IsOpen() );

		Util_LoadFileToMemory( input.filePath, output.compiledData );
	}
	else
	{


		bool	is_normal_map = false;
		buildConfig.GetBool("is_normal_map", is_normal_map);



		//	Util_LoadFileToMemory( input.filePath.ToChars(), output.compiledData );


		D3DX11_IMAGE_INFO	imageInfo;

		{
			const HRESULT hResult = D3DX11GetImageInfoFromFileA(
				filePath,
				nil,//__in   ID3DX11ThreadPump *pPump,
				&imageInfo,//__in   D3DX11_IMAGE_INFO *pSrcInfo,
				nil	//__out  HRESULT *pHResult
				);
			if(FAILED(hResult)) {
				mxWarnf("Couldn't read texture '%s'\n", filePath);
				return;
			}
		}


		D3DX11_IMAGE_LOAD_INFO		imageLoadInfo;
		{
			bool bResizeToPowOf2 = true;
			buildConfig.GetBool("make_pow_of_2", bResizeToPowOf2);

			if( bResizeToPowOf2 )
			{
				if( !IsPowerOfTwo( imageInfo.Width ) ) {
					imageLoadInfo.Width = FloorPowerOfTwo( imageInfo.Width );
				}
				if( !IsPowerOfTwo( imageInfo.Height ) ) {
					imageLoadInfo.Height = FloorPowerOfTwo( imageInfo.Height );
				}
				imageLoadInfo.Depth = imageInfo.Depth;
			}
			else
			{mxDEBUG_BREAK;
			imageLoadInfo.Width = imageInfo.Width;
			imageLoadInfo.Height = imageInfo.Height;
			imageLoadInfo.Depth = imageInfo.Depth;
			}

			// First mip level to load
			imageLoadInfo.FirstMipLevel = D3DX11_DEFAULT;

			// Number of mip levels to load after the first level
			imageLoadInfo.MipLevels = imageInfo.MipLevels;

			// Resample texture to the specified format
			imageLoadInfo.Format =
				DXGI_FORMAT_R8G8B8A8_UNORM
				//DXGI_FORMAT_BC1_UNORM
				//DXGI_FORMAT_BC3_UNORM
				;
			//imageLoadInfo.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//imageLoadInfo.Format = DXGI_FORMAT_R8G8B8A8_UINT;

			// Filter the texture using the specified filter (only when resampling)
			imageLoadInfo.Filter = D3DX11_DEFAULT;

			// Filter the texture mip levels using the specified filter
			// (only if generating mips)
			imageLoadInfo.MipFilter = D3DX11_DEFAULT;
		}


		ID3D11ResourcePtr			pTexture;

		{
			const HRESULT hResult = D3DX11CreateTextureFromFileA(
				GetD3DDevice(),
				filePath,
				&imageLoadInfo,//__in   D3DX11_IMAGE_LOAD_INFO *pLoadInfo,
				nil,//__in   ID3DX11ThreadPump *pPump,
				&pTexture.Ptr,//__out  ID3D11Resource **ppTexture,
				nil	//__out  HRESULT *pHResult
				);
			if(FAILED(hResult)) {
				mxWarnf("Couldn't convert texture '%s'\n", filePath);
				return;
			}
		}

		{
			ID3DBlobPtr		pDestBlob;

			const HRESULT hResult = D3DX11SaveTextureToMemory(
				GetD3DDeviceContext(),
				pTexture,

				D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_DDS,
				//D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_PNG,
				//D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_TIFF,

				&pDestBlob.Ptr,
				0	//__in   UINT Flags
				);
			if(FAILED(hResult)) {
				mxWarnf("Couldn't save texture '%s'\n", filePath);
				return;
			}

			output.compiledData.Set( pDestBlob->GetBufferPointer(), pDestBlob->GetBufferSize() );
		}

	}//if( fileExtension != "dds ")

	output.fileExtension = ".dds";
	//output.fileExtension = ".png";
	//output.fileExtension = ".tiff";

	//const HRESULT hResult = D3DX11SaveTextureToFileA(
	//	GetD3DDeviceContext(),
	//	pTexture,
	//	D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_DDS,
	//	filePath
	//	);
	//if(FAILED(hResult)) {
	//	mxWarnf("Failed to save texture to file '%s'\n", filePath);
	//	return;
	//}


	/*
	HRESULT  D3DX11GetImageInfoFromFile(
	__in   LPCTSTR pSrcFile,
	__in   ID3DX11ThreadPump *pPump,
	__in   D3DX11_IMAGE_INFO *pSrcInfo,
	__out  HRESULT *pHResult
	);

	HRESULT  D3DX11SaveTextureToFile(
	ID3D11DeviceContext *pContext,
	__in  ID3D11Resource *pSrcTexture,
	__in  D3DX11_IMAGE_FILE_FORMAT DestFormat,
	__in  LPCTSTR pDestFile
	);
	*/

}

}//namespace Editor
