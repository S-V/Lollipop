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

