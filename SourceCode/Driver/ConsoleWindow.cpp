/*
=============================================================================
	File:	ConsoleWindow.cpp
	Desc:	
=============================================================================
*/
#include <Driver_PCH.h>
#pragma hdrstop
#include <Driver.h>

#include <fcntl.h>	// _O_TEXT (for creating debug console)

#include "ConsoleWindow.h"

mxNAMESPACE_BEGIN

//
//	Redirect_IO_To_Console
//
//	Redirects standard C I/O streams to the console window
//	( which can be a very useful tool for debugging )
//
static
void Redirect_IO_To_Console()
{
#if 0
	BOOL bOk;

#if 0
	// Set the size and position of a console screen buffer's window
	// so that it doesn't overlap with the main window.

	RECT mainWindowRect, desktopWindowRect;
	::GetWindowRect( hWnd, &mainWindowRect );
	::GetWindowRect( ::GetDesktopWindow(), &desktopWindowRect );

	SMALL_RECT newSize;
	newSize.Left	= mainWindowRect.right;
	newSize.Top		= mainWindowRect.bottom;
	newSize.Right	= desktopWindowRect.right;
	newSize.Bottom	= desktopWindowRect.bottom;
#else
	RECT desktopWindowRect;
	::GetWindowRect( ::GetDesktopWindow(), &desktopWindowRect );
	SMALL_RECT newSize;
	newSize.Left	= desktopWindowRect.right;
	newSize.Top		= desktopWindowRect.bottom;
	newSize.Right	= desktopWindowRect.right;
	newSize.Bottom	= desktopWindowRect.bottom;
#endif

	::SetConsoleWindowInfo(
		::GetStdHandle( STD_OUTPUT_HANDLE ),
		TRUE,
		&newSize
	);

	CONSOLE_SCREEN_BUFFER_INFO  coninfo;

	// Set the screen buffer to be big enough to let us scroll text.
	bOk = ::GetConsoleScreenBufferInfo( ::GetStdHandle(STD_OUTPUT_HANDLE), & coninfo );
	if( !bOk ) {
		mxWarn("Failed to initialize debug console.");
	}

	// Get the size of the largest possible console window,
	// based on the current font and the size of the display.

	const COORD maxConsoleWndSize = ::GetLargestConsoleWindowSize( ::GetStdHandle(STD_OUTPUT_HANDLE) );

	// seems that console can have more lines than 'maxConsoleWndSize.Y'
#if 1
	// maximum number of lines and columns the output console can have
	const UINT maxColumns = maxConsoleWndSize.X;
	const UINT maxRows = maxConsoleWndSize.Y;
#else
	const UINT maxColumns = 1024;
	const UINT maxRows = 1024;
#endif

	coninfo.dwSize.X = maxColumns;
	coninfo.dwSize.Y = maxRows;

	::SetConsoleScreenBufferSize( ::GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize );

#endif

	int  hConHandle;
	long lStdHandle;
	FILE * fp;

	// Redirect unbuffered STDOUT to the console.

	lStdHandle = (long) ::GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = ::_open_osfhandle( lStdHandle, _O_TEXT );
	fp = ::_fdopen( hConHandle, "w" );
	*stdout = *fp;
	::setvbuf( stdout, nil, _IONBF, 0 );

	// Redirect unbuffered STDERR to the console.
	lStdHandle = (long) ::GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = ::_open_osfhandle( lStdHandle, _O_TEXT );
	fp = ::_fdopen( hConHandle, "w" );
	*stderr = *fp;
	::setvbuf( stderr, nil, _IONBF, 0 );
}

/*
========================================================================
The following function retrieves the current console application window handle (HWND).
If the function succeeds, the return value is the handle of the console window.
If the function fails, the return value is NULL. Some error checking is omitted, for brevity.


Because multiple windows may have the same title, you should change the current console window title to a unique title.
This will help prevent the wrong window handle from being returned.
Use SetConsoleTitle() to change the current console window title.

Here is the process: 
Call GetConsoleTitle() to save the current console window title. 
Call SetConsoleTitle() to change the console title to a unique title. 
Call Sleep(40) to ensure the window title was updated. 
Call FindWindow(NULL, uniquetitle), to obtain the HWND this call returns the HWND -- or NULL if the operation failed. 
Call SetConsoleTitle() with the value retrieved from step 1, to restore the original window title. 
You should test the resulting HWND. For example, you can test to see if the returned HWND corresponds with the current process by calling GetWindowText() on the HWND and comparing the result with GetConsoleTitle(). 

The resulting HWND is not guaranteed to be suitable for all window handle operations.

========================================================================
*/
HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
	// WindowTitle.
	char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
	// WindowTitle.

	// Fetch current window title.

	GetConsoleTitleA(pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.

	mxSPrintfAnsi(
		pszNewWindowTitle,
		NUMBER_OF(pszNewWindowTitle),
		"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.

	SetConsoleTitleA(pszNewWindowTitle);

	// Ensure window title has been updated.

	Sleep(40);

	// Look for NewWindowTitle.

	hwndFound=FindWindowA(NULL, pszNewWindowTitle);

	// Restore original window title.

	SetConsoleTitleA(pszOldWindowTitle);

	return(hwndFound);
}



/*================================
		CConsole
================================*/

/* 

Author: 
Gregor S.
Modifications by Samuel Batista 2008

E-Mail: 
foxhunter@hlla.net

Date: 
08/20/2002 

Description: 
This class is a simple wrapper for some console functions.
The main goal is to keep it that simple so that even
beginners have no problems implementing it in their apps.
You can write debug outout or anything else you wish into
the console in different colors.

Usage:
Copy Console.cpp (this file) and Console.h into the directory
of your application. Insert them into the workspace if you wish.
Next, add the line '#include "Console.h"' into one of your source
files and add a member variable of tyle CConsole.

Copyright & disclaimer: 
Do want you want to do with this class: Modify it, extend it to your
needs. Use it in both Freeware, Shareware or commercial applications,
but make sure to leave these comments at the top of this source file 
intact and don't remove my name and E-mail address.

Modifications (by Samuel Batista 2008):
Changed the Output function to take in strings.
Removed some functions that were not required for the profiler.

*/

// macro for the Clear() function
#define ERR(bSuccess) { if(!bSuccess) return; }
// macro to check whether hConsole is valid
#define CHECK(hHandle) { if(hHandle == NULL) return; };

CConsole::CConsole(ENoInit)
{
	hConsole = NULL;
}
CConsole::CConsole(const char* szTitle)
{
	hConsole = NULL;
	
	if( !Create(szTitle) )
	{
		mxWarnf("Failed to create console window with title %s",szTitle);
	}
}
bool CConsole::Create(const char *szTitle)
{
	// Has console been already created?
	if(hConsole != NULL)
		return false;

	// Allocate a new console for our app
	if(!AllocConsole())
		return false;

	// Create the actual console
	hConsole = CreateFileA("CONOUT$", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if(hConsole == INVALID_HANDLE_VALUE)
		return false;

	if(SetConsoleMode(hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT ) == 0)
		return false;

	//DisableClose();

	// set the console title
	if(szTitle != NULL)
		SetConsoleTitleA(szTitle);

	Redirect_IO_To_Console();

	return true;
}
bool CConsole::IsOpen() const
{
	return hConsole != NULL;
}
void CConsole::DisableClose()
{
	CHECK(hConsole);

	HWND hWnd = GetHWND();

	// disable the [x] button if we found our console
	if(hWnd != NULL)
	{
		HMENU hMenu = GetSystemMenu(hWnd,0);
		if(hMenu != NULL)
		{
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			DrawMenuBar(hWnd);
		}
	}
}

static
WORD MapColor(EConsoleTextColor color)
{
	switch(color)
	{
		case COLOR_DEFAULT:				return 0;
		case COLOR_FOREGROUND_BLUE:		return FOREGROUND_BLUE;
		case COLOR_FOREGROUND_GREEN:	return FOREGROUND_GREEN;
		case COLOR_FOREGROUND_RED:		return FOREGROUND_RED;
		case COLOR_FOREGROUND_INTENSITY:return FOREGROUND_INTENSITY;
		case COLOR_BACKGROUND_BLUE:		return BACKGROUND_BLUE;
		case COLOR_BACKGROUND_GREEN:	return BACKGROUND_GREEN;
		case COLOR_BACKGROUND_RED:		return BACKGROUND_RED;
		case COLOR_BACKGROUND_INTENSITY:return BACKGROUND_INTENSITY;
	default:
		Unreachable;
	}
	return 0;
}

void CConsole::Color(EConsoleTextColor color)
{
	CHECK(hConsole);

	WORD wColor = MapColor(color);
	
	if(wColor != 0) {
		// change font and/or background color
		SetConsoleTextAttribute(hConsole, wColor );
	}
	else {
		// no color specified, reset to defaults (white font on black background)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // white text on black bg
	}
}

void CConsole::Put( const mxChar* msg )
{
	AssertPtr(msg);

	DWORD		dwWritten;

	SizeT strLen = mxStrLenUnicode( msg );

	// if not parameter set, write a new line
	WriteConsole( hConsole, msg, strLen, &dwWritten, 0 );
}

void CConsole::SetTitle(const char *newTitle)
{
	// self-explanatory
	SetConsoleTitleA(newTitle);
}

const char* CConsole::GetTitle() const
{
	mxMT_UNSAFE;
	// get the title of our console and return it
	static char szWindowTitle[16] = "";
	GetConsoleTitleA(szWindowTitle,sizeof(szWindowTitle));

	return szWindowTitle;
}

HWND CConsole::GetHWND()
{
	return GetConsoleHwnd();
}

void CConsole::Show(bool bShow)
{
	CHECK(hConsole);

	// get out console's HWND and show/hide the console
	HWND hWnd = GetHWND();
	if(hWnd != NULL)
		ShowWindow(hWnd, SW_HIDE ? SW_SHOW : bShow);
}


void CConsole::Clear()
{
	CHECK(hConsole);

	/***************************************/
	// This code is from one of Microsoft's
	// knowledge base articles, you can find it at 
	// http://support.microsoft.com/default.aspx?scid=KB;EN-US;q99261&
	/***************************************/

	COORD coordScreen = { 0, 0 };

	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */ 
	DWORD dwConSize; 

	/* get the number of character cells in the current buffer */ 

	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	ERR(bSuccess);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */ 

	bSuccess = FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten );
	ERR(bSuccess);

	/* get the current text attribute */ 

	bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
	ERR(bSuccess);

	/* now set the buffer's attributes accordingly */ 

	bSuccess = FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );
	ERR(bSuccess);

	/* put the cursor at (0, 0) */ 

	bSuccess = SetConsoleCursorPosition( hConsole, coordScreen );
	ERR(bSuccess);
}

void CConsole::Close()
{
	// free the console, now it can't be used anymore until we Create() it again
	FreeConsole();
	hConsole = NULL;
}

void CConsole::setTopLeft( int x, int y )
{
	BOOL bOk;

	CONSOLE_SCREEN_BUFFER_INFO  coninfo;

	// Set the screen buffer to be big enough to let us scroll text.
	bOk = ::GetConsoleScreenBufferInfo( ::GetStdHandle(STD_OUTPUT_HANDLE), & coninfo );
	if( !bOk ) {
		mxWarn("Failed to initialize debug console.");
	}

	SMALL_RECT newSize;
	newSize.Left	= x;
	newSize.Top		= y;
	newSize.Right	= 800 + x;
	newSize.Bottom	= 600 + y;

	::SetConsoleWindowInfo(
		::GetStdHandle(STD_OUTPUT_HANDLE),
		TRUE,
		&newSize
	);
}


/*
On windows you can move the cursor to any location on the screen, then start printing from there:

	COORD c;
	c.X = x; c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
*/

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
