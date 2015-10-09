#pragma once

// Description:
// ThreadEntryPoint_F defines the entry point function pointer for a thread.
// Arguments:
// userData :  The pointer to the user data passed to the PThread::initialize() function.
// Returns:
// The OS dependent thread completion value.
typedef U4 (*ThreadEntryPoint_F)(void *userData);

// Description:
// An enumerated type describing the relative thread priority of a thread being created.
enum EThreadPriority
{
	PE_THREADPRIORITY_HIGHER = 0,		// The thread being created is of a higher priority.
	PE_THREADPRIORITY_SAME,				// The thread being created is of the same priority.
	PE_THREADPRIORITY_LOWER				// The thread being created is of a lower priority.
};

class SCreateThreadArgs
{
public:	
	ThreadEntryPoint_F	m_entryPoint;			// The entry point for the new thread.
	U4				m_stackSize;			// The size of the stack for the new thread. 0 implies that the size of the stack is standard for the operating system. Defaults to PD_DEFAULT_THREAD_STACK_SIZE.
	void				*m_userData;			// The pointer to user data for the new thread.
	EThreadPriority		m_priority;				// The priority of created thread relative to the current thread. Defaults to PE_THREADPRIORITY_SAME.
	const char			*m_threadName;			// Optional. The name for thread, which can be up to 27 characters excluding the NULL terminator. Defaults to NULL.
	U4				m_flags;				// The creation flags for this thread. This value is constructed from the logical OR of PThreadCreationFlags.

public:
	SCreateThreadArgs(ThreadEntryPoint_F entryPoint);
};

class PThreadWin32
{
	HANDLE m_handle;								// The Win32 handle for the thread.

protected:
	PThreadWin32();
	bool initialize(const SCreateThreadArgs &threadInit);
	bool join(U4 &threadResult);
	bool terminate();
};
