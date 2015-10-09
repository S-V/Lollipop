// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#if 0
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#endif



#pragma once

//#include <DXUT11/Core/DXUT.h>
//#include <DXUT11/Optional/DXUTcamera.h>
//#include <DXUT11/Optional/DXUTgui.h>
//#include <DXUT11/Optional/DXUTsettingsDlg.h>
//#include <DXUT11/Optional/SDKmisc.h>
//#include <DXUT11/Optional/SDKMesh.h>

#include <Base/Base.h>
#include <Core/Core.h>
#include <Driver/Driver.h>
#include <Graphics/Graphics_DX11.h>
#include <Renderer/Renderer.h>
#include <Engine/Engine.h>

MX_USING_NAMESPACE;

//#if MX_EDITOR
//#error This is not an editor, this is a final (retail, release) build!
//#endif

#if MX_AUTOLINK
//#pragma comment( lib, "DXUT11.lib" )
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Driver.lib" )
#pragma comment( lib, "Audio.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "Renderer.lib" )
#pragma comment( lib, "Engine.lib" )
#pragma comment( lib, "Game.lib" )
#endif //MX_AUTOLINK
