/*
=============================================================================
	File:	BitSetN.h
	Desc:
=============================================================================
*/

#ifndef __MX_BIT_SET_N_H__
#define __MX_BIT_SET_N_H__

#if !MX_PLATFORM_WIN32 || !MX_CPU_X86
#	error Unsupported platform!
#endif


mxNAMESPACE_BEGIN

template< SizeT NUMBITS >
struct BitSetN
{
	static const SizeT size = ((NUMBITS + 31) / 32);

	U32	mBits[ size ];

	enum : U32 { BITS = BYTES_TO_BITS(sizeof(bits[0])) };

public:
	FORCEINLINE BitSetN()
	{}

	// Returns (in the lowest bit position) the bit at the given index.
	FORCEINLINE int	get( int iBit )
	{
		return mBits[ iBit >> 5 ] & (1 << (iBit & 31));
	}

	// Set the bit at the given index to 1.
	FORCEINLINE void set( int iBit )
	{
		mBits[iBit>>5] |= 1<<(iBit&31);
	}

	// Clear the bit at the given index to 0.
	FORCEINLINE void clear( int iBit )
	{
		mBits[iBit>>5] &= ~(1<<(iBit&31));
	}

	// Flips the bit at the given index.
	FORCEINLINE void flip( int iBit )
	{
		mBits[iBit>>5] ^= 1<<(iBit&31);
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
		mBits = value;
	}
	
	FORCEINLINE void clearAll()
	{
		mBits = 0;
	}

	FORCEINLINE UINT Capacity() const
	{
		return BYTES_TO_BITS(sizeof(*this));
	}
};

mxNAMESPACE_END

#endif // ! __MX_BIT_SET_N_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
