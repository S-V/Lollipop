/*
=============================================================================
	File:	Win32_Memory.h
	Desc:	Memory utilities.
=============================================================================
*/
#pragma once

mxNAMESPACE_BEGIN

// Size of the processor's cache line, in bytes.
#define CACHE_LINE_SIZE		64

// Operating system's page size.
enum { PAGE_SIZE = 4096 };

// Size of a (void*) pointer in bytes.
// SIZE_OF_POINTER
enum { SIZE_OF_VOID = sizeof(void*) };

FORCEINLINE UINT BitsToBytes( UINT numBits )
{
	return (numBits >> 3) + ((numBits & 7) ? 1 : 0);
}
FORCEINLINE UINT BitsToUInts( UINT numBits )
{
	return (numBits >> 5) + ((numBits & 31) ? 1 : 0);
}

//---------------------------------------------------------------
//		Data Alignment.
//---------------------------------------------------------------

// Minimum possible memory alignment, in bytes.
enum { MINIMUM_ALIGNMENT = 4 };

// Maximum possible memory alignment, in bytes.
enum { MAXIMUM_ALIGNMENT = 128 };

// Preferred data alignment.
enum { PREFERRED_ALIGNMENT = sizeof(UINT_PTR) };

// The most efficient memory alignment.
enum { EFFICIENT_ALIGNMENT = 16 };

// Memory allocations are aligned on this boundary by default.
enum { DEFAULT_MEMORY_ALIGNMENT = EFFICIENT_ALIGNMENT };

/*
	Usage of alignment macros.

	e.g. you need to allocate x bytes of memory and align them on 16-byte boundary:

	void * Alloc16( SizeT x ) {
		SizeT size = ALIGN16(x);

		// ... perform actual memory allocation and return a ptr to a new memory block
	}
*/

FORCEINLINE UINT RoundUpTo( UINT num, UINT value )
{
	return (num + (value-1)) % value;
}

template< typename TYPE >
FORCEINLINE TYPE RoundUpToPageSize( TYPE size )
{
	size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	return size;
}

// Rounds something up to be a multiple of the EFFICIENT_ALIGNMENT.
mxSWIPED("OpenDynamicsEngine")
#define MX_EFFICIENT_SIZE( x )		((((x)-1)|(EFFICIENT_ALIGNMENT-1))+1)

#define ALIGN2( len ) (( len+1)&~1) // round up to 16 bits
#define ALIGN4( len ) (( len+3)&~3) // round up to 32 bits
#define ALIGN8( len ) (( len+7)&~7) // round up to 64 bits
#define ALIGN16( len ) (( len + 15 ) & ~15 ) // round up to 128 bits

mxSWIPED("Valve Source Engine L4D SDK");
#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) //  need macro for constant expression

// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

//
//	TAlign< T > - returns an aligned pointer.
//
template< typename T >
FORCEINLINE
T TAlign( const T pointer, SizeT alignmentInBytes )
{
	mxASSERT(IsValidAlignment(alignmentInBytes));
	return (T) ( ((SizeT)pointer + alignmentInBytes - 1)
				& ~(alignmentInBytes - 1) );
}

template< typename T >
FORCEINLINE
T TAlignSimd( const T pointer )
{
	return (T) ( ((SizeT)pointer + EFFICIENT_ALIGNMENT - 1) & ~(EFFICIENT_ALIGNMENT - 1) );
}

template <typename T>
inline T AlignValue( T val, unsigned alignment )
{
	return (T)( ( (uintptr_t)val + alignment - 1 ) & ~( alignment - 1 ) );
}

mxSWIPED("git://android.git.kernel.org/platform/external/skia.git/gpu/");
/**
 *  divide, rounding up
 */
static inline UINT GrUIDivRoundUp(UINT x, UINT y) {
    return (x + (y-1)) / y;
}
static inline size_t GrSizeDivRoundUp(size_t x, UINT y) {
    return (x + (y-1)) / y;
}

/**
 *  align up
 */
static inline UINT GrUIAlignUp(UINT x, UINT alignment) {
    return GrUIDivRoundUp(x, alignment) * alignment;
}
static inline UINT GrSizeAlignUp(size_t x, UINT alignment) {
    return GrSizeDivRoundUp(x, alignment) * alignment;
}

/**
 * amount of pad needed to align up
 */
static inline UINT GrUIAlignUpPad(UINT x, UINT alignment) {
    return (alignment - x % alignment) % alignment;
}
static inline size_t GrSizeAlignUpPad(size_t x, UINT alignment) {
    return (alignment - x % alignment) % alignment;
}

/**
 *  align down
 */
static inline UINT GrUIAlignDown(UINT x, UINT alignment) {
    return (x / alignment) * alignment;
}
static inline UINT GrSizeAlignDown(size_t x, UINT alignment) {
    return (x / alignment) * alignment;
}


/*
Helpers to avoid bugs resulting from casting. See:
http://cbloomrants.blogspot.com/2011/06/06-17-11-c-casting-is-devil.html
*/
template< typename T >
T * ByteStepPointer( T* ptr, ptrdiff_t step )
{
    return (T*) ( ((intptr_t)ptr) + step );
}
template<typename T>
T * CastVoid(void * ptr)
{
    return (T *)( ptr );
}
template<typename T>
void * VoidCast(T * ptr)
{
    return (void *)( ptr );
}
// check_value_cast just does a static_cast and makes sure you didn't wreck the value
template <typename t_to, typename t_fm>
t_to check_cast( const t_fm & from )
{
    t_to to = static_cast<t_to>(from);
    ASSERT( static_cast<t_fm>(to) == from );
    return to;
}

FORCEINLINE UINT AlignToPowerOf2( UINT Value, UINT Alignment )
{
    mxASSERT((Alignment & (Alignment - 1)) == 0);
    // to align to 2^N, add 2^N - 1 and AND with all but lowest N bits set
	mxANALYSIS_ASSUME(Alignment > 0 && Value < MAX_UINT32 - Alignment);
    return (Value + Alignment - 1) & (~(Alignment - 1));
}
mxSWIPED("DirectX SDK")
FORCEINLINE void * AlignToPowerOf2( void *pValue, UINT_PTR Alignment )
{
    mxASSERT((Alignment & (Alignment - 1)) == 0);
    // to align to 2^N, add 2^N - 1 and AND with all but lowest N bits set
    return (void *)(((UINT_PTR)pValue + Alignment - 1) & (~((UINT_PTR)Alignment - 1)));
}

//---------------------------------------------------------------------------

//
//	IsValidAlignment
//
FORCEINLINE bool IsValidAlignment( SizeT alignmentInBytes )
{
	return (alignmentInBytes >= MINIMUM_ALIGNMENT)
		&& (alignmentInBytes <= MAXIMUM_ALIGNMENT)
		&& (alignmentInBytes & (alignmentInBytes - 1)) == 0// alignment must be a power of two
		;
}

// Forces prefetch of memory.
FORCEINLINE void mxTouchMemory( void const* ptr )
{
	(void)* ( char const volatile* ) ptr;
}

/*
#define MX_PREFETCH( ptr )				_mm_prefetch((char*)(ptr),_MM_HINT_T0);
#define MX_PREFETCH( ptr )				_mm_prefetch((const char*)ptr,_MM_HINT_NTA);
#define MX_PREFETCH_LOOP( num, ptr )	{ for(int prefetchLoop = 0; prefetchLoop < num; prefetchLoop+=64) {_mm_prefetch( (const char*)(ptr)+prefetchLoop,_MM_HINT_T0); } }
*/

#ifndef mxUNALIGNED
#define mxUNALIGNED( x )	__unaligned( x )
#endif

#define mxALIGN( decl, bytes )	__declspec(align(bytes))	decl
#define mxALIGN_4( decl )		__declspec(align(4))		decl
#define mxALIGN_16( decl )		__declspec(align(16))		decl
#define mxALIGN_32( decl )		__declspec(align(32))		decl
#define mxALIGN_64( decl )		__declspec(align(64))		decl
#define mxALIGN_128( decl )		__declspec(align(128))		decl

// Align by the size of cache line.
#define mxALIGN_BY_CACHE_LINE	__declspec(align( CACHE_LINE_SIZE ))


#define mxDEFINE_ALIGNED_DATA( type, name, alignment )			_declspec(align(alignment)) type name;
#define mxDEFINE_ALIGNED_DATA_STATIC( type, name, alignment )	static _declspec(align(alignment)) type name;
#define mxDEFINE_ALIGNED_DATA_CONST( type, name, alignment )	const _declspec(align(alignment)) type name;


#define IS_8_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 7) == 0)
#define IS_16_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 15) == 0)
#define IS_32_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 31) == 0)
#define IS_64_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 63) == 0)
#define IS_ALIGNED_BY( pointer, bytes )		(( (UINT_PTR)(pointer) & ((bytes) - 1)) == 0)

// Get the alignment of the passed type/variable, in bytes.
//	BUG: there's a bug in MSVC++: http://old.nabble.com/-Format--Asserts-with-non-default-structure-packing-in-1.37-(MSVC)-td21215959.html
//	_alignof does correctly report the alignment of 2-byte aligned objects,
//	but that only the alignment of structures can be specified by the user.
#define mxALIGNMENT( x )		__alignof( x )

// These macros change alignment of the enclosed class/struct.
#define MX_SET_PACKING( byteAlignment )		mxPRAGMA( pack( push, byteAlignment ) )
#define MX_END_PACKING						mxPRAGMA( pack( pop ) )

//
//	MemCmp - Compare two blocks of memory.
//
//	Compares the first num bytes of the block of memory
//	pointed by ptr1 to the first num bytes pointed by ptr2,
//	returning zero if they all match or a value different from zero representing
//	which is greater if they do not.
//
//	NOTE: 'pDestAddress' and 'pSrcAddress' must not point at the same memory location!
//
#define MemCmp( ptr1, ptr2, size )	::memcmp( (ptr1), (ptr2), (size) )
/*
FORCEINLINE int MemCmp( const void* __restrict pMem1, const void* __restrict pMem2, SizeT numBytes )
{
	MX_ASSERT_PTR( pMem1 );	MX_ASSERT_PTR( pMem2 );
	MX_ASSERT( pMem1 != pMem2 );
	MX_ASSERT( numBytes > 0 );
	return ::memcmp( pMem1, pMem2, numBytes );
}
*/

//
//	MemCopy
//
//	NOTE: 'pDestAddress' and 'pSrcAddress' must not point at the same memory location!
//
#define MemCopy( Destination, Source, Length )	::memcpy( (Destination), (Source), (Length) )
/*
FORCEINLINE void MemCopy( void* __restrict pDestAddress, const void* __restrict pSrcAddress, SizeT numBytes )
{
	MX_ASSERT( numBytes > 0 );
	MX_ASSERT_PTR( pDestAddress );	MX_ASSERT_PTR( pSrcAddress );
	MX_ASSERT( pDestAddress != pSrcAddress );
	::memcpy( pDestAddress, pSrcAddress, numBytes );
}
*/

//
//	MemMove
//
//	NOTE: 'pDestAddress' and 'pSrcAddress' must not point at the same memory location!
//
#define MemMove( Destination, Source, Length )	::memmove( (Destination), (Source), (Length) )
/*
FORCEINLINE void MemMove( void* __restrict pDestAddress, const void* __restrict pSrcAddress, SizeT numBytes )
{
	MX_ASSERT( numBytes > 0 );
	MX_ASSERT_PTR( pDestAddress );	MX_ASSERT_PTR( pSrcAddress );
	MX_ASSERT( pDestAddress != pSrcAddress );
	::memmove( pDestAddress, pSrcAddress, numBytes );
}
*/

//
//	MemSet
//
#define MemSet( Destination, Value, Length )	::memset( (Destination), (Value), (Length) )
/*
FORCEINLINE void MemSet( void* pAddress, INT value, SizeT numBytes )
{
	MX_ASSERT( numBytes > 0 );
	MX_ASSERT_PTR( pAddress );
	::memset( pAddress, value, numBytes );
}
*/

//
//	MemZero
//
#define MemZero( Destination, Length )	::memset( (Destination), 0, (Length) )
/*
FORCEINLINE void MemZero( void* pAddress, SizeT numBytes )
{
	MX_ASSERT( numBytes > 0 );
	MX_ASSERT_PTR( pAddress );
	::memset( pAddress, 0, numBytes );
}
*/

//
// Test if two given memory areas are overlapping.
//
INLINE bool mxMemOverlap( const BYTE* srcPtr, SizeT srcSize, const BYTE* dstPtr, SizeT dstSize )
{
	if ( srcPtr == dstPtr ) {
		return true;
	}
	else if ( srcPtr > dstPtr ) {
		return ( srcPtr + srcSize ) > dstPtr;
	}
	else {
		return ( dstPtr + dstSize ) > srcPtr;
	}
}

INLINE bool mxMemIsZero( const void* pMem, SizeT numBytes )
{
	const BYTE* ptr = (const BYTE*) pMem;
	while( numBytes > 0 )
	{
		if( *ptr != 0 ) {
			return false ;
		}
		ptr++;
		numBytes--;
	}
	return true;
}

mxSWIPED("DirectX SDK")
// Fast (unrolled) memcpy
FORCEINLINE void dwordMemcpy(
	__out_bcount(uByteCount) void * RESTRICT_PTR( pDest ),
	__in_bcount(uByteCount) CONST void * RESTRICT_PTR( pSource ),
	UINT uByteCount )
{
    UINT i;
    mxASSERT(uByteCount % 4 == 0);
#ifdef _AMD64_
    const UINT qwordCount = uByteCount >> 3;

    __int64* src64 = (__int64*) pSource;
    __int64* dst64 = (__int64*) pDest;

    for (i=0; i<(qwordCount & 0x3); i++)
    {
        *(dst64) = *(src64);
        dst64++;
        src64++;
    }

    for (; i<qwordCount; i+= 4)
    {
        *(dst64     ) = *(src64     );
        *(dst64 + 1 ) = *(src64 + 1 );
        *(dst64 + 2 ) = *(src64 + 2 );
        *(dst64 + 3 ) = *(src64 + 3 );
        dst64 += 4;
        src64 += 4;
    }

    mxANALYSIS_ASSUME( dst64 - static_cast< __int64* >(pDest) <= uByteCount - 4 );
    mxANALYSIS_ASSUME( src64 - static_cast< const __int64* >(pSource) <= uByteCount - 4 );
    if( uByteCount & 0x4 )
    {
        *((UINT*)dst64) = *((UINT*)src64);
    }
#else
    const UINT dwordCount = uByteCount >> 2;

    for (i=0; i<(dwordCount & 0x3); i++)
    {
#pragma prefast(suppress: __WARNING_UNRELATED_LOOP_TERMINATION, "(dwordCount & 03) < dwordCount")
        ((UINT*)pDest)[i  ] = ((UINT*)pSource)[i  ];
    }
    for (; i<dwordCount; i+= 4)
    {
        ((UINT*)pDest)[i  ] = ((UINT*)pSource)[i  ];
        ((UINT*)pDest)[i+1] = ((UINT*)pSource)[i+1];
        ((UINT*)pDest)[i+2] = ((UINT*)pSource)[i+2];
        ((UINT*)pDest)[i+3] = ((UINT*)pSource)[i+3];
    }
#endif
}

#define MemCopy16( Destination, Source, Length )	dwordMemcpy( (Destination), (Source), (Length) )

/*
Obsolete functions preserved only because of their curious technolodgy.

mxSWIPED("ICE (Pierre Terdiman)")
//!	Fills a buffer with a given dword.
//!	\param		addr	[in] buffer address
//!	\param		nb		[in] number of dwords to write
//!	\param		value	[in] the dword value
//!	\see		FillMemory
//!	\see		ZeroMemory
//!	\see		CopyMemory
//!	\see		MoveMemory
//!	\warning	writes nb*4 bytes !
FORCEINLINE void StoreDwords( udword* dest, udword nb, udword value )
{
	// The asm code below **SHOULD** be equivalent to one of those C versions
	// or the other if your compiled is good: (checked on VC++ 6.0)
	//
	//	1) while(nb--)	*dest++ = value;
	//
	//	2) for(udword i=0;i<nb;i++)	dest[i] = value;
	//
	_asm push	EAX
	_asm push	ECX
	_asm push	EDI
	_asm mov	EDI, dest
	_asm mov	ECX, nb
	_asm mov	EAX, value
	_asm rep	stosd
	_asm pop	EDI
	_asm pop	ECX
	_asm pop	EAX
}

FORCEINLINE void dwordmemcpy( void* pDest, const void* pSrc, int nCount )
{	
	ASM
	{
		mov		ECX, nCount
		mov		ESI, pSrc
		mov		EDI, pDest
		mov     EDX, ECX
		shr     ECX, 2
		and     EDX, 3
		rep     movsd
		mov     ECX, EDX
		rep     movsb
	}
}

FORCEINLINE void dwordmemzero( void* pDest, int nCount )
{	
	ASM
	{
		mov		ECX, [nCount]
		mov		EDI, [pDest]
		xor     EAX, EAX
		mov		EDX, ECX
		shr		ECX, 2
		and		EDX, 3
		rep     stosd
		mov     ECX, EDX
		rep     stosb
	}
}
*/
//=============================================================================

//
// Stack memory allocation.
//
//	NOTE: use with care! It can easily crash your program and/or become a source of a hard-to-find bug!
//

enum { MAX_STACK_CAPACITY = 512*1024 /* 512 Kb */ };

/*
  NOTE: these CANNOT be wrapped into functions, they MUST be #defined!
  i.e. this is WRONG:

FORCEINLINE void* StackAlloc( SizeT numBytes )
{
	MX_ASSERT( numBytes > 0 );
	return _alloca( (numBytes + 7) & ~7 );
}
FORCEINLINE void StackFree( void* pMem )
{
	// nothing
}
*/

#define mxStackAlloc( numBytes )	_alloca( (numBytes) )
#define mxStackAlloc8( numBytes )	_alloca( ((numBytes) + 7) & ~7 )
#define mxStackAlloc16( numBytes )	_alloca( ((numBytes) + 15) & ~15 )

#define mxStackFree( pMem )

//=============================================================================

mxSWIPED("OOOII, file: oByte.h");
// Utility functions helpful when dealing with memory buffers and 
// pointers, especially when it is useful to go back and forth 
// between thinking of the buffer as bytes and as its type without
// a lot of casting.

// Alignment
template<typename T> inline T oByteAlign(T _Value, size_t _Alignment) { return (T)(((size_t)_Value + _Alignment - 1) & ~(_Alignment - 1)); }
template<typename T> inline T oByteAlignDown(T _Value, size_t _Alignment) { return (T)((size_t)_Value & ~(_Alignment - 1)); }
template<typename T> inline bool oIsByteAligned(T _Value, size_t _Alignment) { return oByteAlign(_Value, _Alignment) == _Value; }

// Offsets
template<typename T> inline T* oByteAdd(T* _Pointer, size_t _NumBytes) { return reinterpret_cast<T*>(((char*)_Pointer) + _NumBytes); }
template<typename T, typename U> inline T* oByteAdd(T* _RelativePointer, U* _BasePointer) { return reinterpret_cast<T*>(((char*)_RelativePointer) + reinterpret_cast<size_t>(_BasePointer)); }
template<typename T> inline T* oByteAdd(T* _Pointer, size_t _Stride, size_t _Count) { return reinterpret_cast<T*>(((char*)_Pointer) + _Stride * _Count); }
template<typename T, typename U> inline ptrdiff_t oByteDiff(T* t, U* u) { return (ptrdiff_t)((char*)t - (char*)u); }
template<typename T> inline size_t oBytePadding(T value, size_t alignment) { return static_cast<T>(oByteAlign(value, alignment)) - value; }
template<typename T> inline size_t oIndexOf(T* el, T* base) { return oByteDiff(el, base) / sizeof(T); }
template<typename T> inline bool oIsPow2(T n) { return n ? (((n) & ((n)-1)) == 0) : false; }

// Endian swapping
inline unsigned short oByteSwap(unsigned short x) { return (x<<8) | (x>>8); }
inline unsigned int oByteSwap(unsigned int x) { return (x<<24) | ((x<<8) & 0x00ff0000) | ((x>>8) & 0x0000ff00) | (x>>24); }
inline unsigned long long oByteSwap(unsigned long long x) { return (x<<56) | ((x<<40) & 0x00ff000000000000ll) | ((x<<24) & 0x0000ff0000000000ll) | ((x<<8) & 0x000000ff00000000ll) | ((x>>8) & 0x00000000ff000000ll) | ((x>>24) & 0x0000000000ff0000ll) | ((x>>40) & 0x000000000000ff00ll) | (x>>56); }
inline short oByteSwap(short x) { unsigned short r = oByteSwap(*(unsigned short*)&x); return *(short*)&r; }
inline int oByteSwap(int x) { unsigned int r = oByteSwap(*(unsigned int*)&x); return *(int*)&r; }
inline long long oByteSwap(long long x) { unsigned long long r = oByteSwap(*(unsigned long long*)&x); return *(long long*)&r; }



mxSWIPED("Havok");

#define MX_CPU_PTR( A ) A

FORCEINLINE mxInt32 mxPointerToInt32( const void* ptr )
{
	return static_cast<int>( mxULong(ptr) );
}

/// get the byte offset of B - A, as a full long.
FORCEINLINE mxULong mxGetByteOffset( const void* base, const void* pntr)
{
	return mxULong(pntr) - mxULong(base);
}

/// get the byte offset of B - A, as an int (64bit issues, so here for easy code checks)
FORCEINLINE int mxGetByteOffsetInt( const void* base, const void* pntr)
{
	return static_cast<int>( mxGetByteOffset( base, pntr ) );
}

/// get the byte offset of B - A, as a full 64bit mxUint64.
FORCEINLINE mxInt32 mxGetByteOffsetCpuPtr( const MX_CPU_PTR(void*) base, const MX_CPU_PTR(void*) pntr)
{
	return mxInt32(mxULong((MX_CPU_PTR(const char*))(pntr) - (MX_CPU_PTR(const char*))(base)));
}

template <typename TYPE>
ALWAYSINLINE TYPE* mxAddByteOffset( TYPE* base, mxULong offset )
{
	return reinterpret_cast<TYPE*>( reinterpret_cast<char*>(base) + offset );
}

template <typename TYPE>
ALWAYSINLINE TYPE mxAddByteOffsetCpuPtr( TYPE base, mxULong offset )
{
	return reinterpret_cast<TYPE>( reinterpret_cast<char*>(base) + offset );
}

template <typename TYPE>
ALWAYSINLINE const TYPE* mxAddByteOffsetConst( const TYPE* base, mxULong offset )
{
	return reinterpret_cast<const TYPE*>( reinterpret_cast<const char*>(base) + offset );
}

template <typename TYPE>
ALWAYSINLINE TYPE mxAddByteOffsetCpuPtrConst( TYPE base, mxULong offset )
{
	return reinterpret_cast<TYPE>( reinterpret_cast<const char*>(base) + offset );
}


mxSWIPED("Bullet");
//
// mxSelect avoids branches, which makes performance much better for consoles like Playstation 3 and XBox 360
// Thanks Phil Knight. See also http://www.cellperformance.com/articles/2006/04/more_techniques_for_eliminatin_1.html
//
FORCEINLINE unsigned mxSelect(unsigned condition, unsigned valueIfConditionNonZero, unsigned valueIfConditionZero) 
{
    // Set testNz to 0xFFFFFFFF if condition is nonzero, 0x00000000 if condition is zero
    // Rely on positive value or'ed with its negative having sign bit on
    // and zero value or'ed with its negative (which is still zero) having sign bit off 
    // Use arithmetic shift right, shifting the sign bit through all 32 bits
    unsigned testNz = (unsigned)(((int)condition | -(int)condition) >> 31);
    unsigned testEqz = ~testNz;
    return ((valueIfConditionNonZero & testNz) | (valueIfConditionZero & testEqz)); 
}

FORCEINLINE int mxSelect(unsigned condition, int valueIfConditionNonZero, int valueIfConditionZero)
{
    unsigned testNz = (unsigned)(((int)condition | -(int)condition) >> 31);
    unsigned testEqz = ~testNz; 
    return static_cast<int>((valueIfConditionNonZero & testNz) | (valueIfConditionZero & testEqz));
}

mxSWIPED("Havok");
// If you have a pair of pointers and you have one pointer, then this function allows you to quickly get the other pointer of the pair.
template< typename TYPE >
FORCEINLINE TYPE* mxSelectOtherPtr( TYPE* a, TYPE* pairA, TYPE* pairB )
{
	return reinterpret_cast<TYPE*>( mxULong(a) ^ mxULong(pairA) ^ mxULong(pairB) );
}
mxSWIPED("Havok");
// If you have a pair of pointers and you have one pointer, then this function allows you to quickly get the other pointer of the pair.
// NOTE: 'condition' can only be 0 or 1.
//
template< typename TYPE >
FORCEINLINE TYPE* mxSelectPtr( int select, TYPE* pairA, TYPE* pairB )
{
	mxASSERT( select == 0 || select == 1 );
	mxULong ua = mxULong( pairA );
	mxULong ub = mxULong( pairB );
	return reinterpret_cast<TYPE*>( ua ^ ((ua ^ ub) & (-select)) );
}
mxSWIPED("Havok");
//
//	pxSelectPointer< TYPE >
//
template< typename TYPE >
FORCEINLINE TYPE * mxSelectPointer( mxUInt condition, TYPE* valueIfConditionZero, TYPE* valueIfConditionNonZero )
{
	const mxULong testNz = (mxULong)(((mxLong)condition | -(mxLong)condition) >> 31);
	const mxULong testEqz = ~testNz; 
	return reinterpret_cast<TYPE*>((mxULong(valueIfConditionNonZero) & testNz) | (mxULong(valueIfConditionZero) & testEqz));
}

mxSWIPED("Bullet");
FORCEINLINE float mxSelect( unsigned condition, float valueIfConditionNonZero, float valueIfConditionZero )
{
#ifdef BT_HAVE_NATIVE_FSEL
    return (float)mxFsel((mxScalar)condition - mxScalar(1.0f), valueIfConditionNonZero, valueIfConditionZero);
#else
    return (condition != 0) ? valueIfConditionNonZero : valueIfConditionZero; 
#endif
}

namespace MemAlignUtil
{
	SizeT GetAlignedMemSize( SizeT nBytes );
	void* GetAlignedPtr( void* allocatedMem );
	void* GetUnalignedPtr( void* alignedPtr );
}


//
//	ZERO_OUT( x )
//
//#define ZERO_OUT( x )		MemZero( &(x), sizeof(x) )

// Useful function to clean the structure.
template <class T>
inline void ZERO_OUT( T &t ) { MemZero( &t, sizeof(t) ); }

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
