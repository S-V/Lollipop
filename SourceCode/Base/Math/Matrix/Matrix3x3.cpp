/*
=============================================================================
	File:	Matrix3x3.cpp
	Desc:	Column-major float3x3 matrix.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/Math/Math.h>

mxNAMESPACE_BEGIN

//===============================================================
//
//	Matrix3
//
//===============================================================

const Matrix3 Matrix3::mat3_zero( 
	Vec3D( 0.0f, 0.0f, 0.0f ),
	Vec3D( 0.0f, 0.0f, 0.0f ),
	Vec3D( 0.0f, 0.0f, 0.0f ) );

const Matrix3 Matrix3::mat3_identity(
	Vec3D( 1.0f, 0.0f, 0.0f ),
	Vec3D( 0.0f, 1.0f, 0.0f ),
	Vec3D( 0.0f, 0.0f, 1.0f ) );

/*
============
Matrix3::ToAngles
============
*/
Angles Matrix3::ToAngles( void ) const {
	Angles	angles;
	DOUBLE		theta;
	DOUBLE		cp;
	FLOAT		sp;

	sp = mColumns[ 0 ][ 2 ];

	// cap off our sin value so that we don't get any NANs
	if ( sp > 1.0f ) {
		sp = 1.0f;
	} else if ( sp < -1.0f ) {
		sp = -1.0f;
	}

	theta = -asin( sp );
	cp = cos( theta );

	if ( cp > 8192.0f * MX_FLT_EPSILON ) {
		angles.pitch	= RAD2DEG( theta );
		angles.yaw		= RAD2DEG( mxATan2( mColumns[ 0 ][ 1 ], mColumns[ 0 ][ 0 ] ) );
		angles.roll		= RAD2DEG( mxATan2( mColumns[ 1 ][ 2 ], mColumns[ 2 ][ 2 ] ) );
	} else {
		angles.pitch	= RAD2DEG( theta );
		angles.yaw		= RAD2DEG( -mxATan2( mColumns[ 1 ][ 0 ], mColumns[ 1 ][ 1 ] ) );
		angles.roll		= 0;
	}
	return angles;
}

/*
============
Matrix3::ToQuat
============
*/
Quat Matrix3::ToQuat( void ) const {
	Quat		q;
	FLOAT		trace;
	FLOAT		s;
	FLOAT		t;
	INT     	i;
	INT			j;
	INT			k;

	static INT 	next[ 3 ] = { 1, 2, 0 };

	trace = mColumns[ 0 ][ 0 ] + mColumns[ 1 ][ 1 ] + mColumns[ 2 ][ 2 ];

	if ( trace > 0.0f )
	{
		t = trace + 1.0f;
		s = mxInvSqrt( t ) * 0.5f;

		q[3] = s * t;
		q[0] = ( mColumns[ 2 ][ 1 ] - mColumns[ 1 ][ 2 ] ) * s;
		q[1] = ( mColumns[ 0 ][ 2 ] - mColumns[ 2 ][ 0 ] ) * s;
		q[2] = ( mColumns[ 1 ][ 0 ] - mColumns[ 0 ][ 1 ] ) * s;
	}
	else
	{
		i = 0;
		if ( mColumns[ 1 ][ 1 ] > mColumns[ 0 ][ 0 ] ) {
			i = 1;
		}
		if ( mColumns[ 2 ][ 2 ] > mColumns[ i ][ i ] ) {
			i = 2;
		}
		j = next[ i ];
		k = next[ j ];

		t = ( mColumns[ i ][ i ] - ( mColumns[ j ][ j ] + mColumns[ k ][ k ] ) ) + 1.0f;
		s = mxInvSqrt( t ) * 0.5f;

		q[i] = s * t;
		q[3] = ( mColumns[ k ][ j ] - mColumns[ j ][ k ] ) * s;
		q[j] = ( mColumns[ j ][ i ] + mColumns[ i ][ j ] ) * s;
		q[k] = ( mColumns[ k ][ i ] + mColumns[ i ][ k ] ) * s;
	}
	return q;
}

/*
============
Matrix3::ToCQuat
============
*/
CQuat Matrix3::ToCQuat( void ) const {
	Quat q = ToQuat();
	if ( q.w < 0.0f ) {
		return CQuat( -q.x, -q.y, -q.z );
	}
	return CQuat( q.x, q.y, q.z );
}

/*
============
Matrix3::ToRotation
============
*/
Rotation Matrix3::ToRotation( void ) const {
	Rotation	r;
	FLOAT		trace;
	FLOAT		s;
	FLOAT		t;
	INT     	i;
	INT			j;
	INT			k;
	local_ const INT 	next[ 3 ] = { 1, 2, 0 };

	trace = mColumns[ 0 ][ 0 ] + mColumns[ 1 ][ 1 ] + mColumns[ 2 ][ 2 ];
	if ( trace > 0.0f ) {

		t = trace + 1.0f;
		s = mxInvSqrt( t ) * 0.5f;
    
		r.angle = s * t;
		r.vec[0] = ( mColumns[ 2 ][ 1 ] - mColumns[ 1 ][ 2 ] ) * s;
		r.vec[1] = ( mColumns[ 0 ][ 2 ] - mColumns[ 2 ][ 0 ] ) * s;
		r.vec[2] = ( mColumns[ 1 ][ 0 ] - mColumns[ 0 ][ 1 ] ) * s;

	} else {

		i = 0;
		if ( mColumns[ 1 ][ 1 ] > mColumns[ 0 ][ 0 ] ) {
			i = 1;
		}
		if ( mColumns[ 2 ][ 2 ] > mColumns[ i ][ i ] ) {
			i = 2;
		}
		j = next[ i ];  
		k = next[ j ];
    
		t = ( mColumns[ i ][ i ] - ( mColumns[ j ][ j ] + mColumns[ k ][ k ] ) ) + 1.0f;
		s = mxInvSqrt( t ) * 0.5f;
    
		r.vec[i]	= s * t;
		r.angle		= ( mColumns[ k ][ j ] - mColumns[ j ][ k ] ) * s;
		r.vec[j]	= ( mColumns[ j ][ i ] + mColumns[ i ][ j ] ) * s;
		r.vec[k]	= ( mColumns[ k ][ i ] + mColumns[ i ][ k ] ) * s;
	}
	r.angle = Math::ACos( r.angle );
	if ( mxFabs( r.angle ) < 1e-10f ) {
		r.vec.Set( 0.0f, 0.0f, 1.0f );
		r.angle = 0.0f;
	} else {
		//vec *= (1.0f / sin( angle ));
		r.vec.Normalize();
		r.vec.FixDegenerateNormal();
		r.angle *= 2.0f * MX_RAD2DEG;
	}

	r.origin.SetZero();
	r.axis = *this;
	r.axisValid = true;
	return r;
}

/*
=================
Matrix3::ToAngularVelocity
=================
*/
Vec3D Matrix3::ToAngularVelocity( void ) const {
	Rotation rotation = ToRotation();
	return rotation.GetVec() * DEG2RAD( rotation.GetAngle() );
}

/*
============
Matrix3::Determinant
============
*/
FLOAT Matrix3::Determinant( void ) const {

	FLOAT det2_12_01 = mColumns[1][0] * mColumns[2][1] - mColumns[1][1] * mColumns[2][0];
	FLOAT det2_12_02 = mColumns[1][0] * mColumns[2][2] - mColumns[1][2] * mColumns[2][0];
	FLOAT det2_12_12 = mColumns[1][1] * mColumns[2][2] - mColumns[1][2] * mColumns[2][1];

	return mColumns[0][0] * det2_12_12 - mColumns[0][1] * det2_12_02 + mColumns[0][2] * det2_12_01;
}

/*
============
Matrix3::InverseSelf
============
*/
bool Matrix3::InverseSelf( void ) {
	// 18+3+9 = 30 multiplications
	//			 1 division
	Matrix3 inverse;
	DOUBLE det, invDet;

	inverse[0][0] = mColumns[1][1] * mColumns[2][2] - mColumns[1][2] * mColumns[2][1];
	inverse[1][0] = mColumns[1][2] * mColumns[2][0] - mColumns[1][0] * mColumns[2][2];
	inverse[2][0] = mColumns[1][0] * mColumns[2][1] - mColumns[1][1] * mColumns[2][0];

	det = mColumns[0][0] * inverse[0][0] + mColumns[0][1] * inverse[1][0] + mColumns[0][2] * inverse[2][0];

	if ( mxFabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	inverse[0][1] = mColumns[0][2] * mColumns[2][1] - mColumns[0][1] * mColumns[2][2];
	inverse[0][2] = mColumns[0][1] * mColumns[1][2] - mColumns[0][2] * mColumns[1][1];
	inverse[1][1] = mColumns[0][0] * mColumns[2][2] - mColumns[0][2] * mColumns[2][0];
	inverse[1][2] = mColumns[0][2] * mColumns[1][0] - mColumns[0][0] * mColumns[1][2];
	inverse[2][1] = mColumns[0][1] * mColumns[2][0] - mColumns[0][0] * mColumns[2][1];
	inverse[2][2] = mColumns[0][0] * mColumns[1][1] - mColumns[0][1] * mColumns[1][0];

	mColumns[0][0] = inverse[0][0] * invDet;
	mColumns[0][1] = inverse[0][1] * invDet;
	mColumns[0][2] = inverse[0][2] * invDet;

	mColumns[1][0] = inverse[1][0] * invDet;
	mColumns[1][1] = inverse[1][1] * invDet;
	mColumns[1][2] = inverse[1][2] * invDet;

	mColumns[2][0] = inverse[2][0] * invDet;
	mColumns[2][1] = inverse[2][1] * invDet;
	mColumns[2][2] = inverse[2][2] * invDet;

	return true;
}

/*
============
Matrix3::InverseFastSelf
============
*/
bool Matrix3::InverseFastSelf( void ) {
#if 1
	// 18+3+9 = 30 multiplications
	//			 1 division
	Matrix3 inverse;
	DOUBLE det, invDet;

	inverse[0][0] = mColumns[1][1] * mColumns[2][2] - mColumns[1][2] * mColumns[2][1];
	inverse[1][0] = mColumns[1][2] * mColumns[2][0] - mColumns[1][0] * mColumns[2][2];
	inverse[2][0] = mColumns[1][0] * mColumns[2][1] - mColumns[1][1] * mColumns[2][0];

	det = mColumns[0][0] * inverse[0][0] + mColumns[0][1] * inverse[1][0] + mColumns[0][2] * inverse[2][0];

	if ( mxFabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	inverse[0][1] = mColumns[0][2] * mColumns[2][1] - mColumns[0][1] * mColumns[2][2];
	inverse[0][2] = mColumns[0][1] * mColumns[1][2] - mColumns[0][2] * mColumns[1][1];
	inverse[1][1] = mColumns[0][0] * mColumns[2][2] - mColumns[0][2] * mColumns[2][0];
	inverse[1][2] = mColumns[0][2] * mColumns[1][0] - mColumns[0][0] * mColumns[1][2];
	inverse[2][1] = mColumns[0][1] * mColumns[2][0] - mColumns[0][0] * mColumns[2][1];
	inverse[2][2] = mColumns[0][0] * mColumns[1][1] - mColumns[0][1] * mColumns[1][0];

	mColumns[0][0] = inverse[0][0] * invDet;
	mColumns[0][1] = inverse[0][1] * invDet;
	mColumns[0][2] = inverse[0][2] * invDet;

	mColumns[1][0] = inverse[1][0] * invDet;
	mColumns[1][1] = inverse[1][1] * invDet;
	mColumns[1][2] = inverse[1][2] * invDet;

	mColumns[2][0] = inverse[2][0] * invDet;
	mColumns[2][1] = inverse[2][1] * invDet;
	mColumns[2][2] = inverse[2][2] * invDet;

	return true;
#elif 0
	// 3*10 = 30 multiplications
	//		   3 divisions
	FLOAT *mColumns = reinterpret_cast<FLOAT *>(this);
	FLOAT s;
	DOUBLE d, di;

	di = mColumns[0];
	s = di;
	mColumns[0] = d = 1.0f / di;
	mColumns[1] *= d;
	mColumns[2] *= d;
	d = -d;
	mColumns[3] *= d;
	mColumns[6] *= d;
	d = mColumns[3] * di;
	mColumns[4] += mColumns[1] * d;
	mColumns[5] += mColumns[2] * d;
	d = mColumns[6] * di;
	mColumns[7] += mColumns[1] * d;
	mColumns[8] += mColumns[2] * d;
	di = mColumns[4];
	s *= di;
	mColumns[4] = d = 1.0f / di;
	mColumns[3] *= d;
	mColumns[5] *= d;
	d = -d;
	mColumns[1] *= d;
	mColumns[7] *= d;
	d = mColumns[1] * di;
	mColumns[0] += mColumns[3] * d;
	mColumns[2] += mColumns[5] * d;
	d = mColumns[7] * di;
	mColumns[6] += mColumns[3] * d;
	mColumns[8] += mColumns[5] * d;
	di = mColumns[8];
	s *= di;
	mColumns[8] = d = 1.0f / di;
	mColumns[6] *= d;
	mColumns[7] *= d;
	d = -d;
	mColumns[2] *= d;
	mColumns[5] *= d;
	d = mColumns[2] * di;
	mColumns[0] += mColumns[6] * d;
	mColumns[1] += mColumns[7] * d;
	d = mColumns[5] * di;
	mColumns[3] += mColumns[6] * d;
	mColumns[4] += mColumns[7] * d;

	return ( s != 0.0f && !FLOAT_IS_NAN( s ) );
#else
	//	4*2+4*4 = 24 multiplications
	//		2*1 =  2 divisions
	Matrix2 r0;
	FLOAT r1[2], r2[2], r3;
	FLOAT det, invDet;
	FLOAT *mColumns = reinterpret_cast<FLOAT *>(this);

	// r0 = m0.Inverse();	// 2x2
	det = mColumns[0*3+0] * mColumns[1*3+1] - mColumns[0*3+1] * mColumns[1*3+0];

	if ( mxFabs( det ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	invDet = 1.0f / det;

	r0[0][0] =   mColumns[1*3+1] * invDet;
	r0[0][1] = - mColumns[0*3+1] * invDet;
	r0[1][0] = - mColumns[1*3+0] * invDet;
	r0[1][1] =   mColumns[0*3+0] * invDet;

	// r1 = r0 * m1;		// 2x1 = 2x2 * 2x1
	r1[0] = r0[0][0] * mColumns[0*3+2] + r0[0][1] * mColumns[1*3+2];
	r1[1] = r0[1][0] * mColumns[0*3+2] + r0[1][1] * mColumns[1*3+2];

	// r2 = m2 * r1;		// 1x1 = 1x2 * 2x1
	r2[0] = mColumns[2*3+0] * r1[0] + mColumns[2*3+1] * r1[1];

	// r3 = r2 - m3;		// 1x1 = 1x1 - 1x1
	r3 = r2[0] - mColumns[2*3+2];

	// r3.InverseSelf();
	if ( mxFabs( r3 ) < MATRIX_INVERSE_EPSILON ) {
		return false;
	}

	r3 = 1.0f / r3;

	// r2 = m2 * r0;		// 1x2 = 1x2 * 2x2
	r2[0] = mColumns[2*3+0] * r0[0][0] + mColumns[2*3+1] * r0[1][0];
	r2[1] = mColumns[2*3+0] * r0[0][1] + mColumns[2*3+1] * r0[1][1];

	// m2 = r3 * r2;		// 1x2 = 1x1 * 1x2
	mColumns[2*3+0] = r3 * r2[0];
	mColumns[2*3+1] = r3 * r2[1];

	// m0 = r0 - r1 * m2;	// 2x2 - 2x1 * 1x2
	mColumns[0*3+0] = r0[0][0] - r1[0] * mColumns[2*3+0];
	mColumns[0*3+1] = r0[0][1] - r1[0] * mColumns[2*3+1];
	mColumns[1*3+0] = r0[1][0] - r1[1] * mColumns[2*3+0];
	mColumns[1*3+1] = r0[1][1] - r1[1] * mColumns[2*3+1];

	// m1 = r1 * r3;		// 2x1 = 2x1 * 1x1
	mColumns[0*3+2] = r1[0] * r3;
	mColumns[1*3+2] = r1[1] * r3;

	// m3 = -r3;
	mColumns[2*3+2] = -r3;

	return true;
#endif
}

/*
============
Matrix3::InertiaTranslate
============
*/
Matrix3 Matrix3::InertiaTranslate( const FLOAT mass, const Vec3D &centerOfMass, const Vec3D &translation ) const {
	Matrix3 m;
	Vec3D newCenter;

	newCenter = centerOfMass + translation;

	m[0][0] = mass * ( ( centerOfMass[1] * centerOfMass[1] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[1] * newCenter[1] + newCenter[2] * newCenter[2] ) );
	m[1][1] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[0] * newCenter[0] + newCenter[2] * newCenter[2] ) );
	m[2][2] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[1] * centerOfMass[1] )
				- ( newCenter[0] * newCenter[0] + newCenter[1] * newCenter[1] ) );

	m[0][1] = m[1][0] = mass * ( newCenter[0] * newCenter[1] - centerOfMass[0] * centerOfMass[1] );
	m[1][2] = m[2][1] = mass * ( newCenter[1] * newCenter[2] - centerOfMass[1] * centerOfMass[2] );
	m[0][2] = m[2][0] = mass * ( newCenter[0] * newCenter[2] - centerOfMass[0] * centerOfMass[2] );

	return (*this) + m;
}

/*
============
Matrix3::InertiaTranslateSelf
============
*/
Matrix3 &Matrix3::InertiaTranslateSelf( const FLOAT mass, const Vec3D &centerOfMass, const Vec3D &translation ) {
	Matrix3 m;
	Vec3D newCenter;

	newCenter = centerOfMass + translation;

	m[0][0] = mass * ( ( centerOfMass[1] * centerOfMass[1] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[1] * newCenter[1] + newCenter[2] * newCenter[2] ) );
	m[1][1] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[2] * centerOfMass[2] )
				- ( newCenter[0] * newCenter[0] + newCenter[2] * newCenter[2] ) );
	m[2][2] = mass * ( ( centerOfMass[0] * centerOfMass[0] + centerOfMass[1] * centerOfMass[1] )
				- ( newCenter[0] * newCenter[0] + newCenter[1] * newCenter[1] ) );

	m[0][1] = m[1][0] = mass * ( newCenter[0] * newCenter[1] - centerOfMass[0] * centerOfMass[1] );
	m[1][2] = m[2][1] = mass * ( newCenter[1] * newCenter[2] - centerOfMass[1] * centerOfMass[2] );
	m[0][2] = m[2][0] = mass * ( newCenter[0] * newCenter[2] - centerOfMass[0] * centerOfMass[2] );

	(*this) += m;

	return (*this);
}

/*
============
Matrix3::InertiaRotate
============
*/
Matrix3 Matrix3::InertiaRotate( const Matrix3 &rotation ) const {
	// NOTE: the rotation matrix is stored column-major
	return rotation.Transpose() * (*this) * rotation;
}

/*
============
Matrix3::InertiaRotateSelf
============
*/
Matrix3 &Matrix3::InertiaRotateSelf( const Matrix3 &rotation ) {
	// NOTE: the rotation matrix is stored column-major
	*this = rotation.Transpose() * (*this) * rotation;
	return *this;
}

/*
=============
Matrix3::ToChars
=============
*/
const char *Matrix3::ToChars( INT precision ) const {
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

//
//	Matrix3::SetRotationX
//
void Matrix3::SetRotationX( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxSinCos( angle, fSin, fCos );

	mColumns[0][0] = 1.0f;		mColumns[1][0] = 0.0f;		mColumns[2][0] = 0.0f;
	mColumns[0][1] = 0.0f;		mColumns[1][1] = fCos;		mColumns[2][1] = -fSin;
	mColumns[0][2] = 0.0f;		mColumns[1][2] = fSin;		mColumns[2][2] = fCos;
}

//
//	Matrix3::SetRotationY
//
void Matrix3::SetRotationY( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxSinCos( angle, fSin, fCos );

	mColumns[0][0] = fCos;		mColumns[1][0] = 0.0f;		mColumns[2][0] = fSin;
	mColumns[0][1] = 0.0f;		mColumns[1][1] = 1.0f;		mColumns[2][1] = 0.0f;
	mColumns[0][2] = -fSin;		mColumns[1][2] = 0.0f;		mColumns[2][2] = fCos;
}

//
//	Matrix3::SetRotationZ
//
void Matrix3::SetRotationZ( FLOAT angle )
{
	FLOAT fSin, fCos;
	mxSinCos( angle, fSin, fCos );

	mColumns[0][0] = fCos;		mColumns[1][0] = -fSin;		mColumns[2][0] = 0.0f;
	mColumns[0][1] = fSin;		mColumns[1][1] = fCos;		mColumns[2][1] = 0.0f;
	mColumns[0][2] = 0.0f;		mColumns[1][2] = 0.0f;		mColumns[2][2] = 1.0f;
}

//
//	Matrix3::SetRotationAxis
//
void Matrix3::SetRotationAxis( FLOAT angle, const Vec3D& axis )
{
	Assert( axis.IsNormalized() );

	FLOAT s, c;
	mxSinCos( angle, s, c );

	const FLOAT xy = axis.x * axis.y;
	const FLOAT yz = axis.y * axis.z;
	const FLOAT zx = axis.z * axis.x;
	const FLOAT xs = axis.x * s;
	const FLOAT ys = axis.y * s;
	const FLOAT zs = axis.z * s;
	const FLOAT oneMinusC = 1.0f - c;

	mColumns[0].Set(
		oneMinusC * axis.x * axis.x + c,
		oneMinusC * xy + zs,
		oneMinusC * zx - ys
	);
	mColumns[1].Set(
		oneMinusC * xy - zs,
		oneMinusC * axis.y * axis.y + c,
		oneMinusC * yz + xs
	);
	mColumns[2].Set(
		oneMinusC * zx + ys,
		oneMinusC * yz - xs,
		oneMinusC * axis.z * axis.z + c
	);
}

/*
=============
Matrix3::CreateRotation
=============
*/

Matrix3	Matrix3::CreateRotation( const Vec3D& vFrom, const Vec3D& vTo )
{
	FLOAT mat[3][3];
	FromToRotation( vFrom.ToFloatPtr(), vTo.ToFloatPtr(), mat );
	return Matrix3( mat );
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
