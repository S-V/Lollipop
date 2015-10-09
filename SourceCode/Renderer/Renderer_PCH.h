// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once

#include <Base/Base.h>
mxUSING_NAMESPACE;

#include <Base/Math/Math.h>

#include <Core/Core.h>


#include <Graphics/Graphics.h>
#include <Graphics/Graphics_DX11.h>
#include <Graphics/DX11/DX11Helpers.h>
#include <Graphics/DX11/D3D11DeviceContext.h>
#include <Graphics/D3D/dds.h>

#include <Renderer/Common.h>
//#include <Renderer/Core/View.h>
//#include <Renderer/Core/Material.h>
//#include <Renderer/Core/Geometry.h>
//#include <Renderer/Pipeline/RenderQueue.h>
//#include <Renderer/Scene/Model.h>
//#include <Renderer/Scene/Light.h>



#define rxSET_SHADER_SCOPED( shaderClassName, d3dContext )\
	GPU::shaderClassName::SetScoped	scopedSetShader_##shaderClassName( d3dContext )

#define rxSET_SHADER_SCOPED_X( shaderClassName, d3dContext, shaderInstanceId )\
	GPU::shaderClassName::SetScoped	scopedSetShader_##shaderClassName( d3dContext, shaderInstanceId )

