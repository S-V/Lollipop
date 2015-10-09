#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/Graphics_DX11.h>
#include <Graphics/DX11/DX11Private.h>
#include "TakeScreenshot.h"


void TakeScreenShot( rxViewport& viewport, EImageFileFormat format )
{
	//Optimize: use stack strings:
	StackString	currDate, currTime;
	GetCurrentDateString( currDate );
	GetCurrentTimeString( currTime, '-' );

	StackString	screenShotFileName( currDate );
	screenShotFileName += "_";
	screenShotFileName += currTime;

	screenShotFileName += GetImageFileFormatFileExt( format );

	TakeScreenShotEx( viewport, screenShotFileName.ToChars(), format );
}

void TakeScreenShotEx( rxViewport& viewport, PCSTR fileName, EImageFileFormat format )
{
	HRESULT hr = D3DX11SaveTextureToFileA(
		D3DContext,
		viewport.mainRT.pTexture,
		D3D_GetImageFormat( format ),
		fileName
	);
	if(FAILED( hr )) {
		dxWarnf(hr,"Failed to save screenshot to file '%s'.\n",fileName);
	} else {
		dxMsgf("Saving screenshot to file '%s'.\n",fileName);
	}
}


NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
