/*
=============================================================================
	File:	Utilities.h
	Desc:	
=============================================================================
*/

#ifndef __PX_UTILITIES_H__
#define __PX_UTILITIES_H__





MX_SWIPED("Bullet");
///The pxConvexSeparatingDistanceUtil can help speed up convex collision detection 
///by conservatively updating a cached separating distance/vector instead of re-calculating the closest distance
class	pxConvexSeparatingDistanceUtil
{
	pxQuat	m_ornA;
	pxQuat	m_ornB;
	pxVec3	m_posA;
	pxVec3	m_posB;
	
	pxVec3	m_separatingNormal;

	pxReal	m_boundingRadiusA;
	pxReal	m_boundingRadiusB;
	pxReal	m_separatingDistance;

public:

	pxConvexSeparatingDistanceUtil(pxReal	boundingRadiusA,pxReal	boundingRadiusB)
		:m_boundingRadiusA(boundingRadiusA),
		m_boundingRadiusB(boundingRadiusB),
		m_separatingDistance(0.f)
	{
	}

	pxReal	getConservativeSeparatingDistance()
	{
		return m_separatingDistance;
	}

	void	updateSeparatingDistance(const pxTransform& transA,const pxTransform& transB)
	{
		const pxVec3& toPosA = transA.GetOrigin();
		const pxVec3& toPosB = transB.GetOrigin();
		pxQuat toOrnA = transA.GetRotation();
		pxQuat toOrnB = transB.GetRotation();

		if (m_separatingDistance>0.f)
		{
			

			pxVec3 linVelA,angVelA,linVelB,angVelB;
			pxTransformUtil::calculateVelocityQuaternion(m_posA,toPosA,m_ornA,toOrnA,pxReal(1.),linVelA,angVelA);
			pxTransformUtil::calculateVelocityQuaternion(m_posB,toPosB,m_ornB,toOrnB,pxReal(1.),linVelB,angVelB);
			pxReal maxAngularProjectedVelocity = angVelA.Length() * m_boundingRadiusA + angVelB.Length() * m_boundingRadiusB;
			pxVec3 relLinVel = (linVelB-linVelA);
			pxReal relLinVelocLength = relLinVel.Dot(m_separatingNormal);
			if (relLinVelocLength<0.f)
			{
				relLinVelocLength = 0.f;
			}
	
			pxReal	projectedMotion = maxAngularProjectedVelocity + relLinVelocLength;
			m_separatingDistance -= projectedMotion;
		}
	
		m_posA = toPosA;
		m_posB = toPosB;
		m_ornA = toOrnA;
		m_ornB = toOrnB;
	}

	void	initSeparatingDistance(const pxVec3& separatingVector,pxReal separatingDistance,const pxTransform& transA,const pxTransform& transB)
	{
		m_separatingDistance = separatingDistance;

		if (m_separatingDistance>0.f)
		{
			m_separatingNormal = separatingVector;
			
			const pxVec3& toPosA = transA.GetOrigin();
			const pxVec3& toPosB = transB.GetOrigin();
			pxQuat toOrnA = transA.GetRotation();
			pxQuat toOrnB = transB.GetRotation();
			m_posA = toPosA;
			m_posB = toPosB;
			m_ornA = toOrnA;
			m_ornB = toOrnB;
		}
	}

};


MX_SWIPED("idTech 4");

#if MX_EDITOR

//===============================================================
//
//	idVec5 - 5D vector
//
//===============================================================

class idVec5 {
public:
	float			x;
	float			y;
	float			z;
	float			s;
	float			t;

					idVec5( void );
					explicit idVec5( const Vec3D &xyz, const Vec2D &st );
					explicit idVec5( const float x, const float y, const float z, const float s, const float t );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	idVec5 &		operator=( const Vec3D &a );

	int				GetDimension( void ) const;

	const Vec3D &	ToVec3( void ) const;
	Vec3D &			ToVec3( void );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

	void			Lerp( const idVec5 &v1, const idVec5 &v2, const float l );
};

extern idVec5 vec5_origin;
#define vec5_zero vec5_origin

MX_INLINE idVec5::idVec5( void ) {
}

MX_INLINE idVec5::idVec5( const Vec3D &xyz, const Vec2D &st ) {
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	s = st[0];
	t = st[1];
}

MX_INLINE idVec5::idVec5( const float x, const float y, const float z, const float s, const float t ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
	this->t = t;
}

MX_INLINE float idVec5::operator[]( int index ) const {
	return ( &x )[ index ];
}

MX_INLINE float& idVec5::operator[]( int index ) {
	return ( &x )[ index ];
}

MX_INLINE idVec5 &idVec5::operator=( const Vec3D &a ) { 
	x = a.x;
	y = a.y;
	z = a.z;
	s = t = 0;
	return *this;
}

MX_INLINE int idVec5::GetDimension( void ) const {
	return 5;
}

MX_INLINE const Vec3D &idVec5::ToVec3( void ) const {
	return *reinterpret_cast<const Vec3D *>(this);
}

MX_INLINE Vec3D &idVec5::ToVec3( void ) {
	return *reinterpret_cast<Vec3D *>(this);
}

MX_INLINE const float *idVec5::ToFloatPtr( void ) const {
	return &x;
}

MX_INLINE float *idVec5::ToFloatPtr( void ) {
	return &x;
}


//===============================================================
//
//	idVec6 - 6D vector
//
//===============================================================

class idVec6 {
public:	
					idVec6( void );
					explicit idVec6( const float *a );
					explicit idVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );

	void 			Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	void			Zero( void );

	float			operator[]( const int index ) const;
	float &			operator[]( const int index );
	idVec6			operator-() const;
	idVec6			operator*( const float a ) const;
	idVec6			operator/( const float a ) const;
	float			operator*( const idVec6 &a ) const;
	idVec6			operator-( const idVec6 &a ) const;
	idVec6			operator+( const idVec6 &a ) const;
	idVec6 &		operator*=( const float a );
	idVec6 &		operator/=( const float a );
	idVec6 &		operator+=( const idVec6 &a );
	idVec6 &		operator-=( const idVec6 &a );

	friend idVec6	operator*( const float a, const idVec6 b );

	bool			Compare( const idVec6 &a ) const;							// exact compare, no epsilon
	bool			Compare( const idVec6 &a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const idVec6 &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idVec6 &a ) const;						// exact compare, no epsilon

	float			Length( void ) const;
	float			LengthSqr( void ) const;
	float			Normalize( void );			// returns length
	float			NormalizeFast( void );		// returns length

	int				GetDimension( void ) const;

	const Vec3D &	SubVec3( int index ) const;
	Vec3D &			SubVec3( int index );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

private:
	float			p[6];
};

extern idVec6 vec6_origin;
#define vec6_zero vec6_origin
extern idVec6 vec6_infinity;

MX_INLINE idVec6::idVec6( void ) {
}

MX_INLINE idVec6::idVec6( const float *a ) {
	memcpy( p, a, 6 * sizeof( float ) );
}

MX_INLINE idVec6::idVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

MX_INLINE idVec6 idVec6::operator-() const {
	return idVec6( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

MX_INLINE float idVec6::operator[]( const int index ) const {
	return p[index];
}

MX_INLINE float &idVec6::operator[]( const int index ) {
	return p[index];
}

MX_INLINE idVec6 idVec6::operator*( const float a ) const {
	return idVec6( p[0]*a, p[1]*a, p[2]*a, p[3]*a, p[4]*a, p[5]*a );
}

MX_INLINE float idVec6::operator*( const idVec6 &a ) const {
	return p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3] + p[4] * a[4] + p[5] * a[5];
}

MX_INLINE idVec6 idVec6::operator/( const float a ) const {
	float inva;

	assert( a != 0.0f );
	inva = 1.0f / a;
	return idVec6( p[0]*inva, p[1]*inva, p[2]*inva, p[3]*inva, p[4]*inva, p[5]*inva );
}

MX_INLINE idVec6 idVec6::operator+( const idVec6 &a ) const {
	return idVec6( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

MX_INLINE idVec6 idVec6::operator-( const idVec6 &a ) const {
	return idVec6( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

MX_INLINE idVec6 &idVec6::operator*=( const float a ) {
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

MX_INLINE idVec6 &idVec6::operator/=( const float a ) {
	float inva;

	assert( a != 0.0f );
	inva = 1.0f / a;
	p[0] *= inva;
	p[1] *= inva;
	p[2] *= inva;
	p[3] *= inva;
	p[4] *= inva;
	p[5] *= inva;
	return *this;
}

MX_INLINE idVec6 &idVec6::operator+=( const idVec6 &a ) {
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

MX_INLINE idVec6 &idVec6::operator-=( const idVec6 &a ) {
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

MX_INLINE idVec6 operator*( const float a, const idVec6 b ) {
	return b * a;
}

MX_INLINE bool idVec6::Compare( const idVec6 &a ) const {
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

MX_INLINE bool idVec6::Compare( const idVec6 &a, const float epsilon ) const {
	if ( mxFabs( p[0] - a[0] ) > epsilon ) {
		return false;
	}
			
	if ( mxFabs( p[1] - a[1] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[2] - a[2] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[3] - a[3] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[4] - a[4] ) > epsilon ) {
		return false;
	}

	if ( mxFabs( p[5] - a[5] ) > epsilon ) {
		return false;
	}

	return true;
}

MX_INLINE bool idVec6::operator==( const idVec6 &a ) const {
	return Compare( a );
}

MX_INLINE bool idVec6::operator!=( const idVec6 &a ) const {
	return !Compare( a );
}

MX_INLINE void idVec6::Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

MX_INLINE void idVec6::Zero( void ) {
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

MX_INLINE float idVec6::Length( void ) const {
	return ( float )mxSqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

MX_INLINE float idVec6::LengthSqr( void ) const {
	return ( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

MX_INLINE float idVec6::Normalize( void ) {
	float sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = mxInvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

MX_INLINE float idVec6::NormalizeFast( void ) {
	float sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = mxInvSqrtEst( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

MX_INLINE int idVec6::GetDimension( void ) const {
	return 6;
}

MX_INLINE const Vec3D &idVec6::SubVec3( int index ) const {
	return *reinterpret_cast<const Vec3D *>(p + index * 3);
}

MX_INLINE Vec3D &idVec6::SubVec3( int index ) {
	return *reinterpret_cast<Vec3D *>(p + index * 3);
}

MX_INLINE const float *idVec6::ToFloatPtr( void ) const {
	return p;
}

MX_INLINE float *idVec6::ToFloatPtr( void ) {
	return p;
}




/*
===============================================================================

	A winding is an arbitrary convex polygon defined by an array of points.

===============================================================================
*/

class idWinding {

public:
					idWinding( void );
					explicit idWinding( const int n );								// allocate for n points
					explicit idWinding( const Vec3D *verts, const int n );			// winding from points
					explicit idWinding( const Vec3D &normal, const float dist );	// base winding for plane
					explicit idWinding( const Plane3D &plane );						// base winding for plane
					explicit idWinding( const idWinding &winding );
	virtual			~idWinding( void );

	idWinding &		operator=( const idWinding &winding );
	const idVec5 &	operator[]( const int index ) const;
	idVec5 &		operator[]( const int index );

					// add a point to the end of the winding point array
	idWinding &		operator+=( const Vec3D &v );
	idWinding &		operator+=( const idVec5 &v );
// RAVEN BEGIN
	//idWinding &		operator+=( const idDrawVert &dv );
// RAVEN END
	void			AddPoint( const Vec3D &v );
	void			AddPoint( const idVec5 &v );

					// number of points on winding
	int				GetNumPoints( void ) const;
	void			SetNumPoints( int n );
	virtual void	Clear( void );

					// huge winding for plane, the points go counter clockwise when facing the front of the plane
	void			BaseForPlane( const Vec3D &normal, const float dist );
	void			BaseForPlane( const Plane3D &plane );

					// splits the winding into a front and back winding, the winding itself stays unchanged
					// returns a PLANESIDE_?
	int				Split( const Plane3D &plane, const float epsilon, idWinding **front, idWinding **back ) const;
					// returns the winding fragment at the front of the clipping plane,
					// if there is nothing at the front the winding itself is destroyed and NULL is returned
	idWinding *		Clip( const Plane3D &plane, const float epsilon = ON_EPSILON, const bool keepOn = false );
					// cuts off the part at the back side of the plane, returns true if some part was at the front
					// if there is nothing at the front the number of points is set to zero
	bool			ClipInPlace( const Plane3D &plane, const float epsilon = ON_EPSILON, const bool keepOn = false );

					// returns a copy of the winding
	idWinding *		Copy( void ) const;
	idWinding *		Reverse( void ) const;
	void			ReverseSelf( void );
	void			RemoveEqualPoints( const float epsilon = ON_EPSILON );
	void			RemoveColinearPoints( const Vec3D &normal, const float epsilon = ON_EPSILON );
	void			RemovePoint( int point );
	void			InsertPoint( const Vec3D &point, int spot );
	bool			InsertPointIfOnEdge( const Vec3D &point, const Plane3D &plane, const float epsilon = ON_EPSILON );
					// add a winding to the convex hull
	void			AddToConvexHull( const idWinding *winding, const Vec3D &normal, const float epsilon = ON_EPSILON );
					// add a point to the convex hull
	void			AddToConvexHull( const Vec3D &point, const Vec3D &normal, const float epsilon = ON_EPSILON );
					// tries to merge 'this' with the given winding, returns NULL if merge fails, both 'this' and 'w' stay intact
					// 'keep' tells if the contacting points should stay even if they create colinear edges
	idWinding *		TryMerge( const idWinding &w, const Vec3D &normal, int keep = false ) const;
					// check whether the winding is valid or not
	bool			Check( bool print = true ) const;

	float			GetArea( void ) const;
	Vec3D			GetCenter( void ) const;

// RAVEN BEGIN
// scork: Splash Damage's light-resize code
	Vec3D			GetNormal( void ) const;
// RAVEN END
	float			GetRadius( const Vec3D &center ) const;
	void			GetPlane( Vec3D &normal, float &dist ) const;
	void			GetPlane( Plane3D &plane ) const;
	void			GetBounds( AABB &bounds ) const;

	bool			IsTiny( void ) const;
	bool			IsHuge( void ) const;	// base winding for a plane is typically huge
	void			Print( void ) const;

	float			PlaneDistance( const Plane3D &plane ) const;
	int				PlaneSide( const Plane3D &plane, const float epsilon = ON_EPSILON ) const;

	bool			PlanesConcave( const idWinding &w2, const Vec3D &normal1, const Vec3D &normal2, float dist1, float dist2 ) const;

	bool			PointInside( const Vec3D &normal, const Vec3D &point, const float epsilon ) const;
					// returns true if the line or ray intersects the winding
	bool			LineIntersection( const Plane3D &windingPlane, const Vec3D &start, const Vec3D &end, bool backFaceCull = false ) const;
					// intersection point is start + dir * scale
	bool			RayIntersection( const Plane3D &windingPlane, const Vec3D &start, const Vec3D &dir, float &scale, bool backFaceCull = false ) const;

	static float	TriangleArea( const Vec3D &a, const Vec3D &b, const Vec3D &c );

protected:
	int				numPoints;				// number of points
	idVec5 *		p;						// pointer to point data
	int				allocedSize;

	bool			EnsureAlloced( int n, bool keep = false );
	virtual bool	ReAllocate( int n, bool keep = false );
};

MX_INLINE idWinding::idWinding( void ) {
	numPoints = allocedSize = 0;
	p = NULL;
}

MX_INLINE idWinding::idWinding( int n ) {
	numPoints = allocedSize = 0;
	p = NULL;
	EnsureAlloced( n );
}

MX_INLINE idWinding::idWinding( const Vec3D *verts, const int n ) {
	int i;

	numPoints = allocedSize = 0;
	p = NULL;
	if ( !EnsureAlloced( n ) ) {
		numPoints = 0;
		return;
	}
	for ( i = 0; i < n; i++ ) {
		p[i].ToVec3() = verts[i];
		p[i].s = p[i].t = 0.0f;
	}
	numPoints = n;
}

MX_INLINE idWinding::idWinding( const Vec3D &normal, const float dist ) {
	numPoints = allocedSize = 0;
	p = NULL;
	BaseForPlane( normal, dist );
}

MX_INLINE idWinding::idWinding( const Plane3D &plane ) {
	numPoints = allocedSize = 0;
	p = NULL;
	BaseForPlane( plane );
}

MX_INLINE idWinding::idWinding( const idWinding &winding ) {
	int i;
	if ( !EnsureAlloced( winding.GetNumPoints() ) ) {
		numPoints = 0;
		return;
	}
	for ( i = 0; i < winding.GetNumPoints(); i++ ) {
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

MX_INLINE idWinding::~idWinding( void ) {
	delete[] p;
	p = NULL;
}

MX_INLINE idWinding &idWinding::operator=( const idWinding &winding ) {
	int i;

	if ( !EnsureAlloced( winding.numPoints ) ) {
		numPoints = 0;
		return *this;
	}
	for ( i = 0; i < winding.numPoints; i++ ) {
		p[i] = winding.p[i];
	}
	numPoints = winding.numPoints;
	return *this;
}

MX_INLINE const idVec5 &idWinding::operator[]( const int index ) const {
	//assert( index >= 0 && index < numPoints );
	return p[ index ];
}

MX_INLINE idVec5 &idWinding::operator[]( const int index ) {
	//assert( index >= 0 && index < numPoints );
	return p[ index ];
}

MX_INLINE idWinding &idWinding::operator+=( const Vec3D &v ) {
	AddPoint( v );
	return *this;
}

MX_INLINE idWinding &idWinding::operator+=( const idVec5 &v ) {
	AddPoint( v );
	return *this;
}

// RAVEN BEGIN
//MX_INLINE idWinding &idWinding::operator+=( const idDrawVert &dv ) {
//	if ( !EnsureAlloced(numPoints+1, true) ) {
//		return *this;
//	}
//	p[numPoints] = idVec5( dv.xyz, dv.st );
//	numPoints++;
//	return *this;
//}
// RAVEN END

MX_INLINE void idWinding::AddPoint( const Vec3D &v ) {
	if ( !EnsureAlloced(numPoints+1, true) ) {
		return;
	}
	p[numPoints] = v;
	numPoints++;
}

MX_INLINE void idWinding::AddPoint( const idVec5 &v ) {
	if ( !EnsureAlloced(numPoints+1, true) ) {
		return;
	}
	p[numPoints] = v;
	numPoints++;
}

MX_INLINE int idWinding::GetNumPoints( void ) const {
	return numPoints;
}

MX_INLINE void idWinding::SetNumPoints( int n ) {
	if ( !EnsureAlloced( n, true ) ) {
		return;
	}
	numPoints = n;
}

MX_INLINE void idWinding::Clear( void ) {
	numPoints = 0;
	// RAVENBEGIN
	// cdr: need to clear the allocated size too, or we wont' be able to readd points
	allocedSize = 0;
	// RAVENEND
	delete[] p;
	p = NULL;
}

MX_INLINE void idWinding::BaseForPlane( const Plane3D &plane ) {
	BaseForPlane( plane.Normal(), plane.Dist() );
}

MX_INLINE bool idWinding::EnsureAlloced( int n, bool keep ) {
	if ( n > allocedSize ) {
		return ReAllocate( n, keep );
	}
	return true;
}


/*
===============================================================================

	idFixedWinding is a fixed buffer size winding not using
	memory allocations.

	When an operation would overflow the fixed buffer a warning
	is printed and the operation is safely cancelled.

===============================================================================
*/

#define	MAX_POINTS_ON_WINDING	64

class idFixedWinding : public idWinding {

public:
					idFixedWinding( void );
					explicit idFixedWinding( const int n );
					explicit idFixedWinding( const Vec3D *verts, const int n );
					explicit idFixedWinding( const Vec3D &normal, const float dist );
					explicit idFixedWinding( const Plane3D &plane );
					explicit idFixedWinding( const idWinding &winding );
					explicit idFixedWinding( const idFixedWinding &winding );
	virtual			~idFixedWinding( void );

	idFixedWinding &operator=( const idWinding &winding );

	virtual void	Clear( void );

					// splits the winding in a back and front part, 'this' becomes the front part
					// returns a PLANESIDE_?
	int				Split( idFixedWinding *back, const Plane3D &plane, const float epsilon = ON_EPSILON );

protected:
	idVec5			data[MAX_POINTS_ON_WINDING];	// point data

	virtual bool	ReAllocate( int n, bool keep = false );
};

MX_INLINE idFixedWinding::idFixedWinding( void ) {
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
}

MX_INLINE idFixedWinding::idFixedWinding( int n ) {
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
}

MX_INLINE idFixedWinding::idFixedWinding( const Vec3D *verts, const int n ) {
	int i;

	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if ( !EnsureAlloced( n ) ) {
		numPoints = 0;
		return;
	}
	for ( i = 0; i < n; i++ ) {
		p[i].ToVec3() = verts[i];
		p[i].s = p[i].t = 0;
	}
	numPoints = n;
}

MX_INLINE idFixedWinding::idFixedWinding( const Vec3D &normal, const float dist ) {
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	BaseForPlane( normal, dist );
}

MX_INLINE idFixedWinding::idFixedWinding( const Plane3D &plane ) {
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	BaseForPlane( plane );
}

MX_INLINE idFixedWinding::idFixedWinding( const idWinding &winding ) {
	int i;

	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if ( !EnsureAlloced( winding.GetNumPoints() ) ) {
		numPoints = 0;
		return;
	}
	for ( i = 0; i < winding.GetNumPoints(); i++ ) {
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

MX_INLINE idFixedWinding::idFixedWinding( const idFixedWinding &winding ) {
	int i;

	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if ( !EnsureAlloced( winding.GetNumPoints() ) ) {
		numPoints = 0;
		return;
	}
	for ( i = 0; i < winding.GetNumPoints(); i++ ) {
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

MX_INLINE idFixedWinding::~idFixedWinding( void ) {
	p = NULL;	// otherwise it tries to free the fixed buffer
}

MX_INLINE idFixedWinding &idFixedWinding::operator=( const idWinding &winding ) {
	int i;

	if ( !EnsureAlloced( winding.GetNumPoints() ) ) {
		numPoints = 0;
		return *this;
	}
	for ( i = 0; i < winding.GetNumPoints(); i++ ) {
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
	return *this;
}

MX_INLINE void idFixedWinding::Clear( void ) {
	numPoints = 0;
}







/*
===============================================================================

	List template
	Does not allocate memory until the first item is added.

===============================================================================
*/


/*
================
idListSortCompare<type>
================
*/
#ifdef __INTEL_COMPILER
// the intel compiler doesn't do the right thing here
template< class type >
MX_INLINE int idListSortCompare( const type *a, const type *b ) {
	assert( 0 );
	return 0;
}
#else
template< class type >
MX_INLINE int idListSortCompare( const type *a, const type *b ) {
	return *a - *b;
}
#endif

/*
================
idListNewElement<type>
================
*/
template< class type >
MX_INLINE type *idListNewElement( void ) {
	return new type;
}

/*
================
idSwap<type>
================
*/
template< class type >
MX_INLINE void idSwap( type &a, type &b ) {
	type c = a;
	a = b;
	b = c;
}

template< class type >
class idList {
public:

	typedef int		cmp_t( const type *, const type * );
	typedef type	new_t( void );

					idList( int newgranularity = 16 );
					idList( const idList<type> &other );
					~idList<type>( void );

	void			Clear( void );										// clear the list
	int				Num( void ) const;									// returns number of elements in list
	int				NumAllocated( void ) const;							// returns number of elements allocated for
	void			SetGranularity( int newgranularity );				// set new granularity
	int				GetGranularity( void ) const;						// get the current granularity

	size_t			Allocated( void ) const;							// returns total size of allocated memory
	size_t			Size( void ) const;									// returns total size of allocated memory including size of list type
	size_t			MemoryUsed( void ) const;							// returns size of the used elements in the list

	idList<type> &	operator=( const idList<type> &other );
	const type &	operator[]( int index ) const;
	type &			operator[]( int index );

	void			Condense( void );									// resizes list to exactly the number of elements it contains
	void			Resize( int newsize );								// resizes list to the given number of elements
	void			Resize( int newsize, int newgranularity	 );			// resizes list and sets new granularity
	void			SetNum( int newnum, bool resize = true );			// set number of elements in list and resize to exactly this number if necessary
	void			AssureSize( int newSize);							// assure list has given number of elements, but leave them uninitialized
	void			AssureSize( int newSize, const type &initValue );	// assure list has given number of elements and initialize any new elements
	void			AssureSizeAlloc( int newSize, new_t *allocator );	// assure the pointer list has the given number of elements and allocate any new elements

	type *			Ptr( void );										// returns a pointer to the list
	const type *	Ptr( void ) const;									// returns a pointer to the list
	type &			Alloc( void );										// returns reference to a new data element at the end of the list
	int				Append( const type & obj );							// append element
	int				Append( const idList<type> &other );				// append list
	int				AddUnique( const type & obj );						// add unique element
	int				Insert( const type & obj, int index = 0 );			// insert the element at the given index
	int				FindIndex( const type & obj ) const;				// find the index for the given element
	type *			Find( type const & obj ) const;						// find pointer to the given element
	int				FindNull( void ) const;								// find the index for the first NULL pointer in the list
	int				IndexOf( const type *obj ) const;					// returns the index for the pointer to an element in the list
	bool			RemoveIndex( int index );							// remove the element at the given index
	bool			Remove( const type & obj );							// remove the element
	void			Sort( cmp_t *compare = ( cmp_t * )&idListSortCompare<type> );
	void			SortSubSection( int startIndex, int endIndex, cmp_t *compare = ( cmp_t * )&idListSortCompare<type> );
	void			Swap( idList<type> &other );						// swap the contents of the lists
	void			DeleteContents( bool clear );						// delete the contents of the list

private:
	int				num;
	int				size;
	int				granularity;
	type *			list;
};

/*
================
idList<type>::idList( int )
================
*/
template< class type >
MX_INLINE idList<type>::idList( int newgranularity ) {
	assert( newgranularity > 0 );

	list		= NULL;
	granularity	= newgranularity;
	Clear();
}

/*
================
idList<type>::idList( const idList<type> &other )
================
*/
template< class type >
MX_INLINE idList<type>::idList( const idList<type> &other ) {
	list = NULL;
	*this = other;
}

/*
================
idList<type>::~idList<type>
================
*/
template< class type >
MX_INLINE idList<type>::~idList( void ) {
	Clear();
}

/*
================
idList<type>::Clear

Frees up the memory allocated by the list.  Assumes that type automatically handles freeing up memory.
================
*/
template< class type >
MX_INLINE void idList<type>::Clear( void ) {
	if ( list ) {
		delete[] list;
	}

	list	= NULL;
	num		= 0;
	size	= 0;
}

/*
================
idList<type>::DeleteContents

Calls the destructor of all elements in the list.  Conditionally frees up memory used by the list.
Note that this only works on lists containing pointers to objects and will cause a compiler error
if called with non-pointers.  Since the list was not responsible for allocating the object, it has
no information on whether the object still exists or not, so care must be taken to ensure that
the pointers are still valid when this function is called.  Function will set all pointers in the
list to NULL.
================
*/
template< class type >
MX_INLINE void idList<type>::DeleteContents( bool clear ) {
	int i;

	for( i = 0; i < num; i++ ) {
		delete list[ i ];
		list[ i ] = NULL;
	}

	if ( clear ) {
		Clear();
	} else {
		memset( list, 0, size * sizeof( type ) );
	}
}

/*
================
idList<type>::Allocated

return total memory allocated for the list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
MX_INLINE size_t idList<type>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
idList<type>::Size

return total size of list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
MX_INLINE size_t idList<type>::Size( void ) const {
	return sizeof( idList<type> ) + Allocated();
}

/*
================
idList<type>::MemoryUsed
================
*/
template< class type >
MX_INLINE size_t idList<type>::MemoryUsed( void ) const {
	return num * sizeof( *list );
}

/*
================
idList<type>::Num

Returns the number of elements currently contained in the list.
Note that this is NOT an indication of the memory allocated.
================
*/
template< class type >
MX_INLINE int idList<type>::Num( void ) const {
	return num;
}

/*
================
idList<type>::NumAllocated

Returns the number of elements currently allocated for.
================
*/
template< class type >
MX_INLINE int idList<type>::NumAllocated( void ) const {
	return size;
}

/*
================
idList<type>::SetNum

Resize to the exact size specified irregardless of granularity
================
*/
template< class type >
MX_INLINE void idList<type>::SetNum( int newnum, bool resize ) {
	assert( newnum >= 0 );
	if ( resize || newnum > size ) {
		Resize( newnum );
	}
	num = newnum;
}

/*
================
idList<type>::SetGranularity

Sets the base size of the array and resizes the array to match.
================
*/
template< class type >
MX_INLINE void idList<type>::SetGranularity( int newgranularity ) {
	int newsize;

	assert( newgranularity > 0 );
	granularity = newgranularity;

	if ( list ) {
		// resize it to the closest level of granularity
		newsize = num + granularity - 1;
		newsize -= newsize % granularity;
		if ( newsize != size ) {
			Resize( newsize );
		}
	}
}

/*
================
idList<type>::GetGranularity

Get the current granularity.
================
*/
template< class type >
MX_INLINE int idList<type>::GetGranularity( void ) const {
	return granularity;
}

/*
================
idList<type>::Condense

Resizes the array to exactly the number of elements it contains or frees up memory if empty.
================
*/
template< class type >
MX_INLINE void idList<type>::Condense( void ) {
	if ( list ) {
		if ( num ) {
			Resize( num );
		} else {
			Clear();
		}
	}
}

/*
================
idList<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type >
MX_INLINE void idList<type>::Resize( int newsize ) {
	type	*temp;
	int		i;

	assert( newsize >= 0 );

	// free up the list if no data is being reserved
	if ( newsize <= 0 ) {
		Clear();
		return;
	}

	if ( newsize == size ) {
		// not changing the size, so just exit
		return;
	}

	temp	= list;
	size	= newsize;
	if ( size < num ) {
		num = size;
	}

	// copy the old list into our new one
	list = new type[ size ];
	for( i = 0; i < num; i++ ) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if ( temp ) {
		delete[] temp;
	}
}

/*
================
idList<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type >
MX_INLINE void idList<type>::Resize( int newsize, int newgranularity ) {
	type	*temp;
	int		i;

	assert( newsize >= 0 );

	assert( newgranularity > 0 );
	granularity = newgranularity;

	// free up the list if no data is being reserved
	if ( newsize <= 0 ) {
		Clear();
		return;
	}

	temp	= list;
	size	= newsize;
	if ( size < num ) {
		num = size;
	}

	// copy the old list into our new one
	list = new type[ size ];
	for( i = 0; i < num; i++ ) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if ( temp ) {
		delete[] temp;
	}
}

/*
================
idList<type>::AssureSize

Makes sure the list has at least the given number of elements.
================
*/
template< class type >
MX_INLINE void idList<type>::AssureSize( int newSize ) {
	int newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		Resize( newSize );
	}

	num = newNum;
}

/*
================
idList<type>::AssureSize

Makes sure the list has at least the given number of elements and initialize any elements not yet initialized.
================
*/
template< class type >
MX_INLINE void idList<type>::AssureSize( int newSize, const type &initValue ) {
	int newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		num = size;
		Resize( newSize );

		for ( int i = num; i < newSize; i++ ) {
			list[i] = initValue;
		}
	}

	num = newNum;
}

/*
================
idList<type>::AssureSizeAlloc

Makes sure the list has at least the given number of elements and allocates any elements using the allocator.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
MX_INLINE void idList<type>::AssureSizeAlloc( int newSize, new_t *allocator ) {
	int newNum = newSize;

	if ( newSize > size ) {

		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		num = size;
		Resize( newSize );

		for ( int i = num; i < newSize; i++ ) {
			list[i] = (*allocator)();
		}
	}

	num = newNum;
}

/*
================
idList<type>::operator=

Copies the contents and size attributes of another list.
================
*/
template< class type >
MX_INLINE idList<type> &idList<type>::operator=( const idList<type> &other ) {
	int	i;

	Clear();

	num			= other.num;
	size		= other.size;
	granularity	= other.granularity;

	if ( size ) {
		list = new type[ size ];
		for( i = 0; i < num; i++ ) {
			list[ i ] = other.list[ i ];
		}
	}

	return *this;
}

/*
================
idList<type>::operator[] const

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
MX_INLINE const type &idList<type>::operator[]( int index ) const {
	assert( index >= 0 );
	assert( index < num );

	return list[ index ];
}

/*
================
idList<type>::operator[]

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
MX_INLINE type &idList<type>::operator[]( int index ) {
	assert( index >= 0 );
	assert( index < num );

	return list[ index ];
}

/*
================
idList<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
MX_INLINE type *idList<type>::Ptr( void ) {
	return list;
}

/*
================
idList<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
const MX_INLINE type *idList<type>::Ptr( void ) const {
	return list;
}

/*
================
idList<type>::Alloc

Returns a reference to a new data element at the end of the list.
================
*/
template< class type >
MX_INLINE type &idList<type>::Alloc( void ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		Resize( size + granularity );
	}

	return list[ num++ ];
}

/*
================
idList<type>::Append

Increases the size of the list by one element and copies the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
MX_INLINE int idList<type>::Append( type const & obj ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		int newsize;

		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		newsize = size + granularity;
		Resize( newsize - newsize % granularity );
	}

	list[ num ] = obj;
	num++;

	return num - 1;
}


/*
================
idList<type>::Insert

Increases the size of the list by at leat one element if necessary 
and inserts the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
MX_INLINE int idList<type>::Insert( type const & obj, int index ) {
	if ( !list ) {
		Resize( granularity );
	}

	if ( num == size ) {
		int newsize;

		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		newsize = size + granularity;
		Resize( newsize - newsize % granularity );
	}

	if ( index < 0 ) {
		index = 0;
	}
	else if ( index > num ) {
		index = num;
	}
	for ( int i = num; i > index; --i ) {
		list[i] = list[i-1];
	}
	num++;
	list[index] = obj;
	return index;
}

/*
================
idList<type>::Append

adds the other list to this one

Returns the size of the new combined list
================
*/
template< class type >
MX_INLINE int idList<type>::Append( const idList<type> &other ) {
	if ( !list ) {
		if ( granularity == 0 ) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		Resize( granularity );
	}

	int n = other.Num();
	for (int i = 0; i < n; i++) {
		Append(other[i]);
	}

	return Num();
}

/*
================
idList<type>::AddUnique

Adds the data to the list if it doesn't already exist.  Returns the index of the data in the list.
================
*/
template< class type >
MX_INLINE int idList<type>::AddUnique( type const & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index < 0 ) {
		index = Append( obj );
	}

	return index;
}

/*
================
idList<type>::FindIndex

Searches for the specified data in the list and returns it's index.  Returns -1 if the data is not found.
================
*/
template< class type >
MX_INLINE int idList<type>::FindIndex( type const & obj ) const {
	int i;

	for( i = 0; i < num; i++ ) {
		if ( list[ i ] == obj ) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
idList<type>::Find

Searches for the specified data in the list and returns it's address. Returns NULL if the data is not found.
================
*/
template< class type >
MX_INLINE type *idList<type>::Find( type const & obj ) const {
	int i;

	i = FindIndex( obj );
	if ( i >= 0 ) {
		return &list[ i ];
	}

	return NULL;
}

/*
================
idList<type>::FindNull

Searches for a NULL pointer in the list.  Returns -1 if NULL is not found.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
MX_INLINE int idList<type>::FindNull( void ) const {
	int i;

	for( i = 0; i < num; i++ ) {
		if ( list[ i ] == NULL ) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
idList<type>::IndexOf

Takes a pointer to an element in the list and returns the index of the element.
This is NOT a guarantee that the object is really in the list. 
Function will assert in debug builds if pointer is outside the bounds of the list,
but remains silent in release builds.
================
*/
template< class type >
MX_INLINE int idList<type>::IndexOf( type const *objptr ) const {
	int index;

	index = objptr - list;

	assert( index >= 0 );
	assert( index < num );

	return index;
}

/*
================
idList<type>::RemoveIndex

Removes the element at the specified index and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the index is outside the bounds of the list.
Note that the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
MX_INLINE bool idList<type>::RemoveIndex( int index ) {
	int i;

	assert( list != NULL );
	assert( index >= 0 );
	assert( index < num );

	if ( ( index < 0 ) || ( index >= num ) ) {
		return false;
	}

	num--;
	for( i = index; i < num; i++ ) {
		list[ i ] = list[ i + 1 ];
	}

	return true;
}

/*
================
idList<type>::Remove

Removes the element if it is found within the list and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the data is not found in the list.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
MX_INLINE bool idList<type>::Remove( type const & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index >= 0 ) {
		return RemoveIndex( index );
	}
	
	return false;
}

/*
================
idList<type>::Sort

Performs a qsort on the list using the supplied comparison function.  Note that the data is merely moved around the
list, so any pointers to data within the list may no longer be valid.
================
*/
template< class type >
MX_INLINE void idList<type>::Sort( cmp_t *compare ) {
	if ( !list ) {
		return;
	}
	typedef int cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	qsort( ( void * )list, ( size_t )num, sizeof( type ), vCompare );
}

/*
================
idList<type>::SortSubSection

Sorts a subsection of the list.
================
*/
template< class type >
MX_INLINE void idList<type>::SortSubSection( int startIndex, int endIndex, cmp_t *compare ) {
	if ( !list ) {
		return;
	}
	if ( startIndex < 0 ) {
		startIndex = 0;
	}
	if ( endIndex >= num ) {
		endIndex = num - 1;
	}
	if ( startIndex >= endIndex ) {
		return;
	}
	typedef int cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	qsort( ( void * )( &list[startIndex] ), ( size_t )( endIndex - startIndex + 1 ), sizeof( type ), vCompare );
}

/*
================
idList<type>::Swap

Swaps the contents of two lists
================
*/
template< class type >
MX_INLINE void idList<type>::Swap( idList<type> &other ) {
	idSwap( num, other.num );
	idSwap( size, other.size );
	idSwap( granularity, other.granularity );
	idSwap( list, other.list );
}


#endif // MX_EDITOR

#endif // !__PX_UTILITIES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
