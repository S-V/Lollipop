// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once

#ifndef __MX_ENGINE_PCH_H__
#define __MX_ENGINE_PCH_H__

#include <Base/Base.h>
#include <Driver/Driver.h>
#include <Core/Core.h>

mxUSING_NAMESPACE;

#include <Base/Math/Math.h>

#include <Graphics/Graphics_DX11.h>
#include <Graphics/DX11/D3D11DeviceContext.h>

#if MX_AUTOLINK
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Driver.lib" )
#pragma comment( lib, "Audio.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "Renderer.lib" )
#pragma comment( lib, "Engine.lib" )
//#pragma comment( lib, "EditorSupport.lib" )
#endif //MX_AUTOLINK


#if MX_COMPILE_WITH_BULLET_PHYSICS

	#include <btBulletDynamicsCommon.h>

	mxPERM("bullet physics sdk headers and libs");
	#if MX_AUTOLINK
	#pragma comment( lib, "BulletDynamics.lib" )
	#pragma comment( lib, "BulletCollision.lib" )
	#pragma comment( lib, "LinearMath.lib" )
	#endif //MX_AUTOLINK

#endif // !MX_COMPILE_WITH_PHYSICS

#endif // !__MX_ENGINE_PCH_H__
