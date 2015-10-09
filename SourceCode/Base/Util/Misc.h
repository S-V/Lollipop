/*
=============================================================================
	File:	Misc.h
	Desc:	misc utility code that doesn't fit into any particular category
=============================================================================
*/

#ifndef __MX_MISC_H__
#define __MX_MISC_H__

#include <Base/Util/Sorting.h>

mxNAMESPACE_BEGIN

//
//	TObjectPtrTable - a static table of pointers to 'TYPE' instances.
//
//	May be used for accessing objects by indices.
//	Relative ordering of array elements can carry some useful information.
//
template< typename TYPE, UINT MAX >
struct TObjectPtrTable
{
public:

	// returns unique index

	FORCEINLINE UINT GetUniqueIndex() const
	{
		return m_uniqueIndex;
	}

public:
	static FORCEINLINE TYPE* GetPtrByIndex( UINT index )
	{
		//Assert( IsValidIndex( index ) );
		return ms_table[ index ];
	}
	static FORCEINLINE UINT GetTotalCount()
	{
		return ms_table.Num();
	}
	static FORCEINLINE UINT GetMaxCapacity()
	{
		return MAX;
	}
	static FORCEINLINE bool IsValidIndex( UINT index )
	{
		return index >= 0 && index < GetTotalCount();
	}

	static FORCEINLINE TYPE** GetPtrArray()
	{
		return ms_table.ToPtr();
	}

protected:

	inline TObjectPtrTable()
	{
		Assert( ms_table.Num() < GetMaxCapacity() );

		const UINT oldNum = ms_table.Num();
		TYPE* pThis = static_cast< TYPE* >( this );

		m_uniqueIndex = oldNum;

		ms_table.Add( pThis );
	}

	inline ~TObjectPtrTable()
	{
		Assert( m_uniqueIndex != INDEX_NONE );
		ms_table[ m_uniqueIndex ] = nil;
		m_uniqueIndex = INDEX_NONE;
	}

private:
	UINT		m_uniqueIndex;

	static TStaticList_InitZeroed< TYPE*, MAX >	ms_table;
};

template< typename TYPE, UINT MAX >
TStaticList_InitZeroed< TYPE*, MAX >		TObjectPtrTable< TYPE, MAX >::ms_table;



//--------------------------------------------------------------//


//
//	TSortedPtrTable - a static table of pointers to 'TYPE' instances.
//
//	May be used for accessing objects by indices.
//	Relative ordering of array elements can carry some useful information.
//
template< typename TYPE, UINT MAX >
struct TSortedPtrTable
{
	FORCEINLINE UINT GetSortedIndex() const
	{
		Assert(mIsSorted);
		return mSortedIndex;
	}

public:
	static FORCEINLINE TYPE* GetPtrBySortedIndex( UINT index )
	{
		Assert(mIsSorted);
		return mSortedPtrArray[ index ];
	}
	static FORCEINLINE bool IsTableSorted()
	{
		return mIsSorted;
	}
	static void SetUnsorted()
	{
		mIsSorted = FALSE;
	}

	template< typename U >	// where U : TYPE and U has operator < (const U&, const U&)
	static void SortIndices( U** source, UINT num )
	{
		if( num > 1 )
		{
			MemCopy( mSortedPtrArray.ToPtr(), source, num * (sizeof source[0]) );

			U ** pStart = c_cast(U**) mSortedPtrArray.ToPtr();
			U ** pEnd = pStart + num;

			NxQuickSort< U*, U >( pStart, pEnd );

			for( UINT i=0; i<num; i++ )
			{
				TYPE* p = mSortedPtrArray[i];
				p->mSortedIndex = i;
			}
		}

		mIsSorted = true;
	}

protected:

	inline TSortedPtrTable()
	{
		mSortedIndex = 0;
	}

	inline ~TSortedPtrTable()
	{
	}

private:

	UINT	mSortedIndex;

	static BOOL mIsSorted;

	static TStaticArray_InitZeroed< TYPE*, MAX >	mSortedPtrArray;
};

template< typename TYPE, UINT MAX >
BOOL TSortedPtrTable< TYPE, MAX >::mIsSorted = false;

template< typename TYPE, UINT MAX >
TStaticArray_InitZeroed< TYPE*, MAX >	TSortedPtrTable< TYPE, MAX >::mSortedPtrArray;

//--------------------------------------------------------------//

//
//	TSingleton_LazyInit< TYPE >
//
template< typename TYPE >
class TSingleton_LazyInit {
public:
	static TYPE & Get()
	{
		ConstructInstance();
		return (*gInstance);
	}
	static TYPE* GetInstance()
	{
		ConstructInstance();
		return gInstance;
	}
	static bool HasInstance()
	{
		return (gInstance != nil);
	}

	virtual ~TSingleton_LazyInit()
	{
		Assert(HasInstance());
		gInstance = nil;
	}

protected:
	TSingleton_LazyInit()
	{
		AssertX(!HasInstance(),"Singleton class has already been instantiated.");
		gInstance = static_cast< TYPE* >( this );
	}

private:
	typedef TSingleton_LazyInit<TYPE> THIS_TYPE;
	PREVENT_COPY( THIS_TYPE );

private:
	static void ConstructInstance()
	{
		if(!gInstance)
		{
			gInstance = new_one(TYPE());
		}
	}

private:
	MX_THREAD_LOCAL static TYPE * gInstance;
};

template< typename TYPE >
MX_THREAD_LOCAL TYPE * TSingleton_LazyInit< TYPE >::gInstance = nil;

//--------------------------------------------------------------//

//
//	TIndirectObjectTable - a large static table of pointers to 'TYPE' instances.
//
//	May be useful when accessing objects by indices.
//	Even relative ordering of elements can carry some useful information.
//	Usually the first element (with zero index) is the one used by default.
//
template< typename TYPE, UINT MAX = 1024 >
struct TIndirectObjectTable
{
public:
	FORCEINLINE UINT GetUniqueIndex() const
	{
		return mUniqueIndex;
	}

public:
	static FORCEINLINE TYPE* GetPtrByIndex( UINT index )
	{
//		AssertPtr( mTable );
		Assert( index >= 0 && index < mStaticCounter );
		return mTable[ index ];
	}
	static FORCEINLINE UINT GetTotalCount()
	{
		return mStaticCounter;
	}
	static FORCEINLINE UINT GetMaxCapacity()
	{
		return MAX;
	}

public_internal:

	// table should be large enough to hold MAX pointers to instances of TYPE
	static void InitTable( TYPE** table )
	{
		Assert(nil == mTable);
		mTable = table;
	}

	// deletes each entry
	static void Purge()
	{
		if( !mTable ) {
			return;
		}
		for( UINT iObj = 0; iObj < GetMaxCapacity(); iObj++ )
		{
			if( mTable[iObj] ) {
				free_one( mTable[iObj] );
			}
		}
	}

protected:
	inline TIndirectObjectTable()
		: mUniqueIndex( mStaticCounter++ )
	{
		Assert( mStaticCounter < GetMaxCapacity() );
		mTable[ mUniqueIndex ] = static_cast< TYPE* >( this );
	}
	virtual ~TIndirectObjectTable()
	{
		mTable[ mUniqueIndex ] = nil;
	}

protected://private:
	/*const*/ UINT		mUniqueIndex;
	static UINT		mStaticCounter;	// always increases
	static TYPE **	mTable;
};

template< typename TYPE, UINT MAX >
UINT TIndirectObjectTable< TYPE, MAX >::mStaticCounter = 0;

template< typename TYPE, UINT MAX >
TYPE** TIndirectObjectTable< TYPE, MAX >::mTable = nil;

mxNAMESPACE_END

#endif // !__MX_MISC_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
