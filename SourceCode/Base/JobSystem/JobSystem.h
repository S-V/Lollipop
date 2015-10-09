/*
=============================================================================
	File:	JobSystem.h
	Desc:	
=============================================================================
*/

#ifndef __MX_JOB_SYSTEM_H__
#define __MX_JOB_SYSTEM_H__

// the serial job system is always enabled on single-core CPUs
#define MX_USE_SERIAL_JOB_SYSTEM	(0)

#define MX_DEBUG_ASYNC_JOB_SYSTEM	(1)


// Config
enum { TP_MAX_THREADS = 32 };

/*
-----------------------------------------------------------------------------
	AsyncJob
-----------------------------------------------------------------------------
*/
struct AsyncJob
{
public:	// Declarations
	enum EJobPriority
	{
		Priority_Low = 0,
		Priority_Normal,
		Priority_High,
		Priority_Realtime,
	};

public:

	virtual ~AsyncJob()
	{}

	// Execute the task
	// Perform a unit of work which is independent of the rest,
	// i.e. one which can be performed in parallel.
	virtual int Run() = 0;
};

/*
-----------------------------------------------------------------------------
	AsyncJobQueue

	A system for performing parallelized tasks using multiple processors.
-----------------------------------------------------------------------------
*/
class AsyncJobQueue
{
public:	// Declarations

	// The construction info for the thread pool
	struct SInitArgs
	{
		// The number of threads (default=0)
		UINT	numThreads;

		UINT	maxJobs;
		
		// The program stack size for each thread
		UINT	threadStackSize;

		UINT	threadWorkspaceSize;

	public:
		SInitArgs();
	};

public:
	AsyncJobQueue() {}
	virtual ~AsyncJobQueue() {}

	virtual void Initialize( const SInitArgs& cInfo = SInitArgs() ) = 0;
	virtual void Release() = 0;

	virtual void AddTask( AsyncJob* newTask ) = 0;

	virtual void WaitForAllJobs() = 0;

public:
	static AsyncJobQueue* StaticCreate();
};

#endif /* !__MX_JOB_SYSTEM_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
