/*
=============================================================================
	File:	StaticString.h
	Desc:	Compile-time strings.
	Note:	Don't use this with empty strings ("") !
			Empty strings are usually not needed, anyway.
=============================================================================
*/

#ifndef __MX_COMMON_STATIC_STRING_H__
#define __MX_COMMON_STATIC_STRING_H__


// enable this to check for hash collisions (i.e. different strings with equal hash keys)
#define MX_CHECK_STATIC_STR_HASH_COLLISIONS	(MX_DEBUG)


mxNAMESPACE_BEGIN

mxSWIPED("The code is based on work done by Sergey Makeev, http://mail@sergeymakeev.com");

namespace StaticString_Util
{
	const UINT MX_STATIC_STR_HASH_MAGIC_NUMBER = 1315423911;

	FORCEINLINE void HashCombine( UINT & hash, char c )
	{
		hash ^= ((hash << 5) + c + (hash >> 2));
	}

	FORCEINLINE UINT VPCALL StringHashRunTime( const char* str, ULONG & length )
	{
		length = 0;

		AssertPtr(str);

		UINT hash = MX_STATIC_STR_HASH_MAGIC_NUMBER;
		const char * char_ptr = str;

		while( *char_ptr )
		{
			char c = *char_ptr++;
			HashCombine( hash, c );
		}

		length = (ULONG)(char_ptr - str);

		return hash;
	}

	template< INT LENGTH >
	class StringHashCompileTime
	{
	public:
		static FORCEINLINE void VPCALL Build( UINT & hash, INT strLen, const char* input )
		{
			char c = input[ strLen - LENGTH ];
			HashCombine( hash, c );
			StringHashCompileTime< LENGTH-1 >::Build( hash, strLen, input );
		}
	};
	template<>
	class StringHashCompileTime< 0 >
	{
	public:
		static FORCEINLINE void VPCALL Build( UINT & hash, INT strLen, const char* input )
		{
			char c = input[ strLen ];
			HashCombine( hash, c );
		}
	};

}//StaticString_Util

/*
-----------------------------------------------------------------------------
	StaticString
-----------------------------------------------------------------------------
*/
class StaticString
{
protected_internal:
	char const* const	mPtr;
	const ULONG			mSize;
	UINT				mHash;

public_internal:

	FORCEINLINE StaticString( const StaticString& other )
		: mPtr( other.mPtr )
		, mSize( other.mSize )
		, mHash( other.mHash )
	{}

	// 'run-time' constructor
	FORCEINLINE explicit StaticString( const char* str, UINT length, UINT hash )
		: mPtr( str )
		, mSize( length )
		, mHash( hash )
	{}

public:

	// 'compile-time' constructor
	template< INT N >
	FORCEINLINE StaticString( const char (&str)[N] )
		: mPtr( str )
		, mSize( N-1 )
	{
		mxSTATIC_ASSERT( N > 0 );	// disallow empty string ("")

		using namespace StaticString_Util;
		mHash = MX_STATIC_STR_HASH_MAGIC_NUMBER;

		// a small fix for strings which are less than 2 characters in length (e.g. "!")
		if( N < 2 ) {
			StringHashCompileTime< N-1 >::Build( mHash, N-1, str );
		} else {
			StringHashCompileTime< N-2 >::Build( mHash, N-2, str );
		}
	}

	FORCEINLINE ULONG GetSize() const
	{
		return mSize;
	}

	FORCEINLINE const char* c_str() const
	{
		return mPtr;
	}
	FORCEINLINE const char* ToChars() const
	{
		return mPtr;
	}

	FORCEINLINE UINT GetHash() const
	{
		return mHash;
	}

	FORCEINLINE bool Equals( const StaticString& other ) const
	{
	#if MX_CHECK_STATIC_STR_HASH_COLLISIONS

		if( (GetSize() != other.GetSize()) || (GetHash() != other.GetHash()) )
		{
			return false;
		}

		// If a hash collision did occur then try using another hash function or gperf.
		// See:
		// http://blog.gamedeff.com/?p=115
		// http://gnuwin32.sourceforge.net/packages/gperf.htm

		AssertX( mxStrEquAnsi( c_str(), other.c_str() ), "Need to update hash function, collisions must be resolved!" );
		return true;

	#else // !MX_CHECK_STATIC_STR_HASH_COLLISIONS

		// No need to check for hash collisions in release mode because static (hardcoded, compile-time) strings cannot change.
		return (GetSize() == other.GetSize()) && (GetHash() == other.GetHash());

	#endif // MX_CHECK_STATIC_STR_HASH_COLLISIONSMX_CHECK_STATIC_STR_HASH_COLLISIONS
	}
};

FORCEINLINE bool operator == ( const StaticString& first, const StaticString& second )
{
	return first.Equals( second );
}

template<>
struct THashTrait< StaticString >
{
	static FORCEINLINE UINT GetHashCode( const StaticString& s )
	{
		return s.GetHash();
	}
};

template<>
struct TEqualsTrait< StaticString >
{
	static FORCEINLINE bool Equals( const StaticString& a, const StaticString& b )
	{
		return (a == b);
	}
};

mxNAMESPACE_END


/*

Standard tests:

using namespace StaticString_Util;

ULONG len;
UINT h0 = StringHashRunTime( "DS_Normal", len );

StaticString	s( "DS_Normal" );
Assert(h0==s.GetHash());

UINT h2 = StringHashRunTime( "FORCEINLINE operator const", len );
StaticString	s2( "FORCEINLINE operator const" );
Assert(h2==s2.GetHash());

UINT h3 = StringHashRunTime( "A", len );
StaticString	s3( "A" );
Assert(h3==s3.GetHash());


//UINT h4 = StringHashRunTime( "", len );
//UINT lll = mxStrLenAnsi("");	// is zero
//StaticString	s4( "" );// compiler goes into infinite loop?
//Assert(h4==s4.GetHash());


*/

#endif // !__MX_COMMON_STATIC_STRING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
