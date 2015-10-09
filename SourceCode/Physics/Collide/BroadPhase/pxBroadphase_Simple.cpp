/*
=============================================================================
	File:	pxBroadphase_Simple.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

pxBroadphase_Simple::pxBroadphase_Simple()
{
	mHandles.Reserve( 64 );
	this->_ClearHandles();
}

pxBroadphase_Simple::~pxBroadphase_Simple()
{
}

void pxBroadphase_Simple::Add( pxCollideable* object )
{
	AssertPtr(object);

	const UINT newHandleIndex = this->_AllocHandle();

	pxSimpleBroadphaseProxy* proxy = &(mHandles[ newHandleIndex ]);
	{
		proxy->o = object;
		pxSimpleBoundsFromCollideable( object, proxy->bounds );
	}

	object->m_broadphaseProxy = newHandleIndex;
}

void pxBroadphase_Simple::Remove( pxCollideable* object )
{
	AssertPtr(object);
UNDONE;


	/*const Handle bpHandle = object->m_broadphaseHandle;

	pxSimpleBroadphaseProxy* proxy0 = &(mHandles[ bpHandle ]);
	_FreeHandle( proxy0 );

	object->m_broadphaseHandle = INDEX_NONE;*/
}

pxUInt pxBroadphase_Simple::GetNumObjects() const
{
	return this->GetNumHandles();
}

pxUInt pxBroadphase_Simple::GetMaxObjects() const
{
	return this->GetMaxHandles();
}

void pxBroadphase_Simple::Clear()
{
	this->_ClearHandles();
}

void pxBroadphase_Simple::_ClearHandles()
{
	mFirstFreeHandle = 0;

	const UINT maxHandles = this->GetMaxHandles();

	for( UINT i = mFirstFreeHandle; i < maxHandles; i++ )
	{
		(mHandles.ToPtr() + i )->nextFree = i + 1;
	}
	mHandles.ToPtr()[ maxHandles - 1 ].nextFree = 0;
}

void pxBroadphase_Simple::Collide( pxCollisionDispatcher & handler )
{
	const UINT totalNumObjects = this->GetNumHandles();

	//update AABBs
	MX_OPTIMIZE("this should only be done when objects move");
	for( UINT i=0; i < totalNumObjects; i++ )
	{
		pxSimpleBroadphaseProxy & proxy = mHandles[ i ];

		pxSimpleBoundsFromCollideable( proxy.o, proxy.bounds );
	}

	for( UINT i=0; i < totalNumObjects; i++ )
	{
		pxSimpleBroadphaseProxy& objA = mHandles[ i ];

		for( int j=i+1; j < totalNumObjects; j++ )
		{
			pxSimpleBroadphaseProxy& objB = mHandles[ j ];

			//if( objA->o->GetInternalFlags() & pxcBodyFlags::PX_STATIC_OBJECT
			//	&&
			//	objB->o->GetInternalFlags() & pxcBodyFlags::PX_STATIC_OBJECT )
			//{
			//	continue;
			//}

			if( pxSimpleBoundsOverlap( objA.bounds, objB.bounds ) )
			{
				handler.AddPair( objA.o, objB.o );
			}
			else
			{
				handler.RemovePair( objA.o, objB.o );
			}
		}
	}
}

void pxBroadphase_Simple::CastRay( const WorldRayCastInput& input, WorldRayCastOutput &output )
{
	pxShapeRayCastInput	shapeRayCastInput;
	shapeRayCastInput.m_from = pxVec3::From_Vec3D( input.origin );
	shapeRayCastInput.m_to = shapeRayCastInput.m_from + pxVec3::From_Vec3D( input.direction ) * PX_BIG_NUMBER;

	pxShapeRayCastOutput	shapeRayCastOutput;
	TPtr< pxCollideable >	hitObject;

	for( UINT i=0; i < mHandles.Num(); i++ )
	{
		pxSimpleBroadphaseProxy& proxy = mHandles[i];

		pxShapeRayCastOutput	tmpShapeRayCastOutput;
		if( proxy.o->GetShape()->CastRay( shapeRayCastInput, tmpShapeRayCastOutput ) )
		{
			if( tmpShapeRayCastOutput.m_hitFraction < shapeRayCastOutput.m_hitFraction )
			{
				shapeRayCastOutput = tmpShapeRayCastOutput;
				hitObject = proxy.o;
			}
		}
	}

	output.normal = shapeRayCastOutput.m_normal;
	output.hitFraction = shapeRayCastOutput.m_hitFraction;
	output.hitObject = hitObject;
}

void pxBroadphase_Simple::LinearCast( const ConvexCastInput& input, ConvexCastOutput &output )
{
	Unimplemented;
}

void pxBroadphase_Simple::TraceBox( const TraceBoxInput& input, TraceBoxOutput &output )
{
	if( input.start == input.end )
	{
		output.hitPosition = input.start;
		output.hitNormal.SetZero();
		output.hitFraction = 0.0f;
		return;
	}

	ShapePMTraceInput		shapeTraceInput;
	{
	shapeTraceInput.start = input.start;
	shapeTraceInput.end = input.end;
	shapeTraceInput.size = input.size;

	shapeTraceInput.UpdateCachedData();
	}

	F4	minHitFraction = 1.0f;
	UINT hitObjectId = INDEX_NONE;
	Vec3D	hitNormal(0);

	for( UINT i=0; i < mHandles.Num(); i++ )
	{
		pxSimpleBroadphaseProxy& proxy = mHandles[i];

		if( !proxy.bounds.IntersectsBounds( shapeTraceInput.fullTraceBounds ) )
		{
			continue;
		}

		ShapePMTraceOutput	shapeTraceOutput;
		proxy.o->GetShape()->TraceBox( shapeTraceInput, shapeTraceOutput );

		if( shapeTraceOutput.fraction < minHitFraction )
		{
			minHitFraction = shapeTraceOutput.fraction;
			hitObjectId = i;
			hitNormal = shapeTraceOutput.normal;
		}
	}
//DBGOUT("fraction = %f\n",minHitFraction);
	output.hitPosition = input.start + (input.end - input.start) * minHitFraction;
	output.hitNormal = hitNormal;
	output.hitFraction = minHitFraction;
}

void pxBroadphase_Simple::validate()
{
	const UINT numHandles = this->GetNumHandles();
	for( UINT i=1; i < numHandles-1; i++ )
	{
		for( UINT j=i; j < numHandles; j++ )
		{
			Assert( mHandles[ i ].nextFree != mHandles[ j ].nextFree );
		}
	}
}

pxUInt pxBroadphase_Simple::_AllocHandle()
{
	const UINT oldNumHandles = this->GetNumHandles();
	Assert( oldNumHandles <= this->GetMaxHandles());

	const UINT freeHandle = mFirstFreeHandle;

	mFirstFreeHandle = mHandles.ToPtr()[ freeHandle ].nextFree;

	mHandles.SetNum( oldNumHandles + 1 );

	return freeHandle;
}

void pxBroadphase_Simple::_FreeHandle( pxUInt handle )
{
	Assert(handle >= 0 && handle < this->GetMaxHandles());

	pxSimpleBroadphaseProxy& proxy = mHandles[ handle ];

	proxy.o = nil;
	proxy.nextFree = mFirstFreeHandle;

	mFirstFreeHandle = handle;

	UINT numHandles = this->GetNumHandles();
	numHandles--;
	mHandles.SetNum_Unsafe( numHandles );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
