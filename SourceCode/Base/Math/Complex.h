/*
=============================================================================
	File:	Complex.h
	Desc:	Complex number class.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idComplex )
=============================================================================
*/

#ifndef __MATH_COMPLEX_H__
#define __MATH_COMPLEX_H__
mxSWIPED("idSoftware");
/*
=============================================================================

  Complex number

=============================================================================
*/

mxNAMESPACE_BEGIN

//
//	mxComplex
//
class mxComplex {
public:
	FLOAT				r;		// real part
	FLOAT				i;		// imaginary part

public:
						mxComplex( void );
						mxComplex( const FLOAT r, const FLOAT i );

	void 				Set( const FLOAT r, const FLOAT i );
	void				Zero( void );

	FLOAT				operator[]( INT index ) const;
	FLOAT &				operator[]( INT index );

	mxComplex			operator-() const;
	mxComplex &			operator=( const mxComplex &a );

	mxComplex			operator*( const mxComplex &a ) const;
	mxComplex			operator/( const mxComplex &a ) const;
	mxComplex			operator+( const mxComplex &a ) const;
	mxComplex			operator-( const mxComplex &a ) const;

	mxComplex &			operator*=( const mxComplex &a );
	mxComplex &			operator/=( const mxComplex &a );
	mxComplex &			operator+=( const mxComplex &a );
	mxComplex &			operator-=( const mxComplex &a );

	mxComplex			operator*( const FLOAT a ) const;
	mxComplex			operator/( const FLOAT a ) const;
	mxComplex			operator+( const FLOAT a ) const;
	mxComplex			operator-( const FLOAT a ) const;

	mxComplex &			operator*=( const FLOAT a );
	mxComplex &			operator/=( const FLOAT a );
	mxComplex &			operator+=( const FLOAT a );
	mxComplex &			operator-=( const FLOAT a );

	friend mxComplex	operator*( const FLOAT a, const mxComplex &b );
	friend mxComplex	operator/( const FLOAT a, const mxComplex &b );
	friend mxComplex	operator+( const FLOAT a, const mxComplex &b );
	friend mxComplex	operator-( const FLOAT a, const mxComplex &b );

	bool				Compare( const mxComplex &a ) const;						// exact compare, no epsilon
	bool				Compare( const mxComplex &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool				operator==(	const mxComplex &a ) const;						// exact compare, no epsilon
	bool				operator!=(	const mxComplex &a ) const;						// exact compare, no epsilon

	mxComplex			Reciprocal( void ) const;
	mxComplex			Sqrt( void ) const;
	FLOAT				Abs( void ) const;

	INT					GetDimension( void ) const;

	const FLOAT *		ToFloatPtr( void ) const;
	FLOAT *				ToFloatPtr( void );
	const char *		ToChars( INT precision = 2 ) const;

public:
	static const mxComplex complex_origin;
};

FORCEINLINE mxComplex::mxComplex( void ) {
}

FORCEINLINE mxComplex::mxComplex( const FLOAT r, const FLOAT i ) {
	this->r = r;
	this->i = i;
}

FORCEINLINE void mxComplex::Set( const FLOAT r, const FLOAT i ) {
	this->r = r;
	this->i = i;
}

FORCEINLINE void mxComplex::Zero( void ) {
	r = i = 0.0f;
}

FORCEINLINE FLOAT mxComplex::operator[]( INT index ) const {
	Assert( index >= 0 && index < 2 );
	return ( &r )[ index ];
}

FORCEINLINE FLOAT& mxComplex::operator[]( INT index ) {
	Assert( index >= 0 && index < 2 );
	return ( &r )[ index ];
}

FORCEINLINE mxComplex mxComplex::operator-() const {
	return mxComplex( -r, -i );
}

FORCEINLINE mxComplex &mxComplex::operator=( const mxComplex &a ) {
	r = a.r;
	i = a.i;
	return *this;
}

FORCEINLINE mxComplex mxComplex::operator*( const mxComplex &a ) const {
	return mxComplex( r * a.r - i * a.i, i * a.r + r * a.i );
}

FORCEINLINE mxComplex mxComplex::operator/( const mxComplex &a ) const {
	FLOAT s, t;
	if ( mxFabs( a.r ) >= mxFabs( a.i ) ) {
		s = a.i / a.r;
		t = 1.0f / ( a.r + s * a.i );
		return mxComplex( ( r + s * i ) * t, ( i - s * r ) * t );
	} else {
		s = a.r / a.i;
		t = 1.0f / ( s * a.r + a.i );
		return mxComplex( ( r * s + i ) * t, ( i * s - r ) * t );
	}
}

FORCEINLINE mxComplex mxComplex::operator+( const mxComplex &a ) const {
	return mxComplex( r + a.r, i + a.i );
}

FORCEINLINE mxComplex mxComplex::operator-( const mxComplex &a ) const {
	return mxComplex( r - a.r, i - a.i );
}

FORCEINLINE mxComplex &mxComplex::operator*=( const mxComplex &a ) {
	*this = mxComplex( r * a.r - i * a.i, i * a.r + r * a.i );
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator/=( const mxComplex &a ) {
	FLOAT s, t;
	if ( mxFabs( a.r ) >= mxFabs( a.i ) ) {
		s = a.i / a.r;
		t = 1.0f / ( a.r + s * a.i );
		*this = mxComplex( ( r + s * i ) * t, ( i - s * r ) * t );
	} else {
		s = a.r / a.i;
		t = 1.0f / ( s * a.r + a.i );
		*this = mxComplex( ( r * s + i ) * t, ( i * s - r ) * t );
	}
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator+=( const mxComplex &a ) {
	r += a.r;
	i += a.i;
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator-=( const mxComplex &a ) {
	r -= a.r;
	i -= a.i;
	return *this;
}

FORCEINLINE mxComplex mxComplex::operator*( const FLOAT a ) const {
	return mxComplex( r * a, i * a );
}

FORCEINLINE mxComplex mxComplex::operator/( const FLOAT a ) const {
	FLOAT s = 1.0f / a;
	return mxComplex( r * s, i * s );
}

FORCEINLINE mxComplex mxComplex::operator+( const FLOAT a ) const {
	return mxComplex( r + a, i );
}

FORCEINLINE mxComplex mxComplex::operator-( const FLOAT a ) const {
	return mxComplex( r - a, i );
}

FORCEINLINE mxComplex &mxComplex::operator*=( const FLOAT a ) {
	r *= a;
	i *= a;
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator/=( const FLOAT a ) {
	FLOAT s = 1.0f / a;
	r *= s;
	i *= s;
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator+=( const FLOAT a ) {
	r += a;
	return *this;
}

FORCEINLINE mxComplex &mxComplex::operator-=( const FLOAT a ) {
	r -= a;
	return *this;
}

FORCEINLINE mxComplex operator*( const FLOAT a, const mxComplex &b ) {
	return mxComplex( a * b.r, a * b.i );
}

FORCEINLINE mxComplex operator/( const FLOAT a, const mxComplex &b ) {
	FLOAT s, t;
	if ( mxFabs( b.r ) >= mxFabs( b.i ) ) {
		s = b.i / b.r;
		t = a / ( b.r + s * b.i );
		return mxComplex( t, - s * t );
	} else {
		s = b.r / b.i;
		t = a / ( s * b.r + b.i );
		return mxComplex( s * t, - t );
	}
}

FORCEINLINE mxComplex operator+( const FLOAT a, const mxComplex &b ) {
	return mxComplex( a + b.r, b.i );
}

FORCEINLINE mxComplex operator-( const FLOAT a, const mxComplex &b ) {
	return mxComplex( a - b.r, -b.i );
}

FORCEINLINE mxComplex mxComplex::Reciprocal( void ) const {
	FLOAT s, t;
	if ( mxFabs( r ) >= mxFabs( i ) ) {
		s = i / r;
		t = 1.0f / ( r + s * i );
		return mxComplex( t, - s * t );
	} else {
		s = r / i;
		t = 1.0f / ( s * r + i );
		return mxComplex( s * t, - t );
	}
}

FORCEINLINE mxComplex mxComplex::Sqrt( void ) const {
	FLOAT x, y, w;

	if ( r == 0.0f && i == 0.0f ) {
		return mxComplex( 0.0f, 0.0f );
	}
	x = mxFabs( r );
	y = mxFabs( i );
	if ( x >= y ) {
		w = y / x;
		w = mxSqrt( x ) * mxSqrt( 0.5f * ( 1.0f + mxSqrt( 1.0f + w * w ) ) );
	} else {
		w = x / y;
		w = mxSqrt( y ) * mxSqrt( 0.5f * ( w + mxSqrt( 1.0f + w * w ) ) );
	}
	if ( w == 0.0f ) {
		return mxComplex( 0.0f, 0.0f );
	}
	if ( r >= 0.0f ) {
		return mxComplex( w, 0.5f * i / w );
	} else {
		return mxComplex( 0.5f * y / w, ( i >= 0.0f ) ? w : -w );
	}
}

FORCEINLINE FLOAT mxComplex::Abs( void ) const {
	FLOAT x, y, t;
	x = mxFabs( r );
	y = mxFabs( i );
	if ( x == 0.0f ) {
		return y;
	} else if ( y == 0.0f ) {
		return x;
	} else if ( x > y ) {
		t = y / x;
		return x * mxSqrt( 1.0f + t * t );
	} else {
		t = x / y;
		return y * mxSqrt( 1.0f + t * t );
	}
}

FORCEINLINE bool mxComplex::Compare( const mxComplex &a ) const {
	return ( ( r == a.r ) && ( i == a.i ) );
}

FORCEINLINE bool mxComplex::Compare( const mxComplex &a, const FLOAT epsilon ) const {
	if ( mxFabs( r - a.r ) > epsilon ) {
		return false;
	}
	if ( mxFabs( i - a.i ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool mxComplex::operator==( const mxComplex &a ) const {
	return Compare( a );
}

FORCEINLINE bool mxComplex::operator!=( const mxComplex &a ) const {
	return !Compare( a );
}

FORCEINLINE INT mxComplex::GetDimension( void ) const {
	return 2;
}

FORCEINLINE const FLOAT *mxComplex::ToFloatPtr( void ) const {
	return &r;
}

FORCEINLINE FLOAT *mxComplex::ToFloatPtr( void ) {
	return &r;
}

mxNAMESPACE_END

#endif /* !__MATH_COMPLEX_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
