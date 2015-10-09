#pragma once

//------------------------------------------------------------------------
//	Nasty includes
//------------------------------------------------------------------------

#include <Base/Math/Math.h>
#include <Base/Util/Color.h>
#include <Core/Resources.h>
#include <Core/VectorMath.h>


/*
=============================================================================
	Build configuration settings.
	These are compile time settings for the libraries.
=============================================================================
*/

//------------------------------------------------------------------------
//	Defines
//------------------------------------------------------------------------

// DLL dance
#define MX_GRAPHICS_API

// #define to 1 to build for platforms with support for Direct3D 11 and SM 5
#define RX_HW_D3D11		(0)

//	Renderer debugging.
#define RX_DEBUG_RENDERER	MX_DEBUG


#if RX_DEBUG_RENDERER
#define RX_DEBUG_CODE( code )	code
#else
#define RX_DEBUG_CODE( code )
#endif

//	Shader debugging & automatic input layout validation. Slows down a lot!
#define RX_DEBUG_SHADERS	(MX_DEBUG)


// Encode and obfuscate shader source code to protect intellectual property.
#define RX_ENCODE_SHADERS	(0)


// markers
#define RX_KLUDGE( message )
#define RX_OPTIMIZE( message )


//--------------------------------------------------------------------------------------
// Profiling/instrumentation support
//--------------------------------------------------------------------------------------

// performance counters, slows down a lot
#define RX_PROFILE			(MX_ENABLE_PROFILING)


// DirectX-specific
#define RX_D3D_USE_PERF_HUD		(RX_PROFILE)


#if MX_DEVELOPER
#define RX_STATS( code )	code
#else
#define RX_STATS( code )
#endif


//
//	Scoped stat counters.
//

#if RX_PROFILE
	#define RX_TIME_COUNTER( uint_num_milliseconds )		ScopedTimeCounter((uint_num_milliseconds))
	#define RX_CYCLE_COUNTER( uint_num_cpu_cycles )			ScopedCycleCounter((uint_num_cpu_cycles))
#else
	#define RX_TIME_COUNTER( uint_num_milliseconds )
	#define RX_CYCLE_COUNTER( uint_num_cpu_cycles )
#endif


//-------------------------------------------------------------------
//	Bounding volume types
//-------------------------------------------------------------------

typedef float4				rxSphere;
typedef XNA::AxisAlignedBox	rxAABB;
typedef XNA::OrientedBox	rxOOBB;

mxDECLARE_POD_TYPE(rxAABB);
mxDECLARE_BASIC_STRUCT(rxAABB);

mxDECLARE_POD_TYPE(rxOOBB);
mxDECLARE_BASIC_STRUCT(rxOOBB);


// Axis-aligned box + Sphere
//
_DECLSPEC_ALIGN_16_ struct rxBounds
{
	float4	centerAndRadius;//16 xyz - center, w - sphere radius.
	float4	boxHalfSize;	//16 xyz - distance from the center to each side.
};
mxDECLARE_POD_TYPE(rxBounds);



FORCEINLINE void rxAABB_Clear( rxAABB & o )
{
	ZERO_OUT(o);
}
FORCEINLINE void rxAABB_Infinity( rxAABB & o )
{
	ZERO_OUT(o.Center);
	o.Extents.x = MAX_WORLD_HALF_SIZE;
	o.Extents.y = MAX_WORLD_HALF_SIZE;
	o.Extents.z = MAX_WORLD_HALF_SIZE;
}
FORCEINLINE void rxAABB_From_AABB( rxAABB &dest, const AABB& src )
{
	const Vec3D halfSize = src.GetHalfSize();

	dest.Center.x = src.GetCenter().x;
	dest.Center.y = src.GetCenter().y;
	dest.Center.z = src.GetCenter().z;

	dest.Extents.x = halfSize.x;
	dest.Extents.y = halfSize.y;
	dest.Extents.z = halfSize.z;
}
FORCEINLINE void rxAABB_To_AABB( const rxAABB& src, AABB &dest )
{
	dest.mPoints[0] = as_vec3(src.Center) - as_vec3(src.Extents);
	dest.mPoints[1] = as_vec3(src.Center) + as_vec3(src.Extents);
}



typedef int ClipStatus_t;
enum {
	CS_Disjoint = 0,	// no intersection
	CS_Intersect = 1,	// intersection
	CS_FullyInside = 2,	// the tested subject is completely inside the given object
};

// NOTE: XNA frustum only works with perspective projection!
//
mxALIGN_16(struct) rxViewFrustum : public XNA::Frustum
{
	FORCEINLINE rxViewFrustum()
	{}
	FORCEINLINE void Build( const float4x4* worldMatrix, const float4x4* projectionMatrix )
	{
		XNA::ComputeFrustumFromProjection( this, projectionMatrix );
		xna_vec3_copy( (*this).Origin, as_matrix4( *worldMatrix )[3].ToVec3() );
		XMStoreFloat4( &(*this).Orientation, XMQuaternionRotationMatrix( *worldMatrix ) );
	}
	FORCEINLINE void Build( const Vec3D& origin, const Quat& orientation, const float4x4* projectionMatrix )
	{
		XNA::ComputeFrustumFromProjection( this, projectionMatrix );
		xna_vec3_copy( (*this).Origin, origin );
		MemCopy( &(*this).Orientation, &orientation, sizeof(float4) );
	}
	FORCEINLINE ClipStatus_t TestSphere( const Sphere& sphere ) const
	{
		return XNA::IntersectSphereFrustum( (const XNA::Sphere*)&sphere, this )/* != XNAClipStatus::NoIntersection*/;
	}
	FORCEINLINE ClipStatus_t TestAABB( const rxAABB& aabb ) const
	{
		return XNA::IntersectAxisAlignedBoxFrustum( &aabb, this )/* != XNAClipStatus::NoIntersection*/;
	}
	FORCEINLINE ClipStatus_t TestFrustum( const rxViewFrustum& other ) const
	{
		return XNA::IntersectFrustumFrustum( this, &other )/* != XNAClipStatus::NoIntersection*/;
	}

	FORCEINLINE const Vec3D& GetOrigin() const
	{
		return as_vec3( this->Origin );
	}
};



//
//	Computes the view frustum corners in view-space.
//	halfFoVy - half of vertical field of view, in radians
//
inline void GetViewSpaceFrustumCorners( FLOAT halfFoVy, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ, Vec3D corners[8] )
{
	FLOAT t = mxTan(halfFoVy);	// t = farY / farZ

	FLOAT farY = t * FarZ;
	FLOAT farX = farY * AspectRatio;

	FLOAT nearY = t * NearZ;
	FLOAT nearX = nearY * AspectRatio;

	// Near
	corners[0].Set(-nearX,  nearY, NearZ);	// Top Left
	corners[1].Set( nearX,  nearY, NearZ);	// Top Right
	corners[2].Set( nearX, -nearY, NearZ);	// Bottom Right
	corners[3].Set(-nearX, -nearY, NearZ);	// Bottom Left

	// Far
	corners[4].Set(-farX,  farY, FarZ);		// Top Left
	corners[5].Set( farX,  farY, FarZ);		// Top Right
	corners[6].Set( farX, -farY, FarZ);		// Bottom Right
	corners[7].Set(-farX, -farY, FarZ);		// Bottom Left
}





enum EUpperDesignLimits
{
	RX_MAX_TEXTURE_DIMENSIONS = 4096,
	RX_MAX_TEXTURE_MIP_LEVELS = 16,

	RX_MAX_RENDER_TARGET_SIZE = 8192,

	RX_MAX_VIEWPORTS = 8,	// max. allowed number of simultaneously bound viewports
	RX_MAX_RENDER_TARGETS = 4,	// max. allowed number of simultaneously bound render targets
	RX_MAX_VERTEX_STREAMS = 16,

	RX_MAX_VERTEX_SHADER_RESOURCES = 16,
	RX_MAX_GEOMETRY_SHADER_RESOURCES = 16,
	RX_MAX_PIXEL_SHADER_RESOURCES = 16,
};

mxDECLARE_POD_TYPE(Quat);
mxDECLARE_BASIC_STRUCT(Quat);

//
//	rxTransform
//
mxALIGN_16(struct) rxTransform
{
	Quat	orientation;
	Vec3D	translation;
	FLOAT	scaleFactor;	// must be positive (only uniform scaling is supported for now)

public:
	FORCEINLINE rxTransform()
	{}

	FORCEINLINE rxTransform( EInitIdentity )
	{
		this->SetIdentity();
	}

	FORCEINLINE rxTransform( const Vec3D& translation,
		const Quat& orientation = Quat::quat_identity,
		FLOAT scale = 1.0f )
	{
		this->translation = translation;
		this->scaleFactor = scale;
		this->orientation = orientation;
	}

	FORCEINLINE void SetIdentity()
	{
		this->translation = Vec3D::vec3_zero;
		this->scaleFactor = 1.0f;
		this->orientation = Quat::quat_identity;
	}

	FORCEINLINE void SetOrigin( const Vec3D& newOrigin )
	{
		this->translation = newOrigin;
	}
	FORCEINLINE void SetOrientation( const Quat& newOrientation )
	{
		this->orientation = newOrientation;
	}
	// (Non-uniform scale is not supported).
	FORCEINLINE void SetScaling( const FLOAT scaling )
	{
		Assert(scaling > 0.0f);
		this->scaleFactor = scaling;
	}

	FORCEINLINE float4 GetTranslationVec() const
	{
		const XMVECTOR vTranslation = XMVectorSet(this->translation.x,this->translation.y,this->translation.z,1.0f);
		return vTranslation;
	}
	FORCEINLINE float4 GetOrientationVec() const
	{
		//const XMVECTOR vOrientation = this->orientation;
		//return vOrientation;
		return this->orientation.quad;
	}
#if 0
	FORCEINLINE Matrix4 ToMat4() const
	{
		Matrix4	m;
		m.BuildTransform(
			this->translation,
			this->orientation,
			Vec3D(this->scaleFactor)
		);
		return m;
	}
#else
	inline float4x4 ToMat4() const
	{
		const XMVECTOR vScaling = XMVectorReplicate( scaleFactor );
		const XMVECTOR vTranslation = GetTranslationVec();
		return XMMatrixAffineTransformation( vScaling, g_XMIdentityR3, this->orientation.quad, vTranslation );
	}
#endif
	inline bool FromMat4( mat4_carg m )
	{
		XMVECTOR	vOrientation;
		XMVECTOR	vTranslation;
		XMVECTOR	vScale;
		VRET_FALSE_IF_NOT(XMMatrixDecompose( &vScale, &vOrientation, &vTranslation, m ));

		this->orientation = as_quat( vOrientation );
		this->translation = as_vec3( vTranslation );
		this->scaleFactor = XMVectorGetX( vScale );	// only uniform scaling is implemented

		return true;
	}

	FORCEINLINE bool HasScaling() const
	{
		return this->scaleFactor != 1.0f;
	}

	bool isOk() const
	{
		return 1
			&& this->scaleFactor > 0.0f
			;
	}
};

mxDECLARE_POD_TYPE(rxTransform);
mxDECLARE_BASIC_STRUCT(rxTransform);



FORCEINLINE void rxAABB_Transform( rxAABB *dst, const rxAABB& src, const rxTransform& xform )
{
	XNA::TransformAxisAlignedBox(
		dst, &src,
		xform.scaleFactor,
		xform.GetOrientationVec(),
		xform.GetTranslationVec()
	);
}




FORCEINLINE Vec4D F_ProjectPoint( const Vec3D& point, mat4_carg viewProjectionMatrix )
{
	//const float4 pointH = XMVector3TransformCoord( vec_load1(point), viewProjectionMatrix );
	Vec4D pointH( point, 1.0f );
	pointH *= as_matrix4( viewProjectionMatrix );
	FLOAT invW = mxReciprocal( pointH.w );
	return Vec4D( pointH.x*invW, pointH.y*invW, pointH.z*invW, pointH.w );
}

FORCEINLINE Vec4D F_DeprojectPoint( const Vec4D& xyzw, mat4_carg inverseViewProjectionMatrix )
{
	return as_matrix4(inverseViewProjectionMatrix) * Vec4D( xyzw.x * xyzw.w, xyzw.y * xyzw.w, xyzw.z * xyzw.w, xyzw.w );
}

// computes normalized view-space depth of the given point
//
FORCEINLINE FLOAT F_CalculateDepth( const Vec3D& point, mat4_carg viewProjectionMatrix )
{
	Vec4D pointH( point, 1.0f );
	pointH *= as_matrix4( viewProjectionMatrix );
	FLOAT invW = mxReciprocal( pointH.w );
	return pointH.z * invW;
}

// transforms from 2D viewport coords to 3D normalized device coords (clip space)
//
template< class VIEWPORT >
inline void F_ViewportToClip( const VIEWPORT& viewport, const Matrix4& projMatrix,
								const INT iPointX, const INT iPointY,
								FLOAT &OutX, FLOAT &OutY )
{
	const FLOAT twoInvW = 2.0f / (FLOAT)viewport.GetWidth();
	const FLOAT twoInvH = 2.0f / (FLOAT)viewport.GetHeight();

	OutX = ( iPointX * twoInvW - 1.0f ) / projMatrix[0][0];
	OutY = ( 1.0f - iPointY * twoInvH ) / projMatrix[1][1];
}


// Transform_Tex2Clip is used to convert texture coordinates into clip-space position.
// the texture coordinates are in the range (U[0..1],V[0..1]),
// whereas the NDC coordinates are in (X[-1..1],Y[1..-1],Z[0..1] (Y axis is flipped))
//
XMGLOBALCONST
float4x4 Transform_Tex2Clip(
	2.0f,	0.0f,	0.0f,	0.0f,
	0.0f,	-2.0f,	0.0f,	0.0f,
	0.0f,	0.0f,	1.0f,	0.0f,
	-1.0f,	1.0f,	0.0f,	1.0f
);

// to transform from clip space to texture space the following matrix can be used
// which takes 2D clipspace {-1, 1} to {0,1} and inverts the Y:
//
XMGLOBALCONST
float4x4 Transform_Clip2Tex(
	0.5f,	0.0f,	0.0f,   0.0f,
	0.0f,	-0.5,	0.0f,   0.0f,
	0.0f,	0.0f,	1.0f,   0.0f,
	0.5f,	0.5f,	0.0f,   1.0f
);

// Constructs a scale/offset/bias matrix with a custom bias,
// which transforms from [-1,1] post-projection space to [0,1] UV space
//
inline
float4x4 Matrix4x4_Build_Texture_Scale_Bias( const F4 bias = 0.0f )
{
	float4x4 textureMatrix;
	textureMatrix.r[0] = XMVectorSet( 0.5f,  0.0f,  0.0f,  0.0f );
	textureMatrix.r[1] = XMVectorSet( 0.0f, -0.5f,  0.0f,  0.0f );
	textureMatrix.r[2] = XMVectorSet( 0.0f,  0.0f,  1.0f,  0.0f );
	textureMatrix.r[3] = XMVectorSet( 0.5f,  0.5f,  -bias, 1.0f );
/*
		e.g.: scale matrix to go from post-perspective space into texture space ([0,1]^2)
		D3DXMATRIX Clip2Tex = D3DXMATRIX(
			0.5,    0,    0,   0,
			0,	   -0.5,  0,   0,
			0,     0,     1,   0,
			0.5,   0.5,   0,   1 );
*/
	return textureMatrix;
}



// for some reason, this warning is not disabled
#ifdef _MSC_VER
#pragma warning ( disable: 4200 )	// nonstandard extension used : zero-sized array in struct/union
#endif /* _MSC_VER */

// used for storing encrypted shader source code
//
struct rxStaticString
{
	const SizeT	size;
	const char data[];
};

enum EncodingType
{
	NoEncoding = 0,
	XorEncoding,
	RotEncoding,
};

struct GrEncodedString
{
	UINT	originalSize;
	UINT	encodedSize;
	BYTE *	encodedData;
};


/*
=====================================================================
	
	Vertex format.

=====================================================================
*/

//
//	EVertexElementType - is a type of a vertex element.
//
enum EVertexElementType : UINT
{
	VET_Float1,	// One-component float expanded to (float, 0, 0, 1).
	VET_Float2,	// Two-component float expanded to (float, float, 0, 1).
	VET_Float3,	// Three-component float expanded to (float, float, float, 1).
	VET_Float4,	// Four-component float expanded to (float, float, float, float).
	VET_UByte4,	// Four-component, unsigned byte.

	VET_Short2,     //> two-component signed short, expanded to (value, value, 0, 1)
	VET_Short4,     //> four-component signed short
	VET_UByte4N,    //> four-component normalized unsigned byte (value / 255.0f)
	VET_Short2N,    //> two-component normalized signed short (value / 32767.0f)
	VET_Short4N,    //> four-component normalized signed short (value / 32767.0f)

	VET_Unknown,

	VET_MAX,
};

const char* rxUtil_GetVertexElementTypeName( EVertexElementType elementType );

// Utility function for helping to calculate offsets.
SizeT rxUtil_GetVertexElementSize( EVertexElementType elementType );

// Maximum size a single vertex can have, in bytes.
enum { MAX_VERTEX_SIZE = 256 };

//
//	EVertexElementUsage - describes the meaning (semantics) of vertex components.
//
enum EVertexElementUsage : UINT
{
	VEU_Position,		// Position, 3 floats per vertex.
	VEU_Normal,			// Normal, 3 floats per vertex.

	VEU_TexCoords,		// Texture coordinates.

	VEU_Color,		// Vertex color.

	VEU_Tangent,	// X axis if normal is Z
	VEU_Binormal,	// Y axis if normal is Z

	// These are used for skeletal animation.
	VEU_BlendWeights,
	VEU_BlendIndices,

	VEU_Unknown,	// User-defined usage.

	VEU_MAX
};

const char* rxUtil_GetVertexElementSemanticStr( EVertexElementUsage semantic );
EVertexElementUsage rxUtil_GetVertexElementSemanticEnum( const char* str );

void rxUtil_GenerateVertexElementName(
	EVertexElementUsage semantic, const char* SemanticName, UINT SemanticIndex,
	StackString &OutName );

// Maximum number of components a single vertex can have.
enum { RX_MAX_VERTEX_ELEMENTS = 16 };

// Maximum number of vertex streams that can be bound simultaneously.
// don't make it too big (memory overhead)
enum { RX_MAX_INPUT_SLOTS = 4 };

//
//	rxVertexElement - defines a base type that vertices consist of.
//
struct rxVertexElement
{
	TEnum< EVertexElementType, UINT8 >	type;		// The type of this element.
	TEnum< EVertexElementUsage, UINT8 >	usage;		// The meaning of this element.
	UINT8								offset;		// The offset in the buffer that this element starts at, in bytes.
	UINT8								usageIndex;	// Index for multi-input elements like texture coordinates.
	UINT8								inputSlot;	// Input stream (vertex buffer).

public:
	inline rxVertexElement()
	{}

	inline rxVertexElement(
		EVertexElementType inType,
		EVertexElementUsage inUsage,
		UINT inOffset,
		UINT inUsageIndex = 0,
		UINT inStreamIndex = 0
	)
		: type( inType )
		, usage( inUsage )
		, offset( inOffset )
		, usageIndex( inUsageIndex )
		, inputSlot( inStreamIndex )
	{}

	// Returns the size of this element, in bytes.
	UINT GetSize() const { return rxUtil_GetVertexElementSize( this->type ); }

private:
	// this function is never called, it only contains compile-time assertions
	inline void _DebugChecks()
	{
		{CHECK_STORAGE( sizeof(type), VET_MAX );}
		{CHECK_STORAGE( sizeof(usage), VEU_MAX );}
		{CHECK_STORAGE( sizeof(offset), MAX_VERTEX_SIZE );}
		{CHECK_STORAGE( sizeof(inputSlot), RX_MAX_INPUT_SLOTS );}
	}
};

//
//	rxVertexLayout - describes vertex buffers contents.
//
class rxVertexLayout {
public:

	// Returns the number of elements in this vertex type.
	//
	virtual UINT	GetNumElements() const = 0;

	// Returns the vertex component with the given index.
	//
	virtual const rxVertexElement &	GetElement( UINT index ) const = 0;

	// Returns the size of this vertex in bytes.
	//
	virtual SizeT	GetSize() const = 0;

	// Returns true if this is a valid vertex format.
	//
	virtual bool	IsValid() const;

protected:
			rxVertexLayout() {}
	virtual	~rxVertexLayout() {}
};

//
//	rxVertexDeclaration - used for creating new types of vertices.
//
//	NOTE: to ensure maximum portability,
//	vertex elements should be added to vertex declarations
//	in the following order:
//	position, blend weights, normals, diffuse colors, specular colors
//	and texture coordinates.
//	Moreover, there must be no unused gaps in a vertex structure.
//
class rxVertexDeclaration : public rxVertexLayout {
public:

	//
	//	Override ( rxVertexLayout ) :
	//

	// Returns the number of elements in this vertex type.

	UINT	GetNumElements() const;

	// Returns the vertex component with the given index.

	const rxVertexElement &	GetElement( UINT index ) const;

	// Returns the size of this vertex in bytes.

	SizeT	GetSize() const;

	// Returns true if this is a valid vertex format.

	bool	IsValid() const;

public:
			rxVertexDeclaration();

			// Appends a new vertex element to the end.
			// Offset is calculated automatically.

	void	Add( EVertexElementUsage usage, EVertexElementType type,
				UINT usageIndex = 0, UINT inputSlot = 0 );

			// Removes all elements.
	void	Clear();

private:
	typedef TStaticList< rxVertexElement, RX_MAX_VERTEX_ELEMENTS > VertexComponents;

	VertexComponents	elements;	// list of vertex components
	SizeT				vertexSize;	// accumulated offset, in bytes
};

/*
-----------------------------------------------------------------
	rxNormal4 - compressed normal.

	Quote from 'Rubicon':
	http://www.gamedev.net/topic/564892-rebuilding-normal-from-2-components-with-cross-product/
	Trust me, 8 bits per component is plenty,
	especially if it gets your whole vertex to 32 bytes or lower.
	I've used it for years and nobody's ever even noticed
	- you get a perfect spread of lighting across a ball at any magnification you like
	- don't forget its interpolated per pixel
	- all this decompression happens at the VS level, not the PS.

	Excerpt from ShaderX:
	Normalized normals or tangents are usually stored as three floats (D3DVSDT_FLOAT3), but
	this precision is not needed. Each component has a range of –1.0 to 1.0 (due to normalization),
	ideal for quantization. We don’t need any integer components, so we can devote all bits to the
	fractional scale and a sign bit. Biasing the floats makes them fit nicely into an unsigned byte.
	For most normals, 8 bits will be enough (that’s 16.7 million values over the surface of a
	unit sphere). Using D3DVSDT_UBYTE4, we have to change the vertex shader to multiply by
	1.0/127.5 and subtract 1.0 to return to signed floats. I’ll come back to the optimizations that
	you may have noticed, but for now, I’ll accept the one cycle cost for a reduction in memory
	and bandwidth saving of one-fourth for normals and tangents.
	Quantization vertex shader example:
	; v1 = normal in range 0 to 255 (integer only)
	; c0 = <1.0/127.5, -1.0, ???? , ????>
	mad r0, v1, c0.xxxx, c0.yyyy ; multiply compressed normal by 1/127.5, subtract 1

	Also, see:
	http://flohofwoe.blogspot.com/2008/03/vertex-component-packing.html
-----------------------------------------------------------------
*/
#pragma pack (push,1)
struct rxNormal4
{
	union
	{
		struct {
			UINT8 x, y, z, w;
		};
		UINT32	cell;
	};

public:
	FORCEINLINE rxNormal4()
	{
		cell = 0;
	}

	FORCEINLINE Vec3D Unpack() const
	{
		// [0..255] int -> [-1..+1] float
		const FLOAT f = 1.0f / 127.5f;
		return Vec3D(
			(FLOAT) this->x * f - 1.0f,
			(FLOAT) this->y * f - 1.0f,
			(FLOAT) this->z * f - 1.0f
		);
	}

	FORCEINLINE void Pack( const Vec3D& v )
	{
		// [-1..+1] float -> [0..255] int
		this->x = Clamp(mxTruncate( v.x * 127.5f + 127.5f ), 0, 255 );
		this->y = Clamp(mxTruncate( v.y * 127.5f + 127.5f ), 0, 255 );
		this->z = Clamp(mxTruncate( v.z * 127.5f + 127.5f ), 0, 255 );
		this->w = 0;
	}
};
#pragma pack (pop)

template<>
struct TypeTrait< rxNormal4 >
{
	static bool IsPlainOldDataType() { return true; }
};

//
//	rxNormal6 - packed into 6 bytes, conversions to/from a 32-bit float vector are costly.
//	can pack this into 4 bytes and restore z (z = sqrt(x*x + y*y))
//
#pragma pack (push,1)
struct rxNormal6
{
	HalfFloat	x, y, z;

public:
	FORCEINLINE rxNormal6()
	{
		x = y = z = 0;
	}

	FORCEINLINE Vec3D Unpack() const
	{
		return Vec3D(
			mxHalfToFloat( this->x ),
			mxHalfToFloat( this->y ),
			mxHalfToFloat( this->z )
		);
	}

	FORCEINLINE void Pack( const Vec3D& v )
	{
		this->x = mxFloatToHalf( v.x );
		this->y = mxFloatToHalf( v.y );
		this->z = mxFloatToHalf( v.z );
	}
};
#pragma pack (pop)



// Turns a normalized vector into RGBA form. Used to encode vectors into a height map. Comes from NVIDIA.
inline UINT VectorToRGBA( const Vec3D& v, UINT a = 0xff )
{
	const UINT r = (UINT)( 127.0f * v.x + 128.0f );
	const UINT g = (UINT)( 127.0f * v.y + 128.0f );
	const UINT b = (UINT)( 127.0f * v.z + 128.0f );

	return((a<<24L) + (r<<16L) + (g<<8L) + (b<<0L));
}



inline FLOAT Gaussian1D( FLOAT distance, FLOAT standardDeviation )
{
	return
		1.0f / ( mxSqrt( 2.0f * MX_PI ) * standardDeviation )
			*
			mxExp(
				-mxPow( distance, 2.0f ) / ( 2.0f * mxPow( standardDeviation, 2.0f ) )
			);
}

// the same as previous, but somewhat more optimized.
// rho - standard deviation
//
inline FLOAT GaussianDistribution1D( FLOAT x, FLOAT rho )
{
	const FLOAT g = 1.0f / ( rho * mxSqrt( MX_TWO_PI ) );
    return g * mxExp( -( x * x ) / ( 2 * rho * rho ) );
}

inline FLOAT GaussianDistribution2D( FLOAT x, FLOAT y, FLOAT rho )
{
	FLOAT g = mxInvSqrt( MX_TWO_PI * rho * rho );
    g *= mxExp( -( x * x + y * y ) / ( 2 * rho * rho ) );
    return g;
}

void CalculateGaussianBlurTaps(bool bHorizontal, const Vec2D& texelSize, UINT numTaps,
							   Vec2D *vSampleOffsets, FLOAT *fSampleWeights);


/*
=====================================================================

	Elementary graphics primitives used in rasterization.

=====================================================================
*/

//
//	EPrimitiveType - enumerates all allowed types of elementary graphics primitives.
//
//	NOTE: these are defined to have the same value as the equivalent Direct3D constants.
//
struct EPrimitiveType
{
	enum Code
	{
		PT_Unknown		 = 0,	// Error.
		PT_PointList	 = 1,	// A collection of isolated points.
		PT_LineList		 = 2,	// A list of points, one vertex per point.
		PT_LineStrip	 = 3,	// A strip of connected lines, 1 vertex per line plus one 1 start vertex.
		PT_TriangleList  = 4,	// A list of triangles, 3 vertices per triangle.
		PT_TriangleStrip = 5,	// A string of triangles, 3 vertices for the first triangle, and 1 per triange after that.
		PT_TriangleFan	 = 6,	// A string of triangles, 3 vertices for the first triangle, and 1 per triange after that.
		PT_Quads		 = 7,
	};

	static const char* ToString( Code code );

	// Computes number of vertices/indices given a primitive topology and number of primitives.
	static SizeT ComputeNumberOfVertices( Code topology, SizeT numPrimitives );

	// Computes number of primitives given a primitive type and number of vertices.
	static SizeT ComputeNumberOfPrimitives( Code topology, SizeT numVertices );

	// Computes number of primitives given a primitive type and number of indices.
	static SizeT ComputeNumberOfIndexedPrimitives( Code topology, SizeT numIndices );
};

/*
=====================================================================
	
	Render meshes.

=====================================================================
*/

// mesh topology
#if MX_DEBUG
	typedef EPrimitiveType::Code rxTopology;
#else
	typedef TEnum
	<
		EPrimitiveType::Code,
		UINT
	>	rxTopology;
#endif

template<>
struct TypeTrait< rxTopology >
{
	static bool IsPlainOldDataType() { return true; }
};

template< class S >
inline S& operator & ( S & serializer, rxTopology & o )
{
	return serializer & (UINT&)o;
}


/*
-----------------------------------------------------------------------------
	IndexedMesh

	Generic mesh type, used in editor mode.
	Raw mesh data, used as an intermediate form
	for generating platform-optimized geometry from source assets
-----------------------------------------------------------------------------
*/
struct IndexedMesh
{
	// vertex data - memory is managed by the user

	Vec3D *	positions;
	Vec2D *	texCoords;
	Vec3D *	tangents;
	Vec3D *	binormals;
	Vec3D *	normals;
	Vec3D *	colors;
	UINT	numVertices;

	// index data - memory is managed by the user

	void *	indices;	// pointer to indices
	UINT	indexStride;// 32-bit indices or 16-bit indices
	UINT	numIndices;

	// mesh info
	AABB	bounds;	// local-space bounds

	// batches (Primitive Groups, submeshes)
	//EdSubMesh *	subsets;
	//UINT		numSubsets;

	// EPrimitiveType::PT_TriangleList by default
	//rxTopology	primType;

	// should we call delete [] in destructor?
	//bool	bOwnsData;

public:

	IndexedMesh();
	~IndexedMesh();

	bool isOk() const;

	// used by code generators
	static const char* GetStreamNameBySemantic( EVertexElementUsage usage );

	EPrimitiveType::Code GetPrimTopology() const
	{
		return EPrimitiveType::PT_TriangleList;
	}
};

FORCEINLINE void CopyVertexElement( float4& dest, const Vec3D& src )
{
	dest = XMLoadFloat3( (const XMFLOAT3*) &src );
}
FORCEINLINE void CopyVertexElement( float3& dest, const Vec3D& src )
{
	dest = src;
}
FORCEINLINE void CopyVertexElement( float2& dest, const Vec3D& src )
{
	dest = src.ToVec2();
}
FORCEINLINE void CopyVertexElement( float2& dest, const Vec2D& src )
{
	dest = src;
}
FORCEINLINE void CopyVertexElement( rxNormal4& dest, const Vec3D& src )
{
	dest.Pack( src );
}
FORCEINLINE void CopyVertexElement( R8G8B8A8& dest, const Vec3D& src )
{
	FColor	color;
	color.Set( src.x, src.y, src.z, 1.0f );
	dest.asU32 = color.ToRGBA32();
}

template< typename DEST_TYPE, typename SRC_TYPE >
inline void TCopyVertices( DEST_TYPE* dest, const SRC_TYPE* src,
						  const SizeT vertexCount, const SizeT stride, const SizeT startOffset = 0 )
{
	AssertPtr(dest);
	AssertPtr(src);
	Assert(vertexCount > 0);
	Assert(stride > 0);

	dest = c_cast(DEST_TYPE*) ( c_cast(BYTE*)dest + startOffset );

	for ( UINT iVertex = 0; iVertex < vertexCount; iVertex++ )
	{
		CopyVertexElement( *dest, src[iVertex] );

		dest = c_cast(DEST_TYPE*) ( c_cast(BYTE*)dest + stride );
	}
}

// vertex format GUID (usually, it's a hash of the vertex format's name)
typedef UINT rxVertexFormatGUID;

// unique vertex format index (offset into the table with vertex info)
typedef UINT rxVertexFormatID;

/*
=======================================================================

	Render view

=======================================================================
*/

#define RX_DEFAULT_ASPECT_RATIO		1.333333F

#define RX_CAMERA_DEFAULT_NEAR_PLANE	1.0f
#define RX_CAMERA_DEFAULT_FAR_PLANE		5000.0f
#define RX_CAMERA_WIDESCREEN_ASPECT		(16.0f/9.0f)
#define RX_CAMERA_DEFAULT_ASPECT		(RX_DEFAULT_ASPECT_RATIO)
#define RX_CAMERA_DEFAULT_FIELD_OF_VIEW	(MX_ONEFOURTH_PI)


FORCEINLINE FLOAT CalcViewportAspectRatio( UINT viewportWidth, UINT viewportHeight )
{
	FLOAT aspectRatio = (FLOAT) INT(viewportWidth) / (FLOAT) INT(viewportHeight);
	return aspectRatio;
}

//
//	rxView
//
class rxView : public SBaseType
{
public:
	// Basis vectors, expressed in world space:
	// X(right), Y(up) and Z(lookDir) vectors, must be orthonormal (they form the camera coordinate system).
	Vec3D		right;			// Right direction.
	Vec3D		up;				// Up direction.
	Vec3D		look;			// Look direction.
	Vec3D		origin;			// Eye position - Location of the camera, expressed in world space.
	FLOAT		nearZ, farZ;	// Near and far clipping planes.
	FLOAT		fovY;			// Vertical field of view angle, in radians.
	FLOAT		aspectRatio;	// projection ratio (ratio between width and height of view surface)

public:
	inline rxView()
	{
		SetDefaults();
	}

	inline void SetView(
		const Vec3D& eyePosition,
		const Vec3D& lookDirection,
		const Vec3D& upVector = Vec3D::vec3_unit_y
	)
	{
		Assert(lookDirection.LengthSqr() > SMALL_NUMBER);
		Assert(upVector.LengthSqr() > SMALL_NUMBER);

		up		= upVector;
		look	= lookDirection;
		right	= up.Cross( look ).GetNormalized();
		origin	= eyePosition;
	}

	inline void SetLens(
		FLOAT inNearZ, FLOAT inFarZ,
		FLOAT inFoV_Y
	)
	{
		Assert( inNearZ > 0.0f && inNearZ < inFarZ );
		Assert( inFoV_Y > 0.0f && inFoV_Y < MX_HALF_PI );
		nearZ	= inNearZ;
		farZ	= inFarZ;
		fovY	= inFoV_Y;
	}

	inline void SetDefaults()
	{
		right	.Set( 1.0f, 0.0f, 0.0f );
		up		.Set( 0.0f, 1.0f, 0.0f );
		look	.Set( 0.0f, 0.0f, 1.0f );
		origin	.Set( 0.0f, 0.0f, 0.0f );
		nearZ	= 1.0f;
		farZ	= 5000.0f;
		fovY	= RX_CAMERA_DEFAULT_FIELD_OF_VIEW;
		aspectRatio = RX_DEFAULT_ASPECT_RATIO;
	}

	bool isOk() const
	{
		bool bOk = 1;

		bOk &= CHK(right.IsNormalized());
		bOk &= CHK(up.IsNormalized());
		bOk &= CHK(look.IsNormalized());

		bOk &= CHK(nearZ > 0.0f);
		bOk &= CHK(nearZ < farZ);

		bOk &= CHK(fovY > 0.0f && fovY < MX_HALF_PI);

		return bOk;
	}

	//--------------------------------------------------------------//
	//	Serializers.
	//--------------------------------------------------------------//

	template< class SERIALIZER >
	friend SERIALIZER& operator & ( SERIALIZER & serializer, rxView & o )
	{
		return serializer
			& o.right
			& o.up
			& o.look
			& o.origin
			& o.nearZ
			& o.farZ
			& o.fovY
			& o.aspectRatio
			;
	}
	friend mxArchive& operator && ( mxArchive & serializer, rxView & o )
	{
		return Serialize_ArcViaBin( serializer, o );
	}

	float GetHorizontalFov() const
	{
		float fFractionVert = mxTan( fovY * 0.5f );
		float fFractionHoriz = fFractionVert * aspectRatio;
		float fHorizFov = mxATan(fFractionHoriz) * 2.0f;

		return fHorizFov;
	}

	float4x4 CreateViewMatrix() const
	{
#if 1
		float4 eyePosition = vec_load1( origin );
		float4 eyeDirection = vec_load0( look );
		float4 upDirection = vec_load0( up );
		return XMMatrixLookToLH( eyePosition, eyeDirection, upDirection );
#else
		float4x4	viewMatrix;
		as_matrix4(viewMatrix).BuildLookAtLH(
			origin,
			right,
			up,
			look
		);
		return viewMatrix;
#endif
	}


	FORCEINLINE float4x4 GetInverseViewMatrix() const
	{
#if 1
		float4 dirX = vec_load0( right );
		float4 dirY = vec_load0( up );
		float4 dirZ = vec_load0( look );
		float4 vecO = vec_load1( origin );

		return float4x4(
			dirX,
			dirY,
			dirZ,
			vecO
		);
#else
		return Matrix4(
			Vec4D( this->right,		0.0f ),
			Vec4D( this->up,		0.0f ),
			Vec4D( this->look,		0.0f ),
			Vec4D( this->origin,	1.0f )
		);
#endif
	}

	float4x4 CreateProjectionMatrix() const
	{
#if 1
		return XMMatrixPerspectiveFovLH( fovY, aspectRatio, nearZ, farZ );
#else
		float4x4	projMatrix;
		as_matrix4(projMatrix).BuildPerspectiveLH(
			fovY,
			aspectRatio,
			nearZ,
			farZ
		);
		return projMatrix;
#endif
	}

	float4x4 CalcViewProjMatrix() const
	{
		float4x4 viewMatrix = CreateViewMatrix();
		float4x4 projMatrix = CreateProjectionMatrix();
		return XMMatrixMultiply(viewMatrix,projMatrix);
	}

	mxDECLARE_CLASS(rxView,SBaseType);
	mxDECLARE_REFLECTION;

	void edCreateProperties( EdPropertyList *properties, bool bClear = true );
};


/*
=======================================================================

	GPU programs (shaders)

=======================================================================
*/

//
//	EShaderType
//
enum EShaderType
{
	ST_Unknown_Shader = 0,

	ST_Vertex_Shader,
	ST_Hull_Shader,
	ST_Tessellation_Shader,
	ST_Domain_Shader,
	ST_Geometry_Shader,
	ST_Pixel_Shader,

	ST_Compute_Shader,

	NumShaderTypes
};

const char* GetShaderTypeString( EShaderType type );

//
//	EGpuConstantType
//
struct EGpuConstantType
{
	// This enum contains types of constants used in GPU programs.
	// Note: the order is important (optimized for range checks).
	enum Code
	{
		Float1,
		Float2,
		Float3,
		Float4,

		Matrix2x2,
		Matrix2x3,
		Matrix2x4,
		Matrix3x2,
		Matrix3x3,
		Matrix3x4,
		Matrix4x2,
		Matrix4x3,
		Matrix4x4,

		Int1,
		Int2,
		Int3,
		Int4,
		
		Sampler1D,
		Sampler2D,
		Sampler3D,
		SamplerCube,
	};

	FORCEINLINE
		bool IsInteger( EGpuConstantType::Code type )
	{
		return (type >= Int1) && (type <= Int4);
	}

	FORCEINLINE
	bool IsFloat( EGpuConstantType::Code type )
	{
		return (type >= Float1) && (type <= Matrix4x4);
	}

	FORCEINLINE
	bool IsSampler( EGpuConstantType::Code type )
	{
		return (type >= Sampler1D) && (type <= SamplerCube);
	}
};

// immutable render state objects
//
enum ERenderStateType
{
	RST_DepthStencil,
	RST_Blend,
	RST_Rasterizer,
	RST_Sampler,

	RST_MAX
};

enum EImageFileFormat
{
	IFF_Unknown,
	IFF_BMP,
	IFF_JPG,
	IFF_PNG,
	IFF_DDS,
	IFF_TIFF,
	IFF_GIF,
	IFF_WMP,
};

// e.g. ".jpg", ".bmp"
const char* GetImageFileFormatFileExt( EImageFileFormat e );

/*
=======================================================================

	Initialization and destruction

=======================================================================
*/






/*
=======================================================================

	Utilities

=======================================================================
*/

//
//	EDriverType
//
enum EDriverType
{
	GAPI_None = 0,		// can be used for applications which don't need visualization

	GAPI_Direct3D_9,	// D3D9 without shaders (fixed-function pipeline, D3D7-like).
	GAPI_Direct3D_9S,	// D3D9 using shaders only.

	GAPI_Direct3D_10,	//  on Windows Vista and above (which is always shaders only).
	GAPI_Direct3D_10_1,
	GAPI_Direct3D_11,

	GAPI_OpenGL,		// OpenGL without shaders.
	GAPI_OpenGLS,		// OpenGL using shaders only.

	GAPI_Unknown,

	GAPI_AutomaticSelection,	// Select the best GAPI automatically.

	GAPI_Count
};

//
// rxUtil_GetDriverName(EDriverType) - Converts enum to the corresponding string.
//
const char* rxUtil_GetDriverName( EDriverType driver );

//
// EDeviceVendor
//
enum EDeviceVendor
{
	Vendor_Unknown	= 0,
	Vendor_3DLABS	= 1,
	Vendor_ATI		= 2,
	Vendor_Intel	= 3,
	Vendor_Matrox	= 4,
	Vendor_NVidia	= 5,
	Vendor_S3		= 6,
	Vendor_SIS		= 7,
};

EDeviceVendor	rxUtil_GetGPUVendorEnum( UINT vendorId );
const WChar *	rxUtil_GetGPUVendorName( EDeviceVendor vendor );


// Camera model.

class SCamera : public SBaseType
{
public:
	enum Actions
	{
		Unknown,
		//ResetToDefaults,
		MoveForward,
		MoveBackward,
		MoveUp,
		MoveDown,
		StrafeLeft,
		StrafeRight,
		MAX
	};

public:

	static FLOAT GetMinMovementVelocity();
	static FLOAT GetMaxMovementVelocity();
	static FLOAT GetDefaultMovementVelocity();

	static FLOAT GetDefaultRotationVelocity();

	SCamera();
	//FPSCamera( const FPSCamera& other );

	void Update( FLOAT deltaTime );

	const rxView& GetView() const { return view; }
	rxView& GetView() { return view; }

	void OnKeyPressed( enum EKeyCode key );
	void OnKeyReleased( enum EKeyCode key );

	void OnMouseMove( FLOAT mouseDeltaX, FLOAT mouseDeltaY );

	// resets to defaults
	void ResetView();

	void SetView( const rxView& src );

	void SetAspectRatio( FLOAT newAspectRatio );

	inline void operator = ( const SCamera& other )
	{
		this->view = other.view;

		this->rotationScaler = other.rotationScaler;
		this->moveScaler = other.moveScaler;

		this->totalDragTimeToZero = other.totalDragTimeToZero;

		this->yawAngle = other.yawAngle;
		this->pitchAngle = other.pitchAngle;

		this->numFramesToSmoothMouseData = other.numFramesToSmoothMouseData;
	}

	//--------------------------------------------------------------//
	//	Serializers.
	//--------------------------------------------------------------//

	template< class SERIALIZER >
	friend SERIALIZER& operator & ( SERIALIZER & serializer, SCamera & o )
	{
		serializer & o.view;

		serializer & o.rotationScaler;
		serializer & o.moveScaler;

		serializer & o.totalDragTimeToZero;

		serializer & o.yawAngle;
		serializer & o.pitchAngle;

		serializer & o.numFramesToSmoothMouseData;

		return serializer;
	}
	friend mxArchive& operator && ( mxArchive & serializer, SCamera & o )
	{
		return Serialize_ArcViaBin( serializer, o );
	}

	mxDECLARE_CLASS(SCamera,SBaseType);
	mxDECLARE_REFLECTION;

	void edCreateProperties( EdPropertyList *properties, bool bClear = true );

private:
	rxView	view;	// view parameters

	bool	pressedKeys[ Actions::MAX ];	// +no save

	FLOAT	rotationScaler;      // Scaler for rotation
	FLOAT	moveScaler;          // Scaler for movement

	Vec3D	velocity;            // Velocity of camera
	Vec3D	velocityDrag;        // Velocity drag force
	FLOAT	totalDragTimeToZero; // Time it takes for velocity to go from full to 0
	FLOAT	dragTimer;           // Countdown timer to apply drag
	Vec2D	rotationVelocity;    // Velocity of camera
	FLOAT	yawAngle;
	FLOAT	pitchAngle;

	Vec2D	mouseDelta;			// Mouse relative delta smoothed over a few frames
	FLOAT	numFramesToSmoothMouseData;// Number of frames to smooth mouse data over

private:
	NO_COPY_CONSTRUCTOR(SCamera);
	//NO_COMPARES(FPSCamera);
};

/*
=======================================================================

	Fonts

=======================================================================
*/

// describes character position and size in a font texture
struct GlyphInfo
{
	FLOAT X;	// in texels
	FLOAT Y;	// in texels
	FLOAT Width;	// in texels
	FLOAT Height;	// in texels
};
mxDECLARE_POD_TYPE(GlyphInfo);

// NOTE: these values mirror the corresponding values in GDI+
enum EFontStyle
{
	Regular		= 0,
	Bold		= 1 << 0,
	Italic		= 1 << 1,
	BoldItalic	= Bold | Italic,
	Underline	= 1 << 2,
	Strikeout	= 1 << 3
};
namespace FontGen
{
	static const WCHAR StartChar = '!';
	static const WCHAR EndChar = 127;
	static const SizeT NumChars = EndChar - StartChar;
	static UINT TexWidth = 1024;	// default texture width

}//namespace FontGen

struct FontInfo
{
	GlyphInfo		charDescs[ FontGen::NumChars ];
	FLOAT			size;		// font size
	UINT			texWidth;	// the width of the font texture
	UINT			texHeight;	// the height of the font texture
	FLOAT			spaceWidth;	// the width of the whitespace character (' ')
	FLOAT			charHeight;

public:
	FontInfo()
	{
		ZERO_OUT( charDescs );
		size = 0.0f;
		texWidth = 0;
		texHeight = 0;
		spaceWidth = 0.0f;
		charHeight = 0.0f;
	}

	template< class SERIALIZER >
	friend SERIALIZER& operator & ( SERIALIZER & serializer, FontInfo & o )
	{
		serializer.SerializeChunk( o.charDescs );
		return serializer
			& o.size
			& o.texWidth
			& o.texHeight
			& o.spaceWidth
			& o.charHeight
			;
	}
};

//
//	ETextAlignment
//
enum ETextAlignment
{
	TA_AlignLeft,
	TA_AlignRight,
	TA_AlignCenter,
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
