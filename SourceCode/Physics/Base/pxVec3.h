/*
=============================================================================
	File:	pxVec3.h
	Desc:	.
=============================================================================
*/

#ifndef __PX_Vector3D_H__
#define __PX_Vector3D_H__

#include <Physics/Base/pxQuadWord.h>

MX_SWIPED("Bullet/Solid")
/**@brief pxVec3 can be used to represent 3D points and vectors.
 * It has an un-used w component to suit 16-byte alignment when pxVec3 is stored in containers. This extra component can be used by derived classes (Quaternion?) or by user
 * Ideally, this class should be replaced by a platform optimized SIMD version that keeps the data in registers
 */
MX_ALIGN_16(class) pxVec3
{
public:
	union {
		pxQuadReal mVec128;
		pxReal	m_floats[4];
		struct { pxReal x, y, z, w; };
		struct { pxReal r, g, b, a; };
		struct { pxReal cell[4]; };
		struct { pxInt ix, iy, iz, iw; };
		struct { pxQuadReal xyzw; };
	};
	PX_INLINE	__m128	get128() const
	{
		return mVec128;
	}
	PX_INLINE	void	set128(__m128 v128)
	{
		mVec128 = v128;
	}

public:

	PX_INLINE pxVec3()
	{}
 
	PX_INLINE pxVec3( __m128 q )
	{
		mVec128 = q;
	}

	PX_INLINE pxVec3( pxReal xyz )
	{
		m_floats[0] = xyz;
		m_floats[1] = xyz;
		m_floats[2] = xyz;
		m_floats[3] = pxReal(0.);
	}

	PX_INLINE pxVec3( pxReal x, pxReal y, pxReal z )
	{
		m_floats[0] = x;
		m_floats[1] = y;
		m_floats[2] = z;
		m_floats[3] = pxReal(0.);
	}

	PX_INLINE pxVec3& operator += ( const pxVec3& v )
	{
		m_floats[0] += v.m_floats[0]; m_floats[1] += v.m_floats[1];m_floats[2] += v.m_floats[2];
		return *this;
	}

	PX_INLINE pxVec3& operator -= ( const pxVec3& v ) 
	{
		m_floats[0] -= v.m_floats[0]; m_floats[1] -= v.m_floats[1];m_floats[2] -= v.m_floats[2];
		return *this;
	}

	PX_INLINE pxVec3& operator *= ( pxReal s )
	{
		m_floats[0] *= s; m_floats[1] *= s;m_floats[2] *= s;
		return *this;
	}

  /**@brief Inversely scale the vector 
   * @param s Scale factor to divide by */
	PX_INLINE pxVec3& operator/=( pxReal s ) 
	{
		AlwaysAssert(s != pxReal(0.0));
		return *this *= pxReal(1.0) / s;
	}

  /**@brief Return the dot product
   * @param v The other vector in the dot product */
	PX_INLINE pxReal Dot( const pxVec3& v ) const
	{
		return m_floats[0] * v.m_floats[0] + m_floats[1] * v.m_floats[1] +m_floats[2] * v.m_floats[2];
	}

	PX_INLINE pxReal* ToPtr() {
		return (pxReal*)this;
	}
	PX_INLINE const pxReal* ToPtr() const {
		return (const pxReal*)this;
	}

	PX_INLINE pxVec3 GetScaled( const pxReal f ) const {
		return pxVec3(
			m_floats[0] * f,
			m_floats[1] * f,
			m_floats[2] * f
		);
	}

  /**@brief Return the length of the vector squared */
	PX_INLINE pxReal LengthSqr() const
	{
		return Dot(*this);
	}

  /**@brief Return the length of the vector */
	PX_INLINE pxReal Length() const
	{
		return mxSqrt(LengthSqr());
	}

  /**@brief Return the distance squared between the ends of this and another vector
   * This is symantically treating the vector like a point */
	PX_INLINE pxReal distance2( const pxVec3& v ) const;

  /**@brief Return the distance between the ends of this and another vector
   * This is symantically treating the vector like a point */
	PX_INLINE pxReal distance( const pxVec3& v ) const;

	PX_INLINE pxVec3& safeNormalize() 
	{
		pxVec3 absVec = this->absolute();
		int maxIndex = absVec.maxAxis();
		if (absVec[maxIndex]>0)
		{
			*this /= absVec[maxIndex];
			return *this /= Length();
		}
		SetValue(1,0,0);
		return *this;
	}

  /**@brief Normalize this vector 
   * x^2 + y^2 + z^2 = 1 */
	PX_INLINE pxVec3& Normalize() 
	{
		return *this /= Length();
	}

	PX_INLINE bool IsNormalized( const pxReal epsilon = PX_TINY_NUMBER ) const
	{
		return mxFabs( LengthSqr() - 1.0f ) < epsilon;
	}
	FORCEINLINE const pxVec3 GetNormalized() const
	{
		return XMVector3Normalize(mVec128);
	}

  /**@brief Return a normalized version of this vector */
	PX_INLINE pxVec3 normalized() const;

  /**@brief Return a rotated version of this vector
   * @param wAxis The axis to rotate about 
   * @param angle The angle to rotate by */
	PX_INLINE pxVec3 rotate( const pxVec3& wAxis, const pxReal angle ) const;

  /**@brief Return the angle between this and another vector
   * @param v The other vector */
	PX_INLINE pxReal angle( const pxVec3& v ) const 
	{
		pxReal s = mxSqrt(LengthSqr() * v.LengthSqr());
		AlwaysAssert(s != pxReal(0.0));
		return mxACos( Dot(v) / s );
	}
  /**@brief Return a vector with the absolute values of each element */
	PX_INLINE pxVec3 absolute() const
	{
		return pxVec3(
			mxFabs(m_floats[0]), 
			mxFabs(m_floats[1]), 
			mxFabs(m_floats[2])
		);
	}
  /**@brief Return the cross product between this and another vector 
   * @param v The other vector */
	PX_INLINE pxVec3 Cross( const pxVec3& v ) const
	{
		return pxVec3(
			m_floats[1] * v.m_floats[2] - m_floats[2] * v.m_floats[1],
			m_floats[2] * v.m_floats[0] - m_floats[0] * v.m_floats[2],
			m_floats[0] * v.m_floats[1] - m_floats[1] * v.m_floats[0]
		);
	}

	PX_INLINE pxReal triple( const pxVec3& v1, const pxVec3& v2 ) const
	{
		return
			m_floats[0] * (v1.m_floats[1] * v2.m_floats[2] - v1.m_floats[2] * v2.m_floats[1]) + 
			m_floats[1] * (v1.m_floats[2] * v2.m_floats[0] - v1.m_floats[0] * v2.m_floats[2]) + 
			m_floats[2] * (v1.m_floats[0] * v2.m_floats[1] - v1.m_floats[1] * v2.m_floats[0]);
	}

  /**@brief Return the axis with the smallest value 
   * Note return values are 0,1,2 for x, y, or z */
	PX_INLINE int minAxis() const
	{
		return m_floats[0] < m_floats[1] ? (m_floats[0] <m_floats[2] ? 0 : 2) : (m_floats[1] <m_floats[2] ? 1 : 2);
	}

  /**@brief Return the axis with the largest value 
   * Note return values are 0,1,2 for x, y, or z */
	PX_INLINE int maxAxis() const 
	{
		return m_floats[0] < m_floats[1] ? (m_floats[1] <m_floats[2] ? 2 : 1) : (m_floats[0] <m_floats[2] ? 2 : 0);
	}

	PX_INLINE int furthestAxis() const
	{
		return absolute().minAxis();
	}

	PX_INLINE int closestAxis() const 
	{
		return absolute().maxAxis();
	}

	PX_INLINE void setInterpolate3( const pxVec3& v0, const pxVec3& v1, pxReal rt)
	{
		pxReal s = pxReal(1.0) - rt;
		m_floats[0] = s * v0.m_floats[0] + rt * v1.m_floats[0];
		m_floats[1] = s * v0.m_floats[1] + rt * v1.m_floats[1];
		m_floats[2] = s * v0.m_floats[2] + rt * v1.m_floats[2];
		//don't do the unused w component
		//		m_co[3] = s * v0[3] + rt * v1[3];
	}

  /**@brief Return the linear interpolation between this and another vector 
   * @param v The other vector 
   * @param t The ration of this to v (t = 0 => return this, t=1 => return other) */
	PX_INLINE pxVec3 lerp( const pxVec3& v,  pxReal t) const 
	{
		return pxVec3(m_floats[0] + (v.m_floats[0] - m_floats[0]) * t,
			m_floats[1] + (v.m_floats[1] - m_floats[1]) * t,
			m_floats[2] + (v.m_floats[2] -m_floats[2]) * t);
	}

  /**@brief Elementwise multiply this vector by the other 
   * @param v The other vector */
	PX_INLINE pxVec3& operator*=( const pxVec3& v )
	{
		m_floats[0] *= v.m_floats[0]; m_floats[1] *= v.m_floats[1];m_floats[2] *= v.m_floats[2];
		return *this;
	}


	/**@brief Set the x value */
	PX_INLINE void	setX(pxReal x) { m_floats[0] = x;};
	/**@brief Set the y value */
	PX_INLINE void	setY(pxReal y) { m_floats[1] = y;};
	/**@brief Set the z value */
	PX_INLINE void	setZ(pxReal z) {m_floats[2] = z;};
	/**@brief Set the w value */
	PX_INLINE void	setW(pxReal w) { m_floats[3] = w;};
	/**@brief Return the x value */
	PX_INLINE  pxReal getX() const { return m_floats[0]; }
	/**@brief Return the y value */
	PX_INLINE  pxReal getY() const { return m_floats[1]; }
	/**@brief Return the z value */
	PX_INLINE  pxReal getZ() const { return m_floats[2]; }
	/**@brief Return the w value */
	PX_INLINE  pxReal GetW() const { return m_floats[3]; }

	//PX_INLINE pxReal&       operator[](int i)       { return (&m_floats[0])[i];	}      
	//PX_INLINE  pxReal operator[](int i) const { return (&m_floats[0])[i]; }
	///operator pxReal*() replaces operator[], using implicit conversion. We added operator != and operator == to avoid pointer comparisons.
	PX_INLINE	operator       pxReal *()       { return &m_floats[0]; }
	PX_INLINE	operator const pxReal *() const { return &m_floats[0]; }

	PX_INLINE	bool	operator==( const pxVec3& other) const
	{
		return ((m_floats[3]==other.m_floats[3]) && (m_floats[2]==other.m_floats[2]) && (m_floats[1]==other.m_floats[1]) && (m_floats[0]==other.m_floats[0]));
	}

	PX_INLINE	bool	operator!=( const pxVec3& other) const
	{
		return !(*this == other);
	}

	/**@brief Set each element to the max of the current values and the values of another pxVec3
	* @param other The other pxVec3 to compare with 
	*/
	PX_INLINE void	setMax( const pxVec3& other)
	{
		TSetMax(m_floats[0], other.m_floats[0]);
		TSetMax(m_floats[1], other.m_floats[1]);
		TSetMax(m_floats[2], other.m_floats[2]);
		TSetMax(m_floats[3], other.GetW());
	}
	/**@brief Set each element to the min of the current values and the values of another pxVec3
	* @param other The other pxVec3 to compare with 
	*/
	PX_INLINE void setMin( const pxVec3& other )
	{
		TSetMin(m_floats[0], other.m_floats[0]);
		TSetMin(m_floats[1], other.m_floats[1]);
		TSetMin(m_floats[2], other.m_floats[2]);
		TSetMin(m_floats[3], other.GetW());
	}

	PX_INLINE void set( const pxVec3& other )
	{
		mVec128 = other.mVec128;
	}

	PX_INLINE void SetValue( pxReal x,  pxReal y,  pxReal z )
	{
		m_floats[0] = x;
		m_floats[1] = y;
		m_floats[2] = z;
		m_floats[3] = pxReal(0.);
	}

	PX_INLINE void SetAll(  pxReal xyz )
	{
		m_floats[0] = xyz;
		m_floats[1] = xyz;
		m_floats[2] = xyz;
		m_floats[3] = pxReal(0.);
	}

	PX_INLINE void Set( const pxReal* src )
	{
		m_floats[0] = src[0];
		m_floats[1] = src[1];
		m_floats[2] = src[2];
		m_floats[3] = pxReal(0.);
	}

	void getSkewSymmetricMatrix(pxVec3* v0,pxVec3* v1,pxVec3* v2) const
	{
		v0->SetValue(0.		,-getZ()		,getY());
		v1->SetValue(getZ()	,0.			,-getX());
		v2->SetValue(-getY()	,getX()	,0.);
	}

	void	SetZero()
	{
		SetValue(pxReal(0.),pxReal(0.),pxReal(0.));
	}

	PX_INLINE bool isZero() const 
	{
		return m_floats[0] == pxReal(0) && m_floats[1] == pxReal(0) && m_floats[2] == pxReal(0);
	}

	PX_INLINE bool fuzzyZero() const 
	{
		return LengthSqr() < PX_EPSILON;
	}


	PX_INLINE pxVec3& AddScaled( const pxVec3& v, pxReal scale )
	{
		m_floats[0] += v.m_floats[0] * scale;
		m_floats[1] += v.m_floats[1] * scale;
		m_floats[2] += v.m_floats[2] * scale;
		return *this;
	}

	inline void dump() const {
		printf("%.2f %.2f %.2f\n",getX(),getY(),getZ());
	}

	// allow implicit casting to Vec3D
	FORCEINLINE operator const Vec3D& () const
	{
		return this->As_Vec3D();
	}
	FORCEINLINE const Vec3D& As_Vec3D() const
	{
		return *reinterpret_cast<const Vec3D*>(this);
	}
	static FORCEINLINE pxVec3 From_Vec3D( const Vec3D& v )
	{
		return pxVec3( v.x, v.y, v.z );
	}
};

MX_DECLARE_POD_TYPE(pxVec3);

/**@brief Return the sum of two vectors (Point symantics)*/
PX_INLINE pxVec3 
operator+( const pxVec3& v1, const pxVec3& v2 ) 
{
	return pxVec3(v1.m_floats[0] + v2.m_floats[0], v1.m_floats[1] + v2.m_floats[1], v1.m_floats[2] + v2.m_floats[2]);
}

/**@brief Return the elementwise product of two vectors */
PX_INLINE pxVec3 
operator*( const pxVec3& v1, const pxVec3& v2 ) 
{
	return pxVec3(v1.m_floats[0] * v2.m_floats[0], v1.m_floats[1] * v2.m_floats[1], v1.m_floats[2] * v2.m_floats[2]);
}

/**@brief Return the difference between two vectors */
PX_INLINE pxVec3 
operator-( const pxVec3& v1, const pxVec3& v2 )
{
	return pxVec3(v1.m_floats[0] - v2.m_floats[0], v1.m_floats[1] - v2.m_floats[1], v1.m_floats[2] - v2.m_floats[2]);
}
/**@brief Return the negative of the vector */
PX_INLINE pxVec3 
operator-( const pxVec3& v )
{
	return pxVec3(-v.m_floats[0], -v.m_floats[1], -v.m_floats[2]);
}

/**@brief Return the vector scaled by s */
PX_INLINE pxVec3 
operator*( const pxVec3& v,  pxReal s )
{
	return pxVec3(v.m_floats[0] * s, v.m_floats[1] * s, v.m_floats[2] * s);
}

/**@brief Return the vector scaled by s */
PX_INLINE pxVec3 
operator*( pxReal s, const pxVec3& v )
{ 
	return v * s; 
}

/**@brief Return the vector inversely scaled by s */
PX_INLINE pxVec3
operator/( const pxVec3& v,  pxReal s )
{
	AlwaysAssert(s != pxReal(0.0));
	return v * (pxReal(1.0) / s);
}

/**@brief Return the vector inversely scaled by s */
PX_INLINE pxVec3
operator/( const pxVec3& v1, const pxVec3& v2 )
{
	return pxVec3(v1.m_floats[0] / v2.m_floats[0],v1.m_floats[1] / v2.m_floats[1],v1.m_floats[2] / v2.m_floats[2]);
}

/**@brief Return the dot product between two vectors */
PX_INLINE pxReal 
pxDot( const pxVec3& v1, const pxVec3& v2 ) 
{ 
	return v1.Dot(v2); 
}


/**@brief Return the distance squared between two vectors */
PX_INLINE pxReal
pxDistance2( const pxVec3& v1, const pxVec3& v2 ) 
{ 
	return v1.distance2(v2); 
}


/**@brief Return the distance between two vectors */
PX_INLINE pxReal
pxDistance( const pxVec3& v1, const pxVec3& v2 ) 
{ 
	return v1.distance(v2); 
}

/**@brief Return the angle between two vectors */
PX_INLINE pxReal
pxAngle( const pxVec3& v1, const pxVec3& v2 ) 
{ 
	return v1.angle(v2); 
}

/**@brief Return the cross product of two vectors */
PX_INLINE pxVec3 
pxCross( const pxVec3& v1, const pxVec3& v2 ) 
{ 
	return v1.Cross(v2); 
}

PX_INLINE pxReal
pxTriple( const pxVec3& v1, const pxVec3& v2, const pxVec3& v3)
{
	return v1.triple(v2, v3);
}

/**@brief Return the linear interpolation between two vectors
 * @param v1 One vector 
 * @param v2 The other vector 
 * @param t The ration of this to v (t = 0 => return v1, t=1 => return v2) */
PX_INLINE pxVec3 
lerp( const pxVec3& v1, const pxVec3& v2,  pxReal t)
{
	return v1.lerp(v2, t);
}



PX_INLINE pxReal pxVec3::distance2( const pxVec3& v ) const
{
	return (v - *this).LengthSqr();
}

PX_INLINE pxReal pxVec3::distance( const pxVec3& v ) const
{
	return (v - *this).Length();
}

PX_INLINE pxVec3 pxVec3::normalized() const
{
	return *this / Length();
} 

PX_INLINE pxVec3 pxVec3::rotate( const pxVec3& wAxis, const pxReal angle ) const
{
	// wAxis must be a unit lenght vector

	pxVec3 o = wAxis * wAxis.Dot( *this );
	pxVec3 x = *this - o;
	pxVec3 y;

	y = wAxis.Cross( *this );

	return ( o + x * mxCos( angle ) + y * mxSin( angle ) );
}

/*
 * given a unit length "normal" vector n, generate vectors p and q vectors
 * that are an orthonormal basis for the plane space perpendicular to n.
 * i.e. this makes p,q such that n,p,q are all perpendicular to each other.
 * q will equal n x p. if n is not unit length then p will be unit length but
 * q wont be.
 */
template< class T >
PX_INLINE void TPlaneSpace( const T& n, T& p, T& q )
{
	if (mxFabs(n[2]) > MX_INV_SQRT_2) {
		// choose p in y-z plane
		pxReal a = n[1]*n[1] + n[2]*n[2];
		pxReal k = mxInvSqrt (a);
		p[0] = 0;
		p[1] = -n[2]*k;
		p[2] = n[1]*k;
		// set q = n x p
		q[0] = a*k;
		q[1] = -n[0]*p[2];
		q[2] = n[0]*p[1];
	}
	else {
		// choose p in x-y plane
		pxReal a = n[0]*n[0] + n[1]*n[1];
		pxReal k = mxInvSqrt (a);
		p[0] = -n[1]*k;
		p[1] = n[0]*k;
		p[2] = 0;
		// set q = n x p
		q[0] = -n[2]*p[1];
		q[1] = n[2]*p[0];
		q[2] = a*k;
	}
}

#endif // !__PX_Vector3D_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
