/*
=============================================================================
	File:	Matrix3x3.h
	Desc:	Column-major float3x3 matrix.

	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idMat3 )
=============================================================================
*/

#ifndef __MATH_MATRIX_3x3_H__
#define __MATH_MATRIX_3x3_H__
mxSWIPED("idSoftware");
/*
=============================================================================

  All matrix classes are row-major except Matrix3.

=============================================================================
*/

mxNAMESPACE_BEGIN

//===============================================================
//
//	Matrix3 - 3x3 matrix
//
//	NOTE:	matrix is column-major.
//
//===============================================================

class Matrix3 {
public:
					Matrix3( void );
					explicit Matrix3( const Vec3D &x, const Vec3D &y, const Vec3D &z );
					explicit Matrix3( const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT zx, const FLOAT zy, const FLOAT zz );
					explicit Matrix3( const FLOAT src[ 3 ][ 3 ] );

	const Vec3D &	operator[]( INT index ) const;
	Vec3D &			operator[]( INT index );
	Matrix3			operator-() const;
	Matrix3			operator*( const FLOAT a ) const;
	Vec3D			operator*( const Vec3D &vec ) const;
	Matrix3			operator*( const Matrix3 &a ) const;
	Matrix3			operator+( const Matrix3 &a ) const;
	Matrix3			operator-( const Matrix3 &a ) const;
	Matrix3 &		operator*=( const FLOAT a );
	Matrix3 &		operator*=( const Matrix3 &a );
	Matrix3 &		operator+=( const Matrix3 &a );
	Matrix3 &		operator-=( const Matrix3 &a );

	friend Matrix3	operator*( const FLOAT a, const Matrix3 &mat );
	friend Vec3D	operator*( const Vec3D &vec, const Matrix3 &mat );
	friend Vec3D &	operator*=( Vec3D &vec, const Matrix3 &mat );

	bool			Compare( const Matrix3 &a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix3 &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix3 &a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix3 &a ) const;					// exact compare, no epsilon

	void			Set( FLOAT xx, FLOAT xy, FLOAT xz,
						 FLOAT yx, FLOAT yy, FLOAT yz,
						 FLOAT zx, FLOAT zy, FLOAT zz );
	void			SetZero( void );
	void			SetIdentity( void );
	bool			IsIdentity( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated( void ) const;

	void			ProjectVector( const Vec3D &src, Vec3D &dst ) const;
	void			UnprojectVector( const Vec3D &src, Vec3D &dst ) const;

	bool			FixDegeneracies( void );	// fix degenerate axial cases
	bool			FixDenormals( void );		// change tiny numbers to zero

	FLOAT			Trace( void ) const;
	FLOAT			Determinant( void ) const;
	Matrix3			OrthoNormalize( void ) const;
	Matrix3 &		OrthoNormalizeSelf( void );
	Matrix3			Transpose( void ) const;	// returns transpose
	Matrix3 &		TransposeSelf( void );
	Matrix3			Inverse( void ) const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf( void );		// returns false if determinant is zero
	Matrix3			InverseFast( void ) const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf( void );	// returns false if determinant is zero
	Matrix3			TransposeMultiply( const Matrix3 &b ) const;

	Matrix3			InertiaTranslate( const FLOAT mass, const Vec3D &centerOfMass, const Vec3D &translation ) const;
	Matrix3 &		InertiaTranslateSelf( const FLOAT mass, const Vec3D &centerOfMass, const Vec3D &translation );
	Matrix3			InertiaRotate( const Matrix3 &rotation ) const;
	Matrix3 &		InertiaRotateSelf( const Matrix3 &rotation );

	INT				GetDimension( void ) const;

	Angles			ToAngles( void ) const;
	Quat			ToQuat( void ) const;
	CQuat			ToCQuat( void ) const;
	Rotation		ToRotation( void ) const;
	Matrix4			ToMat4( void ) const;
	Vec3D			ToAngularVelocity( void ) const;
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

	void	SetRotationX( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationY( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationZ( FLOAT angle );

	void	SetRotationAxis( FLOAT angle, const Vec3D& axis );

	friend void		TransposeMultiply( const Matrix3 &inv, const Matrix3 &b, Matrix3 &dst );
	friend Matrix3	SkewSymmetric( Vec3D const &src );

public:

	// Creates a rotation matrix that rotates vector 'from' into another vector 'to'.
	//
	static Matrix3	CreateRotation( const Vec3D& vFrom, const Vec3D& vTo );

public:
	static const Matrix3	mat3_zero;
	static const Matrix3	mat3_identity;

private:
	Vec3D		mColumns[ 3 ];	// stored in column-major order
};

mxDECLARE_BUILTIN_TYPE( Matrix3,	ETypeKind::Type_Matrix3 );

FORCEINLINE Matrix3::Matrix3( void ) {
}

FORCEINLINE Matrix3::Matrix3( const Vec3D &x, const Vec3D &y, const Vec3D &z ) {
	mColumns[ 0 ].x = x.x; mColumns[ 0 ].y = x.y; mColumns[ 0 ].z = x.z;
	mColumns[ 1 ].x = y.x; mColumns[ 1 ].y = y.y; mColumns[ 1 ].z = y.z;
	mColumns[ 2 ].x = z.x; mColumns[ 2 ].y = z.y; mColumns[ 2 ].z = z.z;
}

FORCEINLINE Matrix3::Matrix3( const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT zx, const FLOAT zy, const FLOAT zz ) {
	mColumns[ 0 ].x = xx; mColumns[ 0 ].y = xy; mColumns[ 0 ].z = xz;
	mColumns[ 1 ].x = yx; mColumns[ 1 ].y = yy; mColumns[ 1 ].z = yz;
	mColumns[ 2 ].x = zx; mColumns[ 2 ].y = zy; mColumns[ 2 ].z = zz;
}

FORCEINLINE Matrix3::Matrix3( const FLOAT src[ 3 ][ 3 ] ) {
	memcpy( mColumns, src, 3 * 3 * sizeof( FLOAT ) );
}

FORCEINLINE const Vec3D &Matrix3::operator[]( INT index ) const {
	//Assert( ( index >= 0 ) && ( index < 3 ) );
	return mColumns[ index ];
}

FORCEINLINE Vec3D &Matrix3::operator[]( INT index ) {
	//Assert( ( index >= 0 ) && ( index < 3 ) );
	return mColumns[ index ];
}

FORCEINLINE Matrix3 Matrix3::operator-() const {
	return Matrix3(	-mColumns[0][0], -mColumns[0][1], -mColumns[0][2],
					-mColumns[1][0], -mColumns[1][1], -mColumns[1][2],
					-mColumns[2][0], -mColumns[2][1], -mColumns[2][2] );
}

FORCEINLINE Vec3D Matrix3::operator*( const Vec3D &vec ) const {
	return Vec3D(
		mColumns[ 0 ].x * vec.x + mColumns[ 1 ].x * vec.y + mColumns[ 2 ].x * vec.z,
		mColumns[ 0 ].y * vec.x + mColumns[ 1 ].y * vec.y + mColumns[ 2 ].y * vec.z,
		mColumns[ 0 ].z * vec.x + mColumns[ 1 ].z * vec.y + mColumns[ 2 ].z * vec.z );
}

FORCEINLINE Matrix3 Matrix3::operator*( const Matrix3 &a ) const {
	INT i, j;
	const FLOAT *m1Ptr, *m2Ptr;
	FLOAT *dstPtr;
	Matrix3 dst;

	m1Ptr = reinterpret_cast<const FLOAT *>(this);
	m2Ptr = reinterpret_cast<const FLOAT *>(&a);
	dstPtr = reinterpret_cast<FLOAT *>(&dst);

#if 1
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
			dstPtr++;
		}
		m1Ptr += 3;
	}
#else //HUMANHEAD rww - sse test (unideal, no aligned calculation)
	__asm {
		//get our memory locations
		mov			eax, m1Ptr
		mov			ecx, m2Ptr
		mov			edx, dstPtr

	//=====iteration 1
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax]
		movss		xmm1, [eax+4]
		movss		xmm2, [eax+8]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+4], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+8], xmm3

	//=====iteration 2
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax+12]
		movss		xmm1, [eax+16]
		movss		xmm2, [eax+20]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+12], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+16], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+20], xmm3

	//=====iteration 3
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax+24]
		movss		xmm1, [eax+28]
		movss		xmm2, [eax+32]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+24], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+28], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+32], xmm3
	}
#endif //HUMANHEAD END

	return dst;
}

FORCEINLINE Matrix3 Matrix3::operator*( const FLOAT a ) const {
	return Matrix3(
		mColumns[0].x * a, mColumns[0].y * a, mColumns[0].z * a,
		mColumns[1].x * a, mColumns[1].y * a, mColumns[1].z * a,
		mColumns[2].x * a, mColumns[2].y * a, mColumns[2].z * a );
}

FORCEINLINE Matrix3 Matrix3::operator+( const Matrix3 &a ) const {
	return Matrix3(
		mColumns[0].x + a[0].x, mColumns[0].y + a[0].y, mColumns[0].z + a[0].z,
		mColumns[1].x + a[1].x, mColumns[1].y + a[1].y, mColumns[1].z + a[1].z,
		mColumns[2].x + a[2].x, mColumns[2].y + a[2].y, mColumns[2].z + a[2].z );
}
    
FORCEINLINE Matrix3 Matrix3::operator-( const Matrix3 &a ) const {
	return Matrix3(
		mColumns[0].x - a[0].x, mColumns[0].y - a[0].y, mColumns[0].z - a[0].z,
		mColumns[1].x - a[1].x, mColumns[1].y - a[1].y, mColumns[1].z - a[1].z,
		mColumns[2].x - a[2].x, mColumns[2].y - a[2].y, mColumns[2].z - a[2].z );
}

FORCEINLINE Matrix3 &Matrix3::operator*=( const FLOAT a ) {
	mColumns[0].x *= a; mColumns[0].y *= a; mColumns[0].z *= a;
	mColumns[1].x *= a; mColumns[1].y *= a; mColumns[1].z *= a; 
	mColumns[2].x *= a; mColumns[2].y *= a; mColumns[2].z *= a;

    return *this;
}

FORCEINLINE Matrix3 &Matrix3::operator*=( const Matrix3 &a ) {
	INT i, j;
	const FLOAT *m2Ptr;
	FLOAT *m1Ptr, dst[3];

	m1Ptr = reinterpret_cast<FLOAT *>(this);
	m2Ptr = reinterpret_cast<const FLOAT *>(&a);

#if 1
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			dst[j]  = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
		}
		m1Ptr[0] = dst[0]; m1Ptr[1] = dst[1]; m1Ptr[2] = dst[2];
		m1Ptr += 3;
	}
#else //HUMANHEAD rww - sse test (unideal, no aligned calculation)
	FLOAT *dstPtr = dst;
	__asm {
		//get our memory locations
		mov			eax, m1Ptr
		mov			ecx, m2Ptr
		mov			edx, dstPtr

	//=====iteration 1
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax]
		movss		xmm1, [eax+4]
		movss		xmm2, [eax+8]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+4], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[eax+8], xmm3

		//copy back to m1Ptr
		mov			ebx, [edx]
		mov			[eax], ebx
		mov			ebx, [edx+4]
		mov			[eax+4], ebx

	//=====iteration 2
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax+12]
		movss		xmm1, [eax+16]
		movss		xmm2, [eax+20]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+4], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[eax+20], xmm3

		//copy back to m1Ptr
		mov			ebx, [edx]
		mov			[eax+12], ebx
		mov			ebx, [edx+4]
		mov			[eax+16], ebx

	//=====iteration 3
		//load 3 elements of mColumns a into sse regs
		movss		xmm0, [eax+24]
		movss		xmm1, [eax+28]
		movss		xmm2, [eax+32]

		//-----iteration 1
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx]		//0 * 3 + 0
		movss		xmm4, [ecx+12]	//1 * 3 + 0
		movss		xmm5, [ecx+24]	//2 * 3 + 0
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx], xmm3

		//-----iteration 2
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+4]	//0 * 3 + 1
		movss		xmm4, [ecx+16]	//1 * 3 + 1
		movss		xmm5, [ecx+28]	//2 * 3 + 1
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[edx+4], xmm3

		//-----iteration 3
		//load 3 elements of mColumns b into sse regs
		movss		xmm3, [ecx+8]	//0 * 3 + 2
		movss		xmm4, [ecx+20]	//1 * 3 + 2
		movss		xmm5, [ecx+32]	//2 * 3 + 2
		//multiply the elements
		mulss		xmm3, xmm0
		mulss		xmm4, xmm1
		mulss		xmm5, xmm2
		//add
		addss		xmm3, xmm4
		addss		xmm3, xmm5
		//store result to dest
		movss		[eax+32], xmm3

		//copy back to m1Ptr
		mov			ebx, [edx]
		mov			[eax+24], ebx
		mov			ebx, [edx+4]
		mov			[eax+28], ebx
	}
#endif //HUMANHEAD END

	return *this;
}

FORCEINLINE Matrix3 &Matrix3::operator+=( const Matrix3 &a ) {
	mColumns[0].x += a[0].x; mColumns[0].y += a[0].y; mColumns[0].z += a[0].z;
	mColumns[1].x += a[1].x; mColumns[1].y += a[1].y; mColumns[1].z += a[1].z;
	mColumns[2].x += a[2].x; mColumns[2].y += a[2].y; mColumns[2].z += a[2].z;

    return *this;
}

FORCEINLINE Matrix3 &Matrix3::operator-=( const Matrix3 &a ) {
	mColumns[0].x -= a[0].x; mColumns[0].y -= a[0].y; mColumns[0].z -= a[0].z;
	mColumns[1].x -= a[1].x; mColumns[1].y -= a[1].y; mColumns[1].z -= a[1].z;
	mColumns[2].x -= a[2].x; mColumns[2].y -= a[2].y; mColumns[2].z -= a[2].z;

    return *this;
}

FORCEINLINE Vec3D operator*( const Vec3D &vec, const Matrix3 &mat ) {
	return mat * vec;
}

FORCEINLINE Matrix3 operator*( const FLOAT a, const Matrix3 &mat ) {
	return mat * a;
}

FORCEINLINE Vec3D &operator*=( Vec3D &vec, const Matrix3 &mat ) {
	FLOAT x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
	FLOAT y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
	vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
	vec.x = x;
	vec.y = y;
	return vec;
}

FORCEINLINE bool Matrix3::Compare( const Matrix3 &a ) const {
	if ( mColumns[0].Compare( a[0] ) &&
		mColumns[1].Compare( a[1] ) &&
		mColumns[2].Compare( a[2] ) ) {
		return true;
	}
	return false;
}

FORCEINLINE bool Matrix3::Compare( const Matrix3 &a, const FLOAT epsilon ) const {
	if ( mColumns[0].Compare( a[0], epsilon ) &&
		mColumns[1].Compare( a[1], epsilon ) &&
		mColumns[2].Compare( a[2], epsilon ) ) {
		return true;
	}
	return false;
}

FORCEINLINE bool Matrix3::operator==( const Matrix3 &a ) const {
	return Compare( a );
}

FORCEINLINE bool Matrix3::operator!=( const Matrix3 &a ) const {
	return !Compare( a );
}

FORCEINLINE
void Matrix3::Set(
		FLOAT e00, FLOAT e01, FLOAT e02,
		FLOAT e10, FLOAT e11, FLOAT e12,
		FLOAT e20, FLOAT e21, FLOAT e22 )
{
	mColumns[0].Set( e00, e10, e20 );
	mColumns[1].Set( e01, e11, e21 );
	mColumns[2].Set( e02, e12, e22 );
}

FORCEINLINE void Matrix3::SetZero( void ) {
	MemSet( mColumns, 0, sizeof( Matrix3 ) );
}

FORCEINLINE void Matrix3::SetIdentity( void ) {
	*this = mat3_identity;
}

FORCEINLINE bool Matrix3::IsIdentity( const FLOAT epsilon ) const {
	return Compare( mat3_identity, epsilon );
}

FORCEINLINE bool Matrix3::IsSymmetric( const FLOAT epsilon ) const {
	if ( mxFabs( mColumns[0][1] - mColumns[1][0] ) > epsilon ) {
		return false;
	}
	if ( mxFabs( mColumns[0][2] - mColumns[2][0] ) > epsilon ) {
		return false;
	}
	if ( mxFabs( mColumns[1][2] - mColumns[2][1] ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Matrix3::IsDiagonal( const FLOAT epsilon ) const {
	if ( mxFabs( mColumns[0][1] ) > epsilon ||
		mxFabs( mColumns[0][2] ) > epsilon ||
		mxFabs( mColumns[1][0] ) > epsilon ||
		mxFabs( mColumns[1][2] ) > epsilon ||
		mxFabs( mColumns[2][0] ) > epsilon ||
		mxFabs( mColumns[2][1] ) > epsilon ) {
		return false;
	}
	return true;
}

FORCEINLINE bool Matrix3::IsRotated( void ) const {
	return !Compare( mat3_identity );
}

FORCEINLINE void Matrix3::ProjectVector( const Vec3D &src, Vec3D &dst ) const {
	dst.x = src * mColumns[ 0 ];
	dst.y = src * mColumns[ 1 ];
	dst.z = src * mColumns[ 2 ];
}

FORCEINLINE void Matrix3::UnprojectVector( const Vec3D &src, Vec3D &dst ) const {
	dst = mColumns[ 0 ] * src.x + mColumns[ 1 ] * src.y + mColumns[ 2 ] * src.z;
}

FORCEINLINE bool Matrix3::FixDegeneracies( void ) {
	bool r = mColumns[0].FixDegenerateNormal();
	r |= mColumns[1].FixDegenerateNormal();
	r |= mColumns[2].FixDegenerateNormal();
	return r;
}

FORCEINLINE bool Matrix3::FixDenormals( void ) {
	bool r = mColumns[0].FixDenormals();
	r |= mColumns[1].FixDenormals();
	r |= mColumns[2].FixDenormals();
	return r;
}

FORCEINLINE FLOAT Matrix3::Trace( void ) const {
	return ( mColumns[0][0] + mColumns[1][1] + mColumns[2][2] );
}

FORCEINLINE Matrix3 Matrix3::OrthoNormalize( void ) const {
	Matrix3 ortho;

	ortho = *this;
	ortho[ 0 ].Normalize();
	ortho[ 2 ].Cross( mColumns[ 0 ], mColumns[ 1 ] );
	ortho[ 2 ].Normalize();
	ortho[ 1 ].Cross( mColumns[ 2 ], mColumns[ 0 ] );
	ortho[ 1 ].Normalize();
	return ortho;
}

FORCEINLINE Matrix3 &Matrix3::OrthoNormalizeSelf( void ) {
	mColumns[ 0 ].Normalize();
	mColumns[ 2 ].Cross( mColumns[ 0 ], mColumns[ 1 ] );
	mColumns[ 2 ].Normalize();
	mColumns[ 1 ].Cross( mColumns[ 2 ], mColumns[ 0 ] );
	mColumns[ 1 ].Normalize();
	return *this;
}

FORCEINLINE Matrix3 Matrix3::Transpose( void ) const {
	return Matrix3(	mColumns[0][0], mColumns[1][0], mColumns[2][0],
					mColumns[0][1], mColumns[1][1], mColumns[2][1],
					mColumns[0][2], mColumns[1][2], mColumns[2][2] );
}

FORCEINLINE Matrix3 &Matrix3::TransposeSelf( void ) {
	FLOAT tmp0, tmp1, tmp2;

	tmp0 = mColumns[0][1];
	mColumns[0][1] = mColumns[1][0];
	mColumns[1][0] = tmp0;
	tmp1 = mColumns[0][2];
	mColumns[0][2] = mColumns[2][0];
	mColumns[2][0] = tmp1;
	tmp2 = mColumns[1][2];
	mColumns[1][2] = mColumns[2][1];
	mColumns[2][1] = tmp2;

	return *this;
}

FORCEINLINE Matrix3 Matrix3::Inverse( void ) const {
	Matrix3 invMat;

	invMat = *this;
	//HH rww SDK - removed unnecessary variable initialization
	invMat.InverseSelf();
	return invMat;
}

FORCEINLINE Matrix3 Matrix3::InverseFast( void ) const {
	Matrix3 invMat;

	invMat = *this;
	//HH rww SDK - removed unnecessary variable initialization
	invMat.InverseFastSelf();
	return invMat;
}

FORCEINLINE Matrix3 Matrix3::TransposeMultiply( const Matrix3 &b ) const {
	return Matrix3(	mColumns[0].x * b[0].x + mColumns[1].x * b[1].x + mColumns[2].x * b[2].x,
					mColumns[0].x * b[0].y + mColumns[1].x * b[1].y + mColumns[2].x * b[2].y,
					mColumns[0].x * b[0].z + mColumns[1].x * b[1].z + mColumns[2].x * b[2].z,
					mColumns[0].y * b[0].x + mColumns[1].y * b[1].x + mColumns[2].y * b[2].x,
					mColumns[0].y * b[0].y + mColumns[1].y * b[1].y + mColumns[2].y * b[2].y,
					mColumns[0].y * b[0].z + mColumns[1].y * b[1].z + mColumns[2].y * b[2].z,
					mColumns[0].z * b[0].x + mColumns[1].z * b[1].x + mColumns[2].z * b[2].x,
					mColumns[0].z * b[0].y + mColumns[1].z * b[1].y + mColumns[2].z * b[2].y,
					mColumns[0].z * b[0].z + mColumns[1].z * b[1].z + mColumns[2].z * b[2].z );
}

FORCEINLINE void TransposeMultiply( const Matrix3 &transpose, const Matrix3 &b, Matrix3 &dst ) {
	dst[0].x = transpose[0].x * b[0].x + transpose[1].x * b[1].x + transpose[2].x * b[2].x;
	dst[0].y = transpose[0].x * b[0].y + transpose[1].x * b[1].y + transpose[2].x * b[2].y;
	dst[0].z = transpose[0].x * b[0].z + transpose[1].x * b[1].z + transpose[2].x * b[2].z;
	dst[1].x = transpose[0].y * b[0].x + transpose[1].y * b[1].x + transpose[2].y * b[2].x;
	dst[1].y = transpose[0].y * b[0].y + transpose[1].y * b[1].y + transpose[2].y * b[2].y;
	dst[1].z = transpose[0].y * b[0].z + transpose[1].y * b[1].z + transpose[2].y * b[2].z;
	dst[2].x = transpose[0].z * b[0].x + transpose[1].z * b[1].x + transpose[2].z * b[2].x;
	dst[2].y = transpose[0].z * b[0].y + transpose[1].z * b[1].y + transpose[2].z * b[2].y;
	dst[2].z = transpose[0].z * b[0].z + transpose[1].z * b[1].z + transpose[2].z * b[2].z;
}

FORCEINLINE Matrix3 SkewSymmetric( Vec3D const &src ) {
	return Matrix3(
		0.0f,	-src.z,	src.y,
		src.z,	0.0f,	-src.x,
		-src.y,	src.x,	0.0f
	);
}

FORCEINLINE INT Matrix3::GetDimension( void ) const {
	return 9;
}

FORCEINLINE const FLOAT *Matrix3::ToFloatPtr( void ) const {
	return mColumns[0].ToFloatPtr();
}

FORCEINLINE FLOAT *Matrix3::ToFloatPtr( void ) {
	return mColumns[0].ToFloatPtr();
}

mxNAMESPACE_END

#endif /* !__MATH_MATRIX_3x3_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
