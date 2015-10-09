/*
=============================================================================
	File:	Win32_Threading.h
	Desc:
=============================================================================
*/
#pragma once

//---------------------------------------------------------------
//		Defines.
//---------------------------------------------------------------

// 1 - select implementation based on "Fast critical sections with timeout" by Vladislav Gelfer
#define MX_USER_CRITICAL_SECTION	0

//
// thread local storage specifier
//
#define MX_THREAD_LOCAL		__declspec( thread )


/*
=====================================================================

			Thread-local storage.

=====================================================================
*/
#if defined(WIN32) || defined(WIN64) || defined(XENON)
#ifndef XENON
	extern "C" {
		__declspec(dllimport) unsigned long __stdcall TlsAlloc();
		__declspec(dllimport) void* __stdcall TlsGetValue(unsigned long dwTlsIndex);
		__declspec(dllimport) int __stdcall TlsSetValue(unsigned long dwTlsIndex, void* lpTlsValue);
	}
#endif

	#define TLS_DECLARE(type,var) extern int var##idx;
	#define TLS_DEFINE(type,var) \
	int var##idx; \
	struct Init##var { \
		Init##var() { var##idx = TlsAlloc(); } \
	}; \
	Init##var g_init##var;
	#define TLS_GET(type,var) (type)TlsGetValue(var##idx)
	#define TLS_SET(var,val) TlsSetValue(var##idx,(void*)(val))
#else
#if defined(LINUX)
	#define TLS_DECLARE(type,var) extern __thread type var;
	#define TLS_DEFINE(type,var) __thread type var = 0;
	#define TLS_GET(type,var) (var)
	#define TLS_SET(var,val) (var=(val))
#else
	#define TLS_DECLARE(type,var) extern THREADLOCAL type var;
	#define TLS_DEFINE(type,var) THREADLOCAL type var;
	#define TLS_GET(type,var) (var)
	#define TLS_SET(var,val) (var=(val))
#endif // defined(LINUX)
#endif










//
//	Intrinsics.
//

//ReadWriteBarrier prevents the compiler from re-ordering memory
//accesses accross the barrier.

extern "C" void _WriteBarrier();
extern "C" void _ReadWriteBarrier();

#pragma intrinsic(_WriteBarrier)
#pragma intrinsic(_ReadWriteBarrier)

#define ReadWriteBarrier _ReadWriterBarrier

//MemoryBarrier prevents the CPU from reordering memory access across
//the barrier (all memory access will be finished before the barrier
//is crossed).

// MemoryBarrier is in WinNT.h.

/*
=====================================================================
	
			Atomic operations.

=====================================================================
*/

mxNAMESPACE_BEGIN

// integer type used for atomic locks
typedef volatile LONG	AtomicInt;


FORCEINLINE int AtomicIncrement( AtomicInt& var )
{
	return ::InterlockedIncrement( (AtomicInt*)&var );
}

FORCEINLINE int AtomicDecrement( AtomicInt& var )
{
	return ::InterlockedDecrement( (AtomicInt*)&var );
}

FORCEINLINE int AtomicAdd( AtomicInt& var, int add )
{
	return ::InterlockedExchangeAdd( (AtomicInt*)&var, add );
}

// Sets a 32-bit 'dest' variable to the specified value as an atomic operation.
// The function sets this variable to 'value', and returns its prior value.
//
FORCEINLINE int AtomicExchange( AtomicInt* dest, int value )
{
	return ::InterlockedExchange( dest, value );
}

//
// Atomic Compare and Swap.
// Performs an atomic compare-and-exchange operation on the specified values.
// The function compares two specified 32-bit values and exchanges
// with another 32-bit value based on the outcome of the comparison.
//
// Arguments:
// 'dest' - A pointer to the destination value.
// 'comparand' - The value to compare to 'dest'.
// 'exchange' - The exchange value.
// 
// Returns
// the previous value of 'dest'
//
FORCEINLINE int AtomicCompareExchange( AtomicInt* dest, int comparand, int exchange )
{
	return ::InterlockedCompareExchange( dest, exchange, comparand );
}

// Atomic Compare and Swap.
// Performs an atomic compare-and-exchange operation on the specified values.
// The function compares two specified 32-bit values and exchanges
// with another 32-bit value based on the outcome of the comparison.
//
// Returns
// 'true' if swap operation has occurred
//
FORCEINLINE bool AtomicCAS( AtomicInt* valuePtr, int oldValue, int newValue )
{
	return ::InterlockedCompareExchange( valuePtr, newValue, oldValue ) == oldValue;
}


// Description:
// Atomically increments a value.
// Arguments:
// value - The pointer to the value.
// incAmount - The amount to increment by.
//
INLINE void AtomicIncrement( AtomicInt* value, int incAmount )
{
	AtomicInt count = *value;

	while( !AtomicCAS( value, count, count + incAmount ) )
	{
		count = *value;
	}
}

// Description:
// Atomically decrements a value.
// Arguments:
// value - The pointer to the value.
// decAmount - The amount to decrement by.
//
INLINE void AtomicDecrement( AtomicInt* value, int decAmount )
{
	AtomicInt count = *value;

	while( !AtomicCAS( value, count, count - decAmount ) )
	{
		count = *value;
	}
}


// A very simple spin lock for 'busy waiting'.
// The calling process obtains the lock if the old value was 0.
// It spins writing 1 to the variable until this occurs.
//
INLINE void SpinLock( AtomicInt* valuePtr,int oldValue,int newValue )
{
	while( !AtomicCAS( valuePtr, oldValue, newValue ) )
		;
}

class AtomicLock
{
public:
	INLINE AtomicLock( AtomicInt* theValue )
		: m_valuePtr( theValue )
	{
		SpinLock( m_valuePtr, 0, 1 );
	}
	INLINE ~AtomicLock()
	{
		SpinLock( m_valuePtr, 1, 0 );
	}

private:
	AtomicInt *	m_valuePtr;
};


INLINE void WaitLock( AtomicInt* valuePtr, int value )
{
	while( !AtomicCAS( valuePtr, value, value ) )
		;
}



//
//	mxSafeFlag - A thread-safe flag variable.
//
class mxSafeFlag {
public:
			mxSafeFlag();

			// set the flag
	void	Set();

			// clear the flag
	void	Clear();

			// test if the flag is set
	bool	Test() const;

			// test if flag is set, if yes, clear flag
	bool	TestAndClearIfSet();

private:
    AtomicInt	flag;
};

FORCEINLINE
mxSafeFlag::mxSafeFlag()
	: flag( 0 )
{}

FORCEINLINE void mxSafeFlag::Set()
{
    AtomicExchange( &flag, 1 );
}

FORCEINLINE void mxSafeFlag::Clear()
{
    AtomicExchange( &flag, 0 );
}

FORCEINLINE bool mxSafeFlag::Test() const
{
    return (0 != flag);
}

FORCEINLINE bool mxSafeFlag::TestAndClearIfSet()
{
    return (1 == AtomicCompareExchange( &flag, 0, 1 ));
}

// lock memory so it cannot get swapped out to disk
FORCEINLINE bool mxLockMemory( void *ptr, int bytes )
{
	// Locks the specified region of the process's virtual address space into physical memory,
	// ensuring that subsequent access to the region will not incur a page fault.
	return ::VirtualLock( ptr, bytes );
}

// Unlocks a specified range of pages in the virtual address space of a process, enabling the system to swap the pages out to the paging file if necessary.
FORCEINLINE bool mxUnlockMemory( void *ptr, int bytes )
{
	return ::VirtualUnlock( ptr, bytes );
}

/*
=====================================================================
	
			Synchronization primitives.

=====================================================================
*/

/*
=====================================================================
	Critical sections are used to protect a portion of code
	from parallel execution.
	Define a static critical section object
	and use its Enter() and Leave() methods to protect critical sections
    of your code.
=====================================================================
*/

	//
	//	mxCriticalSection
	//

#if MX_USER_CRITICAL_SECTION

	class mxCriticalSection {
	public:
				mxCriticalSection();
				~mxCriticalSection();

		void	Enter() const;	// enter the critical section
		void	Leave() const;	// leave the critical section

	private:
		// lock with increment
		bool PerfLockImmediate( DWORD dwThreadID ) const;
		
		// lock with semaphore
		bool PerfLock( DWORD dwThreadID ) const;
		
		// performance lock with system wait and kernel mode 
		bool PerfLockKernel( DWORD dwThreadID ) const;
		
		// unlock
		void PerfUnlock() const; 
		
		// increment waiter count
		void WaiterPlus() const; 
		
		// decrement waiter count
		void WaiterMinus() const;
		
		// allocate semaphore if not done yet
		void AllocateKernelSemaphore() const;

		// Declare all variables volatile, so that the compiler won't
		// try to optimize something important.

		mutable volatile	DWORD		lockerThread;
		volatile			DWORD		spinMax;   
		mutable volatile	long		waiterCount; 
		mutable volatile	HANDLE		semaphore;
		mutable				UINT		recursiveLockCount;
	};         

	INLINE void mxCriticalSection::WaiterPlus() const
	{
		::_InterlockedIncrement( &waiterCount );
	}

	INLINE void mxCriticalSection::WaiterMinus() const
	{
		::_InterlockedDecrement( &waiterCount );
	}

	INLINE bool mxCriticalSection::PerfLockImmediate( DWORD dwThreadID ) const 
	{
		return (0 == ::InterlockedCompareExchange( (long*)&lockerThread, dwThreadID, 0 ));
	}

	INLINE void mxCriticalSection::Enter() const
	{
		DWORD threadId = ::GetCurrentThreadId();
		if( threadId != lockerThread )
		{
			if( ( lockerThread == 0 ) && PerfLockImmediate( threadId ) )
			{
				// single instruction atomic quick-lock was successful
			}            
			else
			{
				// potentially locked elsewhere, so do a more expensive lock with system critical section
				PerfLock( threadId );
			}
		}
		recursiveLockCount++;
	}

	INLINE void mxCriticalSection::AllocateKernelSemaphore() const
	{
		if( !semaphore )
		{
			HANDLE handle = ::CreateSemaphore( NULL, 0, 0x7FFFFFFF, NULL );
			assert( NULL != handle );
			if( ::InterlockedCompareExchangePointer( &semaphore, handle, NULL ) )
			{
				::CloseHandle( handle ); // we're late
			}
		}
	}

	INLINE bool mxCriticalSection::PerfLock( DWORD dwThreadID ) const
	{
		// Attempt spin-lock
		for( DWORD dwSpin = 0; dwSpin < spinMax; dwSpin++ )
		{
			if( PerfLockImmediate( dwThreadID ) ) {
				return true;
			}
			YieldProcessor();
		}
	           
		// Ensure we have the kernel event created
		AllocateKernelSemaphore();

		// do a more expensive lock
		bool result = PerfLockKernel( dwThreadID );
		WaiterMinus();

		return result;
	}

	INLINE bool mxCriticalSection::PerfLockKernel( DWORD dwThreadID ) const
	{
		bool waiter = false;

		for (;;)
		{
			if( !waiter ) {
				WaiterPlus();
			}
			if( PerfLockImmediate( dwThreadID ) ) {
				return true;
			}

			assert(semaphore);
			switch( ::WaitForSingleObject( semaphore, INFINITE ) )
			{
			case WAIT_OBJECT_0:
				waiter = false;
				break;

			case WAIT_TIMEOUT:
				waiter = true;
				break;

			default:
				break;
			};
		}
		// unreachable
		//return false;
	}

	INLINE void mxCriticalSection::Leave() const
	{
		if( --recursiveLockCount == 0 )
		{
			PerfUnlock();
		}        
	}

	INLINE void mxCriticalSection::PerfUnlock() const
	{
		_WriteBarrier(); // changes done to the shared resource are committed.

		lockerThread = 0;

		_ReadWriteBarrier(); // The CS is released.

		if( waiterCount > 0 ) // AFTER it is released we check if there're waiters.
		{
			WaiterMinus(); 
			::ReleaseSemaphore( semaphore, 1, NULL );                  
		}
	}

#else

	class mxCriticalSection
	{
		CRITICAL_SECTION	criticalSection;

	public:

		mxCriticalSection()
		{
			//::InitializeCriticalSection( &criticalSection );
			BOOL ret = ::InitializeCriticalSectionAndSpinCount( &criticalSection, 4096 );
			mxASSERT( 0 != ret );
			(void)ret;
		}

		~mxCriticalSection()
		{
			::DeleteCriticalSection( &criticalSection );
		}

		// Locks the critical section.
		void Enter()
		{
			::EnterCriticalSection( &criticalSection );
		}

		// Attempts to acquire exclusive ownership of a mutex.
		bool TryEnter()
		{
			return ( ::TryEnterCriticalSection( &criticalSection ) != FALSE );
		}

		// Releases the lock on the critical seciton.
		void Leave()
		{
			::LeaveCriticalSection( &criticalSection );
		}
	};

#endif // !MX_USER_CRITICAL_SECTION

typedef mxCriticalSection mxMutex;

//
//	mxScopedMutex
//
class mxScopedMutex
{
public:
	FORCEINLINE mxScopedMutex( mxCriticalSection* cs )
		: mutex( cs )
	{
		mutex->Enter();
	}
	FORCEINLINE ~mxScopedMutex()
	{
		mutex->Leave();
	}

private:
	mxCriticalSection * mutex;
};

/*
=====================================================================
			
			Events.

=====================================================================
*/

//
//	mxEvent
//
class mxEvent {
public:
		mxEvent( bool bManualReset = false, bool bSignalled = false, const TCHAR* szName = nil );
		~mxEvent();

	// signal the event
	void Signal();

	// reset the event (only if manual reset)
	void Reset();

	// wait for the event to become signalled
	void Wait( UINT timeoutMsec = INFINITE ) const;

	// wait for the event with timeout in millisecs
	bool WaitTimeout(int ms) const;

	// check if event is signalled
	bool Peek() const;

private:
	HANDLE	mEvent;
};

INLINE mxEvent::mxEvent( bool bManualReset, bool bSignalled, const TCHAR* szName )
{
	mEvent = ::CreateEvent( NULL, bManualReset, bSignalled, szName );
	assert(NULL != mEvent);
}

INLINE mxEvent::~mxEvent()
{
	::CloseHandle(mEvent);
	mEvent = NULL;
}

INLINE void mxEvent::Signal()
{
	::SetEvent( mEvent );
}

INLINE void mxEvent::Reset()
{
	::ResetEvent( mEvent );
}

INLINE void mxEvent::Wait( UINT timeoutMsec ) const
{
	::WaitForSingleObject( mEvent, timeoutMsec );
}

/**
    Waits for the event to become signaled with a specified timeout
    in milliseconds. If the method times out it will return false,
    if the event becomes signalled within the timeout it will return 
    true.
*/
INLINE bool mxEvent::WaitTimeout( int timeoutInMilliSec ) const
{
    DWORD res = ::WaitForSingleObject( mEvent, timeoutInMilliSec );
    return (WAIT_TIMEOUT == res) ? false : true;
}

/**
    This checks if the event is signalled and returnes immediately.
*/
INLINE bool mxEvent::Peek() const
{
	DWORD res = ::WaitForSingleObject( mEvent, 0 );
    return (WAIT_TIMEOUT == res) ? false : true;
}

/*
=====================================================================
	
			Threads.

=====================================================================
*/

	//
	//	EThreadPriority
	//
	enum EThreadPriority
	{
		Thread_Low,
		Thread_Normal,
		Thread_High
	};

	typedef DWORD	ThreadID;	// mxThread identifier.

	static const ThreadID	INVALID_THREAD_ID = 0xFFFFFFFF;

	//
	//	Main thread function.
	//
	typedef unsigned int (*ThreadFunc_t)( void * );

#if 1
	//
	//	mxThread - is a thread of execution.
	//
	class mxThread {
	public:
						mxThread();
		virtual			~mxThread();

		virtual void	Run() = 0;

		// These functions return false in case of failure.

		bool			Create();
		bool			Suspend();
		bool			Resume();
		bool			Wait();	// wait for completion

		// [Dangerous function]
		bool			Kill();

		bool			IsActive() const;

	private:
		HANDLE		hThread;
		DWORD		threadId;	// thread identifier
		bool		bActive;
	};

#else
	class mxThread {
	public:
		virtual ~mxThread() {}

		// Sets the priority of a thread.

		virtual void	SetThreadPriority( EThreadPriority newPriority ) = 0;

		// Indicates whether a thread has completed execution.

		virtual bool	IsComplete() const = 0;

		// Suspends the current thread for a period of time. Accepts the length of time to sleep, in milliseconds.

		virtual void	Suspend( unsigned long Ms ) = 0;	// Sleep is already defined.

		// Yields the time slice for the current thread.
		// Notes: The Yield function causes the calling thread to give up the remainder of its time slice.
		// The thread will continue running normally once the operating system schedules it for execution again.

		virtual void	YieldTime( void ) = 0;	// Yield is already defined.
	};
#endif

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
