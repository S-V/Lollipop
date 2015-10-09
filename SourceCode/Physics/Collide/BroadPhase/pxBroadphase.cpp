/*
=============================================================================
	File:	pxBroadphase.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

TraceBoxOutput::TraceBoxOutput()
{
	hitPosition.SetZero();
	hitNormal = Vec3D::vec3_unit_y;
	hitFraction = 1.0f;
}

/*================================
		pxBroadphaseDesc
================================*/

void pxBroadphaseDesc::setDefaults()
{
	const pxVec3 defaultExtent(REAL(1000.0));
	worldBounds.Set( -defaultExtent, defaultExtent );

	maxObjects = 1024;
	maxPairs = 1024;
}
bool pxBroadphaseDesc::isOk() const
{
	return 1
		&& CHK(worldBounds.GetSize().Length() <= PX_MAX_BROADPHASE_SIZE)

		&& CHK(IsPowerOfTwo(maxObjects))
		&& CHK(maxObjects < PX_MAX_BROADPHASE_PROXIES)

		&& CHK(IsPowerOfTwo(maxPairs))
		&& CHK(maxPairs < PX_MAX_BROADPHASE_PAIRS)
		;
}

/*================================
		pxBroadphase
================================*/

pxBroadphase::pxBroadphase()
{
}

//pxBroadphase::pxBroadphase( const pxBroadphaseDesc& desc )
//{
//	Assert(desc.isOk());
//}

pxBroadphase::~pxBroadphase()
{
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
