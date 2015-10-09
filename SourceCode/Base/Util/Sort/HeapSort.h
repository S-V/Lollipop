//
//	HeapSort.h
//
//	Templated function that uses the binary heap sort in-place technique for sorting arrays of elements.  Sorting time
//	can be on the order of O(N ln(N)).  This technique does not use recursion.
//
//	The passed in "compare" function compares two elements of type T and returns true if the first
//	element is greater than the second.
//
//	Copyright 2005 Paul Higinbotham
//


#ifndef __HEAPSORT_H_
#define __HEAPSORT_H_

#include "SortUtils.h"

namespace ZGraphics
{


template <typename T>
inline void MakeHeap( T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2) )
{
	size_t tp = 0;

	while (1)
	{
		// Left hand child
		size_t tcl = (tp << 1) + 1;
		size_t tcr = tcl + 1;
		if (tcl >= tCount)
			break;

		// Push child up tree if needed
		size_t tptemp = tp;
		while (compare(pArray[tcl], pArray[tptemp]))
		{
			Swap<T>(pArray[tcl], pArray[tptemp]);
			if (tptemp == 0)
				break;
			tcl = tptemp;

			(tptemp % 2) ? (tptemp = (tcl - 1) >> 1) : (tptemp = (tcl - 2) >> 1);
		}
		
		// Right hand child
		if (tcr >= tCount)
			break;

		// Push child up tree if needed
		tptemp = tp;
		while (compare(pArray[tcr], pArray[tptemp]))
		{
			Swap<T>(pArray[tcr], pArray[tptemp]);
			if (tptemp == 0)
				break;
			tcr = tptemp;

			(tptemp % 2) ? (tptemp = (tcr - 1) >> 1) : (tptemp = (tcr -2) >> 1);
		}

		++tp;
	}
}

template <class T>
inline void ReHeapify( T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2) )
{
	size_t tp = 0;

	while (1)
	{
		size_t tcl = (tp << 1) + 1;
		size_t tcr = tcl + 1;
		if (tcl >= tCount)
			break;

		// Push root node value down the tree until the tree is complete
		if (compare(pArray[tcl], pArray[tp]))
		{
			// We want to swap with the largest child (left or right)
			if ( tcr < tCount && compare(pArray[tcr], pArray[tcl]) )
			{
				Swap<T>(pArray[tp], pArray[tcr]);
				tp = tcr;
			}
			else
			{
				Swap<T>(pArray[tp], pArray[tcl]);
				tp = tcl;
			}
		}
		else if (tcr < tCount && compare(pArray[tcr], pArray[tp]))
		{
			Swap<T>(pArray[tp], pArray[tcr]);
			tp = tcr;
		}
		else
		{
			break;
		}
	}
}

template <class T>
void HeapSort(T * pArray, size_t tCount, bool (*compare)(const T& ele1, const T& ele2))
{
	// Sanity check
	if (tCount < 2)
		return;

	// First create heap from array (binary tree structure)
	MakeHeap(pArray, tCount, compare);

	// Walk binary tree and sort array elements
	size_t tUnsorted = tCount;
	
	while (tUnsorted > 1)
	{
		Swap<T>(pArray[0], pArray[--tUnsorted]);
		ReHeapify(pArray, tUnsorted, compare);
	}
}


} /* namespace ZGraphics */


#endif /* __HEAPSORT_H_ */
