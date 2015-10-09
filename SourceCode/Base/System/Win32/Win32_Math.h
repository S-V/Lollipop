/*
=============================================================================
	File:	Win32_Math.h
	Desc:
=============================================================================
*/
#pragma once
mxNAMESPACE_BEGIN

/*
=======================================================================

		Functions.

=======================================================================
*/

/*
In 32-bit environment Microsoft Visual C++ compiler emits a function call (_allmul)
when it comes to multiplying two long (64-bit) integer numbers .

See:

http://www.cbloom.com/oldrants8.html (12-05-08 | 64 Bit Multiply)
http://drdobbs.com/184416623 (64-Bit Integers and Floating Points)
http://software.intel.com/ru-ru/blogs/2009/04/21/int64/
*/
//
// Define operations to logically shift an int64 by 0..31 bits and to multiply
// 32-bits by 32-bits to form a 64-bit product.
//
// IA64 and AMD64 have native 64-bit operations that are just as fast as their
// 32-bit counter parts. Therefore, the int64 data type is used directly to form
// shifts of 0..31 and multiplies of 32-bits times 32-bits to form a 64-bit
// product.
//
#if 0	// these are defined in WinNT.h

#define Int32x32To64(a, b)  (((INT64)((long)(a))) * ((INT64)((long)(b))))
#define UInt32x32To64(a, b) (((UINT64)((UINT32)(a))) * ((UINT64)((UINT32)(b))))

#define Int64ShllMod32(a, b) (((UINT64)(a)) << (b))
#define Int64ShraMod32(a, b) (((INT64)(a)) >> (b))
#define Int64ShrlMod32(a, b) (((UINT64)(a)) >> (b))

#endif


FORCEINLINE FLOAT mxFabs( FLOAT x )
{
#if 0
	INT tmp = *(INT*) &x;
	tmp &= 0x7FFFFFFF;	// Clears the sign bit.
	return *(FLOAT*) &tmp;
#else
	return ::fabsf( x );
#endif
}

FORCEINLINE FLOAT mxCos( FLOAT x )
{
	return ::cosf( x );
}

FORCEINLINE FLOAT mxSin( FLOAT x )
{
	return ::sinf( x );
}

FORCEINLINE void mxSinCos( FLOAT x, FLOAT &s, FLOAT &c )
{
#if MX_USE_ASM
	asmSinCos( x, s, c );
#else
	s = ::sinf( x );
	c = ::cosf( x );
#endif
}

FORCEINLINE FLOAT mxTan( FLOAT x )
{
	return ::tanf( x );
}

FORCEINLINE FLOAT mxACos( FLOAT x )
{
	return ::acosf( x );
}

FORCEINLINE FLOAT mxASin( FLOAT x )
{
	return ::asinf( x );
}

FORCEINLINE FLOAT mxATan( FLOAT x )
{
	return ::atanf(x);
}

FORCEINLINE FLOAT mxATan2( FLOAT y, FLOAT x )
{
	return ::atan2f( y, x );
}

FORCEINLINE FLOAT mxExp( FLOAT x )
{
	return ::expf( x );
}

FORCEINLINE FLOAT mxLog( FLOAT x )
{
	return ::logf( x );
}

FORCEINLINE FLOAT mxLog10( FLOAT x )
{
	return ::log10f( x );
}

FORCEINLINE FLOAT mxPow( FLOAT x, FLOAT y )
{
	return ::powf( x, y );
}

FORCEINLINE FLOAT mxFmod( FLOAT x,FLOAT y )
{
	return ::fmodf(x,y);
}

static FORCEINLINE int float32_to_int32( const float value )
{
	union { int asInt[2]; double asDouble; } n;
	n.asDouble = value + 6755399441055744.0;

#if MX_BIG_ENDIAN
	return n.asInt [1];
#else
	return n.asInt [0];
#endif
}

mxSWIPED("C4");
//
// Count the number of leading zeros in the given 32-bit number.
//
FORCEINLINE UINT32 Cntlz32( UINT32 n )
{
	#ifdef MX_COMPILER_MSVC

		UINT32 x;

		C_ASSERT( sizeof(x) == sizeof(DWORD) );

		// _BitScanReverse - Searches the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).

		if( ::_BitScanReverse( (DWORD*)&x, n ) == 0 ) {
			return (32);
		}
		return (31 - x);

	#elif defined(MX_COMPILER_GCC)

			asm
			{
						bsr		ecx,n
						je		zero
						mov		eax,31
						sub		eax,ecx
						leave
						ret
				zero:	mov		eax,32
			}

	#else

		static const UINT8 mxALIGN_32 data[32] =
		{
			0x1F, 0x09, 0x1E, 0x03, 0x08, 0x12, 0x1D, 0x02,
			0x05, 0x07, 0x0E, 0x0C, 0x11, 0x16, 0x1C, 0x01,
			0x0A, 0x04, 0x13, 0x06, 0x0F, 0x0D, 0x17, 0x0B,
			0x14, 0x10, 0x18, 0x15, 0x19, 0x1A, 0x1B, 0x00
		};

		UINT32 x = ~(n | -(long) n) >> 31;
		UINT32 m = n | (n >> 1);
		m |= m >> 2;
		m |= m >> 4;
		m |= m >> 8;
		m |= m >> 16;

		return (data[(m * 0x07DCD629UL) >> 27] + x);

	#endif
}



FORCEINLINE int mxFindLowestSetBit(UINT32 v) {
	unsigned long index;
	return _BitScanForward(&index, v) ? index : 32;
}
FORCEINLINE int mxFindHighestSetBit(UINT32 v) {
	unsigned long index;
	return _BitScanReverse(&index, v) ? index : -1;
}
FORCEINLINE int mxFindLowestSetBitFast(UINT32 v) {
	unsigned long index;
	_BitScanForward(&index, v);
	return index;
}
FORCEINLINE int mxFindHighestSetBitFast(UINT32 v) {
	unsigned long index;
	_BitScanReverse(&index, v);
	return index;
}



mxSWIPED("CryEngine3");
class ScopedSetFloatExceptionMask
{
	unsigned oldMask;
public:
	ScopedSetFloatExceptionMask(unsigned int disable = _EM_INEXACT | _EM_UNDERFLOW | _EM_OVERFLOW | _EM_DENORMAL |_EM_INVALID)
	{
		_clearfp();
		_controlfp_s(&oldMask, 0, 0);
		unsigned temp;
		_controlfp_s(&temp, disable, _MCW_EM);
	}
	~ScopedSetFloatExceptionMask()
	{
		_clearfp();
		unsigned temp;
		_controlfp_s(&temp, oldMask, _MCW_EM);
	}
};

#define SCOPED_ENABLE_FLOAT_EXCEPTIONS ScopedSetFloatExceptionMask scopedSetFloatExceptionMask(0)
#define SCOPED_DISABLE_FLOAT_EXCEPTIONS ScopedSetFloatExceptionMask scopedSetFloatExceptionMask

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
