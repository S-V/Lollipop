/*
=============================================================================
	File:	TQueueLIFO.h
	Desc:	'last in, first out' queue.
=============================================================================
*/

#ifndef __PX_TQueueLIFO_H__
#define __PX_TQueueLIFO_H__

//
//	TQueueLIFO< TYPE >
//
template< typename TYPE, const SizeT SIZE >
class TQueueLIFO {
public:
	FORCEINLINE TQueueLIFO()
	{}

private:
	typedef TQueueLIFO<TYPE,SIZE> THIS_TYPE;
	PREVENT_COPY(THIS_TYPE);

private:
	TYPE	mData[ SIZE ];
};

#endif // !__PX_TQueueLIFO_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
