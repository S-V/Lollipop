/*
=============================================================================
	File:	SkyModel.h
	Desc:	Sky rendering.
=============================================================================
*/
#pragma once

//#include <Renderer/Core/Geometry.h>
#include <Renderer/Core/Mesh.h>
#include <Renderer/Core/SceneView.h>

/*
-----------------------------------------------------------------------------
	Sky Domes
-----------------------------------------------------------------------------
*/

typedef GPU::Vertex_P3f_TEX2f DomeVertex;


/*
-----------------------------------------------------------------------------
	rxSkyModel
-----------------------------------------------------------------------------
*/
class rxSkyModel : public SBaseType
{
public:
	mxDECLARE_CLASS( rxSkyModel, SBaseType );
	mxDECLARE_REFLECTION;

	rxSkyModel();
	~rxSkyModel();

	void Render( const rxRenderContext& context );

private:
	TVertexSource< DomeVertex >	m_VB;
	GrIndexBuffer16				m_IB;
	UINT						m_numIndices;

	rxTexture::Ref			m_skyTexture;

private:NO_COPY_CONSTRUCTOR(rxSkyModel);
};


#if 0
//
//	SkyDome
//
class SkyDome
	: public rxSkyDome
	, public LList< SkyDome >
{
public:

	//
	// Override ( rxSky ) :
	//

	void	SetOrientation( const Quat& newOrientation );
	void	SetSkyTexture( rxTexture* newSkyTexture );

	//
	// Override ( rxEntity ) :
	//
	void	AddSelfToWorld( rxWorld* renderWorld );

public:
	SkyDome(
		rxTexture* skyTexture,
		UINT horizontalResolution = 16, UINT verticalResolution = 8,
		FLOAT texturePercentage = 0.9f,
		FLOAT spherePercentage = 2.0f,
		FLOAT radius = 1.0f
	);

	~SkyDome();

	void Render( const dxView& view );

public:
	struct SkyVertex {
		Vec3D XYZ;
		Vec3D Normal;
		Vec2D UV;
	};
	typedef UINT16		SkyIndex;

	enum { MAX_SKY_DOME_VERTICES = 4096 };
	enum { MAX_SKY_DOME_INDICES = 4096*3 };

	typedef TStaticList< SkyVertex, MAX_SKY_DOME_VERTICES >	SkyDomeVertexList;
	typedef TStaticList< SkyIndex, MAX_SKY_DOME_INDICES >	SkyDomeIndexList;

private:
	dxMesh	skyMesh;

	dxPtr< ID3D10ShaderResourceView >	texture;

	rxPtr< fxSkydome >	skyShader;
};
#endif

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
