/*
=============================================================================
	File:	HashFunctions.h
	Desc:	Useful hash functions.
=============================================================================
*/

#ifndef __MX_HASH_FUNCTIONS_H__
#define __MX_HASH_FUNCTIONS_H__

mxNAMESPACE_BEGIN


mxSWIPED("idLib")
INLINE INT SimpleStrHash( const char *str )
{
	INT hash = 0;
	for ( INT i = 0; *str != '\0'; i++ ) {
		hash += ( *str++ ) * ( i + 119 );
	}
	return hash;
}
mxSWIPED("idLib")
INLINE UINT SimpleUIntHash( UINT key )
{
	return UINT( ( key >> ( 8 * sizeof( UINT ) - 1 ) ) ^ key );
}
INLINE INT NameHash( const char *name )
{
	INT hash, i;

	hash = 0;
	for ( i = 0; name[i]; i++ ) {
		hash += name[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	return hash;
}
mxSWIPED("Quake 3 source code")
INLINE INT FileNameHash( const char *name, UINT len )
{
	char letter;
	long hash = 0;
	UINT i = 0;
	while( i < len ) 
	{
		letter = mxAnsiCharToLower( name[i] );
		if ( letter == '.' ) {
			break;				// don't include extension
		}
		if ( letter == PATHSEPARATOR_CHAR || letter == PATHSEPARATOR_CHAR2 ) {
			letter = '/';                      // damn path names
		}
		hash += (long)( letter ) * ( i + 119 );
		i++;
	}
	return hash;
}
INLINE INT FileNameHash( const char *name ) {
	return FileNameHash( name, mxStrLenAnsi( name ) );
}

//
//	HashPjw - has been successfully used in symbol tables (in old compilers).
//
INLINE INT HashPjw( const char* s )
{
	enum { PRIME = 211 };

#if 0
	// generates warning : assignment within conditional expression
	UINT h = 0, g;
	for ( const char *p = s; *p; p++ ) {
		if ( g = (h = (h << 4) + *p) & 0xF0000000 ) {
			h ^= g >> 24 ^ g;
		}
	}
#else
	UINT h = 0, g;
	for ( const char *p = s; *p; p++ ) {
		h = (h << 4) + *p;
		g = h & 0xF0000000;
		if ( g ) {
			h ^= g >> 24 ^ g;
		}
	}
#endif
	return h % PRIME;
}

//
//	String hashing functions from D compiler (by Walter Bright).
//
UINT CalcStringHash( const char* str, UINT len );
//	Case insensitive version:
UINT CalcStringHashI( const char* str, UINT len );

UINT StringHash2( const char* str, UINT length = 0 );

/*
SuperFastHash from Paul Hsieh site, that is very fast hash to variable string size. 
Mode information here: http://www.azillionmonkeys.com/qed/hash.html 
*/

UINT32 SuperFastHash( const char* data, int len );

//
// MurmurHash is a non-cryptographic hash function suitable for general hash-based lookup.
//
UINT32 MurmurHash( const void* buffer, UINT32 sizeBytes, UINT32 seed );
UINT64 MurmurHash64( const void* buffer, UINT32 sizeBytes, UINT32 seed );

// Fowler / Noll / Vo (FNV) Hash
// See: http://isthe.com/chongo/tech/comp/fnv/
//
UINT32 FNV32_StringHash( const char* str );


// Quick hashing function
inline UINT32 Adler32_Hash( const char *data )
{
	static const UINT32 MOD_ADLER = 65521;
	UINT32 a = 1, b = 0;

	/* Loop over each byte of data, in order */
	for (size_t index = 0; data[index]; ++index)
	{
		a = (a + data[index]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

/*
==================================================================
	Integer hash functions.

	An integer hash function accepts an integer hash key
	and returns an integer hash result with uniform distribution.
==================================================================
*/

// Thomas Wang's hash
// see:http://www.concentric.net/~Ttwang/tech/inthash.htm
INLINE INT32 Hash32Bits_0( INT32 key )
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
}
INLINE INT64 Hash64Bits_0( INT64 key )
{
	key += ~(key << 32);
	key ^= (key >> 22);
	key += ~(key << 13);
	key ^= (key >> 8);
	key += (key << 3);
	key ^= (key >> 15);
	key += ~(key << 27);
	key ^= (key >> 31);
	return key;
}

// Bob Jenkin's hash
// http://burtleburtle.net/bob/hash/integer.html
//
INLINE INT32 Hash32Bits_1( INT32 key )
{
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);
	return key;
}
INLINE INT64 Hash64Bits_1( INT64 key )
{
	INT64 c1 = 0x6e5ea73858134343L;
	INT64 c2 = 0xb34e8f99a2ec9ef5L;
	key ^= ((c1 ^ key) >> 32);
	key *= c1;
	key ^= ((c2 ^ key) >> 31);
	key *= c2;
	key ^= ((c1 ^ key) >> 32);
	return key;
}
INLINE UINT32 Hash32Bits_2( UINT32 a )
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

// multiplicative hash function
INLINE INT32 Hash32ShiftMult( INT32 key )
{
	INT32 c2 = 0x27d4eb2d; // a prime or an odd constant
	key = (key ^ 61) ^ ((UINT32)key >> 16);
	key = key + (key << 3);
	key = key ^ ((UINT32)key >> 4);
	key = key * c2;
	key = key ^ ((UINT32)key >> 15);
	return key;
}

// 64 bit to 32 bit hash function
INLINE INT32 hash6432shift( INT64 key )
{
	key = (~key) + (key << 18); // key = (key << 18) - key - 1;
	key = key ^ ((UINT64)key >> 31);
	key = key * 21; // key = (key + (key << 2)) + (key << 4);
	key = key ^ ((UINT64)key >> 11);
	key = key + (key << 6);
	key = key ^ ((UINT64)key >> 22);
	return key;
}

mxSWIPED("Havok");
//
//
// mxPointerHash - uses multiplication for hashing.
// In Knuth's "The Art of Computer Programming", section 6.4,
// a multiplicative hashing scheme is introduced as a way to write hash function.
// The key is multiplied by the golden ratio of 2^32 (2654435761) to produce a hash result. 
// Since 2654435761 and 2^32 has no common factors in common,
// the multiplication produces a complete mapping of the key to hash result with no overlap.
// This method works pretty well if the keys have small values.
// Bad hash results are produced if the keys vary in the upper bits.
// As is true in all multiplications, variations of upper digits
// do not influence the lower digits of the multiplication result.
//
FORCEINLINE mxULong mxPointerHash( const void* ptr )
{
	const mxULong addr = (mxULong) ptr;
	// We ignore the lowest four bits on the address, since most addresses will be 16-byte aligned.
	return (addr >> 4) * 2654435761U;	// Knuth's multiplicative golden hash.
}

struct mxPointerHasher
{
	static FORCEINLINE UINT GetHashCode( const void* ptr )
	{
		#if (MX_POINTER_SIZE == 4)
		{
			return Hash32Bits_0( (INT32)ptr );
		}
		#elif (MX_POINTER_SIZE == 8)
		{
			return Hash64Bits_0( (INT64)ptr );
		}
		#else
		{
			return mxPointerHash( ptr );
		}
		#endif
	}
};

mxNAMESPACE_END

#endif // ! __MX_HASH_FUNCTIONS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
