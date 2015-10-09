/*
=============================================================================
	File:	pxBroadphasePair.h
	Desc:	
=============================================================================
*/

#ifndef __PX_BROADPHASE_PAIR_H__
#define __PX_BROADPHASE_PAIR_H__




#if 0

	//
	//	pxBroadphasePair
	//
	MX_ALIGN_16(class) pxBroadphasePair
	{
		PX_DECLARE_POD_ALLOCATOR( pxBroadphasePair, PX_MEMORY_COLLISION_BROADPHASE );

	public_internal:
		pxBroadphaseHandle *	proxy0;
		pxBroadphaseHandle *	proxy1;

	public:
		PX_INLINE pxBroadphasePair()
			: proxy0( nil ), proxy1( nil )
		{}
	};

	typedef TList< pxBroadphasePair >	pxBroadphasePairList;

#else


	union pxBroadphasePair
	{
		struct {
			pxU2	id0;
			pxU2	id1;
		};
		pxU4	id;

	public:
		FORCEINLINE void Order()
		{
			if( id0 > id1 ) {
				TSwap( id0, id1 );
			}
		}
	};

#endif


#endif // !__PX_BROADPHASE_PAIR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
