#pragma once

//#include <Renderer/Core/Material.h>

#if 0
//
//	rxPrimitiveGroup - Defines a group of primitives as a subset of a vertex buffer and index
//	buffer plus the primitive topology and object-space bounds.
//
struct rxPrimitiveGroup
{
	UINT 					startVertex;
	UINT 					numVertices;
	UINT 					startIndex;
	UINT 					numIndices;
	EPrimitiveType::Code	topology;
	AABB					localBounds;	// object-space bounds of this batch

public:
	rxPrimitiveGroup()
		: startVertex(0), numVertices(0)
		, startIndex(0), numIndices(0)
		, topology( EPrimitiveType::PT_Unknown )
		, localBounds( AABB::invalid_aabb )
	{}

	bool isOk() const
	{
		return 1
			&& CHK(startVertex >= 0)
			&& CHK(numVertices > 0)
			&& CHK(startIndex >= 0)
			&& CHK(numIndices > 0)
			&& CHK(topology != EPrimitiveType::PT_Unknown)
			&& CHK(localBounds.IsValid())
			;
	}
};

// a subset of a vertex buffer and index buffer usually associated with a single material
struct rxSubmesh
{
	S32		baseVertex;	// index of the first vertex
	U32		startIndex;	// offset of the first index
	U16		indexCount;	// number of indices
	U16		vertexCount;// number of vertices

public:
	inline rxSubmesh()
		: baseVertex( 0 )
		, startIndex( 0 )
		, indexCount( 0 )
		, vertexCount( 0 )
	{}
};

template<>
struct TypeTrait< rxSubmesh >
{
	static bool IsPlainOldDataType() { return true; }
};
#endif


#if 0
/*
-----------------------------------------------------------------------------
	EdSubMesh - part of mesh (mesh subset) associated with a single material
-----------------------------------------------------------------------------
*/
struct EdSubMesh
{
	U4				baseVertex;	// index of the first vertex
	U4				startIndex;	// offset of the first index
	U4				indexCount;	// number of indices
	U4				vertexCount;// number of vertices
	Sphere			bounds;		// bounds in local space
	ObjectGUID		material;	// material id (usually assigned in the editor)

public:

	EdSubMesh();
	~EdSubMesh();

	bool isOk() const;

	template< class S >
	friend S& operator & ( S & serializer, EdSubMesh & o )
	{
		return serializer
			& o.baseVertex & o.startIndex & o.indexCount & o.vertexCount
			& o.bounds
			& o.material
			;
	}
	inline friend mxArchive& operator && ( mxArchive & serializer, EdSubMesh & o )
	{
		return Serialize_ArcViaBin( serializer, o );
	}

	mxNO_SERIALIZABLE_POINTERS;

	// Editor
	void edCreateProperties( EdPropertyList *properties, bool bClear = true );
};

/*
-----------------------------------------------------------------------------
	rxModelBatch

	part of mesh typically associated with a single material.
	this structure is actually used at run time for rendering.
-----------------------------------------------------------------------------
*/
struct rxModelBatch
{
	rxMaterial::Ref	material;	// material id

	U4		indexCount;	// number of indices
	U4		startIndex;	// offset of the first index
	U4		baseVertex;	// index of the first vertex

	//Sphere	bounds;	// bounds in local space (VFC and quick depth sorting)

public:
	rxModelBatch();

	void Setup( const EdSubMesh& source );

	friend mxArchive& operator && ( mxArchive & serializer, rxModelBatch & o );

	void edCreateProperties( EdPropertyList *properties, bool bClear = true );
};

typedef TFixedSizeArray< rxModelBatch, U2 >	rxModelBatchList;

#if MX_EDITOR

struct HModelBatch : AHitProxy
{
	mxDECLARE_ABSTRACT_CLASS( HModelBatch, AHitProxy );

	TPtr<rxSpatialObject>	pEntity;
	TPtr<rxModelBatch>		pBatch;

	HModelBatch()
	{
	}
	virtual rxSpatialObject* GetParentEntity() override {return pEntity;}

	//virtual EMouseCursor GetMouseCursor() const {return Mouse_OpenHandCursor;}
	//virtual EMouseCursor GetMouseDragCursor() const {return Mouse_ClosedHandCursor;}
};
#endif // MX_EDITOR

#endif