/*
=============================================================================
	File:	Util.cpp
	Desc:	
=============================================================================
*/
#include <Driver_PCH.h>
#pragma hdrstop
#include <Driver.h>

mxNAMESPACE_BEGIN

//--------------------------------------------------------------------------------------
// Limit the current thread to one processor (the current one). This ensures that timing code 
// runs on only one processor, and will not suffer any ill effects from power management.
// See "Game Timing and Multicore Processors" for more details.
//--------------------------------------------------------------------------------------
//
void Util_LimitThreadAffinityToCurrentProcessor()
{
    HANDLE hCurrentProcess = ::GetCurrentProcess();

    // Get the processor affinity mask for this process
    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;

    if( ::GetProcessAffinityMask( hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask ) != 0 &&
        dwProcessAffinityMask )
    {
        // Find the lowest processor that our process is allows to run against
        DWORD_PTR dwAffinityMask = ( dwProcessAffinityMask & ( ( ~dwProcessAffinityMask ) + 1 ) );

        // Set this as the processor that our thread must always run against
        // This must be a subset of the process affinity mask
        HANDLE hCurrentThread = ::GetCurrentThread();
        if( INVALID_HANDLE_VALUE != hCurrentThread )
        {
            ::SetThreadAffinityMask( hCurrentThread, dwAffinityMask );
            ::CloseHandle( hCurrentThread );
        }
    }

	::CloseHandle( hCurrentProcess );
}

mxSWIPED("glfw");
//========================================================================
// Function for bringing a window into focus and placing it on top of the
// window z stack. Due to some nastiness with how Win98/ME/2k/XP handles
// SetForegroundWindow, we have to go through some really bizarre measures to
// achieve this (thanks again, MS, for making life so much easier)!
//========================================================================

void Util_SetForegroundWindow( HWND hWnd )
{
	int try_count = 0;

	// Try the standard approach first...
	BringWindowToTop( hWnd );
	SetForegroundWindow( hWnd );

	// If it worked, return now
	if( hWnd == GetForegroundWindow() )
	{
		// Try to modify the system settings (since this is the foreground
		// process, we are allowed to do this)
		SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0,
			SPIF_SENDCHANGE );
		return;
	}

	// For other Windows versions than 95 & NT4.0, the standard approach
	// may not work, so if we failed we have to "trick" Windows into
	// making our window the foreground window: Iconify and restore
	// again. It is ugly, but it seems to work (we turn off those annoying
	// zoom animations to make it look a bit better at least).

	// We try this a few times, just to be on the safe side of things...
	do
	{
		// Iconify & restore
		ShowWindow( hWnd, SW_HIDE );
		ShowWindow( hWnd, SW_SHOWMINIMIZED );
		ShowWindow( hWnd, SW_SHOWNORMAL );

		// Try to get focus
		BringWindowToTop( hWnd );
		SetForegroundWindow( hWnd );

		// We do not want to keep going on forever, so we keep track of
		// how many times we tried
		try_count ++;
	}
	while( hWnd != GetForegroundWindow() && try_count <= 3 );

	// Try to modify the system settings (since this is now hopefully the
	// foreground process, we are probably allowed to do this)
	SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0,
		SPIF_SENDCHANGE );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
