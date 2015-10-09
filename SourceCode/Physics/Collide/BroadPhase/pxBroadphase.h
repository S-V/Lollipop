/*
=============================================================================
	File:	pxBroadphase.h
	Desc:	collision broadphase is used to quickly identify pairs
			of potentially colliding objects
=============================================================================
*/

#ifndef __PX_BROAD_PHASE_H__
#define __PX_BROAD_PHASE_H__

enum pxcBroadphaseLimits
{
	PX_MAX_BROADPHASE_PROXIES = (1<<12),
	PX_MAX_BROADPHASE_PAIRS = (1<<12),
};

const pxReal PX_MAX_BROADPHASE_SIZE = REAL(999999.0);

//
//	pxBroadphaseDesc
//
struct pxBroadphaseDesc
{
	pxAABB	worldBounds;	// max. size of the world, the less the better

	pxUInt	maxObjects;	// max. allowed number of bodies, must be a power of two
	pxUInt	maxPairs;	// max. allowed number of overlapping pairs, must be a power of two

public:
	pxBroadphaseDesc()
	{
		setDefaults();
	}
	void setDefaults();
	bool isOk() const;
};


struct WorldRayCastInput
{
	Vec3D	origin;
	Vec3D	direction;
	TPtr<pxCollideable>	hitObject;
};
struct WorldRayCastOutput
{
	Vec3D	normal;
	FLOAT	hitFraction;
	TPtr<pxCollideable>	hitObject;
};


struct ConvexCastInput
{
	Vec3D	start;
	Vec3D	end;
	FLOAT	radius;

	// precomputed data
	//Vec3D	bounds[2];	// enclosing box of start and end surrounding by size
};
struct ConvexCastOutput
{
	Vec3D	hitPosition;
	Vec3D	hitNormal;
	FLOAT	hitFraction;
};

struct TraceBoxInput
{
	Vec3D	start;
	Vec3D	end;
	AABB	size;	// size of the box being swept through the model
};
struct TraceBoxOutput
{
	Vec3D	hitPosition;
	Vec3D	hitNormal;
	FLOAT	hitFraction;

public:
	TraceBoxOutput();
};


//
//	pxBroadphase
//
class pxBroadphase
{
	PX_DECLARE_CLASS_ALLOCATOR( pxBroadphase, PX_MEMORY_COLLISION_BROADPHASE );

public:
			pxBroadphase();
			//pxBroadphase( const pxBroadphaseDesc& desc );
	virtual	~pxBroadphase();

	virtual void Reserve( UINT numObjects ) {}

	// Adds an object to this broadphase.
	virtual void Add( pxCollideable* object ) = 0;

	// Removes an object from the broadphase.
	virtual void Remove( pxCollideable* object ) = 0;

	// Returns the total number of objects added to this broadphase.
	virtual pxUInt GetNumObjects() const = 0;

	// Returns the maximum allowed number of objects that can be added to this broadphase.
	virtual pxUInt GetMaxObjects() const = 0;

	// Removes all objects from the broadphase.
	virtual void Clear() = 0;

	// Performs a broad-phase collision detection and feeds potentially colliding pairs to the collision handler.
	virtual void Collide( pxCollisionDispatcher & handler ) = 0;

	virtual void CastRay( const WorldRayCastInput& input, WorldRayCastOutput &output )
	{Unimplemented;}

	virtual void LinearCast( const ConvexCastInput& input, ConvexCastOutput &output )
	{Unimplemented;}

	virtual void TraceBox( const TraceBoxInput& input, TraceBoxOutput &output )
	{Unimplemented;}

	//testing & debugging
	virtual void validate() {};

	virtual void DebugDraw( pxDebugDrawer* renderer ) {};
};

#endif // !__PX_BROAD_PHASE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
