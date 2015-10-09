/*
=============================================================================
	File:	View.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include "Renderer.h"

rxViewportConfig::rxViewportConfig()
{
	windowHandle = INVALID_WINDOW_HANDLE;
	bIsSceneViewport = true;
	bDepthStencil = true;
}

bool rxViewportConfig::isOk() const
{
	return 1
		&& windowHandle != nil
		;
}

rxFrameContext::rxFrameContext()
{
	clearColor = FColor::BLACK;
}

rxViewport::rxViewport()
{
	uniqueId = INDEX_NONE;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
