/*
=============================================================================
	File:	BitSet32.h
	Desc:
=============================================================================
*/

#ifndef __MX_BIT_SET_32_H__
#define __MX_BIT_SET_32_H__

#if !MX_PLATFORM_WIN32 || !MX_CPU_X86
#	error Unsupported platform!
#endif


mxNAMESPACE_BEGIN

struct BitSet32
{
	U4		v;

public:
	FORCEINLINE BitSet32()
	{}

	FORCEINLINE BitSet32( U4 i )
		: v( i )
	{}

	// Returns (in the lowest bit position) the bit at the given index.
	FORCEINLINE int	get( int index )
	{
		return v & (1 << index);
		//_bittest( v, index );
	}

	// Set the bit at the given index to 1.
	FORCEINLINE void set( int index )
	{
		v |= (1 << index);
	}

	// Clear the bit at the given index to 0.
	FORCEINLINE void clear( int index )
	{
		v &= ~(1 << index);
	}

	// Flips the bit at the given index.
	FORCEINLINE void flip( int index )
	{
		v ^= (1 << index);
	}

	// Returns the index of the first set bit.
	FORCEINLINE unsigned firstOneBit()
	{
		DWORD index;
		// Search the mask data from least significant bit (LSB) to the most significant bit (MSB) for a set bit (1).
		_BitScanForward( &index, v );
		return index;
	}
	
	// Returns the index of the first zero bit. Slow.
	FORCEINLINE unsigned firstZeroBit()
	{
		DWORD inv = ~this->v;
		DWORD index;
		_BitScanForward( &index, inv );
		return index;
	}

	FORCEINLINE void setAll( int value )
	{
		v = value;
	}
	
	FORCEINLINE void clearAll()
	{
		v = 0;
	}

	FORCEINLINE UINT Capacity() const
	{
		return BYTES_TO_BITS(sizeof(*this));
	}
};

mxNAMESPACE_END

#endif // ! __MX_BIT_SET_32_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
