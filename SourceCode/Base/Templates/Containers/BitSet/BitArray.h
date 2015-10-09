/*
=============================================================================
	File:	BitArray.h
	Desc:	Swiped from ICE (Pierre Terdiman)
=============================================================================
*/

#ifndef __MX_BIT_ARRAY_H__
#define __MX_BIT_ARRAY_H__
mxSWIPED("ICE");
mxNAMESPACE_BEGIN

//
//	BitArray - should be used instead of an array of bools.
//
class BitArray
{
public:
			BitArray();
			BitArray( UINT numBits );
			~BitArray();

	bool	Init( UINT numBits );

	// Data management
	
	FORCEINLINE	void	SetBit( UINT iBit )
	{
		// the same as
		// mBits[i / 32] |= (1 << (i % 32));
		mBits[iBit>>5] |= 1<<(iBit&31);
	}

	FORCEINLINE	void	ClearBit( UINT iBit )
	{
		// the same as
		// mBits[i / 32] &= ~(1 << (i % 32));
		mBits[iBit>>5] &= ~(1<<(iBit&31));
	}
	
	FORCEINLINE	void	ToggleBit( UINT iBit )
	{
		mBits[iBit>>5] ^= 1<<(iBit&31);
	}

	FORCEINLINE	void	ClearAll()
	{
		ZeroMemory( mBits, mSize*4 );
	}
	FORCEINLINE	void	SetAll()
	{
		FillMemory( mBits, mSize*4, 0xff );
	}

	// Data access

	FORCEINLINE	BOOL	IsSet( UINT iBit ) const
	{
		return mBits[iBit>>5] & (1<<(iBit&31));
	}

	FORCEINLINE	const UINT *	GetBits() const	{ return mBits;	}
	FORCEINLINE	UINT			GetSize() const	{ return mSize;	}

protected:
	UINT *		mBits;		//!< Array of bits
	UINT		mSize;		//!< Size of the array in dwords
};

// - We consider square symmetric N*N matrices
// - A N*N symmetric matrix has N(N+1)/2 elements
// - A boolean version needs N(N+1)/16 bytes
//		N		NbBits	NbBytes
//		4		10		-
//		8		36		4.5
//		16		136		17		<= the one we select
//		32		528		66
static const UBYTE BitMasks[]		= { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static const UBYTE NegBitMasks[]	= { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

//
//	BoolSquareSymmetricMatrix16
//
struct BoolSquareSymmetricMatrix16
{
	FORCEINLINE	UINT	Index( UINT x, UINT y )	const	{ if(x>y) TSwap(x,y);	return x + (y ? ((y-1)*y)>>1 : 0);			}

	FORCEINLINE	void	Set( UINT x, UINT y )			{ UINT i = Index(x, y);	mBits[i>>3] |= BitMasks[i&7];				}
	FORCEINLINE	void	Clear( UINT x, UINT y )			{ UINT i = Index(x, y);	mBits[i>>3] &= NegBitMasks[i&7];			}
	FORCEINLINE	void	Toggle( UINT x, UINT y )		{ UINT i = Index(x, y);	mBits[i>>3] ^= BitMasks[i&7];				}
	FORCEINLINE	bool	IsSet( UINT x, UINT y )	const	{ UINT i = Index(x, y);	return (mBits[i>>3] & BitMasks[i&7])!=0;	}

	FORCEINLINE	void	ClearAll()						{ ZeroMemory(mBits, 17);		}
	FORCEINLINE	void	SetAll()						{ FillMemory(mBits, 17, 0xff);	}

	UBYTE	mBits[17];
};

mxNAMESPACE_END

#endif // ! __MX_BIT_ARRAY_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
