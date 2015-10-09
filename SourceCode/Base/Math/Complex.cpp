/*
=============================================================================
	File:	Complex.cpp
	Desc:
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

//
//	Static members.
//
const mxComplex mxComplex::complex_origin( 0.0f, 0.0f );

/*
=============
mxComplex::ToChars
=============
*/
const char *mxComplex::ToChars( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
