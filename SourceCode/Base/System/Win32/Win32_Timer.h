/*
=============================================================================
	File:	Win32_Timer.h
	Desc:	A very simple high resolution Win32 timer class.
			Swiped from Bullet ( OGRE ).
=============================================================================
*/

#ifndef __MX_WIN32_TIMER_H__
#define __MX_WIN32_TIMER_H__

mxNAMESPACE_BEGIN

#define mymin(a,b) (a < b ? a : b)

//
//	The mxTimer is a basic clock that measures accurate time in seconds, used for profiling.
//
class mxTimer {
public:
	mxTimer()
	{
		Initialize();
		Reset();
	}

	~mxTimer()
	{
	}

	/// Resets the initial reference time.
	void Reset()
	{
		::QueryPerformanceCounter(&mStartTime);
		mStartTick = ::GetTickCount();
		mPrevElapsedTime = 0;
	}

	/// Returns the time in ms since the last call to reset or since 
	/// the mxTimer was created.
	unsigned long int GetTimeMilliseconds()
	{
		//MX_ASSERT(!mxMemIsZero(&mClockFrequency,sizeof(mClockFrequency)));

		LARGE_INTEGER currentTime;
		::QueryPerformanceCounter(&currentTime);
		LONGLONG elapsedTime = currentTime.QuadPart - 
			mStartTime.QuadPart;

		// Compute the number of millisecond ticks elapsed.
		unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
			mClockFrequency.QuadPart);

		// Check for unexpected leaps in the Win32 performance counter.  
		// (This is caused by unexpected data across the PCI to ISA 
		// bridge, aka south bridge.  See Microsoft KB274323.)
		unsigned long elapsedTicks = ::GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(msecTicks - elapsedTicks);
		if (msecOff < -100 || msecOff > 100)
		{
			// Adjust the starting time forwards.
			LONGLONG msecAdjustment = mymin(msecOff * 
				mClockFrequency.QuadPart / 1000, elapsedTime - 
				mPrevElapsedTime);
			mStartTime.QuadPart += msecAdjustment;
			elapsedTime -= msecAdjustment;

			// Recompute the number of millisecond ticks elapsed.
			msecTicks = (unsigned long)(1000 * elapsedTime / 
				mClockFrequency.QuadPart);
		}

		// Store the current elapsed time for adjustments next time.
		mPrevElapsedTime = elapsedTime;

		return msecTicks;
	}

	/// Returns the time in us since the last call to reset or since 
	/// the mxTimer was created.
	unsigned long int GetTimeMicroseconds()
	{
		//MX_ASSERT(!mxMemIsZero(&mClockFrequency,sizeof(mClockFrequency)));

		LARGE_INTEGER currentTime;
		::QueryPerformanceCounter(&currentTime);
		LONGLONG elapsedTime = currentTime.QuadPart - 
			mStartTime.QuadPart;

		// Compute the number of millisecond ticks elapsed.
		unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
			mClockFrequency.QuadPart);


		// Check for unexpected leaps in the Win32 performance counter.  
		// (This is caused by unexpected data across the PCI to ISA 
		// bridge, aka south bridge.  See Microsoft KB274323.)
		unsigned long elapsedTicks = ::GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(msecTicks - elapsedTicks);
		if (msecOff < -100 || msecOff > 100)
		{
			// Adjust the starting time forwards.
			LONGLONG msecAdjustment = mymin(msecOff * 
				mClockFrequency.QuadPart / 1000, elapsedTime - 
				mPrevElapsedTime);
			mStartTime.QuadPart += msecAdjustment;
			elapsedTime -= msecAdjustment;
		}

		// Store the current elapsed time for adjustments next time.
		mPrevElapsedTime = elapsedTime;

		// Convert to microseconds.
		unsigned long usecTicks = (unsigned long)(1000000 * elapsedTime / 
			mClockFrequency.QuadPart);

		return usecTicks;
	}

	void Initialize()
	{
		::QueryPerformanceFrequency( &mClockFrequency );
	}

private:
	LARGE_INTEGER	mClockFrequency;
	DWORD			mStartTick;
	LONGLONG		mPrevElapsedTime;
	LARGE_INTEGER	mStartTime;
};

#undef mymin

mxNAMESPACE_END

#endif // ! __MX_WIN32_TIMER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
