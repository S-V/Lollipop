// model.h
// .v3m model file format as used by Red Faction

#pragma once

/*
paper1.v3m
at
0038D000

paper1_v3m_offset
*/

#pragma pack (push,1)

// used to reference external objects by name
struct v3m_string
{
	BYTE	data[0x18];	// null-terminated string, 24 chars
};
MX_DECLARE_POD_TYPE(v3m_string);

// model batch information?
struct v3m_batch_info
{
	U4		id;	// 0000 418C
	BYTE	unkn[52];	// some flags

public:
	void Dbg_Print();
};
MX_DECLARE_POD_TYPE(v3m_batch_info);

// goes right after v3m_batch_info
struct v3m_padding
{
	BYTE	padding[10];	//10 zeros
};
MX_DECLARE_POD_TYPE(v3m_padding);

//
struct v3m_header_fixed_size_data 
{
	BYTE	fourCC[4];		//[0x00]4 "D3FR"
	U4		unkn0;			//[0x04]4 ? major version (e.g. 0000 0400)
	U4		unkn1;			//[0x08]4 ? minor version (e.g. 0100 0000)
	BYTE	unkn2[12];		//[0x0C]12 (0)
	U4		unkn2At24;		//[0x18]4 (0100 0000) or (0600 0000)
	BYTE	unkn3[12];		//[0x1C]12 (0)
	BYTE	magicAt40[4];	//[0x28]4 "MBUS"
	U4		unkn4;			//[0x00]4 (0)

	BYTE	unknStr1[24];	//[0x30]24 object name tag? e.g. "cables", "Plane01", "Plane03", "Parker", "Box01", "BOX", "Cylinder01", "OBJ_spool", "OBJ_Fighter01", "OBJ_Jeep01_A"
	BYTE	unknStr2[24];	//[0x48]24 e.g. "None"
	U4		unkn5;			//[0x60]4 ? version e.g. (0700 0000)
};
MX_DECLARE_POD_TYPE(v3m_header_fixed_size_data);


struct v3m_bounds
{
	Vec3D	center;			//12 model center
	F4		radius;			//4 bounding sphere radius
	Vec3D	aabbMin;		//12 bounding box minima
	Vec3D	aabbMax;		//12 bounding box maxima

public:
	void Dbg_Print();
};
MX_DECLARE_POD_TYPE(v3m_bounds);


enum { MAX_LODS = 4 };
enum { MAX_BATCHES = 8 };

/*
-----------------------------------------------------------------------------
	v3m_header
-----------------------------------------------------------------------------
*/
struct v3m_header : v3m_header_fixed_size_data
{
	U4		numLoDs;		//[0x68]4 number of levels of detail, >= 1
	F4		lodDistances[MAX_LODS];//[0x6C] F4[numLoDs] - LoD distances, start with 0.0f

	v3m_bounds	bounds;

	U4		unkn1;			//4 version? e.g. 2000 0000, 3000 0000
	U4		numVertices;	//4
	U2		numBatches;		//2 number of submeshes (parts with single texture), >= 1
	U2		unkn2;			//2

	v3m_batch_info	meshInfo[MAX_BATCHES];	// v3m_batch_info[numBatches]

public:
	v3m_header();
	~v3m_header();

	void Load( FileReader& file );

	void Dbg_Print();

private:	PREVENT_COPY(v3m_header);
};

struct v3m_triangle
{
	U2	indices[3];
	U2	pad_zero;	// null
};
MX_DECLARE_POD_TYPE(v3m_triangle);


/*
-----------------------------------------------------------------------------
	v3m_model
-----------------------------------------------------------------------------
*/
struct v3m_model
{
	v3m_header	header;

	TList< Vec3D >	positions;
	TList< Vec3D >	normals;
	TList< Vec2D >	texCoords;

	TList< v3m_triangle >	triangles;

	TList< Vec4D >	unknownFloats;	//bone weights?

public:
	v3m_model();

	void Load( FileReader& file );

	void Dbg_Print();

private:	PREVENT_COPY(v3m_model);
};

struct rfVertex
{
	Vec3D	position;		// 12 position
	Vec3D	normal;	// 12 normal
	Vec2D	texCoords;	// 8 texture coords 1
	Vec2D	lightMapTexCoords;		// 8 texture coords 2

	// 40 bytes
};

#pragma pack (pop)




extern BYTE gBuf[mxMEBIBYTE];






























#include <Renderer/Common.h>



/*
--------------------------------------------------------------
	TDynHandleMgr

	NOTE: objects are copied via operator =.

	Both objects and indices are stored in one array:

	_____________________________
	| IndicesData | ObjectsData |
	-----------------------------
	^
	start of allocated memory
--------------------------------------------------------------
*/
template< typename STRUCT, class CALLBACKS >
struct TDynHandleMgr
{
	UINT			m_numObjects;	// total number of objects
	UINT			m_firstFreeSlot;// index of the first free slot in the indices array
	STRUCT *		m_objectsArray;	// objects are always laid out contiguously without holes
	SRemapObjIndex*	m_indicesArray;	// array with 'holes'
	UINT			m_numAllocated;	// number of allocated entries
	const HMemory	m_memoryHeap;	// memory manager

public:
	typedef TDynHandleMgr< STRUCT, CALLBACKS >	THIS_TYPE;
	typedef STRUCT	ITEM_TYPE;
	typedef TypeTrait< STRUCT >	TYPE_TRAIT;

	enum { InvalidIndex = MAX_UINT16 };
	enum { MAX_OBJECTS = MAX_UINT16 };
	StaticAssert( TYPE_TRAIT::IsPlainOldDataType );

public:
	explicit TDynHandleMgr( HMemory hMemoryMgr = EMemHeap::DefaultHeap )
		: m_memoryHeap( hMemoryMgr )
	{
		m_numObjects = 0;
		m_firstFreeSlot = InvalidIndex;
		m_objectsArray = nil;
		m_indicesArray = nil;
		m_numAllocated = 0;
	}
	~TDynHandleMgr()
	{
		this->ReleaseMemory( m_indicesArray );
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
	FORCEINLINE STRUCT& GetRefById( ObjectHandle objectHandle )
	{
		Assert(objectHandle < m_numAllocated);
		AssertPtr(m_indicesArray);

		SRemapObjIndex & rIndex = m_indicesArray[ objectHandle ];
		Assert(rIndex.objectIndex < m_numObjects);
		AssertPtr(m_objectsArray);

		return m_objectsArray[ rIndex.objectIndex ];
	}
	FORCEINLINE STRUCT* GetObjectsArray()
	{
		AssertPtr(m_objectsArray);
		return m_objectsArray;
	}
	FORCEINLINE STRUCT & GetObjAt( UINT index )
	{
		Assert(index < m_numObjects);
		AssertPtr(m_objectsArray);
		return m_objectsArray[ index ];
	}
	FORCEINLINE const STRUCT & GetObjAt( UINT index ) const
	{
		Assert(index < m_numObjects);
		AssertPtr(m_objectsArray);
		return m_objectsArray[ index ];
	}
	inline STRUCT* Add()
	{
		this->Reserve( m_numObjects + 1 );

		Assert( m_firstFreeSlot != InvalidIndex );

		const ObjectHandle newObjID = m_firstFreeSlot;

		SRemapObjIndex & rFreeSlot = m_indicesArray[ m_firstFreeSlot ];

		const UINT oldFreeSlot = m_firstFreeSlot;
		m_firstFreeSlot = rFreeSlot.nextFreeSlot;

		rFreeSlot.objectIndex = m_numObjects++;
		rFreeSlot.nextFreeSlot = oldFreeSlot;

		STRUCT & rObject = m_objectsArray[ rFreeSlot.objectIndex ];

		//rObject.m_objectIndex = newObjID;
		CALLBACKS::OnObjectAdded( rObject, newObjID );

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
			// ... then swap the object being deleted with the last element
			STRUCT & lastObj = m_objectsArray[ lastObjIndex ];

			m_objectsArray[ thisObjIndex ] = lastObj;

			// update the reference to the last object
			SRemapObjIndex & rLastObjRef = m_indicesArray[ lastObj.m_objectIndex ];
			rLastObjRef.objectIndex = thisObjIndex;
		}

		// ...otherwise, we're deleting the last element.

		rObjRef.objectIndex = InvalidIndex;
		rObjRef.nextFreeSlot = m_firstFreeSlot;

		m_firstFreeSlot = objectHandle;

		//CALLBACKS::OnObjectRemoved( rObject, newObjID );
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
		void* oldMemoryBlock = m_indicesArray;

		// Allocate a new memory buffer

		const UINT sizeOfObjectsArray = newCapacity * sizeof STRUCT;
		const UINT sizeOfIndicesArray = newCapacity * sizeof SRemapObjIndex;
		const UINT sizeOfAllocatedMem = sizeOfObjectsArray + sizeOfIndicesArray;

		BYTE* newMemBlock = (BYTE*) F_GetMemoryManager(m_memoryHeap)->Allocate( sizeOfAllocatedMem );

		SRemapObjIndex* newIndicesArray = c_cast(SRemapObjIndex*) newMemBlock;
		STRUCT* newObjectsArray = TAlignSimd( c_cast(STRUCT*) (newMemBlock + sizeOfIndicesArray) );


		// Initialize the new memory block

		// call default constructors
		TConstructN_IfNonPOD< STRUCT >( newObjectsArray, newCapacity );


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
				STRUCT& rObj = newObjectsArray[ iObj ];

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

		archive && o.m_firstFreeSlot;

		return archive;
	}
};
