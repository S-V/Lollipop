/*
=============================================================================
	File:	pxCollisionWorld.h
	Desc:	
=============================================================================
*/

#ifndef __PX_COLLISION_WORLD_H__
#define __PX_COLLISION_WORLD_H__

//
//	pxCollisionWorldDesc
//
struct pxCollisionWorldDesc
{
	pxBroadphase *			broadphase;
	pxCollisionDispatcher *	dispatcher;

public:
	pxCollisionWorldDesc()
	{
		setDefaults();
	}
	void setDefaults();
	bool isOk() const
	{
		return 1
			&& CHK(broadphase)
			&& CHK(dispatcher)
			;
	}
};

//
//	pxCollisionWorld
//
class pxCollisionWorld {
public:
			pxCollisionWorld( const pxCollisionWorldDesc& desc );
	virtual	~pxCollisionWorld();

	void AddCollisionObject( pxCollideable* object );
	void RemoveCollisionObject( pxCollideable* object );

	void Collide( pxContactCache &cache );

	void Clear();

	pxCollideable * CastRay( const pxVec3& origin, const pxVec3& direction, pxReal &fraction );

	bool isOk() const
	{
		return (mBroadphase != nil)
			&& (mDispatcher != nil)
			;
	}

private: PREVENT_COPY(pxCollisionWorld);
private:
	TPtr< pxBroadphase >			mBroadphase;
	TPtr< pxCollisionDispatcher >	mDispatcher;
};

#endif // !__PX_COLLISION_WORLD_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
