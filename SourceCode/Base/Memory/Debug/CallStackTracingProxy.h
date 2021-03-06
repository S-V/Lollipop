// CallStackTracingProxy.h
#pragma once

#include <Base/Math/Hashing/HashFunctions.h>
#include <Base/Templates/Containers/HashMap/TMap.h>

#define debugf(...)

// Memory allocator.

enum
{
	MEMORYIMAGE_NotUsed			= 0,
	MEMORYIMAGE_Loading			= 1,
	MEMORYIMAGE_Saving			= 2,
	MEMORYIMAGE_DebugXMemAlloc	= 4,
};

enum ECacheBehaviour
{
	CACHE_Normal		= 0,
	CACHE_WriteCombine	= 1,
	CACHE_None			= 2
};

//
// C style memory allocation stubs that are safe to call before GMalloc.Init
//
#define appSystemMalloc(num)	::malloc(num)
#define appSystemFree			::free
#define appSystemRealloc		::realloc
#define appSizeOfMem(x)			::_msize(x)

/*=============================================================================
	CallStackTracingProxy.h: Proxy allocator for logging backtrace of 
	                            allocations.
	Copyright 2004 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Daniel Vogel
=============================================================================*/

#pragma pack(push,8)
#include <DbgHelp.h>
#pragma pack(pop)

template<>
struct THashTrait<DWORD>
{
	static FORCEINLINE UINT GetHashCode( const DWORD key )
	{
		return key;
	}
};

/**
 * Debug memory proxy that captures stack backtraces while passing allocations through
 * to the mxMemoryManager passed in via the constructor.
 */
class CallStackTracingProxy : public mxMemoryManager
{
private:
	/** Flags used for allocation information.									*/
	enum EAllocationFlags
	{
		ALLOCATION_Regular = 1,
	};

	//@todo: "static const int" doesn't compile in release mode for some obscure reason.

	/** Maximum string length of symbol name									*/
	#define	MAX_SYMBOL_NAME_LENGTH		1024
	/** Maximum depth of stack backtrace										*/
	#define	MAX_BACKTRACE_DEPTH			16
	/** Maximum length of human readable callstack entry						*/
	#define MAX_CALLSTACK_ENTRY_LENGTH	100

	/** Malloc we're based on, aka using under the hood							*/
	mxMemoryManager *	UsedMalloc;

	/** Current process															*/
	HANDLE				Process;
	/** Current thread															*/
	HANDLE				Thread;
	/** Options used for symbol retrieval										*/
	DWORD				SymOptions;
	/**	Pointer to symbol information											*/
	SYMBOL_INFO*		Symbol;
	/** Symbol Line information													*/
	IMAGEHLP_LINE64		Line;
	
	/**	Whether we are currently dumping allocations or not						*/
	BOOL				CurrentlyDumpingAllocs;

	/**
	 * Debug information kept for each allocation... Consumes A LOT of memory!
	 */
	struct FAllocInfo
	{
		/** Base address of memory allocation									*/
		void*				BaseAddress;
		/** Size of memory allocation											*/
		DWORD				Size;
		/** Flags, e.g. whether this allocation occured during dumping allocs	*/
		DWORD				Flags;
		/** Stack backtrace, MAX_BACKTRACE_DEPTH levels deep					*/
		DWORD64				BackTrace[MAX_BACKTRACE_DEPTH];
		/** CRC of BackTrace, used for output and grouping allocations			*/
		DWORD CRC;

		/** Next allocation in pool												*/
		FAllocInfo*			Next;
		/** Previous allocation in pool											*/
		FAllocInfo*			Previous;

		/**
		 * Links current allocation info to pool. Note that we can only be linked to one pool at a time.
		 *
		 * @param Pool FAllocInfo "this" is going to be linked to.
		 */
		void LinkPool( FAllocInfo*& Pool )
		{
			if( Pool )
			{
				Next		= Pool->Next;
				Previous	= Pool;
				if( Pool->Next )
                    Pool->Next->Previous = this;
				Pool->Next = this;
			}
			else
			{
				Pool		= this;
				Next		= NULL;
				Previous	= NULL;
			}
		}
		/**
		 * Unlinks current allocation info from pool. Note that we can only be linked to one pool at a time.
		 *
		 * @param Pool FAllocInfo "this" is going to be unlinked from.
		 */
		void UnlinkPool( FAllocInfo*& Pool )
		{
			if( Previous )
				Previous->Next = Next;

			if( Next )
				Next->Previous = Previous;

			if( !Previous && !Next )
				Pool = NULL;

			if( !Previous && Next )
				Pool = Next;
		}
	};

	/** Array of allocation infos - one pool per each pointer & 0xFFFF			*/
	FAllocInfo* AllocationPool[0xFFFF];

private:
	/**
	 * Captures stack backtrace for this allocation and also keeps track of pointer & size.
	 *
	 * @param	Ptr		pointer to keep track off	
	 * @param	Size	size of this allocation
	 */
	void InternalMalloc( void* Ptr, DWORD Size )
	{
		// We don't want to waste gobs of memory if we're not keeping track of anything.
		Assert( Ptr );
	
		// Create new FAllocInfo...
		FAllocInfo* Allocation			= (FAllocInfo*) appSystemMalloc( sizeof(FAllocInfo) );
		Assert( Allocation );
		MemZero(Allocation,sizeof(FAllocInfo));
	
		// ... and fill in member variables.
		Allocation->BaseAddress			= Ptr;
		Allocation->Size				= Size;
		
		//Allocation->Flags				= CurrentlyDumpingAllocs ? 0 : ALLOCATION_Regular;
		Allocation->Flags				= ALLOCATION_Regular;

		// Get context record (aka registers used for stack walking).
		CONTEXT	ContextRecord;
		MemZero( &ContextRecord, sizeof(ContextRecord) );
		ContextRecord.ContextFlags		= CONTEXT_CONTROL;
		mxVERIFY( GetThreadContext( Thread, &ContextRecord ) );

		// Initialize stack frame.
		STACKFRAME64 StackFrame;
		MemZero( &StackFrame, sizeof(StackFrame) );
		StackFrame.AddrPC.Offset		= ContextRecord.Eip;
		StackFrame.AddrPC.Mode			= AddrModeFlat;
		StackFrame.AddrFrame.Offset		= ContextRecord.Ebp;
		StackFrame.AddrFrame.Mode		= AddrModeFlat;
		StackFrame.AddrStack.Offset		= ContextRecord.Esp;
		StackFrame.AddrStack.Mode		= AddrModeFlat;
		StackFrame.AddrBStore.Mode		= AddrModeFlat;
		StackFrame.AddrReturn.Mode		= AddrModeFlat;

		// Walk the stack unless we're currently dumping allocations.
		INT Depth = 0;
		while( Depth < MAX_BACKTRACE_DEPTH && !CurrentlyDumpingAllocs )
		{
			if( !StackWalk64( IMAGE_FILE_MACHINE_I386, Process, Thread, &StackFrame, &ContextRecord, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL ) )
				break;
			
			// Keep track of program counter for symbol lookup later on.
			Allocation->BackTrace[Depth++] = StackFrame.AddrPC.Offset;
		}

		// Zero fill unused space.
		for( ; Depth<MAX_BACKTRACE_DEPTH; Depth++ )
			Allocation->BackTrace[Depth] = 0;

		// Use the lower 16 bits of the pointer as an index to a pool and link it to it.
		const UINT iPoolIndex = (DWORD)Ptr & 0xFFFF;
		Allocation->LinkPool( AllocationPool[iPoolIndex] );




		DWORD	Offset		= 0;
		DWORD64	Offset64	= 0;
		BOOL	ValidFromAddr	= SymFromAddr( Process, Allocation->BackTrace[0], &Offset64, Symbol )		? 1 : 0;
		DWORD	ErrorFromAddr	= GetLastError();
		BOOL	ValidLine		= SymGetLineFromAddr64( Process, Allocation->BackTrace[0], &Offset, &Line )	? 1 : 0;
		DWORD	ErrorLine		= GetLastError();

		if( ValidFromAddr && ValidLine )
		{
			printf("File: %s, symbol: %s\n",Line.FileName,Symbol->Name);
		}
	}

	/**
	 * Removes pointer from internal allocation pool.
	 *
	 * @param	Ptr		pointer to free	
	 */
	void InternalFree( void* Ptr )
	{
		// delete NULL is legal ANSI C++
		if( Ptr == NULL )
			return;

		// Try to find allocation in pool indexed by the lower 16 bits of the passed in pointer.
		FAllocInfo* Allocation = AllocationPool[(DWORD)Ptr & 0xFFFF];
		while( Allocation && (Allocation->BaseAddress != Ptr ) )
			Allocation = Allocation->Next;

		// Rather than crashing, we handle double free'ing gracefully as the underlying mxMemoryManager might do the same (e.g. FMallocAnsi).
		if( !Allocation )
		{
			debugf(TEXT("Tried to free pointer [%p] not currently allocated - most likely double free"), Ptr);
			return;
		}

		// Unlink from pool and free allocation info.
		Assert( Allocation->BaseAddress == Ptr );
		Allocation->UnlinkPool( AllocationPool[(DWORD)Ptr & 0xFFFF] );
		appSystemFree( Allocation );
	}

public:
	// mxMemoryManager interface.
	CallStackTracingProxy( mxMemoryManager* InMalloc )
	:	UsedMalloc( InMalloc )
	{}
	void* Allocate( SizeT numBytes )
	{
		//DBG_TRACE_CALL;
		void* Ptr = UsedMalloc->Allocate( numBytes );
		InternalMalloc( Ptr, numBytes );
		return Ptr;
	}
#if 0
	void* Realloc( void* Ptr, DWORD NewSize )
	{
		void* NewPtr = UsedMalloc->Realloc( Ptr, NewSize );
		InternalFree( Ptr );
		if( NewPtr )
			InternalMalloc( NewPtr, NewSize );
		return NewPtr;
	}
#endif
	void Free( void* Ptr ) override
	{
		UsedMalloc->Free( Ptr );
		InternalFree( Ptr );
	}
#if 0
	void* PhysicalAlloc( DWORD Size, ECacheBehaviour InCacheBehaviour )
	{
		void* Ptr = UsedMalloc->PhysicalAlloc( Size, InCacheBehaviour );
		InternalMalloc( Ptr, Size );
		return Ptr;
	}
	void PhysicalFree( void* Ptr )
	{
		UsedMalloc->PhysicalFree( Ptr );
		InternalFree( Ptr );
	}
#endif

	void GetStats( mxMemoryStatistics &outStats ) override
	{
		//
	}

	/** Stats for allocations with same call stack */
	struct FGroupedAllocationInfo
	{
		/** Human readable callstack, separated by comma										*/
		TCHAR		Callstack[MAX_BACKTRACE_DEPTH * MAX_CALLSTACK_ENTRY_LENGTH];
		/** Total size of allocations in this group (aka allocations with the same callstack	*/
		SIZE_T		TotalSize;
		/** Number of allocations in this group													*/
		DWORD		Count;
	};

	void DumpAllocs( mxOutputDevice& log )
	{
		CurrentlyDumpingAllocs = 1;

		// Load symbols.
		//GFileManager->SetDefaultDirectory();
		//verify( SymInitialize( GetCurrentProcess(), ".", 1 ) );
		// Initialize the symbol handler for a process.
		mxVERIFY( SymInitialize( Process, NULL, TRUE ) );

		ANSICHAR	FileName[ MAX_SYMBOL_NAME_LENGTH ];
		ANSICHAR	SymbolName[ MAX_SYMBOL_NAME_LENGTH ];
		SIZE_T		TotalSize	= 0;



		// Group allocations by callstack (or rather CRC of return addresses).
		TMap<DWORD,FGroupedAllocationInfo> GroupedAllocations(EMemHeap::HeapProcess);
		
		// Iterate over all pools (16 lower bit of pointer are index into pool).
		for( INT i=0; i<0xFFFF; i++ )
		{
			FAllocInfo* AllocInfo = AllocationPool[i];
			while( AllocInfo )
			{
				// Don't report allocations that occured while dumping allocations like e.g. the bazillion String Reallocs.
				if( AllocInfo->Flags & ALLOCATION_Regular )
				{
					Assert( AllocInfo->Size );

					TotalSize += AllocInfo->Size;

					// Create unique identifier based on call stack.
					AllocInfo->CRC = MurmurHash( AllocInfo->BackTrace, sizeof(AllocInfo->BackTrace), 0 );

					FGroupedAllocationInfo* GroupedInfo = GroupedAllocations.Find( AllocInfo->CRC );
					if( !GroupedInfo )
					{
						DWORD	Offset		= 0;
						DWORD64	Offset64	= 0;
						//String Callstack;

						// Retrieve human readable callstack information.
						for( INT Depth=0; Depth<MAX_BACKTRACE_DEPTH; Depth++ )
						{
							BOOL	ValidFromAddr	= SymFromAddr( Process, AllocInfo->BackTrace[Depth], &Offset64, Symbol )		? 1 : 0;
							DWORD	ErrorFromAddr	= GetLastError();
							BOOL	ValidLine		= SymGetLineFromAddr64( Process, AllocInfo->BackTrace[Depth], &Offset, &Line )	? 1 : 0;
							DWORD	ErrorLine		= GetLastError();

							if( ValidFromAddr && ValidLine )
							{
								log.Logf(LL_Info,"File: %s, symbol: %s\n",/*mxTO_ANSI*/(Line.FileName),/*mxTO_ANSI*/(Symbol->Name));

								// We can't use ANSI_TO_TCHAR without running out of stack.
								/*mxStrCpyNAnsi( FileName, Line.FileName, MAX_SYMBOL_NAME_LENGTH );
								mxStrCpyNAnsi( SymbolName, Symbol->Name, MAX_SYMBOL_NAME_LENGTH );*/

								//// Make sure we don't use ',' in symbol name so we don't throw off the comma separation of variables.
								//String SymbolNameString( SymbolName );
								//SymbolNameString = SymbolNameString.Replace( TEXT(","), TEXT(".") );

								//// We don't really care about the fully qualified path name as most files are unambigously named.
								//FFilename FileNameString( FileName );
						
								//// Append file, function and line number information.
								//Callstack += String::Printf(TEXT("%s::%i,"),*FileNameString.GetCleanFilename(),Line.LineNumber);							
							}
							else
							{
								//@todo: need to track down what causes this for non NULL addresses.
								/*DWORD Pointer = AllocInfo->BackTrace[Depth];
								Callstack += String::Printf(TEXT("%p,"),Pointer,ErrorFromAddr,ErrorLine);*/
							}
						}

						FGroupedAllocationInfo NewGroupedInfo;
						
						//mxStrCpyNAnsi( NewGroupedInfo.Callstack, *Callstack, ARRAY_COUNT(NewGroupedInfo.Callstack) );

						NewGroupedInfo.TotalSize	= AllocInfo->Size;
						NewGroupedInfo.Count		= 1;

						GroupedAllocations.Set( AllocInfo->CRC, NewGroupedInfo );
					}
					else
					{
						GroupedInfo->TotalSize += AllocInfo->Size;
						GroupedInfo->Count++;
					}
				}

				AllocInfo = AllocInfo->Next;
			}
		}

		//String Output;
		//
		//// Iterate over all allocations outputting a csv of size, count and callstack.
		//for( TMap<DWORD,FGroupedAllocationInfo>::TIterator It(GroupedAllocations); It; ++It )
		//{
		//	FGroupedAllocationInfo	GroupedInfo = It.Value();
		//	Output += String::Printf(TEXT("%i,%i,%s\n"), GroupedInfo.TotalSize, GroupedInfo.Count, GroupedInfo.Callstack);
		//}

		//// Write out comma separated variables file to logs directory.
		//appSaveStringToFile( Output, *(appGameDir() * TEXT("Logs") * TEXT("memory.csv")) );

		//debugf(TEXT(""));
		//debugf(TEXT("Total size of tracked allocations %.3fM"), TotalSize / 1024.f / 1024.f );
		//debugf(TEXT(""));

		// Clean up temporary ANSI/ UNICODE helper.
		//appSystemFree( FileName );
		//appSystemFree( SymbolName );

		CurrentlyDumpingAllocs = 0;
	}
	void ValidateHeap() override
	{
		UsedMalloc->ValidateHeap();
	}
	void Initialize()
	{
		MemZero( AllocationPool, sizeof(AllocationPool) );

		// Initialize variables for stack walking.
		Process							= GetCurrentProcess();
		Thread							= GetCurrentThread();
		SymOptions						= SymGetOptions();
		Symbol							= (SYMBOL_INFO*) appSystemMalloc( sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LENGTH );
	
		// Set symbol options.
		SymOptions						|= SYMOPT_LOAD_LINES;
		SymOptions						|= SYMOPT_UNDNAME;
		SymOptions						|= SYMOPT_EXACT_SYMBOLS;
		SymSetOptions( SymOptions );

		// Initialize symbol.
		memset( Symbol, 0, sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LENGTH );
		Symbol->SizeOfStruct			= sizeof(SYMBOL_INFO);
		Symbol->MaxNameLen				= MAX_SYMBOL_NAME_LENGTH;

		// Initialize line number info.
		memset( &Line, 0, sizeof(Line) );
		Line.SizeOfStruct				= sizeof(Line);

		// Initialize the symbol handler for a process.
		mxVERIFY( SymInitialize( Process, NULL, TRUE ) );

		CurrentlyDumpingAllocs = 0;

		UsedMalloc->Initialize();
	}

#if 0
	void DumpMemoryImage()
	{
		UsedMalloc->DumpMemoryImage();
	}
#endif
	void Shutdown() override
	{
		SymCleanup( Process );

		if( Symbol )
		{
			appSystemFree( Symbol );
			Symbol = nil;
		}

		if( UsedMalloc )
		{
			UsedMalloc->Shutdown();
			UsedMalloc = nil;
		}		
	}

	SizeT SizeOf( const void* ptr ) const override
	{
		return UsedMalloc->SizeOf( ptr );
	}

public:
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

