/*
=============================================================================
	File:	Common.h
	Desc:	Miscellaneous common stuff (template classes,etc)
	ToDo:	use 'unsigned int' instead of 'SizeT' ?
=============================================================================
*/

#ifndef __MX_BASE_COMMON_H__
#define __MX_BASE_COMMON_H__

mxNAMESPACE_BEGIN


// Value indicating invalid index.
//#define INDEX_NONE	(long)(-1)
enum { INDEX_NONE = -1 };

enum { INDEX_NONE_32 = (UINT32)-1 };
enum { INDEX_NONE_16 = (UINT16)-1 };

// ENoInit - Tag for suppressing initialization (also used to mark uninitialized variables).
enum ENoInit { _NoInit };

// EInitDefault - means 'initialize with default value'.
enum EInitDefault { _InitDefault = 0 };

// EInitZero - Used to mark data initialized with invalid values.
enum EInitInvalid { _InitInvalid = -1 };

// EInitZero - Used to mark data initialized with zeros.
enum EInitZero { _InitZero = 0 };

// EInitIdentity - Used to mark data initialized with default Identity/One value.
enum EInitIdentity { _InitIdentity = 1 };

// EInitInfinity - Provided for convenience.
enum EInitInfinity { _InitInfinity = -1 };

// for creating (slow) named constructors
enum EInitSlow { _InitSlow = -1 };

enum EInitCustom { _InitCustom };

//enum EInPlaceSerializationHelper { _FinishedLoadingFlag };
struct _FinishedLoadingFlag { _FinishedLoadingFlag(){} };

enum EDontAddRef { DontAddRef };

// relative or absolute mode
enum ERelAbs
{
	kAbsolute,
	kRelative
};


mxSWIPED("PhyreEngine");
// This enumeration defines acceptable return values from functions that can fail to complete successfully.
//
enum EReturnCode
{
	mxRESULT_NO_ERROR						= 0,	// The function completed successfully.
	mxRESULT_NULL_POINTER_ARGUMENT			= 1,	// An argument to the function was NULL, but should be non-NULL to behave correctly.
	mxRESULT_NEGATIVE_ARGUMENT				= 2,	// An argument to the function was negative, but should be positive to behave correctly.
	mxRESULT_ARGUMENT_OUT_OF_RANGE			= 3,	// An argument to the function was outside of the accepted range.
	mxRESULT_EMPTY_ARGUMENT					= 4,	// A string passed to the function was empty.
	mxRESULT_INVALID_ARGUMENT				= 5,	// A passed object was of the wrong type.
	mxRESULT_CORRUPT_DATA_SOURCE			= 6,	// An error was generated while parsing.
	mxRESULT_INSUFFICIENT_INFORMATION		= 7,	// A matching type could not be found, or the required handler is not registered.
	mxRESULT_FUNCTION_OBSOLETE				= 8,	// The function is now obsolete.
	mxRESULT_UNKNOWN_ERROR					= 9,	// An unknown error occurred.
	mxRESULT_FILE_DOES_NOT_EXIST			= 10,	// The requested file does not exist.
	mxRESULT_FILE_ACCESS_DENIED				= 11,	// Access to the requested file was denied.
	mxRESULT_FILE_WRITE_PROTECTED			= 12,	// The file cannot be written to because it is read only.
	mxRESULT_OUT_OF_MEMORY					= 13,	// The function could not complete successfully due to a lack of memory.
	mxRESULT_UNABLE_TO_ALLOCATE				= 14,	// The function could not complete successfully because it was unable to allocate or lock a required resource.
	mxRESULT_DLL_NOT_LOADED					= 15,	// The DLL required is not loaded.
	mxRESULT_DLL_INCORRECT_VERSION			= 16,	// The loaded DLL is an incorrect version.
	mxRESULT_INSUFFICIENT_DATA				= 17,	// The file was truncated, or the array was not long enough.
	mxRESULT_OBJECT_OF_SAME_NAME_EXISTS		= 18,	// An object with the same name already exists.
	mxRESULT_OBJECT_NOT_FOUND				= 19,	// The requested object was not found.
	mxRESULT_OBJECT_OF_WRONG_TYPE			= 20,	// The object was of the wrong type.
	mxRESULT_UNINITIALIZED_DATA				= 21,	// Some data required has not been correctly initialized before use.
	mxRESULT_VALUE_ALREADY_SET				= 22,	// The function is trying to update a value that can only be set once.
	mxRESULT_FEATURE_NOT_AVAILABLE			= 23,	// The function could not complete because it tries to use a feature (or set of features) not currently supported.

	mxRESULT_COUNT									// Marker, don't use.
};


//
// Helper templates.
//

template< typename T > FORCEINLINE T	Max( T x, T y ) { return ( x > y ) ? x : y; }
template< typename T > FORCEINLINE T	Min( T x, T y ) { return ( x < y ) ? x : y; }
template< typename T > FORCEINLINE INT	MaxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
template< typename T > FORCEINLINE INT	MinIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

template< typename T > FORCEINLINE T	Max3( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? x : z ) : ( ( y > z ) ? y : z ); }
template< typename T > FORCEINLINE T	Min3( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? x : z ) : ( ( y < z ) ? y : z ); }
template< typename T > FORCEINLINE INT	Max3Index( T x, T y, T z ) { return ( x > y ) ? ( ( x > z ) ? 0 : 2 ) : ( ( y > z ) ? 1 : 2 ); }
template< typename T > FORCEINLINE INT	Min3Index( T x, T y, T z ) { return ( x < y ) ? ( ( x < z ) ? 0 : 2 ) : ( ( y < z ) ? 1 : 2 ); }

template< typename T > FORCEINLINE T	Abs( T x )		{ return ( x >= 0 ) ? x : -x; }
template< typename T > FORCEINLINE T	Sign( T x )		{ return ( x > 0 ) ? 1 : ( ( x < 0 ) ? -1 : 0 ); }
template< typename T > FORCEINLINE T	Square( T x )	{ return x * x; }
template< typename T > FORCEINLINE T	Cube( T x )		{ return x * x * x; }

template< typename T > FORCEINLINE T	Clamp( const T value, const T Min, const T Max )	{ return (value < Min) ? Min : (value > Max) ? Max : value; }
template< typename T > FORCEINLINE T	Saturate( const T value )						{ return Clamp<T>( value, 0, 1 ); }

template< typename T > FORCEINLINE T	Mean( const T a, const T b )	{ return (a + b) * (T)0.5f; }

template< typename T > FORCEINLINE T	Wrap( const T a, const T min, const T max )	{ return (a < min)? max - (min - a) : (a > max)? min - (max - a) : a; }

template< typename T > FORCEINLINE bool	IsInRange( const T value, const T Min, const T Max )	{ return (value > Min) && (value < Max); }
template< typename T > FORCEINLINE bool	IsInRangeInc( const T value, const T Min, const T Max )	{ return (value >= Min) && (value <= Max); }


template< typename T >
FORCEINLINE void TSetMin( T& a, const T& b ) {
    if( b < a ) {
		a = b;
	}
}

template< typename T >
FORCEINLINE void TSetMax( T& a, const T& b ) {
    if( a < b ) {
		a = b;
	}
}

template< typename T >
FORCEINLINE const T& TGetClamped( const T& a, const T& lo, const T& hi )
{
	return (a < lo) ? lo : (hi < a ? hi : a);
}

// maps negative values to -1, zero to 0 and positive to +1
template< typename T >
FORCEINLINE T MapToUnitRange( const T& value ) {
	return T( (value > 0) - (value < 0) );
}

//
//	TSwap - exchanges values of two elements.
//
template< typename type >
FORCEINLINE
void TSwap( type & a, type & b )
{
	const type  temp = a;
	a = b;
	b = temp;
}

/*
	swap in C++ 11:

	template <class T> swap(T& a, T& b)
	{
		T tmp(std::move(a));
		a = std::move(b);   
		b = std::move(tmp);
	}
*/

//
//	TSwap16 - exchanges values of two 16-byte aligned elements.
//
template< typename type >
FORCEINLINE
void TSwap16( type & a, type & b )
{
	mxALIGN_16( const type  temp ) = a;
	a = b;
	b = temp;
}

//template <class T> inline void Exchange( T & X, T & Y )
//{
//	const T Tmp = X;
//	X = Y;
//	Y = Tmp;
//}


// TODO: endian swap


template< typename TYPE >
FORCEINLINE void TOrderPointers( TYPE *& pA, TYPE *& pB )
{
	if( (pxULong)pA > (pxULong)pB )
	{
		TSwap( pA, pB );
	}
}

template< typename TYPE >
FORCEINLINE void TSort2( TYPE & oA, TYPE & oB )
{
	if( oA > oB )
	{
		TSwap( oA, oB );
	}
}

template< typename TYPE >
FORCEINLINE void TSort3( TYPE & a, TYPE & b, TYPE & c )
{
	if( a > b ) {
		TSwap( a, b );
	}
	if( b > c ) {
		TSwap( b, c );
	}
	if( a > b ) {
		TSwap( a, b );
	}
	if( b > c ) {
		TSwap( b, c );
	}
}


// Figure out if a type is a pointer
template< typename TYPE > struct is_pointer
{
	enum { VALUE = false };
};
template< typename TYPE > struct is_pointer< TYPE* >
{
	enum { VALUE = true };
};

// Removes the pointer from a type
template< typename TYPE > struct strip_pointer
{
	typedef TYPE Type;
};
template< typename TYPE > struct strip_pointer< TYPE* >
{
	typedef TYPE Type;
};



//
// The "Empty Member" C++ Optimization
// See:
// http://www.cantrip.org/emptyopt.html
// http://www.tantalon.com/pete/cppopt/final.htm#EmptyMemberOptimization
//
template< class Base, class Member >
struct BaseOpt : Base {
	Member m;
	BaseOpt( Base const& b, Member const& mem ) 
		: Base(b), m(mem)
	{ }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class T >
FORCEINLINE T* ConstructInPlace( T* mem )
{
	return ::new(mem) T;
}

template< class T >
FORCEINLINE T* ConstructInPlace( T* mem, const T* copy )
{
	return ::new(mem) T( *copy );
}

template< class T, class T2 >
FORCEINLINE T* ConstructInPlace( T* mem, T2 t2 )
{
	return ::new(mem) T( t2 );
}

template< class T, class T2, class T3 >
FORCEINLINE T* ConstructInPlace( T* mem, T2 t2, T3 t3 )
{
	return ::new(mem) T( t2, t3 );
}

template< class T, class T2, class T3, class T4 >
FORCEINLINE T* ConstructInPlace( T* mem, T2 t2, T3 t3, T4 t4 )
{
	return ::new(mem) T( t2, t3, t4 );
}

template< class T, class T2, class T3, class T4, class T5 >
FORCEINLINE T* ConstructInPlace( T* mem, T2 t2, T3 t3, T4 t4, T5 t5 )
{
	return ::new(mem) T( t2, t3, t4, t5 );
}

//-----------------------------------------------------------------------------
// Methods to invoke the constructor, copy constructor, and destructor
//-----------------------------------------------------------------------------

template< class T >
FORCEINLINE void Construct( T* pMemory )
{
	::new( pMemory ) T;
}

template< class T >
FORCEINLINE void CopyConstruct( T* pMemory, T const& src )
{
	::new( pMemory ) T(src);
}

// Destructs an object without freeing the memory associated with it.
template< class T >
FORCEINLINE void Destruct( T* ptr )
{
	ptr->~T();
//#if MX_DEBUG
//	MemSet( ptr, FREED_MEM_ID, sizeof(T) );
//#endif
}


// Constructs an array of objects with placement ::new.
template< typename T >
inline T * TConstructN( T* basePtr, SizeT objectCount )
{
	for( SizeT i = 0; i < objectCount; i++ )
	{
		::new ( basePtr + i ) T();
	}
	return basePtr;
}

// Explicitly calls destructors for an array of objects.
//
template< typename T >
inline void TDestructN( T* basePtr, SizeT objectCount )
{
#if 1
	for( SizeT i = 0; i < objectCount; i++ )
	{
		(basePtr + i)->~T();
	}
#else
	Assert( objectCount != 0 );

	T* start = basePtr;
	T* current = basePtr + objectCount - 1;

	// Destroy in reverse order, to match construction order.
	while( current-- > start )
	{
		current->~T();
	}
#endif
//#if MX_DEBUG
//	MemSet( basePtr, FREED_MEM_ID, sizeof(T) * objectCount );
//#endif
}


template< typename T >
FORCEINLINE void TDestructOne_IfNonPOD( T & o )
{
	if( !TypeTrait<T>::IsPlainOldDataType )
	{
		o.~T();
	}
}

template< typename T >
inline void TConstructN_IfNonPOD( T* basePtr, SizeT objectCount )
{
	if( !TypeTrait<T>::IsPlainOldDataType )
	{
		TConstructN< T >( basePtr, objectCount );
	}
}

template< typename T >
inline void TDestructN_IfNonPOD( T* basePtr, SizeT objectCount )
{
	if( !TypeTrait<T>::IsPlainOldDataType )
	{
		TDestructN< T >( basePtr, objectCount );
	}
}


template< typename T >
inline void TCopyArray( T* dest, const T* src, SizeT count )
{
	if( TypeTrait<T>::IsPlainOldDataType )
	{
		//if( count )
		MemCopy( dest, src, count * sizeof T );
	}
	else
	{
		for( SizeT i = 0; i < count; i++ )
		{
			dest[ i ] = src[ i ];
		}
	}
}

template< typename T >
inline void TCopyConstructArray( T* dest, const T* src, SizeT count )
{
	if( TypeTrait<T>::IsPlainOldDataType )
	{
		//if( count )
		MemCopy( dest, src, count * sizeof T );
	}
	else
	{
		for( SizeT i = 0; i < count; i++ )
		{
			new (&dest[i]) T( src[i] );
		}
	}
}


template< typename T >
inline void TSetArray( T* dest, SizeT count, const T& srcValue )
{
	// memset - The value is passed as an int, but the function fills the block of memory
	// using the unsigned char conversion of this value
	//if( TypeTrait<T>::IsPlainOldDataType
	//	&& ( sizeof srcValue <= sizeof int )
	//	)
	//{
	//	//if( count )
	//	MemSet( dest, &srcValue, count * sizeof T );
	//}
	//else
	{
		for( SizeT i = 0; i < count; i++ )
		{
			dest[ i ] = srcValue;
		}
	}
}


template< typename T >
inline void TMoveArray( T* dest, const T* src, SizeT count )
{
	if( TypeTrait<T>::IsPlainOldDataType )
	{
		//if( count )
		MemMove( dest, src, count * sizeof T );
	}
	else
	{
		for( SizeT i = 0; i < count; i++ )
		{
			dest[ i ] = src[ i ];
		}
	}
}


template< UINT NUM_BUCKETS >
void Build_Offset_Table_1D(const UINT (&counts)[NUM_BUCKETS],
						   UINT (&offsets)[NUM_BUCKETS])
{
	UINT	numSoFar = 0;

	for( UINT iBucket = 0; iBucket < NUM_BUCKETS; iBucket++ )
	{
		offsets[ iBucket ] = numSoFar;

		numSoFar += counts[ iBucket ];
	}
}

// builds a 2D table of offsets for indexing into sorted list
// (similar to prefix sum computation in radix sort algorithm)
//
template< UINT NUM_ROWS, UINT NUM_COLUMNS >
void Build_Offset_Table_2D(const UINT counts[NUM_ROWS][NUM_COLUMNS],
						UINT offsets[NUM_ROWS][NUM_COLUMNS])
{
	UINT	numSoFar = 0;

	for( UINT iRow = 0; iRow < NUM_ROWS; iRow++ )
	{
		for( UINT iCol = 0; iCol < NUM_COLUMNS; iCol++ )
		{
			// items in cell [iRow][iCol] start at 'numSoFar'
			offsets[iRow][iCol] = numSoFar;

			numSoFar += counts[iRow][iCol];
		}
	}
}

template< UINT COUNT >
UINT Calculate_Sum_1D( const UINT (&elements)[COUNT] )
{
	UINT totalCount = 0;
	for( UINT i = 0; i < COUNT; i++ )
	{
		totalCount += elements[ i ];
	}
	return totalCount;
}

template< UINT NUM_ROWS, UINT NUM_COLUMNS >
UINT Calculate_Sum_2D( const UINT (&elements)[NUM_ROWS][NUM_COLUMNS] )
{
	UINT totalCount = 0;
	for( UINT iRow = 0; iRow < NUM_ROWS; iRow++ )
	{
		for( UINT iCol = 0; iCol < NUM_COLUMNS; iCol++ )
		{
			totalCount += elements[iRow][iCol];
		}
	}
	return totalCount;
}

/*
====================================================================
	
	Handle type

====================================================================
*/

//
//	THandle - a templated integer handle class, initialized to INDEX_NONE by default.
//
template< typename T >
class THandle {
public:
				THandle();
				THandle( const T inValue );
	explicit	THandle( ENoInit );
	explicit	THandle( EInitZero );
	explicit	THandle( EInitIdentity );
	explicit	THandle( EInitInvalid );	// default behavior

	// Type conversions.

	operator T () const;

	// Assignment.

	THandle &	operator = ( const THandle other );

private:
	T		value;
};

template< class T >
FORCEINLINE THandle< T >::THandle()
	: value( (T)INDEX_NONE )
{}

template< class T >
FORCEINLINE THandle< T >::THandle( const T inValue )
	: value( inValue )
{}

template< class T >
FORCEINLINE THandle< T >::THandle( ENoInit )
{}

template< class T >
FORCEINLINE THandle< T >::THandle( EInitZero )
	: value( (T)0 )
{}

template< class T >
FORCEINLINE THandle< T >::THandle( EInitIdentity )
	: value( (T)1 )
{}

template< class T >
FORCEINLINE THandle< T >::THandle( EInitInvalid )
	: value( (T)INDEX_NONE )
{}

template< class T >
FORCEINLINE THandle< T >::operator T () const
{
	return value;
}

template< class T >
FORCEINLINE THandle<T> & THandle< T >::operator = ( const THandle<T> other )
{
	value = other.value;
	return *this;
}



// Handle to a memory manager.
// Containers store handles instead of pointers to conserve space.
// 16-bit indices should be enough.
//
typedef THandle< U2 > HMemory;







// memory usage stats
//
struct MemStatsCollector
{
	SizeT	staticMem;	// size of non-allocated memory, in bytes
	SizeT	dynamicMem;	// size of allocated memory, in bytes

public:
	inline MemStatsCollector()
	{
		Reset();
	}
	inline void Reset()
	{
		staticMem = 0;
		dynamicMem = 0;
	}
};

mxNAMESPACE_END

#endif /* !__MX_BASE_COMMON_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
