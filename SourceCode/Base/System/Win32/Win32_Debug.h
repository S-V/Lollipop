/*
=============================================================================
	File:	Win32_Debug.h
	Desc:	These can be very useful for debugging.
			This is an OS specific header file
=============================================================================
*/
#pragma once

#pragma pack( push, 8 )
	#include <DbgHelp.h>
#pragma pack( pop )

#if MX_AUTOLINK
	#pragma comment( lib, "Dbghelp.lib" )
#endif

//
// Embed debug info in each file.
//
//#if MX_DEBUG
//	#pragma comment( compiler )
//	#pragma comment( exestr,"DEBUG version, compile: " __DATE__ ", " __TIME__ )
//#endif

FORCEINLINE BOOL mxIsInDebugger()
{
	return ::IsDebuggerPresent();
}

//
// mxDebugBreak() - Causes a breakpoint exception to occur.
//
//#define mxDebugBreak()	DebugBreak()

// DebugBreak() is defined in "winbase.h"


//------------------------------------------------------------------
//		Defines.
//------------------------------------------------------------------

//#define ALWAYS_BREAK					{ _asm int 3 }
#define mxALWAYS_BREAK					__debugbreak()

#define mxBREAK_IF( _CONDITION_ )			if (_CONDITION_) mxDEBUG_BREAK;

// These are removed in release builds.
#if MX_DEBUG
	#define mxDEBUG_BREAK				if(mxIsInDebugger()) __debugbreak()
	#define mxASSERT( x )				mxBREAK_IF( !(x) )
	#define mxASSERT_NZ( x )			mxBREAK_IF( 0 == (x) )
#else
	#define mxDEBUG_BREAK				mxNOOP
	#define mxASSERT( x )				(void)(x);mxNOOP
	#define mxASSERT_NZ( x )			(void)(x);mxNOOP
#endif // ! MX_DEBUG

#define mxANALYSIS_ASSUME( x )		{ mxASSERT((x)); __assume(x); }


#ifndef mxDEFAULT_UNREACHABLE
	//
	// Macro used to eliminate compiler warning 4715 within a switch statement
	// when all possible cases have already been accounted for.
	//
	// switch (a & 3) {
	//     case 0: return 1;
	//     case 1: return Foo();
	//     case 2: return Bar();
	//     case 3: return 1;
	//     DEFAULT_UNREACHABLE;
	//
	#if MX_DEBUG
		#define mxDEFAULT_UNREACHABLE		default: mxALWAYS_BREAK
	#else
		#if (_MSC_VER > 1200)
			#define mxDEFAULT_UNREACHABLE	default: __assume(0)
		#endif
	#endif
#endif


#if MX_MATH_CHECKS
	#define mxMATH_CHECK( x )		mxBREAK_IF( !(x) )
#else
	#define mxMATH_CHECK( x )		(void)(x);mxNOOP
#endif //!MX_MATH_CHECKS

mxSWIPED("OOOII, The MIT License, Copyright (c) 2011 Antony Arciuolo & Kevin Myers");

inline bool oSetLastError(int _Error, const char* _Format, ...)
{
	return true;
}

// _____________________________________________________________________________
// Error check wrappers. These interpret HRESULTS, so should not be used on 
// anything but WIN32 API. oVB is for the return FALSE then GetLastError() 
// pattern, and oV is for direct HRESULT return values.

//#ifdef _DEBUG
//	#define oVB(fn) do { if (!(fn)) { oWinSetLastError(::GetLastError()); oASSERT_PRINT_MESSAGE(TYPE_ASSERT, oAssert::IGNORE_ONCE, fn, "%s", oGetLastErrorDesc()); } } while(false)
//	#define oV(fn) do { HRESULT HR__ = fn; if (FAILED(HR__)) { oWinSetLastError(HR__); oASSERT_PRINT_MESSAGE(TYPE_ASSERT, oAssert::IGNORE_ONCE, fn, "%s", oGetLastErrorDesc()); } } while(false)
//#else
	#define oVB(fn) fn
	#define oV(fn) fn
//#endif

#define oWINDOWS_DEFAULT 0x80000000

// Given the specified HRESULT, set both the closest errno value and the 
// platform-specific description associated with the error code.
// if oWINDOWS_DEFAULT is specified, ::GetLastError() is used
bool oWinSetLastError(HRESULT _hResult = oWINDOWS_DEFAULT, const char* _ErrorDescPrefix = 0);

// oV_RETURN executes a block of code that returns an HRESULT
// if the HRESULT is not S_OK it returns the HRESULT
#define oV_RETURN(fn) do { HRESULT HR__ = fn; if (FAILED(HR__)) return HR__; } while(false)

// oVB_RETURN executes a block of Windows API code that returns bool and populates
// oGetLastError() with ::GetLastError() and returns false.
#define oVB_RETURN(fn) do { if (!(fn)) { oWinSetLastError(oWINDOWS_DEFAULT, #fn " failed: "); return false; } } while(false)


mxSWIPED("OOOII, The MIT License, Copyright (c) 2011 Antony Arciuolo & Kevin Myers");
// _____________________________________________________________________________
// Wrappers for the Windows-specific crtdbg API. Prefer oASSERT macros found
// in oAssert.h, but for a few systems that are lower-level than oAssert, these
// may be necessary.

//#ifdef _DEBUG
//	#include <crtdbg.h>
//
//	#define oCRTASSERT(expr, msg, ...) if (!(expr)) { _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "OOOii Debug Library", #expr "\n\n%s", msg, ## __VA_ARGS__); }
//	#define oCRTWARNING(msg, ...) do { _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "OOOii Debug Library", "WARNING: " msg, ## __VA_ARGS__); } while(false)
//	#define oCRTTRACE(msg, ...) do { _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "OOOii Debug Library", msg, ## __VA_ARGS__); } while(false)
//
//	// Convenience wrapper for quick scoped leak checking
//	class oLeakCheck
//	{
//		const char* Name;
//		_CrtMemState StartState;
//	public:
//		oLeakCheck(const char* _ConstantName = "") : Name(_ConstantName ? _ConstantName : "(unnamed)") { _CrtMemCheckpoint(&StartState); }
//		~oLeakCheck()
//		{
//			_CrtMemState endState, stateDiff;
//			_CrtMemCheckpoint(&endState);
//			_CrtMemDifference(&stateDiff, &StartState, &endState);
//			oCRTTRACE("---- Mem diff for %s ----", Name);
//			_CrtMemDumpStatistics(&stateDiff);
//		}
//	};
//#else
	#define oCRTASSERT(expr, msg, ...) __noop
	#define oCRTWARNING(msg, ...) __noop
	#define oCRTTRACE(msg, ...) __noop
//#endif


//------------------------------------------------------------------
//		Compile-time messages.
//------------------------------------------------------------------

// Statements like:
//		#pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
//		C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles.  Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution.  You can double
// click on these messages and jump to the line in question.
#define Reminder				__FILE__ "(" TO_STR(__LINE__) "): "

// Once defined, use like so: 
// #pragma message(Reminder "Fix this problem!")
// This will create output like: 
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!

// TIP: you can use #pragma with some bogus parameter
// and Visual C++ will issue a warning (C4068: unknown pragma).

// Compile time output. This macro writes message provided as its argument
// into the build window together with source file name and line number at compile time.
//
// Syntax: mxMSG( ... your text here ...)
//
// Sample: the following statement writes the text "Hello world" into the build window:
//         mxMSG( Hello world )
//
#define mxMSG( x )	__pragma(message( __FILE__ "(" TO_STR(__LINE__) "): " #x ))




//------------------------------------------------------------------
//		Memory.
//------------------------------------------------------------------

//
//	Returns true if given a valid heap pointer.
//	Can be used for debugging purposes.
//
FORCEINLINE bool mxIsValidHeapPointer( const void* ptr )
{
	const size_t value = (size_t) ptr;
	return ( value != 0 )

		&& ( value != 0xCCCCCCCC )	// <- MVC++-specific (uninitialized stack memory)
		&& ( value != 0xFEEEFEEE )	// <- MVC++-specific (freed memory)
		&& ( value != 0xDDDDDDDD )	// <- MVC++-specific (freed memory)
		&& ( value != 0xFDFDFDFD )	// <- MVC++-specific
		&& ( value != 0xABABABAB )	// <- MVC++-specific ("no man's land" guard bytes after allocated heap memory)
		&& ( value != 0xCDCDCDCD )	// <- MVC++-specific (uninitialized heap memory)

		;
}

/*
If you're using the debug heap in MVC++, memory is initialized and cleared with special values:

0xCD - "Clean" - Allocated in heap, but not initialized (new objects are filled with 0xCD when they are allocated).
0xDD - "Dead" - Released heap memory (freed blocks).
0xED - no-man's land for aligned routines
0xFD - "Fence" - "NoMansLand" fences automatically placed at boundary of heap memory (on either side of the memory used by an application). Should never be overwritten. If you do overwrite one, you're probably walking off the end of an array.
0xCC - "Clean" - Allocated on stack, but not initialized.
0xAB - "?" - Used by Microsoft's HeapAlloc() to mark "no man's land" guard bytes after allocated heap memory.
0xBA - "?" - Used by Microsoft's LocalAlloc(LMEM_FIXED) to mark uninitialized allocated heap memory.
0xFD - "Fence" Used by Microsoft's C++ debugging heap to mark "no man's land" guard bytes before and after allocated heap memory
0xFE - "Free" - Used by Microsoft's HeapFree() to mark freed heap memory

See the file "dbgheap.c".
*/

#define MX_ASSERT_PTR( ptr )		mxASSERT( mxIsValidHeapPointer( ptr ) )

#define ASSERT_MEM( ptr, bytes )	mxASSERT( ::_CrtIsValidPointer( ptr, bytes, true ) )

// freed memory is overwritten with this value
#define FREED_MEM_ID	0xDD





#if MX_DEBUG

	// used to track down bugs caused by dangling pointers (e.g. deleted objects)
	#define mxDBG_CHECK_VTBL( objectPtr )\
		if( objectPtr != nil )\
		{\
			const mxULong vtblAddress = *((const mxULong*)objectPtr);\
			Assert( vtblAddress != nil );\
			Assert( vtblAddress != 0xCCCCCCCC );\
			Assert( vtblAddress != 0xFEEEFEEE );\
			Assert( vtblAddress != 0xDDDDDDDD );\
			Assert( vtblAddress != 0xFEEEFEEE );\
			Assert( vtblAddress != 0xFDFDFDFD );\
			Assert( vtblAddress != 0xABABABAB );\
			Assert( vtblAddress != 0xCDCDCDCD );\
		}

#else

	#define mxDBG_CHECK_VTBL( objectPtr )

#endif // MX_DEBUG




mxSWIPED("CryEngine SDK");
inline void CryHeapCheck()
{
#if !defined(LINUX) && !defined (PS3)
	int Result = _heapchk();
	(void)Result;
	assert(Result!=_HEAPBADBEGIN);
	assert(Result!=_HEAPBADNODE);
	assert(Result!=_HEAPBADPTR);
	assert(Result!=_HEAPEMPTY);
	assert(Result==_HEAPOK);
#endif
}


//------------------------------------------------------------------
//		Functions.
//------------------------------------------------------------------

mxNAMESPACE_BEGIN



mxNAMESPACE_END




/*
================================================================================
	Minidump.

	From MSDN:

		Applications can produce user-mode minidump files, which contain a useful
	subset of the information contained in a crash dump file.
	Applications can create minidump files very quickly and efficiently.
		Because minidump files are small, they can be easily sent over the internet
	to technical support for the application.
	A minidump file does not contain as much information as a full crash dump file,
	but it contains enough information to perform basic debugging operations.
		To read a minidump file, you must have the binaries and symbol files
	available for the debugger.
		Current versions of Microsoft Office and Microsoft Windows
	create minidump files for the purpose of analyzing failures on customers' computers.

	The following DbgHelp functions are used with minidump files.
	
		MiniDumpCallback 
		MiniDumpReadDumpStream 
		MiniDumpWriteDump
================================================================================
*/
mxSWIPED("Nebula3")
mxNAMESPACE_BEGIN

class Win32MiniDump
{
public:
	/// setup the the Win32 exception callback hook
	static void Setup();
	/// write a mini dump
	static bool WriteMiniDump();

private:
	/// the actual exception handler function called back by Windows
	static LONG WINAPI ExceptionCallback( EXCEPTION_POINTERS* exceptionInfo );

	/// build a filename for the dump file
	static bool BuildMiniDumpFilename( char *fileName, UINT numChars );

	/// internal mini-dump-writer method with extra exception info
	static bool WriteMiniDumpInternal( EXCEPTION_POINTERS* exceptionInfo );
};

/**********************************************************************
 * 
 * StackWalker.h
 *
 *
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
 * **********************************************************************/
// #pragma once is supported starting with _MCS_VER 1000, 
// so we need not to check the version (because we only support _MSC_VER >= 1100)!

#ifdef ENABLE_STACK_WALKER

#include <windows.h>

// special defines for VC5/6 (if no actual PSDK is installed):
#if _MSC_VER < 1300
typedef unsigned __int64 DWORD64, *PDWORD64;
#if defined(_WIN64)
typedef unsigned __int64 SIZE_T, *PSIZE_T;
#else
typedef unsigned long SIZE_T, *PSIZE_T;
#endif
#endif  // _MSC_VER < 1300

namespace vapor {

	//-----------------------------------//

	class StackWalkerInternal;  // forward
	class StackWalker
	{
	public:
		typedef enum StackWalkOptions
		{
			// No addition info will be retrived 
			// (only the address is available)
			RetrieveNone = 0,

			// Try to get the symbol-name
			RetrieveSymbol = 1,

			// Try to get the line for this symbol
			RetrieveLine = 2,

			// Try to retrieve the module-infos
			RetrieveModuleInfo = 4,

			// Also retrieve the version for the DLL/EXE
			RetrieveFileVersion = 8,

			// Contains all the abouve
			RetrieveVerbose = 0xF,

			// Generate a "good" symbol-search-path
			SymBuildPath = 0x10,

			// Also use the public Microsoft-Symbol-Server
			SymUseSymSrv = 0x20,

			// Contains all the abouve "Sym"-options
			SymAll = 0x30,

			// Contains all options (default)
			OptionsAll = 0x3F
		} StackWalkOptions;

		StackWalker(
			int options = OptionsAll, // 'int' is by design, to combine the enum-flags
			LPCSTR szSymPath = NULL, 
			DWORD dwProcessId = GetCurrentProcessId(), 
			HANDLE hProcess = GetCurrentProcess()
			);
		StackWalker(DWORD dwProcessId, HANDLE hProcess);
		virtual ~StackWalker();

		typedef BOOL (__stdcall *PReadProcessMemoryRoutine)(
			HANDLE      hProcess,
			DWORD64     qwBaseAddress,
			PVOID       lpBuffer,
			DWORD       nSize,
			LPDWORD     lpNumberOfBytesRead,
			LPVOID      pUserData  // optional data, which was passed in "ShowCallstack"
			);

		BOOL LoadModules();

		BOOL ShowCallstack(
			HANDLE hThread = GetCurrentThread(), 
			const CONTEXT *context = NULL, 
			PReadProcessMemoryRoutine readMemoryFunction = NULL,
			LPVOID pUserData = NULL  // optional to identify some data in the 'readMemoryFunction'-callback
			);

#if _MSC_VER >= 1300
		// due to some reasons, the "STACKWALK_MAX_NAMELEN" must be declared as "public" 
		// in older compilers in order to use it... starting with VC7 we can declare it as "protected"
	protected:
#endif
		enum { STACKWALK_MAX_NAMELEN = 1024 }; // max name length for found symbols

	protected:
		// Entry for each Callstack-Entry
		typedef struct CallstackEntry
		{
			DWORD64 offset;  // if 0, we have no valid entry
			CHAR name[STACKWALK_MAX_NAMELEN];
			CHAR undName[STACKWALK_MAX_NAMELEN];
			CHAR undFullName[STACKWALK_MAX_NAMELEN];
			DWORD64 offsetFromSmybol;
			DWORD offsetFromLine;
			DWORD lineNumber;
			CHAR lineFileName[STACKWALK_MAX_NAMELEN];
			DWORD symType;
			LPCSTR symTypeString;
			CHAR moduleName[STACKWALK_MAX_NAMELEN];
			DWORD64 baseOfImage;
			CHAR loadedImageName[STACKWALK_MAX_NAMELEN];
		} CallstackEntry;

		typedef enum CallstackEntryType {firstEntry, nextEntry, lastEntry};

		virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
		virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
		virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
		virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
		virtual void OnOutput(LPCSTR szText);

		StackWalkerInternal *m_sw;
		HANDLE m_hProcess;
		DWORD m_dwProcessId;
		BOOL m_modulesLoaded;
		LPSTR m_szSymPath;

		int m_options;
		int m_MaxRecursionCount;

		static BOOL __stdcall myReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);

		friend StackWalkerInternal;
	};


	// The "ugly" assembler-implementation is needed for systems before XP
	// If you have a new PSDK and you only compile for XP and later, then you can use 
	// the "RtlCaptureContext"
	// Currently there is no define which determines the PSDK-Version... 
	// So we just use the compiler-version (and assumes that the PSDK is 
	// the one which was installed by the VS-IDE)

	// INFO: If you want, you can use the RtlCaptureContext if you only target XP and later...
	//       But I currently use it in x64/IA64 environments...
	//#if defined(_M_IX86) && (_WIN32_WINNT <= 0x0500) && (_MSC_VER < 1400)

#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
	// TODO: The following is not a "good" implementation, 
	// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
	do { \
	memset(&c, 0, sizeof(CONTEXT)); \
	EXCEPTION_POINTERS *pExp = NULL; \
	__try { \
	throw 0; \
	} __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
	if (pExp != NULL) \
	memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
	c.ContextFlags = contextFlags; \
	} while(0);
#else
	// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
	do { \
	memset(&c, 0, sizeof(CONTEXT)); \
	c.ContextFlags = contextFlags; \
	__asm    call x \
	__asm x: pop eax \
	__asm    mov c.Eip, eax \
	__asm    mov c.Ebp, ebp \
	__asm    mov c.Esp, esp \
	} while(0);
#endif

#else

	// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags) \
	do { \
	memset(&c, 0, sizeof(CONTEXT)); \
	c.ContextFlags = contextFlags; \
	RtlCaptureContext(&c); \
	} while(0);
#endif

	//-----------------------------------//

	class StackWalkerLog : public StackWalker
	{
	public:

		StackWalkerLog() : StackWalker(
			RetrieveSymbol | RetrieveLine | SymBuildPath )
		{
			OnOutput("Callstack: \n\n");
		}

		void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
		{ }

		void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size,
			DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
		{ }

		void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
		{
			CHAR buffer[STACKWALK_MAX_NAMELEN];

			if(!(eType != lastEntry && entry.offset != 0))
				return;

			if (entry.name[0] == 0)
				strcpy_s(entry.name, STACKWALK_MAX_NAMELEN, "(function-name not available)");

			_snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "\t%s(%d)\n", entry.name, entry.lineNumber);
			buffer[STACKWALK_MAX_NAMELEN-1] = 0;

			OnOutput(buffer);
		}

		void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
		{ }

		void OnOutput(LPCSTR szText)
		{
			output.append(szText);
			StackWalker::OnOutput(szText);
		}

		void Log()
		{
			LogError( output.c_str() );
		}

		String output;
	};

	//-----------------------------------//

} // end namespace 

#endif

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
