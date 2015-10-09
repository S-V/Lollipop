/*
=============================================================================
	File:	Float16.h
	Desc:	16-bit floating point types.
=============================================================================
*/

#ifndef __MX_FLOAT_16_H__
#define __MX_FLOAT_16_H__

mxNAMESPACE_BEGIN
mxSWIPED("Havok")
//
//	mxFloat16 - A wrapper to store a float in 16 bits.
//
// This is a non IEEE representation.
// Basically we simply chop off the last 16 bits. That means the whole floating point range
// will be supported, but only with 7 bit precision.
//
struct mxFloat16
{
	mxInt16		value;

public:
	FORCEINLINE mxFloat16()
	{}

	FORCEINLINE mxFloat16( const mxFloat32& f )
	{
		mxInt32 t = ((const mxInt32*)&f)[0];
		value = mxInt16( t >> 16 );
	}

	FORCEINLINE mxFloat16 & operator = ( const mxFloat32& f )
	{
		mxInt32 t = ((const mxInt32*)&f)[0];
		value = mxInt16( t >> 16 );
		return *this;
	}

	FORCEINLINE operator mxFloat32() const
	{
		union
		{
			mxInt32 i;
			mxFloat32 f;
		} u;

		u.i = (value << 16);

		return u.f;
	}
};

mxSWIPED("WildMagic5.0")
//----------------------------------------------------------------------------
// A 16-bit floating-point number has a 1-bit sign (S), a 5-bit
// exponent (E), and a 10-bit mantissa (M).  The value of a 16-bit
// floating-point number is determined by the following:
//    (-1)^S * 0.0,                        if E == 0 and M == 0,
//    (-1)^S * 2^-14 * (M / 2^10),         if E == 0 and M != 0,
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if 0 < E < 31,
//    (-1)^S * INF,                        if E == 31 and M == 0, or
//    NaN,                                 if E == 31 and M != 0,
// where
//    S = floor((N mod 65536) / 32768),
//    E = floor((N mod 32768) / 1024), and
//    M = N mod 1024.
// Implementations are also allowed to use any of the following
// alternative encodings:
//    (-1)^S * 0.0,                        if E == 0 and M != 0,
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if E == 31 and M == 0, or
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if E == 31 and M != 0,
//
// This table explains the conversions in ToHalf and ToFloat.
// ---------------------------
// bias16  exp32  bias32
// ---------------------------
//     0    -127      0 (0x00)
//     :       :      :
//     0     -15    112 (0x70)
// ---------------------------
//     1     -14    113 (0x71)
//     :       :      :
//    15       0    127 (0x7F)
//     :       :      :
//    30      15    142 (0x8E)
// ---------------------------
//    31      16    143 (0x8F)
//     :       :      :
//    31     128    255 (0xFF)
// ---------------------------
//----------------------------------------------------------------------------

typedef UINT16 HalfFloat;

inline HalfFloat mxFloatToHalf( FLOAT value )
{
    UINT bits = *(UINT*)&value;
    UINT16 biasExp = (UINT16)((bits & 0x7F800000) >> 23);
    if (biasExp >= 0x0071)
    {
        if (biasExp <= 0x008E)
        {
            if (biasExp != 0)
            {
                // Truncate 23-bit mantissa to 10 bits.
                UINT16 signBit =
                    (UINT16)((bits & 0x80000000) >> 16);
                UINT mantissa = (bits & 0x007FFFFF) >> 13;
                biasExp = (biasExp - 0x0070) << 10;
                return (HalfFloat)(signBit | biasExp | mantissa);
            }
            else
            {
                // E = 0 (alternate encoding of zero, M does not matter)
                return (UINT16)0;
            }
        }
        else
        {
            // E = 30, M = 1023 (largest magnitude half-FLOAT)
            UINT16 signBit =
                (UINT16)((bits & 0x80000000) >> 16);
            return signBit | (UINT16)0x7BFF;
        }
    }
    else
    {
        // E = 1, M = 0 (smallest magnitude half-FLOAT)
        UINT16 signBit = (UINT16)((bits & 0x80000000) >> 16);
        return signBit | (UINT16)0x0400;
    }
}

inline FLOAT mxHalfToFloat( HalfFloat value )
{
    UINT biasExp  = (UINT)(value & 0x7C00) >> 10;
    if (biasExp != 0)
    {
        UINT signBit  = (UINT)(value & 0x8000) << 16;
        UINT mantissa = (UINT)(value & 0x03FF) << 13;
        biasExp = (biasExp + 0x0070) << 23;
        UINT result = signBit | biasExp | mantissa;
        return *(FLOAT*)&result;
    }
    else
    {
        // E = 0 (alternate encoding of zero, M does not matter)
        return 0.0f;
    }
}


/*
//
//	FloatToByte - this is evil (due to LHS issues, etc)
//
FORCEINLINE UBYTE FloatToByte( float x )
{
	union {
		FLOAT f;
		UINT  i;
	} f2i;

	// shift float to have 8bit fraction at base of number
	f2i.f = x + 32768.0f;
	f2i.i &= 0x7FFFFF;

	// then read as integer and kill float bits...
	return ( UBYTE )MinInt( f2i.i, 255 );
}
*/

mxNAMESPACE_END

#endif /* !__MX_FLOAT_16_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
