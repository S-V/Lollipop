/*
=============================================================================
	File:	FPSTracker.h
	Desc:	
=============================================================================
*/

#ifndef __MX_FPS_TRACKER_H__
#define __MX_FPS_TRACKER_H__

mxNAMESPACE_BEGIN

//--------------------------------------------------------------//

mxSWIPED("based on code written by MJP (Matt Pettineo)");
template< UINT HISTORY_SIZE = 64 >
class FPSTracker
{
	FLOAT	m_historyBuffer[ HISTORY_SIZE ];	// time deltas for smoothing
	UINT	m_currentTimeDeltaSample;

public:
	FPSTracker()
	{
		this->Reset();
	}
	void Reset()
	{
		ZERO_OUT(m_historyBuffer);
		m_currentTimeDeltaSample = 0;
	}
	// Call this function once per frame.
	FLOAT CalculateFPS( FLOAT deltaSeconds )
	{
		Assert(deltaSeconds > 0.0f);

		m_historyBuffer[ m_currentTimeDeltaSample ] = deltaSeconds;
		m_currentTimeDeltaSample = (m_currentTimeDeltaSample + 1) % HISTORY_SIZE;

		float averageDelta = 0;
		for(UINT i = 0; i < HISTORY_SIZE; ++i) {
			averageDelta += m_historyBuffer[i];
		}
		averageDelta /= HISTORY_SIZE;

		Assert(averageDelta > 0.0f);

		return mxFloor( (1.0f / averageDelta) + 0.5f );
	}
};

mxNAMESPACE_END

#endif // !__MX_FPS_TRACKER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
