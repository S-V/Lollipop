/*
=============================================================================
	File:	pxShape.h
	Desc:	Collision shape.
=============================================================================
*/

#ifndef __PX_COLLISION_SHAPE_H__
#define __PX_COLLISION_SHAPE_H__

#include <Core/Resources.h>
#include <Physics/Base/pxAABB.h>

class pxShapeRayCastInput;
class pxShapeRayCastOutput;
class pxRayHitCollector;

//
//	pxcShapeType - enumerates types of collision shapes.
//
enum pxcShapeType
{
	PX_SHAPE_UNKNOWN = 0,


	//--- Static collision shape types begin here ------------

		PX_SHAPE_HALFSPACE,

	//--- Static collision shape types end here ------------



	//--- Convex collision shape types begin here ------------

	PX_FIRST_CONVEX_SHAPE,

		PX_SHAPE_SPHERE = PX_FIRST_CONVEX_SHAPE,
		PX_SHAPE_BOX,

	PX_LAST_CONVEX_SHAPE = PX_SHAPE_BOX,

	//--- Convex collision shape types end here ------------

	PX_FIRST_CONCAVE_SHAPE,

		// closed (2-manifold) mesh (not necessarily convex),
		// i.e. a shape which has a closed hull and represents a solid volume object.
		PX_SHAPE_STATIC_BSP = PX_FIRST_CONCAVE_SHAPE,
		PX_SHAPE_DYNAMIC_BSP,	// can move and supports real-time CSG

	PX_LAST_CONCAVE_SHAPE = PX_SHAPE_DYNAMIC_BSP,

	PX_SHAPE_MAX
};

typedef TEnum<pxcShapeType,pxU4>	pxeShapeType;

// A utility function to return a useful name for a given shape type.
const char* pxDbgGetShapeName( pxcShapeType eShapeType );

// A utility function that returns 'true'
// if the given shape type can only be used for static objects,
// and 'false' otherwise.
//
FORCEINLINE
bool pxIsStaticShape( pxcShapeType code )
{
	return 0//(code == PX_SHAPE_PLANE)
		|| (code == PX_SHAPE_HALFSPACE)
		;
}

FORCEINLINE
bool pxShapeIsConvex( pxcShapeType eShapeType )
{
	return eShapeType >= PX_FIRST_CONVEX_SHAPE
		&& eShapeType <= PX_LAST_CONVEX_SHAPE
		;
}

FORCEINLINE
bool pxShapeIsConcave( pxcShapeType eShapeType )
{
	return eShapeType >= PX_FIRST_CONCAVE_SHAPE
		&& eShapeType <= PX_LAST_CONCAVE_SHAPE
		;
}

FORCEINLINE
bool pxShapeIsBSP( pxcShapeType eShapeType )
{
	return eShapeType == PX_SHAPE_STATIC_BSP
		//|| eShapeType == PX_SHAPE_DYNAMIC_BSP
		;
}


// trace for player movement
//
struct ShapePMTraceInput
{
	Vec3D	start, end;
	AABB	size;	// size of the box being swept through the model

	//F4		radius;

	// precomputed data
	Vec3D	direction;	// normalized direction

	AABB	fullTraceBounds;	// bounds of full trace

public:
	void UpdateCachedData();
};

struct ShapePMTraceOutput
{
	//Vec3D	endPos;	// final position
	Vec3D	normal;// surface normal at impact, transformed to world space
	F4		fraction;	// time completed, 1.0 = didn't hit anything
	//bool	startsolid;	// if true, the initial point was in a solid area
	//bool	allsolid;

public:
	ShapePMTraceOutput();
};






//
//	pxShapeDesc
//
struct pxShapeDesc
{
	pxcShapeType	type;

public:
	pxShapeDesc()
	{
		setDefaults();
	}
	void setDefaults()
	{
		type = pxcShapeType::PX_SHAPE_UNKNOWN;
	}
	bool isOk() const
	{
		return (type != pxcShapeType::PX_SHAPE_UNKNOWN)
			&& 1
			;
	}
};

//
//	pxShape - represents collision geometry.
//
//	It is usually a surface/volume representation for doing collision detection.
//
//	pxShape is the base class for all shapes used in narrowphase collision detection.
//
class pxShape : public SResourceObject
{
	PX_DECLARE_NON_POD_CLASS_ALLOCATOR( pxShape, PX_MEMORY_COLLISION );

public:

	typedef TypedHandle< pxShape, pxcShapeType >	Handle;


	// Returns the shape's type. This is used by the collision dispatcher to dispatch between pairs of shapes.
	//
	PX_INLINE pxcShapeType GetType() const;

public_internal:

	// Gets the AABB for the shape given a local to world transform.
	//
	virtual void GetWorldBounds( const pxTransform& xform, pxAABB &outBounds ) const = 0;

	// Gets the AABB for the shape given a local to world transform and an extra tolerance.
	//
	inline void GetWorldBounds( const pxTransform& xform, pxReal tolerance, pxAABB &outBounds ) const
	{
		pxAABB boundsWithoutMargin;
		this->GetWorldBounds( xform, boundsWithoutMargin );

		const pxVec3 margin( tolerance, tolerance, tolerance );
		outBounds.mMin = boundsWithoutMargin.mMin - margin;
		outBounds.mMax = boundsWithoutMargin.mMax + margin;
	};

	// Finds the closest intersection between the shape and a ray defined in the shape's local space, starting at fromLocal, ending at toLocal.
	//
	virtual bool CastRay( const pxShapeRayCastInput& input, pxShapeRayCastOutput &output ) const;

	// Finds the closest intersection between the shape and a ray defined in the shape's local space, starting at fromLocal, ending at toLocal.
	// This is a callback driven raycast. For each hit found, the pxRayHitCollector receives a callback with the hit info.
	//
	virtual void CastRay( const pxShapeRayCastInput& input, pxRayHitCollector &collector ) const;


	virtual void TraceBox( ShapePMTraceInput& input, ShapePMTraceOutput &output ) const;


	// GetMaximumProjection()
	// Support for creating bounding volume hierarchies of shapes.
	// This function returns the maximal extent of a shape along a given direction. 
	// It is for the purposes of creating bounding volumes around the shape ( mid-phase ) rather than exact collision
	// detection (narrow-phase).
	// The default implementation of this function uses the AABB of the shape. For custom shapes, you can get a better fit.
	//
	virtual pxReal GetEffectiveRadius( const pxVec3& direction ) const;

	// Calculates inertia tensor in body space. Since inertia tensor is diagonal in the shape's local space it can be stored in a 3D vector.
	//
	virtual void CalculateInertiaLocal( pxReal mass, pxVec3 &inertia ) const;


	//virtual bool IsPlaceable() const {return true;}

public_internal:
	virtual void Serialize( mxArchive& archive ) {Unimplemented;};

	virtual void DestroySelf()
	{delete this;}

	static pxShape* Static_GetPointerByHandle( pxShape::Handle handle );

	template< class SHAPE >	// where SHAPE : pxShape
	FORCEINLINE SHAPE* UpCast()
	{
	#if PX_DEBUG
		return static_cast< SHAPE* >( this );
	#else
		return checked_cast< SHAPE* >( this );
	#endif // PX_DEBUG
	}


protected:
	pxShape( pxcShapeType type = PX_SHAPE_UNKNOWN );
	virtual	~pxShape();

public_internal:
	/*const*/ TEnum< pxcShapeType, pxU4 >	mType;

	void *	mUserData;	// user-manageable pointer

private:
	NO_ASSIGNMENT(pxShape);
	NO_COMPARES(pxShape);
	NO_COPY_CONSTRUCTOR(pxShape);
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PX_INLINE
pxcShapeType pxShape::GetType() const {
	return mType;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
pxShape* pxUtil_CreateShape( const pxcShapeType eShapeType );


#endif // !__PX_COLLISION_SHAPE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
