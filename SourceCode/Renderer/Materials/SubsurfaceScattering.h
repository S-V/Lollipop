/*
=============================================================================
	File:	SubsurfaceScattering.h
	Desc:	
=============================================================================
*/
#pragma once

#include <Renderer/Core/Material.h>

/*
-----------------------------------------------------------------------------
	Material_SSS
-----------------------------------------------------------------------------
*/
mxALIGN_16(class) Material_SSS
	: public rxMaterial
{
public:
	mxDECLARE_CLASS(Material_SSS,rxMaterial);
	mxDECLARE_REFLECTION;

	Material_SSS();
	~Material_SSS();

	// returns FALSE when done
	virtual void rfSubmitBatches( const rxMaterialViewContext& context ) override;

public:
	FColor		diffuseColor;
	FColor		specularColor;
	FColor		emissiveColor;

	rxTexture::Ref	baseMap;
	rxTexture::Ref	normalMap;
	rxTexture::Ref	specularMap;
	rxTexture::Ref	beckmannMap;	// for subsurface scattering
};

extern bool g_cvar_enable_subsurface_scattering;
extern bool g_cvar_enable_separable_sss_post_fx;

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
