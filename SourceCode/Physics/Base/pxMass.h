/*
=============================================================================
	File:	Mass.h
	Desc:	
=============================================================================
*/

#ifndef __PX_MASS_H__
#define __PX_MASS_H__

//
//Inertia matrix is a 3x3 matrix used to represent the momentum 
//which is required to rotate a rigid body around the fixed basis axis
//e_x, e_y, and e_z. The inertia matrix is given by
//
//     | Ixx Ixy Ixz |
// I = | Iyx Iyy Iyz |
//     | Izx Izy Izz |
// 
// which is symetric because I = I^T.
//
MX_ALIGN_16(struct) pxInertia
{
	union {
		pxReal	data[ 3 * 4 ];
		struct {
			pxReal	Ixx, Ixy, Ixz, _pad0;
			pxReal	Iyx, Iyy, Iyz, _pad1;
			pxReal	Izx, Izy, Izz, _pad2;
		};
	};

	FORCEINLINE pxInertia()
	{}

	FORCEINLINE void SetValue(
		const pxReal xx, const pxReal xy, const pxReal xz, 
		const pxReal yx, const pxReal yy, const pxReal yz, 
		const pxReal zx, const pxReal zy, const pxReal zz )
	{
		this->Ixx = xx;	this->Ixy = xy;	this->Ixz = xz;
		this->Iyx = yx;	this->Iyy = yy;	this->Iyz = yz;
		this->Izx = zx;	this->Izy = zy;	this->Izz = zz;
	}

	FORCEINLINE pxMat3x3 ToMat3x3 () const {
		return pxMat3x3(
			Ixx, Ixy, Ixz,
			Iyx, Iyy, Iyz,
			Izx, Izy, Izz
		);
	}
	FORCEINLINE operator pxMat3x3 () const {
		return ToMat3x3();
	}
};

//
// Rotates a 3x3 symmetric inertia tensor I into a space R where it can be represented with the diagonal matrix D. 
// I = R * D * R'
// Returns false on failure.
//
bool pxDiagonalizeInertiaTensor(
	const pxMat3x3& denseInertia,	// The dense inertia tensor.
	pxVec3 & diagonalInertia,		// The diagonalized inertia tensor.
	pxMat3x3 & rotation				// Rotation for the frame of the diagonalized inertia tensor. 
);

//
//	pxMass
//
struct pxMass
{
	pxVec4		com;	// center of mass and mass
	pxInertia	I;		// inertia tensor
};


//
//	pxMassProperties
//
struct pxMassProperties
{
	pxInertia	inertiaTensor;	// The inertia tensor.
	pxVec3		centerOfMass;	// The center of mass.
	pxReal		volume;			// The volume of an object.
	pxReal		mass;			// The mass of an object.
};

// This structure is used to help construct mass properties of "compound" objects.
struct pxMassElement
{
	pxTransform			transform;	// A transform for this element with respect to body local space.
	pxMassProperties	properties;	// All mass properties for this element.
};

// A helper utility to compute the inertia tensor, center of mass and volume of various classes of objects.
struct pxUtil_MassPropertiesComputer
{
	//---------------------------------------------
	//	Explicit surface types calculations
	//---------------------------------------------

	// Creates mass properties given a sphere, considered as a volume of uniform density.
	static bool ComputeSphereVolumeMassProperties( pxReal radius, pxReal mass, pxMassProperties &result );

	// Creates mass properties given box half extents, considered as a volume of uniform density.
	static bool ComputeBoxVolumeMassProperties( const pxVec3& halfExtents, pxReal mass, pxMassProperties& result );
};

#endif // !__PX_MASS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
