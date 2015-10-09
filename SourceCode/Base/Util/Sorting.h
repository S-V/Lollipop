/*
=============================================================================
	File:	Sorting.h
	Desc:	
=============================================================================
*/

#ifndef __MX_SORTING_H__
#define __MX_SORTING_H__

mxNAMESPACE_BEGIN

/*
=============================================================================
	Insertion sort

	- Efficient for (quite) small data sets
	- Adaptive, i.e. efficient for data sets that are already substantially sorted: the time complexity is O(n + d), where d is the number of inversions
	- More efficient in practice than most other simple quadratic, i.e. O(n2) algorithms such as selection sort or bubble sort; the best case (nearly sorted input) is O(n)
	- Stable, i.e. does not change the relative order of elements with equal keys
	- In-place, i.e. only requires a constant amount O(1) of additional memory space
	- Online, i.e. can sort a list as it receives it
=============================================================================
*/
template< typename TYPE, class COMPARATOR >
void InsertionSort( TYPE* data, UINT startIndex, UINT endIndex )
{
	Assert( startIndex < endIndex );
	TYPE * lo = data + startIndex;
	TYPE * hi = data + endIndex;
	TYPE * best;
	TYPE * p;

	while( hi > lo )
	{
		best = lo;
		for( p = lo+1; p <= hi; p++ )
		{
			if( COMPARATOR::Compare( *p, *best ) )
			{
				best = p;
			}
		}
		TSwap( *best,*hi );
		hi--;
	}
}

/*
=============================================================================
	Heap sort

	Sortables must provide an implementation of operator '<'.

	NOTE: Heap sort is an in-place algorithm, but is not a stable sort.

	There is no additional memory usage and the algorithm performs
	O(n*log n) in worst case.
=============================================================================
*/

mxSWIPED("Irrlicht");

// Sorts an elements with size 'size' using heap sort.
template< typename TYPE >
inline void HeapSort( TYPE* elements, UINT size )
{
	struct HeapSortUtil
	{
		//! Sinks an element into the heap.
		static inline void HeapSink( TYPE* elements, UINT element, UINT max )
		{
			while( (element<<1) < max ) // there is a left child
			{
				UINT j = (element<<1);

				if( j+1 < max && elements[j] < elements[j+1] )
				{
					j = j+1; // take right child
				}
				if( elements[element] < elements[j] )
				{
					TYPE t = elements[j]; // swap elements
					elements[j] = elements[element];
					elements[element] = t;
					element = j;
				}
				else
				{
					return;
				}
			}
		}
	};

	// for HeapSink we pretend this is not c++, where
	// arrays start with index 0. So we decrease the elements pointer,
	// the maximum always +2 and the element always +1

	TYPE* virtualArray = elements - 1;
	UINT virtualSize = size + 2;
	UINT i;

	// build heap

	for( i = ((size-1)/2); i>=0; --i )
	{
		HeapSortUtil::HeapSink( virtualArray, i+1, virtualSize-1 );
	}
	// sort elements, leave out the last element (0)
	for( i = size-1; i>0; --i )
	{
		TYPE t = elements[0];
		elements[0] = elements[i];
		elements[i] = t;
		HeapSortUtil::HeapSink( virtualArray, 1, i + 1 );
	}
}

/*
=============================================================================
	Quick sort

	NOTE: quick sort is an in-place algorithm, but is not a stable sort, 
	that is, the original order of elements with equal sort keys is not preserved
=============================================================================
*/

//-----------------------------------------------------------------------------
//	Naive quick sort implementation.
//
//	Sortables must provide an implementation of operators '<' and '>'.
//
//-----------------------------------------------------------------------------
//
template< typename TYPE >
void QuickSort( TYPE* a, UINT n )
{
	struct QuickSortUtil
	{
		static void SortSubsection( TYPE* a, UINT lo, UINT hi )
		{
			if( lo >= hi ) {
				return;
			}
			TYPE& pivot = a[ hi ];
			UINT i = lo - 1;
			UINT j = hi;

			while( i < j )
			{
				while( a[++i] < pivot )
					;
				while( j >= lo && a[--j] > pivot )
					;
				if( i < j ) {
					TSwap( &a[i], &a[j] );
				}
			}
			TSwap( &a[i], &a[hi] );
			SortSubsection( a, lo, i-1 );
			SortSubsection( a, i+1, hi );
		}
	};
	QuickSortUtil::SortSubsection( a, 0, n-1 );
}

//-----------------------------------------------------------------------------
// Quick sort using CRT library implementation.
//-----------------------------------------------------------------------------
//
typedef int (CDECL* QSortComparisonFunction)( const void* A, const void* B );

void QSort( void* pBase, INT numOfElements, INT sizeOfElements, QSortComparisonFunction pCompareFunc );




mxSWIPED("PhysX SDK");
/**
\brief Sorts an array of Sortables or an array of pointers to Sortables.
(the second tends to be much faster)

NOTE: this function sorts elements in ascending order, i.e. first will go elements with lesser sort keys.

In the first case Predicate is simply:

class SortableCompareDirect
	{
	public:
	inline bool operator () (const Sortable &a, const Sortable & b)
		{
		return a < b;
		}
	};
In the second case it should include the dereference:

class SortableComparePtr
	{
	public:
	inline bool operator () (const Sortable *a, const Sortable * b)
		{
		return (*a) < (*b);
		}
	};

Where it is assumed that Sortable implements the compare operator:

class SortElem
	{
	public:
	int sortKey;

	....

	inline bool operator <(const SortElem & e) const
		{
		return sortKey < e.sortKey;
		}
	};

This is not used by the below code directly, only the example
predicates above.

Called like this:
std::vector<SortElem> sortVector;
NxQuickSort<SortElem, SortElemCompareDirect>(&sortVector[0], &sortVector[sortVector.size()-1]);

//faster if SortElem is a large object, otherwise its about the same:
std::vector<SortElem * > sortPtrVector;
NxQuickSort<SortElem *, SortElemComparePtr>(&sortPtrVector[0], &sortPtrVector[sortPtrVector.size()-1]);
*/
template< class Sortable, class Predicate >
inline void NxQuickSort( Sortable * start, Sortable * end, Predicate & p )
{
	Sortable * i;
	Sortable * j;
	Sortable m;

	do
	{
		i = start;
		j = end;
		m = *(i + ((j - i) >> 2));

		while (i <= j) 
		{
			while(p(*i,m))
				i++;
			while(p(m,*j))
				j--;
			if (i <= j)
			{
				if (i != j)
				{
					Sortable k = *i;
					*i = *j;
					*j = k;
				}
				i++; 
				j--;
			}
		}

		if ( start < j ) {
			NxQuickSort<Sortable,Predicate>( start, j, p );
		}

		start = i;
	}
	while (i < end);	//we do this instead of recursing:
}


/*
=============================================================================
	Radix sort
=============================================================================
*/



mxNAMESPACE_END

#endif // ! __MX_SORTING_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
