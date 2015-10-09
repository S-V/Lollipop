/*
=============================================================================
	File:	Matrix4x4.h
	Desc:	Row-major float4x4 matrix.

	Originally written by Id Software.
	Copyright (C) 2004 Id Software, Inc. ( idMat4 )
=============================================================================
*/

#ifndef __MATH_MATRIX_4x4_H__
#define __MATH_MATRIX_4x4_H__
mxSWIPED("idSoftware");

mxNAMESPACE_BEGIN

//===============================================================
//
//	Matrix4 - 4x4 matrix ( row-major, DirectX-style ).
//
//===============================================================

mxALIGN_16( class Matrix4 ) {
public:
					Matrix4( void );
					
					explicit Matrix4( const Vec4D &x, const Vec4D &y, const Vec4D &z, const Vec4D &w );
					
					explicit Matrix4(const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
									const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
									const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
									const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww );

					explicit Matrix4( const Matrix3 &rotation, const Vec3D &translation );
					
					explicit Matrix4( const FLOAT src[ 4 ][ 4 ] );
					explicit Matrix4( const FLOAT * m );
					
					explicit Matrix4( EInitIdentity );

	const Vec4D &	operator []( INT index ) const;
	Vec4D &			operator []( INT index );
	Matrix4			operator *( const FLOAT a ) const;
	Vec4D			operator *( const Vec4D &vec ) const;
	Vec3D			operator *( const Vec3D &vec ) const;
	Matrix4			operator *( const Matrix4 &a ) const;
	Matrix4			operator +( const Matrix4 &a ) const;
	Matrix4			operator -( const Matrix4 &a ) const;
	Matrix4 &		operator *=( const FLOAT a );
	Matrix4 &		operator *=( const Matrix4 &a );
	Matrix4 &		operator +=( const Matrix4 &a );
	Matrix4 &		operator -=( const Matrix4 &a );

	friend Matrix4	operator * ( const FLOAT a, const Matrix4 &mat );
	friend Vec4D	operator * ( const Vec4D &vec, const Matrix4 &mat );
	friend Vec3D	operator * ( const Vec3D &vec, const Matrix4 &mat );
	friend Vec4D &	operator *=( Vec4D &vec, const Matrix4 &mat );
	friend Vec3D &	operator *=( Vec3D &vec, const Matrix4 &mat );

	bool			Compare( const Matrix4 &a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix4 &a, const FLOAT epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix4 &a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix4 &a ) const;					// exact compare, no epsilon

	void			Set(const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
						const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
						const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
						const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww );

	void			SetZero( void );
	void			SetIdentity( void );
	bool			IsIdentity( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const FLOAT epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated( void ) const;

	void			ProjectVector( const Vec4D &src, Vec4D &dst ) const;
	void			UnprojectVector( const Vec4D &src, Vec4D &dst ) const;

	FLOAT			Trace( void ) const;		// Returns the sum of diagonal components.
	FLOAT			Determinant( void ) const;
	Matrix4			Transpose( void ) const;	// returns transpose
	Matrix4 &		TransposeSelf( void );
	Matrix4			Inverse( void ) const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf( void );		// returns false if determinant is zero
	Matrix4			InverseFast( void ) const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf( void );	// returns false if determinant is zero
	Matrix4			TransposeMultiply( const Matrix4 &b ) const;
	Matrix4			GetAbsolute() const;		// Returns a matrix with non-negative values.

	void			SetRotationInDegrees( const Vec3D& rotation );
	void			SetRotationInRadians( const Vec3D& rotation );
	Vec3D			GetRotationInDegrees() const;

			// Builds a left-handed view matrix.
	void	BuildLookAtLH(
				const Vec3D& eyePosition,
				const Vec3D& focusPosition,
				const Vec3D& upVector = Vec3D::vec3_unit_y
			);


			// Builds a left-handed view matrix.
	void	BuildLookAtLH(
				const Vec3D& origin,
				const Vec3D& xAxis, const Vec3D& yAxis, const Vec3D& zAxis
			);

			// Builds a left-handed view matrix.
	void	BuildViewLH(
				const Vec3D& eyePosition,
				const Vec3D& lookAt,
				const Vec3D& upVector = Vec3D::vec3_unit_y
			);

			// Builds a left-handed view matrix assuming this is a valid world matrix (i.e. coordinate frame).
	void	BuildInverseWorldMatrix();

			// Builds a left-handed perspective projection matrix based on a field of view.
	void	BuildPerspectiveLH(
				FLOAT FOVy,
				FLOAT Aspect,
				FLOAT NearZ, FLOAT FarZ
			);

			// Builds a left-handed orthographic projection matrix.
	void	BuildOrthoLH(
				FLOAT Width,
				FLOAT Height,
				FLOAT NearZ, FLOAT FarZ
			);

	void	BuildOrthoOffCenterLH(
				FLOAT minX, FLOAT maxX,
				FLOAT minY, FLOAT maxY,
				FLOAT nearZ, FLOAT farZ
			);

	void	BuildViewportTransform(
				const FLOAT TopLeftX, const FLOAT TopLeftY,
				const FLOAT Width, const FLOAT Height,
				const FLOAT MinDepth, const FLOAT MaxDepth
			);

			// Makes a transformation matrix with Scaling, Rotation and Translation.
	void	BuildTransform(
				const Vec3D& translation,
				const Quat& rotation,
				const Vec3D& scale
			);

			// Makes a translation and rotation matrix.
	void	BuildTransform(
				const Vec3D& translation,
				const Quat& rotation
			);

					// Transforms a point.
	Vec3D			TransformVector( const Vec3D& point ) const;

					// Note: assumes that the matrix is composed from translation and rotation (and, maybe, uniform scaling).
	Vec3D			InverseTransformVector( const Vec3D& point ) const;

					// Transforms a normal ( direction ).
	Vec3D			TransformNormal( const Vec3D& normal ) const;

					// Note: assumes that the matrix is composed from translation and rotation (and, maybe, uniform scaling).
	Vec3D			InverseTransformNormal( const Vec3D& normal ) const;

	Vec4D			GetTransformedPlane( const Vec4D& plane ) const;

					// The same as TransformNormal(), provided for convenience.
	void			RotateVector( Vec3D & /* in out */ v ) const { v = TransformNormal( v ); }

	INT				GetDimension( void ) const;
	bool			IsAffine() const;	// An affine matrix has no projective coefficients.

	Matrix3			ToMat3() const;	// returns the rotation part of the matrix
	Quat			ToQuat() const;	// extracts quaternion from the matrix

	void			SetRow( UINT iRow, const Vec4D& v );
	const FLOAT *	ToFloatPtr( void ) const;
	FLOAT *			ToFloatPtr( void );
	const char *	ToChars( INT precision = 2 ) const;

	void			SetTranslation( const Vec3D& v );
	const Vec3D &	GetTranslation() const;

	const Vec3D &	GetWorldRight() const;
	const Vec3D &	GetWorldUp() const;
	const Vec3D &	GetWorldAhead() const;
	const Vec3D &	GetWorldOrigin() const;

					// this function is provided for convenience
	void			SetOrigin( const Vec3D& newOrigin ) { this->SetTranslation( newOrigin ); }

	void			SetScale( const Vec3D& scaleFactor );	// Sets the scale part of the matrix.
	void			SetDiagonal( const Vec3D& v );

	void	SetRotationX( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationY( FLOAT angle );						// NOTE: Angle in radians.
	void	SetRotationZ( FLOAT angle );						// NOTE: Angle in radians.
	
	void	SetRotationAxis( FLOAT angle, const Vec3D& axis );	// NOTE: Angle in radians.

	// Builds a matrix than rotates from one vector to another vector.
	void	SetRotationAxis( FLOAT angle, const Vec3D& vFrom, const Vec3D& vTo );	// NOTE: Angle in radians.

	Vec3D	HomogeneousMultiply( const Vec3D& v, FLOAT w ) const;

public:
	static Matrix4	CreateScale( FLOAT uniformScaleFactor );
	static Matrix4	CreateScale( const Vec3D& scaleFactor );

	static Matrix4	CreateTranslation( const Vec3D& translation );

public:
	static const Matrix4	mat4_zero;
	static const Matrix4	mat4_identity;

	// takes 2D clipspace {-1,1} to {0,1} and inverts Y.
	static const Matrix4	mat4_clip_space_to_image_space;

	// takes 2D imagespace {0,1} to {-1,1} and inverts Y.
	static const Matrix4	mat4_image_space_to_clip_space;

protected:
	mxALIGN_16( Vec4D		mRows[ 4 ] );
};

mxDECLARE_BUILTIN_TYPE( Matrix4,	ETypeKind::Type_Matrix4 );

FORCEINLINE Matrix4::Matrix4( void ) {
}

FORCEINLINE Matrix4::Matrix4( const Vec4D &x, const Vec4D &y, const Vec4D &z, const Vec4D &w )
{
	mRows[ 0 ] = x;
	mRows[ 1 ] = y;
	mRows[ 2 ] = z;
	mRows[ 3 ] = w;
}

FORCEINLINE Matrix4::Matrix4( const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
							const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
							const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
							const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww )
{
	mRows[0][0] = xx; mRows[0][1] = xy; mRows[0][2] = xz; mRows[0][3] = xw;
	mRows[1][0] = yx; mRows[1][1] = yy; mRows[1][2] = yz; mRows[1][3] = yw;
	mRows[2][0] = zx; mRows[2][1] = zy; mRows[2][2] = zz; mRows[2][3] = zw;
	mRows[3][0] = wx; mRows[3][1] = wy; mRows[3][2] = wz; mRows[3][3] = ww;
}

FORCEINLINE Matrix4::Matrix4( const Matrix3 &rotation, const Vec3D &translation )
{
	// NOTE: Matrix3 is transposed because it is column-major.
	mRows[ 0 ][ 0 ] = rotation[0][0];
	mRows[ 0 ][ 1 ] = rotation[1][0];
	mRows[ 0 ][ 2 ] = rotation[2][0];
	mRows[ 0 ][ 3 ] = 0.0f;
	mRows[ 1 ][ 0 ] = rotation[0][1];
	mRows[ 1 ][ 1 ] = rotation[1][1];
	mRows[ 1 ][ 2 ] = rotation[2][1];
	mRows[ 1 ][ 3 ] = 0.0f;
	mRows[ 2 ][ 0 ] = rotation[0][2];
	mRows[ 2 ][ 1 ] = rotation[1][2];
	mRows[ 2 ][ 2 ] = rotation[2][2];
	mRows[ 2 ][ 3 ] = 0.0f;
	mRows[ 3 ][ 0 ] = translation[0];
	mRows[ 3 ][ 1 ] = translation[1];
	mRows[ 3 ][ 2 ] = translation[2];
	mRows[ 3 ][ 3 ] = 1.0f;
}

FORCEINLINE Matrix4::Matrix4( const FLOAT src[ 4 ][ 4 ] ) {
	MemCopy( mRows, src, 4 * 4 * sizeof( FLOAT ) );
}

FORCEINLINE Matrix4::Matrix4( const FLOAT * m )
{
	mRows[0][0] = m[ 0]; mRows[0][1] = m[ 4]; mRows[0][2] = m[ 8]; mRows[0][3] = m[12];
	mRows[1][0] = m[ 1]; mRows[1][1] = m[ 5]; mRows[1][2] = m[ 9]; mRows[1][3] = m[13];
	mRows[2][0] = m[ 2]; mRows[2][1] = m[ 6]; mRows[2][2] = m[10]; mRows[2][3] = m[14];
	mRows[3][0] = m[ 3]; mRows[3][1] = m[ 7]; mRows[3][2] = m[11]; mRows[3][3] = m[15];
}

FORCEINLINE Matrix4::Matrix4( EInitIdentity )
{
	*this = mat4_identity;
}

FORCEINLINE const Vec4D &Matrix4::operator[]( INT index ) const {
	//Assert( ( index >= 0 ) && ( index < 4 ) );
	return mRows[ index ];
}

FORCEINLINE Vec4D &Matrix4::operator[]( INT index ) {
	//Assert( ( index >= 0 ) && ( index < 4 ) );
	return mRows[ index ];
}

FORCEINLINE Matrix4 Matrix4::operator*( const FLOAT a ) const {
	return Matrix4(
		mRows[0].x * a, mRows[0].y * a, mRows[0].z * a, mRows[0].w * a,
		mRows[1].x * a, mRows[1].y * a, mRows[1].z * a, mRows[1].w * a,
		mRows[2].x * a, mRows[2].y * a, mRows[2].z * a, mRows[2].w * a,
		mRows[3].x * a, mRows[3].y * a, mRows[3].z * a, mRows[3].w * a );
}

FORCEINLINE Vec4D Matrix4::operator*( const Vec4D &vec ) const
{
	return Vec4D(
		mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w * vec.w,
		mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w * vec.w,
		mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w * vec.w,
		mRows[ 3 ].x * vec.x + mRows[ 3 ].y * vec.y + mRows[ 3 ].z * vec.z + mRows[ 3 ].w * vec.w );
}

FORCEINLINE Vec3D Matrix4::operator*( const Vec3D &vec ) const
{
	FLOAT s = mRows[ 3 ].x * vec.x + mRows[ 3 ].y * vec.y + mRows[ 3 ].z * vec.z + mRows[ 3 ].w;
	if ( s == 0.0f ) {
		return Vec3D( 0.0f, 0.0f, 0.0f );
	}
	if ( s == 1.0f ) {
		return Vec3D(
			mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w,
			mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w,
			mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w );
	}
	else {
		FLOAT invS = 1.0f / s;
		return Vec3D(
			(mRows[ 0 ].x * vec.x + mRows[ 0 ].y * vec.y + mRows[ 0 ].z * vec.z + mRows[ 0 ].w) * invS,
			(mRows[ 1 ].x * vec.x + mRows[ 1 ].y * vec.y + mRows[ 1 ].z * vec.z + mRows[ 1 ].w) * invS,
			(mRows[ 2 ].x * vec.x + mRows[ 2 ].y * vec.y + mRows[ 2 ].z * vec.z + mRows[ 2 ].w) * invS );
	}
}

FORCEINLINE Matrix4 Matrix4::operator*( const Matrix4 &a ) const
{
	INT i, j;
	const FLOAT *m1Ptr, *m2Ptr;
	FLOAT *dstPtr;
	Matrix4 dst;

	m1Ptr = reinterpret_cast<const FLOAT *>(this);
	m2Ptr = reinterpret_cast<const FLOAT *>(&a);
	dstPtr = reinterpret_cast<FLOAT *>(&dst);
mxOPTIMIZE("unroll loops and help RVO, but better use new XNA/D3D11 math");
	for ( i = 0; i < 4; i++ ) {
		for ( j = 0; j < 4; j++ ) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
					+ m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
			dstPtr++;
		}
		m1Ptr += 4;
	}
	return dst;
}

FORCEINLINE Matrix4 Matrix4::operator+( const Matrix4 &a ) const {
	return Matrix4( 
		mRows[0].x + a[0].x, mRows[0].y + a[0].y, mRows[0].z + a[0].z, mRows[0].w + a[0].w,
		mRows[1].x + a[1].x, mRows[1].y + a[1].y, mRows[1].z + a[1].z, mRows[1].w + a[1].w,
		mRows[2].x + a[2].x, mRows[2].y + a[2].y, mRows[2].z + a[2].z, mRows[2].w + a[2].w,
		mRows[3].x + a[3].x, mRows[3].y + a[3].y, mRows[3].z + a[3].z, mRows[3].w + a[3].w );
}
    
FORCEINLINE Matrix4 Matrix4::operator-( const Matrix4 &a ) const {
	return Matrix4( 
		mRows[0].x - a[0].x, mRows[0].y - a[0].y, mRows[0].z - a[0].z, mRows[0].w - a[0].w,
		mRows[1].x - a[1].x, mRows[1].y - a[1].y, mRows[1].z - a[1].z, mRows[1].w - a[1].w,
		mRows[2].x - a[2].x, mRows[2].y - a[2].y, mRows[2].z - a[2].z, mRows[2].w - a[2].w,
		mRows[3].x - a[3].x, mRows[3].y - a[3].y, mRows[3].z - a[3].z, mRows[3].w - a[3].w );
}

FORCEINLINE Matrix4 &Matrix4::operator*=( const FLOAT a ) {
	mRows[0].x *= a; mRows[0].y *= a; mRows[0].z *= a; mRows[0].w *= a;
	mRows[1].x *= a; mRows[1].y *= a; mRows[1].z *= a; mRows[1].w *= a;
	mRows[2].x *= a; mRows[2].y *= a; mRows[2].z *= a; mRows[2].w *= a;
	mRows[3].x *= a; mRows[3].y *= a; mRows[3].z *= a; mRows[3].w *= a;
    return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator*=( const Matrix4 &a ) {
	*this = (*this) * a;
	return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator+=( const Matrix4 &a ) {
	mRows[0].x += a[0].x; mRows[0].y += a[0].y; mRows[0].z += a[0].z; mRows[0].w += a[0].w;
	mRows[1].x += a[1].x; mRows[1].y += a[1].y; mRows[1].z += a[1].z; mRows[1].w += a[1].w;
	mRows[2].x += a[2].x; mRows[2].y += a[2].y; mRows[2].z += a[2].z; mRows[2].w += a[2].w;
	mRows[3].x += a[3].x; mRows[3].y += a[3].y; mRows[3].z += a[3].z; mRows[3].w += a[3].w;
    return *this;
}

FORCEINLINE Matrix4 &Matrix4::operator-=( const Matrix4 &a ) {
	mRows[0].x -= a[0].x; mRows[0].y -= a[0].y; mRows[0].z -= a[0].z; mRows[0].w -= a[0].w;
	mRows[1].x -= a[1].x; mRows[1].y -= a[1].y; mRows[1].z -= a[1].z; mRows[1].w -= a[1].w;
	mRows[2].x -= a[2].x; mRows[2].y -= a[2].y; mRows[2].z -= a[2].z; mRows[2].w -= a[2].w;
	mRows[3].x -= a[3].x; mRows[3].y -= a[3].y; mRows[3].z -= a[3].z; mRows[3].w -= a[3].w;
    return *this;
}

FORCEINLINE Matrix4 operator*( const FLOAT a, const Matrix4 &mat ) {
	return mat * a;
}

FORCEINLINE Vec4D operator*( const Vec4D &vec, const Matrix4 &mat ) {
	return mat * vec;
}

FORCEINLINE Vec3D operator*( const Vec3D &vec, const Matrix4 &mat ) {
	return mat * vec;
}

FORCEINLINE Vec4D &operator*=( Vec4D &vec, const Matrix4 &mat ) {
	vec = mat * vec;
	return vec;
}

FORCEINLINE Vec3D &operator*=( Vec3D &vec, const Matrix4 &mat ) {
	vec = mat * vec;
	return vec;
}

FORCEINLINE bool Matrix4::Compare( const Matrix4 &a ) const {
	UINT i;
	const FLOAT *ptr1, *ptr2;

	ptr1 = reinterpret_cast<const FLOAT *>(mRows);
	ptr2 = reinterpret_cast<const FLOAT *>(a.mRows);
	for ( i = 0; i < 4*4; i++ ) {
		if ( ptr1[i] != ptr2[i] ) {
			return false;
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::Compare( const Matrix4 &a, const FLOAT epsilon ) const {
	dword i;
	const FLOAT *ptr1, *ptr2;

	ptr1 = reinterpret_cast<const FLOAT *>(mRows);
	ptr2 = reinterpret_cast<const FLOAT *>(a.mRows);
	for ( i = 0; i < 4*4; i++ ) {
		if ( mxFabs( ptr1[i] - ptr2[i] ) > epsilon ) {
			return false;
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::operator==( const Matrix4 &a ) const {
	return Compare( a );
}

FORCEINLINE bool Matrix4::operator!=( const Matrix4 &a ) const {
	return !Compare( a );
}

FORCEINLINE void Matrix4::Set(
	const FLOAT xx, const FLOAT xy, const FLOAT xz, const FLOAT xw,
	const FLOAT yx, const FLOAT yy, const FLOAT yz, const FLOAT yw,
	const FLOAT zx, const FLOAT zy, const FLOAT zz, const FLOAT zw,
	const FLOAT wx, const FLOAT wy, const FLOAT wz, const FLOAT ww )
{
	mRows[0][0] = xx; mRows[0][1] = xy; mRows[0][2] = xz; mRows[0][3] = xw;
	mRows[1][0] = yx; mRows[1][1] = yy; mRows[1][2] = yz; mRows[1][3] = yw;
	mRows[2][0] = zx; mRows[2][1] = zy; mRows[2][2] = zz; mRows[2][3] = zw;
	mRows[3][0] = wx; mRows[3][1] = wy; mRows[3][2] = wz; mRows[3][3] = ww;
}

FORCEINLINE void Matrix4::SetZero( void ) {
	memset( mRows, 0, sizeof( Matrix4 ) );
}

FORCEINLINE void Matrix4::SetIdentity( void ) {
	*this = mat4_identity;
}

FORCEINLINE bool Matrix4::IsIdentity( const FLOAT epsilon ) const {
	return Compare( mat4_identity, epsilon );
}

FORCEINLINE bool Matrix4::IsSymmetric( const FLOAT epsilon ) const {
	for ( INT i = 1; i < 4; i++ ) {
		for ( INT j = 0; j < i; j++ ) {
			if ( mxFabs( mRows[i][j] - mRows[j][i] ) > epsilon ) {
				return false;
			}
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::IsDiagonal( const FLOAT epsilon ) const {
	for ( INT i = 0; i < 4; i++ ) {
		for ( INT j = 0; j < 4; j++ ) {
			if ( i != j && mxFabs( mRows[i][j] ) > epsilon ) {
				return false;
			}
		}
	}
	return true;
}

FORCEINLINE bool Matrix4::IsRotated( void ) const {
	if ( !mRows[ 0 ][ 1 ] && !mRows[ 0 ][ 2 ] &&
		!mRows[ 1 ][ 0 ] && !mRows[ 1 ][ 2 ] &&
		!mRows[ 2 ][ 0 ] && !mRows[ 2 ][ 1 ] ) {
		return false;
	}
	return true;
}

FORCEINLINE void Matrix4::ProjectVector( const Vec4D &src, Vec4D &dst ) const {
	dst.x = src * mRows[ 0 ];
	dst.y = src * mRows[ 1 ];
	dst.z = src * mRows[ 2 ];
	dst.w = src * mRows[ 3 ];
}

FORCEINLINE void Matrix4::UnprojectVector( const Vec4D &src, Vec4D &dst ) const {
	dst = mRows[ 0 ] * src.x + mRows[ 1 ] * src.y + mRows[ 2 ] * src.z + mRows[ 3 ] * src.w;
}

FORCEINLINE FLOAT Matrix4::Trace( void ) const {
	return ( mRows[0][0] + mRows[1][1] + mRows[2][2] + mRows[3][3] );
}

FORCEINLINE Matrix4 Matrix4::Inverse( void ) const
{
	Matrix4 invMat;
	invMat = *this;
	invMat.InverseSelf();
	return invMat;
}

FORCEINLINE Matrix4 Matrix4::InverseFast( void ) const {
	Matrix4 invMat;

	invMat = *this;
	invMat.InverseFastSelf();
	return invMat;
}

FORCEINLINE Matrix4 Matrix3::ToMat4( void ) const {
	// NOTE: Matrix3 is transposed because it is column-major.
	return Matrix4(	mColumns[0][0],	mColumns[1][0],	mColumns[2][0],	0.0f,
					mColumns[0][1],	mColumns[1][1],	mColumns[2][1],	0.0f,
					mColumns[0][2],	mColumns[1][2],	mColumns[2][2],	0.0f,
					0.0f,			0.0f,			0.0f,			1.0f );
}

FORCEINLINE INT Matrix4::GetDimension( void ) const {
	return 16;
}

FORCEINLINE Matrix3 Matrix4::ToMat3() const
{
	// NOTE: matrix is transposed because Matrix3 column-major
	return Matrix3(	mRows[0][0],	mRows[1][0],	mRows[2][0],
					mRows[0][1],	mRows[1][1],	mRows[2][1],
					mRows[0][2],	mRows[1][2],	mRows[2][2] );
}

FORCEINLINE void Matrix4::SetRow( UINT iRow, const Vec4D& v )
{
	mRows[ iRow ] = v;
}

FORCEINLINE const FLOAT *Matrix4::ToFloatPtr( void ) const {
	return mRows[0].ToFloatPtr();
}

FORCEINLINE FLOAT *Matrix4::ToFloatPtr( void ) {
	return mRows[0].ToFloatPtr();
}

FORCEINLINE void Matrix4::SetTranslation( const Vec3D& v ) {
	mRows[3].x = v.x;
	mRows[3].y = v.y;
	mRows[3].z = v.z;
}

FORCEINLINE const Vec3D & Matrix4::GetTranslation() const {
	return *(const Vec3D*) & mRows[3]; // the fourth row
}

FORCEINLINE const Vec3D & Matrix4::GetWorldRight() const {
	return mRows[0].ToVec3();
}

FORCEINLINE const Vec3D & Matrix4::GetWorldUp() const {
	return mRows[1].ToVec3();
}

FORCEINLINE const Vec3D & Matrix4::GetWorldAhead() const {
	return mRows[2].ToVec3();
}

FORCEINLINE const Vec3D & Matrix4::GetWorldOrigin() const {
	return mRows[3].ToVec3();
}

FORCEINLINE void Matrix4::SetScale( const Vec3D& scaleFactor )
{
	mRows[0][0] = scaleFactor.x;
	mRows[1][1] = scaleFactor.y;
	mRows[2][2] = scaleFactor.z;
}

FORCEINLINE void Matrix4::SetDiagonal( const Vec3D& v ) {
	mRows[0][0] = v.x;
	mRows[1][1] = v.y;
	mRows[2][2] = v.z;
}

FORCEINLINE Matrix4 Matrix4::CreateScale( FLOAT uniformScaleFactor ) {
	return Matrix4( 
		uniformScaleFactor,	0.0f,				0.0f,				0.0f,
		0.0f,				uniformScaleFactor,	0.0f,				0.0f,
		0.0f,				0.0f,				uniformScaleFactor,	0.0f,
		0.0f,				0.0f,				0.0f,				1.0f );
}

FORCEINLINE Matrix4 Matrix4::CreateScale( const Vec3D& scaleFactor ) {
	return Matrix4( 
		scaleFactor.x,		0.0f,				0.0f,				0.0f,
		0.0f,				scaleFactor.y,		0.0f,				0.0f,
		0.0f,				0.0f,				scaleFactor.z,		0.0f,
		0.0f,				0.0f,				0.0f,				1.0f );
}

FORCEINLINE Matrix4 Matrix4::CreateTranslation( const Vec3D& translation ) {
	return Matrix4( 
		1.0f,				0.0f,				0.0f,				0.0f,
		0.0f,				1.0f,				0.0f,				0.0f,
		0.0f,				0.0f,				1.0f,				0.0f,
		translation.x,		translation.y,		translation.z,		1.0f );
}

//
//	Matrix4::BuildLookAtLH - Creates a left-handed view transformation matrix.
//
//	NOTE: watch out for singularities if look vector (i.e.(target - eyePos)) is close to up vector!
//
FORCEINLINE void Matrix4::BuildLookAtLH(
	const Vec3D& eyePos,
	const Vec3D& targetPoint,	// the camera look-at target
	const Vec3D& upVector )
{
	Vec3D 	zaxis( targetPoint - eyePos );
	zaxis.NormalizeFast();

	Vec3D 	xaxis( upVector.Cross( zaxis ) );
	xaxis.NormalizeFast();

	Vec3D 	yaxis( zaxis.Cross( xaxis ) );

	mRows[0].Set(	xaxis.x,           		yaxis.x,           		zaxis.x,				0.0f );
	mRows[1].Set(	xaxis.y,           		yaxis.y,           		zaxis.y,				0.0f );
	mRows[2].Set(	xaxis.z,           		yaxis.z,           		zaxis.z,				0.0f );
	mRows[3].Set(	-xaxis.Dot( eyePos ),  -yaxis.Dot( eyePos ),	-zaxis.Dot( eyePos ),	1.0f );
}

//
//	Matrix4::BuildLookAtLH - Creates a left-handed view transformation matrix.
//
//	NOTE: axes should form an orthonormal basis!
//
FORCEINLINE void Matrix4::BuildLookAtLH(
   const Vec3D& origin,
   const Vec3D& xAxis, const Vec3D& yAxis, const Vec3D& zAxis )
{
	mRows[0].Set(	xAxis.x,           		yAxis.x,           		zAxis.x,          	0.0f	);
	mRows[1].Set(	xAxis.y,           		yAxis.y,           		zAxis.y,          	0.0f	);
	mRows[2].Set(	xAxis.z,           		yAxis.z,           		zAxis.z,          	0.0f	);
	mRows[3].Set(	-xAxis.Dot( origin ),  -yAxis.Dot( origin ),  -zAxis.Dot( origin ),	1.0f	);
}

//
//	Matrix4::BuildViewLH - Creates a left-handed view transformation matrix.
//	NOTE: watch out for singularities if look vector is close to up vector! 
//
FORCEINLINE void Matrix4::BuildViewLH(
	const Vec3D& eyePos,
	const Vec3D& lookDir,	// the normalized camera look direction
	const Vec3D& upVector )
{
	Assert( lookDir.IsNormalized() );

	Vec3D 	zaxis( lookDir );

	Vec3D 	xaxis( upVector.Cross( zaxis ) );
	xaxis.NormalizeFast();

	Vec3D 	yaxis( zaxis.Cross( xaxis ) );

	mRows[0].Set(	xaxis.x,           		yaxis.x,           		zaxis.x,          	0.0f );
	mRows[1].Set(	xaxis.y,           		yaxis.y,           		zaxis.y,          	0.0f );
	mRows[2].Set(	xaxis.z,           		yaxis.z,           		zaxis.z,          	0.0f );
	mRows[3].Set(	-xaxis.Dot( eyePos ),  -yaxis.Dot( eyePos ),  -zaxis.Dot( eyePos ),	1.0f );
}

FORCEINLINE void Matrix4::BuildInverseWorldMatrix()
{
	this->BuildLookAtLH(
		mRows[3].ToVec3(),
		mRows[0].ToVec3(),
		mRows[1].ToVec3(),
		mRows[2].ToVec3()
	);
}

//
//	Matrix4::BuildPerspectiveLH - makes a left-handed perspective projection matrix.
//
// FOVy - (Full) Vertical field of view ( in the y direction ), in radians (usually 45,60,90).
// Aspect - Aspect ratio ( screen width divided by screen height ).
//
FORCEINLINE void Matrix4::BuildPerspectiveLH( FLOAT FOVy, FLOAT Aspect, FLOAT NearZ, FLOAT FarZ )
{
	const FLOAT yScale = 1.0f / Math::Tan( 0.5f * FOVy );
	const FLOAT xScale = yScale / Aspect;
	const FLOAT F = FarZ/(FarZ - NearZ);

	mRows[0].Set( xScale,		0.0f,		0.0f,			0.0f );
	mRows[1].Set( 0.0f,			yScale,		0.0f,			0.0f );
	mRows[2].Set( 0.0f,			0.0f,       F,				1.0f );
	mRows[3].Set( 0.0f,			0.0f,       -NearZ * F,		0.0f );
}

//
//	Matrix4::BuildOrthoLH - makes a left-handed orthographic projection matrix.
//
FORCEINLINE void Matrix4::BuildOrthoLH( FLOAT Width, FLOAT Height, FLOAT NearZ, FLOAT FarZ )
{
	mRows[0].Set( 2.0f/Width,  0.0f,		0.0f,					0.0f );
	mRows[1].Set( 0.0f,          2.0f/Height,	0.0f,					0.0f );
	mRows[2].Set( 0.0f,        0.0f,        1.0f/(FarZ - NearZ),	0.0f );
	mRows[3].Set( 0.0f,        0.0f,        NearZ/(NearZ - FarZ),	1.0f );
}

FORCEINLINE void Matrix4::BuildOrthoOffCenterLH(
	FLOAT l,	// Minimum x-value of view volume.
	FLOAT r,	// Maximum x-value of view volume.
	FLOAT b,	// Minimum y-value of view volume.
	FLOAT t,	// Maximum y-value of view volume.
	FLOAT zn,	// Minimum z-value of view volume.
	FLOAT zf	// Maximum z-value of view volume.
	)
{
	this->Set(
		2.0f/(r-l),   0.0f,         0.0f,          0.0f,
		0.0f,         2.0f/(t-b),   0.0f,          0.0f,
		0.0f,         0.0f,         1.0f/(zf-zn),  0.0f,
		(l+r)/(l-r),  (t+b)/(b-t),  zn/(zn-zf),    1.0f
	);
}

FORCEINLINE void Matrix4::BuildViewportTransform(
	const FLOAT TopLeftX, const FLOAT TopLeftY,
	const FLOAT Width, const FLOAT Height,
	const FLOAT MinDepth, const FLOAT MaxDepth
	)
{
	const FLOAT halfWidth = Width * 0.5f;
	const FLOAT halfHeight = Height * 0.5f;

#if 1
#define M	(*this)
	M[0][0] = halfWidth;			M[0][1] = 0.0f;						M[0][2] = 0.0f;					M[0][3] = 0.0f;
	M[1][0] = 0.0f;					M[1][1] = -halfHeight;				M[1][2] = 0.0f;					M[1][3] = 0.0f;
	M[2][0] = 0.0f;					M[2][1] = 0.0f;						M[2][2] = MaxDepth - MinDepth;	M[2][3] = 0.0f;
	M[3][0] = TopLeftX + halfWidth;	M[3][1] = TopLeftY + halfHeight;	M[3][2] = MinDepth;				M[3][3] = 1.0f;
#undef M
#else
	return XMMatrixSet(
		halfWidth,				0.0f,						0.0f,					0.0f,
		0.0f,					-halfHeight,				0.0f,					0.0f,
		0.0f,					0.0f,						MaxDepth - MinDepth,	0.0f,
		TopLeftX + halfWidth,	TopLeftY + halfHeight,		MinDepth,				1.0f
	);
#endif
}

//
//	Matrix4::TransformVector
//
FORCEINLINE Vec3D Matrix4::TransformVector( const Vec3D& point ) const
{
	const FLOAT * __restrict M = (const FLOAT*) mRows;

	return Vec3D(
		point.x * M[0] + point.y * M[4] + point.z * M[8] + M[12],
		point.x * M[1] + point.y * M[5] + point.z * M[9] + M[13],
		point.x * M[2] + point.y * M[6] + point.z * M[10] + M[14]
	);
}

//
//	Matrix4::InverseTransformVector
//
FORCEINLINE Vec3D Matrix4::InverseTransformVector( const Vec3D& point ) const
{
#define M	(*this)

	const Vec3D	temp(
		point.x - M[3][0],
		point.y - M[3][1],
		point.z - M[3][2]
	);

	return Vec3D(
		temp.x * M[0][0] + temp.y * M[0][1] + temp.z * M[0][2],
		temp.x * M[1][0] + temp.y * M[1][1] + temp.z * M[1][2],
		temp.x * M[2][0] + temp.y * M[2][1] + temp.z * M[2][2]
	);
#undef M
}

//
//	Matrix4::TransformNormal
//
FORCEINLINE Vec3D Matrix4::TransformNormal( const Vec3D& normal ) const
{
	const FLOAT * __restrict M = (const FLOAT*) mRows;

	return Vec3D(
		normal.x * M[0] + normal.y * M[4] + normal.z * M[8],
		normal.x * M[1] + normal.y * M[5] + normal.z * M[9],
		normal.x * M[2] + normal.y * M[6] + normal.z * M[10]
	);
}

//
//	Matrix4::InverseTransformNormal
//
FORCEINLINE Vec3D Matrix4::InverseTransformNormal( const Vec3D& normal ) const
{
#define M	(*this)
	return Vec3D(
		normal.x * M[0][0] + normal.y * M[0][1] + normal.z * M[0][2],
		normal.x * M[1][0] + normal.y * M[1][1] + normal.z * M[1][2],
		normal.x * M[2][0] + normal.y * M[2][1] + normal.z * M[2][2]
	);
#undef M
}

//
//	Matrix4::GetTransformedPlane
//
FORCEINLINE Vec4D Matrix4::GetTransformedPlane( const Vec4D& plane ) const
{
#define M	(*this)
	return Vec4D(
		plane.x * M[0][0] + plane.y * M[1][0] + plane.z * M[2][0] + plane.w * M[3][0],
		plane.x * M[0][1] + plane.y * M[1][1] + plane.z * M[2][1] + plane.w * M[3][1],
		plane.x * M[0][2] + plane.y * M[1][2] + plane.z * M[2][2] + plane.w * M[3][2],
		plane.x * M[0][3] + plane.y * M[1][3] + plane.z * M[2][3] + plane.w * M[3][3]
	);
#undef M
}

FORCEINLINE Vec3D Matrix4::HomogeneousMultiply( const Vec3D& v, FLOAT w ) const
{
    const Vec4D r = (*this) * Vec4D( v, w );
	return r.ToVec3() * ( 1.0f / r.w );
}

//
// Multiplies two column-major matrices 'm1' and 'm2' and returns result in 'm'.
// NOTE: pointers should point at different memory locations!
//
inline
void mul_mat4_mat4( const FLOAT* RESTRICT_PTR(m1), const FLOAT* RESTRICT_PTR(m2), FLOAT * RESTRICT_PTR(m) )
{
	//AssertPtr(m1);	AssertPtr(m2);	AssertPtr(m);

	m[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	m[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	m[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	m[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	m[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	m[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	m[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	m[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	m[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	m[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	m[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	m[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	m[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	m[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	m[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	m[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
}

FORCEINLINE Matrix4	MatrixFromAxes( const Vec3D& x, const Vec3D& y, const Vec3D& z )
{
	return Matrix4(
		Vec4D( x, 0.0f ),
		Vec4D( y, 0.0f ),
		Vec4D( z, 0.0f ),
		Vec4D( 0.0f, 0.0f, 0.0f, 1.0f )
	);
}

mxNAMESPACE_END

#endif /* !__MATH_MATRIX_4x4_H__ */

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
