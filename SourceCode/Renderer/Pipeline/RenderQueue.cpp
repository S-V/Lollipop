/*
=============================================================================
	File:	Backend.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Core/Util/Tweakable.h>
#include <Renderer/Core/Material.h>
#include <Renderer/Scene/RenderEntity.h>


#include <Renderer/Pipeline/RenderQueue.h>


#if MX_EDITOR

const char* ERenderStageToChars( ERenderStage e )
{
	switch( e )
	{
	case RS_Deferred_FillGBuffer :	return "RS_Deferred_FillGBuffer";
	case RS_Deferred_Lighting :	return "RS_Deferred_Lighting";
	case RS_Unlit_Opaque :	return "RS_Forward_Opaque";
	case RS_Forward_Opaque :	return "RS_Forward_Opaque";
	case RS_Forward_SSS :	return "RS_Forward_SSS";
	case RS_Forward_Translucency :	return "RS_Forward_Translucency";
	mxNO_SWITCH_DEFAULT;
	}
	return nil;
}

const char* EDrawOrderToChars( EDrawOrder e )
{
	switch( e )
	{
	case DO_Background :	return "Background";
	case DO_Opaque :	return "Opaque";
	case DO_Transparent :	return "Transparent";
	mxNO_SWITCH_DEFAULT;
	}
	return nil;
}

#endif // MX_EDITOR

#if 0
void rxSurface::DbgPrint()
{
	const UINT sortKey_offset = OFFSET_OF(rxSurface,sortKey);
	mxPutf("offset_of( rxSurface::sortKey ) = %u\n", sortKey_offset );

	rxSurface	opaqueBatch;
	opaqueBatch.key = 0;
	opaqueBatch.material = 1;
	opaqueBatch.order = DO_Opaque;

	rxSurface	translucentBatch;
	translucentBatch.key = 0;
	translucentBatch.material = 2;
	translucentBatch.order = DO_Transparent;

	MX_DEBUG_BREAK;
}
//--------------------------------------------------------------
static inline
void RadixSortPass( const UINT byte, const UINT size, const rxSurface* source, rxSurface *dest )
{
	UINT	count[ 256 ] = { 0 };
	UINT	index[ 256 ];
	UINT	i;
	BYTE *	sortKey = nil;
	BYTE *	end = nil;

	sortKey = ( (BYTE*) &source[ 0 ].sortKey ) + byte;
	end = sortKey + ( size * (sizeof rxSurface) );
	for( ; sortKey < end; sortKey += (sizeof rxSurface) )
	{
		++count[ *sortKey ];
	}
	index[ 0 ] = 0;

	for( i = 1; i < 256; ++i )
	{
		index[ i ] = index[ i - 1 ] + count[ i - 1 ];
	}

	sortKey = ( (BYTE*) &source[ 0 ].sortKey ) + byte;
	for( i = 0; i < size; ++i, sortKey += (sizeof rxSurface) )
	{
		dest[ index[ *sortKey ]++ ] = source[ i ];
	}
}
//--------------------------------------------------------------
mxMT_UNSAFE
static inline
void SortBatches_RadixSort( rxSurface* source, UINT size )
{
	static rxSurface scratch[ MAX_BATCHES ];

	// Radix sort with 4 byte size buckets.

	StaticAssert( FIELD_SIZE(rxSurface,sortKey) == 4 * (sizeof BYTE) );

#if MX_LITTLE_ENDIAN
	RadixSortPass( 0, size, source, scratch );
	RadixSortPass( 1, size, scratch, source );
	RadixSortPass( 2, size, source, scratch );
	RadixSortPass( 3, size, scratch, source );
#else
	RadixSortPass( 3, size, source, scratch );
	RadixSortPass( 2, size, scratch, source );
	RadixSortPass( 1, size, source, scratch );
	RadixSortPass( 0, size, scratch, source );
#endif //!MX_LITTLE_ENDIAN
}
#endif


rxRenderQueue::rxRenderQueue()
	: surfaces(_InitZero)
	, localLights(_InitZero)
{
	this->Empty();
}

rxRenderQueue::~rxRenderQueue()
{

}

void rxRenderQueue::Initialize()
{
	//surfaces.Reserve(1024);
}

void rxRenderQueue::Shutdown()
{
	surfaces.Clear();
	localLights.Clear();
}

void rxRenderQueue::Empty()
{
	ZERO_OUT(numBatches);
	surfaces.Empty();

	globalLights.Empty();

	ZERO_OUT(numLocalLights);
	localLights.Empty();

	sky = nil;
}

void rxRenderQueue::Sort()
{
#if MX_EDITOR && RX_DEBUG_RENDER_QUEUE
	static bool bDumpRenderQueue = HOT_BOOL(bDumpRenderQueue);
	if(bDumpRenderQueue){
		DBG_DO_INTERVAL(this->DbgPrint("\n---Render queue before sorting:\n"),2000);
	}
#endif // MX_EDITOR && RX_DEBUG_RENDER_QUEUE


	// Sort objects by material, entity index and eye-space depth.
	{
		rxSurface* batches = this->GetBatches();
		const UINT numBatches = this->NumBatches();

		if( numBatches > 1 )
		{
			//SortBatches_RadixSort( batches, numBatches );
			rxSurface::Compare	predicate;
			NxQuickSort( batches, batches + numBatches - 1, predicate );
		}
	}

	// Sort lights.
	{
		const UINT numLights = this->localLights.Num();
		rxLightEntry* lights = this->localLights.ToPtr();

		if( numLights > 1 )
		{
			rxLightEntry::Compare	predicate;
			NxQuickSort( lights, lights + numLights - 1, predicate );
		}
	}

#if MX_EDITOR && RX_DEBUG_RENDER_QUEUE
	if(bDumpRenderQueue){
		DBG_DO_INTERVAL(this->DbgPrint("\n---Render queue after sorting:\n"),2000);
	}
#endif // MX_EDITOR && RX_DEBUG_RENDER_QUEUE
}

#if MX_EDITOR && RX_DEBUG_RENDER_QUEUE
void rxRenderQueue::DbgPrint( PCSTR header ) const
{
	if( header != nil )
	{
		mxPut(header);
	}

	const UINT numBatches = this->NumBatches();

	//mxPutf("%u surfaces(%u opaque, %u translucent)\n",
	//	numBatches,
	//	this->numBatches[RS_Deferred_FillGBuffer],
	//	this->numBatches[RS_Deferred_Translucency]
	//);

	UINT	iBatch = 0;

	while( iBatch < numBatches )
	{
		const rxSurface & batch = this->surfaces[ iBatch ];

		rxMaterial *	material = batch.k.material;
		//rxRenderEntity *entity = batch.e.entity;

		AssertPtr( material );
		//AssertPtr( entity );

		ERenderStage stage;
		EDrawOrder order;

		BatchSortKey64::DecomposeSort32( batch.k.sort, &stage, &order );


		mxPutf("[%u] material: %p = '%s' (%s, %s)\n",
			iBatch,
			material, material->rttiGetTypeName(),
			ERenderStageToChars(stage),EDrawOrderToChars(order)
		);

		++iBatch;
	}
}
#endif // MX_EDITOR && RX_DEBUG_RENDER_QUEUE


NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
