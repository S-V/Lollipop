/*
=============================================================================
	File:	pxMat3x3.h
	Desc:	3x3 matrix, row-major.
=============================================================================
*/

#ifndef __PX_Matrix3x3_H__
#define __PX_Matrix3x3_H__

#include <Physics/Base/pxVec3.h>
#include <Physics/Base/pxQuat.h>

MX_SWIPED("Bullet/Solid")
/*
=======================================================================
	
	pxMat3x3
	
	3x3 matrix, row-major
=======================================================================
*/
struct pxMat3x3
{
	pxVec3	mRows[3];

public:
	FORCEINLINE pxMat3x3()
	{}

	FORCEINLINE explicit pxMat3x3( const pxQuat& q )
	{
		SetRotation(q);
	}

	FORCEINLINE pxMat3x3( const pxVec3& row0,
		const pxVec3& row1, const pxVec3& row2 )
	{
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
	}

	FORCEINLINE pxMat3x3(
		const pxReal& xx, const pxReal& xy, const pxReal& xz,
		const pxReal& yx, const pxReal& yy, const pxReal& yz,
		const pxReal& zx, const pxReal& zy, const pxReal& zz)
	{ 
		SetValue(
			xx, xy, xz,
			yx, yy, yz,
			zx, zy, zz
		);
	}

	FORCEINLINE pxMat3x3( const pxMat3x3& other )
	{
		mRows[0] = other.mRows[0];
		mRows[1] = other.mRows[1];
		mRows[2] = other.mRows[2];
	}

	FORCEINLINE pxMat3x3& operator = ( const pxMat3x3& other )
	{
		mRows[0] = other.mRows[0];
		mRows[1] = other.mRows[1];
		mRows[2] = other.mRows[2];
		return *this;
	}

	/** @brief Set the values of the matrix explicitly (row major)
	*  @param xx Top left
	*  @param xy Top Middle
	*  @param xz Top Right
	*  @param yx Middle Left
	*  @param yy Middle Middle
	*  @param yz Middle Right
	*  @param zx Bottom Left
	*  @param zy Bottom Middle
	*  @param zz Bottom Right
	*/
	FORCEINLINE void SetValue(
		const pxReal& xx, const pxReal& xy, const pxReal& xz, 
		const pxReal& yx, const pxReal& yy, const pxReal& yz, 
		const pxReal& zx, const pxReal& zy, const pxReal& zz )
	{
		mRows[0].SetValue( xx, xy, xz );
		mRows[1].SetValue( yx, yy, yz );
		mRows[2].SetValue( zx, zy, zz );
	}

	/**@brief Set the matrix to the identity */
	FORCEINLINE void SetIdentity()
	{
		SetValue(
			pxReal(1.0), pxReal(0.0), pxReal(0.0),
			pxReal(0.0), pxReal(1.0), pxReal(0.0),
			pxReal(0.0), pxReal(0.0), pxReal(1.0)
		); 
	}

	FORCEINLINE void SetZero()
	{
		SetValue(
			pxReal(0.0), pxReal(0.0), pxReal(0.0),
			pxReal(0.0), pxReal(0.0), pxReal(0.0),
			pxReal(0.0), pxReal(0.0), pxReal(0.0)
		); 
	}

	FORCEINLINE static const pxMat3x3&	GetIdentity()
	{
		static const pxMat3x3 identityMatrix(pxReal(1.0), pxReal(0.0), pxReal(0.0), 
			pxReal(0.0), pxReal(1.0), pxReal(0.0), 
			pxReal(0.0), pxReal(0.0), pxReal(1.0));
		return identityMatrix;
	}

	/** @brief Get a column of the matrix as a vector 
	*  @param i Column number 0 indexed */
	FORCEINLINE pxVec3 GetColumn( int i ) const
	{
		return pxVec3( mRows[0][i], mRows[1][i], mRows[2][i] );
	}

	/** @brief Get a row of the matrix as a vector 
	*  @param i Row number 0 indexed */
	FORCEINLINE const pxVec3& GetRow( int i ) const
	{
		Assert( 0 <= i && i < 3 );
		return mRows[i];
	}

	/** @brief Get a mutable reference to a row of the matrix as a vector 
	*  @param i Row number 0 indexed */
	FORCEINLINE pxVec3& operator[] ( int i )
	{ 
		Assert( 0 <= i && i < 3 );
		return mRows[i]; 
	}

	/** @brief Get a const reference to a row of the matrix as a vector 
	*  @param i Row number 0 indexed */
	FORCEINLINE const pxVec3& operator[](int i) const
	{
		Assert( 0 <= i && i < 3 );
		return mRows[i]; 
	}

	FORCEINLINE pxReal* ToPtr() {
		return (pxReal*)this;
	}
	FORCEINLINE const pxReal* ToPtr() const {
		return (const pxReal*)this;
	}

	/**@brief Create a scaled copy of the matrix 
	* @param s Scaling vector The elements of the vector will scale each column
	*/
	FORCEINLINE pxMat3x3 GetScaled( const pxVec3& s ) const {
		return pxMat3x3(
			mRows[0] * s.getX(),
			mRows[1] * s.getY(),
			mRows[2] * s.getZ()
		);
	}

	/**@brief Return the matrix with all values non negative
	*/
	FORCEINLINE pxMat3x3 GetAbsolute() const {
		return pxMat3x3(
			mxFabs(mRows[0].getX()), mxFabs(mRows[0].getY()), mxFabs(mRows[0].getZ()),
			mxFabs(mRows[1].getX()), mxFabs(mRows[1].getY()), mxFabs(mRows[1].getZ()),
			mxFabs(mRows[2].getX()), mxFabs(mRows[2].getY()), mxFabs(mRows[2].getZ())
		);
	}

	FORCEINLINE pxVec3 GetDiagonal() const
	{
		return pxVec3(
			mRows[0][0],
			mRows[1][1],
			mRows[2][2]
			);
	}

	/**@brief Return the transpose of the matrix
	*/
	FORCEINLINE pxMat3x3 GetTranspose() const {
		return pxMat3x3(
			mRows[0].getX(), mRows[1].getX(), mRows[2].getX(),
			mRows[0].getY(), mRows[1].getY(), mRows[2].getY(),
			mRows[0].getZ(), mRows[1].getZ(), mRows[2].getZ()
		);
	}

	/**@brief Calculate the matrix cofactor 
	* @param r1 The first row to use for calculating the cofactor
	* @param c1 The first column to use for calculating the cofactor
	* @param r1 The second row to use for calculating the cofactor
	* @param c1 The second column to use for calculating the cofactor
	* See http://en.wikipedia.org/wiki/Cofactor_(linear_algebra) for more details
	*/
	pxReal Cofactor( int r1, int c1, int r2, int c2 ) const 
	{
		return mRows[r1][c1] * mRows[r2][c2] - mRows[r1][c2] * mRows[r2][c1];
	}

	/**@brief Return the determinant of the matrix */
	pxReal GetDeterminant() const;
	MX_TEMP

	/**@brief Return the adjoint of the matrix */
	pxMat3x3 GetAdjoint() const
	{
		return pxMat3x3(
			Cofactor( 1, 1, 2, 2 ), Cofactor( 0, 2, 2, 1 ), Cofactor( 0, 1, 1, 2 ),
			Cofactor( 1, 2, 2, 0 ), Cofactor( 0, 0, 2, 2 ), Cofactor( 0, 2, 1, 0 ),
			Cofactor( 1, 0, 2, 1 ), Cofactor( 0, 1, 2, 0 ), Cofactor( 0, 0, 1, 1 )
		);
	}

	/**@brief Return the inverse of the matrix */
	pxMat3x3 GetInverse() const
	{
		pxVec3 co(Cofactor( 1, 1, 2, 2 ), Cofactor( 1, 2, 2, 0 ), Cofactor( 1, 0, 2, 1 ));
		pxReal det = (*this)[0].Dot(co);
		AlwaysAssert(det != pxReal(0.0));
		pxReal s = pxReal(1.0) / det;
		return pxMat3x3(
			co.getX() * s, Cofactor( 0, 2, 2, 1 ) * s, Cofactor( 0, 1, 1, 2 ) * s,
			co.getY() * s, Cofactor( 0, 0, 2, 2 ) * s, Cofactor( 0, 2, 1, 0 ) * s,
			co.getZ() * s, Cofactor( 0, 1, 2, 0 ) * s, Cofactor( 0, 0, 1, 1 ) * s
		);
	}

	FORCEINLINE pxReal tdotx(const pxVec3& v) const 
	{
		return mRows[0].getX() * v.getX() + mRows[1].getX() * v.getY() + mRows[2].getX() * v.getZ();
	}
	FORCEINLINE pxReal tdoty(const pxVec3& v) const 
	{
		return mRows[0].getY() * v.getX() + mRows[1].getY() * v.getY() + mRows[2].getY() * v.getZ();
	}
	FORCEINLINE pxReal tdotz(const pxVec3& v) const 
	{
		return mRows[0].getZ() * v.getX() + mRows[1].getZ() * v.getY() + mRows[2].getZ() * v.getZ();
	}

	FORCEINLINE bool IsDiagonal( const FLOAT epsilon = MATRIX_EPSILON ) const
	{
		if( mxFabs( mRows[0][1] ) > epsilon ||
			mxFabs( mRows[0][2] ) > epsilon ||
			mxFabs( mRows[1][0] ) > epsilon ||
			mxFabs( mRows[1][2] ) > epsilon ||
			mxFabs( mRows[2][0] ) > epsilon ||
			mxFabs( mRows[2][1] ) > epsilon )
		{
				return false;
		}
		return true;
	}

	/**@brief Equality operator between two matrices
	* It will test all elements are equal.
	*/
	FORCEINLINE friend bool operator == ( const pxMat3x3& m1, const pxMat3x3& m2 )
	{
		return 
			m1[0][0] == m2[0][0] && m1[1][0] == m2[1][0] && m1[2][0] == m2[2][0] &&
			m1[0][1] == m2[0][1] && m1[1][1] == m2[1][1] && m1[2][1] == m2[2][1] &&
			m1[0][2] == m2[0][2] && m1[1][2] == m2[1][2] && m1[2][2] == m2[2][2];
	}


	/** @brief Multiply by the target matrix on the right
	*  @param m Rotation matrix to be applied 
	* Equivalent to this = this * m
	*/
	pxMat3x3& operator *= ( const pxMat3x3& m );
	

	pxMat3x3 transposeTimes(const pxMat3x3& m) const;
	pxMat3x3 timesTranspose(const pxMat3x3& m) const;


	// post-multiplication with vector
	//                   
	//               V | v1
	//                 | v2
	//                 | v3            
	//     -----------------
	//  A  a11 a12 a13 | c1
	//     a21 a22 a23 | c2
	//     a31 a32 a33 | c3
	//
	// c1 = a11*v1.x + a12*v1.y + a13*v1.z
	// c1 = a21*v2.x + a22*v2.y + a23*v2.z
	// c1 = a31*v3.x + a32*v3.y + a33*v3.z
	//
	FORCEINLINE friend pxVec3 operator * ( const pxMat3x3& m, const pxVec3& v )
	{
		return pxVec3(m[0].Dot(v), m[1].Dot(v), m[2].Dot(v));
	}

	// pre-multiplication with vector
	FORCEINLINE friend pxVec3 operator * (const pxVec3& v, const pxMat3x3& m )
	{
		return pxVec3(m.tdotx(v), m.tdoty(v), m.tdotz(v));
	}
	FORCEINLINE friend pxMat3x3 operator * ( const pxMat3x3& m1, const pxMat3x3& m2 )
	{
		return pxMat3x3(
			m2.tdotx(m1[0]), m2.tdoty(m1[0]), m2.tdotz(m1[0]),
			m2.tdotx(m1[1]), m2.tdoty(m1[1]), m2.tdotz(m1[1]),
			m2.tdotx(m1[2]), m2.tdoty(m1[2]), m2.tdotz(m1[2])
		);
	}

	FORCEINLINE pxVec3 RotatePoint( const pxVec3& v )
	{
		return pxVec3(
			mRows[0].Dot(v),
			mRows[1].Dot(v),
			mRows[2].Dot(v)
		);
	}

	/** @brief Set the matrix from a quaternion
	*  @param q The Quaternion to match */  
	FORCEINLINE void SetRotation(const pxQuat& q) 
	{
		pxReal d = q.LengthSqr();
		AlwaysAssert(d != pxReal(0.0));
		pxReal s = pxReal(2.0) / d;
		pxReal xs = q.getX() * s,   ys = q.getY() * s,   zs = q.getZ() * s;
		pxReal wx = q.GetW() * xs,  wy = q.GetW() * ys,  wz = q.GetW() * zs;
		pxReal xx = q.getX() * xs,  xy = q.getX() * ys,  xz = q.getX() * zs;
		pxReal yy = q.getY() * ys,  yz = q.getY() * zs,  zz = q.getZ() * zs;
		SetValue(
			pxReal(1.0) - (yy + zz), xy - wz, xz + wy,
			xy + wz, pxReal(1.0) - (xx + zz), yz - wx,
			xz - wy, yz + wx, pxReal(1.0) - (xx + yy)
		);
	}

	/**@brief Get the matrix represented as a quaternion 
	* @param q The quaternion which will be set */
	void GetRotation( pxQuat& q ) const
	{
		pxReal trace = mRows[0].getX() + mRows[1].getY() + mRows[2].getZ();
		pxReal temp[4];

		if( trace > pxReal(0.0) )
		{
			pxReal s = mxSqrt(trace + pxReal(1.0));
			temp[3]=(s * pxReal(0.5));
			s = pxReal(0.5) / s;

			temp[0]=((mRows[2].getY() - mRows[1].getZ()) * s);
			temp[1]=((mRows[0].getZ() - mRows[2].getX()) * s);
			temp[2]=((mRows[1].getX() - mRows[0].getY()) * s);
		} 
		else 
		{
			int i = mRows[0].getX() < mRows[1].getY() ? 
				(mRows[1].getY() < mRows[2].getZ() ? 2 : 1) :
				(mRows[0].getX() < mRows[2].getZ() ? 2 : 0); 
			int j = (i + 1) % 3;  
			int k = (i + 2) % 3;

			pxReal s = mxSqrt(mRows[i][i] - mRows[j][j] - mRows[k][k] + pxReal(1.0));
			temp[i] = s * pxReal(0.5);
			s = pxReal(0.5) / s;

			temp[3] = (mRows[k][j] - mRows[j][k]) * s;
			temp[j] = (mRows[j][i] + mRows[i][j]) * s;
			temp[k] = (mRows[k][i] + mRows[i][k]) * s;
		}
		q.SetValue( temp[0],temp[1],temp[2],temp[3] );
	}

	/**@brief Get the matrix represented as euler angles around YXZ, roundtrip with setEulerYPR
	* @param yaw Yaw around Y axis
	* @param pitch Pitch around X axis
	* @param roll around Z axis */	
	void getEulerYPR( pxReal& yaw, pxReal& pitch, pxReal& roll ) const
	{
		// first use the normal calculus
		yaw = pxReal(mxATan2(mRows[1].getX(), mRows[0].getX()));
		pitch = pxReal(mxASin(-mRows[2].getX()));
		roll = pxReal(mxATan2(mRows[2].getY(), mRows[2].getZ()));

		// on pitch = +/-HalfPI
		if (mxFabs(pitch)==MX_HALF_PI)
		{
			if (yaw>0)
				yaw-=MX_PI;
			else
				yaw+=MX_PI;

			if (roll>0)
				roll-=MX_PI;
			else
				roll+=MX_PI;
		}
	};


	/**@brief Get the matrix represented as euler angles around ZYX
	* @param yaw Yaw around X axis
	* @param pitch Pitch around Y axis
	* @param roll around X axis 
	* @param solution_number Which solution of two possible solutions ( 1 or 2) are possible values*/	
	void getEulerZYX(pxReal& yaw, pxReal& pitch, pxReal& roll, unsigned int solution_number = 1) const
	{
		struct Euler
		{
			pxReal yaw;
			pxReal pitch;
			pxReal roll;
		};

		Euler euler_out;
		Euler euler_out2; //second solution
		//get the pointer to the raw data

		// Check that pitch is not at a singularity
		if (mxFabs(mRows[2].getX()) >= 1)
		{
			euler_out.yaw = 0;
			euler_out2.yaw = 0;

			// From difference of angles formula
			pxReal delta = mxATan2(mRows[0].getX(),mRows[0].getZ());
			if (mRows[2].getX() > 0)  //gimbal locked up
			{
				euler_out.pitch = MX_PI / pxReal(2.0);
				euler_out2.pitch = MX_PI / pxReal(2.0);
				euler_out.roll = euler_out.pitch + delta;
				euler_out2.roll = euler_out.pitch + delta;
			}
			else // gimbal locked down
			{
				euler_out.pitch = -MX_PI / pxReal(2.0);
				euler_out2.pitch = -MX_PI / pxReal(2.0);
				euler_out.roll = -euler_out.pitch + delta;
				euler_out2.roll = -euler_out.pitch + delta;
			}
		}
		else
		{
			euler_out.pitch = - mxASin(mRows[2].getX());
			euler_out2.pitch = MX_PI - euler_out.pitch;

			euler_out.roll = mxATan2(mRows[2].getY()/mxCos(euler_out.pitch), 
				mRows[2].getZ()/mxCos(euler_out.pitch));
			euler_out2.roll = mxATan2(mRows[2].getY()/mxCos(euler_out2.pitch), 
				mRows[2].getZ()/mxCos(euler_out2.pitch));

			euler_out.yaw = mxATan2(mRows[1].getX()/mxCos(euler_out.pitch), 
				mRows[0].getX()/mxCos(euler_out.pitch));
			euler_out2.yaw = mxATan2(mRows[1].getX()/mxCos(euler_out2.pitch), 
				mRows[0].getX()/mxCos(euler_out2.pitch));
		}

		if (solution_number == 1)
		{
			yaw = euler_out.yaw; 
			pitch = euler_out.pitch;
			roll = euler_out.roll;
		}
		else
		{
			yaw = euler_out2.yaw;
			pitch = euler_out2.pitch;
			roll = euler_out2.roll;
		}
	}

	/**@brief Fill the values of the matrix into a 9 element array 
	* @param m The array to be filled */
	void GetOpenGLSubMatrix( pxReal *m ) const 
	{
		m[0]  = pxReal(mRows[0].x); 
		m[1]  = pxReal(mRows[1].x);
		m[2]  = pxReal(mRows[2].x);
		m[3]  = pxReal(0.0); 
		m[4]  = pxReal(mRows[0].y);
		m[5]  = pxReal(mRows[1].y);
		m[6]  = pxReal(mRows[2].y);
		m[7]  = pxReal(0.0); 
		m[8]  = pxReal(mRows[0].z); 
		m[9]  = pxReal(mRows[1].z);
		m[10] = pxReal(mRows[2].z);
		m[11] = pxReal(0.0); 
	}
	/** @brief Set from a carray of btScalars 
	*  @param m A pointer to the beginning of an array of 9 btScalars */
	void SetFromOpenGLSubMatrix( const pxReal *m )
	{
		mRows[0].SetValue(m[0],m[4],m[8]);
		mRows[1].SetValue(m[1],m[5],m[9]);
		mRows[2].SetValue(m[2],m[6],m[10]);
	}
};

FORCEINLINE pxMat3x3 
pxMat3x3::transposeTimes(const pxMat3x3& m) const
{
	return pxMat3x3(
		mRows[0].getX() * m[0].getX() + mRows[1].getX() * m[1].getX() + mRows[2].getX() * m[2].getX(),
		mRows[0].getX() * m[0].getY() + mRows[1].getX() * m[1].getY() + mRows[2].getX() * m[2].getY(),
		mRows[0].getX() * m[0].getZ() + mRows[1].getX() * m[1].getZ() + mRows[2].getX() * m[2].getZ(),
		mRows[0].getY() * m[0].getX() + mRows[1].getY() * m[1].getX() + mRows[2].getY() * m[2].getX(),
		mRows[0].getY() * m[0].getY() + mRows[1].getY() * m[1].getY() + mRows[2].getY() * m[2].getY(),
		mRows[0].getY() * m[0].getZ() + mRows[1].getY() * m[1].getZ() + mRows[2].getY() * m[2].getZ(),
		mRows[0].getZ() * m[0].getX() + mRows[1].getZ() * m[1].getX() + mRows[2].getZ() * m[2].getX(),
		mRows[0].getZ() * m[0].getY() + mRows[1].getZ() * m[1].getY() + mRows[2].getZ() * m[2].getY(),
		mRows[0].getZ() * m[0].getZ() + mRows[1].getZ() * m[1].getZ() + mRows[2].getZ() * m[2].getZ());
}

FORCEINLINE pxMat3x3 
pxMat3x3::timesTranspose(const pxMat3x3& m) const
{
	return pxMat3x3(
		mRows[0].Dot(m[0]), mRows[0].Dot(m[1]), mRows[0].Dot(m[2]),
		mRows[1].Dot(m[0]), mRows[1].Dot(m[1]), mRows[1].Dot(m[2]),
		mRows[2].Dot(m[0]), mRows[2].Dot(m[1]), mRows[2].Dot(m[2]));

}

#endif // !__PX_Matrix3x3_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
