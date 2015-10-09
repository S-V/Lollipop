#include "Renderer_PCH.h"
#pragma hdrstop

#include <Renderer/Core/SceneView.h>

#include <Renderer/GPU/Main.hxx>
#include <Renderer/GPU/ShaderPrograms.hxx>
#include <Renderer/GPU/VertexFormats.hxx>

#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/Scene/Light.h>
#include <Renderer/Scene/SkyModel.h>
#include <Renderer/Scene/RenderWorld.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

enum { MAX_SKY_DOME_VERTICES = 4096 };
enum { MAX_SKY_DOME_INDICES = 4096*3 };


static
void GenerateSkyDomeMesh(
						 TStaticList< DomeVertex, MAX_SKY_DOME_VERTICES >	&vertices,
						 TStaticList< U2, MAX_SKY_DOME_INDICES >			&indices,
						 UINT horizontalResolution = 16, UINT verticalResolution = 8,
						 FLOAT texturePercentage = 0.9f,
						 FLOAT spherePercentage = 2.0f,
						 FLOAT radius = 1.0f
						 )
{
#if MX_DEVELOPER
	const UINT startTime = mxGetTimeInMicroseconds();
#endif // MX_DEVELOPER

	// Create sky dome geometry.

	FLOAT azimuth;
	UINT k;

	const FLOAT azimuth_step = (MX_PI * 2.f) / horizontalResolution;
	if( spherePercentage < 0.f ) {
		spherePercentage = -spherePercentage;
	}
	if( spherePercentage > 2.f ) {
		spherePercentage = 2.f;
	}
	const FLOAT elevation_step = spherePercentage * MX_HALF_PI / (FLOAT)verticalResolution;

	const UINT numVertices = (horizontalResolution + 1) * (verticalResolution + 1);
	const UINT numIndices = 3 * (2*verticalResolution - 1) * horizontalResolution;

	//const UINT sizeOfVertexData = numVertices * sizeof(DomeVertex);
	//const UINT sizeOfIndexData = numIndices * sizeof(SkyIndex);

	vertices.SetNum( numVertices );
	indices.SetNum( numIndices );

	DomeVertex vertex;

	UINT iVertex = 0;
	UINT iIndex = 0;

	const FLOAT tcV = texturePercentage / verticalResolution;

	for( k = 0, azimuth = 0; k <= horizontalResolution; ++k )
	{
		FLOAT elevation = MX_HALF_PI;

		const FLOAT tcU = (FLOAT)k / (FLOAT)horizontalResolution;

		FLOAT sinA, cosA;
		mxSinCos( azimuth, sinA, cosA );

		for( UINT j = 0; j <= verticalResolution; ++j )
		{
			FLOAT sinEl, cosEl;
			mxSinCos( elevation, sinEl, cosEl );

			const FLOAT cosEr = radius * cosEl;
			vertex.xyz.Set( cosEr*sinA, radius*sinEl, cosEr*cosA );
			vertex.uv.Set( tcU, j*tcV );

			//vertex.Normal = -vertex.XYZ;
			//vertex.Normal.Normalize();

			vertices[ iVertex++ ] = vertex;

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	for( k = 0; k < horizontalResolution; ++k )
	{
		indices[ iIndex++ ] = (verticalResolution + 2 + (verticalResolution + 1)*k);
		indices[ iIndex++ ] = (1 + (verticalResolution + 1)*k);
		indices[ iIndex++ ] = (0 + (verticalResolution + 1)*k);

		for( UINT j = 1; j < verticalResolution; ++j )
		{
			indices[ iIndex++ ] = (verticalResolution + 2 + (verticalResolution + 1)*k + j);
			indices[ iIndex++ ] = (1 + (verticalResolution + 1)*k + j);
			indices[ iIndex++ ] = (0 + (verticalResolution + 1)*k + j);

			indices[ iIndex++ ] = (verticalResolution + 1 + (verticalResolution + 1)*k + j);
			indices[ iIndex++ ] = (verticalResolution + 2 + (verticalResolution + 1)*k + j);
			indices[ iIndex++ ] = (0 + (verticalResolution + 1)*k + j);
		}
	}

#if MX_DEVELOPER
	const UINT endTime = mxGetTimeInMicroseconds();
	const UINT elapsedTime = endTime - startTime;
	DEVOUT("Generated skydome geometry (%u vertices, %u indices, %u KiB) in %u milliseconds.\n",
		vertices.Num(), indices.Num(), vertices.GetDataSize() + indices.GetDataSize(), elapsedTime/1000
	);
#endif // MX_DEVELOPER
}



/*
-----------------------------------------------------------------------------
	rxSkyModel
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS_NO_DEFAULT_CTOR( rxSkyModel );
mxBEGIN_REFLECTION( rxSkyModel )
	mxMEMBER_FIELD2( m_skyTexture, Texture_Layer,	Field_NoDefaultInit )
mxEND_REFLECTION

rxSkyModel::rxSkyModel( )
{
	m_numIndices = 0;

	TStaticList< DomeVertex, MAX_SKY_DOME_VERTICES >	vertices;
	TStaticList< U2, MAX_SKY_DOME_INDICES >				indices;

	GenerateSkyDomeMesh(
		vertices,
		indices
		,16,16//UINT horizontalResolution = 16, UINT verticalResolution = 8,
		,1.0//FLOAT texturePercentage = 0.9f,
		,1.0//FLOAT spherePercentage = 2.0f,
		//FLOAT radius = 1.0f
		);

	m_VB.Create( vertices.ToPtr(), vertices.GetDataSize() );
	m_IB.Create( indices.ToPtr(), indices.GetDataSize() );

	m_numIndices = indices.Num();
}

rxSkyModel::~rxSkyModel()
{

}

void rxSkyModel::Render( const rxRenderContext& context )
{
	if( m_skyTexture.IsValid() )
	{
		GPU::SkyLast.Set( context.pD3D );

		GPU::p_sky_dome::Data* pData = GPU::p_sky_dome::cb_Data.Map( context.pD3D );
		{
			const rxSceneContext* sceneContext = context.s;
			const float skyDomeScale = sceneContext->farZ * 0.9f;

			float4x4	skyLocalToWorld = XMMatrixAffineTransformation(
				XMVectorReplicate( skyDomeScale ),
				g_XMIdentityR3,
				XMQuaternionIdentity(),
				vec_load1( sceneContext->GetOrigin() )
			);

			skyLocalToWorld = XMMatrixMultiply( skyLocalToWorld, sceneContext->viewProjectionMatrix );

			pData->skyDomeTransform = skyLocalToWorld;

			mxUNDONE;
			pData->sunDirection = g_XMIdentityR0;
		}	
		GPU::p_sky_dome::cb_Data.Unmap( context.pD3D );

		GPU::p_sky_dome::skyTextureSampler = GPU::Shared_Globals::linearClampSampler;
		GPU::p_sky_dome::skyTexture = m_skyTexture.ToPtr()->pSRV;
		GPU::p_sky_dome::Set( context.pD3D );

		m_VB.Bind( context.pD3D );
		m_IB.Bind( context.pD3D );

		context.pD3D->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		context.pD3D->DrawIndexed( m_numIndices, 0, 0 );
	}
}

NO_EMPTY_FILE

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
