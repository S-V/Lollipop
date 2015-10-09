#pragma once

#include <Renderer/Core/Texture.h>
#include <Renderer/Core/Material.h>

/*
-----------------------------------------------------------------------------
	PhongMaterial
-----------------------------------------------------------------------------
*/
class PhongMaterial
	: public rxMaterial
{
public:
	mxDECLARE_CLASS(PhongMaterial,rxMaterial);
	mxDECLARE_REFLECTION;

	PhongMaterial();
	~PhongMaterial();

	// returns FALSE when done
	virtual void rfSubmitBatches( const rxMaterialViewContext& context ) override;

public:
	FColor		diffuseColor;
	FColor		specularColor;
	FColor		emissiveColor;

	rxTexture::Ref	baseMap;
	rxTexture::Ref	normalMap;
	rxTexture::Ref	specularMap;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
