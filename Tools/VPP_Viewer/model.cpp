// .v3m model loading
#include "stdafx.h"
#pragma hdrstop
#include "model.h"

void v3m_batch_info::Dbg_Print()
{
	const UINT sizeInDWords = 56/4;
	StaticAssert( sizeof *this == sizeInDWords * sizeof U4 );
	DEVOUT("=== Model batch header: ===\n");
	for( UINT i=0; i < sizeInDWords; i++ )
	{
		const U4 x = (c_cast(const U4*) this) [i];
		DEVOUT("0x%X ", x);
	}
	DEVOUT("\n");
}

void v3m_bounds::Dbg_Print()
{
	DEVOUT("Sphere: (%f %f %f, R=%f)\n"
		"AABB: min(%f %f %f), max(%f %f %f)\n",
		center.x, center.y, center.z, radius,
		aabbMin.x, aabbMin.y, aabbMin.z, aabbMax.x, aabbMax.y, aabbMax.z );
}

/*
-----------------------------------------------------------------------------
	v3m_header
-----------------------------------------------------------------------------
*/
v3m_header::v3m_header()
{
	ZERO_OUT(*this);
}

v3m_header::~v3m_header()
{
}

void v3m_header::Load( FileReader& file )
{
	file >> (v3m_header_fixed_size_data&) *this;

	file >> numLoDs;
	Assert( numLoDs < MAX_LODS );
	numLoDs &= MAX_LODS-1;

	for( UINT iLodLevel = 0; iLodLevel < numLoDs; iLodLevel++ )
	{
		file >> lodDistances[ iLodLevel ];
	}

	file >> bounds;

	file >> unkn1;
	file >> numVertices;
	file >> numBatches;
	file >> unkn2;

	for( UINT iModelBatch = 0; iModelBatch < numBatches; iModelBatch++ )
	{
		file >> meshInfo[ iModelBatch ];
	}

	v3m_padding	dummy;
	file >> dummy;

	// dummy is filled with zeros
}

void v3m_header::Dbg_Print()
{
	bounds.Dbg_Print();

	for( UINT iModelBatch = 0; iModelBatch < numBatches; iModelBatch++ )
	{
		meshInfo[ iModelBatch ].Dbg_Print();
	}

	DEVOUT("%u vertices, %u batch(es)\n", numVertices,numBatches);
}

/*
-----------------------------------------------------------------------------
	v3m_model
-----------------------------------------------------------------------------
*/
v3m_model::v3m_model()
{
}

void v3m_model::Load( FileReader& file )
{
	header.Load( file );

	//DBG
	header.numVertices = 100;

	const UINT numVerts = header.numVertices;

	positions.SetNum( numVerts );
	normals.SetNum( numVerts );
	texCoords.SetNum( numVerts );


	const UINT nPositionsOffset = file.Tell();
	DEVOUT("Positions at offset %u bytes from beginning.\n", nPositionsOffset);
	file.Read( positions.ToPtr(), positions.GetDataSize() );

	const UINT nNormalsOffset = file.Tell();
	DEVOUT("Normals at offset %u bytes from beginning.\n", nNormalsOffset);
	file.Read( normals.ToPtr(), normals.GetDataSize() );

	const UINT nTexCoordsOffset = file.Tell();
	DEVOUT("TexCoords at offset %u bytes from beginning.\n", nTexCoordsOffset);
	file.Read( texCoords.ToPtr(), texCoords.GetDataSize() );


	const UINT nIndexDataOffset = file.Tell();
	(void)nIndexDataOffset;

	MX_UNDONE("don't know yet where to get this info");
	// 12 triangles in "minifridge1.v3m"
	const UINT numTriangles = 12;

	triangles.SetNum( numTriangles );
	for( UINT iTriangle = 0; iTriangle < numTriangles; iTriangle++ )
	{
		file >> triangles[ iTriangle ];
	}

	unknownFloats.SetNum( numTriangles );
	for( UINT iWeight= 0; iWeight < numTriangles; iWeight++ )
	{
		file >> unknownFloats[ iWeight ];
	}
}

void v3m_model::Dbg_Print()
{
	DEVOUT("==== Dumping .v3m model: ====\n");

	header.Dbg_Print();

	const UINT numVerts = header.numVertices;
	const F4 slack = 0.001f;
	const Sphere sphere( header.bounds.center, header.bounds.radius + slack );
	const AABB aabb = AABB( header.bounds.aabbMin, header.bounds.aabbMax ).ExpandSelf(slack);

	DEVOUT("\n==== Vertex positions ====\n");
	for( UINT iVertex = 0; iVertex < numVerts; iVertex++ )
	{
		const Vec3D& pos = positions[ iVertex ];
		const F4 length = pos.GetLength();

		const bool bInsideSphere = sphere.ContainsPoint( pos );
		const bool bInsideBox = aabb.ContainsPoint( pos );

		const char* dbgMsg = (bInsideSphere && bInsideBox) ? "" : ", OUTSIDE BOUNDS!";

		DEVOUT("XYZ[%u]: (%f %f %f), length = %f%s\n",
			iVertex, pos.x, pos.y, pos.z, length, dbgMsg );
	}

	DEVOUT("\n==== Vertex normals ====\n");
	for( UINT iVertex = 0; iVertex < numVerts; iVertex++ )
	{
		const Vec3D& normal = normals[ iVertex ];

		const F4 length = normal.GetLength();

		DEVOUT("N[%u]: (%f %f %f), length = %f\n",
			iVertex, normal.x, normal.y, normal.z, length );
	}

	DEVOUT("\n==== Texture coordinates ====\n");
	for( UINT iVertex = 0; iVertex < numVerts; iVertex++ )
	{
		const Vec2D& uv = texCoords[ iVertex ];

		DEVOUT("UV[%u]: (%f %f)\n",
			iVertex, uv.x, uv.y );
	}


	DEVOUT("\n==== Triangles ====\n");
	for( UINT iTriangle = 0; iTriangle < triangles.Num(); iTriangle++ )
	{
		const v3m_triangle& tri = triangles[ iTriangle ];

		const UINT index1 = tri.indices[0];
		const UINT index2 = tri.indices[1];
		const UINT index3 = tri.indices[2];

		DEVOUT("Tri[%u]: (%u %u %u)\n",
			iTriangle, index1, index2, index3 );
	}


	DEVOUT("\n==== Collision planes? ====\n");
	for( UINT iWeight = 0; iWeight < triangles.Num(); iWeight++ )
	{
		const Vec4D& v = unknownFloats[ iWeight ];
	
		DEVOUT("[%u]: (%f %f %f %f)\n",
			iWeight, v.x, v.y, v.z, v.w );
	}

	// then goes padding
}
