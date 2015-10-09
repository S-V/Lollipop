/*
=============================================================================
	File:	Mass.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

bool pxUtil_MassPropertiesComputer::ComputeSphereVolumeMassProperties( pxReal radius, pxReal mass, pxMassProperties &result )
{
	if( !CHK( radius > 0.0f ) ) {
		return false;
	}
	if( !CHK( mass > 0.0f ) ) {
		return false;
	}

	result.centerOfMass.SetZero();

	// volume = (4/3) * MX_PI * (radius^3)
	result.volume = pxReal(4.0/3.0) * MX_PI * cubef( radius );

	result.mass = mass;

	const pxReal radiusSq = squaref( radius );
	/*------------------------------------------------------
	| (2/5) * M * (R^2)           0           0
	|         0           (2/5) * M * (R^2)   0
	|         0                   0   (2/5) * M * (R^2)
	*/
	result.inertiaTensor.SetValue(
		pxReal(0.4)*radiusSq,	0.0f,					0.0f,
		0.0f,					pxReal(0.4)*radiusSq,	0.0f,
		0.0f,					0.0f,					pxReal(0.4)*radiusSq
	);

	return true;
}

bool pxUtil_MassPropertiesComputer::ComputeBoxVolumeMassProperties( const pxVec3& halfExtents, pxReal mass, pxMassProperties& result )
{
	if( halfExtents.getX() <= 0.0f || halfExtents.getY() <= 0.0f || halfExtents.getZ() <= 0.0f ) {
		return false;
	}
	if( mass <= 0.0f ) {
		return false;
	}

	result.centerOfMass.SetZero();

	pxReal lx = pxReal(2.) * halfExtents.getX();
	pxReal ly = pxReal(2.) * halfExtents.getY();
	pxReal lz = pxReal(2.) * halfExtents.getZ();

	// volume = length * width * height
	result.volume = lz*ly*lz;

	result.mass = mass;

	result.inertiaTensor.SetValue(
		pxReal(1./12.) * mass * (ly*ly + lz*lz),	0.0f,					0.0f,
		0.0f,					pxReal(1./12.) * mass * (lx*lx + lz*lz),	0.0f,
		0.0f,					0.0f,					pxReal(1./12.) * mass * (lx*lx + ly*ly)
	);

	return true;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
