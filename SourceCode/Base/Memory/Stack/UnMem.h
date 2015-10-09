/*=============================================================================
	UnMem.h: FMemStack class, ultra-fast temporary memory allocation
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

mxSWIPED("Unreal Engine 3");

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Enums for specifying memory allocation type.
enum EMemZeroed {MEM_Zeroed=1};
enum EMemOned   {MEM_Oned  =1};

/*-----------------------------------------------------------------------------
	FMemStack.
-----------------------------------------------------------------------------*/

//
// Simple linear-allocation memory stack.
// Items are allocated via PushBytes() or the specialized operator new()s.
// Items are freed en masse by using FMemMark to Pop() them.
//
class FMemStack
{
public:
	// Get bytes.
	BYTE* PushBytes( INT AllocSize, INT Align = DEFAULT_MEMORY_ALIGNMENT )
	{
		// Debug checks.
		Assert(AllocSize>=0);
		Assert((Align&(Align-1))==0);
		Assert(Top<=End);

		// Try to get memory from the current chunk.
		BYTE* Result = (BYTE *)(((INT)Top+(Align-1))&~(Align-1));
		Top = Result + AllocSize;

		// Make sure we didn't overflow.
		if( Top > End )
		{
			// We'd pass the end of the current chunk, so allocate a new one.
			AllocateNewChunk( AllocSize + Align );
			Result = (BYTE *)(((INT)Top+(Align-1))&~(Align-1));
			Top    = Result + AllocSize;
		}
		return Result;
	}

	// Main functions.
	void Init( HMemory heap, INT DefaultChunkSize );
	void Exit();
	void Tick();
	INT GetByteCount();

	// Friends.
	friend class FMemMark;
	friend void* operator new( size_t Size, FMemStack& Mem, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT );
	friend void* operator new( size_t Size, FMemStack& Mem, EMemZeroed Tag, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT );
	friend void* operator new( size_t Size, FMemStack& Mem, EMemOned Tag, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT );

	// Types.
	struct FTaggedMemory
	{
		FTaggedMemory* Next;
		INT DataSize;
		BYTE Data[1];
	};

private:
	// Constants.
	enum {MAX_CHUNKS=1024};

	// Variables.
	BYTE*			Top;				// Top of current chunk (Top<=End).
	BYTE*			End;				// End of current chunk.
	INT				DefaultChunkSize;	// Maximum chunk size to allocate.
	FTaggedMemory*	TopChunk;			// Only chunks 0..ActiveChunks-1 are valid.
	HMemory			memHeap;

	// Static.
	static FTaggedMemory* UnusedChunks;

	// Functions.
	BYTE* AllocateNewChunk( INT MinSize );
	void FreeChunks( FTaggedMemory* NewTopChunk );
};

/*-----------------------------------------------------------------------------
	FMemStack templates.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
template <class T> inline T* New( FMemStack& Mem, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT )
{
	return (T*)Mem.PushBytes( Count*sizeof(T), Align );
}
template <class T> inline T* NewZeroed( FMemStack& Mem, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT )
{
	BYTE* Result = Mem.PushBytes( Count*sizeof(T), Align );
	MemZero( Result, Count*sizeof(T) );
	return (T*)Result;
}
template <class T> inline T* NewOned( FMemStack& Mem, INT Count=1, INT Align=DEFAULT_MEMORY_ALIGNMENT )
{
	BYTE* Result = Mem.PushBytes( Count*sizeof(T), Align );
#if MX_DEBUG
	MemSet( Result, 0xff, Count*sizeof(T) );
#endif // MX_DEBUG
	return (T*)Result;
}

/*-----------------------------------------------------------------------------
	FMemStack operator new's.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
inline void* operator new( size_t Size, FMemStack& Mem, INT Count, INT Align )
{
	// Get uninitialized memory.
	return Mem.PushBytes( Size*Count, Align );
}
inline void* operator new( size_t Size, FMemStack& Mem, EMemZeroed Tag, INT Count, INT Align )
{
	// Get zero-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	MemZero( Result, Size*Count );
	return Result;
}
inline void* operator new( size_t Size, FMemStack& Mem, EMemOned Tag, INT Count, INT Align )
{
	// Get one-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	MemSet( Result, 0xff, Size*Count );
	return Result;
}

/*-----------------------------------------------------------------------------
	FMemMark.
-----------------------------------------------------------------------------*/

//
// FMemMark marks a top-of-stack position in the memory stack.
// When the marker is constructed or initialized with a particular memory 
// stack, it saves the stack's current position. When marker is popped, it
// pops all items that were added to the stack subsequent to initialization.
//
class FMemMark
{
public:
	// Constructors.
	FMemMark()
	{}
	FMemMark( FMemStack& InMem )
	{
		this->Record( InMem );
	}
	~FMemMark()
	{
		this->Pop();
	}
	void Record( FMemStack& InMem )
	{
		Mem          = &InMem;
		Top          = Mem->Top;
		SavedChunk   = Mem->TopChunk;
	}
	// FMemMark interface.
	void Pop()
	{
		// Unlock any new chunks that were allocated.
		if( SavedChunk != Mem->TopChunk )
		{
			Mem->FreeChunks( SavedChunk );
		}
		// Restore the memory stack's state.
		Mem->Top = Top;
	}

private:
	// Implementation variables.
	FMemStack* Mem;
	BYTE* Top;
	FMemStack::FTaggedMemory* SavedChunk;
};

class ScopedMemMark : public FMemMark
{
public:
	ScopedMemMark( FMemStack& InMem )
		: FMemMark( InMem )
	{
	}
	~ScopedMemMark()
	{
		FMemMark::Pop();
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

