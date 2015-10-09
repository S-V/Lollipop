// This is a precompiled header.  Include a bunch of common stuff.
// This is kind of ugly in that it adds a bunch of dependency where it isn't needed.
// But on balance, the compile time is much lower (even incrementally) once the precompiled
// headers contain these headers.

#ifndef MX_DRIVER_PCH
#define MX_DRIVER_PCH

#include <Base/Base.h>

mxUSING_NAMESPACE;



mxNAMESPACE_BEGIN

void Util_LimitThreadAffinityToCurrentProcessor();

void Util_SetForegroundWindow( HWND hWnd );

mxNAMESPACE_END



#endif // MX_DRIVER_PCH
