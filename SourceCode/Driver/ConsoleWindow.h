/*
=============================================================================
	File:	ConsoleWindow.h
	Desc:	
=============================================================================
*/

#ifndef __MX_CONSOLE_WINDOW_H__
#define __MX_CONSOLE_WINDOW_H__

mxNAMESPACE_BEGIN

/* 

Author: 
Samuel Batista 2008

E-Mail: 
sambatista@live.com

Date: 
10/08/2008

Description: 
This class was written by Hernan Di Pietro and heavily modified by me.
It logs function calls and the time that each function takes to execute.
If the user desires it also outputs a text LOG file with all functions profiled
and the time each function took to execute. It supports multiple profiled functions
(within loops) and multiple calls to functions that are meant to be profiled once.
You can also profile functions within a functio, so for example, you can profile
the time it takes for something to initialize and profile individual functions
called within that initialize function.

The Console class was written by Gregor S. see the .cpp file for further information.

Usage:
Copy Profiler.h (this file) and Profiler.cpp into the directory
of your application. Insert them into the workspace if you wish.
Next, add the line '#include "Profiler.h"' into one of your source
files and add a pointer to the CProfiler class.
Example Startup:

CProfiler profiler = CProfiler::GetInstance();
profiler->Initialize(parameters);

Then, before a function call call profiler->ProfileStart(params);
and after the function call do profiler->ProfileEnd(params);

Don't forget to call profiler->Shutdown(); after the program has finished
executing, otherwise it won't output the results to a LOG file.

Copyright & disclaimer: 
Do want you want to do with this class: Modify it, extend it to your
needs. Use it in both Freeware, Shareware or commercial applications,
but make sure to leave these comments at the top of this source file 
intact and don't remove my name and E-mail address.
*/
///////////////////////////////////////////////////
// See Profiler.cpp for implementation
//////////////////////////////////////////////////


enum EConsoleTextColor
{
	COLOR_DEFAULT = 0,
	COLOR_FOREGROUND_BLUE,			// Text color contains blue.
	COLOR_FOREGROUND_GREEN,			// Text color contains green.
	COLOR_FOREGROUND_RED,			// Text color contains red.
	COLOR_FOREGROUND_INTENSITY,		// Text color is intensified.
	COLOR_BACKGROUND_BLUE,			// Background color contains blue.
	COLOR_BACKGROUND_GREEN,			// Background color contains green.
	COLOR_BACKGROUND_RED,			// Background color contains red.
	COLOR_BACKGROUND_INTENSITY,		// Background color is intensified.
};


class CConsole {
public:
	CConsole(ENoInit);
	// automatically creates the console
	CConsole(const char* szTitle = "Debug Console");

	// create the console
	bool   Create(const char* szTitle);

	bool IsOpen() const;

	void DisableClose();

	// set color for output
	void   Color(EConsoleTextColor color);
	
	// write output to console
	void   Put( const mxChar* msg );

	// show/hide the console
	void   Show(bool bShow = true);

	// set and get title of console
	void   SetTitle(const char* newTitle);
	const char* GetTitle() const;

	// get HWND and/or HANDLE of console
	HWND   GetHWND();
	HANDLE GetHandle();

	// clear all output
	void   Clear();

	// close the console and delete it
	void   Close();

	void setTopLeft( int x, int y );

private:
	HANDLE	hConsole;

	PREVENT_COPY(CConsole);
};

mxNAMESPACE_END

#endif // ! __MX_CONSOLE_WINDOW_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
