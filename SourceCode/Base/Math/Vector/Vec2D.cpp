/*
=============================================================================
	File:	Vec2D.cpp
	Desc:.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

const Vec2D Vec2D::vec2_origin	( 0.0f, 0.0f );
const Vec2D Vec2D::vec2_unit	( 1.0f, 1.0f );

/*
=============
Vec2D::ToChars
=============
*/
const char *Vec2D::ToChars( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vec2D::Lerp( const Vec2D &v1, const Vec2D &v2, const FLOAT l ) {
	if ( l <= 0.0f ) {
		(*this) = v1;
	} else if ( l >= 1.0f ) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * ( v2 - v1 );
	}
}

/*
=============
Vec2D::Rotate
=============
*/
Vec2D & Vec2D::Rotate( FLOAT angle )
{
	FLOAT s, c;
	mxSinCos( angle, s, c );
   
    FLOAT nx = c * x - s * y;
    FLOAT ny = s * x + c * y;
   
    this->x = nx;
    this->y = ny;
    
    return *this;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
