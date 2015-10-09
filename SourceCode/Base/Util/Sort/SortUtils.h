//
//	SortUtils.h
//
//	Various templated functions used by other sorting functions.
//
//	Copyright 2005 Paul Higinbotham
//


#ifndef __SORTUTILS_H_
#define __SORTUTILS_H_


namespace ZGraphics
{


template <typename T>
inline void Swap(T& rcEle1, T& rcEle2)
{
	T temp = rcEle1;
	rcEle1 = rcEle2;
	rcEle2 = temp;
}

template <typename T>
void RotateArrayRight(T * pArray, size_t tCount, size_t tRotateNum)
{
	tRotateNum = tRotateNum % tCount;

	size_t t1 = 0;
	size_t t2 = tCount - tRotateNum;
	
	while (tRotateNum != 0)
	{
		// Perform exchange for rotation
		Swap<T>(pArray[t1], pArray[t2]);
		++t1;
		++t2;

		if (t2 == tCount)
		{
			// Recompute rotate number for end of exchange fix up.  If rotate is zero then we are done.
			tRotateNum = tRotateNum % (t2-t1);
			t2 = tCount - tRotateNum;
		}
	}
}

// @todo Add RotateArrayLeft templated function


} /* namespace ZSort */


#endif /* __SORTUTILS_H_ */
