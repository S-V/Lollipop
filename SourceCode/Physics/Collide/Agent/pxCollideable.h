/*
=============================================================================
	File:	pxCollideable.h
	Desc:	basic primitive used for collision detection
=============================================================================
*/

#ifndef __PX_COLLIDEABLE_H__
#define __PX_COLLIDEABLE_H__

#include <Physics/Base/pxTransform.h>
#include <Physics/Base/pxMaterial.h>
#include <Physics/Collide/Shape/pxShape.h>

#if MX_EDITOR

#include <Core/Editor/EditableProperties.h>

#endif // MX_EDITOR


class pxWorld;

// handle to broadphase proxy used for quick and coarse collision detection
typedef pxU4 pxBroadphaseProxy;




//
//	pxCollideableInfo
//
struct pxCollideableInfo
{
	pxShape::Handle shape;	// collision shape

	pxMaterial::Handle	material;	// material parameters

public:
	pxCollideableInfo()
	{
		setDefaults();
	}
	void setDefaults()
	{
		shape.SetInvalid();
		material = pxMaterial::DefaultId;
	}
	bool isOk() const
	{
		return 1
			&& CHK(shape.IsValid())
			;
	}
};

//
//	pxcBodyFlags
//
enum pxcBodyFlags
{
	PX_STATIC_OBJECT	= BIT(0),	// is the object static or movable?
//	PX_FANTOM_OBJECT	= BIT(1),	// is it a ghost object?
//	PX_CHARACTER_OBJECT	= BIT(2),	// is it a character controller?
};
typedef TBits<
	pxcBodyFlags,
	pxU4
> pxBodyFlags;

/*
-----------------------------------------------------------------------------
	pxCollideable

	this should a POD type (and as small as possible)
-----------------------------------------------------------------------------
*/
MX_ALIGN_16(struct) pxCollideable
{
	//@todo: collision object should only reference the world transform
	pxTransform			m_transform;	//«64 local-to-world transform

	//could also store a 16-byte aligned pointer and keep shape's type in lowest 4 bits
	pxShape::Handle		m_collisionShape;	//«4 link to collision model

	//pxReal	m_ccdHitFraction;	//«4 fraction of movement completed before impact

	//pxWorldEntityInfo	m_self;	//«4 info about itself and containing world

	pxBroadphaseProxy	m_broadphaseProxy;	//«4 link to broadphase collision proxy

	pxMaterial::Handle	m_material;	//« physics material

public:
	typedef pxU4 Handle;
	enum { NullHandle = -1 };

public:
	void SetOrigin( const pxVec3& newOrigin );

	// Returns the model (local) to world transformation.
	const pxTransform& GetTransform() const;

	// Gets the center of mass of the rigid body in world space.
	const pxVec3& GetCenterOfMassPosition() const;

	//this function is provided for convenience
	const pxVec3& GetPosition() const { return GetCenterOfMassPosition(); }
	//this function is provided for convenience
	const pxVec3& GetOrigin() const { return GetCenterOfMassPosition(); }


	// Sets the local-to-world transformation.
	void SetTransform( const pxTransform& newTransform );

	// Computes world-space bounds from collision shape and local-to-world transform.
	void GetWorldBounds( pxAABB & bounds ) const;

	pxShape* GetShape();
	const pxShape* GetShape() const;
	void SetShape( pxShape::Handle newShape );

	void SetMaterial( pxMaterial::Handle newMaterial );
	pxMaterial::Handle getMaterialId() const;

public_internal:
	PX_DECLARE_POD_ALLOCATOR( pxCollideable, PX_MEMORY_COLLISION );

	PX_INLINE pxCollideable() {}
	PX_INLINE ~pxCollideable() {}

	void _ctor( const pxCollideableInfo& desc );
	void _dtor();

#if MX_EDITOR

public:
	void edCreateProperties( EdPropertyCollector & outProperties );

	MX_IMPLEMENT_COMMON_PROPERTIES(pxCollideable);

#endif // MX_EDITOR

};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PX_INLINE
void pxCollideable::SetOrigin( const pxVec3& newOrigin ) {
	m_transform.SetOrigin( newOrigin );
}

PX_INLINE
const pxTransform& pxCollideable::GetTransform() const
{
	return m_transform;
}
PX_INLINE
const pxVec3& pxCollideable::GetCenterOfMassPosition() const
{
	return m_transform.GetOrigin();
}

PX_INLINE
void pxCollideable::SetTransform( const pxTransform& newTransform ) {
	m_transform = newTransform;
}

PX_INLINE
void pxCollideable::GetWorldBounds( pxAABB & bounds ) const {
	m_collisionShape.ToPtr()->GetWorldBounds( m_transform, bounds );
}

#endif // !__PX_COLLIDEABLE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
