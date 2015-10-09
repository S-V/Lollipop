/*
=============================================================================
	File:	Vec4D.cpp
	Desc:.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN


//mxBEGIN_REFLECTION(Vec4D)
//	FIELD(FLOAT,x)
//	FIELD(FLOAT,y)
//	FIELD(FLOAT,z)
//	FIELD(FLOAT,w)
//mxEND_REFLECTION;

const Vec4D Vec4D::vec4_origin	( 0.0f, 0.0f, 0.0f, 0.0f );
const Vec4D Vec4D::vec4_unit	( 1.0f, 1.0f, 1.0f, 1.0f );

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vec4D::Lerp( const Vec4D &v1, const Vec4D &v2, const FLOAT l ) {
	if ( l <= 0.0f ) {
		*this = v1;
	} else if ( l >= 1.0f ) {
		*this = v2;
	} else {
		*this = v1 + l * ( v2 - v1 );
	}
}

/*
=============
Vec4D::ToChars
=============
*/
const char *Vec4D::ToChars( int precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}


/*
=============
Vec4D::RotateAboutX
=============
*/
Vec4D& Vec4D::RotateAboutX( FLOAT angle )
{
    FLOAT s, c;
	mxSinCos( angle, s, c );
    
    FLOAT ny = c * y - s * z;
    FLOAT nz = c * z + s * y;
    
    this->y = ny;
    this->z = nz;
    
    return *this;
}

/*
=============
Vec4D::RotateAboutY
=============
*/
Vec4D& Vec4D::RotateAboutY( FLOAT angle )
{
    FLOAT s, c;
	mxSinCos( angle, s, c );
    
    FLOAT nx = c * x + s * z;
    FLOAT nz = c * z - s * x;
    
    this->x = nx;
    this->z = nz;
    
    return *this;
}

/*
=============
Vec4D::RotateAboutZ
=============
*/
Vec4D& Vec4D::RotateAboutZ( FLOAT angle )
{
    FLOAT s, c;
	mxSinCos( angle, s, c );
    
    FLOAT nx = c * x - s * y;
    FLOAT ny = c * y + s * x;
    
    this->x = nx;
    this->y = ny;
    
    return *this;
}

/*
=============
Vec4D::RotateAboutAxis
=============
*/
Vec4D& Vec4D::RotateAboutAxis( FLOAT angle, const Vec3D& axis )
{
	FLOAT s, c;
	mxSinCos( angle, s, c );

    FLOAT k = 1.0F - c;
    
    FLOAT nx = x * (c + k * axis.x * axis.x) + y * (k * axis.x * axis.y - s * axis.z)
            + z * (k * axis.x * axis.z + s * axis.y);
    FLOAT ny = x * (k * axis.x * axis.y + s * axis.z) + y * (c + k * axis.y * axis.y)
            + z * (k * axis.y * axis.z - s * axis.x);
    FLOAT nz = x * (k * axis.x * axis.z - s * axis.y) + y * (k * axis.y * axis.z + s * axis.x)
            + z * (c + k * axis.z * axis.z);
    
    this->x = nx;
    this->y = ny;
    this->z = nz;
    
    return *this;
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
