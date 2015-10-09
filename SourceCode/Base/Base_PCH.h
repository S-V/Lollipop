// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#pragma once

#ifndef __MX_BASE_PCH__
#define __MX_BASE_PCH__

#include "Build/BuildDefines.h"		// Build configuration options, compile settings.
#include "Common/Helpers.h"			// Common stuff.
#include "System/Platform.h"		// Platform-specific stuff.

//---------------------------------------------------------------------

mxNAMESPACE_BEGIN

class mxBaseSubsystem {
public:
	mxBaseSubsystem();
	~mxBaseSubsystem();
};

mxNAMESPACE_END

#endif // !__MX_BASE_PCH__
