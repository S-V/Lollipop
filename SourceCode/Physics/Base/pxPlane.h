/*
=============================================================================
	File:	pxPlane.h
	Desc:	3-dimensional plane.
=============================================================================
*/

#ifndef __PX_PLANE_H__
#define __PX_PLANE_H__

MX_ALIGN_16(class) pxPlane
{
public:
	union {
		struct {
			FLOAT		a;
			FLOAT		b;
			FLOAT		c;
			FLOAT		d;	// distance from the origin
		};
		pxQuadReal	simdQuad;
	};

public:
	FORCEINLINE pxPlane()
	{}

	FORCEINLINE pxPlane( const pxPlane& other )
	{
		this->simdQuad = other.simdQuad;
	}

	FORCEINLINE pxPlane( FLOAT a, FLOAT b, FLOAT c, FLOAT d )
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	FORCEINLINE pxReal GetDistance( const pxVec3& point ) const {
		return (a * point.x + b * point.y) + (c * point.z + d);
	}

	FORCEINLINE pxVec3& GetNormal() {
		return reinterpret_cast<pxVec3&>(a);
	}
	FORCEINLINE const pxVec3& GetNormal() const {
		return reinterpret_cast<const pxVec3&>(a);
	}

	FORCEINLINE pxReal* ToPtr() {
		return (pxReal*)this;
	}
	FORCEINLINE const pxReal* ToPtr() const {
		return (const pxReal*)this;
	}

};

#endif // !__PX_PLANE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
