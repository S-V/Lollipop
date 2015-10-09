// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

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
MX_USING_NAMESPACE;

#if MX_AUTOLINK
//#pragma comment( lib, "DXUT11.lib" )
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Driver.lib" )
#pragma comment( lib, "Graphics.lib" )
#endif //MX_AUTOLINK
