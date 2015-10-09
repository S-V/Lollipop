/*
=============================================================================
	File:	collide_box_box.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

MX_SWIPED("Bullet");
#if 0
struct CollisionDetectorOutput
{
	pxTransform m_rootTransA, m_rootTransB;
	pxCollisionResult & mResult;

	CollisionDetectorOutput( pxCollideable* objA, pxCollideable* objB, pxCollisionResult& result )
		: mResult(result)
	{
		m_rootTransA = objA->GetTransform();
		m_rootTransB = objB->GetTransform();
		mResult.manifold.numPoints = 0;
	}
	virtual ~CollisionDetectorOutput(){}	
	///setShapeIdentifiersA/B provides experimental support for per-triangle material / custom material combiner
	void setShapeIdentifiersA(int partId0,int index0){UNDONE;};
	void setShapeIdentifiersB(int partId1,int index1){UNDONE;};
	void addContactPoint(const pxVec3& normalOnBInWorld,const pxVec3& pointInWorld,pxReal depth)
	{

		pxVec3 pointA = pointInWorld + normalOnBInWorld * depth;

		pxVec3 localA;
		pxVec3 localB;

		if (0)//isSwapped)
		{
			localA = m_rootTransB.invXform(pointA );
			localB = m_rootTransA.invXform(pointInWorld);
		} else
		{
			localA = m_rootTransA.invXform(pointA );
			localB = m_rootTransB.invXform(pointInWorld);
		}

		if( depth > PX_EPSILON )
		{
			mResult.manifold.AddPoint( pointInWorld, normalOnBInWorld, depth );

			//	printf("depth=%f\n",depth);
			//pxDebugDrawer::Get().drawContactPoint( pointInWorld, normalOnBInWorld,depth, 0, pxVec3(1,0,0) );
		}
	}

	
};

#include "BoxBoxDetector.inl"

#endif

#if 0
pxUInt FCollide_Box_Box( pxCollideable* objA, pxCollideable* objB, pxCollisionResult &result )
{
	//PX_ASSERT_PTR2(objA,objB);
	//Assert( objA->GetShape()->GetType() == pxcShapeType::PX_SHAPE_BOX );
	//Assert( objB->GetShape()->GetType() == pxcShapeType::PX_SHAPE_BOX );

	pxShape_Box * boxA = (pxShape_Box*) objA->GetShape();
	pxShape_Box * boxB = (pxShape_Box*) objB->GetShape();

	const pxTransform& transformA = objA->GetTransform();
	const pxTransform& transformB = objA->GetTransform();
	
	int skip = 0;
	dContactGeom *contact = 0;

	dMatrix3 R1;
	dMatrix3 R2;

	for (int j=0;j<3;j++)
	{
		R1[0+4*j] = transformA.GetBasis()[j].getX();
		R2[0+4*j] = transformB.GetBasis()[j].getX();

		R1[1+4*j] = transformA.GetBasis()[j].getY();
		R2[1+4*j] = transformB.GetBasis()[j].getY();

		R1[2+4*j] = transformA.GetBasis()[j].getZ();
		R2[2+4*j] = transformB.GetBasis()[j].getZ();
	}


	CollisionDetectorOutput	output(objA,objB,result);

	pxVec3 normal;
	pxReal depth;
	int return_code;
	int maxc = 4;

	const pxVec3 halfSizeWithMarginA = boxA->mHalfSize;
	const pxVec3 halfSizeWithMarginB = boxB->mHalfSize;

	/*int num = */dBoxBox2 (
		transformA.GetOrigin(), 
		R1,
		2.f*halfSizeWithMarginA,
		transformB.GetOrigin(),
		R2, 
		2.f*halfSizeWithMarginB,
		normal, &depth, &return_code,
		maxc, contact, skip,
		output
	);

	pxContactManifold & manifold = result.manifold;

	pxContactPointMaterial	contactMaterial;
	contactMaterial.Set( objA->GetSurfaceInfo(), objB->GetSurfaceInfo() );

	for( int i = 0; i<manifold.numPoints; i++ )
	{
		manifold.points[i].material = contactMaterial;
	}

	result.oA = objA;
	result.oB = objB;

	return manifold.numPoints;
}

#endif
NO_EMPTY_FILE
//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
