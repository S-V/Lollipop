#include <QtGui>

#include <Base/Base.h>
#include <Core/Core.h>
#include <Driver/Driver.h>
#include <Graphics/Graphics_PCH.h>
#include <Renderer/Renderer_PCH.h>
MX_USING_NAMESPACE;

#if MX_AUTOLINK
//#pragma comment( lib, "DXUT11.lib" )
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Driver.lib" )
//#pragma comment( lib, "Network.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "Renderer.lib" )
#pragma comment( lib, "EditorSupport.lib" )
#pragma comment( lib, "QtSupport.lib" )
#endif //MX_AUTOLINK

#include <Base/Text/TextUtils.h>

#include <EditorSupport/EditorSupport.h>

