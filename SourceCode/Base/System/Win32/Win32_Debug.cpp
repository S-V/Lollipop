/*
=============================================================================
	File:	Win32_Debug.cpp
	Desc:
=============================================================================
*/
#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>
#include <errno.h>

mxSWIPED("OOOII, The MIT License, Copyright (c) 2011 Antony Arciuolo & Kevin Myers");


#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void oSetThreadNameInDebugger(DWORD _ThreadID, const char* _Name)
{
	if (_Name && *_Name)
	{
		// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		Sleep(10);
		THREADNAME_INFO i;
		i.dwType = 0x1000;
		i.szName = _Name;
		i.dwThreadID = _ThreadID ? _ThreadID : -1;
		i.dwFlags = 0;

		const static DWORD MS_VC_EXCEPTION = 0x406D1388;
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(i)/sizeof(ULONG_PTR), (ULONG_PTR*)&i);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

int oGetWindowsErrorDescription(char* _StrDestination, size_t _SizeofStrDestination, HRESULT _hResult)
{
	int len = 0;
	*_StrDestination = 0;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, static_cast<DWORD>(_hResult), 0, _StrDestination, static_cast<DWORD>(_SizeofStrDestination), 0);
	if (!*_StrDestination || !memcmp(_StrDestination, "???", 3))
	{
#if defined(_DX9) || defined(_DX10) || defined(_DX11)
		strcpy_s(_StrDestination, _SizeofStrDestination, DXGetErrorStringA(_hResult));
#else
		len = sprintf_s(_StrDestination, _SizeofStrDestination, "unrecognized error code 0x%08x", _hResult);
#endif
	}

	return len != -1 ? 0 : STRUNCATE;
}

bool oIsWindows64Bit()
{
	if (sizeof(void*) != 4) // If ptr size is larger than 32-bit we must be on 64-bit windows
		return true;

	// If ptr size is 4 bytes then we're a 32-bit process so check if we're running under
	// wow64 which would indicate that we're on a 64-bit system
	BOOL bWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bWow64);
	return !!bWow64;
}

#if 0
bool oIsAeroEnabled()
{
	BOOL enabled = FALSE;
	oV(oWinDWMAPI::Singleton()->DwmIsCompositionEnabled(&enabled));
	return !!enabled;
}
#endif

// Use the Windows Vista UI look. If this causes issues or the dialog not to appear, try other values from processorAchitecture { x86 ia64 amd64 * }
//#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

bool oWinSetLastError(HRESULT _hResult, const char* _ErrorDescPrefix)
{
	if (_hResult == oWINDOWS_DEFAULT)
		_hResult = ::GetLastError();

	char err[2048];
	char* p = err;
	size_t count = NUMBER_OF(err);
	if (_ErrorDescPrefix)
	{
		size_t len = sprintf_s(err, "%s", _ErrorDescPrefix);
		p += len;
		count -= len;
	}

	size_t len = sprintf_s(p, count, "HRESULT 0x%08x: ", _hResult);
	p += len;
	count -= len;

	if (oGetWindowsErrorDescription(p, count, _hResult))
		return false;

	// @oooii-tony: it would be nice to convert the errno a bit better, but that's
	// a lot of typing! Maybe one day...
	return oSetLastError(EINVAL, err);
}
#if 0


const char* oGetErrnoString(int _Errno)
{
	switch (_Errno)
	{
		case 0: return "OK";
		case E2BIG: return "E2BIG";
		case EACCES: return "EACCES";
		case EADDRINUSE: return "EADDRINUSE";
		case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
		case EAFNOSUPPORT: return "EAFNOSUPPORT";
		case EAGAIN: return "EAGAIN";
		case EALREADY: return "EALREADY";
		case EBADF: return "EBADF";
		case EBADMSG: return "EBADMSG";
		case EBUSY: return "EBUSY";
		case ECANCELED: return "ECANCELED";
		case EEOF: return "EEOF";
		case ECHILD: return "ECHILD";
		case ECONNABORTED: return "ECONNABORTED";
		case ECONNREFUSED: return "ECONNREFUSED";
		case ECONNRESET: return "ECONNRESET";
		case EDEADLK: return "EDEADLK";
		case EDESTADDRREQ: return "EDESTADDRREQ";
		case EDOM: return "EDOM";
		case EDQUOT: return "EDQUOT";
		case EEXIST: return "EEXIST";
		case EFAULT: return "EFAULT";
		case EFBIG: return "EFBIG";
		case EHOSTUNREACH: return "EHOSTUNREACH";
		case EIDRM: return "EIDRM";
		case EILSEQ: return "EILSEQ";
		case EINPROGRESS: return "EINPROGRESS";
		case EINTR: return "EINTR";
		case EINVAL: return "EINVAL";
		case EIO: return "EIO";
		case EISCONN: return "EISCONN";
		case EISDIR: return "EISDIR";
		case ELOOP: return "ELOOP";
		case EMFILE: return "EMFILE";
		case EMLINK: return "EMLINK";
		case EMSGSIZE: return "EMSGSIZE";
		case EMULTIHOP: return "EMULTIHOP";
		case ENAMETOOLONG: return "ENAMETOOLONG";
		case ENETDOWN: return "ENETDOWN";
		case ENETRESET: return "ENETRESET";
		case ENETUNREACH: return "ENETUNREACH";
		case ENFILE: return "ENFILE";
		case ENOBUFS: return "ENOBUFS";
		case ENODATA: return "ENODATA";
		case ENODEV: return "ENODEV";
		case ENOENT: return "ENOENT";
		case ENOEXEC: return "ENOEXEC";
		case ENOLCK: return "ENOLCK";
		case ENOLINK: return "ENOLINK";
		case ENOMEM: return "ENOMEM";
		case ENOMSG: return "ENOMSG";
		case ENOPROTOOPT: return "ENOPROTOOPT";
		case ENOSPC: return "ENOSPC";
		case ENOSR: return "ENOSR";
		case ENOSTR: return "ENOSTR";
		case ENOSYS: return "ENOSYS";
		case ENOTCONN: return "ENOTCONN";
		case ENOTDIR: return "ENOTDIR";
		case ENOTEMPTY: return "ENOTEMPTY";
		case ENOTSOCK: return "ENOTSOCK";
		case EOPNOTSUPP: return "EOPNOTSUPP";
		case ENOTTY: return "ENOTTY";
		case ENXIO: return "ENXIO";
		case EOVERFLOW: return "EOVERFLOW";
		case EPERM: return "EPERM";
		case EPIPE: return "EPIPE";
		case EPROTO: return "EPROTO";
		case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
		case EPROTOTYPE: return "EPROTOTYPE";
		case ERANGE: return "ERANGE";
		case EROFS: return "EROFS";
		case ESPIPE: return "ESPIPE";
		case ESRCH: return "ESRCH";
		case ESTALE: return "ESTALE";
		case ETIME: return "ETIME";
		case ETIMEDOUT: return "ETIMEDOUT";
		case EXDEV: return "EXDEV";
		case ECHRNG: return "ECHRNG";
		case EL2NSYNC: return "EL2NSYNC";
		case EL3HLT: return "EL3HLT";
		case EL3RST: return "EL3RST";
		case ELNRNG: return "ELNRNG";
		case EUNATCH: return "EUNATCH";
		case ENOCSI: return "ENOCSI";
		case EL2HLT: return "EL2HLT";
		case EBADE: return "EBADE";
		case EBADR: return "EBADR";
		case EXFULL: return "EXFULL";
		case ENOANO: return "ENOANO";
		case EBADRQC: return "EBADRQC";
		case EBADSLT: return "EBADSLT";
		case EBFONT: return "EBFONT";
		case ENONET: return "ENONET";
		case ENOPKG: return "ENOPKG";
		case EREMOTE: return "EREMOTE";
		case EADV: return "EADV";
		case ESRMNT: return "ESRMNT";
		case ECOMM: return "ECOMM";
		case EDOTDOT: return "EDOTDOT";
		case ENOTUNIQ: return "ENOTUNIQ";
		case EBADFD: return "EBADFD";
		case EREMCHG: return "EREMCHG";
		case ELIBACC: return "ELIBACC";
		case ELIBBAD: return "ELIBBAD";
		case ELIBSCN: return "ELIBSCN";
		case ELIBMAX: return "ELIBMAX";
		case ELIBEXEC: return "ELIBEXEC";
		case ERESTART: return "ERESTART";
		case ESTRPIPE: return "ESTRPIPE";
		case EUSERS: return "EUSERS";
		case ESOCKTNOSUPPORT: return "ESOCKTNOSUPPORT";
		case EPFNOSUPPORT: return "EPFNOSUPPORT";
		case ESHUTDOWN: return "ESHUTDOWN";
		case ETOOMANYREFS: return "ETOOMANYREFS";
		case EHOSTDOWN: return "EHOSTDOWN";
		case EUCLEAN: return "EUCLEAN";
		case ENOTNAM: return "ENOTNAM";
		case ENAVAIL: return "ENAVAIL";
		case EISNAM: return "EISNAM";
		case EREMOTEIO: return "EREMOTEIO";
		case ENOMEDIUM: return "ENOMEDIUM";
		case EMEDIUMTYPE: return "EMEDIUMTYPE";
		case ENOKEY: return "ENOKEY";
		case EKEYEXPIRED: return "EKEYEXPIRED";
		case EKEYREVOKED: return "EKEYREVOKED";
		case EKEYREJECTED: return "EKEYREJECTED";
		case EOWNERDEAD: return "EOWNERDEAD";
		case ENOTRECOVERABLE: return "ENOTRECOVERABLE";
		default: break;
	}

	Unreachable;
	static MX_THREAD_LOCAL char buf[64];
	sprintf_s(buf, "Unknown error code: %u", _Errno);
	return buf;
}
#endif

// Link to MessageBoxTimeout based on code from:
// http://www.codeproject.com/KB/cpp/MessageBoxTimeout.aspx

//Functions & other definitions required-->
typedef int (__stdcall *MSGBOXAAPI)(IN HWND hWnd, 
									IN LPCSTR lpText, IN LPCSTR lpCaption, 
									IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef int (__stdcall *MSGBOXWAPI)(IN HWND hWnd, 
									IN LPCWSTR lpText, IN LPCWSTR lpCaption, 
									IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, 
					   LPCSTR lpCaption, UINT uType, WORD wLanguageId, 
					   DWORD dwMilliseconds)
{
	static MSGBOXAAPI MsgBoxTOA = NULL;

	if (!MsgBoxTOA)
	{
		HMODULE hUser32 = GetModuleHandle(TEXT("user32.dll"));
		if (hUser32)
		{
			MsgBoxTOA = (MSGBOXAAPI)GetProcAddress(hUser32, 
				"MessageBoxTimeoutA");
			//fall through to 'if (MsgBoxTOA)...'
		}
		else
		{
			//stuff happened, add code to handle it here 
			//(possibly just call MessageBox())
			return 0;
		}
	}

	if (MsgBoxTOA)
	{
		return MsgBoxTOA(hWnd, lpText, lpCaption, 
			uType, wLanguageId, dwMilliseconds);
	}

	return 0;
}

int MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, 
					   LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds)
{
	static MSGBOXWAPI MsgBoxTOW = NULL;

	if (!MsgBoxTOW)
	{
		HMODULE hUser32 = GetModuleHandle(TEXT("user32.dll"));
		if (hUser32)
		{
			MsgBoxTOW = (MSGBOXWAPI)GetProcAddress(hUser32, 
				"MessageBoxTimeoutW");
			//fall through to 'if (MsgBoxTOW)...'
		}
		else
		{
			//stuff happened, add code to handle it here 
			//(possibly just call MessageBox())
			return 0;
		}
	}

	if (MsgBoxTOW)
	{
		return MsgBoxTOW(hWnd, lpText, lpCaption, 
			uType, wLanguageId, dwMilliseconds);
	}

	return 0;
}

void oUnixTimeToFileTime(time_t _Time, FILETIME* _pFileTime)
{
	// http://msdn.microsoft.com/en-us/library/ms724228(v=vs.85).aspx
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll = Int32x32To64(_Time, 10000000) + 116444736000000000;
	_pFileTime->dwLowDateTime = (DWORD) ll;
	_pFileTime->dwHighDateTime = ll >> 32;
}

time_t oFileTimeToUnixTime(const FILETIME* _pFileTime)
{
	if (!_pFileTime) return 0;
	// this ought to be the reverse of to_filetime
	LONGLONG ll = ((LONGLONG)_pFileTime->dwHighDateTime << 32) | _pFileTime->dwLowDateTime;
	return static_cast<time_t>((ll - 116444736000000000) / 10000000);
}

void oUnixTimeToSystemTime(time_t _Time, SYSTEMTIME* _pSystemTime)
{
	FILETIME ft;
	oUnixTimeToFileTime(_Time, &ft);
	FileTimeToSystemTime(&ft, _pSystemTime);
}

time_t oSystemTimeToUnixTime(const SYSTEMTIME* _pSystemTime)
{
	FILETIME ft;
	SystemTimeToFileTime(_pSystemTime, &ft);
	return oFileTimeToUnixTime(&ft);
}


mxSWIPED("Nebula3")
mxNAMESPACE_BEGIN


//------------------------------------------------------------------------------
/**
    Exception handler function called back by Windows when something
    unexpected happens.
*/
LONG WINAPI /*static*/
Win32MiniDump::ExceptionCallback( EXCEPTION_POINTERS* exceptionInfo )
{
    Win32MiniDump::WriteMiniDumpInternal(exceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}


//------------------------------------------------------------------------------
/**
    This static method registers our own exception handler with Windows.
*/
void /*static*/
Win32MiniDump::Setup()
{
	:: SetUnhandledExceptionFilter( Win32MiniDump::ExceptionCallback );
}

//------------------------------------------------------------------------------
/**
    This method is called by assert() and error() to write out
    a minidump file.
*/
bool /*static*/
Win32MiniDump::WriteMiniDump()
{
    return Win32MiniDump::WriteMiniDumpInternal( 0 );
}

//------------------------------------------------------------------------------

bool /*static*/
Win32MiniDump::BuildMiniDumpFilename( char *outDumpFileName, UINT numChars )
{
	// get our module filename directly from windows
	char exeFileName[256] = {0};
	char driveName[32] = {0};
	char folderName[256] = {0};
	char pureFileName[256] = {0};
	char fileExtension[32] = {0};
	
	DWORD numBytes = ::GetModuleFileNameA( NULL, exeFileName, sizeof(exeFileName) - 1 );
	if( numBytes > 0 )
	{
		int ret = _splitpath_s(
			exeFileName,
			driveName,
			folderName,
			pureFileName,
			fileExtension
		);
		if( ret != 0 ) {
			mxWarnf( "_splitpath_s() failed with error code '%d'.\n", ret );
		}

		// get the current calender time
		SYSTEMTIME t;
		::GetLocalTime( &t );

		char timeStr[256];
		mxSPrintfAnsi( timeStr, ARRAY_SIZE(timeStr), "%04d-%02d-%02d_%02d-%02d-%02d",
										t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );

		// build the dump filename
		mxSPrintfAnsi( outDumpFileName, numChars, "%s%s_%s.dmp", folderName, pureFileName, timeStr );

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
    Private method to write a mini-dump with extra exception info. This 
    method is either called from the public WriteMiniDump() method or
    from the ExceptionCallback() function.
*/
bool /*static*/
Win32MiniDump::WriteMiniDumpInternal( EXCEPTION_POINTERS* exceptionInfo )
{
	char dumpFilename[512] = {0};
    if( BuildMiniDumpFilename( dumpFilename, ARRAY_SIZE(dumpFilename) ) )
    {
		char msgBoxText[1024] = {0};
		MX_SPRINTF_ANSI( msgBoxText,
			"An unhandled error occured.\nDo you wish to dump debug info into file '%s'?",
			dumpFilename
		);
		const int ret = ::MessageBoxA( NULL, msgBoxText, "Fatal error", MB_OKCANCEL );
		if( IDCANCEL == ret ) {
			return false;
		}

        HANDLE hFile = 
			::CreateFileA(
				dumpFilename,             // lpFileName
				GENERIC_WRITE,            // dwDesiredAccess
				FILE_SHARE_READ,          // dwShareMode
				0,                        // lpSecurityAttributes
				CREATE_ALWAYS,            // dwCreationDisposition,
				FILE_ATTRIBUTE_NORMAL,    // dwFlagsAndAttributes
				NULL                      // hTemplateFile
		);
        if( NULL != hFile )
        {
			HANDLE hProc = ::GetCurrentProcess();
            DWORD procId = ::GetCurrentProcessId();
            BOOL res = FALSE;
            if( NULL != exceptionInfo )
            {
                // extended exception info is available
                MINIDUMP_EXCEPTION_INFORMATION extInfo = { 0 };
                extInfo.ThreadId = ::GetCurrentThreadId();
                extInfo.ExceptionPointers = exceptionInfo;
                extInfo.ClientPointers = TRUE;
                res = ::MiniDumpWriteDump( hProc, procId, hFile, MiniDumpNormal, &extInfo, NULL, NULL );
            }
            else
            {
                // extended exception info is not available
                res = ::MiniDumpWriteDump( hProc, procId, hFile, MiniDumpNormal, NULL, NULL, NULL );
            }
            ::CloseHandle( hFile );
            return true;
        }
    }

    return false;
}

/**********************************************************************
 * 
 * StackWalker.cpp
 *
 *
 * History:
 *  2005-07-27   v1    - First public release on http://www.codeproject.com/
 *                       http://www.codeproject.com/threads/StackWalker.asp
 *  2005-07-28   v2    - Changed the params of the constructor and ShowCallstack
 *                       (to simplify the usage)
 *  2005-08-01   v3    - Changed to use 'CONTEXT_FULL' instead of CONTEXT_ALL 
 *                       (should also be enough)
 *                     - Changed to compile correctly with the PSDK of VC7.0
 *                       (GetFileVersionInfoSizeA and GetFileVersionInfoA is wrongly defined:
 *                        it uses LPSTR instead of LPCSTR as first paremeter)
 *                     - Added declarations to support VC5/6 without using 'dbghelp.h'
 *                     - Added a 'pUserData' member to the ShowCallstack function and the 
 *                       PReadProcessMemoryRoutine declaration (to pass some user-defined data, 
 *                       which can be used in the readMemoryFunction-callback)
 *  2005-08-02   v4    - OnSymInit now also outputs the OS-Version by default
 *                     - Added example for doing an exception-callstack-walking in main.cpp
 *                       (thanks to owillebo: http://www.codeproject.com/script/profile/whos_who.asp?id=536268)
 *  2005-08-05   v5    - Removed most Lint (http://www.gimpel.com/) errors... thanks to Okko Willeboordse!
 *  2008-08-04   v6    - Fixed Bug: Missing LEAK-end-tag
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=2502890#xx2502890xx
 *                       Fixed Bug: Compiled with "WIN32_LEAN_AND_MEAN"
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=1824718#xx1824718xx
 *                       Fixed Bug: Compiling with "/Wall"
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=2638243#xx2638243xx
 *                       Fixed Bug: Now checking SymUseSymSrv
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1388979#xx1388979xx
 *                       Fixed Bug: Support for recursive function calls
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1434538#xx1434538xx
 *                       Fixed Bug: Missing FreeLibrary call in "GetModuleListTH32"
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=1326923#xx1326923xx
 *                       Fixed Bug: SymDia is number 7, not 9!
 *  2008-09-11   v7      For some (undocumented) reason, dbhelp.h is needing a packing of 8!
 *                       Thanks to Teajay which reported the bug...
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=2718933#xx2718933xx
 *  2008-11-27   v8      Debugging Tools for Windows are now stored in a different directory
 *                       Thanks to Luiz Salamon which reported this "bug"...
 *                       http://www.codeproject.com/KB/threads/StackWalker.aspx?msg=2822736#xx2822736xx
 *  2009-04-10   v9      License slihtly corrected (<ORGANIZATION> replaced)
 *  2010-04-15   v10     Added support for VS2010 RTM
 *  2010-05-2ß   v11     Now using secure MyStrcCpy. Thanks to luke.simon:
 *                       http://www.codeproject.com/KB/applications/leakfinder.aspx?msg=3477467#xx3477467xx
 *
 * LICENSE (http://www.opensource.org/licenses/bsd-license.php)
 *
 *   Copyright (c) 2005-2010, Jochen Kalmbach
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without modification, 
 *   are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer. 
 *   Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution. 
 *   Neither the name of Jochen Kalmbach nor the names of its contributors may be 
 *   used to endorse or promote products derived from this software without 
 *   specific prior written permission. 
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 *   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 *   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#ifdef ENABLE_STACK_WALKER

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "version.lib")  // for "VerQueryValue"
#pragma warning(disable:4826)

// If VC7 and later, then use the shipped 'dbghelp.h'-file
#pragma pack(push,8)
#if _MSC_VER >= 1300
#include <dbghelp.h>
#else
// inline the important dbghelp.h-declarations...
typedef enum {
    SymNone = 0,
    SymCoff,
    SymCv,
    SymPdb,
    SymExport,
    SymDeferred,
    SymSym,
    SymDia,
    SymVirtual,
    NumSymTypes
} SYM_TYPE;
typedef struct _IMAGEHLP_LINE64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
    PVOID                       Key;                    // internal
    DWORD                       LineNumber;             // line number in file
    PCHAR                       FileName;               // full filename
    DWORD64                     Address;                // first instruction of line
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;
typedef struct _IMAGEHLP_MODULE64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64                     BaseOfImage;            // base load address of module
    DWORD                       ImageSize;              // virtual size of the loaded module
    DWORD                       TimeDateStamp;          // date/time stamp from pe header
    DWORD                       CheckSum;               // checksum from the pe header
    DWORD                       NumSyms;                // number of symbols in the symbol table
    SYM_TYPE                    SymType;                // type of symbols loaded
    CHAR                        ModuleName[32];         // module name
    CHAR                        ImageName[256];         // image name
    CHAR                        LoadedImageName[256];   // symbol file name
} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;
typedef struct _IMAGEHLP_SYMBOL64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_SYMBOL64)
    DWORD64                     Address;                // virtual address including dll base address
    DWORD                       Size;                   // estimated size of symbol, can be zero
    DWORD                       Flags;                  // info about the symbols, see the SYMF defines
    DWORD                       MaxNameLength;          // maximum size of symbol name in 'Name'
    CHAR                        Name[1];                // symbol name (null terminated string)
} IMAGEHLP_SYMBOL64, *PIMAGEHLP_SYMBOL64;
typedef enum {
    AddrMode1616,
    AddrMode1632,
    AddrModeReal,
    AddrModeFlat
} ADDRESS_MODE;
typedef struct _tagADDRESS64 {
    DWORD64       Offset;
    WORD          Segment;
    ADDRESS_MODE  Mode;
} ADDRESS64, *LPADDRESS64;
typedef struct _KDHELP64 {
    DWORD64   Thread;
    DWORD   ThCallbackStack;
    DWORD   ThCallbackBStore;
    DWORD   NextCallback;
    DWORD   FramePointer;
    DWORD64   KiCallUserMode;
    DWORD64   KeUserCallbackDispatcher;
    DWORD64   SystemRangeStart;
    DWORD64  Reserved[8];
} KDHELP64, *PKDHELP64;
typedef struct _tagSTACKFRAME64 {
    ADDRESS64   AddrPC;               // program counter
    ADDRESS64   AddrReturn;           // return address
    ADDRESS64   AddrFrame;            // frame pointer
    ADDRESS64   AddrStack;            // stack pointer
    ADDRESS64   AddrBStore;           // backing store pointer
    PVOID       FuncTableEntry;       // pointer to pdata/fpo or NULL
    DWORD64     Params[4];            // possible arguments to the function
    BOOL        Far;                  // WOW far call
    BOOL        Virtual;              // is this a virtual frame?
    DWORD64     Reserved[3];
    KDHELP64    KdHelp;
} STACKFRAME64, *LPSTACKFRAME64;
typedef
BOOL
(__stdcall *PREAD_PROCESS_MEMORY_ROUTINE64)(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    );
typedef
PVOID
(__stdcall *PFUNCTION_TABLE_ACCESS_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 AddrBase
    );
typedef
DWORD64
(__stdcall *PGET_MODULE_BASE_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 Address
    );
typedef
DWORD64
(__stdcall *PTRANSLATE_ADDRESS_ROUTINE64)(
    HANDLE    hProcess,
    HANDLE    hThread,
    LPADDRESS64 lpaddr
    );
#define SYMOPT_CASE_INSENSITIVE         0x00000001
#define SYMOPT_UNDNAME                  0x00000002
#define SYMOPT_DEFERRED_LOADS           0x00000004
#define SYMOPT_NO_CPP                   0x00000008
#define SYMOPT_LOAD_LINES               0x00000010
#define SYMOPT_OMAP_FIND_NEAREST        0x00000020
#define SYMOPT_LOAD_ANYTHING            0x00000040
#define SYMOPT_IGNORE_CVREC             0x00000080
#define SYMOPT_NO_UNQUALIFIED_LOADS     0x00000100
#define SYMOPT_FAIL_CRITICAL_ERRORS     0x00000200
#define SYMOPT_EXACT_SYMBOLS            0x00000400
#define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS   0x00000800
#define SYMOPT_IGNORE_NT_SYMPATH        0x00001000
#define SYMOPT_INCLUDE_32BIT_MODULES    0x00002000
#define SYMOPT_PUBLICS_ONLY             0x00004000
#define SYMOPT_NO_PUBLICS               0x00008000
#define SYMOPT_AUTO_PUBLICS             0x00010000
#define SYMOPT_NO_IMAGE_SEARCH          0x00020000
#define SYMOPT_SECURE                   0x00040000
#define SYMOPT_DEBUG                    0x80000000
#define UNDNAME_COMPLETE                 (0x0000)  // Enable full undecoration
#define UNDNAME_NAME_ONLY                (0x1000)  // Crack only the name for primary declaration;
#endif  // _MSC_VER < 1300
#pragma pack(pop)

// Some missing defines (for VC5/6):
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif  


// secure-CRT_functions are only available starting with VC8
#if _MSC_VER < 1400
#define strcpy_s strcpy
#define strncpy_s strncpy
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

static void MyStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)
{
  if (nMaxDestSize <= 0) return;
  if (strlen(szSrc) < nMaxDestSize)
  {
    strcpy_s(szDest, nMaxDestSize, szSrc);
  }
  else
  {
    strncpy_s(szDest, nMaxDestSize, szSrc, nMaxDestSize);
    szDest[nMaxDestSize-1] = 0;
  }
}  // MyStrCpy

// Normally it should be enough to use 'CONTEXT_FULL' (better would be 'CONTEXT_ALL')
#define USED_CONTEXT_FLAGS CONTEXT_FULL

namespace vapor {

//-----------------------------------//

class StackWalkerInternal
{
public:
  StackWalkerInternal(StackWalker *parent, HANDLE hProcess)
  {
    m_parent = parent;
    m_hDbhHelp = NULL;
    pSC = NULL;
    m_hProcess = hProcess;
    m_szSymPath = NULL;
    pSFTA = NULL;
    pSGLFA = NULL;
    pSGMB = NULL;
    pSGMI = NULL;
    pSGO = NULL;
    pSGSFA = NULL;
    pSI = NULL;
    pSLM = NULL;
    pSSO = NULL;
    pSW = NULL;
    pUDSN = NULL;
    pSGSP = NULL;
  }
  ~StackWalkerInternal()
  {
    if (pSC != NULL)
      pSC(m_hProcess);  // SymCleanup
    if (m_hDbhHelp != NULL)
      FreeLibrary(m_hDbhHelp);
    m_hDbhHelp = NULL;
    m_parent = NULL;
    if(m_szSymPath != NULL)
      free(m_szSymPath);
    m_szSymPath = NULL;
  }
  BOOL Init(LPCSTR szSymPath)
  {
    if (m_parent == NULL)
      return FALSE;
    // Dynamically load the Entry-Points for dbghelp.dll:
    // First try to load the newsest one from
    TCHAR szTemp[4096];
    // But before wqe do this, we first check if the ".local" file exists
    if (GetModuleFileName(NULL, szTemp, 4096) > 0)
    {
      _tcscat_s(szTemp, _T(".local"));
      if (GetFileAttributes(szTemp) == INVALID_FILE_ATTRIBUTES)
      {
        // ".local" file does not exist, so we can try to load the dbghelp.dll from the "Debugging Tools for Windows"
        // Ok, first try the new path according to the archtitecture:
#ifdef _M_IX86
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (x86)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#elif _M_X64
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (x64)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#elif _M_IA64
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows (ia64)\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#endif
        // If still not found, try the old directories...
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows\\dbghelp.dll"));
          // now check if the file exists:
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#if defined _M_X64 || defined _M_IA64
        // Still not found? Then try to load the (old) 64-Bit version:
        if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(_T("ProgramFiles"), szTemp, 4096) > 0) )
        {
          _tcscat_s(szTemp, _T("\\Debugging Tools for Windows 64-Bit\\dbghelp.dll"));
          if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
          {
            m_hDbhHelp = LoadLibrary(szTemp);
          }
        }
#endif
      }
    }
    if (m_hDbhHelp == NULL)  // if not already loaded, try to load a default-one
      m_hDbhHelp = LoadLibrary( _T("dbghelp.dll") );
    if (m_hDbhHelp == NULL)
      return FALSE;
    pSI = (tSI) GetProcAddress(m_hDbhHelp, "SymInitialize" );
    pSC = (tSC) GetProcAddress(m_hDbhHelp, "SymCleanup" );

    pSW = (tSW) GetProcAddress(m_hDbhHelp, "StackWalk64" );
    pSGO = (tSGO) GetProcAddress(m_hDbhHelp, "SymGetOptions" );
    pSSO = (tSSO) GetProcAddress(m_hDbhHelp, "SymSetOptions" );

    pSFTA = (tSFTA) GetProcAddress(m_hDbhHelp, "SymFunctionTableAccess64" );
    pSGLFA = (tSGLFA) GetProcAddress(m_hDbhHelp, "SymGetLineFromAddr64" );
    pSGMB = (tSGMB) GetProcAddress(m_hDbhHelp, "SymGetModuleBase64" );
    pSGMI = (tSGMI) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
    //pSGMI_V3 = (tSGMI_V3) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
    pSGSFA = (tSGSFA) GetProcAddress(m_hDbhHelp, "SymGetSymFromAddr64" );
    pUDSN = (tUDSN) GetProcAddress(m_hDbhHelp, "UnDecorateSymbolName" );
    pSLM = (tSLM) GetProcAddress(m_hDbhHelp, "SymLoadModule64" );
    pSGSP =(tSGSP) GetProcAddress(m_hDbhHelp, "SymGetSearchPath" );

    if ( pSC == NULL || pSFTA == NULL || pSGMB == NULL || pSGMI == NULL ||
      pSGO == NULL || pSGSFA == NULL || pSI == NULL || pSSO == NULL ||
      pSW == NULL || pUDSN == NULL || pSLM == NULL )
    {
      FreeLibrary(m_hDbhHelp);
      m_hDbhHelp = NULL;
      pSC = NULL;
      return FALSE;
    }

    // SymInitialize
    if (szSymPath != NULL)
      m_szSymPath = _strdup(szSymPath);
    if (this->pSI(m_hProcess, m_szSymPath, FALSE) == FALSE)
      m_parent->OnDbgHelpErr("SymInitialize", GetLastError(), 0);
      
    DWORD symOptions = this->pSGO();  // SymGetOptions
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
    //symOptions |= SYMOPT_NO_PROMPTS;
    // SymSetOptions
    symOptions = this->pSSO(symOptions);

    char buf[StackWalker::STACKWALK_MAX_NAMELEN] = {0};
    if (this->pSGSP != NULL)
    {
      if (this->pSGSP(m_hProcess, buf, StackWalker::STACKWALK_MAX_NAMELEN) == FALSE)
        m_parent->OnDbgHelpErr("SymGetSearchPath", GetLastError(), 0);
    }
    char szUserName[1024] = {0};
    DWORD dwSize = 1024;
    GetUserNameA(szUserName, &dwSize);
    m_parent->OnSymInit(buf, symOptions, szUserName);

    return TRUE;
  }

  StackWalker *m_parent;

  HMODULE m_hDbhHelp;
  HANDLE m_hProcess;
  LPSTR m_szSymPath;

/*typedef struct IMAGEHLP_MODULE64_V3 {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    // new elements: 07-Jun-2002
    CHAR     LoadedImageName[256];   // symbol file name
    CHAR     LoadedPdbName[256];     // pdb file name
    DWORD    CVSig;                  // Signature of the CV record in the debug directories
    CHAR         CVData[MAX_PATH * 3];   // Contents of the CV record
    DWORD    PdbSig;                 // Signature of PDB
    GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
    DWORD    PdbAge;                 // DBI age of pdb
    BOOL     PdbUnmatched;           // loaded an unmatched pdb
    BOOL     DbgUnmatched;           // loaded an unmatched dbg
    BOOL     LineNumbers;            // we have line number information
    BOOL     GlobalSymbols;          // we have internal symbol information
    BOOL     TypeInfo;               // we have type information
    // new elements: 17-Dec-2003
    BOOL     SourceIndexed;          // pdb supports source server
    BOOL     Publics;                // contains public symbols
};
*/

#pragma pack(push,8)
typedef struct IMAGEHLP_MODULE64_V2 {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    CHAR     LoadedImageName[256];   // symbol file name
};
#pragma pack(pop)


  // SymCleanup()
  typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );
  tSC pSC;

  // SymFunctionTableAccess64()
  typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD64 AddrBase );
  tSFTA pSFTA;

  // SymGetLineFromAddr64()
  typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );
  tSGLFA pSGLFA;

  // SymGetModuleBase64()
  typedef DWORD64 (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD64 dwAddr );
  tSGMB pSGMB;

  // SymGetModuleInfo64()
  typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V2 *ModuleInfo );
  tSGMI pSGMI;

//  // SymGetModuleInfo64()
//  typedef BOOL (__stdcall *tSGMI_V3)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V3 *ModuleInfo );
//  tSGMI_V3 pSGMI_V3;

  // SymGetOptions()
  typedef DWORD (__stdcall *tSGO)( VOID );
  tSGO pSGO;

  // SymGetSymFromAddr64()
  typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );
  tSGSFA pSGSFA;

  // SymInitialize()
  typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
  tSI pSI;

  // SymLoadModule64()
  typedef DWORD64 (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
    IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );
  tSLM pSLM;

  // SymSetOptions()
  typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );
  tSSO pSSO;

  // StackWalk64()
  typedef BOOL (__stdcall *tSW)( 
    DWORD MachineType, 
    HANDLE hProcess,
    HANDLE hThread, 
    LPSTACKFRAME64 StackFrame, 
    PVOID ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
  tSW pSW;

  // UnDecorateSymbolName()
  typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
    DWORD UndecoratedLength, DWORD Flags );
  tUDSN pUDSN;

  typedef BOOL (__stdcall WINAPI *tSGSP)(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);
  tSGSP pSGSP;


private:
  // **************************************** ToolHelp32 ************************
  #define MAX_MODULE_NAME32 255
  #define TH32CS_SNAPMODULE   0x00000008
  #pragma pack( push, 8 )
  typedef struct tagMODULEENTRY32
  {
      DWORD   dwSize;
      DWORD   th32ModuleID;       // This module
      DWORD   th32ProcessID;      // owning process
      DWORD   GlblcntUsage;       // Global usage count on the module
      DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
      BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
      DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
      HMODULE hModule;            // The hModule of this module in th32ProcessID's context
      char    szModule[MAX_MODULE_NAME32 + 1];
      char    szExePath[MAX_PATH];
  } MODULEENTRY32;
  typedef MODULEENTRY32 *  PMODULEENTRY32;
  typedef MODULEENTRY32 *  LPMODULEENTRY32;
  #pragma pack( pop )

  BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid)
  {
    // CreateToolhelp32Snapshot()
    typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
    // Module32First()
    typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    // Module32Next()
    typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

    // try both dlls...
    const TCHAR *dllname[] = { _T("kernel32.dll"), _T("tlhelp32.dll") };
    HINSTANCE hToolhelp = NULL;
    tCT32S pCT32S = NULL;
    tM32F pM32F = NULL;
    tM32N pM32N = NULL;

    HANDLE hSnap;
    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    BOOL keepGoing;
    size_t i;

    for (i = 0; i<(sizeof(dllname) / sizeof(dllname[0])); i++ )
    {
      hToolhelp = LoadLibrary( dllname[i] );
      if (hToolhelp == NULL)
        continue;
      pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
      pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
      pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
      if ( (pCT32S != NULL) && (pM32F != NULL) && (pM32N != NULL) )
        break; // found the functions!
      FreeLibrary(hToolhelp);
      hToolhelp = NULL;
    }

    if (hToolhelp == NULL)
      return FALSE;

    hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
    if (hSnap == (HANDLE) -1)
    {
      FreeLibrary(hToolhelp);
      return FALSE;
    }

    keepGoing = !!pM32F( hSnap, &me );
    int cnt = 0;
    while (keepGoing)
    {
      this->LoadModule(hProcess, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize);
      cnt++;
      keepGoing = !!pM32N( hSnap, &me );
    }
    CloseHandle(hSnap);
    FreeLibrary(hToolhelp);
    if (cnt <= 0)
      return FALSE;
    return TRUE;
  }  // GetModuleListTH32

  // **************************************** PSAPI ************************
  typedef struct _MODULEINFO {
      LPVOID lpBaseOfDll;
      DWORD SizeOfImage;
      LPVOID EntryPoint;
  } MODULEINFO, *LPMODULEINFO;

  BOOL GetModuleListPSAPI(HANDLE hProcess)
  {
    // EnumProcessModules()
    typedef BOOL (__stdcall *tEPM)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
    // GetModuleFileNameEx()
    typedef DWORD (__stdcall *tGMFNE)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
    // GetModuleBaseName()
    typedef DWORD (__stdcall *tGMBN)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
    // GetModuleInformation()
    typedef BOOL (__stdcall *tGMI)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );

    HINSTANCE hPsapi;
    tEPM pEPM;
    tGMFNE pGMFNE;
    tGMBN pGMBN;
    tGMI pGMI;

    DWORD i;
    //ModuleEntry e;
    DWORD cbNeeded;
    MODULEINFO mi;
    HMODULE *hMods = 0;
    char *tt = NULL;
    char *tt2 = NULL;
    const SIZE_T TTBUFLEN = 8096;
    int cnt = 0;

    hPsapi = LoadLibrary( _T("psapi.dll") );
    if (hPsapi == NULL)
      return FALSE;

    pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );
    pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );
    pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );
    pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );
    if ( (pEPM == NULL) || (pGMFNE == NULL) || (pGMBN == NULL) || (pGMI == NULL) )
    {
      // we couldn´t find all functions
      FreeLibrary(hPsapi);
      return FALSE;
    }

    hMods = (HMODULE*) malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof HMODULE));
    tt = (char*) malloc(sizeof(char) * TTBUFLEN);
    tt2 = (char*) malloc(sizeof(char) * TTBUFLEN);
    if ( (hMods == NULL) || (tt == NULL) || (tt2 == NULL) )
      goto cleanup;

    if ( ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) )
    {
      //_ftprintf(fLogFile, _T("%lu: EPM failed, GetLastError = %lu\n"), g_dwShowCount, gle );
      goto cleanup;
    }

    if ( cbNeeded > TTBUFLEN )
    {
      //_ftprintf(fLogFile, _T("%lu: More than %lu module handles. Huh?\n"), g_dwShowCount, lenof( hMods ) );
      goto cleanup;
    }

    for ( i = 0; i < cbNeeded / sizeof hMods[0]; i++ )
    {
      // base address, size
      pGMI(hProcess, hMods[i], &mi, sizeof mi );
      // image file name
      tt[0] = 0;
      pGMFNE(hProcess, hMods[i], tt, TTBUFLEN );
      // module name
      tt2[0] = 0;
      pGMBN(hProcess, hMods[i], tt2, TTBUFLEN );

      DWORD dwRes = this->LoadModule(hProcess, tt, tt2, (DWORD64) mi.lpBaseOfDll, mi.SizeOfImage);
      if (dwRes != ERROR_SUCCESS)
        m_parent->OnDbgHelpErr("LoadModule", dwRes, 0);
      cnt++;
    }

  cleanup:
    if (hPsapi != NULL) FreeLibrary(hPsapi);
    if (tt2 != NULL) free(tt2);
    if (tt != NULL) free(tt);
    if (hMods != NULL) free(hMods);

    return cnt != 0;
  }  // GetModuleListPSAPI

  DWORD LoadModule(HANDLE hProcess, LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size)
  {
    CHAR *szImg = _strdup(img);
    CHAR *szMod = _strdup(mod);
    DWORD result = ERROR_SUCCESS;
    if ( (szImg == NULL) || (szMod == NULL) )
      result = ERROR_NOT_ENOUGH_MEMORY;
    else
    {
      if (pSLM(hProcess, 0, szImg, szMod, baseAddr, size) == 0)
        result = GetLastError();
    }
    ULONGLONG fileVersion = 0;
    if ( (m_parent != NULL) && (szImg != NULL) )
    {
      // try to retrive the file-version:
      if ( (m_parent->m_options & StackWalker::RetrieveFileVersion) != 0)
      {
        VS_FIXEDFILEINFO *fInfo = NULL;
        DWORD dwHandle;
        DWORD dwSize = GetFileVersionInfoSizeA(szImg, &dwHandle);
        if (dwSize > 0)
        {
          LPVOID vData = malloc(dwSize);
          if (vData != NULL)
          {
            if (GetFileVersionInfoA(szImg, dwHandle, dwSize, vData) != 0)
            {
              UINT len;
              TCHAR szSubBlock[] = _T("\\");
              if (VerQueryValue(vData, szSubBlock, (LPVOID*) &fInfo, &len) == 0)
                fInfo = NULL;
              else
              {
                fileVersion = ((ULONGLONG)fInfo->dwFileVersionLS) + ((ULONGLONG)fInfo->dwFileVersionMS << 32);
              }
            }
            free(vData);
          }
        }
      }

      // Retrive some additional-infos about the module
      IMAGEHLP_MODULE64_V2 Module;
      const char *szSymType = "-unknown-";
      if (this->GetModuleInfo(hProcess, baseAddr, &Module) != FALSE)
      {
        switch(Module.SymType)
        {
          case SymNone:
            szSymType = "-nosymbols-";
            break;
          case SymCoff:  // 1
            szSymType = "COFF";
            break;
          case SymCv:  // 2
            szSymType = "CV";
            break;
          case SymPdb:  // 3
            szSymType = "PDB";
            break;
          case SymExport:  // 4
            szSymType = "-exported-";
            break;
          case SymDeferred:  // 5
            szSymType = "-deferred-";
            break;
          case SymSym:  // 6
            szSymType = "SYM";
            break;
          case 7: // SymDia:
            szSymType = "DIA";
            break;
          case 8: //SymVirtual:
            szSymType = "Virtual";
            break;
        }
      }
      m_parent->OnLoadModule(img, mod, baseAddr, size, result, szSymType, Module.LoadedImageName, fileVersion);
    }
    if (szImg != NULL) free(szImg);
    if (szMod != NULL) free(szMod);
    return result;
  }
public:
  BOOL LoadModules(HANDLE hProcess, DWORD dwProcessId)
  {
    // first try toolhelp32
    if (GetModuleListTH32(hProcess, dwProcessId))
      return true;
    // then try psapi
    return GetModuleListPSAPI(hProcess);
  }


  BOOL GetModuleInfo(HANDLE hProcess, DWORD64 baseAddr, IMAGEHLP_MODULE64_V2 *pModuleInfo)
  {
    if(this->pSGMI == NULL)
    {
      SetLastError(ERROR_DLL_INIT_FAILED);
      return FALSE;
    }
    // First try to use the larger ModuleInfo-Structure
//    memset(pModuleInfo, 0, sizeof(IMAGEHLP_MODULE64_V3));
//    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V3);
//    if (this->pSGMI_V3 != NULL)
//    {
//      if (this->pSGMI_V3(hProcess, baseAddr, pModuleInfo) != FALSE)
//        return TRUE;
//      // check if the parameter was wrong (size is bad...)
//      if (GetLastError() != ERROR_INVALID_PARAMETER)
//        return FALSE;
//    }
    // could not retrive the bigger structure, try with the smaller one (as defined in VC7.1)...
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
    void *pData = malloc(4096); // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
    if (pData == NULL)
    {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
    }
    memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64_V2));
    if (this->pSGMI(hProcess, baseAddr, (IMAGEHLP_MODULE64_V2*) pData) != FALSE)
    {
      // only copy as much memory as is reserved...
      memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64_V2));
      pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
      free(pData);
      return TRUE;
    }
    free(pData);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
};

// #############################################################
StackWalker::StackWalker(DWORD dwProcessId, HANDLE hProcess)
{
  m_options = OptionsAll;
  m_modulesLoaded = FALSE;
  m_hProcess = hProcess;
  m_sw = new StackWalkerInternal(this, m_hProcess);
  m_dwProcessId = dwProcessId;
  m_szSymPath = NULL;
  m_MaxRecursionCount = 1000;
}
StackWalker::StackWalker(int options, LPCSTR szSymPath, DWORD dwProcessId, HANDLE hProcess)
{
  m_options = options;
  m_modulesLoaded = FALSE;
  m_hProcess = hProcess;
  m_sw = new StackWalkerInternal(this, m_hProcess);
  m_dwProcessId = dwProcessId;
  if (szSymPath != NULL)
  {
    m_szSymPath = _strdup(szSymPath);
    m_options |= SymBuildPath;
  }
  else
    m_szSymPath = NULL;
  m_MaxRecursionCount = 1000;
}

StackWalker::~StackWalker()
{
  if (m_szSymPath != NULL)
    free(m_szSymPath);
  m_szSymPath = NULL;
  if (m_sw != NULL)
    delete m_sw;
  m_sw = NULL;
}

BOOL StackWalker::LoadModules()
{
  if (m_sw == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }
  if (m_modulesLoaded != FALSE)
    return TRUE;

  // Build the sym-path:
  char *szSymPath = NULL;
  if ( (m_options & SymBuildPath) != 0)
  {
    const size_t nSymPathLen = 4096;
    szSymPath = (char*) malloc(nSymPathLen);
    if (szSymPath == NULL)
    {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
    }
    szSymPath[0] = 0;
    // Now first add the (optional) provided sympath:
    if (m_szSymPath != NULL)
    {
      strcat_s(szSymPath, nSymPathLen, m_szSymPath);
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    strcat_s(szSymPath, nSymPathLen, ".;");

    const size_t nTempLen = 1024;
    char szTemp[nTempLen];
    // Now add the current directory:
    if (GetCurrentDirectoryA(nTempLen, szTemp) > 0)
    {
      szTemp[nTempLen-1] = 0;
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    // Now add the path for the main-module:
    if (GetModuleFileNameA(NULL, szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen-1] = 0;
      for (char *p = (szTemp+strlen(szTemp)-1); p >= szTemp; --p)
      {
        // locate the rightmost path separator
        if ( (*p == '\\') || (*p == '/') || (*p == ':') )
        {
          *p = 0;
          break;
        }
      }  // for (search for path separator...)
      if (strlen(szTemp) > 0)
      {
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, ";");
      }
    }
    if (GetEnvironmentVariableA("_NT_SYMBOL_PATH", szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen-1] = 0;
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }
    if (GetEnvironmentVariableA("_NT_ALTERNATE_SYMBOL_PATH", szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen-1] = 0;
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }
    if (GetEnvironmentVariableA("SYSTEMROOT", szTemp, nTempLen) > 0)
    {
      szTemp[nTempLen-1] = 0;
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
      // also add the "system32"-directory:
      strcat_s(szTemp, nTempLen, "\\system32");
      strcat_s(szSymPath, nSymPathLen, szTemp);
      strcat_s(szSymPath, nSymPathLen, ";");
    }

    if ( (m_options & SymUseSymSrv) != 0)
    {
      if (GetEnvironmentVariableA("SYSTEMDRIVE", szTemp, nTempLen) > 0)
      {
        szTemp[nTempLen-1] = 0;
        strcat_s(szSymPath, nSymPathLen, "SRV*");
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, "\\websymbols");
        strcat_s(szSymPath, nSymPathLen, "*http://msdl.microsoft.com/download/symbols;");
      }
      else
        strcat_s(szSymPath, nSymPathLen, "SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;");
    }
  }  // if SymBuildPath

  // First Init the whole stuff...
  BOOL bRet = m_sw->Init(szSymPath);
  if (szSymPath != NULL) free(szSymPath); szSymPath = NULL;
  if (bRet == FALSE)
  {
    this->OnDbgHelpErr("Error while initializing dbghelp.dll", 0, 0);
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  bRet = m_sw->LoadModules(m_hProcess, m_dwProcessId);
  if (bRet != FALSE)
    m_modulesLoaded = TRUE;
  return bRet;
}


// The following is used to pass the "userData"-Pointer to the user-provided readMemoryFunction
// This has to be done due to a problem with the "hProcess"-parameter in x64...
// Because this class is in no case multi-threading-enabled (because of the limitations 
// of dbghelp.dll) it is "safe" to use a static-variable
static StackWalker::PReadProcessMemoryRoutine s_readMemoryFunction = NULL;
static LPVOID s_readMemoryFunction_UserData = NULL;

BOOL StackWalker::ShowCallstack(HANDLE hThread, const CONTEXT *context, PReadProcessMemoryRoutine readMemoryFunction, LPVOID pUserData)
{
  CONTEXT c;
  CallstackEntry csEntry;
  IMAGEHLP_SYMBOL64 *pSym = NULL;
  StackWalkerInternal::IMAGEHLP_MODULE64_V2 Module;
  IMAGEHLP_LINE64 Line;
  int frameNum;
  bool bLastEntryCalled = true;
  int curRecursionCount = 0;

  if (m_modulesLoaded == FALSE)
    this->LoadModules();  // ignore the result...

  if (m_sw->m_hDbhHelp == NULL)
  {
    SetLastError(ERROR_DLL_INIT_FAILED);
    return FALSE;
  }

  s_readMemoryFunction = readMemoryFunction;
  s_readMemoryFunction_UserData = pUserData;

  if (context == NULL)
  {
    // If no context is provided, capture the context
    if (hThread == GetCurrentThread())
    {
      GET_CURRENT_CONTEXT(c, USED_CONTEXT_FLAGS);
    }
    else
    {
      SuspendThread(hThread);
      memset(&c, 0, sizeof(CONTEXT));
      c.ContextFlags = USED_CONTEXT_FLAGS;
      if (GetThreadContext(hThread, &c) == FALSE)
      {
        ResumeThread(hThread);
        return FALSE;
      }
    }
  }
  else
    c = *context;

  // init STACKFRAME for first call
  STACKFRAME64 s; // in/out stackframe
  memset(&s, 0, sizeof(s));
  DWORD imageType;
#ifdef _M_IX86
  // normally, call ImageNtHeader() and use machine info from PE header
  imageType = IMAGE_FILE_MACHINE_I386;
  s.AddrPC.Offset = c.Eip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Ebp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Esp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  imageType = IMAGE_FILE_MACHINE_AMD64;
  s.AddrPC.Offset = c.Rip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Rsp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Rsp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  imageType = IMAGE_FILE_MACHINE_IA64;
  s.AddrPC.Offset = c.StIIP;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.IntSp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrBStore.Offset = c.RsBSP;
  s.AddrBStore.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.IntSp;
  s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

  pSym = (IMAGEHLP_SYMBOL64 *) malloc(sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
  if (!pSym) goto cleanup;  // not enough memory...
  memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
  pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
  pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

  memset(&Line, 0, sizeof(Line));
  Line.SizeOfStruct = sizeof(Line);

  memset(&Module, 0, sizeof(Module));
  Module.SizeOfStruct = sizeof(Module);

  for (frameNum = 0; ; ++frameNum )
  {
    // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
    // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
    // assume that either you are done, or that the stack is so hosed that the next
    // deeper frame could not be found.
    // CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
    if ( ! m_sw->pSW(imageType, m_hProcess, hThread, &s, &c, myReadProcMem, m_sw->pSFTA, m_sw->pSGMB, NULL) )
    {
      // INFO: "StackWalk64" does not set "GetLastError"...
      this->OnDbgHelpErr("StackWalk64", 0, s.AddrPC.Offset);
      break;
    }

    csEntry.offset = s.AddrPC.Offset;
    csEntry.name[0] = 0;
    csEntry.undName[0] = 0;
    csEntry.undFullName[0] = 0;
    csEntry.offsetFromSmybol = 0;
    csEntry.offsetFromLine = 0;
    csEntry.lineFileName[0] = 0;
    csEntry.lineNumber = 0;
    csEntry.loadedImageName[0] = 0;
    csEntry.moduleName[0] = 0;
    if (s.AddrPC.Offset == s.AddrReturn.Offset)
    {
      if ( (m_MaxRecursionCount > 0) && (curRecursionCount > m_MaxRecursionCount) )
      {
        this->OnDbgHelpErr("StackWalk64-Endless-Callstack!", 0, s.AddrPC.Offset);
        break;
      }
      curRecursionCount++;
    }
    else
      curRecursionCount = 0;
    if (s.AddrPC.Offset != 0)
    {
      // we seem to have a valid PC
      // show procedure info (SymGetSymFromAddr64())
      if (m_sw->pSGSFA(m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromSmybol), pSym) != FALSE)
      {
        MyStrCpy(csEntry.name, STACKWALK_MAX_NAMELEN, pSym->Name);
        // UnDecorateSymbolName()
        m_sw->pUDSN( pSym->Name, csEntry.undName, STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY );
        m_sw->pUDSN( pSym->Name, csEntry.undFullName, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE );
      }
      else
      {
        this->OnDbgHelpErr("SymGetSymFromAddr64", GetLastError(), s.AddrPC.Offset);
      }

      // show line number info, NT5.0-method (SymGetLineFromAddr64())
      if (m_sw->pSGLFA != NULL )
      { // yes, we have SymGetLineFromAddr64()
        if (m_sw->pSGLFA(m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromLine), &Line) != FALSE)
        {
          csEntry.lineNumber = Line.LineNumber;
          MyStrCpy(csEntry.lineFileName, STACKWALK_MAX_NAMELEN, Line.FileName);
        }
        else
        {
          this->OnDbgHelpErr("SymGetLineFromAddr64", GetLastError(), s.AddrPC.Offset);
        }
      } // yes, we have SymGetLineFromAddr64()

      // show module info (SymGetModuleInfo64())
      if (m_sw->GetModuleInfo(m_hProcess, s.AddrPC.Offset, &Module ) != FALSE)
      { // got module info OK
        switch ( Module.SymType )
        {
        case SymNone:
          csEntry.symTypeString = "-nosymbols-";
          break;
        case SymCoff:
          csEntry.symTypeString = "COFF";
          break;
        case SymCv:
          csEntry.symTypeString = "CV";
          break;
        case SymPdb:
          csEntry.symTypeString = "PDB";
          break;
        case SymExport:
          csEntry.symTypeString = "-exported-";
          break;
        case SymDeferred:
          csEntry.symTypeString = "-deferred-";
          break;
        case SymSym:
          csEntry.symTypeString = "SYM";
          break;
#if API_VERSION_NUMBER >= 9
        case SymDia:
          csEntry.symTypeString = "DIA";
          break;
#endif
        case 8: //SymVirtual:
          csEntry.symTypeString = "Virtual";
          break;
        default:
          //_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
          csEntry.symTypeString = NULL;
          break;
        }

        // TODO: Mache dies sicher...!
        MyStrCpy(csEntry.moduleName, STACKWALK_MAX_NAMELEN, Module.ModuleName);
        csEntry.baseOfImage = Module.BaseOfImage;
        MyStrCpy(csEntry.loadedImageName, STACKWALK_MAX_NAMELEN, Module.LoadedImageName);
      } // got module info OK
      else
      {
        this->OnDbgHelpErr("SymGetModuleInfo64", GetLastError(), s.AddrPC.Offset);
      }
    } // we seem to have a valid PC

    CallstackEntryType et = nextEntry;
    if (frameNum == 0)
      et = firstEntry;
    bLastEntryCalled = false;
    this->OnCallstackEntry(et, csEntry);
    
    if (s.AddrReturn.Offset == 0)
    {
      bLastEntryCalled = true;
      this->OnCallstackEntry(lastEntry, csEntry);
      SetLastError(ERROR_SUCCESS);
      break;
    }
  } // for ( frameNum )

  cleanup:
    if (pSym) free( pSym );

  if (bLastEntryCalled == false)
      this->OnCallstackEntry(lastEntry, csEntry);

  if (context == NULL)
    ResumeThread(hThread);

  return TRUE;
}

BOOL __stdcall StackWalker::myReadProcMem(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    )
{
  if (s_readMemoryFunction == NULL)
  {
    SIZE_T st;
    BOOL bRet = ReadProcessMemory(hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, &st);
    *lpNumberOfBytesRead = (DWORD) st;
    //printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
    return bRet;
  }
  else
  {
    return s_readMemoryFunction(hProcess, qwBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead, s_readMemoryFunction_UserData);
  }
}

void StackWalker::OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
{
  CHAR buffer[STACKWALK_MAX_NAMELEN];
  if (fileVersion == 0)
    _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s'\n", img, mod, (LPVOID) baseAddr, size, result, symType, pdbName);
  else
  {
    DWORD v4 = (DWORD) fileVersion & 0xFFFF;
    DWORD v3 = (DWORD) (fileVersion>>16) & 0xFFFF;
    DWORD v2 = (DWORD) (fileVersion>>32) & 0xFFFF;
    DWORD v1 = (DWORD) (fileVersion>>48) & 0xFFFF;
    _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s', fileVersion: %d.%d.%d.%d\n", img, mod, (LPVOID) baseAddr, size, result, symType, pdbName, v1, v2, v3, v4);
  }
  OnOutput(buffer);
}

void StackWalker::OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
{
  CHAR buffer[STACKWALK_MAX_NAMELEN];
  if ( (eType != lastEntry) && (entry.offset != 0) )
  {
    if (entry.name[0] == 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, "(function-name not available)");
    if (entry.undName[0] != 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undName);
    if (entry.undFullName[0] != 0)
      MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undFullName);
    if (entry.lineFileName[0] == 0)
    {
      MyStrCpy(entry.lineFileName, STACKWALK_MAX_NAMELEN, "(filename not available)");
      if (entry.moduleName[0] == 0)
        MyStrCpy(entry.moduleName, STACKWALK_MAX_NAMELEN, "(module-name not available)");
      _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%p (%s): %s: %s\n", (LPVOID) entry.offset, entry.moduleName, entry.lineFileName, entry.name);
    }
    else
      _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%s (%d): %s\n", entry.lineFileName, entry.lineNumber, entry.name);
    buffer[STACKWALK_MAX_NAMELEN-1] = 0;
    OnOutput(buffer);
  }
}

void StackWalker::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
{
  CHAR buffer[STACKWALK_MAX_NAMELEN];
  _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "ERROR: %s, GetLastError: %d (Address: %p)\n", szFuncName, gle, (LPVOID) addr);
  OnOutput(buffer);
}

void StackWalker::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
{
  CHAR buffer[STACKWALK_MAX_NAMELEN];
  _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "SymInit: Symbol-SearchPath: '%s', symOptions: %d, UserName: '%s'\n", szSearchPath, symOptions, szUserName);
  OnOutput(buffer);
  // Also display the OS-version
#if _MSC_VER <= 1200
  OSVERSIONINFOA ver;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOA));
  ver.dwOSVersionInfoSize = sizeof(ver);
  if (GetVersionExA(&ver) != FALSE)
  {
    _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "OS-Version: %d.%d.%d (%s)\n", 
      ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,
      ver.szCSDVersion);
    OnOutput(buffer);
  }
#else
  OSVERSIONINFOEXA ver;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOEXA));
  ver.dwOSVersionInfoSize = sizeof(ver);
  if (GetVersionExA( (OSVERSIONINFOA*) &ver) != FALSE)
  {
    _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "OS-Version: %d.%d.%d (%s) 0x%x-0x%x\n", 
      ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,
      ver.szCSDVersion, ver.wSuiteMask, ver.wProductType);
    OnOutput(buffer);
  }
#endif
}

void StackWalker::OnOutput(LPCSTR buffer)
{
  OutputDebugStringA(buffer);
}

//-----------------------------------//

} // end namespace 

#endif

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
