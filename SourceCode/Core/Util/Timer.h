/*
=============================================================================
	File:	Timer.h
	Desc:	Game timer.
=============================================================================
*/

#ifndef __MX_CORE_UTIL_TIMER_H__
#define __MX_CORE_UTIL_TIMER_H__

mxNAMESPACE_BEGIN

class GameTimer
{
	mxTimer	timer;
	F4		deltaSeconds;

public:
	GameTimer()
	{
		Reset();
	}

	void Reset()
	{
		this->timer.Reset();
		this->deltaSeconds = 0.0f;
	}

	F4 TickFrame()
	{
		unsigned long int t = this->timer.GetTimeMicroseconds();
		this->timer.Reset();

		double dt = double(t) * 1e-6;
		this->deltaSeconds = dt;

		return this->deltaSeconds;
	}

	F4 DeltaSeconds() const
	{
		return this->deltaSeconds;
	}

	//ULONG GetTimeMilliseconds() const
	//{
	//	return this->timer.GetTimeMilliseconds();
	//}
	//ULONG GetTimeMicroseconds() const
	//{
	//	return this->timer.GetTimeMicroseconds();
	//}
};

mxNAMESPACE_END

#endif // !__MX_CORE_UTIL_TIMER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
