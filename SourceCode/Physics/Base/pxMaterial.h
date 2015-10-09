/*
=============================================================================
	File:	Material.h
	Desc:	Physics material.
=============================================================================
*/

#ifndef __PX_MATERIAL_H__
#define __PX_MATERIAL_H__

FORCEINLINE bool pxIsValidFriction( pxReal mu ) { return IsInRangeInc( mu, 0.0f, 1.0f ); }
FORCEINLINE bool pxIsValidRestitution( pxReal e ) { return IsInRangeInc( e, 0.0f, 1.0f ); }

//
//	pxMaterialDesc
//
class pxMaterialDesc
{
public:
	pxReal		density;	// mass properties

	// friction with contact surfaces
	pxReal		staticFriction;
	pxReal		dynamicFriction;	// aka 'kinetic friction'

	pxReal		restitution;	// coefficient of restitution (aka 'bouncyness', 'impact coefficient')
	pxReal		elasticity;	// only valid for deformable objects
	pxReal		roughness;	// used for perturbing contact normals
	pxReal		adhesion;

public:
	pxMaterialDesc()
	{
		setDefaults();
	}
	void setDefaults()
	{
		staticFriction = 0.6f;
		dynamicFriction = 0.5f;
		
		restitution = 0.0f;
		elasticity = 0.0f;
		roughness = 0.0f;
		density = 1.0f;
	}
	bool isOk() const
	{
		return 1
			&& CHK(pxIsValidFriction( staticFriction ))
			&& CHK(pxIsValidFriction( dynamicFriction ))
			
			&& CHK(pxIsValidRestitution( restitution ))

		//	&& CHK(elasticity >= 0.0f && elasticity <= 1.0f)

			&& CHK(density > 0.0f)
			;
	}
};

//------------------------------------------------------------

// Friction mixing law. Feel free to customize this.
PX_INLINE
pxReal pxCombineFriction( pxReal friction1, pxReal friction2 )
{
	const pxReal mu = 0.5f * (friction1 + friction2);
	return mu;
}

// Restitution mixing law. Feel free to customize this.
PX_INLINE
pxReal pxCombineRestitution( pxReal restitution1, pxReal restitution2 )
{
//	return 0.5f * (restitution1 + restitution2);
	return (restitution1 * restitution2);
}

//------------------------------------------------------------

//
//	pxMaterial - physics material internal class.
//
struct pxMaterial
{
	pxReal		friction;
	pxReal		restitution;
	void *		userData;

public:
	pxMaterial();

	typedef U2 Handle;
	enum { MaxCount = MAX_UINT16-1 };
	enum { DefaultId = 0 };

	static pxMaterial* Static_GetPointerByHandle( Handle handle );
};

MX_DECLARE_POD_TYPE( pxMaterial );

#endif // !__PX_MATERIAL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
