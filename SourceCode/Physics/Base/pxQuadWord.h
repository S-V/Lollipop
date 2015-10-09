/*
=============================================================================
	File:	pxQuadWord.h
	Desc:	
=============================================================================
*/

#ifndef __PX_QuadWord_H__
#define __PX_QuadWord_H__

#include <Physics/Base/pxTypes.h>

typedef __m128			pxSimdReal;	// only the first (x) element contains a valid value
typedef __m128			pxQuadReal;	// all four elements are used
union pxQuadRealUnion
{
	pxReal      r[4];
	pxQuadReal  quad;
};

MX_SWIPED("Bullet")
#if defined (__CELLOS_LV2) && defined (__SPU__)
#include <altivec.h>
#endif

/**@brief The pxQuadWord class is base class for pxVec3 and pxQuat. 
 * Some issues under PS3 Linux with IBM 2.1 SDK, gcc compiler prevent from using aligned quadword.
 */
#ifndef USE_LIBSPE2
MX_ALIGN_16(class) pxQuadWord
#else
class pxQuadWord
#endif
{
protected:
	pxReal	m_floats[4];

public:


	/**@brief Return the x value */
	FORCEINLINE const pxReal& getX() const { return m_floats[0]; }
	/**@brief Return the y value */
	FORCEINLINE const pxReal& getY() const { return m_floats[1]; }
	/**@brief Return the z value */
	FORCEINLINE const pxReal& getZ() const { return m_floats[2]; }
	/**@brief Set the x value */
	FORCEINLINE void	setX(pxReal x) { m_floats[0] = x;};
	/**@brief Set the y value */
	FORCEINLINE void	setY(pxReal y) { m_floats[1] = y;};
	/**@brief Set the z value */
	FORCEINLINE void	setZ(pxReal z) { m_floats[2] = z;};
	/**@brief Set the w value */
	FORCEINLINE void	setW(pxReal w) { m_floats[3] = w;};


	//FORCEINLINE pxReal&       operator[](int i)       { return (&m_floats[0])[i];	}      
	//FORCEINLINE const pxReal& operator[](int i) const { return (&m_floats[0])[i]; }
	///operator pxReal*() replaces operator[], using implicit conversion. We added operator != and operator == to avoid pointer comparisons.
	FORCEINLINE	operator       pxReal *()       { return &m_floats[0]; }
	FORCEINLINE	operator const pxReal *() const { return &m_floats[0]; }

	FORCEINLINE	bool	operator==(const pxQuadWord& other) const
	{
		return ((m_floats[3]==other.m_floats[3]) && (m_floats[2]==other.m_floats[2]) && (m_floats[1]==other.m_floats[1]) && (m_floats[0]==other.m_floats[0]));
	}

	FORCEINLINE	bool	operator!=(const pxQuadWord& other) const
	{
		return !(*this == other);
	}

	/**@brief Set x,y,z and zero w 
	* @param x Value of x
	* @param y Value of y
	* @param z Value of z
	*/
	FORCEINLINE void 	SetValue(const pxReal& x, const pxReal& y, const pxReal& z)
	{
		m_floats[0]=x;
		m_floats[1]=y;
		m_floats[2]=z;
		m_floats[3] = 0.f;
	}

	/*		void getValue(pxReal *m) const 
	{
	m[0] = m_floats[0];
	m[1] = m_floats[1];
	m[2] = m_floats[2];
	}
	*/
	/**@brief Set the values 
	* @param x Value of x
	* @param y Value of y
	* @param z Value of z
	* @param w Value of w
	*/
	FORCEINLINE void	SetValue(const pxReal& x, const pxReal& y, const pxReal& z,const pxReal& w)
	{
		m_floats[0]=x;
		m_floats[1]=y;
		m_floats[2]=z;
		m_floats[3]=w;
	}
	/**@brief No initialization constructor */
	FORCEINLINE pxQuadWord()
		//	:m_floats[0](pxReal(0.)),m_floats[1](pxReal(0.)),m_floats[2](pxReal(0.)),m_floats[3](pxReal(0.))
	{
	}

	/**@brief Three argument constructor (zeros w)
	* @param x Value of x
	* @param y Value of y
	* @param z Value of z
	*/
	FORCEINLINE pxQuadWord(const pxReal& x, const pxReal& y, const pxReal& z)		
	{
		m_floats[0] = x, m_floats[1] = y, m_floats[2] = z, m_floats[3] = 0.0f;
	}

	/**@brief Initializing constructor
	* @param x Value of x
	* @param y Value of y
	* @param z Value of z
	* @param w Value of w
	*/
	FORCEINLINE pxQuadWord(const pxReal& x, const pxReal& y, const pxReal& z,const pxReal& w) 
	{
		m_floats[0] = x, m_floats[1] = y, m_floats[2] = z, m_floats[3] = w;
	}

	/**@brief Set each element to the max of the current values and the values of another pxQuadWord
	* @param other The other pxQuadWord to compare with 
	*/
	FORCEINLINE void	setMax(const pxQuadWord& other)
	{
		TSetMax(m_floats[0], other.m_floats[0]);
		TSetMax(m_floats[1], other.m_floats[1]);
		TSetMax(m_floats[2], other.m_floats[2]);
		TSetMax(m_floats[3], other.m_floats[3]);
	}
	/**@brief Set each element to the min of the current values and the values of another pxQuadWord
	* @param other The other pxQuadWord to compare with 
	*/
	FORCEINLINE void	setMin(const pxQuadWord& other)
	{
		TSetMin(m_floats[0], other.m_floats[0]);
		TSetMin(m_floats[1], other.m_floats[1]);
		TSetMin(m_floats[2], other.m_floats[2]);
		TSetMin(m_floats[3], other.m_floats[3]);
	}
};

#endif // !__PX_QuadWord_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
