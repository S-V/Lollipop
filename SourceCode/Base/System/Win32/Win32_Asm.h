/*
=============================================================================
	File:	Win32_Asm.h
	Desc:
=============================================================================
*/
#pragma once
//---------------------------------------------------------------
//		Defines.
//---------------------------------------------------------------

#define ASM _asm

//---------------------------------------------------------------

mxNAMESPACE_BEGIN

//
//	Returns true if the CPUID command is available on the processor.
//
FORCEINLINE bool Is_CPUID_available()
{
	int bitChanged = 0;

    // We have to check if we can toggle the flag register bit 21.
    // If we can't the processor does not support the CPUID command.

    ASM
	{
        push	EAX
        push	EBX
        pushfd
        pushfd
        pop		EAX
        mov		EBX, EAX
        xor		EAX, 0x00200000 
        push	EAX
        popfd
        pushfd
        pop		EAX
        popfd
        xor		EAX, EBX 
        mov		bitChanged, EAX
        pop		EBX
        pop		EAX
    }

    return ( bitChanged != 0 );
}

enum CPUID_FUNCTION
{
	CPUID_VENDOR_ID              = 0x00000000,
	CPUID_PROCESSOR_FEATURES     = 0x00000001,
	CPUID_NUM_CORES              = 0x00000004,
	CPUID_GET_HIGHEST_FUNCTION   = 0x80000000,
	CPUID_EXTENDED_FEATURES      = 0x80000001,

	CPUID_BRAND_STRING_0         = 0x80000002,
	CPUID_BRAND_STRING_1         = 0x80000003,
	CPUID_BRAND_STRING_2         = 0x80000004,

	CPUID_CACHE_INFO             = 0x80000006,
};

struct CPUID_ARGS
{ 
	DWORD eax; 
	DWORD ebx; 
	DWORD ecx; 
	DWORD edx; 
}; 

/*
See http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration/
or  http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
for description of the arguments.
*/
FORCEINLINE void Do_CPUID( UINT32 func, UINT32 *a, UINT32 *b, UINT32 *c, UINT32 *d )
{
	ASM
	{
		push	EAX
		push	EBX
		push	ECX
		push	EDX

		mov		EAX, func
		cpuid
		mov		EDI, a
		mov		[EDI], EAX
		mov		EDI, b
		mov		[EDI], EBX
		mov		EDI, c
		mov		[EDI], ECX
		mov		EDI, d
		mov		[EDI], EDX

		pop		EDX
		pop		ECX
		pop		EBX
		pop		EAX
	}
}

FORCEINLINE UINT64 GetCPUCycles()
{
	ASM rdtsc	// read time stamp into EDX:EAX
}

UINT64 EstimateProcessorFrequencyHz();

#ifdef _CPPUNWIND
inline
bool asmOSSupportsSSE()
{
	__try {
		__asm xorps XMM0, XMM0
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		if( _exception_code() == STATUS_ILLEGAL_INSTRUCTION ) {
			return false;	// SSE not supported
		}
		return false;	// unexpected error
	}
	return true;
}
#endif

// Generates run-time exceptions when NaN is produced, on division by zero,
// and on floating point overflow
/*
inline void asmTrapFPE()
{
	//Set the x86 floating-point control word according to what
	//exceptions you want to trap. 
	::_clearfp(); //Always call _clearfp before setting the control word
	//Because the second parameter in the following call is 0, it
	//only returns the floating-point control word
	unsigned int cw = ::_controlfp( 0, 0 ); //Get the default control word
	//Set the exception masks off for exceptions that you want to
	//trap.  When a mask bit is set, the corresponding floating-point
	//exception is //blocked from being generating.
	cw &=~( EM_OVERFLOW|EM_ZERODIVIDE|EM_INVALID );
	//For any bit in the second parameter (mask) that is 1, the 
	//corresponding bit in the first parameter is used to update
	//the control word.
	unsigned int cwOriginal = ::_controlfp( cw, MCW_EM ); //Set it.
	(void)cwOriginal;
	//MCW_EM is defined in float.h.
	//Restore the original value when done:
	//_controlfp(cwOriginal, MCW_EM);

	unsigned long cntrReg;
	ASM
	{
		stmxcsr [cntrReg]        	//Get MXCSR register
		and [cntrReg], 0FFFFFF7Fh	//bit 7 - invalid instruction mask
		//bit 9 - divide-by-zero mask
		//bit 10 - overflow mask
		//bit 11 - underflow mask
		ldmxcsr [cntrReg]			//Load MXCSR register
	}
}
*/
//
//  asmLog2 - computes the logarithm of a value using basis 2
//
FORCEINLINE UINT asmLog2( UINT x )
{
	mxMATH_CHECK( x > 0 );
	ASM bsr	EAX, x
	// return value in EAX.
}

//
//  asmRound - Returns integer part of specified floating point value.
//
static FORCEINLINE INT asmRound( FLOAT x )
{
#if 1
	// rounds a floating point number to the nearest integer

	// CVTSS2SI - Converts the lower single-precision, floating-point value of a to a 32-bit integer according to the current rounding mode.
	// If a converted result is larger than the maximum signed doubleword integer,
	// the floating-point invalid exception is raised, and if this exception is masked,
	// the indefinite integer value (80000000H) is returned.
	/*
	__m128 v = _mm_load_ss( &x );
	return _mm_cvtss_si32( v );
	*/
	ASM cvtss2si EAX, [x]

#else
	// Quick rounding of float to integer.
	// 6 cycles or so. Can be used for quick float->int conversion (eg. for
	// positive numbers: int i = round( f - 0.5f )).
	// Note that rounding type is dependent on FPU state
	// so this routine should not be used when absolute accuracy is required.
	INT retval;
	ASM fld		x
	ASM fistp	retval
	return retval;	// use default rouding mode (round nearest)
#endif
}

// the same thing using intrinsics - rounds a floating point number to the nearest integer
static FORCEINLINE INT asmFloatToInt( FLOAT x )
{
	return _mm_cvt_ss2si( _mm_load_ss( &x ) );
}

//
// the same as mxTruncate()
//
static FORCEINLINE INT mxFloor( FLOAT x )
{
#if MX_USE_ASM
	// CVTTSS2SI - Converts the lower single-precision, floating-point value of a to a 32-bit integer with truncation.
	// If a converted result is larger than the maximum signed doubleword integer,
	// the floating-point invalid exception is raised, and if this exception is masked,
	// the indefinite integer value (80000000H) is returned.
	/*
	__m128 v = _mm_load_ss( &x );
	return _mm_cvttss_si32( v );
	*/
	ASM cvttss2si EAX, [x]
#else
	return (INT)::floorf( x );
#endif
}

//
//	mxTruncate - Converts the given floating-point value to an integer with truncation towards zero.
//
static FORCEINLINE INT mxTruncate( FLOAT x )
{
#if 0
	// CVTTSS2SI - Converts the lower single-precision, floating-point value of a to a 32-bit integer with truncation.
	// If a converted result is larger than the maximum signed doubleword integer,
	// the floating-point invalid exception is raised, and if this exception is masked,
	// the indefinite integer value (80000000H) is returned.
	ASM cvttss2si EAX, [x]
#else
	return _mm_cvtt_ss2si( _mm_load_ss( &x ) );	// SSE1
#endif
}

FORCEINLINE UINT8 asmF32toU8( FLOAT x )
{
	static const FLOAT SSE_FLOAT_ZERO = 0.0f;
	static const FLOAT SSE_FLOAT_255 = 255.0f;
	// If a converted result is negative the value (0) is returned and if the
	// converted result is larger than the maximum byte the value (255) is returned.
	UINT8	b;
	ASM movss		XMM0, x
	ASM maxss		XMM0, SSE_FLOAT_ZERO
	ASM minss		XMM0, SSE_FLOAT_255
	ASM cvttss2si	EAX, XMM0
	ASM mov			b, AL
	return b;
}

// Fast rounding, however last bit might be wrong.
static FORCEINLINE void asmF32toI32( FLOAT x, INT32 *p )
{
	ASM	mov		EAX, p
	ASM fld		x
	ASM fistp	dword ptr [EAX]
}

// Fast rounding, however last bit might be wrong.
static FORCEINLINE void asmF32toI64( FLOAT x, INT64 *p )
{
	ASM	mov		EAX, p
	ASM fld		x
	ASM fistp	qword ptr [EAX]
}

// double->int
static FORCEINLINE INT32 asmF64toI32( DOUBLE f )
{
	return _mm_cvttsd_si32( _mm_load_sd( &f ) );	// SSE2
}

//
// calculates 1 / x
//
FORCEINLINE FLOAT mxReciprocal( FLOAT x )
{
	mxMATH_CHECK( x != 0.0f );
#if MX_USE_ASM
	// SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
	// bits of the mantissa
	// One Newtown-Raphson Iteration:
	// f(i+1) = 2 * rcpss(x) - x * rcpss(x) * rcpss(x)
	FLOAT rec;
	ASM rcpss XMM0, x               // xmm0 = rcpss(x)
	ASM movss XMM1, x               // xmm1 = x
	ASM mulss XMM1, XMM0            // xmm1 = x * rcpss(x)
	ASM mulss XMM1, XMM0            // xmm2 = x * rcpss(x) * rcpss(x)
	ASM addss XMM0, XMM0            // xmm0 = 2 * rcpss(x)
	ASM subss XMM0, XMM1            // xmm0 = 2 * rcpss(x)
									//        - x * rcpss(x) * rcpss(x)
	ASM movss rec, XMM0             // return xmm0
	return rec;
#else
	return 1.0f / x;
#endif
}

FORCEINLINE FLOAT asmReciprocalEst( FLOAT x )
{
	mxMATH_CHECK( x != 0.0f );
#if MX_USE_ASM
	// SSE reciprocal estimate, accurate to 12 significant bits of the mantissa
	FLOAT rec;
	ASM rcpss XMM0, x	// xmm0 = rcpss(x)
	ASM movss rec, XMM0	// return xmm0
	return rec;
#else
	return 1.0f / x;
#endif
}

//
// Square root
//

static FORCEINLINE FLOAT mxSqrt( FLOAT x )
{
	mxMATH_CHECK( x >= 0.0f );	//<- this helped a lot to catch bugs
#if MX_USE_ASM
    ASM fld	x
    ASM fsqrt
#else
	return ::sqrtf( x );
#endif
}

//
// Square root estimate
//
static FORCEINLINE FLOAT mxSqrtEst( FLOAT f )
{
	mxMATH_CHECK( f >= 0.0f );
#if 1
	FLOAT temp;
	ASM {
		rsqrtss	XMM1, f		// 1/sqrt(f) estimate
		rcpss	XMM1, XMM1	// result = 1/estimate
		movss	temp, XMM1
	}
	return temp;
#else
	FLOAT retval;
	ASM {
		mov     EAX, f
		sub     EAX, 0x3F800000
		sar     EAX, 1
		add     EAX, 0x3F800000
		mov     [retval], EAX
	}
	return retval;
#endif
}

//
//	Rsq function calculates 1 / sqrtf ( x )
//
FORCEINLINE FLOAT mxInvSqrt( FLOAT x )
{
	mxMATH_CHECK( x > 0.0f );
#if MX_USE_ASM
#if 1
	// SSE Newton-Raphson reciprocal square root estimate, accurate to 23 significant
	// bits of the mantissa

	FLOAT result;
	const FLOAT fThree = 3.0f;
	const FLOAT fHalf = 0.5f;
	ASM
	{
		movss		XMM1, [x]
		rsqrtss		XMM0, XMM1
		movss		XMM3,[fThree]
		movss		XMM2, XMM0
		mulss		XMM0, XMM1
		mulss		XMM0, XMM2
		mulss		XMM2, [fHalf]
		subss		XMM3, XMM0
		mulss		XMM3, XMM2
		movss		[result], XMM3
	}
	return result;
#else
	ASM {
        fld1 // r0 = 1.f
        fld x // r1 = r0, r0 = x
        fsqrt // r0 = sqrtf( r0 )
        fdiv // r0 = r1 / r0
    } // returns r0
#endif

#else
	return 1.0f / ::sqrtf( x );
#endif
}

// SSE reciprocal square root estimate, accurate to 12 significant
// bits of the mantissa
//
static FORCEINLINE FLOAT mxInvSqrtEst( FLOAT x )
{
	mxMATH_CHECK( x > 0.0f );
#if MX_USE_ASM
	__m128 v = _mm_load_ss( &x );
	__m128 rsqrt = _mm_rsqrt_ss( v );
	_mm_store_ss( &x, rsqrt );
	return x;
#else
	return 1.0f / ::sqrtf( x );
#endif
}

// returns the maximum random number
FORCEINLINE FLOAT asmRandMax()
{
#if 1
	return 9223372036854775807.0f;
#else
	return float( RAND_MAX );
#endif
}
/*
// returns a random number
FORCEINLINE FLOAT asmRand()
{
	static unsigned __int64 q = time( NULL );

	ASM {
		movq MM0, q

		// do the magic MMX thing
		pshufw MM1, MM0, 0x1E
		paddd MM0, MM1

		// move to integer memory location and free MMX
		movq q, MM0
		emms
	}
	return FLOAT( q );
}
*/

/*
FORCEINLINE float asmMin( FLOAT x, FLOAT y )
{
	FLOAT result;
	ASM
	{
		fld		[x]
		fld		[y]
		fcomi	st(0), st(1)
		fcmovnb	st(0), st(1)
		fstp	[result]
		fcomp
	}
	return result;
}
*/

FORCEINLINE static NAKED
FLOAT __fastcall asmSin( FLOAT x )
{
	ASM {
		fld		DWORD PTR [esp+4] 
		fsin
		ret		4
	}
}

FORCEINLINE static NAKED
FLOAT __fastcall asmCos( FLOAT x )
{
	ASM {
		fld		DWORD PTR [esp+4] 
		fcos
		ret		4
	}
}

FORCEINLINE void asmSinCos( FLOAT x, FLOAT &sine, FLOAT &cosine )
{
	ASM {
		fld		x
		fsincos
		mov		ECX, cosine
		mov		EDX, sine
		fstp	dword ptr [ECX]
		fstp	dword ptr [EDX]
	}
}
FORCEINLINE void asmSinCos64( FLOAT x, DOUBLE &sine, DOUBLE &cosine )
{
	ASM {
		fld		x
		fsincos
		mov		ECX, cosine
		mov		EDX, sine
		fstp	qword ptr [ECX]
		fstp	qword ptr [EDX]
	}
}

INLINE mxSimdQuad SSEDot( mxSimdQuad va, mxSimdQuad vb )
{
	mxSimdQuad t0 = _mm_mul_ps( va, vb );
	mxSimdQuad t1 = _mm_shuffle_ps( t0, t0, _MM_SHUFFLE(1,0,3,2) );
	mxSimdQuad t2 = _mm_add_ps( t0, t1 );
	mxSimdQuad t3 = _mm_shuffle_ps( t2, t2, _MM_SHUFFLE(2,3,0,1) );
	mxSimdQuad dot = _mm_add_ps( t3, t2 );
	return (dot);
}

// SSE3
INLINE __m128 DotSSE3( const mxSimdQuad v0, const mxSimdQuad v1 )   
{
   __m128 v;
   v = _mm_mul_ps( v0, v1 );
   v = _mm_hadd_ps( v, v );
   v = _mm_hadd_ps( v, v );   
   return v;
}

mxSWIPED("Intel Colony Demo [2010]")
/*
	The Intel compiler solves the problem of code bloat
	caused by passing SIMD vectors by reference instead of by value (i.e. in registers).
*/
FORCEINLINE void SSENormalize( mxSimdQuad& fX, mxSimdQuad& fY )
{
    // Calculate the squares
    mxSimdQuad fSqX = _mm_mul_ps( fX, fX );
    mxSimdQuad fSqY = _mm_mul_ps( fY, fY );

    // Add them up for the lengths
    mxSimdQuad fLength = _mm_add_ps( fSqX, fSqY );

    // Get the sqrt
    fLength = _mm_sqrt_ps( fLength );

    // Then divide by it (normalize), and save as the new direction
    fX = _mm_div_ps( fX, fLength );
    fY = _mm_div_ps( fY, fLength );
}

mxSWIPED("Intel Colony Demo [2010]")
FORCEINLINE mxSimdQuad SSELengthSq( const mxSimdQuad& fX, const mxSimdQuad& fY )
{
    // Calculate the squares
    mxSimdQuad fSqX = _mm_mul_ps( fX, fX );
    mxSimdQuad fSqY = _mm_mul_ps( fY, fY );

    // Add them up for the lengths
    return _mm_add_ps( fSqX, fSqY );
}

mxSWIPED("Intel Colony Demo [2010]")
FORCEINLINE mxSimdQuad SSEDotProduct(
	const mxSimdQuad& fX1,
	const mxSimdQuad& fY1,
	const mxSimdQuad& fX2,
	const mxSimdQuad& fY2 )
{
    return _mm_add_ps( _mm_mul_ps( fX1, fX2 ), _mm_mul_ps( fY2, fY2 ) );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
