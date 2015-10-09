/*
=============================================================================
	File:	Memory.h
	Desc:	memory management
=============================================================================
*/

#pragma once

/*
-----------------------------------------------------------------------------
	Data structure used to do the lookup from ID to system object.
	Requirements that need to be fulfilled:
	- There should be a 1-1 mapping between live objects and IDs.
	- If the system is supplied with an ID to an old object, it should be able to detect that the object is no longer alive.
	- Lookup from ID to object should be very fast (this is the most common operation).
	- Adding and removing objects should be fast.
-----------------------------------------------------------------------------
*/
#if 0
mxSWIPED("http://bitsquid.blogspot.com/2011/09/managing-decoupling-part-4-id-lookup.html");

enum { INDEX_MASK = 0xffff };
enum { NEW_OBJECT_ID_ADD = 0x10000 };

struct SObjIndex
{
	ObjectHandle id;
	U2 index;
	U2 next;
};
mxDECLARE_POD_TYPE(SObjIndex);

/*
 The ID Lookup Table with a fixed array size, a 32 bit ID and a FIFO free list.
*/
template< class OBJECT, UINT MAX_OBJECTS = 64*1024 >
struct TObjectSystem
{
	typedef TStaticArray<OBJECT,MAX_OBJECTS>	ObjectsArray;
	typedef TStaticArray<SObjIndex,MAX_OBJECTS>	IndicesArray;

	UINT			m_num_objects;
	ObjectsArray	m_objects;
	IndicesArray	m_indices;
	UINT			m_freelist_enqueue;
	UINT			m_freelist_dequeue;

public:
	typedef TObjectSystem THIS_TYPE;

	TObjectSystem()
	{
		m_num_objects = 0;
		for( UINT i=0; i<MAX_OBJECTS; ++i )
		{
			m_indices[i].id = i;
			m_indices[i].next = i+1;
		}
		m_freelist_dequeue = 0;
		m_freelist_enqueue = MAX_OBJECTS-1;
	}

	inline bool has( ObjectHandle id )
	{
		//Assert(m_indices.IsValidIndex(id));
		SObjIndex &in = m_indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	inline OBJECT& lookup( ObjectHandle id )
	{
		return m_objects[ m_indices[ id & INDEX_MASK ].index ];
	}
	inline const OBJECT& lookup_const( ObjectHandle id ) const
	{
		return m_objects[ m_indices[ id & INDEX_MASK ].index ];
	}

	inline ObjectHandle Add()
	{
		Assert(m_num_objects < m_objects.Capacity());

		SObjIndex & in = m_indices[ m_freelist_dequeue ];
		m_freelist_dequeue = in.next;
		in.id += NEW_OBJECT_ID_ADD;
		in.index = m_num_objects++;

		OBJECT &o = m_objects[ in.index ];
		o.id = in.id;
		return o.id;
	}

	FORCEINLINE UINT Num() const
	{
		return m_num_objects;
	}
	FORCEINLINE bool IsFull() const
	{
		//return m_num_objects == MAX_OBJECTS-1;
		return m_num_objects == MAX_OBJECTS;
	}
	FORCEINLINE OBJECT & GetObjAt( UINT index )
	{
		return m_objects[ index ];
	}
	FORCEINLINE const OBJECT & GetObjAt( UINT index ) const
	{
		return m_objects[ index ];
	}

	inline void remove( ObjectHandle id )
	{
		//Assert(m_indices.IsValidIndex(id));

		SObjIndex &in = m_indices[id & INDEX_MASK];

		OBJECT &o = m_objects[in.index];
		o = m_objects[--m_num_objects];
		m_indices[o.id & INDEX_MASK].index = in.index;

		in.index = USHRT_MAX;
		m_indices[m_freelist_enqueue].next = id & INDEX_MASK;
		m_freelist_enqueue = id & INDEX_MASK;
	}

public:
	friend inline mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		archive && o.m_num_objects;
		archive && o.m_objects;
		archive && o.m_indices;
		archive && o.m_freelist_enqueue;
		archive && o.m_freelist_dequeue;
		return archive;
	}
};
#endif

//
// Handle manager which always keeps objects in a fixed-size contiguous block of memory
// and supports removal of elements.
// NOTE: objects must be POD types!
//
// code based on:
// "http://bitsquid.blogspot.com/2011/09/managing-decoupling-part-4-id-lookup.html");
//

// See: http://c0de517e.blogspot.com/2011/03/alternatives-to-object-handles.html
// Permutation array.
// We add a second indirection level, to be able to sort resources to cause less cache misses.
// An handle in an index into an array of indices (permutation) of the array that stores the resources.

// this structure introduces another level of indirection
// so that object data stays contiguous after deleting elements at random places
//
struct SRemapObjIndex
{
	U2	objectIndex;	// index into array of objects
	U2	nextFreeSlot;	// index of the next free slot (in the indices array)

public:
	FORCEINLINE bool IsFree() const { return objectIndex != -1; }
};
mxDECLARE_POD_TYPE(SRemapObjIndex);







template< class STRUCT >
struct HandleManagerDefaultCallbacks
{
	// called when a new object is added
	static void InitObject( STRUCT & o, const OID newObjID )
	{
		//o.objectIndex = newObjID;
	}
	static void MoveObject( STRUCT &dest, const STRUCT& src )
	{
		dest = src;
	}
	// called when an existing object is removed
	static void FreeObject( STRUCT & o )
	{
		//o.objectIndex = InvalidIndex;
	}
};

/*
--------------------------------------------------------------
	TIndirectHandleManager< STRUCT, MAX_OBJECTS, CALLBACKS >

	it's meant to be used with POD objects.

	NOTE: addition is fast (O(1)),
	removal in the middle of the list can be slow (O(n)).
--------------------------------------------------------------
*/
template
<
	class STRUCT,
	UINT MAX_OBJECTS,
	class CALLBACKS = HandleManagerDefaultCallbacks<STRUCT>
>
class TIndirectHandleManager
	: public TArrayBase< STRUCT, TIndirectHandleManager<STRUCT,MAX_OBJECTS,CALLBACKS> >
{
	typedef TStaticArray<STRUCT,MAX_OBJECTS>			ObjectsArray;
	typedef TStaticArray<SRemapObjIndex,MAX_OBJECTS>	IndicesArray;

	UINT			m_numObjects;	// total number of objects
	UINT			m_firstFreeHandle;// index of the first free slot in the indices array
	ObjectsArray	m_objectsArray;	// objects are always laid out contiguously, without unused 'holes'
	IndicesArray	m_indicesArray;	// array with 'holes', maps virtual handles to real object indices

public:
	typedef TIndirectHandleManager< STRUCT, MAX_OBJECTS, CALLBACKS >	THIS_TYPE;
	typedef STRUCT	ITEM_TYPE;
	typedef OID		HANDLE_TYPE;
	typedef TypeTrait< STRUCT >	TYPE_TRAIT;

	enum { InvalidIndex = MAX_UINT16 };
	enum { IndexMask = MAX_OBJECTS-1 };

	mxSTATIC_ASSERT( MAX_OBJECTS >= 2 );
	mxSTATIC_ASSERT( MAX_OBJECTS < MAX_UINT16 );
	mxSTATIC_ASSERT( TIsPowerOfTwo< MAX_OBJECTS >::value == true );
	mxSTATIC_ASSERT( TYPE_TRAIT::IsPlainOldDataType );

public:
	TIndirectHandleManager()
	{
		m_numObjects = 0;

		this->Clear();
	}

	FORCEINLINE UINT GetCapacity() const
	{
		return MAX_OBJECTS;
	}
	FORCEINLINE UINT Num() const
	{
		return m_numObjects;
	}
	FORCEINLINE bool IsFull() const
	{
		return m_numObjects == MAX_OBJECTS;
	}

	FORCEINLINE STRUCT* ToPtr()
	{
		return m_objectsArray.ToPtr();
	}
	FORCEINLINE const STRUCT* ToPtr() const
	{
		return m_objectsArray.ToPtr();
	}

	void Clear()
	{
		for( UINT iObj = 0; iObj < m_numObjects; iObj++ )
		{
			CALLBACKS::FreeObject( m_objectsArray[ iObj ] );
		}
		m_numObjects = 0;

		for( UINT iObj = 0; iObj < MAX_OBJECTS; iObj++ )
		{
			SRemapObjIndex & rIndex = m_indicesArray[ iObj ];
			rIndex.objectIndex = InvalidIndex;
			rIndex.nextFreeSlot = iObj + 1;
		}
		m_indicesArray[ MAX_OBJECTS-1 ].nextFreeSlot = InvalidIndex;
		m_firstFreeHandle = 0;
	}

	FORCEINLINE bool Has( HANDLE_TYPE handle )
	{
		SRemapObjIndex & rIndex = m_indicesArray[ handle.d ];
		return rIndex.objectIndex != InvalidIndex;
	}

	FORCEINLINE STRUCT* GetPtrByHandle( HANDLE_TYPE handle )
	{
		SRemapObjIndex & rIndex = m_indicesArray[ handle.GetHandleValue() ];
		//Assert(rIndex.objectIndex != InvalidIndex);
		Assert(rIndex.objectIndex < m_numObjects);
		return &m_objectsArray[ rIndex.objectIndex ];
	}

	inline HANDLE_TYPE Add()
	{
		Assert(m_numObjects < MAX_OBJECTS);
		if( m_firstFreeHandle != InvalidIndex )
		{
			const HANDLE_TYPE newObjHandle( m_firstFreeHandle );

			SRemapObjIndex & rFreeSlot = m_indicesArray[ m_firstFreeHandle ];

			m_firstFreeHandle = rFreeSlot.nextFreeSlot;

			rFreeSlot.objectIndex = m_numObjects++;
			rFreeSlot.nextFreeSlot = InvalidIndex;

			STRUCT & rObject = m_objectsArray[ rFreeSlot.objectIndex ];

			CALLBACKS::InitObject( rObject, newObjHandle );

			//return &rObject;
			return newObjHandle;
		}
		else
		{
			mxDBG_UNIMPLEMENTED;
			//return nil;
			return OID(InvalidIndex);
		}
	}

	inline void Remove( HANDLE_TYPE handle )
	{
		Assert(m_numObjects > 0);

		// reference to the object being deleted
		SRemapObjIndex & rThisObjRef = m_indicesArray[ handle.GetHandleValue() ];

		const UINT thisObjIndex = rThisObjRef.objectIndex;	// index of the object being removed
		const UINT lastObjIndex = --m_numObjects;	// index of the last object in the array

		STRUCT & thisObj = m_objectsArray[ thisObjIndex ];

		CALLBACKS::FreeObject( thisObj );

		// if we're not deleting the last item...
		if( lastObjIndex != thisObjIndex )
		{
			STRUCT & lastObj = m_objectsArray[ lastObjIndex ];

			// swap the object being deleted with the last element
			CALLBACKS::MoveObject( thisObj, lastObj );

			// update the reference to the last object
			const UINT lastObjHandle = FindHandleOfItemWithIndex( lastObjIndex ).GetHandleValue();
			SRemapObjIndex & rLastObjRef = m_indicesArray[ lastObjHandle ];
			rLastObjRef.objectIndex = thisObjIndex;
		}

		// we're deleting the last element.

		rThisObjRef.objectIndex = InvalidIndex;
		rThisObjRef.nextFreeSlot = m_firstFreeHandle;

		m_firstFreeHandle = handle.GetHandleValue();
	}

	inline OID FindHandleOfItemWithIndex( UINT objectIndex )
	{
		const UINT numIndices = m_indicesArray.Capacity();
		for( UINT i = 0; i < numIndices; i++ )
		{
			const SRemapObjIndex & rObjRef = m_indicesArray[ i ];
			if( rObjRef.objectIndex == objectIndex ) {
				return OID(i);
			}
		}
		return OID(InvalidIndex);
	}

public:
	friend inline mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		U4 numObjects = o.m_numObjects;
		archive && numObjects;

		Assert( numObjects <= MAX_OBJECTS );

		o.m_numObjects = numObjects;

		if( numObjects > 0 ) {
			TSerializeArray( archive, o.m_objectsArray.ToPtr(), numObjects );
		}

		archive && o.m_indicesArray;
		archive && o.m_firstFreeHandle;

		return archive;
	}

	PREVENT_COPY(THIS_TYPE);
};












#if 0
/*
--------------------------------------------------------------
	TDynamicHandleManager

	NOTE: objects are copied via operator =.

	Objects and indices are allocated
	in a (contiguous) linear memory block
--------------------------------------------------------------
*/
template< class OBJECT >
struct TDynamicHandleManager
{
	UINT			m_numObjects;	// total number of objects
	UINT			m_firstFreeSlot;// index of the first free slot in the indices array
	OBJECT *		m_objectsArray;	// objects are always laid out contiguously
	SRemapObjIndex*	m_indicesArray;	// array with 'holes'
	UINT			m_numAllocated;	// number of allocated entries (m_objectsArray - start of the whole memory block)
	const HMemory	m_memoryHeap;	// memory manager

public:
	typedef TDynamicHandleManager< OBJECT >	THIS_TYPE;
	typedef OBJECT	ITEM_TYPE;
	typedef TypeTrait< OBJECT >	TYPE_TRAIT;

	enum { InvalidIndex = MAX_UINT16 };
	enum { MAX_OBJECTS = MAX_UINT16 };
	StaticAssert( TYPE_TRAIT::IsPlainOldDataType );

public:
	explicit TDynamicHandleManager( HMemory hMemoryMgr = EMemHeap::DefaultHeap )
		: m_memoryHeap( hMemoryMgr )
	{UNDONE;	//BUGS
		m_numObjects = 0;
		m_firstFreeSlot = InvalidIndex;
		m_objectsArray = nil;
		m_indicesArray = nil;
		m_numAllocated = 0;
	}
	~TDynamicHandleManager()
	{
		this->ReleaseMemory( m_objectsArray );
	}
	FORCEINLINE UINT GetCapacity() const
	{
		return m_numAllocated;
	}
	FORCEINLINE UINT Num() const
	{
		return m_numObjects;
	}
	FORCEINLINE bool IsFull() const
	{
		return false;
	}
	FORCEINLINE bool Has( ObjectHandle objectHandle )
	{
		Assert(objectHandle < m_numAllocated);
		AssertPtr(m_indicesArray);

		SRemapObjIndex & rIndex = m_indicesArray[ objectHandle ];
		return rIndex.objectIndex != InvalidIndex;
	}
	FORCEINLINE OBJECT& GetRefById( ObjectHandle objectHandle )
	{
		Assert(objectHandle < m_numAllocated);
		AssertPtr(m_indicesArray);

		SRemapObjIndex & rIndex = m_indicesArray[ objectHandle ];
		Assert(rIndex.objectIndex < m_numObjects);
		AssertPtr(m_objectsArray);

		return m_objectsArray[ rIndex.objectIndex ];
	}
	FORCEINLINE OBJECT* GetObjectsArray()
	{
		AssertPtr(m_objectsArray);
		return m_objectsArray;
	}
	FORCEINLINE OBJECT & GetObjAt( UINT index )
	{
		Assert(index < m_numObjects);
		AssertPtr(m_objectsArray);
		return m_objectsArray[ index ];
	}
	FORCEINLINE const OBJECT & GetObjAt( UINT index ) const
	{
		Assert(index < m_numObjects);
		AssertPtr(m_objectsArray);
		return m_objectsArray[ index ];
	}
	inline OBJECT* Add()
	{
		this->Reserve( m_numObjects + 1 );

		Assert( m_firstFreeSlot != InvalidIndex );

		const ObjectHandle newObjID = m_firstFreeSlot;

		SRemapObjIndex & rFreeSlot = m_indicesArray[ m_firstFreeSlot ];

		const UINT oldFreeSlot = m_firstFreeSlot;
		m_firstFreeSlot = rFreeSlot.nextFreeSlot;

		rFreeSlot.objectIndex = m_numObjects++;
		rFreeSlot.nextFreeSlot = oldFreeSlot;

		OBJECT & rObject = m_objectsArray[ rFreeSlot.objectIndex ];

		rObject.m_objectIndex = newObjID;

		return &rObject;
	}
	inline void Remove( ObjectHandle objectHandle )
	{
		Assert(m_numObjects > 0);

		SRemapObjIndex & rObjRef = m_indicesArray[ objectHandle ];

		const UINT thisObjIndex = rObjRef.objectIndex;	// index of the object being removed
		const UINT lastObjIndex = --m_numObjects;	// index of the last object in the array

		// if we're not deleting the last item...
		if( lastObjIndex != thisObjIndex )
		{
			// swap the object being deleted with the last element
			OBJECT & lastObj = m_objectsArray[ lastObjIndex ];

			m_objectsArray[ thisObjIndex ] = lastObj;

			// update the reference to the last object
			SRemapObjIndex & rLastObjRef = m_indicesArray[ lastObj.m_objectIndex ];
			rLastObjRef.objectIndex = thisObjIndex;
		}

		// we're deleting the last element.

		rObjRef.objectIndex = InvalidIndex;
		rObjRef.nextFreeSlot = m_firstFreeSlot;

		m_firstFreeSlot = objectHandle;
	}

	// Ensures no reallocation occurs until at least size 'numElements'.
	INLINE void Reserve( UINT numElements )
	{
		if( numElements > m_numAllocated )
		{
			const UINT newCapacity = Array_Util::CalculateNewCapacity( numElements );
			// grow the arrays
			this->Resize( newCapacity );
		}
	}

	// this is called if m_firstFreeSlot is InvalidIndex
	void Resize( UINT newCapacity )
	{
		const UINT oldCapacity = m_numAllocated;
		const UINT oldNumObjects = m_numObjects;
		void* oldMemoryBlock = m_objectsArray;

		// Allocate a new memory buffer

		const UINT sizeOfObjectsArray = newCapacity * sizeof OBJECT;
		const UINT sizeOfIndicesArray = newCapacity * sizeof SRemapObjIndex;
		const UINT sizeOfAllocatedMem = sizeOfObjectsArray + sizeOfIndicesArray;

		void* newMemBlock = F_GetMemoryManager(m_memoryHeap)->Allocate( sizeOfAllocatedMem );

		OBJECT* newObjectsArray = c_cast(OBJECT*) newMemBlock;
		SRemapObjIndex* newIndicesArray = c_cast(SRemapObjIndex*) ((BYTE*)newMemBlock + sizeOfObjectsArray);

		// Initialize the new memory block

		// call default constructors
		TConstructN_IfNonPOD< OBJECT >( newObjectsArray, newCapacity );


		// Initialize new indices.
		{
			const UINT iStartSlot = 0;	// this is the first free slot
			const UINT iLastSlot = newCapacity - 1;

			for( UINT i = iStartSlot; i < iLastSlot+1; i++ )
			{
				SRemapObjIndex & rIndex = newIndicesArray[ i ];
				rIndex.objectIndex = InvalidIndex;
				rIndex.nextFreeSlot = i + 1;
			}
			newIndicesArray[ iLastSlot ].nextFreeSlot = m_firstFreeSlot;

			m_firstFreeSlot = iStartSlot;
		}


		// copy the old data into the memory block
		if( oldNumObjects > 0 )
		{
			TCopyArray( newObjectsArray, m_objectsArray, oldNumObjects );
			TDestructN_IfNonPOD( m_objectsArray, oldNumObjects );

			// update the indices
			for( UINT iObj = 0; iObj < oldNumObjects; iObj++ )
			{
				OBJECT& rObj = newObjectsArray[ iObj ];

				const ObjectHandle oldObjectHandle = rObj.m_objectIndex;

				Assert( oldObjectHandle < m_numAllocated );

				SRemapObjIndex& rObjRef = newIndicesArray[ oldObjectHandle ];

				rObjRef.objectIndex = iObj;

				const UINT oldFreeSlot = m_firstFreeSlot;

				m_firstFreeSlot = rObjRef.nextFreeSlot;

				rObjRef.nextFreeSlot = oldFreeSlot;
			}

			this->ReleaseMemory( oldMemoryBlock );
		}

		m_objectsArray = newObjectsArray;
		m_indicesArray = newIndicesArray;

		m_numAllocated = newCapacity;
	}

	FORCEINLINE void ReleaseMemory( void* ptr )
	{
		F_GetMemoryManager( m_memoryHeap )->Free( ptr );
	}

public:
	friend inline mxArchive& operator && ( mxArchive & archive, THIS_TYPE & o )
	{
		U4 num = o.m_numObjects;
		archive && num;

		if( archive.IsReading() )
		{
			Assert(o.Num() == 0);
			o.Resize( num );
		}

		o.m_numObjects = num;

		for( UINT i = 0; i < num; i++ )
		{
			archive && o.m_objectsArray[ i ];
		}

		archive.SerializeMemory( o.m_indicesArray, num * sizeof o.m_indicesArray[0] );

		archive && o.m_firstFreeHandle;

		return archive;
	}
};
#endif

#if 0
/*
--------------------------------------------------------------
	TFreeListHandleManager< STRUCT, MAX_OBJECTS, CALLBACKS >

	objects are never moved in memory,
	so it's safe to keep pointers to them.

	objects are not always stored linearly
	(array may have holes after removals).
--------------------------------------------------------------
*/
template
<
	class STRUCT,
	UINT MAX_OBJECTS,
	class CALLBACKS = HandleManagerDefaultCallbacks<STRUCT>
>
struct TFreeListHandleManager
{
	typedef TStaticArray<STRUCT,MAX_OBJECTS>	ObjectsArray;

	STRUCT *	m_firstFree;

	ObjectsArray	m_objectsArray;

public:
	typedef TFreeListHandleManager< STRUCT, MAX_OBJECTS, CALLBACKS >	THIS_TYPE;
	typedef STRUCT	ITEM_TYPE;
	typedef TypeTrait< STRUCT >	TYPE_TRAIT;

	enum { InvalidIndex = MAX_UINT16 };
	enum { IndexMask = MAX_OBJECTS-1 };

	StaticAssert( MAX_OBJECTS >= 2 );
	StaticAssert( MAX_OBJECTS < MAX_UINT16 );
	StaticAssert( TIsPowerOfTwo< MAX_OBJECTS >::value == true );
	StaticAssert( TYPE_TRAIT::IsPlainOldDataType );

	struct Handle
	{
		STRUCT *	Ptr;
	};

public:
	TFreeListHandleManager()
	{
		this->Clear();
	}
	FORCEINLINE UINT GetCapacity() const
	{
		return MAX_OBJECTS;
	}
	void Clear()
	{
		//
	}
};
#endif


/*
--------------------------------------------------------------
	TPtrTable< OBJECT_TYPE, MAX_OBJECTS, INDEX_TYPE >

	represents an array of pointers to objects
--------------------------------------------------------------
*/
template< typename OBJECT_TYPE, UINT MAX_OBJECTS, typename INDEX_TYPE = U2 >
class TIndirectTable
	: public TArrayBase< OBJECT_TYPE*, TIndirectTable<OBJECT_TYPE,MAX_OBJECTS,INDEX_TYPE> >
{
	UINT	m_totalCount;
	UINT	m_firstFree;
	TStaticArray< OBJECT_TYPE*, MAX_OBJECTS >	m_ptrArray;	// maps indices to pointers
	TStaticArray< INDEX_TYPE, MAX_OBJECTS >		m_freeSlots;	// keeps free indices

public:
	TIndirectTable()
	{
		m_ptrArray.ZeroOut();
		m_firstFree = 0;
		m_totalCount = 0;
		for( UINT i=0; i < MAX_OBJECTS-1; i++)
		{
			m_freeSlots[i] = i+1;
		}
		m_freeSlots[ MAX_OBJECTS-1 ] = INDEX_NONE;
	}
	inline UINT Add( OBJECT_TYPE* p )
	{
		Assert(m_firstFree != INDEX_NONE);
		const UINT newPtrIndex = m_firstFree;
		m_ptrArray[ newPtrIndex ] = p;
		m_firstFree = m_freeSlots[ m_firstFree ];
		m_totalCount++;
		return newPtrIndex;
	}
	inline void Remove( UINT i )
	{
		Assert(m_totalCount > 0);
		m_ptrArray[ i ] = nil;
		m_freeSlots[ i ] = m_firstFree;
		m_firstFree = i;
		m_totalCount--;
	}

	FORCEINLINE UINT GetCapacity() const
	{
		return MAX_OBJECTS;
	}
	FORCEINLINE UINT Num() const
	{
		return m_totalCount;
	}
	FORCEINLINE OBJECT_TYPE** ToPtr()
	{
		return m_ptrArray.ToPtr();
	}
	FORCEINLINE const OBJECT_TYPE** ToPtr() const
	{
		return m_ptrArray.ToPtr();
	}
};




//class MemoryPoolBase
//{
//
//};
//
/*
-----------------------------------------------------------------------------
	TObjectPool

	keeps 'live' elements in a contiguous memory block;

	copies objects via memcpy() so make sure that they are bit-wise copyable
	(have a trivial assignment operator 'has_trivial_assign',)
	(take extreme care when storing linked-lists and pointers!).

	when removing an element:

	destroys the old object (by calling its destructor);
	moves the memory of the last object into the free slot with memcpy()
-----------------------------------------------------------------------------
*/
template< typename TYPE >
class TObjectPool
{
	UINT	m_maxAllowedNum;	// (soft) limit on maximum number of items, usually set in editor

public:
	TObjectPool()
	{
		//
	}
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//

