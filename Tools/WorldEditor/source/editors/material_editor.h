#pragma once

#include <EditorSupport/AssetPipeline.h>

#include "editor_common.h"

class rxMaterial;
class PlainColorMaterial;
class PhongMaterial;
class Material_SSS;

/*
-----------------------------------------------------------------------------
	Material_Editor
-----------------------------------------------------------------------------
*/
class Material_Editor : public AEditableRefCounted
{

public:
	mxDECLARE_ABSTRACT_CLASS(Material_Editor,AEditableRefCounted);

	Material_Editor( rxMaterial* pMaterial );

	virtual void edSaveChanges() override;

public:
	// creates an editor object for the given entity
	static Material_Editor* Create_Editor_For_Material( rxMaterial* pMaterial );
};

/*
-----------------------------------------------------------------------------
	Material_Editor_Plain_Color
-----------------------------------------------------------------------------
*/
class Material_Editor_Plain_Color : public Material_Editor
{
	TValidPtr< PlainColorMaterial >	m_material;

public:
	mxDECLARE_CLASS(Material_Editor_Plain_Color,Material_Editor);

	Material_Editor_Plain_Color( PlainColorMaterial* pMaterial );

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
};

/*
-----------------------------------------------------------------------------
	Material_Editor_Phong
-----------------------------------------------------------------------------
*/
class Material_Editor_Phong : public Material_Editor
{
	TValidPtr< PhongMaterial >	m_material;

public:
	mxDECLARE_CLASS(Material_Editor_Phong,Material_Editor);

	Material_Editor_Phong( PhongMaterial* pMaterial );

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
};


/*
-----------------------------------------------------------------------------
	Material_Editor_SSS
-----------------------------------------------------------------------------
*/
class Material_Editor_SSS : public Material_Editor
{
	TValidPtr< Material_SSS >	m_material;

public:
	mxDECLARE_CLASS(Material_Editor_SSS,Material_Editor);

	Material_Editor_SSS( Material_SSS* pMaterial );

	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
