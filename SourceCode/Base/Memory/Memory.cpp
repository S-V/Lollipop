/*
=============================================================================
	File:	Memory.cpp
	Desc:	Memory management.
	Note:	all functions have been made to allocate 16-byte aligned memory blocks.
	ToDo:	use several memory heaps
	(on Windows use heaps with low fragmentation feature turned on)
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#if MX_DEBUG_MEMORY
#include <memory/Visual Leak Detector/include/vld.h>
#endif // MX_DEBUG_MEMORY

#include <Base/Util/LogUtil.h>
#include "Memory_Private.h"

#include <Base/Memory/Stack/UnMem.h>
//#include "Debug/CallStackTracingProxy.h"

#if MX_OVERRIDE_GLOBAL_NEWDELETE && MX_USE_SSE

	mxMSG(Global 'new' and 'delete' should return 16-byte aligned memory blocks for SIMD.)

#endif //!MX_OVERRIDE_GLOBAL_NEWDELETE


namespace
{
	enum { MAX_MEMORY_MANAGERS = 32 };
	TStaticArray< mxMemoryManager*, MAX_MEMORY_MANAGERS >	g_memoryMgrs(_InitZero);
	UINT g_numMemoryMgrs = 0;

	TStaticArray< mxMemoryStatistics, MAX_MEMORY_MANAGERS >	g_memStats(_InitZero);

}//namespace

/*
===========================================================
	mxGetMemoryHeapName
===========================================================
*/
const char* mxGetMemoryHeapName( EMemHeap code )
{
	local_ const char* names[ EMemHeap::HeapCount ] =
	{
#define DECLARE_MEMORY_HEAP( enm, data )	#enm
	#include "MemoryHeaps.inl"
#undef DECLARE_MEMORY_HEAP
	};
	return names[ code ];
}

/*
===========================================================
	F_RegisterMemoryManager
===========================================================
*/
HMemory F_RegisterMemoryManager( mxMemoryManager* mgr )
{
	// Find empty slot.
	
	for( UINT iMgr = 0; iMgr < MAX_MEMORY_MANAGERS; iMgr++ )
	{
		if( nil == g_memoryMgrs[ iMgr ] )
		{
			g_memoryMgrs[ iMgr ] = mgr;
			g_numMemoryMgrs++;
			return iMgr;
		}
	}

	return INDEX_NONE;
}

/*
===========================================================
	F_GetMemoryManager
===========================================================
*/
mxMemoryManager* F_GetMemoryManager( HMemory mgr )
{
	return g_memoryMgrs[ mgr ];
}

/*
===========================================================
	F_UnregisterMemoryManager
===========================================================
*/
void F_UnregisterMemoryManager( HMemory mgr )
{
#if !MX_DEBUG
	if( mgr > 0 && mgr < MAX_MEMORY_MANAGERS )
#endif
	{
		g_memoryMgrs[ mgr ] = nil;
		g_numMemoryMgrs--;
	}
}
/*
===========================================================
	F_GetNumRegisteredMemoryManagers
===========================================================
*/
UINT F_GetNumRegisteredMemoryManagers()
{
	return g_numMemoryMgrs;
}


//-------------------------------------------------------------------------

void* F_HeapAlloc( HMemory heap, SizeT numBytes )
{
	mxMemoryManager* mgr = F_GetMemoryManager( heap );
	void* ptr = mgr->Allocate( numBytes );

	g_memStats[heap].UpdateOnAllocation( numBytes );

	return ptr;
}

void F_HeapFree( HMemory heap, void* pointer )
{
	mxMemoryManager* mgr = F_GetMemoryManager( heap );


	SizeT numBytes = mgr->SizeOf( pointer );
	g_memStats[heap].UpdateOnDeallocation( numBytes );


	mgr->Free( pointer );
}

SizeT F_HeapSizeOfMemoryBlock( HMemory heap, const void* pointer )
{
	return F_GetMemoryManager( heap )->SizeOf( pointer );
}

//-------------------------------------------------------------------------

SizeT F_GetMaxAllowedAllocationSize()
{
	//return 128*1024*1024;	//128 Mb
	return 32*mxMEGABYTE;
}

void* F_SysAlloc( SizeT numBytes )
{
	Assert(numBytes <= F_GetMaxAllowedAllocationSize());

	//numBytes = ALIGN16(numBytes);

	void* pNewMem = ::_aligned_malloc( numBytes, EFFICIENT_ALIGNMENT );
	AssertPtr(pNewMem);

	if( nil == pNewMem )
	{
		//(*g_onMallocFailed)( numBytes );
		mxFatalf("SysAlloc() failed (%ul bytes)\n", (ULONG)numBytes);
	}

	SizeT actualNumBytes = F_SysSizeOfMemoryBlock( pNewMem );

	g_memStats[EMemHeap::HeapProcess].UpdateOnAllocation( actualNumBytes );

	return pNewMem;
}

void F_SysFree( void* pointer )
{
	// deleting null pointer is valid in ANSI C++
	if( !pointer ) {
		return;
	}

	SizeT numBytes = F_SysSizeOfMemoryBlock( pointer );
	g_memStats[EMemHeap::HeapProcess].UpdateOnDeallocation( numBytes );

	::_aligned_free( pointer );
}

SizeT F_SysSizeOfMemoryBlock( const void* pointer )
{
	if( pointer == nil )
	{
		return 0;
	}
	return ::_aligned_msize( (void*)pointer, EFFICIENT_ALIGNMENT, 0 /*offset*/ );
}

void F_MemDbg_ValidateHeap()
{
	_CrtCheckMemory();
}

/*================================
		mxMemoryStatistics
================================*/

static void WriteMemHeapStats( const mxMemoryStatistics& stats, mxOutputDevice& log )
{
	log.Logf( LL_Info,
		"\nAllocated now: %u bytes, allocated in total: %u bytes, total allocations: %u"
		"\nDeallocated in total: %u bytes, total deallocations: %u"
		"\nPeak memory usage: %u bytes"
		,stats.bytesAllocated ,stats.totalAllocated ,stats.totalNbAllocations
		,stats.totalFreed ,stats.totalNbDeallocations
		,stats.peakMemoryUsage
		);
}

void mxMemoryStatistics::Dump( mxOutputDevice& log )
{
	WriteMemHeapStats( *this, log );
}

/*
================================
	F_GetGlobalMemoryStats
================================
*/
void F_GetGlobalMemoryStats( mxMemoryStatistics &outStats )
{
	outStats = g_memStats[EMemHeap::HeapProcess];
}

/*
================================
	F_DumpGlobalMemoryStats
================================
*/
void F_DumpGlobalMemoryStats( mxOutputDevice& log )
{
	CalendarTime	localTime( CalendarTime::GetCurrentLocalTime() );
	StackString	timeOfDay;
	GetTimeOfDayString( timeOfDay, localTime.hour, localTime.minute, localTime.second );

	log.Logf( LL_Info, "\n\n[%s] Memory heap stats:\n", timeOfDay.ToChars() );

	mxMemoryStatistics	totalStats;

	for( UINT iMemHeap = 0; iMemHeap < EMemHeap::HeapCount; iMemHeap++ )
	{
		const mxMemoryStatistics& heapStats = g_memStats[ iMemHeap ];

		log.Logf( LL_Info, "\n\n--- [%u] Memory heap: '%s' ----------", iMemHeap, mxGetMemoryHeapName( (EMemHeap)iMemHeap ) );
		WriteMemHeapStats( heapStats, log );

		totalStats.bytesAllocated += heapStats.bytesAllocated;
		totalStats.totalAllocated += heapStats.totalAllocated;
		totalStats.totalNbAllocations += heapStats.totalNbAllocations;
		totalStats.peakMemoryUsage += heapStats.peakMemoryUsage;

		totalStats.totalNbReallocations += heapStats.totalNbReallocations;

		totalStats.totalFreed += heapStats.totalFreed;
		totalStats.totalNbDeallocations += heapStats.totalNbDeallocations;
	}

	log.Logf( LL_Info, "\n--- Total ----------\n" );
	WriteMemHeapStats( totalStats, log );
}

/*================================
		mxSystemMemoryManager
================================*/

void* mxSystemMemoryManager::Allocate( SizeT numBytes )
{
	Assert(numBytes <= F_GetMaxAllowedAllocationSize());

	return F_SysAlloc( numBytes );
}

void mxSystemMemoryManager::Free( void* pMemory )
{
	if( !pMemory ) {
		return;
	}
	F_SysFree( pMemory );
}

SizeT mxSystemMemoryManager::SizeOf( const void* ptr ) const
{
	return F_SysSizeOfMemoryBlock( ptr );
}

/*================================
		HMemMgr
================================*/

HMemMgr::HMemMgr( const HMemory memHeap )
	: mMemory( memHeap )
{
}

void* HMemMgr::Alloc( SizeT numBytes )
{
	return F_GetMemoryManager( mMemory )->Allocate( numBytes );
}

void HMemMgr::Free( void* pointer )
{
	F_GetMemoryManager( mMemory )->Free( pointer );
}

SizeT HMemMgr::SizeOf( const void* pointer )
{
	return F_GetMemoryManager( mMemory )->SizeOf( pointer );
}

//-------------------------------------------------------------------------

namespace
{

	mxSystemMemoryManager	TheSysMemMgr;

}//namespace

void F_SetupMemorySubsystem()
{
	//DBG_TRACE_CALL;

	mxMemoryManager* pTheGlobalMemMgr = nil;
	

	pTheGlobalMemMgr = &TheSysMemMgr;


	pTheGlobalMemMgr->Initialize();

	Assert(0 == F_GetNumRegisteredMemoryManagers());

	for( UINT iMgr = 0; iMgr < EMemHeap::HeapCount; iMgr++ )
	{
		F_RegisterMemoryManager( pTheGlobalMemMgr );
	}

	g_memoryMgrs[ EMemHeap::HeapProcess ] = &TheSysMemMgr;


}

void F_ShutdownMemorySubsystem()
{
	//DBG_TRACE_CALL;

#if 0//MX_DEBUG_MEMORY && MX_DEVELOPER
	mxLogger_FILE		memoryStatsLog("MemStats.log");
	F_DumpGlobalMemoryStats(memoryStatsLog);
	//TheDbgMemMgrProxy.DumpAllocs(memoryStatsLog);
#endif



	for( UINT iMgr = 0; iMgr < EMemHeap::HeapCount; iMgr++ )
	{
		mxMemoryManager* mgr = F_GetMemoryManager( iMgr );

		if( mgr != nil ) {
			mgr->Shutdown();
		}

		F_UnregisterMemoryManager( iMgr );
	}
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
