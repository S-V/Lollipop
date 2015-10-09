/*
=============================================================================
	File:	ViewFrustum.h
	Desc:	
=============================================================================
*/

#ifndef __VIEW_FRUSTUM_H__
#define __VIEW_FRUSTUM_H__

mxNAMESPACE_BEGIN


//
//	EViewFrustumPlane
//
enum EViewFrustumPlane
{
	VF_NEAR_PLANE = 0,

	VF_LEFT_PLANE,
	VF_RIGHT_PLANE,

	VF_BOTTOM_PLANE,
	VF_TOP_PLANE,

	VF_FAR_PLANE,	mxOPTIMIZE("<- testing against the far clipping plane can be skipped in huge environments")

	VF_NUM_PLANES,

	// number of planes used for culling
	VF_CLIP_PLANES = VF_NUM_PLANES
};
mxOPTIMIZE("use SSE for frustum culling")


//
//	ViewFrustum
//
struct ViewFrustum
{
	// Empty constructor leaves data uninitialized!
	ViewFrustum();

	// Builds frustum planes from the given matrix.
	ViewFrustum( const Matrix4& mat );

	FASTBOOL			PointInFrustum( const Vec3D& point ) const;
	FASTBOOL			IntersectSphere( const Sphere& sphere ) const;
	FASTBOOL			IntersectsAABB( const AABB& aabb ) const;

	// returns ESpatialRelation (natural int for speed)
	int					Classify( const AABB& aabb ) const;

	//
	// Builds frustum planes from the given matrix.
	//
	//   If the given matrix is equal to the camera projection matrix
	// then the resulting planes will be given in view space ( i.e., camera space ).
	//
	//   If the matrix is equal to the combined view and projection matrices,
	// then the resulting planes will be given in world space.
	//
	//   If the matrix is equal to the combined world, view and projection matrices,
	// then the resulting planes will be given in object space.
	//
	// NOTE: Frustum planes' normals point towards the inside of the frustum.
	//
	void		ExtractFrustumPlanes( const Matrix4& mat );

	bool		GetFarLeftDown( Vec3D & point ) const;
	bool		GetFarLeftUp( Vec3D & point ) const;
	bool		GetFarRightUp( Vec3D & point ) const;
	bool		GetFarRightDown( Vec3D & point ) const;

	// Computes positions of the 8 vertices of the frustum.
	// [0] - far top right corner
	// [1] - near top right corner
	// [2] - far bottom right corner
	// [3] - near bottom right corner
	// [4] - far top left corner
	// [5] - near top left corner
	// [6] - far bottom left corner
	// [7] - near bottom left corner
	// NOTE: this isn't a cheap function to call.
	//
	void		CalculateCornerPoints( Vec3D corners[8] ) const;

	// Returns 1 if all points are outside the frustum.
	//
	FASTBOOL	CullPoints( const Vec3D* points, UINT numPoints ) const;

public_internal:
	Plane3D		planes[ VF_NUM_PLANES ];	// actual frustum planes (the normals point inwards).
	UINT		signs[ VF_NUM_PLANES ];		// signs of planes' normals
};

mxNAMESPACE_END

#endif // ! __VIEW_FRUSTUM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
