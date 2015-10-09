/*
=============================================================================
	File:	Heap.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

mxSWIPED("OOOII, file: oHeap.h");
#include "Heap.h"

void oHeap::Add(STATISTICS* _pStats, const BLOCK_DESC* _pDesc)
{
	_pStats->Blocks++;
	_pStats->Bytes += _pDesc->Size;
	_pStats->Overhead += _pDesc->Overhead;

	if (_pDesc->Used)
	{
		_pStats->BlocksUsed++;
		_pStats->BytesUsed += _pDesc->Size;
	}

	else
	{
		_pStats->BlocksFree++;
		_pStats->BytesFree += _pDesc->Size;
	}

	if (_pDesc->Size <= SMALL_BLOCK_SIZE) _pStats->BlocksSmall++;
	else if (_pDesc->Size >= LARGE_BLOCK_SIZE) _pStats->BlocksLarge++;
	else _pStats->BlocksMedium++;
}

bool oHeap::WalkExternals(WalkerFn _Walker, DescriberFn _Describer, SummarizerFn _Summarizer, void* _pUserData, long _Flags)
{
	HANDLE heaps[128];
	DWORD numHeaps = ::GetProcessHeaps(NUMBER_OF(heaps), heaps);

	const HANDLE hDefault = ::GetProcessHeap();
	const HANDLE hCrt = (HANDLE)_get_heap_handle();

	for (DWORD i = 0; i < numHeaps; i++)
	{
		ULONG heapInfo = 3;
		SIZE_T dummy = 0;
		::HeapQueryInformation(heaps[i], HeapCompatibilityInformation, &heapInfo, sizeof(heapInfo), &dummy);

		const char* desc = "unknown";
		switch (heapInfo)
		{
		case 0: desc = "regular"; break;
		case 1: desc = "fast w/ look-asides"; break;
		case 2: desc = "low-fragmentation (LFH)"; break;
		default: break;
		}

		DESC d;
		sprintf_s(d.Name, "Heap h(%p)", heaps[i]);
		sprintf_s(d.Description, "%s", desc);

		d.Type = EXTERNAL;
		if (heaps[i] == hDefault)
			d.Type = SYSTEM;
		else if (heaps[i] == hCrt)
			d.Type = LIBC;

		(*_Describer)(&d, _pUserData, _Flags);

		STATISTICS s;
		PROCESS_HEAP_ENTRY e;
		e.lpData = 0;
		while (::HeapWalk(heaps[i], &e))
		{
			BLOCK_DESC b;
			b.Address = e.lpData;
			b.Size = e.cbData;
			b.Overhead = e.cbOverhead;
			b.Used = (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0;
			(*_Walker)(&b, _pUserData, _Flags);
			Add(&s, &b);
		}

		s.Valid = GetLastError() == ERROR_NO_MORE_ITEMS ? true : false;

		(*_Summarizer)(&s, _pUserData, _Flags);
	}

	// todo: walk internal list looking for type == external since there could be a user pool
	// that is still external because its a wrapper for something else.

	return true;
}

void oHeap::SetSmallBlockHeapThreshold(unsigned long long _sbhMaxSize)
{
	AssertX(false, "Functionality no longer exists in vcrt10.");
	//	_set_sbh_threshold(static_cast<size_t>(_sbhMaxSize));
}

void oHeap::GetGlobalDesc(GLOBAL_HEAP_DESC* _pDesc)
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(MEMORYSTATUSEX);
	oVB(GlobalMemoryStatusEx(&ms));
	_pDesc->TotalMemoryUsed = ms.dwMemoryLoad;
	_pDesc->AvailablePhysical = ms.ullAvailPhys;
	_pDesc->TotalPhysical = ms.ullTotalPhys;
	_pDesc->AvailableVirtualProcess = ms.ullAvailVirtual;
	_pDesc->TotalVirtualProcess = ms.ullTotalVirtual;
	_pDesc->AvailablePaged = ms.ullAvailPageFile;
	_pDesc->TotalPaged = ms.ullTotalPageFile;
}

unsigned int oHeap::GetSystemAllocationGranularity()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwAllocationGranularity;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
