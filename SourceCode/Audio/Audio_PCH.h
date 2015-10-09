#ifndef MX_AUDIO_PCH
#define MX_AUDIO_PCH

#include <Base/Base.h>

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
 
#if MX_AUTOLINK
	#pragma comment( lib, "x3daudio.lib" )
	#pragma comment( lib, "winmm.lib" )
	#pragma comment( lib, "comctl32.lib" )
#endif // MX_AUTOLINK

#include <Base/Util/DirectX_Helpers.h>

#endif // MX_AUDIO_PCH
