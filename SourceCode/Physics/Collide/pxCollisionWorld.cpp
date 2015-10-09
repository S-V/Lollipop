/*
=============================================================================
	File:	pxCollisionWorld.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

/*
-----------------------------------------------------------------------------
	pxCollisionWorldDesc
-----------------------------------------------------------------------------
*/
void pxCollisionWorldDesc::setDefaults()
{
	broadphase = nil;
	dispatcher = Physics::GetCollisionDispatcher();
}

/*================================
		pxCollisionWorld
================================*/

pxCollisionWorld::pxCollisionWorld( const pxCollisionWorldDesc& desc )
{
	Assert(desc.isOk());
	mBroadphase = desc.broadphase;
	mDispatcher = desc.dispatcher;
}

pxCollisionWorld::~pxCollisionWorld() {
}

void pxCollisionWorld::AddCollisionObject( pxCollideable* object )
{
	AssertPtr(object);
	{
	//	pxBroadphaseHandle * handle = mBroadphase->Add( object );
	//	object->m_broadphaseHandle = handle;
	}
}

void pxCollisionWorld::RemoveCollisionObject( pxCollideable* object )
{
	AssertPtr(object);
	{
	//	pxBroadphaseHandle * handle = object->m_broadphaseHandle;
	//	AssertPtr(handle);
	//	mBroadphase->Remove( handle );
	//	object->m_broadphaseHandle = nil;
	}
}

void pxCollisionWorld::Collide( pxContactCache &cache )
{
	PX_PROFILE("Collision detection");
	{
		PX_PROFILE("Broadphase collision detection");
		PX_SCOPED_COUNTER(gPhysStats.broadphaseMs);
		mBroadphase->Collide( *mDispatcher );
	}
	{
		PX_SCOPED_COUNTER(gPhysStats.narrowphaseMs);
		PX_PROFILE("Nearphase collision detection");
		mDispatcher->Collide( cache );
	}
}

void pxCollisionWorld::Clear()
{
	mBroadphase->Clear();
	mDispatcher->Clear();
}

pxCollideable * pxCollisionWorld::CastRay( const pxVec3& origin, const pxVec3& direction, pxReal &fraction )
{
	Assert(direction.IsNormalized());
	Unimplemented;
/*	const pxVec3 endPoint = origin + direction * MAX_WORLD_SIZE;

	pxReal minFraction = 1.0f;
	pxVec3 normal;

	pxCollideable * hit = nil;

	for( UINT i=0; i<mObjects.Num(); i++ )
	{
		pxCollideable * obj = mObjects[ i ];
		const pxAABB& bounds = obj->GetWorldBounds();
		pxReal frac;
		if( pxRayAabb( origin, endPoint, bounds.GetMin(), bounds.GetMax(), frac, normal ) )
		{
			if( frac < minFraction ) {
				hit = obj;
				minFraction = frac;
			}
		}
	}
	if( minFraction < 1.0f ) {
		fraction = minFraction;
		return hit;
	}
	return nil;
	*/
	return nil;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
