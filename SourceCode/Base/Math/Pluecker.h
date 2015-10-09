/*
=============================================================================
	File:	Pluecker.h
	Desc:	Pluecker coordinates.
	Note:	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idPluecker )
=============================================================================
*/

#ifndef __MATH_PLUECKER_H__
#define __MATH_PLUECKER_H__
mxSWIPED("idSoftware");
/*
=============================================================================

	Pluecker coordinate

=============================================================================
*/

mxNAMESPACE_BEGIN

//
//	mxPluecker
//
class mxPluecker {
public:	
					mxPluecker( void );
					explicit mxPluecker( const FLOAT *a );
					explicit mxPluecker( const Vec3D &start, const Vec3D &end );
					explicit mxPluecker( const FLOAT a1, const FLOAT a2, const FLOAT a3, const FLOAT a4, const FLOAT a5, const FLOAT a6 );

	FLOAT			operator[]( const INT index ) const;
	FLOAT &			operator[]( const INT index );
	mxPluecker		operator-() const;											// flips the direction
	mxPluecker		operator*( const FLOAT a ) const;
	mxPluecker		operator/( const FLOAT a ) const;
	FLOAT			operator*( const mxPluecker &a ) const;						// permuted inner product
	mxPluecker		operator-( const mxPluecker &a ) const;
	mxPluecker		operator+( const mxPluecker &a ) const;
	mxPluecker &	operator*=( const FLOAT a );
	mxPluecker &	operator/=( const FLOAT a );
	mxPluecker &	operator+=( const mxPluecker &a );
	mxPluecker &	operator-=( const mxPluecker &a );

	bool			Compare( const mxPluecker &a ) const;						// exact compare, no epsilon
	bool			Compare( const mxPluecker &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==(	const mxPluecker &a ) const;					// exact compare, no epsilon
	bool			operator!=(	const mxPluecker &a ) const;					// exact compare, no epsilon

	void 			Set( const FLOAT a1, const FLOAT a2, const FLOAT a3, const FLOAT a4, const FLOAT a5, const FLOAT a6 );
	void			Zero( void );

	void			FromLine( const Vec3D &start, const Vec3D &end );			// pluecker from line
	void			FromRay( const Vec3D &start, const Vec3D &dir );			// pluecker from ray
	bool			FromPlanes( const Plane3D &p1, const Plane3D &p2 );			// pluecker from intersection of planes
	bool			ToLine( Vec3D &start, Vec3D &end ) const;					// pluecker to line
	bool			ToRay( Vec3D &start, Vec3D &dir ) const;					// pluecker to ray
	void			ToDir( Vec3D &dir ) const;									// pluecker to direction
	FLOAT			PermutedInnerProduct( const mxPluecker &a ) const;			// pluecker permuted inner product
	FLOAT			Distance3DSqr( const mxPluecker &a ) const;					// pluecker line distance

	FLOAT			GetLength( void ) const;										// pluecker length
	FLOAT			LengthSqr( void ) const;									// pluecker squared length
	mxPluecker		Normalize( void ) const;									// pluecker normalize
	FLOAT			NormalizeSelf( void );										// pluecker normalize

	INT				GetDimension( void ) const;

	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

public:
	static const mxPluecker pluecker_origin;

private:
	FLOAT			p[ 6 ];
};

FORCEINLINE mxPluecker::mxPluecker( void ) {
}

FORCEINLINE mxPluecker::mxPluecker( const FLOAT *a ) {
	memcpy( p, a, 6 * sizeof( FLOAT ) );
}

FORCEINLINE mxPluecker::mxPluecker( const Vec3D &start, const Vec3D &end ) {
	FromLine( start, end );
}

FORCEINLINE mxPluecker::mxPluecker( const FLOAT a1, const FLOAT a2, const FLOAT a3, const FLOAT a4, const FLOAT a5, const FLOAT a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

FORCEINLINE mxPluecker mxPluecker::operator-() const {
	return mxPluecker( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

FORCEINLINE FLOAT mxPluecker::operator[]( const INT index ) const {
	return p[index];
}

FORCEINLINE FLOAT &mxPluecker::operator[]( const INT index ) {
	return p[index];
}

FORCEINLINE mxPluecker mxPluecker::operator*( const FLOAT a ) const {
	return mxPluecker( p[0]*a, p[1]*a, p[2]*a, p[3]*a, p[4]*a, p[5]*a );
}

FORCEINLINE FLOAT mxPluecker::operator*( const mxPluecker &a ) const {
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

FORCEINLINE mxPluecker mxPluecker::operator/( const FLOAT a ) const {
	FLOAT inva;

	Assert( a != 0.0f );
	inva = 1.0f / a;
	return mxPluecker( p[0]*inva, p[1]*inva, p[2]*inva, p[3]*inva, p[4]*inva, p[5]*inva );
}

FORCEINLINE mxPluecker mxPluecker::operator+( const mxPluecker &a ) const {
	return mxPluecker( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

FORCEINLINE mxPluecker mxPluecker::operator-( const mxPluecker &a ) const {
	return mxPluecker( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

FORCEINLINE mxPluecker &mxPluecker::operator*=( const FLOAT a ) {
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

FORCEINLINE mxPluecker &mxPluecker::operator/=( const FLOAT a ) {
	FLOAT inva;

	Assert( a != 0.0f );
	inva = 1.0f / a;
	p[0] *= inva;
	p[1] *= inva;
	p[2] *= inva;
	p[3] *= inva;
	p[4] *= inva;
	p[5] *= inva;
	return *this;
}

FORCEINLINE mxPluecker &mxPluecker::operator+=( const mxPluecker &a ) {
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

FORCEINLINE mxPluecker &mxPluecker::operator-=( const mxPluecker &a ) {
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

FORCEINLINE bool mxPluecker::Compare( const mxPluecker &a ) const {
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

FORCEINLINE bool mxPluecker::Compare( const mxPluecker &a, const FLOAT epsilon ) const {
	if ( mxFabs( p[0] - a[0] ) > epsilon ) {
		return false;
	}
			
	if ( mxFabs( p[1] - a[1] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[2] - a[2] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[3] - a[3] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[4] - a[4] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[5] - a[5] ) > epsilon ) {
		return false;
	}

	return true;
}

FORCEINLINE bool mxPluecker::operator==( const mxPluecker &a ) const {
	return Compare( a );
}

FORCEINLINE bool mxPluecker::operator!=( const mxPluecker &a ) const {
	return !Compare( a );
}

FORCEINLINE void mxPluecker::Set( const FLOAT a1, const FLOAT a2, const FLOAT a3, const FLOAT a4, const FLOAT a5, const FLOAT a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

FORCEINLINE void mxPluecker::Zero( void ) {
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

FORCEINLINE void mxPluecker::FromLine( const Vec3D &start, const Vec3D &end ) {
	p[0] = start[0] * end[1] - end[0] * start[1];
	p[1] = start[0] * end[2] - end[0] * start[2];
	p[2] = start[0] - end[0];
	p[3] = start[1] * end[2] - end[1] * start[2];
	p[4] = start[2] - end[2];
	p[5] = end[1] - start[1];
}

FORCEINLINE void mxPluecker::FromRay( const Vec3D &start, const Vec3D &dir ) {
	p[0] = start[0] * dir[1] - dir[0] * start[1];
	p[1] = start[0] * dir[2] - dir[0] * start[2];
	p[2] = -dir[0];
	p[3] = start[1] * dir[2] - dir[1] * start[2];
	p[4] = -dir[2];
	p[5] = dir[1];
}

FORCEINLINE bool mxPluecker::ToLine( Vec3D &start, Vec3D &end ) const {
	Vec3D dir1, dir2;
	FLOAT d;

	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];

	dir2[0] = -p[2];
	dir2[1] = p[5];
	dir2[2] = -p[4];

	d = dir2 * dir2;
	if ( d == 0.0f ) {
		return false; // pluecker coordinate does not represent a line
	}

	start = dir2.Cross(dir1) * (1.0f / d);
	end = start + dir2;
	return true;
}

FORCEINLINE bool mxPluecker::ToRay( Vec3D &start, Vec3D &dir ) const {
	Vec3D dir1;
	FLOAT d;

	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];

	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];

	d = dir * dir;
	if ( d == 0.0f ) {
		return false; // pluecker coordinate does not represent a line
	}

	start = dir.Cross(dir1) * (1.0f / d);
	return true;
}

FORCEINLINE void mxPluecker::ToDir( Vec3D &dir ) const {
	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];
}

FORCEINLINE FLOAT mxPluecker::PermutedInnerProduct( const mxPluecker &a ) const {
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

FORCEINLINE FLOAT mxPluecker::GetLength( void ) const {
	return ( FLOAT )mxSqrt( p[5] * p[5] + p[4] * p[4] + p[2] * p[2] );
}

FORCEINLINE FLOAT mxPluecker::LengthSqr( void ) const {
	return ( p[5] * p[5] + p[4] * p[4] + p[2] * p[2] );
}

FORCEINLINE FLOAT mxPluecker::NormalizeSelf( void ) {
	FLOAT l, d;

	l = LengthSqr();
	if ( l == 0.0f ) {
		return l; // pluecker coordinate does not represent a line
	}
	d = mxInvSqrt( l );
	p[0] *= d;
	p[1] *= d;
	p[2] *= d;
	p[3] *= d;
	p[4] *= d;
	p[5] *= d;
	return d * l;
}

FORCEINLINE mxPluecker mxPluecker::Normalize( void ) const {
	FLOAT d;

	d = LengthSqr();
	if ( d == 0.0f ) {
		return *this; // pluecker coordinate does not represent a line
	}
	d = mxInvSqrt( d );
	return mxPluecker( p[0]*d, p[1]*d, p[2]*d, p[3]*d, p[4]*d, p[5]*d );
}

FORCEINLINE INT mxPluecker::GetDimension( void ) const {
	return 6;
}

FORCEINLINE const FLOAT *mxPluecker::ToFloatPtr( void ) const {
	return p;
}

FORCEINLINE FLOAT *mxPluecker::ToFloatPtr( void ) {
	return p;
}

mxNAMESPACE_END

#endif /* !__MATH_PLUECKER_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

