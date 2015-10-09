/*
=============================================================================
	File:	StackAlloc.h
	Desc:	Stack-based memory allocator.
=============================================================================
*/

#ifndef __MX_MEMORY_STACK_ALLOC_H__
#define __MX_MEMORY_STACK_ALLOC_H__

mxNAMESPACE_BEGIN

mxSWIPED("lifted and modified from the Hammer engine")

class mxStackAllocator
{
public:
	mxStackAllocator();

	bool Initialize( BYTE* pMemory, UINT sizeBytes );

	// doesn't free the memory
	void Shutdown();

	void* Alloc( UINT sizeBytes );
	void* AllocAligned( UINT sizeBytes, UINT alignment = EFFICIENT_ALIGNMENT );

	UINT GetMarker() const;
	void FreeTo( UINT marker );
	void Reset();

	BYTE* GetBufferPtr() { return mData; }

	template< typename TYPE >
	inline TYPE* AllocateObjects( UINT numObjects, UINT alignment = EFFICIENT_ALIGNMENT )
	{
		return (TYPE*) AllocAligned( numObjects * sizeof(TYPE), alignment );
	}

private:
	BYTE *	mData;
	UINT	mMarker;
	UINT	mCapacity;
};

inline
mxStackAllocator::mxStackAllocator()
{
	mMarker = 0;
	mCapacity = 0;
	mData = nil;
}
inline
bool mxStackAllocator::Initialize( BYTE* pMemory, UINT sizeBytes )
{
	mData = pMemory;
	if( !mData ) {
		return false;
	}

	mMarker = 0;
	mCapacity = sizeBytes;

	return true;
}
inline
void mxStackAllocator::Shutdown()
{
	mData = nil;
	mMarker = 0;
	mCapacity = 0;
}
inline
void* mxStackAllocator::AllocAligned( UINT sizeBytes, UINT alignment )
{
	AssertPtr(mData);
	Assert(sizeBytes > 0);
	Assert(IsValidAlignment(alignment));

	/* implementation from Gregory's "Game Engine Architecture" 5.2.1.3 */

	const UINT expandSizeBytes = sizeBytes + alignment;
	UINT* rawAddress = c_cast(UINT*) Alloc( expandSizeBytes );
	if( rawAddress == 0 ) {
		return nil;
	}

	// calculate the adjustment by masking off the lower bits
	// then use the difference to calculate the 'misalignment'

	const UINT misalign = c_cast(UINT)rawAddress & (alignment - 1);
	const UINT adjustment = alignment - misalign;

	 // now calculate the adjusted address and return it as the actual address

	UINT* alignedAddress = rawAddress + adjustment;
	return alignedAddress;
}
inline
void* mxStackAllocator::Alloc( UINT sizeBytes )
{
	AssertPtr(mData);
	Assert(sizeBytes > 0);

	// check if there's enough space
	if( mMarker + sizeBytes > mCapacity ) {
		DEBUG_BREAK;
		return nil;
	}

	void* ptr = mData + mMarker;
	mMarker += sizeBytes;

	return ptr;
}
inline
UINT mxStackAllocator::GetMarker() const
{
	return mMarker;
}
inline
void mxStackAllocator::FreeTo( UINT marker )
{
	Assert( marker < mCapacity );
	// simply reset the pointer to the old position
	mMarker = marker;
}
inline
void mxStackAllocator::Reset()
{
	mMarker = 0;
}

//------------------------------------------------------------------------------------------------------------------------

// frees the memory when leaves scope
//
class mxScopedStackAlloc
{
	mxStackAllocator &	allocator;
	UINT	marker;

public:
	inline mxScopedStackAlloc( mxStackAllocator & alloc )
		: allocator( alloc )
		, marker( alloc.GetMarker() )
	{}
	inline ~mxScopedStackAlloc()
	{
		allocator.FreeTo( marker );
	}	

	inline void* Allocate( UINT sizeBytes )
	{
		return allocator.Alloc( sizeBytes );
	}
	inline void* AllocateAligned( UINT sizeBytes, UINT alignment = EFFICIENT_ALIGNMENT )
	{
		return allocator.AllocAligned( sizeBytes, alignment );
	}

	template< typename TYPE >
	inline TYPE* AllocateObjects( UINT numObjects, UINT alignment = EFFICIENT_ALIGNMENT )
	{
		return (TYPE*) AllocateAligned( numObjects * sizeof(TYPE), alignment );
	}
};


mxNAMESPACE_END

#endif /* ! __MX_MEMORY_STACK_ALLOC_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
