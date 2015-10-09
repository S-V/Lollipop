/*
=============================================================================
	File:	pxObjects.h
	Desc:
=============================================================================
*/

#ifndef __PX_OBJECTS_H__
#define __PX_OBJECTS_H__

// enumerates all supported types of physics objects
enum pxcObjectType
{

#define PX_DECLARE_OBJECT_TYPE(x) x
#	include "pxObjects.inl"
#undef PX_DECLARE_OBJECT_TYPE

	PX_OBJECT_TYPE_MAX
};

typedef TEnum<
	pxcObjectType,
	pxU4
> pxObjectType;

const char* pxGetObjectName( pxcObjectType type );


// Forward declarations.
class pxCollideable;
class pxCollisionAgent;
class pxBroadphase;
class pxBroadphaseHandle;
class pxCollisionDispatcher;
class pxRigidBody;
class pxWorld;
class pxContactCache;
class pxContactPoint;
class pxContactManifold;

#endif // !__PX_OBJECTS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
