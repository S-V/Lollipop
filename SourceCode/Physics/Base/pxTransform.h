/*
=============================================================================
	File:	pxTransform.h
	Desc:	Rigid body transform.
	A convenient structure for positioning objects in a 3D world.
	Contains a 3 by 3 Matrix for the rotation and a Vec3D for the translation.
	It is assumed that the matrix only contains rotation.
=============================================================================
*/

#ifndef __PX_TRANSFORM_H__
#define __PX_TRANSFORM_H__

#include <Physics/Base/pxMat3x3.h>

MX_SWIPED("Bullet/Solid")
/**@brief The pxTransform class supports rigid transforms with only translation and rotation and no scaling/shear.
 *It can be used in combination with pxVec3, pxQuat and pxMat3x3 linear algebra classes. */
MX_ALIGN_16(class) pxTransform
{
	pxVec3		mOrigin;
	pxMat3x3	mBasis;

public:

  /**@brief No initialization constructor */
	pxTransform() {}
  /**@brief Constructor from pxQuat (optional pxVec3 )
   * @param q Rotation from quaternion 
   * @param c Translation from Vector (default 0,0,0) */
	explicit FORCEINLINE pxTransform( const pxQuat& q, 
		const pxVec3& c = pxVec3(pxReal(0.), pxReal(0.), pxReal(0.)) )
		: mBasis(q), mOrigin(c)
	{}

  /**@brief Constructor from pxMat3x3 (optional pxVec3)
   * @param b Rotation from Matrix 
   * @param c Translation from Vector default (0,0,0)*/
	explicit FORCEINLINE pxTransform(const pxMat3x3& b, 
		const pxVec3& c = pxVec3(pxReal(0), pxReal(0), pxReal(0)))
		: mBasis(b), mOrigin(c)
	{}
  /**@brief Copy constructor */
	FORCEINLINE pxTransform (const pxTransform& other)
		: mBasis(other.mBasis),
		mOrigin(other.mOrigin)
	{
	}
  /**@brief Assignment Operator */
	FORCEINLINE pxTransform& operator=(const pxTransform& other)
	{
		mBasis = other.mBasis;
		mOrigin = other.mOrigin;
		return *this;
	}


  /**@brief Set the current transform as the value of the product of two transforms
   * @param t1 Transform 1
   * @param t2 Transform 2
   * This = Transform1 * Transform2 */
		FORCEINLINE void mult(const pxTransform& t1, const pxTransform& t2) {
			mBasis = t1.mBasis * t2.mBasis;
			mOrigin = t1(t2.mOrigin);
		}

/*		void multInverseLeft(const pxTransform& t1, const pxTransform& t2) {
			pxVec3 v = t2.mOrigin - t1.mOrigin;
			mBasis = pxMultTransposeLeft(t1.mBasis, t2.mBasis);
			mOrigin = v * t1.mBasis;
		}
		*/

	FORCEINLINE pxVec3 TransformPoint( const pxVec3& p ) const
	{
		return pxVec3(
			mBasis[0].Dot( p ) + mOrigin.getX(),
			mBasis[1].Dot( p ) + mOrigin.getY(),
			mBasis[2].Dot( p ) + mOrigin.getZ()
		);
	}
/**@brief Return the transform of the vector */
	FORCEINLINE pxVec3 operator() ( const pxVec3& x ) const
	{
		return TransformPoint( x );
	}

  /**@brief Return the transform of the vector */
	FORCEINLINE pxVec3 operator * ( const pxVec3& x ) const
	{
		return (*this)(x);
	}

  /**@brief Return the transform of the pxQuat */
	FORCEINLINE pxQuat operator*(const pxQuat& q) const
	{
		return GetRotation() * q;
	}

  /**@brief Return the basis matrix for the rotation */
	FORCEINLINE pxMat3x3&       GetBasis()          { return mBasis; }
  /**@brief Return the basis matrix for the rotation */
	FORCEINLINE const pxMat3x3& GetBasis()    const { return mBasis; }

  /**@brief Return the origin vector translation */
	FORCEINLINE pxVec3&         GetOrigin()         { return mOrigin; }
  /**@brief Return the origin vector translation */
	FORCEINLINE const pxVec3&   GetOrigin()   const { return mOrigin; }

  /**@brief Return a quaternion representing the rotation */
	pxQuat GetRotation() const { 
		pxQuat q;
		mBasis.GetRotation(q);
		return q;
	}
	
  /**@brief Set the translational element
   * @param origin The vector to set the translation to */
	FORCEINLINE void SetOrigin(const pxVec3& origin) 
	{ 
		mOrigin = origin;
	}

	FORCEINLINE pxVec3 invXform(const pxVec3& inVec) const;


  /**@brief Set the rotational element by pxMat3x3 */
	FORCEINLINE void setBasis(const pxMat3x3& basis)
	{ 
		mBasis = basis;
	}

  /**@brief Set the rotational element by pxQuat */
	FORCEINLINE void SetRotation(const pxQuat& q)
	{
		mBasis.SetRotation(q);
	}


  /**@brief Set this transformation to the identity */
	void SetIdentity()
	{
		mBasis.SetIdentity();
		mOrigin.SetValue(pxReal(0.0), pxReal(0.0), pxReal(0.0));
	}

  /**@brief Multiply this Transform by another(this = this * another) 
   * @param t The other transform */
	pxTransform& operator*=(const pxTransform& t) 
	{
		mOrigin += mBasis * t.mOrigin;
		mBasis *= t.mBasis;
		return *this;
	}

  /**@brief Return the inverse of this transform */
	pxTransform inverse() const
	{ 
		pxMat3x3 inv = mBasis.GetTranspose();
		return pxTransform(inv, inv * -mOrigin);
	}

  /**@brief Return the inverse of this transform times the other transform
   * @param t The other transform 
   * return this.inverse() * the other */
	pxTransform inverseTimes(const pxTransform& t) const;  

  /**@brief Return the product of this transform and the other */
	pxTransform operator*(const pxTransform& t) const;

  /**@brief Return an identity transform */
	static const pxTransform & GetIdentity()
	{
		static const pxTransform identityTransform(pxMat3x3::GetIdentity());
		return identityTransform;
	}

#if 0
	FORCEINLINE pxMat4x4 ToMat4() const {
		return pxMat4x4(
			mBasis[0].x, mBasis[0].y, mBasis[0].z, 0.0f,
			mBasis[1].x, mBasis[1].y, mBasis[1].z, 0.0f,
			mBasis[2].x, mBasis[2].y, mBasis[2].z, 0.0f,
			mOrigin.x,   mOrigin.y,   mOrigin.z,   1.0f
		);
	}
#else
	FORCEINLINE float4x4 ToMat4() const
	{
		return XMMATRIX(
			mBasis[0].mVec128,
			mBasis[1].mVec128,
			mBasis[2].mVec128,
			mOrigin.mVec128
			);
	}
#endif

	FORCEINLINE bool IsIdentity() const
	{
		float4x4 m = this->ToMat4();
		m.m[3][3] = 1.0f;
		return XMMatrixIsIdentity(m);
	}

	/**@brief Set from an array 
	* @param m A pointer to a 15 element array (12 rotation(row major padded on the right by 1), and 3 translation */
	void SetFromOpenGLMatrix( const pxReal *m )
	{
		mBasis.SetFromOpenGLSubMatrix( m );
		mOrigin.SetValue( m[12], m[13], m[14] );
	}

	/**@brief Fill an array representation
	* @param m A pointer to a 15 element array (12 rotation(row major padded on the right by 1), and 3 translation */
	void GetOpenGLMatrix( pxReal *m ) const 
	{
		mBasis.GetOpenGLSubMatrix( m );
		m[12] = mOrigin.x;
		m[13] = mOrigin.y;
		m[14] = mOrigin.z;
		m[15] = pxReal(1.0);
	}
};


FORCEINLINE pxVec3
pxTransform::invXform(const pxVec3& inVec) const
{
	pxVec3 v = inVec - mOrigin;
	return (mBasis.GetTranspose() * v);
}

FORCEINLINE pxTransform 
pxTransform::inverseTimes(const pxTransform& t) const  
{
	pxVec3 v = t.GetOrigin() - mOrigin;
		return pxTransform(mBasis.transposeTimes(t.mBasis),
			v * mBasis);
}

FORCEINLINE pxTransform 
pxTransform::operator*(const pxTransform& t) const
{
	return pxTransform(mBasis * t.mBasis, 
		(*this)(t.mOrigin));
}

/**@brief Test if two transforms have all elements equal */
FORCEINLINE bool operator==(const pxTransform& t1, const pxTransform& t2)
{
   return ( t1.GetBasis()  == t2.GetBasis() &&
            t1.GetOrigin() == t2.GetOrigin() );
}

/// Utils related to temporal transforms
class pxTransformUtil
{

public:

	static void integrateTransform(const pxTransform& curTrans,const pxVec3& linvel,const pxVec3& angvel,pxReal timeStep,pxTransform& predictedTransform)
	{
		predictedTransform.SetOrigin(curTrans.GetOrigin() + linvel * timeStep);
//	#define QUATERNION_DERIVATIVE
	#ifdef QUATERNION_DERIVATIVE
		pxQuat predictedOrn = curTrans.GetRotation();
		predictedOrn += (angvel * predictedOrn) * (timeStep * pxReal(0.5));
		predictedOrn.Normalize();
	#else
		//Exponential map
		//google for "Practical Parameterization of Rotations Using the Exponential Map", F. Sebastian Grassia

		pxVec3 axis;
		pxReal	fAngle = angvel.Length(); 
		//limit the angular motion
		if (fAngle*timeStep > ANGULAR_MOTION_THRESHOLD)
		{
			fAngle = ANGULAR_MOTION_THRESHOLD / timeStep;
		}

		if ( fAngle < pxReal(0.001) )
		{
			// use Taylor's expansions of sync function
			axis   = angvel*( pxReal(0.5)*timeStep-(timeStep*timeStep*timeStep)*(pxReal(0.020833333333))*fAngle*fAngle );
		}
		else
		{
			// sync(fAngle) = sin(c*fAngle)/t
			axis   = angvel*( mxSin(pxReal(0.5)*fAngle*timeStep)/fAngle );
		}
		pxQuat dorn (axis.getX(),axis.getY(),axis.getZ(),mxCos( fAngle*timeStep*pxReal(0.5) ));
		pxQuat orn0 = curTrans.GetRotation();

		pxQuat predictedOrn = dorn * orn0;
		predictedOrn.Normalize();
	#endif
		predictedTransform.SetRotation(predictedOrn);
	}

	static void	calculateVelocityQuaternion(const pxVec3& pos0,const pxVec3& pos1,const pxQuat& orn0,const pxQuat& orn1,pxReal timeStep,pxVec3& linVel,pxVec3& angVel)
	{
		linVel = (pos1 - pos0) / timeStep;
		pxVec3 axis;
		pxReal  angle;
		if (orn0 != orn1)
		{
			calculateDiffAxisAngleQuaternion(orn0,orn1,axis,angle);
			angVel = axis * angle / timeStep;
		} else
		{
			angVel.SetValue(0,0,0);
		}
	}

	static void calculateDiffAxisAngleQuaternion(const pxQuat& orn0,const pxQuat& orn1a,pxVec3& axis,pxReal& angle)
	{
		pxQuat orn1 = orn0.nearest(orn1a);
		pxQuat dorn = orn1 * orn0.inverse();
		angle = dorn.getAngle();
		axis = pxVec3(dorn.getX(),dorn.getY(),dorn.getZ());
		axis[3] = pxReal(0.);
		//check for axis length
		pxReal len = axis.LengthSqr();
		if (len < PX_EPSILON*PX_EPSILON)
			axis = pxVec3(pxReal(1.),pxReal(0.),pxReal(0.));
		else
			axis /= mxSqrt(len);
	}

	static void	calculateVelocity(const pxTransform& transform0,const pxTransform& transform1,pxReal timeStep,pxVec3& linVel,pxVec3& angVel)
	{
		linVel = (transform1.GetOrigin() - transform0.GetOrigin()) / timeStep;
		pxVec3 axis;
		pxReal  angle;
		calculateDiffAxisAngle(transform0,transform1,axis,angle);
		angVel = axis * angle / timeStep;
	}

	static void calculateDiffAxisAngle(const pxTransform& transform0,const pxTransform& transform1,pxVec3& axis,pxReal& angle)
	{
		pxMat3x3 dmat = transform1.GetBasis() * transform0.GetBasis().GetInverse();
		pxQuat dorn;
		dmat.GetRotation(dorn);

		///floating point inaccuracy can lead to w component > 1..., which breaks 
		dorn.Normalize();
		
		angle = dorn.getAngle();
		axis = pxVec3(dorn.getX(),dorn.getY(),dorn.getZ());
		axis[3] = pxReal(0.);
		//check for axis length
		pxReal len = axis.LengthSqr();
		if (len < PX_EPSILON*PX_EPSILON)
			axis = pxVec3(pxReal(1.),pxReal(0.),pxReal(0.));
		else
			axis /= mxSqrt(len);
	}

};



#endif // !__PX_TRANSFORM_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
