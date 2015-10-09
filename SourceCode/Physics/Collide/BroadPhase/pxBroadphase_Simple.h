/*
=============================================================================
	File:	pxBroadphase_Simple.h
	Desc:	Simple brute-force broadphase
			(no acceleration structure, tests all against all, quadratic complexity).
=============================================================================
*/

#ifndef __PX_BROAD_PHASE_SIMPLE_H__
#define __PX_BROAD_PHASE_SIMPLE_H__

typedef AABB	pxSimpleBounds;

PX_INLINE bool pxSimpleBoundsOverlap( const pxSimpleBounds& bA, const pxSimpleBounds& bB )
{
	return bA.IntersectsBounds( bB );
}

PX_INLINE void pxSimpleBoundsFromCollideable( const pxCollideable* object, pxSimpleBounds & outBounds )
{
	pxAABB aabb;
	object->GetWorldBounds( aabb );

	outBounds = aabb.GetExpandedBy(PX_COLLISION_TOLERANCE).ToAABB();
}

//
//	pxSimpleBroadphaseProxy
//
MX_ALIGN_16(struct) pxSimpleBroadphaseProxy
{
	PX_DECLARE_POD_ALLOCATOR( pxBroadphase_SimpleProxy, PX_MEMORY_COLLISION_BROADPHASE );

	pxSimpleBounds	bounds;// bounds in world space
	TPtr<pxCollideable>	o;
	UINT	nextFree;	// index of the next available handle (for organizing a linked list)
};

//
//	pxBroadphase_Simple
//
//	Uses a Brute-Force method, tests successively all objects pairwise for an actual collision.
//
class pxBroadphase_Simple : public pxBroadphase
{
	PX_DECLARE_CLASS_ALLOCATOR( pxBroadphase_SimpleProxy, PX_MEMORY_COLLISION_BROADPHASE );

public:
	//pxBroadphase_Simple( const pxBroadphaseDesc& desc );
	pxBroadphase_Simple();
	~pxBroadphase_Simple();

public:	//-pxBroadphase
	virtual void Add( pxCollideable* object ) override;
	virtual void Remove( pxCollideable* object ) override;
	virtual pxUInt GetNumObjects() const override;
	virtual pxUInt GetMaxObjects() const override;

	virtual void Clear() override;

	virtual void Collide( pxCollisionDispatcher & handler ) override;

	virtual void CastRay( const WorldRayCastInput& input, WorldRayCastOutput &output ) override;
	virtual void LinearCast( const ConvexCastInput& input, ConvexCastOutput &output ) override;
	virtual void TraceBox( const TraceBoxInput& input, TraceBoxOutput &output ) override;

	virtual void validate() override;

public_internal:
	PX_INLINE pxUInt GetNumHandles() const {
		return mHandles.Num();
	}
	PX_INLINE pxUInt GetMaxHandles() const {
		return mHandles.GetCapacity();
	}

private:
	pxUInt _AllocHandle();
	void _FreeHandle( pxUInt handle );
	void _ClearHandles();

private:
	TList< pxSimpleBroadphaseProxy >	mHandles;
	UINT		mFirstFreeHandle;
};

#endif // !__PX_BROAD_PHASE_SIMPLE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
