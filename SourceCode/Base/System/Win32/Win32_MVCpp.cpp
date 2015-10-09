/*
=============================================================================
	File:	Win32_MVCpp.cpp
	Desc:	Win32-specific code for MVC++ compilers.
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <sys/timeb.h>	// for ftime()
#include <direct.h>		// for chdir()

#include <pdh.h>
#if MX_AUTOLINK
	#pragma comment(lib, "pdh.lib")
#endif

#include <Base/Memory/Memory.h>

#pragma message("Compiling "MX_BUILD_MODE_STRING" version on Win32 with "MX_COMPILER_NAME"...")

mxNAMESPACE_BEGIN

extern void SetThreadName( DWORD dwThreadID, const char* threadName );
extern void DetectCPU( mxSystemInfo & sysInfo );

/**/
// e.g.
// C:/myprojects/game/game.exe -> C:/myprojects/game/
// C:\basedir\dev\demo.exe -> C:\basedir\dev\
//
static void StripFileNameAndExtension( mxChar* str, UINT length )
{
	if( !str || !length ) {
		return;
	}
	UINT iChar;
	for( iChar = length - 1; iChar > 0; iChar-- )
	{
		if( (str[iChar - 1] == PATHSEPARATOR_CHAR) || (str[iChar - 1] == PATHSEPARATOR_CHAR2) )
		{
			break;
		}
	}
	str[ iChar ]=0;
}
/**/
/**/
// e.g. "     Hello, world!" -> "Hello, world!"
//
static void RemoveWhitespacesFromStart( char* str )
{
	if( !str ) {
		return;
	}
	char * p = str;
	while( *p && *p == ' ' ) {
		p++;
	}
	if( p == str ) {
		return;
	}
	UINT i = 0;
	while( *p ) {
		str[i++] = *p;
		p++;
	}
	str[i] = '\0';
}
/**/

template< UINT NUM_CHARS >
void NormalizeDirectory( mxChar (&buffer)[NUM_CHARS] )
{
#if UNICODE
	ConvertBackSlashesUnicode( buffer );
#else
	ConvertBackSlashesAnsi( buffer );
#endif
}

//
//	CSystemLayer_Win32
//
class CSystemLayer_Win32 {
public_internal:
	//mxOPTIMIZE("these waste exe space a bit:")
	mxSystemInfo	sysInfo;
	mxChar			OSVersion[ FS_MAX_PATH ];
	mxChar			OSDirectory[ FS_MAX_PATH ];
	mxChar			systemWindowsDirectory[ FS_MAX_PATH ];

	mxChar			launchDirectory[ FS_MAX_PATH ];	// full base path including exe file name
	mxChar			exeFileName[ MAX_FILENAME_CHARS ];	// fully qualified executable file name including extension

	mxChar			lauch_driveName[16];	// Optional drive letter, followed by a colon (:) (e.g. "C:").
	mxChar			lauch_baseDirName[128];	// Optional directory path, including trailing slash (e.g. "\base\data\").
	mxChar			lauch_baseExeName[128];	// Base filename (no extension) (e.g. "game").
	mxChar			lauch_ExeExtension[128];// Optional filename extension, including leading period (.) (e.g. ".exe").

	mxChar			userName[ 32 ];
	mxChar			computerName[ 32 ];

	HANDLE			hMainThread;
	HINSTANCE		hAppInstance;	// Handle to the application instance.

	HANDLE			hConsoleInput;
	HANDLE			hConsoleOutput;
	HANDLE			hConsoleError;

	HQUERY			hCpuUsageQuery;
	HCOUNTER		hCpuUsageCounter;

	mxTimer			timer;	// starts when the app launches

	MEMORYSTATUSEX	memoryStatusOnExeLaunch;	// Initial memory status.
	MEMORYSTATUSEX	memoryStatusOnExeShutdown;

	shared_ bool	bInitialized;

public:

	void OneTimeInit()
	{
		if( bInitialized )
		{
#if MX_DEBUG
			mxWarn("Win32 system: double initialization!\n");
#endif
			return;
		}

		//NOTE: this should be before any other variable is initialized!
		ZeroMemory( this, sizeof(*this) );


		timer.Initialize();
		timer.Reset();


		//
		//	Initialize platform info.
		//
		sysInfo.platform = EPlatform::Platform_Win32;

		hMainThread = GetCurrentThread();

		SetThreadName( (DWORD)hMainThread, "Main Thread" );

		//
		// Force the main thread to run on CPU 0 to avoid problems with QueryPerformanceCounter().
		//
		::SetThreadAffinityMask( hMainThread, 1 );


		hConsoleInput = ::GetStdHandle( STD_INPUT_HANDLE );
		hConsoleOutput = ::GetStdHandle( STD_OUTPUT_HANDLE );
		hConsoleError = ::GetStdHandle( STD_ERROR_HANDLE );


		DetectCPU( sysInfo );

		//
		// Detect the Windows version.
		//
		{
			OSVERSIONINFOEX osvi;
			BOOL bOsVersionInfoEx;

			::ZeroMemory( &osvi, sizeof(OSVERSIONINFOEX) );
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			bOsVersionInfoEx = ::GetVersionEx( (OSVERSIONINFO*) &osvi );
			if ( !bOsVersionInfoEx )
			{
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				if( ! GetVersionEx((OSVERSIONINFO *) &osvi) ) {
					goto L_End;
				}
			}

			switch( osvi.dwPlatformId )
			{
			case VER_PLATFORM_WIN32_NT :
				if( osvi.dwMajorVersion <= 4 )
					mxStrCat( OSVersion, TEXT("Microsoft Windows NT ") );
				else
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
					mxStrCat( OSVersion, TEXT("Microsoft Windows 2000 ") );
				else
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
					mxStrCat( OSVersion, TEXT("Microsoft Windows XP ") );
				else
				if( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
					mxStrCat( OSVersion, TEXT("Microsoft Windows Vista ") );
				else
				if( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
					mxStrCat( OSVersion, TEXT("Microsoft Windows 7 ") );	// or Windows 2008 R2
				else
					mxStrCat( OSVersion, TEXT("Microsoft Windows ") );

				if( bOsVersionInfoEx )
				{
#ifdef VER_SUITE_ENTERPRISE
					if ( osvi.wProductType == VER_NT_WORKSTATION )
					{
#ifndef __BORLANDC__
						if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
							mxStrCat( OSVersion, TEXT("Personal ") );
						else
							mxStrCat( OSVersion, TEXT("Professional ") );
#endif
					}
					else if ( osvi.wProductType == VER_NT_SERVER )
					{
						if( osvi.wSuiteMask & VER_SUITE_DATACENTER ) {
							mxStrCat( OSVersion, TEXT("DataCenter Server ") );
						}
						else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) {
							mxStrCat( OSVersion, TEXT("Advanced Server ") );
						}
						else {
							mxStrCat( OSVersion, TEXT("Server ") );
						}
					}
#endif
				}
				else
				{
					HKEY hKey;
					mxChar szProductType[80];
					DWORD dwBufLen;

					::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
						TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
						0, KEY_QUERY_VALUE, &hKey );

					::RegQueryValueEx( hKey, TEXT("ProductType"), nil, nil,
						(LPBYTE) szProductType, &dwBufLen );

					::RegCloseKey( hKey );

					if( mxStrCmpI( TEXT("WINNT"), szProductType) == 0 ) {
						mxStrCat( OSVersion, TEXT("Professional ") );
					}
					if( mxStrCmpI( TEXT("LANMANNT"), szProductType) == 0 ) {
						mxStrCat( OSVersion, TEXT("Server ") );
					}
					if( mxStrCmpI( TEXT("SERVERNT"), szProductType) == 0 ) {
						mxStrCat( OSVersion, TEXT("Advanced Server ") );
					}
				}

				// Display version, service pack (if any), and build number.

				mxChar	tmp[ MAX_STRING_CHARS ];

				if( osvi.dwMajorVersion <= 4 )
				{
					MX_SPRINTF
					(
						tmp,
						TEXT("version %ld.%ld %s (Build %ld)"),
						osvi.dwMajorVersion,
						osvi.dwMinorVersion,
						osvi.szCSDVersion,
						osvi.dwBuildNumber & 0xFFFF
					);
				}
				else
				{
					MX_SPRINTF
					(
						tmp,
						TEXT("%s (Build %ld)"), osvi.szCSDVersion,
						osvi.dwBuildNumber & 0xFFFF
					);
				}

				mxStrCat( OSVersion, tmp);
				break;

			case VER_PLATFORM_WIN32_WINDOWS:
				if( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0 )
				{
					mxStrCat( OSVersion, TEXT("Microsoft Windows 95 ") );
					if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' ) {
						mxStrCat( OSVersion, TEXT("OSR2 ") );
					}
				}

				if( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10 )
				{
					mxStrCat( OSVersion, TEXT("Microsoft Windows 98 ") );
					if ( osvi.szCSDVersion[1] == 'A' ) {
						mxStrCat( OSVersion, TEXT("SE ") );
					}
				}

				if( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90 ) {
					mxStrCat( OSVersion, TEXT("Microsoft Windows Me ") );
				}
				break;

			case VER_PLATFORM_WIN32s:
				mxStrCat( OSVersion, TEXT("Microsoft Win32s ") );
				break;

			}//switch
		}
L_End:
		sysInfo.OSName = OSVersion;

		// determine OS directory
		{
			//GetSystemWindowsDirectory
			::GetWindowsDirectory(
				OSDirectory,
				ARRAY_SIZE(OSDirectory)
			);
			NormalizeDirectory( OSDirectory );
			sysInfo.OSDirectory = OSDirectory;
		}


		{
			::GetSystemDirectory(
				systemWindowsDirectory,
				ARRAY_SIZE(systemWindowsDirectory)
			);
			NormalizeDirectory( systemWindowsDirectory );
			sysInfo.SysDirectory = systemWindowsDirectory;
		}


		// determine system language
		{
			static TCHAR systemLanguage[64] = {0};

			int ret = GetLocaleInfo(
				LOCALE_SYSTEM_DEFAULT,
				LOCALE_SNATIVELANGNAME | LOCALE_NOUSEROVERRIDE,
				systemLanguage,
				ARRAY_SIZE(systemLanguage)
			);
			Assert_NZ(ret);

			sysInfo.OSLanguage = systemLanguage;
		}

		hAppInstance = ::GetModuleHandle( nil );

		//
		// Get application executable directory.
		//

/* 
	NOTE: these global vars point to the full path name of the executable file
		_pgmptr
		_get_wpgmptr
*/
		::GetModuleFileName( hAppInstance, launchDirectory, FS_MAX_PATH );
		ConvertBackSlashes( launchDirectory );


#if UNICODE
		::_wsplitpath
#else
		::_splitpath
#endif
		(
			launchDirectory,
			lauch_driveName,
			lauch_baseDirName,
			lauch_baseExeName,
			lauch_ExeExtension
		);

		// get directory name (without exe file name)
		StripFileNameAndExtension( launchDirectory, mxStrLen(launchDirectory) );

		// get exe file name
		mxStrCat( exeFileName, lauch_baseExeName );
		mxStrCat( exeFileName, lauch_ExeExtension );

		// set as current directory
		::SetCurrentDirectory( launchDirectory );

		{
			DWORD size = ARRAY_SIZE(userName);
			::GetUserName( userName, &size );
		}
		{
			DWORD size = ARRAY_SIZE(computerName);
			::GetComputerName( computerName, &size );
		}


		// Create a query object to poll cpu usage using Performance Data Helper.
		if(0)
		{
			UINT numErrors = 0;
			PDH_STATUS status;
			
			status = ::PdhOpenQuery( NULL, 0, &hCpuUsageQuery );
			if( ERROR_SUCCESS != status ) {
				numErrors++;
			}

			// Set query object to poll all cpus in the system.
			status = ::PdhAddCounter( hCpuUsageQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &hCpuUsageCounter );
  //PdhAddCounter(query, TEXT("\\Processor(0)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu0)
    //PdhAddCounter(query, TEXT("\\Processor(1)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu1)
		
			if( ERROR_SUCCESS != status ) {
				numErrors++;
			}

			if( numErrors ) {
				mxErr("Failed to initialize CPU usage counters.\n");
			}
		}



		//
		// Record current memory status.
		//
		memoryStatusOnExeLaunch.dwLength = sizeof memoryStatusOnExeLaunch;
		::GlobalMemoryStatusEx( & memoryStatusOnExeLaunch );

		// no abort/retry/fail errors
		//::SetErrorMode( SEM_FAILCRITICALERRORS );

		Win32MiniDump::Setup();

		bInitialized = true;
	}

	//--------------------------------------------------------------------------------

	void Shutdown()
	{
		// We don't have any termination requirements for Win32 at this time.
		Assert( bInitialized );
		if( bInitialized )
		{
			// Record current memory status.
			memoryStatusOnExeShutdown.dwLength = sizeof memoryStatusOnExeShutdown;
			::GlobalMemoryStatusEx( & memoryStatusOnExeShutdown );

			bInitialized = false;
		}
	}

	//--------------------------------------------------------------------------------

	CSystemLayer_Win32()
	{
		Assert( !bInitialized );

		MemZero( this, sizeof(*this) );

		bInitialized = false;
	}

	//--------------------------------------------------------------------------------

	~CSystemLayer_Win32()
	{}
};

//
//	Global variables.
//
local_	CSystemLayer_Win32	g_Win32System;
bool	CSystemLayer_Win32::bInitialized = false;

#define ENSURE_WIN32_IS_INITIALIZED	Assert( CSystemLayer_Win32::bInitialized );

//
//	mxPlatform_Init()
//
void mxPlatform_Init()
{
	// we're using VLD now
#if 0//MX_DEBUG_MEMORY

	// Send all reports to STDOUT.
	//::_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	//::_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	//::_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	//::_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	//::_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	//::_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

	// Enable run-time memory check for debug builds.
	// By setting the _CRTDBG_LEAK_CHECK_DF flag,
	// we produce a memory leak dump at exit in the visual c++ debug output window.

	int flag = ::_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ); // Get current flag

	// Turn on leak-checking bits.
	flag |= _CRTDBG_LEAK_CHECK_DF;
	flag |= _CRTDBG_ALLOC_MEM_DF;

	// Set flag to the new value.
	// That will report any leaks to the output window when upon exiting the application.
	::_CrtSetDbgFlag( flag );
#endif

	g_Win32System.OneTimeInit();
}

//
//	mxPlatform_Shutdown()
//
void mxPlatform_Shutdown()
{
	g_Win32System.Shutdown();
}

//---------------------------------------------------------------------
//		Debug output and error handling
//---------------------------------------------------------------------

enum {
	breakOnWarnings = 0,//MX_DEVELOPER,
	breakOnErrors = MX_DEVELOPER,
};

void mxPut( const char* str )
{
	//RECURSION_GUARD;
#if 0
	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"%s%s\n", "INFO: ", str );
	mxPrintfAnsi( buffer );
	GetGlobalLogger().Log( LL_Info, buffer, mxStrLenAnsi(buffer) );
#else
	mxPrintfAnsi( str );
	GetGlobalLogger().Log( LL_Info, str, mxStrLenAnsi(str) );
#endif
}

void VARARGS mxPutf( const char* fmt, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	mxPut( buffer );
}

void VARARGS mxMsgBoxf( PCSTR fmt, ... )
{
	//RECURSION_GUARD;

	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	mxPut( buffer );

	::MessageBoxA( nil, buffer, "Message", MB_OK );
}

void DebugOut( const char* message )
{
	//RECURSION_GUARD;

	// global logger will call OutputDebugString()
	//::OutputDebugStringA( message );
	GetGlobalLogger().Log( LL_Info, message, mxStrLenAnsi(message) );
}

void mxWarn( const char* str )
{
	//RECURSION_GUARD;

	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"%s%s\n", "WARN: ", str );

	mxPrintfAnsi( buffer );

	GetGlobalLogger().Log( LL_Warning, buffer, mxStrLenAnsi(buffer) );

	if(breakOnWarnings)	mxDEBUG_BREAK;
}

void VARARGS mxWarnf( const char* fmt, ... )
{
	//RECURSION_GUARD;
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	mxWarn( buffer );
}

static void EmitSoundSignal()
{

#if 0//MX_DEVELOPER
	CalendarTime currTime = CalendarTime::GetCurrentLocalTime();
	if( currTime.hour > 7 && currTime.hour < 22 )
	{
		mxBeep(200);
	}
	else
	{
		// my mom is sleeping...
		mxDEBUG_BREAK;
	}
#endif
}

void mxErr( const char* str )
{
	RECURSION_GUARD;

	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"%s%s\n", "ERROR: ", str );

	mxPrintfAnsi( buffer );

	GetGlobalLogger().Log( LL_Info, buffer, mxStrLenAnsi(buffer) );

	char	szWinErr[512];
	mxGetLastErrorString( szWinErr, NUMBER_OF(szWinErr) );

	EmitSoundSignal();



	if(breakOnErrors)	mxDEBUG_BREAK;

#if !MX_DEBUG

	if( mxIsInDebugger() )
	{
		mxStrCatAnsi( buffer,
			"\nnDo you wish to terminate the application?\n"
			"Y - Exit, N - Debug Break, Cancel - Continue execution.\n" );

		const int result = ::MessageBoxA( nil, buffer, ("Error!"), MB_YESNOCANCEL | MB_ICONERROR );
		if( IDYES == result  ) {
			mxForceExit( -1 );
		}
		else if( IDNO == result ) {
			mxDEBUG_BREAK;
		}
		else {
			// continue execution
		}
	}
	else
	{
		mxStrCatAnsi( buffer, "\nDo you wish to terminate the application?" );

		const int result = ::MessageBoxA( nil, buffer, ("Error!"), MB_YESNO | MB_ICONERROR );
		if( IDYES == result  ) {
			mxForceExit( -1 );
		}
		else {
			// continue execution
		}
	}


#endif

}

void VARARGS mxErrf( const char* fmt, ... )
{
	RECURSION_GUARD;
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	mxErr( buffer );
}

void mxFatal( const char* str )
{
	RECURSION_GUARD;

	char  buffer[ MAX_STRING_CHARS ];
	mxSPrintfAnsi( buffer, NUMBER_OF(buffer),
		"%s%s\n", "ERROR: ", str );

	GetGlobalLogger().Log( LL_Error, buffer, mxStrLenAnsi(buffer) );

	if(breakOnErrors)	mxDEBUG_BREAK;

	::MessageBoxA( nil, str, ("Fatal error, application will exit!"), MB_OK|MB_TOPMOST|MB_ICONERROR );

	if ( g_bDebugMode ) {
		mxDEBUG_BREAK;
	}

	mxForceExit( -1 );
}

void VARARGS mxFatalf( const char* fmt, ... )
{
	RECURSION_GUARD;
	char	buffer[ MAX_STRING_CHARS ];
	MX_GET_VARARGS_ANSI( buffer, fmt );

	mxFatal( buffer );
}

//---------------------------------------------------------------------
//		General system info
//---------------------------------------------------------------------

EPlatform mxCurrentPlatform()
{
	return EPlatform::Platform_Win32;
}

const char* mxGetPlatformName( EPlatform platformType )
{
	switch ( platformType )
	{
	default:
	case EPlatform::Platform_Unknown :
		return "Unknown platform";

	case EPlatform::Platform_Win32 :
		return "Win32 platform";

	case EPlatform::Platform_Win64 :
		return "Win64 platform";
	}
}

const char* mxGetCpuTypeName( ECpuType CpuType )
{
	switch ( CpuType )
	{
	default:
	case ECpuType::CPU_Unknown :
		return "Unknown CPU type";

	case ECpuType::CPU_x86_32 :
		return "x86-32";

	case ECpuType::CPU_x86_64 :
		return "x86-64";
	}
}

void mxGetSystemInfo( mxSystemInfo &OutInfo )
{
	OutInfo = g_Win32System.sysInfo;
}

//---------------------------------------------------------------------
//		DLL-related functions.
//---------------------------------------------------------------------

void * mxDLLOpen( const char* moduleName )
{
	AssertPtr(moduleName);
	return ::LoadLibraryA( moduleName );
}

void   mxDLLClose( void* module )
{
	AssertPtr(module);
	::FreeLibrary( (HMODULE)module );
}

void * mxDLLGetSymbol( void* module, const char* symbolName )
{
	AssertPtr(module);
	AssertPtr(symbolName);
	return ::GetProcAddress( (HMODULE)module, symbolName );
}

//---------------------------------------------------------------------
//		System time
//---------------------------------------------------------------------

UINT mxGetMilliseconds()
{
	return ::timeGetTime();
}

mxUInt64 mxGetTimeInMicroseconds()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.timer.GetTimeMicroseconds();
}

mxFloat64 mxGetTimeInSeconds()
{
	const mxUInt64 microseconds = mxGetTimeInMicroseconds();
	return 1e-06 * microseconds;
}

UINT mxGetClockTicks()
{
	return (UINT)::GetTickCount();
}

UINT mxGetClockTicksPerSecond()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.sysInfo.cpu.estimatedFreqHz;
}

mxReal mxGetClockTicks_F()
{
	return (mxReal) (mxInt)mxGetClockTicks();
}

mxReal mxGetClockTicksPerSecond_F()
{
	return (mxReal) (mxInt)mxGetClockTicksPerSecond();
}

//------------------------------------------------------------------------------

static CalendarTime FromWin32SystemTime( const SYSTEMTIME& t )
{
    CalendarTime calTime;
    calTime.year    = t.wYear;
	calTime.month   = (CalendarTime::Month) t.wMonth;
    calTime.weekday = (CalendarTime::Weekday) t.wDayOfWeek;
    calTime.day     = t.wDay;
    calTime.hour    = t.wHour;
    calTime.minute  = t.wMinute;
    calTime.second  = t.wSecond;
    calTime.milliSecond = t.wMilliseconds;
    return calTime;
}

//------------------------------------------------------------------------------
/*
static SYSTEMTIME ToWin32SystemTime( const CalendarTime& calTime )
{
    SYSTEMTIME t;
    t.wYear         = (WORD) calTime.year;
    t.wMonth        = (WORD) calTime.month;
    t.wDayOfWeek    = (WORD) calTime.weekday;
    t.wDay          = (WORD) calTime.day;
    t.wHour         = (WORD) calTime.hour;
    t.wMinute       = (WORD) calTime.minute;
    t.wSecond       = (WORD) calTime.second;
    t.wMilliseconds = (WORD) calTime.milliSecond;
    return t;
}
*/
//------------------------------------------------------------------------------

// OF_ains the current system time. This does not depend on the current time zone.
CalendarTime CalendarTime::GetCurrentSystemTime()
{
    SYSTEMTIME t;
    ::GetSystemTime( &t );
    return FromWin32SystemTime( t );
}

// OF_ains the current local time (with time-zone adjustment).
CalendarTime CalendarTime::GetCurrentLocalTime()
{
    SYSTEMTIME t;
    ::GetLocalTime( &t );
    return FromWin32SystemTime( t );
}

const char * CalendarTime::MonthToStr( Month e )
{
	switch( e )
	{
		case January:   return "January";
		case February:  return "February";
		case March:     return "March";
		case April:     return "April";
		case May:       return "May";
		case June:      return "June";
		case July:      return "July";
		case August:    return "August";
		case September: return "September";
		case October:   return "October";
		case November:  return "November";
		case December:  return "December";
	}
	Unreachable;
	return NULL_STRING;
}

const char * CalendarTime::WeekdayToStr( Weekday e )
{
	switch( e )
	{
		case Sunday:    return "Sunday";
		case Monday:    return "Monday";
		case Tuesday:   return "Tuesday";
		case Wednesday: return "Wednesday";
		case Thursday:  return "Thursday";
		case Friday:    return "Friday";
		case Saturday:  return "Saturday";
	}
	Unreachable;
	return NULL_STRING;
}

//------------------------------------------------------------------------------

void mxGetTime( UINT & year, UINT & month, UINT & dayOfWeek,
			UINT & day, UINT & hour, UINT & minute, UINT & second, UINT & milliseconds )
{
	SYSTEMTIME	sysTime;

	::GetLocalTime( & sysTime );

	year		= sysTime.wYear;
	month		= sysTime.wMonth;
	dayOfWeek	= sysTime.wDayOfWeek;
	day			= sysTime.wDay;
	hour		= sysTime.wHour;
	minute		= sysTime.wMinute;
	second		= sysTime.wSecond;
	milliseconds = sysTime.wMilliseconds;
}

//---------------------------------------------------------------------
//		CPU information
//---------------------------------------------------------------------

SizeT	mxGetNumCpuCores()
{
	SYSTEM_INFO	win32SysInfo;
	::GetSystemInfo( & win32SysInfo );

	return (SizeT) win32SysInfo.dwNumberOfProcessors;
}

//---------------------------------------------------------------------
//		FPU
//---------------------------------------------------------------------

FP_Class GetFPClass( const double x )
{
	int fpClass = ::_fpclass( x );

	switch ( fpClass )
	{
	case _FPCLASS_SNAN : return FP_Class::FP_SNAN;
	case _FPCLASS_QNAN : return FP_Class::FP_QNAN;
	case _FPCLASS_NINF : return FP_Class::FP_NINF;
	case _FPCLASS_NN   : return FP_Class::FP_NN;
	case _FPCLASS_ND   : return FP_Class::FP_ND;
	case _FPCLASS_NZ   : return FP_Class::FP_NZ;
	case _FPCLASS_PZ   : return FP_Class::FP_PZ;
	case _FPCLASS_PD   : return FP_Class::FP_PD;
	case _FPCLASS_PN   : return FP_Class::FP_PN;
	case _FPCLASS_PINF : return FP_Class::FP_PINF;
	default:	Unreachable; return FP_Class::FP_SNAN;
	}
}

FASTBOOL IsNaN( const double x ) {
	return ::_isnan( x );
}

FASTBOOL IsFinite( const double x ) {
	return ::_finite( x );
}

mxSWIPED("Bullet, John McCutchan");

namespace {

static UINT g_fpuOriginalMask = 0;

static UINT F_FpuDebugMaskToMicrosoft( FPU_Control::EDebugMask mask )
{
	UINT cw = ::_controlfp( 0, 0 ); // get default mask
	if (mask & FPU_Control::FPU_DEBUG_DENORMAL)
	{
		cw &= ~(EM_DENORMAL);
	}
	if (mask & FPU_Control::FPU_DEBUG_INVALID)
	{
		cw &= ~(EM_INVALID);
	}
	if (mask & FPU_Control::FPU_DEBUG_OVERFLOW)
	{
		cw &= ~(EM_OVERFLOW);
	}
	if (mask & FPU_Control::FPU_DEBUG_ZERODIVIDE)
	{
		cw &= ~(EM_ZERODIVIDE);
	}
	if (mask & FPU_Control::FPU_DEBUG_UNDERFLOW)
	{
		cw &= ~(EM_UNDERFLOW);
	}
	return cw;
}

static void F_FpuDebugMicrosoftSet( FPU_Control::EDebugMask mask )
{
	::_clearfp(); // clear 
	UINT msMask = F_FpuDebugMaskToMicrosoft( mask );
	g_fpuOriginalMask = ::_controlfp( msMask, MCW_EM );
}

static void F_FpuDebugMicrosoftReset()
{
	::_controlfp( g_fpuOriginalMask, MCW_EM );
}

}//anonymous namespace

void FPU_Control::EnableFpuDebug( FPU_Control::EDebugMask mask )
{
	F_FpuDebugMicrosoftSet( mask );
}

void FPU_Control::DisableFpuDebug()
{
	F_FpuDebugMicrosoftReset();
}

//---------------------------------------------------------------------
//		Memory stats
//---------------------------------------------------------------------

SizeT mxGetSystemRam()
{
#if 0
	ULONGLONG	TotalMemoryInKilobytes;
	BOOL bOk = ::GetPhysicallyInstalledSystemMemory(
		&TotalMemoryInKilobytes
	);
	Assert(bOk != FALSE);
	return (SizeT) (TotalMemoryInKilobytes / 1024);

#elif 1

	MEMORYSTATUSEX  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatusEx( &memStatus );

	return (SizeT) (memStatus.ullTotalPhys / (1024*1024));

#else
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	return ((SizeT) memStatus.dwTotalPhys) / (1024*1024);
#endif
}

void FromWinMemoryStatus( const MEMORYSTATUS& memStatus, mxMemoryStatus & OutStats )
{
	OutStats.memoryLoad		= memStatus.dwMemoryLoad;
	OutStats.totalPhysical	= memStatus.dwTotalPhys / (1024*1024);
	OutStats.availPhysical	= memStatus.dwAvailPhys / (1024*1024);
	OutStats.totalPageFile	= memStatus.dwTotalPageFile / (1024*1024);
	OutStats.availPageFile	= memStatus.dwAvailPageFile / (1024*1024);
	OutStats.totalVirtual	= memStatus.dwTotalVirtual / (1024*1024);
	OutStats.availVirtual	= memStatus.dwAvailVirtual / (1024*1024);
}
void FromWinMemoryStatus( const MEMORYSTATUSEX& memStatus, mxMemoryStatus & OutStats )
{
	OutStats.memoryLoad		= memStatus.dwMemoryLoad;
	OutStats.totalPhysical	= memStatus.ullTotalPhys / (1024*1024);
	OutStats.availPhysical	= memStatus.ullAvailPhys / (1024*1024);
	OutStats.totalPageFile	= memStatus.ullTotalPageFile / (1024*1024);
	OutStats.availPageFile	= memStatus.ullAvailPageFile / (1024*1024);
	OutStats.totalVirtual	= memStatus.ullTotalVirtual / (1024*1024);
	OutStats.availVirtual	= memStatus.ullAvailVirtual / (1024*1024);
}
void mxGetCurrentMemoryStatus( mxMemoryStatus & OutStats )
{
	ENSURE_WIN32_IS_INITIALIZED;
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	FromWinMemoryStatus( memStatus, OutStats );
}

void mxGetExeLaunchMemoryStatus( mxMemoryStatus & OutStats )
{
	ENSURE_WIN32_IS_INITIALIZED;
	FromWinMemoryStatus( g_Win32System.memoryStatusOnExeLaunch, OutStats );
}

void mxGetMemoryInfo( SizeT &TotalBytes, SizeT &AvailableBytes )
{
#if 0
	MEMORYSTATUS  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatus( &memStatus );

	TotalBytes = memStatus.dwTotalPhys;
	AvailableBytes = memStatus.dwAvailPhys;
#else
	MEMORYSTATUSEX  memStatus;
	memStatus.dwLength = sizeof( memStatus );

	::GlobalMemoryStatusEx( &memStatus );

	TotalBytes = (SizeT) memStatus.ullTotalPhys;
	AvailableBytes = (SizeT) memStatus.ullAvailPhys;
#endif
}

//---------------------------------------------------------------------
//		File system
//---------------------------------------------------------------------

const mxChar* mxGetLauchDirectory()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.launchDirectory;
}

const mxChar* mxGetExeFileName()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.exeFileName;
}

const mxChar* mxGetCmdLine()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return ::GetCommandLine();
}

const mxChar* mxGetComputerName()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.computerName;
}

const mxChar* mxGetUserName()
{
	ENSURE_WIN32_IS_INITIALIZED;
	return g_Win32System.userName;
}

//---------------------------------------------------------------------
//		Miscellaneous
//---------------------------------------------------------------------

void mxSleep( FLOAT seconds )
{
	::Sleep( DWORD(seconds * 1000.0f) );
}
void mxSleepMicroseconds( ULONG usec )
{
	::Sleep( usec / 1000 );
}
void mxSleepMilliseconds( ULONG msecs )
{
	::Sleep( msecs );
}

UINT mxGetCpuUsage()
{
	ENSURE_WIN32_IS_INITIALIZED;
	PDH_STATUS	status;
	status = ::PdhCollectQueryData( g_Win32System.hCpuUsageQuery );
	if( ERROR_SUCCESS != status ) {
		mxErr("PdhCollectQueryData() failed.\n");
	}
	PDH_FMT_COUNTERVALUE value;
	status = ::PdhGetFormattedCounterValue( g_Win32System.hCpuUsageCounter, PDH_FMT_LONG, NULL, &value );
	if( ERROR_SUCCESS != status ) {
		mxErr("PdhCollectQueryData() failed.\n");
	}
	return value.longValue;
}

void mxBeep( UINT delayMs, UINT frequencyInHertz )
{
	::Beep( frequencyInHertz, delayMs );
}

void mxSetMouseCursorVisible( bool bVisible )
{
#if 0
	static HCURSOR hCursor = null;	// application cursor
	if ( !hCursor ) {
		hCursor = ::GetCursor();
	}

	if ( bVisible ) {
		::SetCursor( hCursor );
	} else {
		::SetCursor( null );
	}
#else
	::ShowCursor( bVisible );
#endif
}

void mxSetMouseCursorPosition( UINT x, UINT y )
{
	POINT point;
	point.x = x;
	point.y = y;

	::SetCursorPos( point.x, point.y );
}

void mxGetCurrentDeckstopResolution( UINT &ScreenWidth, UINT &ScreenHeight )
{
	ScreenWidth  = (UINT) ::GetSystemMetrics( SM_CXSCREEN );
	ScreenHeight = (UINT) ::GetSystemMetrics( SM_CYSCREEN );
}

void mxWaitForKeyPress()
{
	//FIXME: this is too dull and expensive
	system("PAUSE");
}

void mxCrash()
{
#if MX_DEVELOPER
	mxMsgBoxf(("Info"),("Crashing at your desire!"));
	*((int*)nil) = 1;
#endif // MX_DEVELOPER
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
