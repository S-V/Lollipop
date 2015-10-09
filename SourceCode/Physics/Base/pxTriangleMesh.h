/*
=============================================================================
	File:	pxTriangleMesh.h
	Desc:	
=============================================================================
*/

#ifndef __PX_TRIANGLE_MESH_H__
#define __PX_TRIANGLE_MESH_H__

#include <Physics/Base/pxVec3.h>

/*
-----------------------------------------------------------------------------
	pxTriangleIndexCallback
-----------------------------------------------------------------------------
*/
struct pxTriangleIndexCallback
{
	virtual void ProcessTriangle( const Vec3D& p0, const Vec3D& p1, const Vec3D& p2 ) = 0;

	virtual ~pxTriangleIndexCallback() {}
};

/*
-----------------------------------------------------------------------------
	pxTriangleIndexCallback_ComputeAABB
-----------------------------------------------------------------------------
*/
struct pxTriangleIndexCallback_ComputeAABB : pxTriangleIndexCallback
{
	AABB	aabb;

	pxTriangleIndexCallback_ComputeAABB()
	{
		aabb.Clear();
	}
	virtual void ProcessTriangle( const Vec3D& p0, const Vec3D& p1, const Vec3D& p2 ) override
	{
		aabb.AddPoint( p0 );
		aabb.AddPoint( p1 );
		aabb.AddPoint( p2 );
	}
};

/*
-----------------------------------------------------------------------------
	pxTriangleMeshInterface
-----------------------------------------------------------------------------
*/
struct pxTriangleMeshInterface
{
	virtual ~pxTriangleMeshInterface() {}

	virtual void ProcessAllTriangles( pxTriangleIndexCallback* callback ) = 0;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< typename INDEX_TYPE >
void F_ProcessAllTriangles(
						   const INDEX_TYPE* indices, const UINT numTriangles,
						   const Vec3D* positions, const UINT numVertices,
						   pxTriangleIndexCallback* callback
						   )
{
	StaticAssert( sizeof indices[0] == 2 || sizeof indices[0] == 4 );

	for( UINT iTriangle = 0; iTriangle < numTriangles; iTriangle++ )
	{
		const UINT	idx0 = indices[ iTriangle*3 + 0 ];
		const UINT	idx1 = indices[ iTriangle*3 + 1 ];
		const UINT	idx2 = indices[ iTriangle*3 + 2 ];

		callback->ProcessTriangle(
			positions[idx0],
			positions[idx1],
			positions[idx2]
			);
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct STriangle
{
	Vec3D	vertices[3];
};

typedef TList< STriangle >	TriangleList;

struct pxTriangleCollector : pxTriangleIndexCallback
{
	TriangleList &	m_triangleList;

	pxTriangleCollector( TriangleList & triangleList )
		: m_triangleList( triangleList )
	{
	}
	virtual void ProcessTriangle( const Vec3D& p0, const Vec3D& p1, const Vec3D& p2 ) override
	{
		STriangle & newTriangle = m_triangleList.Add();

		newTriangle.vertices[0] = p0;
		newTriangle.vertices[1] = p1;
		newTriangle.vertices[2] = p2;
	}
};

#endif // !__PX_TRIANGLE_MESH_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
