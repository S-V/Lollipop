/*
=============================================================================
	File:	PlainColor.h
	Desc:	Plain color material.
=============================================================================
*/
#pragma once

#include <Renderer/Core/Material.h>

/*
-----------------------------------------------------------------------------
	PlainColorMaterial
-----------------------------------------------------------------------------
*/
mxALIGN_16(class) PlainColorMaterial
	: public rxMaterial
{
public:
	mxDECLARE_CLASS(PlainColorMaterial,rxMaterial);
	mxDECLARE_REFLECTION;

	PlainColorMaterial();
	~PlainColorMaterial();

	// returns FALSE when done
	virtual void rfSubmitBatches( const rxMaterialViewContext& context ) override;

public:
	FColor		diffuseColor;
	FColor		specularColor;
	FColor		emissiveColor;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
