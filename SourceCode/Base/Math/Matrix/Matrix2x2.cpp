/*
=============================================================================
	File:	Matrix2x2.cpp
	Desc:	Row-major float2x2 matrix.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

//===============================================================
//
//	Matrix2
//
//===============================================================

const Matrix2 Matrix2::mat2_zero( Vec2D( 0.0f, 0.0f ), Vec2D( 0.0f, 0.0f ) );
const Matrix2 Matrix2::mat2_identity( Vec2D( 1.0f, 0.0f ), Vec2D( 0.0f, 1.0f ) );

/*
============
Matrix2::InverseSelf
============
*/
bool Matrix2::InverseSelf( void ) {
	// 2+4 = 6 multiplications
	//		 1 division
	DOUBLE det, invDet, a;

	det = mRows[0][0] * mRows[1][1] - mRows[0][1] * mRows[1][0];

	if ( mxFabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = mRows[0][0];
	mRows[0][0] =   mRows[1][1] * invDet;
	mRows[0][1] = - mRows[0][1] * invDet;
	mRows[1][0] = - mRows[1][0] * invDet;
	mRows[1][1] =   a * invDet;

	return true;
}

/*
============
Matrix2::InverseFastSelf
============
*/
bool Matrix2::InverseFastSelf( void ) {
#if 1
	// 2+4 = 6 multiplications
	//		 1 division
	DOUBLE det, invDet, a;

	det = mRows[0][0] * mRows[1][1] - mRows[0][1] * mRows[1][0];

	if ( mxFabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	a = mRows[0][0];
	mRows[0][0] =   mRows[1][1] * invDet;
	mRows[0][1] = - mRows[0][1] * invDet;
	mRows[1][0] = - mRows[1][0] * invDet;
	mRows[1][1] =   a * invDet;

	return true;
#else
	// 2*4 = 8 multiplications
	//		 2 division
	FLOAT *mRows = reinterpret_cast<FLOAT *>(this);
	DOUBLE d, di;
	FLOAT s;

	di = mRows[0];
	s = di;
	mRows[0*2+0] = d = 1.0f / di;
	mRows[0*2+1] *= d;
	d = -d;
	mRows[1*2+0] *= d;
	d = mRows[1*2+0] * di;
	mRows[1*2+1] += mRows[0*2+1] * d;
	di = mRows[1*2+1];
	s *= di;
	mRows[1*2+1] = d = 1.0f / di;
	mRows[1*2+0] *= d;
	d = -d;
	mRows[0*2+1] *= d;
	d = mRows[0*2+1] * di;
	mRows[0*2+0] += mRows[1*2+0] * d;

	return ( s != 0.0f && !FLOAT_IS_NAN( s ) );
#endif
}

/*
=============
Matrix2::ToChars
=============
*/
const char *Matrix2::ToChars( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
