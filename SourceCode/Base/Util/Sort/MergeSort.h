//
//	MergeSort.h
//
//	Template MergeSort function that performs a merge sort.  Merging is done in-place so sorting time
//	is anywhere from O(N ln(N)) to O(N^2 ln(N)).
//
//	The passed in "compare" function compares two elements of type T and returns true if the first
//	element is greater than the second.  This is a recursive function.
//
//	Copyright 2005 Paul Higinbotham
//

#ifndef __MERGESORT_H_
#define __MERGESORT_H_

#include "SortUtils.h"


namespace ZGraphics
{


template <typename T>
void MergeSort( T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2) )
{
	if (tCount < 2)
		return;

	if (tCount == 2)
	{
		// Sort binary array
		if (compare(pArray[0], pArray[1]))
		{
			Swap<T>(pArray[0], pArray[1]);
		}
	}
	else
	{
		// Divide and conquer problem
		size_t tMid = tCount / 2;
		MergeSort<T>(pArray, tMid, compare);
		MergeSort<T>(&pArray[tMid], (tCount - tMid), compare);

		// Merge both sorted array halves together
		size_t t1 = 0;
		size_t t2 = tMid;

		while (t2 > t1 && t2 < tCount)
		{
			size_t tRotate = 0;

			while ( (t2 < tCount) && compare(pArray[t1], pArray[t2]) )
			{
				++tRotate;
				++t2;
			}

			if (tRotate > 0)
			{
				RotateArrayRight<T>(&pArray[t1], (t2-t1), tRotate);
				t1 += tRotate + 1;
			}
			else
			{
				++t1;
			}
		}
	}
}


} /* namespace ZGraphics */


#endif /* __MERGESORT_H_ */
