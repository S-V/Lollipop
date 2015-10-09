/*
=============================================================================
	File:	pxQuat.h
	Desc:	
=============================================================================
*/

#ifndef __PX_QUATERNION_H__
#define __PX_QUATERNION_H__

/**@brief The pxQuat implements quaternion to perform linear algebra rotations in combination with pxMat3x3, pxVec3 and pxTransform. */
class pxQuat : public pxQuadWord {
public:
  /**@brief No initialization constructor */
	pxQuat() {}

	//		template <typename pxReal>
	//		explicit Quaternion(const pxReal *v) : Tuple4<pxReal>(v) {}
  /**@brief Constructor from scalars */
	pxQuat(const pxReal& x, const pxReal& y, const pxReal& z, const pxReal& w) 
		: pxQuadWord(x, y, z, w) 
	{}
  /**@brief Axis angle Constructor
   * @param axis The axis which the rotation is around
   * @param angle The magnitude of the rotation around the angle (Radians) */
	pxQuat(const pxVec3& axis, pxReal angle) 
	{ 
		SetRotation(axis, angle); 
	}
  /**@brief Constructor from Euler angles
   * @param yaw Angle around Y unless BT_EULER_DEFAULT_ZYX defined then Z
   * @param pitch Angle around X unless BT_EULER_DEFAULT_ZYX defined then Y
   * @param roll Angle around Z unless BT_EULER_DEFAULT_ZYX defined then X */
	pxQuat( pxReal yaw, pxReal pitch, pxReal roll)
	{ 
#ifndef BT_EULER_DEFAULT_ZYX
		setEuler(yaw, pitch, roll); 
#else
		setEulerZYX(yaw, pitch, roll); 
#endif 
	}
  /**@brief Set the rotation using axis angle notation 
   * @param axis The axis around which to rotate
   * @param angle The magnitude of the rotation in Radians */
	void SetRotation(const pxVec3& axis, pxReal angle)
	{
		pxReal d = axis.Length();
		Assert(d != pxReal(0.0));
		pxReal s = mxSin(angle * pxReal(0.5)) / d;
		SetValue(axis.getX() * s, axis.getY() * s, axis.getZ() * s, 
			mxCos(angle * pxReal(0.5)));
	}
  /**@brief Set the quaternion using Euler angles
   * @param yaw Angle around Y
   * @param pitch Angle around X
   * @param roll Angle around Z */
	void setEuler( pxReal yaw, pxReal pitch, pxReal roll)
	{
		pxReal halfYaw = pxReal(yaw) * pxReal(0.5);  
		pxReal halfPitch = pxReal(pitch) * pxReal(0.5);  
		pxReal halfRoll = pxReal(roll) * pxReal(0.5);  
		pxReal cosYaw = mxCos(halfYaw);
		pxReal sinYaw = mxSin(halfYaw);
		pxReal cosPitch = mxCos(halfPitch);
		pxReal sinPitch = mxSin(halfPitch);
		pxReal cosRoll = mxCos(halfRoll);
		pxReal sinRoll = mxSin(halfRoll);
		SetValue(cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
			cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
			sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);
	}
  /**@brief Set the quaternion using euler angles 
   * @param yaw Angle around Z
   * @param pitch Angle around Y
   * @param roll Angle around X */
	void setEulerZYX( pxReal yaw, pxReal pitch, pxReal roll)
	{
		pxReal halfYaw = pxReal(yaw) * pxReal(0.5);  
		pxReal halfPitch = pxReal(pitch) * pxReal(0.5);  
		pxReal halfRoll = pxReal(roll) * pxReal(0.5);  
		pxReal cosYaw = mxCos(halfYaw);
		pxReal sinYaw = mxSin(halfYaw);
		pxReal cosPitch = mxCos(halfPitch);
		pxReal sinPitch = mxSin(halfPitch);
		pxReal cosRoll = mxCos(halfRoll);
		pxReal sinRoll = mxSin(halfRoll);
		SetValue(sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw, //x
                         cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw, //y
                         cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw, //z
                         cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw); //formerly yzx
	}
  /**@brief Add two quaternions
   * @param q The quaternion to add to this one */
	FORCEINLINE	pxQuat& operator+=(const pxQuat& q)
	{
		m_floats[0] += q.getX(); m_floats[1] += q.getY(); m_floats[2] += q.getZ(); m_floats[3] += q.m_floats[3];
		return *this;
	}

  /**@brief Supxract out a quaternion
   * @param q The quaternion to supxract from this one */
	pxQuat& operator-=(const pxQuat& q) 
	{
		m_floats[0] -= q.getX(); m_floats[1] -= q.getY(); m_floats[2] -= q.getZ(); m_floats[3] -= q.m_floats[3];
		return *this;
	}

  /**@brief Scale this quaternion
   * @param s The scalar to scale by */
	pxQuat& operator*=( pxReal s)
	{
		m_floats[0] *= s; m_floats[1] *= s; m_floats[2] *= s; m_floats[3] *= s;
		return *this;
	}

  /**@brief Multiply this quaternion by q on the right
   * @param q The other quaternion 
   * Equivilant to this = this * q */
	pxQuat& operator*=(const pxQuat& q)
	{
		SetValue(m_floats[3] * q.getX() + m_floats[0] * q.m_floats[3] + m_floats[1] * q.getZ() - m_floats[2] * q.getY(),
			m_floats[3] * q.getY() + m_floats[1] * q.m_floats[3] + m_floats[2] * q.getX() - m_floats[0] * q.getZ(),
			m_floats[3] * q.getZ() + m_floats[2] * q.m_floats[3] + m_floats[0] * q.getY() - m_floats[1] * q.getX(),
			m_floats[3] * q.m_floats[3] - m_floats[0] * q.getX() - m_floats[1] * q.getY() - m_floats[2] * q.getZ());
		return *this;
	}
  /**@brief Return the dot product between this quaternion and another
   * @param q The other quaternion */
	pxReal Dot(const pxQuat& q) const
	{
		return m_floats[0] * q.getX() + m_floats[1] * q.getY() + m_floats[2] * q.getZ() + m_floats[3] * q.m_floats[3];
	}

  /**@brief Return the length squared of the quaternion */
	pxReal LengthSqr() const
	{
		return Dot(*this);
	}

  /**@brief Return the length of the quaternion */
	pxReal GetLength() const
	{
		return mxSqrt(LengthSqr());
	}

  /**@brief Normalize the quaternion 
   * Such that x^2 + y^2 + z^2 +w^2 = 1 */
	pxQuat& Normalize() 
	{
		return *this /= GetLength();
	}

  /**@brief Return a scaled version of this quaternion
   * @param s The scale factor */
	FORCEINLINE pxQuat
	operator*( pxReal s) const
	{
		return pxQuat(getX() * s, getY() * s, getZ() * s, m_floats[3] * s);
	}


  /**@brief Return an inversely scaled versionof this quaternion
   * @param s The inverse scale factor */
	pxQuat operator/( pxReal s) const
	{
		Assert(s != pxReal(0.0));
		return *this * (pxReal(1.0) / s);
	}

  /**@brief Inversely scale this quaternion
   * @param s The scale factor */
	pxQuat& operator/=( pxReal s) 
	{
		Assert(s != pxReal(0.0));
		return *this *= pxReal(1.0) / s;
	}

  /**@brief Return a normalized version of this quaternion */
	pxQuat normalized() const 
	{
		return *this / GetLength();
	} 
  /**@brief Return the angle between this quaternion and the other 
   * @param q The other quaternion */
	pxReal angle(const pxQuat& q) const 
	{
		pxReal s = mxSqrt(LengthSqr() * q.LengthSqr());
		Assert(s != pxReal(0.0));
		return mxACos(Dot(q) / s);
	}
  /**@brief Return the angle of rotation represented by this quaternion */
	pxReal getAngle() const 
	{
		pxReal s = pxReal(2.) * mxACos(m_floats[3]);
		return s;
	}

	/**@brief Return the axis of the rotation represented by this quaternion */
	pxVec3 getAxis() const
	{
		pxReal s_squared = pxReal(1.) - mxPow(m_floats[3], pxReal(2.));
		if (s_squared < pxReal(10.) * PX_EPSILON) //Check for divide by zero
			return pxVec3(1.0, 0.0, 0.0);  // Arbitrary
		pxReal s = mxSqrt(s_squared);
		return pxVec3(m_floats[0] / s, m_floats[1] / s, m_floats[2] / s);
	}

	/**@brief Return the inverse of this quaternion */
	pxQuat inverse() const
	{
		return pxQuat(-m_floats[0], -m_floats[1], -m_floats[2], m_floats[3]);
	}

  /**@brief Return the sum of this quaternion and the other 
   * @param q2 The other quaternion */
	FORCEINLINE pxQuat
	operator+(const pxQuat& q2) const
	{
		const pxQuat& q1 = *this;
		return pxQuat(q1.getX() + q2.getX(), q1.getY() + q2.getY(), q1.getZ() + q2.getZ(), q1.m_floats[3] + q2.m_floats[3]);
	}

  /**@brief Return the difference between this quaternion and the other 
   * @param q2 The other quaternion */
	FORCEINLINE pxQuat
	operator-(const pxQuat& q2) const
	{
		const pxQuat& q1 = *this;
		return pxQuat(q1.getX() - q2.getX(), q1.getY() - q2.getY(), q1.getZ() - q2.getZ(), q1.m_floats[3] - q2.m_floats[3]);
	}

  /**@brief Return the negative of this quaternion 
   * This simply negates each element */
	FORCEINLINE pxQuat operator-() const
	{
		const pxQuat& q2 = *this;
		return pxQuat( - q2.getX(), - q2.getY(),  - q2.getZ(),  - q2.m_floats[3]);
	}
  /**@todo document this and it's use */
	FORCEINLINE pxQuat farthest( const pxQuat& qd) const 
	{
		pxQuat diff,sum;
		diff = *this - qd;
		sum = *this + qd;
		if( diff.Dot(diff) > sum.Dot(sum) )
			return qd;
		return (-qd);
	}

	/**@todo document this and it's use */
	FORCEINLINE pxQuat nearest( const pxQuat& qd) const 
	{
		pxQuat diff,sum;
		diff = *this - qd;
		sum = *this + qd;
		if( diff.Dot(diff) < sum.Dot(sum) )
			return qd;
		return (-qd);
	}


  /**@brief Return the quaternion which is the result of Spherical Linear Interpolation between this and the other quaternion
   * @param q The other quaternion to interpolate with 
   * @param t The ratio between this and q to interpolate.  If t = 0 the result is this, if t=1 the result is q.
   * Slerp interpolates assuming constant velocity.  */
	pxQuat slerp(const pxQuat& q, pxReal t) const
	{
		pxReal theta = angle(q);
		if (theta != pxReal(0.0))
		{
			pxReal d = pxReal(1.0) / mxSin(theta);
			pxReal s0 = mxSin((pxReal(1.0) - t) * theta);
			pxReal s1 = mxSin(t * theta);   
                        if (Dot(q) < 0) // Take care of long angle case see http://en.wikipedia.org/wiki/Slerp
                          return pxQuat((m_floats[0] * s0 + -q.getX() * s1) * d,
                                              (m_floats[1] * s0 + -q.getY() * s1) * d,
                                              (m_floats[2] * s0 + -q.getZ() * s1) * d,
                                              (m_floats[3] * s0 + -q.m_floats[3] * s1) * d);
                        else
                          return pxQuat((m_floats[0] * s0 + q.getX() * s1) * d,
                                              (m_floats[1] * s0 + q.getY() * s1) * d,
                                              (m_floats[2] * s0 + q.getZ() * s1) * d,
                                              (m_floats[3] * s0 + q.m_floats[3] * s1) * d);
                        
		}
		else
		{
			return *this;
		}
	}

	static const pxQuat&	GetIdentity()
	{
		static const pxQuat identityQuat(pxReal(0.),pxReal(0.),pxReal(0.),pxReal(1.));
		return identityQuat;
	}

	FORCEINLINE const pxReal& GetW() const { return m_floats[3]; }
};


/**@brief Return the negative of a quaternion */
FORCEINLINE pxQuat
operator-(const pxQuat& q)
{
	return pxQuat(-q.getX(), -q.getY(), -q.getZ(), -q.GetW());
}



/**@brief Return the product of two quaternions */
FORCEINLINE pxQuat
operator*(const pxQuat& q1, const pxQuat& q2) {
	return pxQuat(q1.GetW() * q2.getX() + q1.getX() * q2.GetW() + q1.getY() * q2.getZ() - q1.getZ() * q2.getY(),
		q1.GetW() * q2.getY() + q1.getY() * q2.GetW() + q1.getZ() * q2.getX() - q1.getX() * q2.getZ(),
		q1.GetW() * q2.getZ() + q1.getZ() * q2.GetW() + q1.getX() * q2.getY() - q1.getY() * q2.getX(),
		q1.GetW() * q2.GetW() - q1.getX() * q2.getX() - q1.getY() * q2.getY() - q1.getZ() * q2.getZ()); 
}

FORCEINLINE pxQuat
operator*(const pxQuat& q, const pxVec3& w)
{
	return pxQuat( q.GetW() * w.getX() + q.getY() * w.getZ() - q.getZ() * w.getY(),
		q.GetW() * w.getY() + q.getZ() * w.getX() - q.getX() * w.getZ(),
		q.GetW() * w.getZ() + q.getX() * w.getY() - q.getY() * w.getX(),
		-q.getX() * w.getX() - q.getY() * w.getY() - q.getZ() * w.getZ()); 
}

FORCEINLINE pxQuat
operator*(const pxVec3& w, const pxQuat& q)
{
	return pxQuat( w.getX() * q.GetW() + w.getY() * q.getZ() - w.getZ() * q.getY(),
		w.getY() * q.GetW() + w.getZ() * q.getX() - w.getX() * q.getZ(),
		w.getZ() * q.GetW() + w.getX() * q.getY() - w.getY() * q.getX(),
		-w.getX() * q.getX() - w.getY() * q.getY() - w.getZ() * q.getZ()); 
}

/**@brief Calculate the dot product between two quaternions */
FORCEINLINE pxReal 
Dot(const pxQuat& q1, const pxQuat& q2) 
{ 
	return q1.Dot(q2); 
}


/**@brief Return the length of a quaternion */
FORCEINLINE pxReal
GetLength(const pxQuat& q) 
{ 
	return q.GetLength(); 
}

/**@brief Return the angle between two quaternions*/
FORCEINLINE pxReal
angle(const pxQuat& q1, const pxQuat& q2) 
{ 
	return q1.angle(q2); 
}

/**@brief Return the inverse of a quaternion*/
FORCEINLINE pxQuat
inverse(const pxQuat& q) 
{
	return q.inverse();
}

/**@brief Return the result of spherical linear interpolation betwen two quaternions 
 * @param q1 The first quaternion
 * @param q2 The second quaternion 
 * @param t The ration between q1 and q2.  t = 0 return q1, t=1 returns q2 
 * Slerp assumes constant velocity between positions. */
FORCEINLINE pxQuat
slerp(const pxQuat& q1, const pxQuat& q2, const pxReal& t) 
{
	return q1.slerp(q2, t);
}

FORCEINLINE pxVec3 
quatRotate(const pxQuat& rotation, const pxVec3& v) 
{
	pxQuat q = rotation * v;
	q *= rotation.inverse();
	return pxVec3(q.getX(),q.getY(),q.getZ());
}

FORCEINLINE pxQuat 
shortestArcQuat(const pxVec3& v0, const pxVec3& v1) // Game Programming Gems 2.10. make sure v0,v1 are normalized
{
	pxVec3 c = v0.Cross(v1);
	pxReal  d = v0.Dot(v1);

	if (d < -1.0 + PX_EPSILON)
	{
		pxVec3 n,unused;
		TPlaneSpace(v0,n,unused);
		return pxQuat(n.getX(),n.getY(),n.getZ(),0.0f); // just pick any vector that is orthogonal to v0
	}

	pxReal  s = mxSqrt((1.0f + d) * 2.0f);
	pxReal rs = 1.0f / s;

	return pxQuat(c.getX()*rs,c.getY()*rs,c.getZ()*rs,s * 0.5f);
}

FORCEINLINE pxQuat 
shortestArcQuatNormalize2(pxVec3& v0,pxVec3& v1)
{
	v0.Normalize();
	v1.Normalize();
	return shortestArcQuat(v0,v1);
}

#endif // !__PX_QUATERNION_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
