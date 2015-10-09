/*
=============================================================================
	File:	Win32_Threading.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxNAMESPACE_BEGIN

#if MX_USER_CRITICAL_SECTION

	/*================================
			mxCriticalSection
	================================*/

	mxCriticalSection::mxCriticalSection()
		: lockerThread( 0 )
		, spinMax( 4096 )
		, semaphore( NULL )
		, waiterCount( 0 )
		, recursiveLockCount( 0 )
	{
		if( GetNumCpuCores() == 1 ) {
			spinMax = 0;
		}
	}

	mxCriticalSection::~mxCriticalSection()
	{
		if( semaphore )
		{
			::CloseHandle( semaphore );
		}
	}

#endif // MX_USER_CRITICAL_SECTION

/*================================
			mxThread
================================*/

mxThread::mxThread()
	: hThread( NULL )
	, threadId( 0 )
	, bActive( false )
{}

mxThread::~mxThread()
{
	Kill();
}

//
//	ThreadFunction - this function is executed by new threads.
//
static DWORD WINAPI ThreadFunction( LPVOID lpParam )
{
	mxThread * pThread = static_cast< mxThread* >( lpParam );
	pThread->Run();
	return 0;
}

//
//	mxThread::Create
//
bool mxThread::Create()
{
	hThread = ::CreateThread(
		NULL,			// LPSECURITY_ATTRIBUTES
		0,				// dwStackSize
		(LPTHREAD_START_ROUTINE) ThreadFunction,	// lpStartAddress
		(LPVOID) this,	// lpParameter
		0,
		(LPDWORD) &threadId
	);

	if( NULL == hThread ) {
		return false;
	}

	bActive = true;

	return true;
}

//
//	mxThread::Suspend
//
bool mxThread::Suspend()
{
	bActive = false;
	return ::SuspendThread( hThread ) != (DWORD) -1;
}

//
//	mxThread::Resume
//
bool mxThread::Resume()
{
	bActive = true;
	return ::ResumeThread( hThread ) != (DWORD) -1;
}

//
//	mxThread::Wait
//
bool mxThread::Wait()
{
	return ::WaitForSingleObject( hThread, INFINITE ) == WAIT_OBJECT_0;
}

//
//	mxThread::Kill
//
bool mxThread::Kill()
{
	bActive = false;
	const DWORD dwExitCode = 1;
	// Using TerminateThread() does not allow proper thread clean up.
	return ::TerminateThread( hThread, dwExitCode ) != 0;
}

//
//	mxThread::IsActive
//
bool mxThread::IsActive() const
{
	return bActive;
}


/*
Most of windows Debuggers supports thread names.
This is really helpful to identify the purpose of threads especially
when you’re having multiple threads in your application.

In Visual Studio  you can see the details of threads including it’s name in the “Threads” window.
There are no direction functions to set the thread names for a thread.

But it’s possible to implement by exploiting exception handling mechanism of a debugger.
The debugger will get the first chance to handle the exception when an application is being debugged.
Using this concept we will be explicitly raising an exception with parameters which specifies the thread name and other information.
The particular exception will be specially handled by debugger and thus it will update the name of corresponding thread. 

When application running without debugger, raising exceptions lead to application crash.
So we’ve to enclose the RaiseException code inside a try-except block.
*/

// The following function can be used either by specifying the thread ID
// of the particular thread to be named or passing -1
// as thread ID will set the name for the current thread which is calling the function.
//
// Usage: SetThreadName (-1, "MainThread");
//
const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, const char* threadName )
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		::RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// printf(“Not under debugger. Swallowing exception”);
	}
}

// OS X 10.6: pthread_setname_np(thread_name);
// Win32: http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
// Linux: http://stackoverflow.com/questions/778085/how-to-name-a-thread-in-linux

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
