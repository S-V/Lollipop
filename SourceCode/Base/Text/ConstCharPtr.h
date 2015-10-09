/*
=============================================================================
	File:	TConstCharPtr.h
	Desc:	Static string that caches the length of the string.
=============================================================================
*/

#ifndef __MX_COMMON_ConstCharPtr_H__
#define __MX_COMMON_ConstCharPtr_H__

mxNAMESPACE_BEGIN

// dynamic, run-time string
struct ConstCharWrapper
{
	FORCEINLINE ConstCharWrapper( const char* str )
		: m_str( str )
	{}

	const char* m_str;
};

// static, compile-time string
//
// caches the length of the string
//
template< typename TYPE >
class TConstCharPtr
	: public TArrayBase< TYPE, TConstCharPtr< TYPE > >
{
	TYPE const* const	data;	// pointer to the string data
	const SizeT			size;	// length excluding null terminator

public:
	template< SizeT N >
	FORCEINLINE TConstCharPtr( const TYPE (&str)[N] )
		: data( str )
		, size( N-1 )
	{}

	FORCEINLINE TConstCharPtr( const TConstCharPtr& other )
		: data( other.data )
		, size( other.size )
	{}

	FORCEINLINE
	TYPE* ToChars()
	{
		return const_cast< TYPE* >( data );
	}
	FORCEINLINE
	const TYPE* ToChars() const
	{
		return data;
	}

	//-- TArrayBase

	FORCEINLINE
	UINT Num() const
	{
		return size;
	}

	FORCEINLINE
	UINT GetCapacity() const
	{
		return size;
	}

	FORCEINLINE
	TYPE* ToPtr()
	{
		return const_cast< TYPE* >( data );
	}
	FORCEINLINE
	const TYPE* ToPtr() const
	{
		return data;
	}


	FORCEINLINE
	UINT Length() const
	{
		return size;
	}


	// case sensitive comparison
	FORCEINLINE
	bool operator == ( const TConstCharPtr& other ) const
	{
		return Equals( other );
	}

	FORCEINLINE
	bool operator != ( const TConstCharPtr& other ) const { return !(*this == other); }


	FORCEINLINE bool Equals( const TConstCharPtr& other ) const
	{
		return mxEqualANSIStrings( data, size, other.data, other.size );
	}


	// Returns the amount of reserved memory in bytes (memory allocated for storing the elements).
	FORCEINLINE
	SizeT GetAllocatedMemory() const
	{
		return 0;;
	}

	// Returns the total amount of occupied memory in bytes.
	FORCEINLINE
	SizeT GetMemoryUsed() const
	{
		return sizeof(this) + size * sizeof(data[0]);
	}

public_internal:
	FORCEINLINE TConstCharPtr( const TYPE* str, SizeT len )
		: data( str )
		, size( len )
	{}
};

typedef TConstCharPtr< ANSICHAR >		ConstCharPtr;
typedef TConstCharPtr< UNICODECHAR >	ConstWCharPtr;


// constants

// "===========================================\n"
extern ConstCharPtr		LINE_CHARS;


mxNAMESPACE_END




#define MXC( constString )		ConstCharPtr( constString )



#endif // !__MX_COMMON_ConstCharPtr_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
