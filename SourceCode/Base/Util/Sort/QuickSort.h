//
//	QuickSort.h
//
//	Templated function that uses the quick sort in-place technique for sorting arrays of elements.  Sorting time
//	can be on the order of O(N ln(N)) to O(N^2).  This is a recursive function.
//
//	The passed in "compare" function compares two elements of type T and returns true if the first
//	element is greater than the second.
//
//	Copyright 2005 Paul Higinbotham
//

#ifndef __QUICKSORT_H_
#define __QUICKSORT_H_


#include "SortUtils.h"


namespace ZGraphics
{


// Function to choose the pivot element from the first, middle, and last elements in the array
// The pivot element is moved to the beginning of the array.
template <typename T>
inline void ChoosePivotElement( T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2) )
{
	if (tCount < 3)
		return;

	size_t tmid = tCount / 2;
	size_t tend = tCount - 1;

	if (compare(pArray[0], pArray[tmid]))
	{
		if (compare(pArray[0], pArray[tend]))
		{
			if (compare(pArray[tmid], pArray[tend]))
			{
				// nmid is pivot element
				Swap<T>(pArray[0], pArray[tmid]);
			}
			else
			{
				// nend is pivot element
				Swap<T>(pArray[0], pArray[tend]);
			}
		}

		// otherwise first element is pivot element and no swap is needed
		return;
	}
	else
	{
		if (compare(pArray[tmid], pArray[tend]))
		{
			if (compare(pArray[tend], pArray[0]))
			{
				// nend is pivot element
				Swap<T>(pArray[0], pArray[tend]);
			}

			// otherwise pArray[0] is pivot element and no swap is needed
			return;
		}
		else
		{
			// nmid is pivot element
			Swap<T>(pArray[0], pArray[tmid]);
		}
	}
}


template <typename T>
void QuickSort( T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2) )
{
	if (tCount < 2)
		return;

	// Choose the pivot element and move it to the first element spot
	ChoosePivotElement<T>(pArray, tCount, compare);

	// First element is always the pivot
	size_t tp = 0;
	size_t t1 = tp + 1;
	size_t t2 = tCount - 1;

	// Move all array elements before or after the pivot value.
	while (1)
	{
		while (t2 > t1)
		{
			if (compare(pArray[tp], pArray[t2]))
				break;
			--t2;
		}

		while (t1 < t2)
		{
			if (!compare(pArray[tp], pArray[t1]))
				break;
			++t1;
		}

		if (t2 > t1)
			Swap<T>(pArray[t1++], pArray[t2--]);
		else
			break;
	}

	// Place pivot point
	while ((t2 > tp) && !compare(pArray[tp], pArray[t2]))
	{
		--t2;
	}
	if (t2 > tp)
	{
		Swap<T>(pArray[tp], pArray[t2]);
		tp = t2;
	}

	// Divide along the pivot index and recurse (divide and conquer).
	QuickSort<T>(pArray, tp, compare);
	if (tp < tCount-2)
	{
		QuickSort<T>(&pArray[tp+1], tCount-tp-1, compare);
	}
}


} /* namespace ZGraphics */


#endif /* __QUICKSORT_H_ */
