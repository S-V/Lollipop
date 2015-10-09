/*
=============================================================================
	File:	Sorting.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include "Sorting.h"

mxNAMESPACE_BEGIN

void QSort( void* pBase, INT count, INT width, QSortComparisonFunction pCompareFunc )
{
	::qsort( pBase, count, width, pCompareFunc );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
