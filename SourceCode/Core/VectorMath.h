/*
=============================================================================
	File:	VectorMath.h
	Desc:	
=============================================================================
*/

#ifndef __MX_CORE_VECTOR_MATH_H__
#define __MX_CORE_VECTOR_MATH_H__

#include <Base/Math/Math.h>
#include <Base/Util/Color.h>

//---------------------------------------------------------------
//		Math libraries.
//---------------------------------------------------------------

#define MX_COMPILE_WITH_XNA_MATH		1
#define MX_COMPILE_WITH_XNA_COLLISION	1


#if !MX_USE_SSE
	#if MX_COMPILE_WITH_XNA_MATH
		#define _XM_NO_INTRINSICS_
		#define XM_NO_MISALIGNED_VECTOR_ACCESS
	#endif
#endif

#if MX_COMPILE_WITH_XNA_MATH
	//#define XM_NO_OPERATOR_OVERLOADS
	#include <xnamath.h>
#endif


#if MX_COMPILE_WITH_XNA_COLLISION
	#include <XNACollision/xnacollision.h>

	#if MX_AUTOLINK
		#pragma comment( lib, "XNACollision.lib" )
	#endif
#endif




/*
=======================================================================
	Common graphics types.
=======================================================================
*/

// Forward declarations.



//-------------------------------------------------------------------
//	Types
//-------------------------------------------------------------------

typedef FLOAT		float1;
typedef Vec2D		float2;
typedef Vec3D		float3;
typedef XMVECTOR	float4;	// mxSimdQuad

typedef Matrix2		float2x2;
typedef Matrix3		float3x3;
typedef XMMATRIX	float4x4;



// constant arguments (passed into functions)

#if (defined(_XM_VMX128_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_))

// should be passed through registers (by value)

typedef const float2	vec2_carg;
typedef const float3	vec3_carg;
typedef const float4	vec4_carg;
typedef const float4x4	mat4_carg;

#else

// win32 VC++ does not support passing aligned objects by value,
// here is a bug-report at Microsoft saying so:
// http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=334581

typedef const float2&	vec2_carg;
typedef const float3&	vec3_carg;
typedef const float4&	vec4_carg;
typedef const float4x4&	mat4_carg;

#endif

// non-constant function arguments

typedef float2&	vec2_arg;
typedef float3&	vec3_arg;
typedef float4&	vec4_arg;
typedef float4x4&	mat4_arg;


//--------------------------------------------------------------//
//	Serializers.
//--------------------------------------------------------------//

mxDECLARE_POD_TYPE(Vec2D);
mxDECLARE_POD_TYPE(Vec3D);
mxDECLARE_POD_TYPE(Vec4D);
mxDECLARE_POD_TYPE(Matrix2);
mxDECLARE_POD_TYPE(Matrix3);
mxDECLARE_POD_TYPE(Matrix4);

mxDECLARE_POD_TYPE(float4);
mxDECLARE_POD_TYPE(float4x4);

//--------------------------------------------------------------//
//	Reflection.
//--------------------------------------------------------------//

mxDECLARE_BUILTIN_TYPE( float4,	ETypeKind::Type_SimdQuad );

mxDECLARE_BASIC_STRUCT(XMFLOAT2)
mxDECLARE_BASIC_STRUCT(XMFLOAT3)
mxDECLARE_BASIC_STRUCT(XMFLOAT4)

mxDECLARE_BASIC_STRUCT(float4x4)

//---------------------------------------------------------------

FORCEINLINE float4 vec_load0( const Vec3D& v )
{
	return XMVectorSet( v.x, v.y, v.z, 0.0f );
}
FORCEINLINE float4 vec_load1( const Vec3D& v )
{
	return XMVectorSet( v.x, v.y, v.z, 1.0f );
}

FORCEINLINE float4 vec_load1( FLOAT x, FLOAT y, FLOAT z )
{
	return XMVectorSet( x, y, z, 1.0f );
}

#if 0

FORCEINLINE float4 vec3_to_vec4( vec3_carg v, FLOAT w )
{
	return XMVectorSet(v.x,v.y,v.z,w);
}

FORCEINLINE float4 vec3_ptr_to_vec4( const FLOAT* pVec3, FLOAT w )
{
	return XMVectorSet(pVec3[0],pVec3[1],pVec3[2],w);
}

FORCEINLINE float4 float_ptr_to_vec4( const FLOAT* pVec4 )
{
	return XMVectorSet(pVec4[0],pVec4[1],pVec4[2],pVec4[3]);
}

FORCEINLINE float4 vec4_get( FLOAT x, FLOAT y, FLOAT z, FLOAT w )
{
	return XMVectorSet(x,y,z,w);
}

FORCEINLINE float3 vec3_get_zero()
{
	return float3(0.0f,0.0f,0.0f);
}

FORCEINLINE float4 vec4_get_zero()
{
	return XMVectorZero();
}



FORCEINLINE float4 vec_add( vec4_carg v1, vec4_carg v2 )
{
	return XMVectorAdd( v1, v2 );
}
FORCEINLINE float4 vec_mul( vec4_carg v1, FLOAT scale )
{
	return XMVectorScale( v1, scale );
}
FORCEINLINE float4 vec3_cross( vec4_carg v1, vec4_carg v2 )
{
	return XMVector3Cross( v1, v2 );
}

FORCEINLINE float4x4 vec_mat4_mul_mat4( mat4_carg mA, mat4_carg mB )
{
	return XMMatrixMultiply( mA, mB );
}

FORCEINLINE float4x4 vec_mat4_inverse( mat4_carg matrix, float4 *determinant ) 
{
	return XMMatrixInverse( determinant, matrix );
}
#endif


FORCEINLINE XMFLOAT3& as_float3( Vec3D& v )
{
	return reinterpret_cast< XMFLOAT3& >( v );
}
FORCEINLINE const XMFLOAT3& as_float3( const Vec3D& v )
{
	return reinterpret_cast< const XMFLOAT3& >( v );
}
FORCEINLINE Vec3D& as_vec3( vec4_arg v )
{
	return reinterpret_cast< Vec3D& >( v );
}
FORCEINLINE const Vec3D& as_vec3( vec4_carg v )
{
	return reinterpret_cast< const Vec3D& >( v );
}
//XMFLOAT3_as_Vec3D
FORCEINLINE Vec3D& as_vec3( XMFLOAT3& v )
{
	return reinterpret_cast< Vec3D& >( v );
}
FORCEINLINE const Vec3D& as_vec3( const XMFLOAT3& v )
{
	return reinterpret_cast< const Vec3D& >( v );
}


FORCEINLINE Vec4D& as_vec4( float4& vector )
{
	return reinterpret_cast< Vec4D& >( vector );
}
FORCEINLINE const Vec4D& as_vec4( const float4& vector )
{
	return reinterpret_cast< const Vec4D& >( vector );
}
FORCEINLINE XMFLOAT4& as_float4( float4& vector )
{
	return reinterpret_cast< XMFLOAT4& >( vector );
}
FORCEINLINE const XMFLOAT4& as_float4( const float4& vector )
{
	return reinterpret_cast< const XMFLOAT4& >( vector );
}


FORCEINLINE Quat& as_quat( float4& vector )
{
	return reinterpret_cast< Quat& >( vector );
}
FORCEINLINE const Quat& as_quat( const float4& vector )
{
	return reinterpret_cast< const Quat& >( vector );
}

FORCEINLINE Matrix4& as_matrix4( float4x4& matrix )
{
	return reinterpret_cast< Matrix4& >( matrix );
}
FORCEINLINE const Matrix4& as_matrix4( const float4x4& matrix )
{
	return reinterpret_cast< const Matrix4& >( matrix );
}


//NOTE: pointer to matrix should be 16-byte aligned
FORCEINLINE mat4_arg to_float4x4( Matrix4& matrix )
{
	return reinterpret_cast< mat4_arg >( matrix );
}
FORCEINLINE mat4_carg to_float4x4( const Matrix4& matrix )
{
	return reinterpret_cast< mat4_carg >( matrix );
}

FORCEINLINE FColor& float4_as_FColor( float4 & v )
{
	return *(FColor*) &v;
}


FORCEINLINE void xna_vec3_copy( XMFLOAT3 &dest, const Vec3D& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}
FORCEINLINE void xna_vec3_copy( Vec3D &dest, const XMFLOAT3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

FORCEINLINE bool MatricesAreEqual( mat4_carg mA, mat4_carg mB )
{
#if 0
	return mA.r[0] == mB.r[0]
		&& mA.r[1] == mB.r[1]
		&& mA.r[2] == mB.r[2]
		&& mA.r[3] == mB.r[3]
	;
#else
	return 0 == MemCmp( &mA, &mB, sizeof mA );
#endif
}


static inline
float4x4 WorldMatrixToViewMatrix( mat4_carg worldMatrix )
{
	const float4	upDir = worldMatrix.r[1];
	const float4	lookDir = worldMatrix.r[2];
	const float4	eyePos = worldMatrix.r[3];
	return XMMatrixLookToLH(
		eyePos,
		lookDir,
		upDir
	);
}

// calculates right and up vectors (it can be used to build a view matrix)
inline
float4x4 BuildBasisVectors( vec4_carg position, vec4_carg direction )
{
	const float4	WORLD_UP = g_XMIdentityR1;
	const float4	WORLD_RIGHT = g_XMIdentityR0;

	float4	rightDir = XMVector3Cross( WORLD_UP, direction );
	float	rightDirInvLen = XMVectorGetX( XMVector3ReciprocalLengthEst( rightDir ) );

	// prevent singularities when light direction is nearly parallel to 'up' direction
	if( rightDirInvLen >= (1.0f / 1e-3f) ) {
		rightDir = WORLD_RIGHT;
	} else {
		rightDir = XMVectorScale( rightDir, rightDirInvLen );
	}

	const float4	upDir = XMVector3Normalize( XMVector3Cross( direction, rightDir ) );

	return XMMATRIX(
		XMVectorSetW( rightDir,	0.0f ),
		XMVectorSetW( upDir,	0.0f ),
		XMVectorSetW( direction,0.0f ),
		position
	);
}

#endif // !__MX_CORE_VECTOR_MATH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
