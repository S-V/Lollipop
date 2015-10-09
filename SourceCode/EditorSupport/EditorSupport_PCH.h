// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once

#ifndef __MX_EDITOR_PCH__
#define __MX_EDITOR_PCH__

#include <Base/Base.h>
#include <Base/Math/Math.h>
#include <Base/IO/Files.h>
#include <Base/Text/String.h>
#include <Base/Util/PathUtils.h>

#include <Core/Core.h>

#include <Driver/Driver.h>

#include <Graphics/Graphics_DX11.h>


mxUSING_NAMESPACE;

#if MX_AUTOLINK
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Driver.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "Renderer.lib" )
#endif //MX_AUTOLINK


#if !MX_EDITOR
#	error Invalid build option
#endif // !MX_EDITOR


#endif // !__MX_EDITOR_PCH__
