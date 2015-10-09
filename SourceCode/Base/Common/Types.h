/*
=============================================================================
	File:	Types.h
	Desc:	Typedefs for common types for scalability, portability, etc.
	Note:	This file uses other base types defined
			in a platform-specific header.
=============================================================================
*/

#ifndef __MX_BASE_TYPES_H__
#define __MX_BASE_TYPES_H__

mxNAMESPACE_BEGIN


//
//	Base integer types.
//




//
//	Array index type.
//
typedef mxUInt	IndexT;




//
//	TEnum< enumName, storage > - a wrapper to store an enum with explicit size.
//
template< typename ENUM, typename STORAGE = UINT32 >
class TEnum {
public:
	FORCEINLINE TEnum()
	{}

	FORCEINLINE TEnum( ENUM e )
	{
		m_value = static_cast< STORAGE >( e );
	}

	FORCEINLINE operator ENUM () const
	{
		return static_cast< ENUM >( m_value );
	}

	FORCEINLINE void operator = ( ENUM e )
	{
		m_value = static_cast< STORAGE >( e );
	}

	FORCEINLINE bool operator == ( ENUM e ) const
	{
		return (m_value == static_cast< STORAGE >( e ));
	}
	FORCEINLINE bool operator != ( ENUM e ) const
	{
		return (m_value != static_cast< STORAGE >( e ));
	}

	FORCEINLINE STORAGE AsInt() const
	{
		return m_value;
	}

	// ??? overload 'forbidden' operators like '+', '-', '/', '<<', etc
	// to catch programming errors early?

private:
	STORAGE		m_value;
};


#if MX_DEBUG

	// view names in debugger
	#define mxDECLARE_ENUM_TYPE( enumName, storage, newType )	\
		typedef enumName	newType;

#else // !MX_DEBUG

	#define mxDECLARE_ENUM_TYPE( enumName, storage, newType )	\
		typedef TEnum< enumName, storage >	newType;

#endif


//
//	Boolean types.
//

//
//	TBool - a boolean type with explicit storage type.
//
template< typename STORAGE = UINT32 >
class TBool {
public:
	TBool()
	{}

	FORCEINLINE	explicit TBool( bool value )
		: m_value( value )
	{}
	
	// Casting to 'bool'.

	FORCEINLINE operator bool () const
	{
		return ( m_value != 0 );
	}

	// Assignment.

	FORCEINLINE TBool & operator = ( bool value )
	{
		m_value = value;
		return *this;
	}
	FORCEINLINE TBool & operator = ( const TBool other )
	{
		m_value = other.m_value;
		return *this;
	}

	// Comparison.

	FORCEINLINE TBool operator == ( bool value ) const
	{
		return (m_value == static_cast< STORAGE >( value ));
	}
	FORCEINLINE TBool operator != ( bool value ) const
	{
		return (m_value != static_cast< STORAGE >( value ));
	}

	FORCEINLINE TBool operator == ( const TBool other ) const
	{
		return m_value == other.m_value;
	}
	FORCEINLINE TBool operator != ( const TBool other ) const
	{
		return m_value != other.m_value;
	}

	// TODO: overload 'forbidden' operators like '+', '-', '/', '<<', etc
	// to catch programming errors early?
	//TBool operator + (const TBool& other) const
	//{ StaticAssert( false && "Invalid operation" ); }

private:
	STORAGE		m_value;
};

//
//	ETroolean
//
//	NOTE: don't modify the values of these constants!
//
enum ETroolean
{
	False	= 0,
	True	= 1,
	DontCare		// Whatever/don't care value
};

typedef TEnum<
	ETroolean,
	int
> mxTrool;


enum ThreeState_t
{
	TRS_FALSE = 0,
	TRS_TRUE,
	TRS_NONE,
};


template< typename BOOL_TYPE >
FORCEINLINE
BOOL_TYPE InvertBool( BOOL_TYPE & o )
{
	return (o = !o);
}

//--------------------------------------------------------

//
//	'Bitfield' type.
//
typedef mxULong	BITFIELD;

//
//	TBits - is a simple class representing a bitmask.
//
template< typename TYPE, typename STORAGE = UINT32 >
class TBits {
public:
	FORCEINLINE TBits()
	{}

	FORCEINLINE TBits( STORAGE value )
		: m_bitmask( value )
	{}

	FORCEINLINE explicit TBits( TYPE e )
	{
		m_bitmask = static_cast< STORAGE >( e );
	}

	FORCEINLINE operator TYPE () const
	{
		return static_cast< TYPE >( m_bitmask );
	}

	FORCEINLINE void operator = ( TYPE e )
	{
		m_bitmask = static_cast< STORAGE >( e );
	}

	FORCEINLINE bool operator == ( TYPE e ) const
	{
		return (m_bitmask == static_cast< STORAGE >( e ));
	}
	FORCEINLINE bool operator != ( TYPE e ) const
	{
		return (m_bitmask != static_cast< STORAGE >( e ));
	}

	FORCEINLINE TBits & operator &= ( TBits other )
	{
		m_bitmask &= other.m_bitmask;
		return *this;
	}
	FORCEINLINE TBits & operator ^= ( TBits other )
	{
		m_bitmask ^= other.m_bitmask;
		return *this;
	}
	FORCEINLINE TBits & operator |= ( TBits other )
	{
		m_bitmask |= other.m_bitmask;
		return *this;
	}

	FORCEINLINE TBits operator ~ () const
	{
		return TBits( ~m_bitmask );
	}
	FORCEINLINE bool operator ! () const
	{
		return !m_bitmask;
	}

	FORCEINLINE void Clear()
	{
		m_bitmask = 0;
	}

	FORCEINLINE void SetAll( STORAGE s )
	{
		m_bitmask = s;
	}

	FORCEINLINE void OrWith( STORAGE s )
	{
		m_bitmask |= s;
	}

	FORCEINLINE void XorWith( STORAGE s )
	{
		m_bitmask ^= s;
	}

	FORCEINLINE void AndWith( STORAGE s )
	{
		m_bitmask &= s;
	}

	FORCEINLINE void SetWithMask( STORAGE s, STORAGE mask )
	{
		m_bitmask = (m_bitmask & ~mask) | (s & mask);
	}

	FORCEINLINE STORAGE Get() const
	{
		return m_bitmask;
	}

	FORCEINLINE STORAGE Get( STORAGE mask ) const
	{
		return m_bitmask & mask;
	}

	FORCEINLINE bool AnyIsSet( STORAGE mask ) const
	{
		return (m_bitmask & mask) != 0;
	}
	
	FORCEINLINE bool AllAreSet( STORAGE mask ) const
	{
		return (m_bitmask & mask) == mask;
	}

public:
	STORAGE		m_bitmask;
};

//---------------------------------------------------------------------------

// See: http://molecularmusings.wordpress.com/2011/08/23/flags-on-steroids/#more-125
//

mxUNDONE
#if 0

template< class TYPE >
class TFlags
{
	typedef typename TYPE::Enum Enum;
	typedef typename TYPE::Bits Bits;

public:
	typedef char Description[512];

	inline TFlags(void)
		: m_flags(0)
	{
	}

	inline explicit TFlags(Enum flag)
		: m_flags(flag)
	{
	}

	inline void Set(Enum flag)
	{
		m_flags |= flag;
	}

	inline void Remove(Enum flag)
	{
		m_flags &= ~flag;
	}

	inline void Clear(void)
	{
		m_flags = 0;
	}

	inline bool IsSet(Enum flag) const
	{
		return ((m_flags & flag) != 0);
	}

	inline bool IsAnySet(void) const
	{
		return (m_flags != 0);
	}

	inline bool AreAllSet(void) const
	{
		return (m_flags == ((1ull << TYPE::Count) - 1u));
	}

	const char* ToString(Description& description) const
	{
		int offset = 0;
		for (size_t i=0; i != 0; i++)
		{
			if ((m_flags & (1u << i)) != 0)
			{
				offset += _snprintf_s(description + offset, sizeof(description) - offset, _TRUNCATE, "%s, ", TYPE::ToString(1u << i));
			}
		}
		// remove the trailing comma, if any     if (offset > 1)
		description[offset-2] = 0;

		return description;
	}

	inline TFlags operator|(TFlags other) const
	{
		return TFlags(m_flags | other.m_flags);
	}

	inline TFlags& operator|=(TFlags other)
	{
		m_flags |= other.m_flags;
		return *this;
	}

private:
	inline explicit TFlags(U32 flags)
		: m_flags(flags)
	{
	}

	union
	{
		U32 m_flags;
		Bits m_bits;
	};
};



#define ME_DECLARE_FLAGS_ENUM(name, n)                    name = (1u << n),
#define ME_DECLARE_FLAGS_BITS(name, n)                    UINT32 name : 1;
#define ME_DECLARE_FLAGS_TO_STRING(name, n)               case name: return ME_PP_STRINGIZE(name);


#define ME_DECLARE_FLAGS(name, ...)    \
struct name    \
{    \
  static const size_t Count = VA_NUM_ARGS(__VA_ARGS__);    \
  static_assert(Count   enum Enum    \
  {    \
    ME_PP_EXPAND_ARGS ME_PP_PASS_ARGS(ME_DECLARE_FLAGS_ENUM, __VA_ARGS__)    \
  };    \
  struct Bits    \
  {    \
    ME_PP_EXPAND_ARGS ME_PP_PASS_ARGS(ME_DECLARE_FLAGS_BITS, __VA_ARGS__)    \
  };    \
  static const char* ToString(size_t value) \
  { \
    switch (value) \
    { \
      ME_PP_EXPAND_ARGS ME_PP_PASS_ARGS(ME_DECLARE_FLAGS_TO_STRING, __VA_ARGS__)    \
      default: \
        mxNO_SWITCH_DEFAULT; \
    } \
  } \
};    \
inline TFlags operator|(name::Enum lhs, name::Enum rhs)    \
{    \
  return (TFlags(lhs) | TFlags(rhs));    \
}
#endif


//---------------------------------------------------------------------------

//
//	mxDeltaTime - can be used to represent delta time values.
//
struct mxDeltaTime
{
	mxReal	fTime;	// in seconds
	mxUInt	iTime;	// in milliseconds

public:
	mxDeltaTime()
	{}

	FORCEINLINE operator mxReal () const
	{
		return fTime;
	}
	FORCEINLINE operator mxUInt () const
	{
		return iTime;
	}
};

inline
void ConvertMicrosecondsToMinutesSeconds(
	U8 microseconds,
	U4 &minutes, U4 &seconds
)
{
	const U8 totalSeconds = microseconds / (1000*1000);
	const U8 totalMinutes = totalSeconds / 60;

	minutes = totalMinutes;

	seconds = totalSeconds - totalMinutes * 60;
}
inline
void ConvertMicrosecondsToHoursMinutesSeconds(
	U8 microseconds,
	U4 &hours, U4 &minutes, U4 &seconds
)
{
	const U8 totalSeconds = microseconds / (1000*1000);
	const U8 totalMinutes = totalSeconds / 60;
	const U8 totalHours = totalMinutes / 60;

	hours = totalHours;
	minutes = totalMinutes - totalHours * 60;
	seconds = totalSeconds - totalMinutes * 60;
}

template< class STRING >
void GetTimeOfDayString( STRING &OutTimeOfDay, UINT hour, UINT minute, UINT second, char separator = '-' )
{
	Assert( hour >= 0 && hour <= 23 );
	Assert( minute >= 0 && minute <= 59 );
	Assert( second >= 0 && second <= 59 );
	//Assert( OutTimeOfDay.IsEmpty() );

	STRING	szHour;
	szHour.SetInt( hour );
	
	STRING	szMinute;
	szMinute.SetInt( minute );

	STRING	szSecond;
	szSecond.SetInt( second );


	if( hour < 10 ) {
		OutTimeOfDay += '0';
	}
	OutTimeOfDay += szHour;
	OutTimeOfDay += separator;

	if( minute < 10 ) {
		OutTimeOfDay += '0';
	}
	OutTimeOfDay += szMinute;
	OutTimeOfDay += separator;

	if( second < 10 ) {
		OutTimeOfDay += '0';
	}
	OutTimeOfDay += szSecond;
}

template< class STRING >
void GetDateString( STRING &OutCurrDate, UINT year, UINT month, UINT day, char separator = '-' )
{
	Assert( year >= 0 && year <= 9999 );
	Assert( month >= 1 && month <= 12 );
	Assert( day >= 1 && day <= 31 );
	//Assert( OutCurrDate.IsEmpty() );

	STRING	szYear;
	szYear.SetInt( year );

	STRING	szMonth;
	szMonth.SetInt( month );

	STRING	szDay;
	szDay.SetInt( day );


	OutCurrDate += szYear;
	OutCurrDate += separator;

	if( month < 10 ) {
		OutCurrDate += '0';
	}
	OutCurrDate += szMonth;
	OutCurrDate += separator;

	if( day < 10 ) {
		OutCurrDate += '0';
	}
	OutCurrDate += szDay;
}

template< class STRING >
void GetDateTimeOfDayString(
	STRING &OutDateTime,
	const CalendarTime& dateTime = CalendarTime::GetCurrentLocalTime(),
	char separator = '-' )
{
	GetTimeOfDayString( OutDateTime, dateTime.hour, dateTime.minute, dateTime.second, separator );
	OutDateTime += '_';
	GetDateString( OutDateTime, dateTime.year, dateTime.month, dateTime.day, separator );
}

template< class STRING >
void GetCurrentDateString( STRING &OutCurrDate )
{
	//Assert( OutCurrDate.IsEmpty() );
	CalendarTime	currDateTime( CalendarTime::GetCurrentLocalTime() );
	GetDateString( OutCurrDate, currDateTime.year, currDateTime.month, currDateTime.day );
}

template< class STRING >
void GetCurrentTimeString( STRING &OutCurrDate, char separator = '-' )
{
	//Assert( OutCurrDate.IsEmpty() );
	CalendarTime	currDateTime( CalendarTime::GetCurrentLocalTime() );
	GetTimeOfDayString( OutCurrDate, currDateTime.hour, currDateTime.minute, currDateTime.second, separator );
}

template< class STRING >
void GetCurrentDateTimeString( STRING &OutCurrDate )
{
	//Assert( OutCurrDate.IsEmpty() );
	CalendarTime	currDateTime( CalendarTime::GetCurrentLocalTime() );
	GetDateTimeOfDayString( OutCurrDate, currDateTime );
}

inline
const char* GetNumberExtension( UINT num )
{
	return	( num == 1 ) ? "st" :
			( num == 2 ) ? "nd" :
			( num == 3 ) ? "rd" :
			"th";
}

inline
const char* GetWordEnding( UINT num )
{
	return ( num < 10 ) ? "s" : "";
}

//
//	EResult - an enum of success/failure codes.
//
enum EResult
{
	E_SUCCESS = 0,
	E_FAILURE = -1,

	ERESULT_FORCE_DWORD = MAX_SDWORD
};

//
//	FCallback
//
typedef void (*FCallback)( void* pUserData );




template< typename A, typename B >	// where A : B
FORCEINLINE
void CopyStruct( A& dest, const B &src )
{
	// compile-time inheritance test
	B* dummy = static_cast<B*>(&dest);
	(void)dummy;

	mxSTATIC_ASSERT( sizeof dest >= sizeof src );
	MemCopy( &dest, &src, sizeof src );
}


// Type punning.
// Always cast any type into any other type.
// (This is evil.)
//	See:
//	http://mail-index.netbsd.org/tech-kern/2003/08/11/0001.html
//	http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Optimize-Options.html#Optimize-Options
//
template<  class TargetType, class SourceType > 
inline TargetType always_cast( SourceType x )
{
	mxCOMPILE_TIME_WARNING( sizeof(TargetType) == sizeof(SourceType), "Type sizes are different" );
	union {
		SourceType S;
		TargetType T;
	} value;

	value.S = x;
	return value.T;
}

template< typename A, typename B >
A& TypePunning( B &v )
{
    union
    {
        A *a;
        B *b;
    } pun;
    pun.b = &v;
    return *pun.a;
}

template< typename A, typename B >
const A& TypePunning( const B &v )
{
    union
    {
        const A *a;
        const B *b;
    } pun;
    pun.b = &v;
    return *pun.a;
}

//
//	TUtil_Conversion< T, U >
//
template< typename T, typename U >
class TUtil_Conversion
{
	typedef char Small;
	class Big { char dummy[2]; };
	static Small test(U);
	static Big test(...);
	static T makeT();
public:
	enum {
		exists = sizeof(test(makeT()))==sizeof(Small),
		sameType = 0
	};
};

template< typename T >
class TUtil_Conversion< T, T > {
public:
	enum {
		exists = 1,
		sameType = 1
	};
};

#define MX_SUPERSUBCLASS( T,U ) \
	TUtil_Conversion< const U*, const T* >::exists && \
	!TUtil_Conversion< const T*, const void* >::sameType



template< class T, class U>
struct SameType {
    enum {
        Result = false
    };
};

template< class T>
struct SameType<T,T> {
    enum {
        Result = true
    };
};


// you can use __is_base_of in MSCV 2008
template< typename BaseT, typename DerivedT >
struct IsRelated
{
    static DerivedT derived();
    static char test(const BaseT&); // sizeof(test()) == sizeof(char)
    static char (&test(...))[2];    // sizeof(test()) == sizeof(char[2])

	// true if conversion exists
    enum { exists = (sizeof(test(derived())) == sizeof(char)) }; 
};


// compile-time inheritance test, can be used to catch common typing errors
template< typename BASE, typename DERIVED >
inline void T_Check_Hierarchy()
{
	BASE* base;
	DERIVED* derived = static_cast<DERIVED*>( base );
	(void)derived;
}




template
<
	bool B,
	class T,
	class U
>
struct ConditionalType {
    typedef T Type;
};

template
<
	class T,
	class U
>
struct ConditionalType<false,T,U> {
    typedef U Type;
};

template< typename TYPE >
struct IncompleteType;


//
//	Type traits.
//

//
//	TypeTrait<T> - used for carrying compile-time type information.
//
//	Can be specialized for custom types.
//
//	NOTE: these templates could also be done with enums.
//
template< typename TYPE >
struct TypeTrait
{
#if 0 // this is not needed for now; and all memory managers allocate aligned memory
	static bool NeedsDestructor()
	{
		return true;
	}
	static bool NeedsDefaultConstructor()
	{
		return true;
	}
	static bool NeedsCopyConstructor()
	{
		return true;
	}
	// returns true if this class is copied via "operator =" (i.e. it's not bitwise copyable via memcpy())
	static bool NeedsAssignment()
	{
		return true;
	}
	static SizeT GetAlignment()
	{
		return EFFICIENT_ALIGNMENT;
	}
#else
	// Plain Old Data types don't need constructors, destructors and assignment operators being called.
	// They can be memset and memcopied.
	enum
	{
		IsPlainOldDataType = false
	};
#endif
};

#if 0
TODO: type traits from Qt:
/*
/*
   Specialize a specific type with:

     Q_DECLARE_TYPEINFO(type, flags);

   where 'type' is the name of the type to specialize and 'flags' is
   logically-OR'ed combination of the flags below.
*/
enum { /* TYPEINFO flags */
    Q_COMPLEX_TYPE = 0,
    Q_PRIMITIVE_TYPE = 0x1,
    Q_STATIC_TYPE = 0,
    Q_MOVABLE_TYPE = 0x2,
    Q_DUMMY_TYPE = 0x4
};

#define Q_DECLARE_TYPEINFO(TYPE, FLAGS) \
template< > \
class QTypeInfo<TYPE > \
{ \
public: \
    enum { \
        isComplex = (((FLAGS) & Q_PRIMITIVE_TYPE) == 0), \
        isStatic = (((FLAGS) & (Q_MOVABLE_TYPE | Q_PRIMITIVE_TYPE)) == 0), \
        isLarge = (sizeof(TYPE)>sizeof(void*)), \
        isPointer = false, \
        isDummy = (((FLAGS) & Q_DUMMY_TYPE) != 0) \
    }; \
    static inline const char *name() { return #TYPE; } \
}

/*
   QTypeInfo primitive specializations
*/
Q_DECLARE_TYPEINFO(bool, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(char, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(signed char, Q_PRIMITIVE_TYPE);
#endif
// Helper macro to avoid typing such things:
/*
template<>
struct TypeTrait<BYTE>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<ANSICHAR>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<UNICODECHAR>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<INT>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<UINT>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<LONG>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<ULONG>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<FLOAT>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<DOUBLE>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<void>
{
	static bool IsPlainOldDataType() { return true; }
};
template<>
struct TypeTrait<void*>
{
	static bool IsPlainOldDataType() { return true; }
};
*/

#define mxDECLARE_POD_TYPE( TYPE )	\
	template< > struct TypeTrait< TYPE > {	\
		enum { IsPlainOldDataType = true };	\
	};	\
	template< > struct TypeTrait< const TYPE > {	\
		enum { IsPlainOldDataType = true };	\
	};	\
	FORCEINLINE AStreamWriter& operator << ( AStreamWriter &outStream, const TYPE& o )	\
	{	\
		outStream.Write( &o, sizeof TYPE );	\
		return outStream;	\
	}	\
	FORCEINLINE AStreamReader& operator >> ( AStreamReader& inStream, TYPE &o )	\
	{	\
		inStream.Read( &o, sizeof TYPE );	\
		return inStream;	\
	}	\
	mxIMPLEMENT_FUNCTION_READ_SINGLE( TYPE,Read_##TYPE );\
	mxIMPLEMENT_FUNCTION_WRITE_SINGLE( TYPE, Write_##TYPE );\
	template< class SERIALIZER >	\
	FORCEINLINE SERIALIZER& operator & ( SERIALIZER & serializer, TYPE & o )	\
	{\
		serializer.SerializeMemory( &o, sizeof TYPE );\
		return serializer;\
	}\
	FORCEINLINE mxArchive& Serialize( mxArchive & serializer, TYPE & o )\
	{\
		serializer.SerializeMemory( &o, sizeof TYPE );\
		return serializer;\
	}\
	FORCEINLINE mxArchive& operator && ( mxArchive & serializer, TYPE & o )\
	{\
		serializer.SerializeMemory( &o, sizeof TYPE );\
		return serializer;\
	}\
	FORCEINLINE void F_UpdateMemoryStats( MemStatsCollector& stats, const TYPE& o )\
	{\
		stats.staticMem += sizeof o;\
	}


mxDECLARE_POD_TYPE(ANSICHAR);
mxDECLARE_POD_TYPE(UNICODECHAR);
mxDECLARE_POD_TYPE(INT8);
mxDECLARE_POD_TYPE(UINT8);
mxDECLARE_POD_TYPE(INT16);
mxDECLARE_POD_TYPE(UINT16);
mxDECLARE_POD_TYPE(INT32);
mxDECLARE_POD_TYPE(UINT32);
mxDECLARE_POD_TYPE(INT64);
mxDECLARE_POD_TYPE(UINT64);
mxDECLARE_POD_TYPE(LONG);
mxDECLARE_POD_TYPE(ULONG);
mxDECLARE_POD_TYPE(FLOAT);
mxDECLARE_POD_TYPE(DOUBLE);

mxDECLARE_POD_TYPE(FileTime);



// Pointers are pod types
template< typename T >
struct TypeTrait<T*>
{
	enum { IsPlainOldDataType = true };
};

// Arrays
template< typename T, unsigned long N >
struct TypeTrait< T[N] >
	: public TypeTrait< T >
{};



// use for non-POD types that can be streamed via << and >> operators.
//
#define mxIMPLEMENT_SERIALIZE_FUNCTION( className, serializeFuncName )\
	FORCEINLINE mxArchive& serializeFuncName( mxArchive & serializer, className & o )\
	{\
		if( AStreamWriter* saver = serializer.IsWriter() )\
		{\
			*saver << o;\
		}\
		if( AStreamReader* loader = serializer.IsReader() )\
		{\
			*loader >> o;\
		}\
		return serializer;\
	}\
	FORCEINLINE mxArchive& operator && ( mxArchive & serializer, className & o )\
	{\
		return serializeFuncName( serializer, o );\
	}


// dummy function used for in-place saving/loading
#define mxNO_SERIALIZABLE_POINTERS\
	template< class S, class P  > inline void CollectPointers( S & s, P p ) const\
	{}

//
//	Hash traits.
//

//
//	THashTrait<T> - used for generating hash values.
//
//	Should be specialized for custom types.
//
template< typename key >
struct THashTrait
{
	static FORCEINLINE UINT GetHashCode( const key& key )
	{
		return mxGetHashCode( key );
	}
};

//
//	TEqualsTrait - used for comparing keys in hash tables.
//
//	Must be specialized for custom types.
//
template< typename key >
struct TEqualsTrait
{
	static FORCEINLINE bool Equals( const key& a, const key& b )
	{
		return (a == b);
	}
};

template< typename T >
struct THashTrait< T* >
{
	static FORCEINLINE UINT GetHashCode( const void* pointer )
	{
		return static_cast< UINT >( mxPointerHash( pointer ) );
	}
};
template<>
struct THashTrait< INT32 >
{
	static FORCEINLINE UINT GetHashCode( INT32 k )
	{
		return static_cast< UINT >( k );
	}
};
template<>
struct THashTrait< UINT32 >
{
	static FORCEINLINE UINT GetHashCode( UINT32 k )
	{
		return static_cast< UINT >( k );
	}
};
template<>
struct THashTrait< INT64 >
{
	static FORCEINLINE UINT GetHashCode( INT64 k )
	{
		return static_cast< UINT >( k );
	}
};
template<>
struct THashTrait< UINT64 >
{
	static FORCEINLINE UINT GetHashCode( UINT64 k )
	{
		return static_cast< UINT >( k );
	}
};

//-----------------------------------------------------------------------------
//	String hasher
//-----------------------------------------------------------------------------

template<>
struct THashTrait< const char* >
{
	static FORCEINLINE UINT GetHashCode( const char* k )
	{
		extern INLINE INT NameHash( const char *name );
		return static_cast< UINT >( NameHash(k) );
	}
};

// Forward declarations.
UINT mxGetHashCode( const String& str );

//
//	TBlob<T>
//
template< typename TYPE >
class TBlob
{
	BYTE	storage[ sizeof(TYPE) ];

public:

	FORCEINLINE TBlob()
	{}

	FORCEINLINE TBlob(EInitZero)
	{
		ZERO_OUT( storage );
	}

	FORCEINLINE TYPE* Ptr()
	{
		return c_cast(TYPE*)&storage;
	}
	FORCEINLINE const TYPE* Ptr () const
	{
		return c_cast(TYPE*)&storage;
	}

	FORCEINLINE TYPE& Get()
	{
		return *Ptr();
	}
	FORCEINLINE const TYPE& Get () const
	{
		return *Ptr();
	}

	FORCEINLINE operator TYPE& () { return Get(); }
	FORCEINLINE operator const TYPE& () const { return Get(); }

	FORCEINLINE TYPE* Construct()
	{
		return new (storage) TYPE();
	}
	FORCEINLINE void Destruct()
	{
		Get().~TYPE();
	}
};

//TAlignedBlob
// maybe, use something like std::aligned_storage< N * sizeof(T), alignof(T) > rawStorage; ?
// @todo: keep a bool flag, check for initialized on access via Get()/Ptr()?
//
template< typename TYPE >
class TBlob16
{
	mxALIGN_16( BYTE	storage[ sizeof(TYPE) ] );

public:

	FORCEINLINE TBlob16()
	{}

	FORCEINLINE TBlob16(EInitZero)
	{
		ZERO_OUT( storage );
	}

	FORCEINLINE TYPE* Ptr()
	{
		return c_cast(TYPE*)&storage;
	}
	FORCEINLINE const TYPE* Ptr () const
	{
		return c_cast(TYPE*)&storage;
	}

	FORCEINLINE TYPE& Get()
	{
		return *Ptr();
	}
	FORCEINLINE const TYPE& Get () const
	{
		return *Ptr();
	}

	FORCEINLINE operator TYPE& () { return Get(); }
	FORCEINLINE operator const TYPE& () const { return Get(); }

	FORCEINLINE TYPE* Construct()
	{
		return new (storage) TYPE();
	}
	FORCEINLINE void Destruct()
	{
		Get().~TYPE();
	}
};

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// supply class name and ctor arguments
// it uses placement new on a static array to avoid dynamic memory allocations
//
#define mxSTATIC_IN_PLACE_CTOR( KLASS, ... )\
	{\
		static TBlob16<KLASS>	KLASS##storage;\
		new( &KLASS##storage ) KLASS( ## __VA_ARGS__ );\
	}

//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
#define mxSTATIC_IN_PLACE_CTOR_X( PTR, KLASS, ... )\
	{\
		Assert( PTR == nil );\
		static TBlob16<KLASS>	KLASS##storage;\
		PTR = new( &KLASS##storage ) KLASS( ## __VA_ARGS__ );\
	}

//---------------------------------------------------------------------------

//
//	Triple
// 
template< typename T >
class Triple {
public:
	union {
		struct { T   iA, iB, iC; };
		struct { T   Points[ 3 ]; };
	};

	FORCEINLINE Triple()
	{}
	FORCEINLINE Triple( const T & A, const T & B, const T & C )
		: iA( A ), iB( B ), iC( C )
	{}
	FORCEINLINE T & operator [] ( mxUInt index )
	{
		//Assert( index >= 0 && index <= 3 );
		return Points[ index ];
	}
	FORCEINLINE const T & operator [] ( mxUInt index ) const
	{
		//Assert( index >= 0 && index <= 3 );
		return Points[ index ];
	}
};


/*
====================================================================
	
	Numeric Limits

====================================================================
*/

mxSWIPED("oooii");

// @oooii-tony: Using <limits> can conflict with macro definitions of min and
// max, so unfortunately, don't use it and use this lame renamed under-featured
// version.

// @oooii-tony: Another lame thing. std::numeric_limit<float>::min() is a
// positive value while std::numeric_limit<int>::min() is a negative value, so
// you can't naively make a template with Min and Max members and set it with
// std::numeric_limit because it won't behave well with signed numbers in both
// float and int cases.

template<typename T> class oNumericLimits
{
public:
	static T GetMin() { return T(0); }
	static T GetMax() { return T(0); }
	static T GetEpsilon() { return T(0); }
	static T GetRoundError() { return T(0); }

	static T GetSignedMin() { return T(0); }
	static T GetSmallestNonZeroMagnitude() { return T(0); }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};

template<> class oNumericLimits<float>
{
public:
	typedef float T;
	static T GetMin() { return FLT_MIN; };
	static T GetMax() { return FLT_MAX; };
	static T GetEpsilon() { return FLT_EPSILON; }
	static T GetRoundError() { return 0.5f; }

	static T GetSignedMin() { return -GetMax(); }
	static T GetSmallestNonZeroMagnitude() { return GetMin(); }

	static unsigned int GetNumMantissaBits() { return FLT_MANT_DIG; }
	static unsigned int GetNumPrecisionDigits() { return FLT_DIG; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<double>
{
public:
	typedef double T;
	static T GetMin() { return DBL_MIN; };
	static T GetMax() { return DBL_MAX; };
	static T GetEpsilon() { return DBL_EPSILON; }
	static T GetRoundError() { return 0.5; }

	static T GetSignedMin() { return -GetMax(); }
	static T GetSmallestNonZeroMagnitude() { return GetMin(); }

	static unsigned int GetNumMantissaBits() { return DBL_MANT_DIG; }
	static unsigned int GetNumPrecisionDigits() { return DBL_DIG; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<long double>
{
public:
	typedef double T;
	static T GetMin() { return LDBL_MIN; };
	static T GetMax() { return LDBL_MAX; };
	static T GetEpsilon() { return LDBL_EPSILON; }
	static T GetRoundError() { return 0.5; }

	static T GetSignedMin() { return -GetMax(); }
	static T GetSmallestNonZeroMagnitude() { return GetMin(); }

	static unsigned int GetNumMantissaBits() { return LDBL_MANT_DIG; }
	static unsigned int GetNumPrecisionDigits() { return LDBL_DIG; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<char>
{
public:
	typedef char T;
	static T GetMin() { return SCHAR_MIN; };
	static T GetMax() { return SCHAR_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return GetMin(); }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<unsigned char>
{
public:
	typedef unsigned char T;
	static T GetMin() { return 0; };
	static T GetMax() { return UCHAR_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return 0; }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};

template<> class oNumericLimits<short>
{
public:
	typedef short T;
	static T GetMin() { return SHRT_MIN; };
	static T GetMax() { return SHRT_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return GetMin(); }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<unsigned short>
{
public:
	typedef unsigned short T;
	static T GetMin() { return 0; };
	static T GetMax() { return USHRT_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return 0; }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};

template<> class oNumericLimits<int>
{
public:
	typedef int T;
	static T GetMin() { return INT_MIN; };
	static T GetMax() { return INT_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return GetMin(); }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<unsigned int>
{
public:
	typedef unsigned int T;
	static T GetMin() { return 0; };
	static T GetMax() { return UINT_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return 0; }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};

template<> class oNumericLimits<long>
{
public:
	typedef long T;
	static T GetMin() { return LONG_MIN; };
	static T GetMax() { return LONG_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return GetMin(); }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<unsigned long>
{
public:
	typedef unsigned long T;
	static T GetMin() { return 0; };
	static T GetMax() { return ULONG_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return 0; }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};

template<> class oNumericLimits<long long>
{
public:
	typedef long long T;
	static T GetMin() { return LLONG_MIN; };
	static T GetMax() { return LLONG_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return GetMin(); }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return true; }
};

template<> class oNumericLimits<unsigned long long>
{
public:
	typedef unsigned long long T;
	static T GetMin() { return 0; };
	static T GetMax() { return ULLONG_MAX; };
	static T GetEpsilon() { return 0; }
	static T GetRoundError() { return 0; }

	static T GetSignedMin() { return 0; }
	static T GetSmallestNonZeroMagnitude() { return 1; }

	static unsigned int GetNumMantissaBits() { return 0; }
	static unsigned int GetNumPrecisionDigits() { return 0; }
	static bool IsSigned() { return false; }
};





mxSWIPED("http://multiturret.googlecode.com/svn");

template<typename TYPE, UINT Size>
class PingPongBuffer
{
	UINT iBegin,iEnd;

	TYPE mBuffer[Size];

	UINT mRealSize;

public:
	PingPongBuffer()
		: iBegin(0),iEnd(0),mRealSize(0)
	{}

	void push_back(const TYPE& value)
	{
		mBuffer[iEnd] = value;
		iEnd = (++iEnd >= Size)?0:iEnd;
		++mRealSize;
		Assert(mRealSize <= Size);
	}
	void pop_front()
	{
		Assert(mRealSize > 0);
		iBegin = (++iBegin >= Size)? 0:iBegin;
		--mRealSize;
	}

	const TYPE& operator[](UINT index)
	{
		Assert(index < Size);
		UINT realIndex = iBegin + index;
		if(realIndex >= Size )
		{
			realIndex = index - (Size - iBegin);
		}

		return mBuffer[realIndex];
	}

private:
	PREVENT_COPY(PingPongBuffer);
};

/*
-----------------------------------------------------------------------------
	UserDataPointer
-----------------------------------------------------------------------------
*/
class UserDataPointer
{
	void *	mUserDataPtr;

public:
	FORCEINLINE UserDataPointer()
	{
		mUserDataPtr = nil;
	}
	FORCEINLINE void SetUserData( void* p )
	{
		mUserDataPtr = p;
	}
	FORCEINLINE void* GetUserData()
	{
		return mUserDataPtr;
	}
	FORCEINLINE const void* GetUserData() const
	{
		return mUserDataPtr;
	}
};

template< typename A, typename B >
static FORCEINLINE void CopyPOD( A &dest, const B& src )
{
	mxSTATIC_ASSERT( sizeof dest == sizeof src );
	MemCopy( &dest, &src, sizeof dest );
}

// base class for objects that need to be initialized
//
struct Initializable
{
protected:

	void OneTimeInit()
	{
#if MX_DEBUG
		Assert(!m__hasBeenIsInitialized);
		m__hasBeenIsInitialized = true;
#endif // MX_DEBUG
	}

	void OneTimeDestroy()
	{
#if MX_DEBUG
		Assert(m__hasBeenIsInitialized);
		m__hasBeenIsInitialized = false;
#endif // MX_DEBUG
	}

	void CheckInitialized()
	{
#if MX_DEBUG
		mxBREAK_IF( !m__hasBeenIsInitialized );
#endif // MX_DEBUG
	}

	Initializable()
	{
#if MX_DEBUG
		m__hasBeenIsInitialized = false;
#endif // MX_DEBUG
	}

	~Initializable()
	{
#if MX_DEBUG
		Assert(!m__hasBeenIsInitialized);
#endif // MX_DEBUG
	}

private:
#if MX_DEBUG
	bool	m__hasBeenIsInitialized;
#endif // MX_DEBUG
};


/*
-----------------------------------------------------------------------------
	NiftyCounter

	Intent:
		Ensure a non-local static object is initialized before its first use
		and destroyed only after last use of the object.

	Also Known As:
		Schwarz Counter.

	To do:
		Locking policies for thread safety.
-----------------------------------------------------------------------------
*/
class NiftyCounter
{
	// this tells how many times the base system has been requested to initialize
	// (think 'reference-counting')
	//
	volatile int	niftyCounter;

public:
	NiftyCounter()
	{
		this->niftyCounter = 0;
	}
	~NiftyCounter()
	{
		Assert( this->niftyCounter == 0 );
	}

	//-----------------------------------------------------
	// returns true if one time initialization is needed
	bool IncRef()
	{
		this->niftyCounter++;

		if( this->niftyCounter > 1 )
		{
			// already initialized
			return false;
		}

		// has been initialized for the first time

		Assert(this->niftyCounter == 1);

		return true;
	}
	//-----------------------------------------------------
	// returns true if one time destruction is needed
	bool DecRef()
	{
		Assert(this->niftyCounter > 0);

		this->niftyCounter--;

		if( this->niftyCounter > 0 )
		{
			// still referenced, needs to stay initialized
			return false;
		}

		Assert(this->niftyCounter == 0);

		// no external references, can shut down

		return true;
	}
	//-----------------------------------------------------
	int NumRefs() const
	{
		return this->niftyCounter;
	}
	//-----------------------------------------------------
	bool IsOpen() const
	{
		return this->NumRefs() > 0;
	}
};



template< typename TYPE >
static inline
void TMemCopyArray( TYPE *dest, const TYPE* src, const UINT numItems )
{
	AssertPtr( dest );
	AssertPtr( src );
	Assert( dest != src );
	Assert( numItems > 0 );
	MemCopy( dest, src, numItems * sizeof src[0] );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

namespace Array_Util
{
	// Returns true if the index is within the boundaries of this array.
	FORCEINLINE bool IsValidIndex( UINT index, const UINT num )
	{
		return (index >= 0) && (index < num);
	}

	template< class ARRAY, typename TYPE >
	bool RemoveElement( ARRAY & theArray, const TYPE& item )
	{
		const INT index = theArray.FindIndexOf( item );
		if( INDEX_NONE == index ) {
			return false;
		}
		theArray.RemoveAt( index );
		return true;
	}

	// Slow!
	template< typename TYPE, typename SIZETYPE >
	void RemoveAt(
		TYPE* theData, SIZETYPE& theNum,
		const UINT startIndex, const UINT count = 1 )
	{
		const UINT oldNum = theNum;

		Assert( IsValidIndex( startIndex, oldNum ) );
		Assert( IsValidIndex( startIndex + count - 1, oldNum ) );


		TYPE* data = theData;
		TYPE* dest = data + startIndex;
		TYPE* src = dest + count;

		// Destroy the elements.
		TDestructN_IfNonPOD( dest, count );

		const UINT numMovedElements = oldNum - startIndex - count;
		TMoveArray( dest, src, numMovedElements );

		theNum -= count;
	}

/*
	// Slow!
	// removes the element from the array, doesn't call it's destructor.
	// it basically wipes out this object with shifted elements.
	// use it only if you know what're doing!
	//
	template< typename TYPE, typename SIZETYPE >
	void RemoveAt_DontCallDtor(
		TYPE* theData, SIZETYPE& theNum,
		const UINT index, const UINT count = 1 )
	{
		const UINT oldNum = theNum;

		Assert( IsValidIndex( index, oldNum ) );
		Assert( IsValidIndex( index + count - 1, oldNum ) );

		TYPE* data = theData;
		TYPE* dest = data + index;
		TYPE* src = dest + count;

		const UINT numBytes = (oldNum - index - count) * (sizeof TYPE);
		MemMove( dest, src, numBytes );

		theNum -= count;
	}
*/

	// this method is faster (uses the 'swap trick')
	// Doesn't preserve the relative order of elements.
	//
	template< typename TYPE, typename SIZETYPE >
	FORCEINLINE void RemoveAt_Fast(
		TYPE* theData, SIZETYPE& theNum,
		const UINT index )
	{
		Assert( IsValidIndex( index, theNum ) );

		TYPE* data = theData;
		const UINT num = --theNum;

		// swap with last element, and destroy the last element
		data[ index ] = data[ num ];

		TDestructOne_IfNonPOD( data[ num ] );
	}

	// deletes the last element
	template< typename TYPE, typename SIZETYPE >
	FORCEINLINE void PopBack(
		TYPE* theData, SIZETYPE& theNum
		)
	{
		--theNum;

		// remove and destroy the last element
		TDestructOne_IfNonPOD( theData[ theNum ] );
	}
}


/*
-----------------------------------------------------------------------------
	TArrayBase

	this mixin implements common array functions,
	it doesn't contain any member variables.

	the derived class must have the following functions
	(you'll get a compile-time error or stack overflow if it does't):

	'UINT Num()'- returns number of elements in the array
	'UINT GetCapacity()'- returns the current capacity of the array
	'TYPE* ToPtr()' - returns a pointer to the contents

	@todo: check if it causes any code bloat
-----------------------------------------------------------------------------
*/
template< typename TYPE, class DERIVED >
struct TArrayBase
{
	typedef TypeTrait
	<
		TYPE
	>
	TYPE_TRAIT;

public:
	FORCEINLINE DERIVED* AsDerived()
	{
		return static_cast<DERIVED*>(this);
	}
	FORCEINLINE const DERIVED* AsDerived() const
	{
		return static_cast<const DERIVED*>(this);
	}

	// Convenience function to get the number of elements in this array.
	// Returns the size (the number of elements in the array).
	FORCEINLINE UINT Num() const
	{
		return this->AsDerived()->Num();
	}
	// Returns the current capacity of this array.
	FORCEINLINE UINT GetCapacity() const
	{
		return this->AsDerived()->GetCapacity();
	}

	FORCEINLINE TYPE* ToPtr()
	{
		return this->AsDerived()->ToPtr();
	}
	FORCEINLINE const TYPE* ToPtr() const
	{
		return this->AsDerived()->ToPtr();
	}

public:

	// Checks if the size is zero.
	FORCEINLINE bool IsEmpty() const
	{
		return !this->Num();
	}

	FORCEINLINE bool NonEmpty() const
	{
		return this->Num();
	}

	// Returns the size of a single element, in bytes.
	FORCEINLINE UINT GetItemSize() const
	{
		//return sizeof this->ToPtr()[0];
		return sizeof(TYPE);
	}

	//SizeT GetItemSize() const

	// Returns the total size of stored elements, in bytes.
	FORCEINLINE SizeT GetDataSize() const
	{
		return this->Num() * this->GetItemSize();
	}

	// Returns true if the index is within the boundaries of this array.
	FORCEINLINE bool IsValidIndex( UINT index ) const
	{
		return /*(index >= 0) && */(index < this->Num());
	}

	FORCEINLINE TYPE & GetFirst()
	{
		Assert(this->Num()>0);
		return this->ToPtr()[ 0 ];
	}
	FORCEINLINE const TYPE& GetFirst() const
	{
		Assert(this->Num()>0);
		return this->ToPtr()[ 0 ];
	}

	FORCEINLINE TYPE & GetLast()
	{
		Assert(this->Num()>0);
		return this->ToPtr()[ this->Num()-1 ];
	}
	FORCEINLINE const TYPE& GetLast() const
	{
		Assert(this->Num()>0);
		return this->ToPtr()[ this->Num()-1 ];
	}

	FORCEINLINE void SetLast( const TYPE& newValue )
	{
		Assert(this->Num()>0);
		this->ToPtr()[ this->Num()-1 ] = newValue;
	}

	// Returns the index of the first occurrence of the given element, or INDEX_NONE if not found.
	// Slow! (uses linear search)
	UINT FindIndexOf( const TYPE& element ) const
	{
		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( arr[ i ] == element ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	bool Contains( const TYPE& element ) const
	{
		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( arr[ i ] == element ) {
				return true;
			}
		}
		return false;
	}

	// assumes that this array stores pointers
	template< typename U >
	INT FindPtrIndex( const U* pointer ) const
	{
		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( arr[ i ] == pointer ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Returns index of the last occurrence of the given element, or INDEX_NONE if not found.
	INT LastIndexOf( const TYPE& element ) const
	{
		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for(UINT i = num-1; i >= 0; i++) {
			if( arr[ i ] == element ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Searches for the given element and returns a pointer to it, or NULL if not found.
	TYPE* Find( const TYPE& element )
	{
		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( arr[ i ] == element ) {
				return &(arr[ i ]);
			}
		}
		return nil;
	}

	UINT CountOf( const TYPE& element ) const
	{
		UINT counter = 0;

		const UINT num = this->Num();
		const TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( arr[ i ] == element ) {
				counter++;
			}
		}
		return counter;
	}

	// Read/write access to the i'th element.
	FORCEINLINE TYPE & operator [] ( UINT i )
	{
		Assert( this->IsValidIndex( i ) );
		return this->ToPtr()[ i ];
	}
	// Read only access to the i'th element.
	FORCEINLINE const TYPE& operator [] ( UINT i ) const
	{
		Assert( this->IsValidIndex( i ) );
		return this->ToPtr()[ i ];
	}

	// Returns null if the index is out of array bounds.
	FORCEINLINE TYPE* SafeGetItemPtr( UINT index )
	{
		return this->IsValidIndex( index ) ? this->ToPtr() + index : nil;
	}
	// use it with pointer types only
	FORCEINLINE TYPE SafeGetItemRef( UINT index )
	{
		return this->IsValidIndex( index ) ? this->ToPtr()[ index ] : (TYPE)nil;
	}
	FORCEINLINE TYPE* SafeGetFirstItemPtr()
	{
		return ( this->Num() > 0 ) ? this->ToPtr() : nil;
	}

	FORCEINLINE TYPE & At( UINT index )
	{
		Assert( this->IsValidIndex( index ) );
		return this->ToPtr()[ index ];
	}
	FORCEINLINE const TYPE& At( UINT index ) const
	{
		Assert( this->IsValidIndex( index ) );
		return this->ToPtr()[ index ];
	}

	FORCEINLINE TYPE * GetItemPtr( UINT index )
	{
		Assert( this->IsValidIndex( index ) );
		return this->ToPtr() + index;
	}
	FORCEINLINE const TYPE* GetItemPtr( UINT index ) const
	{
		Assert( this->IsValidIndex( index ) );
		return this->ToPtr() + index;
	}

	FORCEINLINE UINT GetItemIndex( const TYPE* o ) const
	{
		const TYPE* start = this->ToPtr();
		const UINT index = o - start;
		Assert( this->IsValidIndex( index ) );
		// @fixme: ideally, it should be:
		// (ptrdiff_t)((size_t)o - (size_t)start) / sizeof(TYPE);
		return index;
	}


	FORCEINLINE void ZeroOut()
	{
		//Assert(this->Num()>0);
		MemZero( this->ToPtr(), this->GetDataSize() );
	}

	//Use with care!
	inline void CopyFrom( const void* src, SizeT numBytes )
	{
		MemCopy( this->ToPtr(), src, numBytes );
	}

	template< typename U, SizeT N >
	inline void CopyFromArray( const U (&src)[N] )
	{
		MemCopy( this->ToPtr(), src, sizeof src );
	}

	// Set all items of this array to the given value.
	void SetAll( const TYPE& theValue )
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		for( SizeT i = 0; i < num; ++i )
		{
			arr[ i ] = theValue;
		}
	}


public:	// Iterators and Algorithms.

	typedef TYPE* Iterator;

	FORCEINLINE Iterator GetStart() const { return this->ToPtr(); }
	FORCEINLINE Iterator GetEnd() const { return this->ToPtr() + this->Num(); }

	// Algorithms.

	template< class FUNCTOR >
	void Do_ForEach( FUNCTOR& functor, UINT startIndex, UINT endIndex )
	{
		Assert(this->IsValidIndex(startIndex));
		Assert(this->IsValidIndex(endIndex));

		for( UINT i = startIndex; i < endIndex; i++ ) {
			functor( (this*)[ i ] );
		}
	}

	template< class FUNCTOR >
	void Do_ForAll( FUNCTOR& functor )
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			functor( arr[ i ] );
		}
	}

	template< class FUNCTOR >
	TYPE* Do_FindFirst( FUNCTOR& functor )
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( functor( arr[i] ) ) {
				return &arr[i];
			}
		}
		return nil;
	}

	template< class FUNCTOR >
	INT Do_FindFirstIndex( FUNCTOR& functor )
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			if( functor( arr[i] ) ) {
				return i;
			}
		}
		return INDEX_NONE;
	}

	// Invokes objects' destructors.
	void DestroyContents()
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		TDestructN_IfNonPOD( arr, num );
	}

	// assuming that the stored elements are pointers,
	// deletes them.
	// NOTE: don't forget to empty the array afterwards, if needed.
	//
	void DeleteContents()
	{
		const UINT num = this->Num();
		TYPE* arr = this->ToPtr();

		for( UINT i = 0; i < num; i++ ) {
			delete arr[i];
		}
	}

	//TODO: sorting, binary search, algorithms & iterators

	template< class COMPARATOR >
	void Sort_Insertion( COMPARATOR compare, UINT startIndex, UINT endIndex )
	{
		TYPE * lo = &((*this)[startIndex]);
		TYPE * hi = &((*this)[endIndex]);
		TYPE * best;
		TYPE * p;

		while( hi > lo )
		{
			best = lo;
			for( p = lo+1; p <= hi; p++ ) {
				if( compare( *p, *best ) ) {
					best = p;
				}
			}
			TSwap(*best,*hi);
			hi--;
		}
	}
};


//------------------------------------------------------------------------------
/**
    @class Util::Blob
    
    The Util::Blob class encapsulates a chunk of raw memory into 
    a C++ object which can be copied, compared and hashed.
    
    (C) 2006 Radon Labs GmbH
*/
mxSWIPED("Nebula 3");
class MemoryBlob
	: public TArrayBase< BYTE, MemoryBlob >
{
	//mxDECLARE_CLASS_ALLOCATOR( EMemHeap::HeapTemp, Blob );

public:
	/// default constructor
	explicit MemoryBlob( HMemory heap );
	/// constructor
	explicit MemoryBlob( HMemory heap, const void* ptr, SizeT size);
	/// reserve N bytes
	explicit MemoryBlob( HMemory heap, SizeT size);
	/// copy constructor
	explicit MemoryBlob( HMemory heap, const MemoryBlob& rhs);
	/// destructor
	~MemoryBlob();
	/// assignment operator
	void operator=(const MemoryBlob& rhs);

	/// equality operator
	bool operator==(const MemoryBlob& rhs) const;
	/// inequality operator
	bool operator!=(const MemoryBlob& rhs) const;
	/// greater operator
	bool operator>(const MemoryBlob& rhs) const;
	/// less operator
	bool operator<(const MemoryBlob& rhs) const;
	/// greater-equal operator
	bool operator>=(const MemoryBlob& rhs) const;
	/// less-eqial operator
	bool operator<=(const MemoryBlob& rhs) const;

	/// return true if the blob contains data
	bool IsValid() const;
	/// reserve N bytes
	void Reserve(SizeT size);
	/// trim the size member (without re-allocating!)
	void Trim(SizeT size);
	/// set blob contents
	void Set(const void* ptr, SizeT size);

	/// get blob size
	SizeT GetDataSize() const;
	/// get a hash code (compatible with Util::HashTable)
	IndexT HashCode() const;

	void SetSize( SizeT size );

public:	//=== TArrayBase

	inline UINT Num() const
	{
		return this->size;
	}
	inline UINT GetCapacity() const
	{
		return this->allocSize;
	}
	inline BYTE* ToPtr()
	{
		Assert(this->IsValid());
		return static_cast<BYTE*>(this->ptr);
	}
	inline const BYTE* ToPtr() const
	{
		Assert(this->IsValid());
		return static_cast<const BYTE*>(this->ptr);
	}

public:
	friend AStreamWriter& operator << ( AStreamWriter& file, const MemoryBlob & o );
	friend AStreamReader& operator >> ( AStreamReader& file, MemoryBlob & o );
	friend mxArchive& operator && ( mxArchive& archive, MemoryBlob & o );

private:
	/// delete content
	void Delete();
	/// allocate internal buffer
	void Allocate(SizeT size);
	// grow if needed
	void Reallocate( SizeT newSize );
	/// copy content
	void Copy(const void* ptr, SizeT size);
	/// do a binary comparison between this and other blob
	int BinaryCompare(const MemoryBlob& rhs) const;

private:
	// data allocated from memory heap
	void* ptr;
	SizeT size;
	SizeT allocSize;
	const HMemory memHeap;
};

/*
-----------------------------------------------------------------------------
	MemoryBlobWriter
-----------------------------------------------------------------------------
*/

class MemoryBlobWriter : public AStreamWriter
{
	MemoryBlob & m_memBlob;
	SizeT m_currOffset;

public:
	MemoryBlobWriter( MemoryBlob & memBlob )
		: m_memBlob( memBlob )
	{
		this->DbgSetName( "MemoryBlobWriter" );
		m_currOffset = 0;
	}
	~MemoryBlobWriter()
	{
	}

	virtual SizeT Write( const void *pSrc, SizeT numBytes ) override
	{
		const SizeT totalBytes =  m_currOffset + numBytes;
		m_memBlob.Reserve( totalBytes );
		MemCopy( m_memBlob.ToPtr() + m_currOffset, pSrc, numBytes );
		m_currOffset = totalBytes;
		return numBytes;
	}

	inline const BYTE* ToPtr() const
	{
		return m_memBlob.ToPtr();
	}
	inline BYTE* ToPtr()
	{
		return m_memBlob.ToPtr();
	}

	inline SizeT Tell() const
	{
		return m_currOffset;
	}

	inline void Rewind()
	{
		m_currOffset = 0;
	}
	inline void Seek( SizeT absOffset )
	{
		m_memBlob.Reserve( absOffset );
		m_currOffset = absOffset;
	}

private:	PREVENT_COPY( MemoryBlobWriter );
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



template< class TYPE >
struct TCountedObject
{
	inline TCountedObject()
		: m__serialNumber( ms__numInstances++ )
	{
	}

	const UINT	m__serialNumber;
private:
	static UINT	ms__numInstances;
};
template< typename TYPE >
UINT TCountedObject< TYPE >::ms__numInstances = 0;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

mxSWIPED("Valve's Source Engine/ L4D 2 SDK");
//-----------------------------------------------------------------------------
// Declares a type-safe handle type; you can't assign one handle to the next
//-----------------------------------------------------------------------------

// 32-bit pointer handles.

// Typesafe 8-bit and 16-bit handles.
template< class HandleType >
class CBaseIntHandle
{
public:
	
	FORCEINLINE bool	operator==( const CBaseIntHandle &other )	{ return m_Handle == other.m_Handle; }
	FORCEINLINE bool	operator!=( const CBaseIntHandle &other )	{ return m_Handle != other.m_Handle; }

	// Only the code that doles out these handles should use these functions.
	// Everyone else should treat them as a transparent type.
	FORCEINLINE HandleType	GetHandleValue() const				{ return m_Handle; }
	FORCEINLINE void		SetHandleValue( HandleType val )	{ m_Handle = val; }

	typedef HandleType	HANDLE_TYPE;

protected:

	HandleType	m_Handle;
};

template< class DummyType >
class CIntHandle16 : public CBaseIntHandle< UINT16 >
{
public:
	inline			CIntHandle16() {}

	static inline	CIntHandle16<DummyType> MakeHandle( HANDLE_TYPE val )
	{
		return CIntHandle16<DummyType>( val );
	}

protected:
	inline			CIntHandle16( HANDLE_TYPE val )
	{
		m_Handle = val;
	}
};


template< class DummyType >
class CIntHandle32 : public CBaseIntHandle< UINT32 >
{
public:
	inline			CIntHandle32() {}

	static inline	CIntHandle32<DummyType> MakeHandle( HANDLE_TYPE val )
	{
		return CIntHandle32<DummyType>( val );
	}

protected:
	inline			CIntHandle32( HANDLE_TYPE val )
	{
		m_Handle = val;
	}
};


// NOTE: This macro is the same as windows uses; so don't change the guts of it
#define DECLARE_HANDLE_16BIT(name)	typedef CIntHandle16< struct name##__handle * > name;
#define DECLARE_HANDLE_32BIT(name)	typedef CIntHandle32< struct name##__handle * > name;

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name






// The runtime unique identifier assigned to each object by the corresponding Object System.
// ObjectHandle may not be the same when saving/loading.
// ObjectHandle is mostly used in runtime for fast and unique identification of objects.
//
typedef U4 ObjectHandle;

// Use INDEX_NONE to indicate invalid ObjectHandle's.

// Object ID
class OID
{
	U4	m_handleValue;

public:
	FORCEINLINE OID()
	{}
	FORCEINLINE explicit OID( UINT i )
	{ m_handleValue = i; }

	FORCEINLINE bool operator == ( UINT i ) const
	{ return m_handleValue == i; }

	FORCEINLINE bool operator != ( UINT i ) const
	{ return m_handleValue != i; }

	// Only the code that doles out these handles should use these functions.
	// Everyone else should treat them as a transparent type.
	FORCEINLINE U4		GetHandleValue() const			{ return m_handleValue; }
	FORCEINLINE void	SetHandleValue( U4 newValue )	{ m_handleValue = newValue; }
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



// type of object is stored in 8 bits
struct TypedHandleBase
{
	union	// arrrgh... union cannot be used as a base class so we wrap it in a struct.
	{
		struct
		{
			BITFIELD	index : 24;
			BITFIELD	type : 8;
		};
		U4		value;
	};
};

mxSTATIC_ASSERT(sizeof TypedHandleBase == sizeof U4);



template< typename TYPE, typename ENUM, int INVALID_VALUE = -1 >
struct TypedHandle : TypedHandleBase
{
	typedef TypedHandle<TYPE,ENUM,INVALID_VALUE>	THIS_TYPE;

	FORCEINLINE TypedHandle()
	{
	}
	FORCEINLINE TypedHandle( ENUM eType, UINT nIndex )
	{
		this->index = nIndex;
		this->type = eType;
	}

	// NOTE: implicit conversions are intentionally disabled
	//
#if 0
	T& operator*();
	T* operator->();
	const T& operator*() const;
	const T* operator->() const;
#endif

	// Dereferencing.

	FORCEINLINE TYPE* ToPtr() const
	{
		return TYPE::Static_GetPointerByHandle( *this );
	}

	FORCEINLINE ENUM GetType() const
	{
		return static_cast<ENUM>( this->type );
	}

	FORCEINLINE void operator = ( const THIS_TYPE& other )
	{
		this->value = other.value;
	}
	FORCEINLINE bool operator == ( const THIS_TYPE& other ) const
	{
		this->value == other.value;
	}
	FORCEINLINE bool operator != ( const THIS_TYPE& other ) const
	{
		this->value != other.value;
	}

	FORCEINLINE bool IsValid() const
	{
		return this->value != INVALID_VALUE;
	}
	FORCEINLINE void SetInvalid()
	{
		this->value = INVALID_VALUE;
	}
};




//== MACRO =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// implements common comparison operators
// so that the handles can be tested against each other
//
#define mxIMPLEMENT_HANDLE( KLASS, VAR )\
	FORCEINLINE bool operator != ( const KLASS& other ) const\
	{\
		return this->VAR != other.VAR;\
	}\
	FORCEINLINE bool operator == ( const KLASS& other ) const\
	{\
		return this->VAR == other.VAR;\
	}\
	FORCEINLINE bool operator < ( const KLASS& other ) const\
	{\
		return this->VAR < other.VAR;\
	}\
	FORCEINLINE bool operator > ( const KLASS& other ) const\
	{\
		return this->VAR > other.VAR;\
	}\
	FORCEINLINE bool operator <= ( const KLASS& other ) const\
	{\
		return this->VAR <= other.VAR;\
	}\
	FORCEINLINE bool operator >= ( const KLASS& other ) const\
	{\
		return this->VAR >= other.VAR;\
	}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// 'Tagged' pointer which uses lower 4 bits for storing additional data.
// NOTE: the pointer must be 16-byte aligned!
//
template< typename TYPE >
class TypedPtr16
{
	mxULong		m_ptr;	// 4 LSBs store user tag

public:
	enum { PTR_MASK = (mxULong)((-1)<<4) };
	enum { TAG_MASK = ~PTR_MASK };

	FORCEINLINE void Set( TYPE* ptr, UINT tag )
	{
		Assert(IS_16_BYTE_ALIGNED( ptr ));
		Assert( tag < (1<<4) );	// must fit into 4 bits

		const mxULong ptrAsInt = (mxULong)ptr;
		m_ptr = (ptrAsInt) | (tag);
	}
	FORCEINLINE TYPE* GetPtr() const
	{
		return (TYPE*)(m_ptr & PTR_MASK);
	}
	FORCEINLINE UINT GetTag() const
	{
		return (m_ptr & TAG_MASK);
	}
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class T >
bool SafeOpen( T & o )
{
	if( !o.IsOpen() )
	{
		return o.Open();
	}
	return true;
}

template< class T >
void SafeClose( T & o )
{
	if( o.IsOpen() )
	{
		o.Close();
	}
}


mxNAMESPACE_END

#endif // !__MX_BASE_TYPES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
