/*
=============================================================================
	File:	LightShapes.cpp
	Desc:	Light shape meshes (for deferred lighting).
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include "DeferredLighting.h"

// enlarge low-poly point light shape by this value to hide its 'low-polyness'
static const FLOAT LIGHT_SHAPE_SCALE = 1.05f;//1.165f;


/*================================
		rxLightShape
================================*/

rxLightShape::rxLightShape()
	: m_numVertices( 0 )
	, m_numIndices( 0 )
{}

rxLightShape::~rxLightShape()
{
	this->Clear();
}

void rxLightShape::Clear()
{
	m_vertices.Clear();
	m_indices.Clear();

	m_numVertices = 0;
	m_numIndices = 0;
}

void rxLightShape::CreateBuffers( const LSVertexList& vertices, const LSIndexList& indices )
{
	AssertX( indices.Num() < MAX_UINT16, "only 16-bit indices allowed");
	Assert( indices.Num() % 3 == 0 );

	m_vertices.Create(
		vertices.ToPtr(),
		vertices.GetDataSize()
	);

	graphics.resources->Create_IndexBuffer(
		m_indices,
		indices.GetDataSize(),
		indices.GetItemSize(),
		indices.ToPtr()
	);

	mxOPTIMIZE("reorder index list for optimal performance (vertex cache coherency,etc)?"
		"use triangle strips instead of lists? store optimized mesh data embedded in C++ source files?"
		"use several LoDs?");

	/*
	from GDC 2011:
	Always make sure your light volumes are geometry-optimized!
	For both index re-use (post VS cache) and sequential vertex reads (pre VS cache)
	Common oversight for algorithmically generated meshes (spheres, cones, etc.)
	Especially important when depth/stencil-only rendering is used!!
	No pixel shader = more likely to be VS fetch limited!
	*/
}

/*================================
	rxLightShape_Sphere
================================*/

rxLightShape_Sphere::rxLightShape_Sphere()
	: m_radius( 1.0f )
	, m_numSlices( 4 )
	, m_numStacks( 4 )
{}

rxLightShape_Sphere::~rxLightShape_Sphere()
{}

void rxLightShape_Sphere::Build( FLOAT radius, UINT numSlices, UINT numStacks )
{
	Assert( radius > 0 && numSlices < 64 && numStacks < 64 );
	this->Clear();

	m_radius    = radius;
	m_numSlices = numSlices;
	m_numStacks = numStacks;

	LSVertexList	vertices;
	LSIndexList		indices;

	this->BuildStacks( vertices, indices );

	m_numVertices	= (UINT) vertices.Num();
	m_numIndices	= (UINT) indices.Num();

	this->CreateBuffers( vertices, indices );
}

void rxLightShape_Sphere::BuildStacks( LSVertexList & vertices, LSIndexList & indices )
{
	FLOAT phiStep = MX_PI / (FLOAT) m_numStacks;

	// do not count the poles as rings
	UINT numRings = m_numStacks-1;

	// Compute vertices for each stack ring.
	for ( UINT i = 1; i <= numRings; ++i )
	{
		FLOAT phi = i * phiStep;

		// vertices of ring
		FLOAT thetaStep = MX_TWO_PI / m_numSlices;
		for ( UINT j = 0; j <= m_numSlices; ++j )
		{
			FLOAT theta = j * thetaStep;

			LSVertex v;

			FLOAT sinPhi, cosPhi, sinTheta, cosTheta;
			mxSinCos( phi, sinPhi, cosPhi );
			mxSinCos( theta, sinTheta, cosTheta );

			// spherical to cartesian
			v.xyz.x = m_radius * sinPhi* cosTheta;
			v.xyz.y = m_radius * cosPhi;
			v.xyz.z = m_radius * sinPhi * sinTheta;
#if 0
			// partial derivative of P with respect to theta
			v.tangent.x = -this->radius*Math::Sin(phi)*Math::Sin(theta );
			v.tangent.y = 0.0f;
			v.tangent.z = this->radius*Math::Sin(phi)*Math::Cos(theta );

			D3DXVec3Normalize(&v.normal, &v.pos );

			v.texC.x = theta / (2.0f*PI );
			v.texC.y = phi / PI;
#endif
			vertices.Add( v  );
		}
	}

#if 0
	// poles: note that there will be texture coordinate distortion
	vertices.Add( LSVertex(0.0f, -this->radius, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f) );
	vertices.Add( LSVertex(0.0f, this->radius, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f) );
#else
	vertices.Add( LSVertex( 0.0f, -m_radius, 0.0f ) );
	vertices.Add( LSVertex( 0.0f, m_radius, 0.0f ) );
#endif

	UINT northPoleIndex = (UINT) vertices.Num() - 1;
	UINT southPoleIndex = (UINT) vertices.Num() - 2;

	UINT numRingVertices = m_numSlices + 1;

	// Compute indices for inner stacks (not connected to poles).
	for ( UINT i = 0; i < m_numStacks-2; ++i )
	{
		for( UINT j = 0; j < m_numSlices; ++j )
		{
			indices.Add( i*numRingVertices + j );
			indices.Add( i*numRingVertices + j+1 );
			indices.Add((i+1)*numRingVertices + j );

			indices.Add(( i+1)*numRingVertices + j );
			indices.Add( i*numRingVertices + j+1 );
			indices.Add( (i+1)*numRingVertices + j+1 );
		}
	}

	// Compute indices for top stack.  The top stack was written 
	// first to the vertex buffer.
	for(UINT i = 0; i < m_numSlices; ++i)
	{
		indices.Add(northPoleIndex );
		indices.Add(i+1 );
		indices.Add(i );
	}

	// Compute indices for bottom stack.  The bottom stack was written
	// last to the vertex buffer, so we need to offset to the index
	// of first vertex in the last ring.
	UINT baseIndex = ( numRings - 1 ) * numRingVertices;
	for( UINT i = 0; i < m_numSlices; ++i )
	{
		indices.Add(southPoleIndex );
		indices.Add(baseIndex+i );
		indices.Add(baseIndex+i+1 );
	}
}

/*================================
	rxLightShape_Cylinder
================================*/

rxLightShape_Cylinder::rxLightShape_Cylinder()
	: m_topRadius	( 1.0f )
	, m_bottomRadius( 1.0f )
	, m_height		( 1.0f )

	, m_numSlices( 4 )
	, m_numStacks( 4 )
{}

rxLightShape_Cylinder::~rxLightShape_Cylinder()
{}

void rxLightShape_Cylinder::Build(
	FLOAT topRadius, FLOAT bottomRadius, FLOAT height,
	UINT numSlices, UINT numStacks,
	const Matrix4* shapeTransform )
{
	m_topRadius    = topRadius;
	m_bottomRadius = bottomRadius;
	m_height       = height;
	m_numSlices    = numSlices;
	m_numStacks    = numStacks;

	LSVertexList	vertices;
	LSIndexList		indices;

	this->BuildStacks( vertices, indices );
	this->BuildTopCap( vertices, indices );
	this->BuildBottomCap( vertices, indices );

	if(0)
	{
		AABB	boundsLocal;
		boundsLocal.Clear();
		for(UINT iVertex = 0; iVertex < (UINT)vertices.Num(); iVertex++)
		{
			boundsLocal.AddPoint( vertices[ iVertex ].xyz );
		}
		DBGOUT("Cylinder bounds before transform: min(%s), max(%s)\n",
			boundsLocal.GetMin().ToChars(),boundsLocal.GetMax().ToChars() );
	}

	if( shapeTransform != nil )
	{
		for(UINT iVertex = 0; iVertex < (UINT)vertices.Num(); iVertex++)
		{
			vertices[ iVertex ].xyz = shapeTransform->TransformVector( vertices[ iVertex ].xyz );
		}
	}

	if(0)
	{
		AABB	boundsLocal;
		boundsLocal.Clear();
		for(UINT iVertex = 0; iVertex < (UINT)vertices.Num(); iVertex++)
		{
			boundsLocal.AddPoint( vertices[ iVertex ].xyz );
		}
		DBGOUT("Cylinder bounds after transform: min(%s), max(%s)\n",
			boundsLocal.GetMin().ToChars(),boundsLocal.GetMax().ToChars() );
	}

	m_numVertices = (UINT)vertices.Num();
	m_numIndices = (UINT)indices.Num();

	this->CreateBuffers( vertices, indices );
}

void rxLightShape_Cylinder::BuildStacks( LSVertexList & vertices, LSIndexList & indices )
{
	FLOAT stackHeight = m_height / m_numStacks;

	// Amount to increment radius as we move up each stack level from bottom to top.
	FLOAT radiusStep = (m_topRadius - m_bottomRadius) / m_numStacks;

	UINT numRings = m_numStacks+1;

	// Compute vertices for each stack ring.
	for(UINT i = 0; i < numRings; ++i)
	{
		FLOAT y = -0.5f*m_height + i*stackHeight;
		FLOAT r = m_bottomRadius + i*radiusStep;

		// Height and radius of next ring up.
		FLOAT y_next = -0.5f*m_height + (i+1)*stackHeight;
		FLOAT r_next = m_bottomRadius + (i+1)*radiusStep;
	 
		// vertices of ring
		FLOAT dTheta = MX_TWO_PI / m_numSlices;
		for(UINT j = 0; j <= m_numSlices; ++j)
		{
			FLOAT s, c;
			mxSinCos( j*dTheta, s, c );
#if 0
			FLOAT u = (FLOAT)j/this->numSlices;
			FLOAT v = 1.0f - (FLOAT)i/this->numStacks;

			// Partial derivative in theta direction to get tangent vector (this is a unit vector).
			D3DXVECTOR3 T(-s, 0.0f, c);

			// Compute tangent vector down the slope of the cone (if the top/bottom 
			// radii differ then we get a cone and not a true cylinder).
			D3DXVECTOR3 P(r*c, y, r*s);
			D3DXVECTOR3 P_next(r_next*c, y_next, r_next*s);
			D3DXVECTOR3 B = P - P_next;
			D3DXVec3Normalize(&B, &B);

			D3DXVECTOR3 N;
			D3DXVec3Cross(&N, &T, &B);
			D3DXVec3Normalize(&N, &N);

			vertices.Add( LSVertex(P.x, P.y, P.z, T.x, T.y, T.z, N.x, N.y, N.z, u, v) );
#else
			(void) y_next, (void) r_next;
			vertices.Add( LSVertex( r*c, y, r*s ) );
#endif
		}
	}

	UINT numRingVertices = m_numSlices+1;

	// Compute indices for each stack.
	for(UINT i = 0; i < m_numStacks; ++i)
	{
		for(UINT j = 0; j < m_numSlices; ++j)
		{
			indices.Add(i*numRingVertices + j);
			indices.Add((i+1)*numRingVertices + j);
			indices.Add((i+1)*numRingVertices + j+1);

			indices.Add(i*numRingVertices + j);
			indices.Add((i+1)*numRingVertices + j+1);
			indices.Add(i*numRingVertices + j+1);
		}
	}
}

void rxLightShape_Cylinder::BuildBottomCap( LSVertexList & vertices, LSIndexList & indices )
{
	UINT baseIndex = (UINT)vertices.Num();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	FLOAT y = -0.5f*m_height;

	// vertices of ring
	FLOAT dTheta = MX_TWO_PI / m_numSlices;
	for(UINT i = 0; i <= m_numSlices; ++i)
	{
		FLOAT sine, cosine;
		mxSinCos( i*dTheta, sine, cosine );

		FLOAT x = m_bottomRadius * cosine;
		FLOAT z = m_bottomRadius * sine;
#if 0
		// Map [-1,1]-->[0,1] for planar texture coordinates.
		FLOAT u = +0.5f * x/this->bottomRadius + 0.5f;
		FLOAT v = -0.5f * z/this->bottomRadius + 0.5f;

		vertices.Add( LSVertex(x, y, z, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, u, v) );
#else
		vertices.Add( LSVertex( x, y, z ) );
#endif
	}

	// cap center vertex
#if 0
	vertices.Add( LSVertex(0.0f, y, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f) );
#else
	vertices.Add( LSVertex( 0.0f, y, 0.0f ));
#endif

	// index of center vertex
	UINT centerIndex = (UINT)vertices.Num()-1;

	for(UINT i = 0; i < m_numSlices; ++i)
	{
		indices.Add( centerIndex );
		indices.Add( baseIndex + i );
		indices.Add( baseIndex + i + 1 );
	}
}

void rxLightShape_Cylinder::BuildTopCap( LSVertexList & vertices, LSIndexList & indices )
{
	UINT baseIndex = (UINT)vertices.Num();

	// Duplicate cap vertices because the texture coordinates and normals differ.
	FLOAT y = 0.5f * m_height;

	// vertices of ring
	FLOAT dTheta = MX_TWO_PI / m_numSlices;
	for(UINT i = 0; i <= m_numSlices; ++i)
	{
		FLOAT x = m_topRadius * mxCos( i * dTheta );
		FLOAT z = m_topRadius * mxSin( i * dTheta );

#if 0
		// Map [-1,1]-->[0,1] for planar texture coordinates.
		FLOAT u = +0.5f*x/this->topRadius + 0.5f;
		FLOAT v = -0.5f*z/this->topRadius + 0.5f;

		vertices.Add( LSVertex(x, y, z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, u, v) );
#else
		vertices.Add( LSVertex( x, y, z ));
#endif
	}

	// cap center vertex
#if 0
	vertices.Add( LSVertex(0.0f, y, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f) );
#else
	vertices.Add( LSVertex( 0.0f, y, 0.0f ));
#endif

	// index of center vertex
	UINT centerIndex = (UINT)vertices.Num()-1;

	for(UINT i = 0; i < m_numSlices; ++i)
	{
		indices.Add( centerIndex );
		indices.Add( baseIndex + i + 1 );
		indices.Add( baseIndex + i );
	}
}

/*================================
	rxLightShape_UnitCube
================================*/

rxLightShape_UnitCube::rxLightShape_UnitCube()
{}

rxLightShape_UnitCube::~rxLightShape_UnitCube()
{}

void rxLightShape_UnitCube::Build()
{
	this->Clear();

	LSVertexList	vertices;
	LSIndexList		indices;

	enum { NUM_VERTICES = 24 };
	enum { NUM_INDICES = 36 };

	mxOPTIMIZE("8 vertices should be enough, we don't need texcoords");

	// Create vertex buffer.

	vertices.SetNum( NUM_VERTICES );

	// Fill in the front face vertex data.
	vertices[0] = LSVertex( Vec3D( -0.5f, -0.5f, -0.5f )		);//Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[1] = LSVertex( Vec3D( -0.5f,  0.5f, -0.5f )		);//Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[2] = LSVertex( Vec3D(  0.5f,  0.5f, -0.5f )		);//Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[3] = LSVertex( Vec3D(  0.5f, -0.5f, -0.5f )		);//Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, -1.0f ), Vec3D( 1.0f, 0.0f, 0.0f )	
																		
	// Fill in the back face vertex data.								
	vertices[4] = LSVertex( Vec3D( -0.5f, -0.5f, 0.5f ) 		);//Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[5] = LSVertex( Vec3D(  0.5f, -0.5f, 0.5f ) 		);//Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[6] = LSVertex( Vec3D(  0.5f,  0.5f, 0.5f ) 		);//Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[7] = LSVertex( Vec3D( -0.5f,  0.5f, 0.5f ) 		);//Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f )	
																		
	// Fill in the top face vertex data.								
	vertices[8]  = LSVertex( Vec3D( -0.5f, 0.5f, -0.5f )		);//Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[9]  = LSVertex( Vec3D( -0.5f, 0.5f,  0.5f )		);//Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[10] = LSVertex( Vec3D(  0.5f, 0.5f,  0.5f )		);//Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	
	vertices[11] = LSVertex( Vec3D(  0.5f, 0.5f, -0.5f )		);//Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f )	
																	
	// Fill in the bottom face vertex data.							
	vertices[12] = LSVertex( Vec3D( -0.5f, -0.5f, -0.5f )	);//Vec2D( 1.0f, 1.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[13] = LSVertex( Vec3D(  0.5f, -0.5f, -0.5f )	);//Vec2D( 0.0f, 1.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[14] = LSVertex( Vec3D(  0.5f, -0.5f,  0.5f )	);//Vec2D( 0.0f, 0.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	
	vertices[15] = LSVertex( Vec3D( -0.5f, -0.5f,  0.5f )	);//Vec2D( 1.0f, 0.0f ), 	Vec3D( 0.0f, -1.0f, 0.0f ), Vec3D( -1.0f, 0.0f, 0.0f )	
																	
	// Fill in the left face vertex data.							
	vertices[16] = LSVertex( Vec3D( -0.5f, -0.5f,  0.5f )	);//Vec2D( 0.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	
	vertices[17] = LSVertex( Vec3D( -0.5f,  0.5f,  0.5f )	);//Vec2D( 0.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	
	vertices[18] = LSVertex( Vec3D( -0.5f,  0.5f, -0.5f )	);//Vec2D( 1.0f, 0.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	
	vertices[19] = LSVertex( Vec3D( -0.5f, -0.5f, -0.5f )	);//Vec2D( 1.0f, 1.0f ), 	Vec3D( -1.0f, 0.0f, 0.0f ), Vec3D( 0.0f, 0.0f, -1.0f )	
																	
	// Fill in the right face vertex data.							
	vertices[20] = LSVertex( Vec3D(  0.5f, -0.5f, -0.5f )	);//Vec2D( 0.0f, 1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	
	vertices[21] = LSVertex( Vec3D(  0.5f,  0.5f, -0.5f )	);//Vec2D( 0.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	
	vertices[22] = LSVertex( Vec3D(  0.5f,  0.5f,  0.5f )	);//Vec2D( 1.0f, 0.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	
	vertices[23] = LSVertex( Vec3D(  0.5f, -0.5f,  0.5f )	);//Vec2D( 1.0f, 1.0f ), 	Vec3D( 1.0f, 0.0f, 0.0f ), 	Vec3D( 0.0f, 0.0f, 1.0f )	


	// Create the index buffer.

	indices.SetNum( NUM_INDICES );

	// Fill in the front face index data
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8; indices[13] =  9; indices[14] = 10;
	indices[15] = 8; indices[16] = 10; indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;

	m_numVertices	= (UINT) vertices.Num();
	m_numIndices	= (UINT) indices.Num();

	CreateBuffers( vertices, indices );
}

/*
--------------------------------------------------------------
	LightShapes
--------------------------------------------------------------
*/
void LightShapes::CreateMeshes()
{
	// Create light shape meshes.

	// transform spot light shape so that its axis points in the positive Z direction

	Matrix4 spotLightShapeTransform(
		Matrix4::CreateTranslation( Vec3D( 0.0f, -0.5f /* half height */, 0.0f ))
		* Matrix3::CreateRotation( -Vec3D::vec3_unit_y, Vec3D::vec3_unit_z ).ToMat4()
	);

	// NOTE: if the shapes are too coarse, light flickering will occur when moving in/out of the light volumes.
	const bool bLowTesselatedLightShapes = true;

	if( bLowTesselatedLightShapes )
	{
		const FLOAT radius = LIGHT_SHAPE_SCALE;//<- increased to hide light shape geometry artifacts

		unitPointLightShape.Build(
			radius /*radius*/,
			6 /*numSlices*/,
			6 /*numStacks*/
		);
		unitSpotLightShape.Build(
			0.0001f /*topRadius*/,
			1.01f /*bottomRadius*/,	//<- increased to hide light shape geometry artifacts
			1.0f /*height*/,
			12 /*numSlices*/,
			1 /*numStacks*/,
			&spotLightShapeTransform
		);
	}
	else
	{
		unitPointLightShape.Build(
			1.0f /*radius*/,
			24 /*numSlices*/,
			24 /*numStacks*/
		);
		unitSpotLightShape.Build(
			0.0001f /*topRadius*/,
			1.0f /*bottomRadius*/,
			1.0f /*height*/,
			32 /*numSlices*/,
			32 /*numStacks*/,
			&spotLightShapeTransform
		);
	}

	unitCubeLightShape.Build();

	struct size_calc_t {
		static UINT CalcSize( const rxLightShape& mesh ) {
			return mesh.GetVertexCount()*sizeof(LSVertex)
				+ mesh.GetIndexCount() * sizeof(UINT16);
		}
	};
	DBGOUT("- Created point light shape: %u vertices, %u indices (%u Kb)\n",
		unitPointLightShape.GetVertexCount(), unitPointLightShape.GetIndexCount(), size_calc_t::CalcSize(unitPointLightShape) );

	DBGOUT("- Created spot light shape: %u vertices, %u indices (%u Kb)\n",
		unitSpotLightShape.GetVertexCount(), unitSpotLightShape.GetIndexCount(), size_calc_t::CalcSize(unitSpotLightShape) );

}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
