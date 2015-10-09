/*
=============================================================================
	File:	collide_sphere_box.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

MX_SWIPED("Bullet");
#if 0
MX_BUG("when sphere center is inside the box");

pxReal getSpherePenetration(
	pxCollideable* boxObj,
	pxVec3& pointOnBox,
	pxVec3& v3PointOnSphere,
	const pxVec3& sphereCenter,
	pxReal fRadius,
	const pxVec3& aabbMin,
	const pxVec3& aabbMax
	) 
{

	pxVec3 bounds[2];

	bounds[0] = aabbMin;
	bounds[1] = aabbMax;

	pxVec3	p0, tmp, prel, n[6], normal;
	pxReal   fSep = pxReal(-10000000.0), fSepThis;

	// set p0 and normal to a default value to shup up GCC
	p0.SetValue(pxReal(0.), pxReal(0.), pxReal(0.));
	normal.SetValue(pxReal(0.), pxReal(0.), pxReal(0.));

	n[0].SetValue( pxReal(-1.0),  pxReal(0.0),  pxReal(0.0) );
	n[1].SetValue(  pxReal(0.0), pxReal(-1.0),  pxReal(0.0) );
	n[2].SetValue(  pxReal(0.0),  pxReal(0.0), pxReal(-1.0) );
	n[3].SetValue(  pxReal(1.0),  pxReal(0.0),  pxReal(0.0) );
	n[4].SetValue(  pxReal(0.0),  pxReal(1.0),  pxReal(0.0) );
	n[5].SetValue(  pxReal(0.0),  pxReal(0.0),  pxReal(1.0) );

	const pxTransform&	m44T = boxObj->GetTransform();

	// convert  point in local space
	prel = m44T.invXform( sphereCenter);

	///////////

	for (int i=0;i<6;i++)
	{
		int j = i<3 ? 0:1;
		if ( (fSepThis = ((prel-bounds[j]) .Dot( n[i]))-fRadius) > pxReal(0.0) )	return pxReal(1.0);
		if ( fSepThis > fSep )
		{
			p0 = bounds[j];	normal = (pxVec3&)n[i];
			fSep = fSepThis;
		}
	}

	pointOnBox = prel - normal*(normal.Dot((prel-p0)));
	v3PointOnSphere = pointOnBox + normal*fSep;

	// transform back in world space
	tmp  = m44T( pointOnBox);		
	pointOnBox    = tmp;
	tmp  = m44T( v3PointOnSphere);		v3PointOnSphere = tmp;
	normal = (pointOnBox-v3PointOnSphere).Normalize();

	return fSep;

}

pxReal getSphereDistance(
	pxCollideable* boxObj,
	pxVec3& pointOnBox,
	pxVec3& v3PointOnSphere,
	const pxVec3& sphereCenter,
	pxReal fRadius
	) 
{
	pxReal margins;
	pxVec3 bounds[2];
	pxShape_Box* boxShape= (pxShape_Box*)boxObj->GetShape();
	
	bounds[0] = -boxShape->GetHalfSize();
	bounds[1] = boxShape->GetHalfSize();

	margins = PX_EPSILON;//boxShape->getMargin();//also add sphereShape margin?

	const pxTransform&	m44T = boxObj->GetTransform();

	pxVec3	boundsVec[2];
	pxReal	fPenetration;

	boundsVec[0] = bounds[0];
	boundsVec[1] = bounds[1];

	pxVec3	marginsVec( margins, margins, margins );

	// add margins
	bounds[0] += marginsVec;
	bounds[1] -= marginsVec;

	/////////////////////////////////////////////////

	pxVec3	tmp, prel, n[6], normal, v3P;
	pxReal   fSep = pxReal(10000000.0), fSepThis;

	n[0].SetValue( pxReal(-1.0),  pxReal(0.0),  pxReal(0.0) );
	n[1].SetValue(  pxReal(0.0), pxReal(-1.0),  pxReal(0.0) );
	n[2].SetValue(  pxReal(0.0),  pxReal(0.0), pxReal(-1.0) );
	n[3].SetValue(  pxReal(1.0),  pxReal(0.0),  pxReal(0.0) );
	n[4].SetValue(  pxReal(0.0),  pxReal(1.0),  pxReal(0.0) );
	n[5].SetValue(  pxReal(0.0),  pxReal(0.0),  pxReal(1.0) );

	// convert  point in local space
	prel = m44T.invXform( sphereCenter);
	
	bool	bFound = false;

	v3P = prel;

	for (int i=0;i<6;i++)
	{
		int j = i<3? 0:1;
		if ( (fSepThis = ((v3P-bounds[j]) .Dot(n[i]))) > pxReal(0.0) )
		{
			v3P = v3P - n[i]*fSepThis;		
			bFound = true;
		}
	}
	
	//

	if ( bFound )
	{
		bounds[0] = boundsVec[0];
		bounds[1] = boundsVec[1];

		normal = (prel - v3P).Normalize();
		pointOnBox = v3P + normal*margins;
		v3PointOnSphere = prel - normal*fRadius;

		if ( ((v3PointOnSphere - pointOnBox) .Dot (normal)) > pxReal(0.0) )
		{
			return pxReal(1.0);
		}

		// transform back in world space
		tmp = m44T( pointOnBox);
		pointOnBox    = tmp;
		tmp  = m44T( v3PointOnSphere);		
		v3PointOnSphere = tmp;
		pxReal fSeps2 = (pointOnBox-v3PointOnSphere).LengthSqr();
		
		//if this fails, fallback into deeper penetration case, below
		if (fSeps2 > PX_EPSILON)
		{
			fSep = - mxSqrt(fSeps2);
			normal = (pointOnBox-v3PointOnSphere);
			normal *= pxReal(1.)/fSep;
		}

		return fSep;
	}

	//////////////////////////////////////////////////
	// Deep penetration case

	fPenetration = getSpherePenetration( boxObj,pointOnBox, v3PointOnSphere, sphereCenter, fRadius,bounds[0],bounds[1] );

	bounds[0] = boundsVec[0];
	bounds[1] = boundsVec[1];

	if ( fPenetration <= pxReal(0.0) )
		return (fPenetration-margins);
	else
		return pxReal(1.0);
}



pxUInt FCollide_Sphere_Box(
	pxCollideable* sphereObject,
	pxCollideable* boxObject,
	pxCollisionResult &result )
{
	//PX_ASSERT_PTR2(sphereObject,boxObject);
	//Assert( sphereObject->GetShape()->GetType() == pxcShapeType::PX_SHAPE_SPHERE );
	//Assert( boxObject->GetShape()->GetType() == pxcShapeType::PX_SHAPE_BOX );

	pxShape_Sphere *	sphere	= (pxShape_Sphere*)	sphereObject->GetShape();
//	pxShape_Box *		box		= (pxShape_Box*)	boxObject->GetShape();
	
	const pxReal margin = PX_COLLISION_MARGIN;

	pxVec3 pOnBox,pOnSphere;
	pxVec3 sphereCenter = sphereObject->GetTransform().GetOrigin();
	pxReal radius = sphere->GetRadius() + margin;

	pxReal dist = getSphereDistance(boxObject,pOnBox,pOnSphere,sphereCenter,radius);

	if (dist > PX_EPSILON) {return 0;}

	pxVec3 normalOnSurfaceB = (pOnBox - pOnSphere).Normalize();

	// report a contact. internally this will be kept persistent, and contact reduction is done

	pxContactManifold & manifold = result.manifold;
	pxContact & contact = manifold.points[0];

	//contact.position = pOnSphere - normalOnSurfaceB*dist;
	contact.position = pOnBox + normalOnSurfaceB*dist;
	
	contact.normal = normalOnSurfaceB;
	contact.depth = dist;

	contact.material.Set( sphereObject->GetSurfaceInfo(), boxObject->GetSurfaceInfo() );

	manifold.numPoints = 1;

	result.oA = sphereObject;
	result.oB = boxObject;

	return 1;
}
#endif
NO_EMPTY_FILE
//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
