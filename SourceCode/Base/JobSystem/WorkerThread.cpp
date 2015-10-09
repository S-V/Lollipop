#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#if 0
#include <JobSystem/WorkerThread.h>

// Description:
// A helper function to print out a friendly error code.
// Arguments:
// where - Specifies where the error occurred.
// Return Value List:
// PE_RESULT_UNKNOWN_ERROR - The error is unknown (to Phyre)
static PResult HandleLastError(const PChar *where)
{
	DWORD dwLastError = GetLastError();

	LPSTR MessageBuffer;

	// Call FormatMessage() to allow for message text to be acquired from the system or from the supplied module handle.
	DWORD dwBufferLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
											NULL,
											dwLastError,
											MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
											(LPSTR) &MessageBuffer,
											0,
											NULL);

	if (dwBufferLength)
	{
		(void)where; // Unused in release builds
		PHYRE_SET_LAST_ERROR(PE_RESULT_UNKNOWN_ERROR, "%s: Windows Error: %d\n\t%s", where, dwLastError, MessageBuffer);

		// Free the buffer allocated by the system.
		LocalFree(MessageBuffer);

		return PE_RESULT_UNKNOWN_ERROR;
	}
	else
	{
		return PHYRE_SET_LAST_ERROR(PE_RESULT_UNKNOWN_ERROR, "%s: Windows Error: %d", where, dwLastError);
	}
}

PThreadWin32::PThreadWin32()
	: m_handle(NULL)
{
}

// Description:
// Creates a new thread.
// Arguments:
// threadInit - Thread initialization parameters.
// Return Value List:
// true:		Thread created successfully.
// PE_RESULT_OUT_OF_MEMORY:	Unable to allocate thread storage.
// PE_RESULT_UNKNOWN_ERROR:	OS error occurred.
bool PThreadWin32::initialize(const SCreateThreadArgs &threadInit)
{
	DWORD		threadID	= 0;
	m_handle = CreateThread(
		NULL,											// default security attributes
		threadInit.m_stackSize,							// use default stack size
		(LPTHREAD_START_ROUTINE)threadInit.m_entryPoint,// thread function
		threadInit.m_userData,							// argument to thread function
		0,												// use default creation flags
		&threadID);										// returns the thread identifier

	if(m_handle == NULL)
		return HandleLastError("Unable to allocate thread");

	const char	*threadName	= threadInit.m_threadName;
	if(threadName && threadID)
	{
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // must be 0x1000
			LPCSTR szName; // pointer to name (in user addr space)
			DWORD dwThreadID; // thread ID (-1=caller thread)
			DWORD dwFlags; // reserved for future use, must be zero
		} THREADNAME_INFO;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = threadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}

	// Set the priority based on required priority
	switch (threadInit.m_priority)
	{
	case PE_THREADPRIORITY_HIGHER:
		SetThreadPriority(m_handle, THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case PE_THREADPRIORITY_LOWER:
		SetThreadPriority(m_handle, THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case PE_THREADPRIORITY_SAME:
	default:
		break;
	}

	m_isInitialized = true;

	return true;
}

// Description:
// Wait for the completion of the thread.
// Arguments:
// threadResult - Result returned by the thread.

bool PThreadWin32::join(U4 &threadResult)
{
	WaitForSingleObject(m_handle, INFINITE);

	DWORD	result;
	GetExitCodeThread(m_handle, &result);

	threadResult = result;

	return true;
}

// Description:
// Removes a thread from the list of threads.

bool PThreadWin32::terminate()
{
	m_isInitialized = false;

	return true;
}
#endif
NO_EMPTY_FILE
