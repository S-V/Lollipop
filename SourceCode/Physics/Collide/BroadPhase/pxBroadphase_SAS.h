/*
=============================================================================
	File:	pxBroadphase_SAS.h
	Desc:	Sort and Sweep broadphase (aka 'Sweep And Prune').
=============================================================================
*/
#ifndef __PX_BROAD_PHASE_SAS_H__
#define __PX_BROAD_PHASE_SAS_H__


class pxBroadphase_SAS
{
public:
	pxBroadphase_SAS();
	~pxBroadphase_SAS();

	// Adds an object to this broadphase.
	virtual void Add( pxCollideable* object ) override;

	// Removes an object from the broadphase.
	virtual void Remove( pxCollideable* object ) override;

	// Returns the total number of objects added to this broadphase.
	virtual pxUInt GetNumObjects() const override;

	// Returns the maximum allowed number of objects that can be added to this broadphase.
	virtual pxUInt GetMaxObjects() const override;

	// Removes all objects from the broadphase.
	virtual void Clear() override;

	// Performs a broad-phase collision detection and feeds potentially colliding pairs to the collision handler.
	virtual void Collide( pxCollisionDispatcher & handler ) override;
};

#endif // !__PX_BROAD_PHASE_SAS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
