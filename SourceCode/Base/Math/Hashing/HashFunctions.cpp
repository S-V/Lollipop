/*
=============================================================================
	File:	HashFunctions.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

UINT CalcStringHash( const char* str, UINT len )
{
	UINT hash = 0;

    while (1)
    {
		switch (len)
		{
			case 0:
			return hash;

			case 1:
			hash *= 37;
			hash += *(const U1 *)str;
			return hash;

			case 2:
			hash *= 37;
			hash += str[0] * 256 + str[1];
			return hash;

			case 3:
			hash *= 37;
			hash += (str[0] * 256 + str[1]) * 256 + str[2];

			return hash;

			default:
			hash *= 37;
			hash += ((str[0] * 256 + str[1]) * 256 + str[2]) * 256 + str[3];
			str += 4;
			len -= 4;
			break;
		}
    }
}

UINT CalcStringHashI( const char* str, UINT len )
{
    UINT hash = 0;

    while (1)
    {
		switch (len)
		{
			case 0:
			return hash;

			case 1:
			hash *= 37;
			hash += *(const U1 *)str | 0x20;
			return hash;

			case 2:
			hash *= 37;
			hash += *(const U2 *)str | 0x2020;
			return hash;

			case 3:
			hash *= 37;
			hash += ((*(const U2 *)str << 8) +
				 ((const U8 *)str)[2]) | 0x202020;
			return hash;

			default:
			hash *= 37;
			hash += *(const U4 *)str | 0x20202020;
			str += 4;
			len -= 4;
			break;
		}
    }
}

UINT StringHash2( const char *str, UINT length )
{
	if( !str ) {
		return 0;
	}
	if( !length ) {
		length = mxStrLenAnsi( str );
	}

	UINT hash  = 0;
	const UCHAR* pUChar = (const UCHAR*) str;

	for( UINT i = 0; i < length; ++i, ++pUChar )
	{
		hash = ( hash << 4 ) + *pUChar;

		UINT tmp = hash & 0xF0000000;
		if( tmp ) {
			hash ^= ( tmp >> 24 );
		}
		hash &= ~tmp;
	}

	return hash;
}

/*
 * Hashing algorithm for strings from:
 * Sedgewick's "Algorithms in C++, third edition" 
 * parts 1-4, Chapter 14 (hashing) p.593
 */
unsigned int string_hash(const char* key, unsigned long size) {
	int hash = 0, a = 31415, b = 27183;

	const char *sptr = key;
	
	while(*sptr) {
		hash = (a * hash + *sptr++) % size;
		a = a * b % (size - 1);
	}
	
	return (unsigned int)(hash < 0 ? (hash + size) : hash);
}


/*
SuperFastHash from Paul Hsieh site, that is very fast hash to variable string size. 

Mode information here: http://www.azillionmonkeys.com/qed/hash.html 
Code:

#include "pstdint.h" // Replace with <stdint.h> if appropriate
#undef get16bits 
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \ 
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__) 
#define get16bits(d) (*((const U16 *) (d))) 
#endif 

#if !defined (get16bits) 
#define get16bits(d) ((((UINT32)(((const U8 *)(d))[1])) << 8)\ 
                       +(UINT32)(((const U8 *)(d))[0]) ) 
#endif 
*/

#define get16bits(d) (*( (const UINT16*) (d) ))

UINT32 SuperFastHash( const char* data, int len )
{
	UINT32 hash = len, tmp; 
	int rem; 

	if (len <= 0 || data == NULL) return 0; 

	rem = len & 3; 
	len >>= 2; 

	/* Main loop */ 
	for (;len > 0; len--) { 
		hash  += get16bits (data); 
		tmp    = (get16bits (data+2) << 11) ^ hash; 
		hash   = (hash << 16) ^ tmp; 
		data  += 2*sizeof (UINT16); 
		hash  += hash >> 11; 
	} 

	/* Handle end cases */ 
	switch( rem )
	{ 
	case 3: hash += get16bits (data); 
		hash ^= hash << 16; 
		hash ^= data[sizeof (UINT16)] << 18; 
		hash += hash >> 11; 
		break; 
	case 2: hash += get16bits (data); 
		hash ^= hash << 11; 
		hash += hash >> 17; 
		break; 
	case 1: hash += *data; 
		hash ^= hash << 10; 
		hash += hash >> 1; 
	} 

	/* Force "avalanching" of final 127 bits */ 
	hash ^= hash << 3; 
	hash += hash >> 5; 
	hash ^= hash << 4; 
	hash += hash >> 17; 
	hash ^= hash << 25; 
	hash += hash >> 6; 

	return hash; 
}

#undef get16bits

UINT32 MurmurHash( const void* buffer, UINT32 sizeBytes, UINT32 seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const UINT32 m = 0x5bd1e995;
	const INT r = 24;

	// Initialize the hash to a 'random' value

	UINT32 h = seed ^ sizeBytes;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)buffer;

	while( sizeBytes >= 4 )
	{
		UINT32 k = *(UINT32 *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		data += 4;
		sizeBytes -= 4;
	}

	// Handle the last few bytes of the input array

	switch( sizeBytes )
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

#ifdef _M_X64

UINT64 MurmurHash64( const void* buffer, UINT32 sizeBytes, UINT32 seed )
{
	const UINT64 m = 0xc6a4a7935bd1e995;
	const INT r = 47;

	UINT64 h = seed ^ (sizeBytes * m);

	const UINT64 * data = (const UINT64 *)buffer;
	const UINT64 * end = data + (sizeBytes/8);

	while(data != end)
	{
		UINT64 k = *data++;

		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h ^= k;
		h *= m; 
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(sizeBytes & 7)
	{
	case 7: h ^= UINT64(data2[6]) << 48;
	case 6: h ^= UINT64(data2[5]) << 40;
	case 5: h ^= UINT64(data2[4]) << 32;
	case 4: h ^= UINT64(data2[3]) << 24;
	case 3: h ^= UINT64(data2[2]) << 16;
	case 2: h ^= UINT64(data2[1]) << 8;
	case 1: h ^= UINT64(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

#elif _M_IX86

UINT64 MurmurHash64( const void* buffer, UINT32 sizeBytes, UINT32 seed )
{
	const UINT64 m = 0xc6a4a7935bd1e995;
	const INT r = 47;

	UINT64 h = seed ^ (sizeBytes * m);

	const UINT64 * data = (const UINT64 *)buffer;
	const UINT64 * end = data + (sizeBytes/8);

	while(data != end)
	{
		UINT64 k = *data++;

		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h ^= k;
		h *= m; 
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(sizeBytes & 7)
	{
	case 7: h ^= UINT64(data2[6]) << 48;
	case 6: h ^= UINT64(data2[5]) << 40;
	case 5: h ^= UINT64(data2[4]) << 32;
	case 4: h ^= UINT64(data2[3]) << 24;
	case 3: h ^= UINT64(data2[2]) << 16;
	case 2: h ^= UINT64(data2[1]) << 8;
	case 1: h ^= UINT64(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

#endif


// Fowler / Noll / Vo (FNV) Hash
// See: http://isthe.com/chongo/tech/comp/fnv/
// FNV hashes are designed to be fast while maintaining a low collision rate.
// The FNV speed allows one to quickly hash lots of data while maintaining a reasonable collision rate.
// The high dispersion of the FNV hashes makes them well suited for hashing nearly identical strings
// such as URLs, hostnames, filenames, text, IP addresses, etc.

namespace
{

#define FNV_32_PRIME ((Fnv32_t)0x01000193)

/*
 * 32 bit FNV-0 hash type
 */
typedef U4 Fnv32_t;

/*
 * fnv_32_str - perform a 32 bit Fowler/Noll/Vo hash on a string
 *
 * input:
 *	str	- string to hash
 *	hval	- previous hash value or 0 if first call
 *
 * returns:
 *	32 bit hash as a static hash type
 *
 * NOTE: To use the 32 bit FNV-0 historic hash, use FNV0_32_INIT as the hval
 *	 argument on the first call to either fnv_32_buf() or fnv_32_str().
 *
 * NOTE: To use the recommended 32 bit FNV-1 hash, use FNV1_32_INIT as the hval
 *	 argument on the first call to either fnv_32_buf() or fnv_32_str().
 */
Fnv32_t
fnv_32_str(const char *str, Fnv32_t hval)
{
    const unsigned char *s = c_cast(const unsigned char *)str;	/* unsigned string */

    /*
     * FNV-1 hash each octet in the buffer
     */
    while (*s) {

	/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
	hval *= FNV_32_PRIME;
#else
	hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif

	/* xor the bottom with the current octet */
	hval ^= (Fnv32_t)*s++;
    }

    /* return our new hash value */
    return hval;
}

}//namespace

UINT32 FNV32_StringHash( const char* str )
{
	return fnv_32_str( str, 0 );
}

// hash algorithms used by GTA IV.
// Taken from:
// http://www.gtamodding.com/index.php?title=Cryptography#AES

unsigned int CRC32_gta4(char* text)
{
	size_t textLen = strlen(text);
	int i = 0;
	unsigned int retHash = 0;
	if(text[0] == '"')
		i = 1;
	for(i;i<textLen;i++)
	{
		char ctext = text[i];
		if(ctext == '"')
			break;
		if(ctext - 65 > 25)
		{
			if(ctext == '\\')
				ctext = '/';
		}
		else ctext += 32;
		retHash = (1025 * (retHash + ctext) >> 6) ^ 1025 * (retHash + ctext);
	}
	return 32769 * (9 * retHash ^ (9 * retHash >> 11));
}

unsigned int oneAtATimeHash(char* inpStr)
{
	unsigned int value = 0,temp = 0;
	for(size_t i=0;i<strlen(inpStr);i++)
	{
		char ctext = tolower(inpStr[i]);
		temp = ctext;
		temp += value;
		value = temp << 10;
		temp += value;
		value = temp >> 6;
		value ^= temp;
	}
	temp = value << 3;
	temp += value;
	unsigned int temp2 = temp >> 11;
	temp = temp2 ^ temp;
	temp2 = temp << 15;
	value = temp2 + temp;
	if(value < 2) value += 2;
	return value;
}





mxSWIPED("Effects11, DX SDK, June 2010");


//////////////////////////////////////////////////////////////////////////
// Hash table
//////////////////////////////////////////////////////////////////////////

#define HASH_MIX(a,b,c) \
{ \
    a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8); \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12);  \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
}

static UINT ComputeHash(BYTE *pb, UINT cbToHash)
{
    UINT a;
    UINT b;
    UINT c;
    UINT cbLeft;

    cbLeft = cbToHash;

    a = b = 0x9e3779b9; // the golden ratio; an arbitrary value
    c = 0;

    while (cbLeft >= 12)
    {
        UINT *pdw = reinterpret_cast<UINT *>(pb);

        a += pdw[0];
        b += pdw[1];
        c += pdw[2];

        HASH_MIX(a,b,c);
        pb += 12; 
        cbLeft -= 12;
    }

    c += cbToHash;

    switch(cbLeft) // all the case statements fall through
    {
    case 11: c+=((UINT) pb[10] << 24);
    case 10: c+=((UINT) pb[9]  << 16);
    case 9 : c+=((UINT) pb[8]  <<  8);
        // the first byte of c is reserved for the length
    case 8 : b+=((UINT) pb[7]  << 24);
    case 7 : b+=((UINT) pb[6]  << 16);
    case 6 : b+=((UINT) pb[5]  <<  8);
    case 5 : b+=pb[4];
    case 4 : a+=((UINT) pb[3]  << 24);
    case 3 : a+=((UINT) pb[2]  << 16);
    case 2 : a+=((UINT) pb[1]  <<  8);
    case 1 : a+=pb[0];
    }

    HASH_MIX(a,b,c);

    return c;
}

static UINT ComputeHashLower(BYTE *pb, UINT cbToHash)
{
    UINT a;
    UINT b;
    UINT c;
    UINT cbLeft;

    cbLeft = cbToHash;

    a = b = 0x9e3779b9; // the golden ratio; an arbitrary value
    c = 0;

    while (cbLeft >= 12)
    {
        BYTE pbT[12];
        for( UINT i = 0; i < 12; i++ )
            pbT[i] = (BYTE)tolower(pb[i]);

        UINT *pdw = reinterpret_cast<UINT *>(pbT);

        a += pdw[0];
        b += pdw[1];
        c += pdw[2];

        HASH_MIX(a,b,c);
        pb += 12; 
        cbLeft -= 12;
    }

    c += cbToHash;

    BYTE pbT[12];
    for( UINT i = 0; i < cbLeft; i++ )
        pbT[i] = (BYTE)tolower(pb[i]);

    switch(cbLeft) // all the case statements fall through
    {
    case 11: c+=((UINT) pbT[10] << 24);
    case 10: c+=((UINT) pbT[9]  << 16);
    case 9 : c+=((UINT) pbT[8]  <<  8);
        // the first byte of c is reserved for the length
    case 8 : b+=((UINT) pbT[7]  << 24);
    case 7 : b+=((UINT) pbT[6]  << 16);
    case 6 : b+=((UINT) pbT[5]  <<  8);
    case 5 : b+=pbT[4];
    case 4 : a+=((UINT) pbT[3]  << 24);
    case 3 : a+=((UINT) pbT[2]  << 16);
    case 2 : a+=((UINT) pbT[1]  <<  8);
    case 1 : a+=pbT[0];
    }

    HASH_MIX(a,b,c);

    return c;
}


static UINT ComputeHash(LPCSTR pString)
{
    return ComputeHash((BYTE*) pString, (UINT)strlen(pString));
}


// 1) these numbers are prime
// 2) each is slightly less than double the last
// 4) each is roughly in between two powers of 2;
//    (2^n hash table sizes are VERY BAD; they effectively truncate your
//     precision down to the n least significant bits of the hash)
static const UINT c_PrimeSizes[] = 
{
    11,
    23,
    53,
    97,
    193,
    389,
    769,
    1543,
    3079,
    6151,
    12289,
    24593,
    49157,
    98317,
    196613,
    393241,
    786433,
    1572869,
    3145739,
    6291469,
    12582917,
    25165843,
    50331653,
    100663319,
    201326611,
    402653189,
    805306457,
    1610612741,
};

static const UINT c_NumPrimes = NUMBER_OF(c_PrimeSizes);

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
