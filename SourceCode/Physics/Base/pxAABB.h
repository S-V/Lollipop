/*
=============================================================================
	File:	pxAABB.h
	Desc:	Axis aligned bounding box.
=============================================================================
*/

#ifndef __PX_AABB_H__
#define __PX_AABB_H__

#include <Physics/Base/pxSphere.h>
#include <Physics/Base/pxTransform.h>

//
//	pxAABB
//
MX_ALIGN_16(struct) pxAABB
{
	pxVec3 mMin;	// The minimum boundary of the aabb (i.e. the coordinates of the corner with the lowest numerical values).
	pxVec3 mMax;	// The maximum boundary of the aabb (i.e. the coordinates of the corner with the highest numerical values).

public:
	FORCEINLINE pxAABB()
	{}

	FORCEINLINE pxAABB( const pxVec3& min, const pxVec3& max )
		: mMin( min ), mMax( max )
	{}

	FORCEINLINE const pxVec3& GetMin() const
	{
		return mMin;
	}
	FORCEINLINE const pxVec3& GetMax() const
	{
		return mMin;
	}


	// Returns true if the given AABB overlaps with this one. Zero volume overlaps are reported as an overlap.
	FORCEINLINE bool Overlaps( const pxAABB& other ) const {
		// conservative test for overlap between two aabbs
		bool overlap = true;
		overlap = (mMin.getX() > other.mMax.getX() || mMax.getX() < other.mMin.getX()) ? false : overlap;
		overlap = (mMin.getZ() > other.mMax.getZ() || mMax.getZ() < other.mMin.getZ()) ? false : overlap;
		overlap = (mMin.getY() > other.mMax.getY() || mMax.getY() < other.mMin.getY()) ? false : overlap;
		return overlap;
	}

	FORCEINLINE bool Contains( const pxVec3& point ) const
	{
		bool overlap = true;
		overlap = (mMin.getX() > point.getX() || mMax.getX() < point.getX()) ? false : overlap;
		overlap = (mMin.getZ() > point.getZ() || mMax.getZ() < point.getZ()) ? false : overlap;
		overlap = (mMin.getY() > point.getY() || mMax.getY() < point.getY()) ? false : overlap;
		return overlap;
	}
	
	FORCEINLINE pxVec3 GetCenter() const {
		return pxReal(0.5) * (mMin + mMax);
	}
	
	FORCEINLINE pxVec3 GetSize() const {
		return (mMax - mMin);
	}

	FORCEINLINE pxVec3 GetHalfSize() const {
		return pxReal(0.5) * GetSize();
	}

	FORCEINLINE pxAABB GetExpandedBy( pxReal margin ) const {
		const pxVec3 extent( margin, margin, margin );
		return pxAABB(
			mMin - extent,
			mMax + extent
		);
	}

	FORCEINLINE void ExpandByPoint( const pxVec3& point )
	{
#if PX_FORCE_FPU
		mMin.setMin( mMin, point );
		mMax.setMax( mMax, point );
#else
		mMin.mVec128 = _mm_min_ps( mMin.mVec128, point.mVec128 );
		mMax.mVec128 = _mm_min_ps( mMax.mVec128, point.mVec128 );
#endif
	}

	// inside out bounds
	FORCEINLINE void Clear( void ) {
		mMin.SetAll( +PX_LARGE_FLOAT );
		mMax.SetAll( -PX_LARGE_FLOAT );
	}

	FORCEINLINE void Set( const pxVec3& min, const pxVec3& max ) {
		mMin = min;
		mMax = max;
	}

	FORCEINLINE void SetZero( void ) {
		mMin.SetAll( pxReal(0.0) );
		mMax.SetAll( pxReal(0.0) );
	}

	FORCEINLINE void SetInfinity() {
		mMin.SetAll( -PX_LARGE_FLOAT );
		mMax.SetAll( +PX_LARGE_FLOAT );
	}

	FORCEINLINE pxSphere ToSphere() const {
		return pxSphere(
			GetCenter(),
			GetHalfSize().Length()
		);
	}

	FORCEINLINE AABB ToAABB() const
	{
		return AABB( mMin.As_Vec3D(), mMax.As_Vec3D() );
	}
};

FORCEINLINE void AabbExpand (pxVec3& aabbMin,
							 pxVec3& aabbMax,
							 const pxVec3& expansionMin,
							 const pxVec3& expansionMax)
{
	aabbMin = aabbMin + expansionMin;
	aabbMax = aabbMax + expansionMax;
}

FORCEINLINE bool TestPointAgainstAabb2(const pxVec3 &aabbMin1, const pxVec3 &aabbMax1,
									   const pxVec3 &point)
{
	bool overlap = true;
	overlap = (aabbMin1.getX() > point.getX() || aabbMax1.getX() < point.getX()) ? false : overlap;
	overlap = (aabbMin1.getZ() > point.getZ() || aabbMax1.getZ() < point.getZ()) ? false : overlap;
	overlap = (aabbMin1.getY() > point.getY() || aabbMax1.getY() < point.getY()) ? false : overlap;
	return overlap;
}


/// conservative test for overlap between two aabbs
FORCEINLINE bool TestAabbAgainstAabb2(const pxVec3 &aabbMin1, const pxVec3 &aabbMax1,
									  const pxVec3 &aabbMin2, const pxVec3 &aabbMax2)
{
	bool overlap = true;
	overlap = (aabbMin1.getX() > aabbMax2.getX() || aabbMax1.getX() < aabbMin2.getX()) ? false : overlap;
	overlap = (aabbMin1.getZ() > aabbMax2.getZ() || aabbMax1.getZ() < aabbMin2.getZ()) ? false : overlap;
	overlap = (aabbMin1.getY() > aabbMax2.getY() || aabbMax1.getY() < aabbMin2.getY()) ? false : overlap;
	return overlap;
}

/// conservative test for overlap between triangle and aabb
FORCEINLINE bool TestTriangleAgainstAabb2(const pxVec3 *vertices,
										  const pxVec3 &aabbMin, const pxVec3 &aabbMax)
{
	const pxVec3 &p1 = vertices[0];
	const pxVec3 &p2 = vertices[1];
	const pxVec3 &p3 = vertices[2];

	if (minf(minf(p1[0], p2[0]), p3[0]) > aabbMax[0]) return false;
	if (maxf(maxf(p1[0], p2[0]), p3[0]) < aabbMin[0]) return false;

	if (minf(minf(p1[2], p2[2]), p3[2]) > aabbMax[2]) return false;
	if (maxf(maxf(p1[2], p2[2]), p3[2]) < aabbMin[2]) return false;

	if (minf(minf(p1[1], p2[1]), p3[1]) > aabbMax[1]) return false;
	if (maxf(maxf(p1[1], p2[1]), p3[1]) < aabbMin[1]) return false;
	return true;
}


FORCEINLINE int	pxOutcode(const pxVec3& p,const pxVec3& halfExtent) 
{
	return (p.getX()  < -halfExtent.getX() ? 0x01 : 0x0) |    
		(p.getX() >  halfExtent.getX() ? 0x08 : 0x0) |
		(p.getY() < -halfExtent.getY() ? 0x02 : 0x0) |    
		(p.getY() >  halfExtent.getY() ? 0x10 : 0x0) |
		(p.getZ() < -halfExtent.getZ() ? 0x4 : 0x0) |    
		(p.getZ() >  halfExtent.getZ() ? 0x20 : 0x0);
}



FORCEINLINE bool pxRayAabb2(const pxVec3& rayFrom,
							const pxVec3& rayInvDirection,
							const unsigned int raySign[3],
							const pxVec3 bounds[2],
							pxReal& tmin,
							pxReal lambda_min,
							pxReal lambda_max)
{
	pxReal tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[raySign[0]].getX() - rayFrom.getX()) * rayInvDirection.getX();
	tmax = (bounds[1-raySign[0]].getX() - rayFrom.getX()) * rayInvDirection.getX();
	tymin = (bounds[raySign[1]].getY() - rayFrom.getY()) * rayInvDirection.getY();
	tymax = (bounds[1-raySign[1]].getY() - rayFrom.getY()) * rayInvDirection.getY();

	if ( (tmin > tymax) || (tymin > tmax) )
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[raySign[2]].getZ() - rayFrom.getZ()) * rayInvDirection.getZ();
	tzmax = (bounds[1-raySign[2]].getZ() - rayFrom.getZ()) * rayInvDirection.getZ();

	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ( (tmin < lambda_max) && (tmax > lambda_min) );
}

FORCEINLINE bool pxRayAabb(const pxVec3& rayFrom, 
						   const pxVec3& rayTo, 
						   const pxVec3& aabbMin, 
						   const pxVec3& aabbMax,
						   pxReal& param, pxVec3& normal) 
{
	pxVec3 aabbHalfExtent = (aabbMax-aabbMin)* pxReal(0.5);
	pxVec3 aabbCenter = (aabbMax+aabbMin)* pxReal(0.5);
	pxVec3	source = rayFrom - aabbCenter;
	pxVec3	target = rayTo - aabbCenter;
	int	sourceOutcode = pxOutcode(source,aabbHalfExtent);
	int targetOutcode = pxOutcode(target,aabbHalfExtent);
	if ((sourceOutcode & targetOutcode) == 0x0)
	{
		pxReal lambda_enter = pxReal(0.0);
		pxReal lambda_exit  = param;
		pxVec3 r = target - source;
		int i;
		pxReal	normSign = 1;
		pxVec3	hitNormal(0,0,0);
		int bit=1;

		for (int j=0;j<2;j++)
		{
			for (i = 0; i != 3; ++i)
			{
				if (sourceOutcode & bit)
				{
					pxReal lambda = (-source[i] - aabbHalfExtent[i]*normSign) / r[i];
					if (lambda_enter <= lambda)
					{
						lambda_enter = lambda;
						hitNormal.SetValue(0,0,0);
						hitNormal[i] = normSign;
					}
				}
				else if (targetOutcode & bit) 
				{
					pxReal lambda = (-source[i] - aabbHalfExtent[i]*normSign) / r[i];
					TSetMin(lambda_exit, lambda);
				}
				bit<<=1;
			}
			normSign = pxReal(-1.);
		}
		if (lambda_enter <= lambda_exit)
		{
			param = lambda_enter;
			normal = hitNormal;
			return true;
		}
	}
	return false;
}

FORCEINLINE	void pxTransformAabb(const pxVec3& halfExtents,const pxTransform& t,pxVec3& aabbMinOut,pxVec3& aabbMaxOut)
{
	pxMat3x3 abs_b = t.GetBasis().GetAbsolute();  
	pxVec3 center = t.GetOrigin();
	pxVec3 extent = pxVec3(
		abs_b[0].Dot(halfExtents),
		abs_b[1].Dot(halfExtents),
		abs_b[2].Dot(halfExtents)
		);
	aabbMinOut = center - extent;
	aabbMaxOut = center + extent;
}

FORCEINLINE	void pxTransformAabb(const pxVec3& halfExtents, pxReal margin,const pxTransform& t,pxVec3& aabbMinOut,pxVec3& aabbMaxOut)
{
	pxVec3 halfExtentsWithMargin = halfExtents+pxVec3(margin,margin,margin);
	pxMat3x3 abs_b = t.GetBasis().GetAbsolute();  
	pxVec3 center = t.GetOrigin();
	pxVec3 extent = pxVec3(abs_b[0].Dot(halfExtentsWithMargin),
		abs_b[1].Dot(halfExtentsWithMargin),
		abs_b[2].Dot(halfExtentsWithMargin));
	aabbMinOut = center - extent;
	aabbMaxOut = center + extent;
}


FORCEINLINE	void pxTransformAabb(const pxVec3& localAabbMin,const pxVec3& localAabbMax, pxReal margin,const pxTransform& trans,pxVec3& aabbMinOut,pxVec3& aabbMaxOut)
{
	Assert(localAabbMin.getX() <= localAabbMax.getX());
	Assert(localAabbMin.getY() <= localAabbMax.getY());
	Assert(localAabbMin.getZ() <= localAabbMax.getZ());

	pxVec3 localHalfExtents = pxReal(0.5)*(localAabbMax-localAabbMin);
	localHalfExtents+=pxVec3(margin,margin,margin);

	pxVec3 localCenter = pxReal(0.5)*(localAabbMax+localAabbMin);

	pxMat3x3 abs_b = trans.GetBasis().GetAbsolute();

	pxVec3 center = trans(localCenter);
	pxVec3 extent = pxVec3(
		abs_b[0].Dot(localHalfExtents),
		abs_b[1].Dot(localHalfExtents),
		abs_b[2].Dot(localHalfExtents));

	aabbMinOut = center-extent;
	aabbMaxOut = center+extent;
}

#define USE_BRANCHLESS 1
#ifdef USE_BRANCHLESS
//This block replaces the block below and uses no branches, and replaces the 8 bit return with a 32 bit return for improved performance (~3x on XBox 360)
FORCEINLINE unsigned testQuantizedAabbAgainstQuantizedAabb(const unsigned short int* aabbMin1,const unsigned short int* aabbMax1,const unsigned short int* aabbMin2,const unsigned short int* aabbMax2)
{		
	return static_cast<unsigned int>(
		mxSelect((unsigned)(
		(aabbMin1[0] <= aabbMax2[0]) & (aabbMax1[0] >= aabbMin2[0])
		& (aabbMin1[2] <= aabbMax2[2]) & (aabbMax1[2] >= aabbMin2[2])
		& (aabbMin1[1] <= aabbMax2[1]) & (aabbMax1[1] >= aabbMin2[1])),
		1, 0)
		);
}
#else
FORCEINLINE bool testQuantizedAabbAgainstQuantizedAabb(const unsigned short int* aabbMin1,const unsigned short int* aabbMax1,const unsigned short int* aabbMin2,const unsigned short int* aabbMax2)
{
	bool overlap = true;
	overlap = (aabbMin1[0] > aabbMax2[0] || aabbMax1[0] < aabbMin2[0]) ? false : overlap;
	overlap = (aabbMin1[2] > aabbMax2[2] || aabbMax1[2] < aabbMin2[2]) ? false : overlap;
	overlap = (aabbMin1[1] > aabbMax2[1] || aabbMax1[1] < aabbMin2[1]) ? false : overlap;
	return overlap;
}
#endif //USE_BRANCHLESS

FORCEINLINE pxVec3 pxAabbSupport(const pxVec3& halfExtents,const pxVec3& supportDir)
{
	return pxVec3(
		supportDir.getX() < pxReal(0.0) ? -halfExtents.getX() : halfExtents.getX(),
		supportDir.getY() < pxReal(0.0) ? -halfExtents.getY() : halfExtents.getY(),
		supportDir.getZ() < pxReal(0.0) ? -halfExtents.getZ() : halfExtents.getZ()
		);
}

#endif // !__PX_AABB_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
