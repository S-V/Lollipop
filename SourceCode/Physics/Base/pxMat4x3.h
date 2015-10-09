/*
=============================================================================
	File:	pxMat4x3.h
	Desc:	.
=============================================================================
*/

#ifndef __PX_Matrix4x3_H__
#define __PX_Matrix4x3_H__

/*
=======================================================================
	
	pxMat4x3
	
	4x3 matrix, row-major (4 rows, 3 columns)
=======================================================================
*/
struct pxMat4x3
{
	pxVec3	mRows[4];

public:
	FORCEINLINE pxMat4x3()
	{}

	FORCEINLINE explicit pxMat4x3( const pxQuat& q )
	{
		SetRotation( q );
	}

	FORCEINLINE pxMat4x3(
		const pxVec3& row0, const pxVec3& row1,
		const pxVec3& row2, const pxVec3& row3 )
	{
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
		mRows[3] = row3;
	}

	FORCEINLINE pxMat4x3( const pxMat4x3& other )
	{
		mRows[0] = other.mRows[0];
		mRows[1] = other.mRows[1];
		mRows[2] = other.mRows[2];
		mRows[3] = other.mRows[3];
	}

	FORCEINLINE pxMat4x3& operator = ( const pxMat4x3& other )
	{
		mRows[0] = other.mRows[0];
		mRows[1] = other.mRows[1];
		mRows[2] = other.mRows[2];
		mRows[3] = other.mRows[3];
		return *this;
	}

	FORCEINLINE void SetRotation( const pxQuat& q )
	{
		mRows[0].SetValue( pxReal(0.5) * -q.getX(), pxReal(0.5) * -q.getY(), pxReal(0.5) * -q.getZ() );
		mRows[1].SetValue( pxReal(0.5) * q.GetW(),  pxReal(0.5) * q.getZ(),  pxReal(0.5) * -q.getY() );
		mRows[2].SetValue( pxReal(0.5) * -q.getZ(), pxReal(0.5) * q.GetW(),  pxReal(0.5) * q.getX()  );
		mRows[3].SetValue( pxReal(0.5) * q.getY(),  pxReal(0.5) * -q.getX(), pxReal(0.5) * q.GetW()  );
	}
};

#endif // !__PX_Matrix4x3_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
