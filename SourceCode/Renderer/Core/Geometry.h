/*
=============================================================================
	File:	Geometry.h
	Desc:	structures for loading renderable geometry
	ToDo:	move to low-level Graphics
=============================================================================
*/
#pragma once

#include <Core/Resources.h>

#include <Renderer/GPU/VertexFormats.hxx>

/*
-----------------------------------------------------------------------------
	VertexBuffer
-----------------------------------------------------------------------------
*/
struct VertexBuffer
{
	TList<BYTE>	data;
	UINT		stride;	// the size (in bytes) of a single element

public:
	VertexBuffer( HMemory heap = EMemHeap::HeapStreaming )
		: data(heap)
		, stride(0)
	{
	}
	VertexBuffer(_FinishedLoadingFlag x)
		: data(x)
	{}

	template< class S >
	friend S& operator & ( S & serializer, VertexBuffer & o )
	{
		return serializer & o.data & o.stride;
	}
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		data.CollectPointers( s, p );
	}

	// Returns the number of vertices in the buffer.

	FORCEINLINE UINT NumVertices() const
	{
		Assert(stride > 0);
		return data.GetDataSize() / stride;
	}

	FORCEINLINE UINT SizeInBytes() const
	{
		return data.GetDataSize();
	}

	FORCEINLINE void SetNum( UINT inVertices )
	{
		Assert(stride > 0);
		data.SetNum( stride * inVertices );
	}

	FORCEINLINE void SetSize( UINT inBytes )
	{
		Assert(stride > 0);
		Assert(inBytes % stride == 0);
		data.SetNum( inBytes );
	}
};

/*
-----------------------------------------------------------------------------
	VertexStream
-----------------------------------------------------------------------------
*/
struct VertexStream : VertexBuffer
{
	// the number of bytes between the first element of a vertex buffer
	// and the first element that will be used
	//UINT	offset;

public:
	VertexStream( HMemory heap = EMemHeap::HeapStreaming )
		: VertexBuffer( heap )
		//: offset(0)
	{
	}
	VertexStream(_FinishedLoadingFlag x)
		: VertexBuffer(x)
	{}

	template< class S >
	friend S& operator & ( S & serializer, VertexStream & o )
	{
		return serializer & ((VertexBuffer&)o);
	}
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		VertexBuffer::CollectPointers( s, p );
	}

	FORCEINLINE BYTE* ToVoidPtr()
	{
		return this->data.ToPtr();
	}
	FORCEINLINE const BYTE* ToVoidPtr() const
	{
		return this->data.ToPtr();
	}
};

/*
-----------------------------------------------------------------------------
	VertexData

	raw vertex data
-----------------------------------------------------------------------------
*/
struct VertexData
{
	TList< VertexStream >	streams;	// raw vertex data
	rxVertexFormatID		formatID;	// tells us how to interpret raw bytes of vertex data

public:
	template< class S >
	friend S& operator & ( S & serializer, VertexData & o )
	{
		return serializer & o.streams & o.formatID
			;
	}
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		streams.CollectPointers( s, p );
	}

	VertexData( HMemory heap = EMemHeap::HeapStreaming )
		: streams(heap)
		, formatID(INDEX_NONE)
	{
	}
	VertexData(_FinishedLoadingFlag x)
		: streams(x)
	{}
};

/*
-----------------------------------------------------------------------------
	IndexData

	raw index data
-----------------------------------------------------------------------------
*/
struct IndexData
{
	TList<BYTE>	data;
	UINT		stride;

public:
	IndexData( HMemory heap = EMemHeap::HeapStreaming )
		: data(heap)
		, stride(0)
	{
	}
	IndexData(_FinishedLoadingFlag x)
		: data(x)
	{}

	void Clear()
	{
		data.Empty();
		stride = 0;
	}

	// Returns the number of indices in the buffer.

	FORCEINLINE UINT NumIndices() const
	{
		return data.GetDataSize() / stride;
	}

	FORCEINLINE UINT SizeInBytes() const
	{
		return data.GetDataSize();
	}

	template< class S >
	friend S& operator & ( S & serializer, IndexData & o )
	{
		return serializer & o.data & o.stride;
	}
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		data.CollectPointers( s, p );
	}

	template< typename INDEX_TYPE >
	inline void CopyFrom( const INDEX_TYPE* srcIndices, UINT numIndices )
	{
		AssertPtr(srcIndices);
		Assert(numIndices > 0);

		const UINT indexSize = sizeof srcIndices[0];
		const UINT indexDataSize = indexSize * numIndices;

		data.SetNum( indexDataSize );
		MemCopy( data.ToPtr(), srcIndices, indexDataSize );

		stride = indexSize;
	}

	template< typename INDEX_TYPE >
	FORCEINLINE INDEX_TYPE* ToTypedPtr()
	{
		Assert( sizeof INDEX_TYPE == stride );
		return c_cast(INDEX_TYPE*) data.ToPtr();
	}
	template< typename INDEX_TYPE >
	FORCEINLINE const INDEX_TYPE* ToTypedPtr() const
	{
		Assert( sizeof INDEX_TYPE == stride );
		return c_cast(const INDEX_TYPE*) data.ToPtr();
	}

	FORCEINLINE void* ToVoidPtr() {return data.ToPtr();}
	FORCEINLINE const void* ToVoidPtr() const {return data.ToPtr();}

	FORCEINLINE U2* ToU16() {return ToTypedPtr<U2>();}
	FORCEINLINE const U2* ToU16() const {return ToTypedPtr<const U2>();}

	FORCEINLINE U4* ToU32() {return ToTypedPtr<U4>();}
	FORCEINLINE const U4* ToU32() const {return ToTypedPtr<const U4>();}
};

/*
-----------------------------------------------------------------------------
	MeshData
	Raw mesh data, which is used for loading/filling hardware mesh buffers
-----------------------------------------------------------------------------
*/
struct RawMeshData
{
	VertexData		vertexData;	// raw vertex data
	IndexData		indexData;	// raw index data
	rxTopology		topology;	// primitive topology
	rxAABB			localBounds;

public:
	mxDECLARE_CLASS_ALLOCATOR(EMemHeap::HeapStreaming,RawMeshData);

	inline RawMeshData::RawMeshData( HMemory heap = EMemHeap::HeapStreaming )
		: topology(EPrimitiveType::PT_Unknown)
	{
		rxAABB_Infinity(localBounds);
	}

	inline RawMeshData(_FinishedLoadingFlag x)
		: vertexData(x)
		, indexData(x)
	{}

	template< class VERTEX >
	void Build( const IndexedMesh& srcMesh )
	{
		this->Clear();

		// Vertex data.

		VERTEX::AssembleVertexData( srcMesh, this->vertexData );

		this->vertexData.formatID = VERTEX::UID;

		// Index data.

		if( srcMesh.indexStride == sizeof U4 )
		{
			const U4* srcIndices = c_cast( const U4* )( srcMesh.indices );
			this->indexData.CopyFrom( srcIndices, srcMesh.numIndices );
		}
		else if( srcMesh.indexStride == sizeof U2 )
		{
			const U2* srcIndices = c_cast( const U2* )( srcMesh.indices );
			this->indexData.CopyFrom( srcIndices, srcMesh.numIndices );
		}
		else {
			Unreachable;
		}

		// Primitive topology.

		this->topology = srcMesh.GetPrimTopology();

		rxAABB_From_AABB( this->localBounds, srcMesh.bounds );
	}

public:	// Serialization
	template< class S >
	friend S& operator & ( S & serializer, RawMeshData & o )
	{
		//UINT32 tag('MESH');
		//serializer & tag;
		return serializer & o.vertexData & o.indexData & o.topology;
	}
	template< class S, class P > inline void CollectPointers( S & s, P p ) const
	{
		vertexData.CollectPointers( s, p );
		indexData.CollectPointers( s, p );
	}

	bool DbgCheckValid() const;

	void Clear();
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
