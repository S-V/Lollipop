/*
=============================================================================
	File:	pxVec4.h
	Desc:	.
=============================================================================
*/

#ifndef __PX_Vector4D_H__
#define __PX_Vector4D_H__
MX_SWIPED("Bullet/Solid")
//
//	pxVec4
//
class pxVec4 : public pxVec3
{
public:
	PX_INLINE pxVec4() {}

	PX_INLINE pxVec4( pxReal x,  pxReal y,  pxReal z, pxReal w) 
		: pxVec3(x,y,z)
	{
		m_floats[3] = w;
	}

	PX_INLINE pxVec4 absolute4() const 
	{
		return pxVec4(
			mxFabs(m_floats[0]), 
			mxFabs(m_floats[1]), 
			mxFabs(m_floats[2]),
			mxFabs(m_floats[3]));
	}

	pxReal	GetW() const { return m_floats[3];}

	PX_INLINE int maxAxis4() const
	{
		int maxIndex = -1;
		pxReal maxVal = pxReal(-PX_LARGE_FLOAT);
		if (m_floats[0] > maxVal)
		{
			maxIndex = 0;
			maxVal = m_floats[0];
		}
		if (m_floats[1] > maxVal)
		{
			maxIndex = 1;
			maxVal = m_floats[1];
		}
		if (m_floats[2] > maxVal)
		{
			maxIndex = 2;
			maxVal =m_floats[2];
		}
		if (m_floats[3] > maxVal)
		{
			maxIndex = 3;
			maxVal = m_floats[3];
		}
		return maxIndex;
	}

	PX_INLINE int minAxis4() const
	{
		int minIndex = -1;
		pxReal minVal = pxReal(PX_LARGE_FLOAT);
		if (m_floats[0] < minVal)
		{
			minIndex = 0;
			minVal = m_floats[0];
		}
		if (m_floats[1] < minVal)
		{
			minIndex = 1;
			minVal = m_floats[1];
		}
		if (m_floats[2] < minVal)
		{
			minIndex = 2;
			minVal =m_floats[2];
		}
		if (m_floats[3] < minVal)
		{
			minIndex = 3;
			minVal = m_floats[3];
		}
		
		return minIndex;

	}

	PX_INLINE int closestAxis4() const 
	{
		return absolute4().maxAxis4();
	}


  /**@brief Set x,y,z and zero w 
   * @param x Value of x
   * @param y Value of y
   * @param z Value of z
   */
		

/*		void getValue(pxReal *m) const 
		{
			m[0] = m_floats[0];
			m[1] = m_floats[1];
			m[2] =m_floats[2];
		}
*/
/**@brief Set the values 
   * @param x Value of x
   * @param y Value of y
   * @param z Value of z
   * @param w Value of w
   */
	PX_INLINE void SetValue( pxReal x,  pxReal y,  pxReal z, pxReal w)
	{
		m_floats[0]=x;
		m_floats[1]=y;
		m_floats[2]=z;
		m_floats[3]=w;
	}
};

#endif // !__PX_Vector4D_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
