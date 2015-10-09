/*
=============================================================================
	File:	BitArray.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Templates/Containers/BitSet/BitArray.h>

mxNAMESPACE_BEGIN

/*================================
			BitArray
================================*/

BitArray::BitArray()
	: mSize( 0 )
	, mBits( nil )
{
}

BitArray::BitArray( UINT numBits )
	: mSize( 0 )
	, mBits( nil )
{
	Init( numBits );
}

BitArray::~BitArray()
{
	free_array( mBits );
	mSize = 0;
}

/*
==============================================================
	BitArray::Init

	Initializes the bit array for a given number of entries.

	\param		numBits		[in] max number of entries in the array
	\return		true if success
==============================================================
*/
bool BitArray::Init( UINT numBits )
{
	mSize = BitsToUInts( numBits );
	// Get ram for n bits.
	free_array( mBits );
	mBits = c_cast(UINT*)mxAlloc( sizeof(UINT) * mSize );
	// Set all bits to 0.
	ClearAll();
	return true;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
