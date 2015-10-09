#include "stdafx.h"

#include <Core/Editor/EditableProperties.h>

#include <Renderer/Core/Material.h>
#include <Renderer/Materials/PlainColor.h>
#include <Renderer/Materials/Phong.h>
#include <Renderer/Materials/SubsurfaceScattering.h>

#include "editors/material_editor.h"

/*
-----------------------------------------------------------------------------
	Material_Editor
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(Material_Editor);

Material_Editor::Material_Editor( rxMaterial* pMaterial )
{
	AssertPtr(pMaterial);
	Assert(pMaterial->pEditor == nil);
	pMaterial->pEditor = this;
}

void Material_Editor::edSaveChanges()
{
	mxDBG_UNIMPLEMENTED;
}

Material_Editor* Material_Editor::Create_Editor_For_Material( rxMaterial* pMaterial )
{
	CHK_VRET_NIL_IF_NIL(pMaterial);

	{
		PlainColorMaterial* pPlainColorMaterial = SafeCast< PlainColorMaterial >( pMaterial );
		if( pPlainColorMaterial != nil ) {
			return new Material_Editor_Plain_Color( pPlainColorMaterial );
		}
	}

	{
		PhongMaterial* pPhongMaterial = SafeCast< PhongMaterial >( pMaterial );
		if( pPhongMaterial != nil ) {
			return new Material_Editor_Phong( pPhongMaterial );
		}
	}

	{
		Material_SSS* pSSSMaterial = SafeCast< Material_SSS >( pMaterial );
		if( pSSSMaterial != nil ) {
			return new Material_Editor_SSS( pSSSMaterial );
		}
	}

	mxDBG_UNIMPLEMENTED;
	return nil;
}

/*
-----------------------------------------------------------------------------
	Material_Editor_Plain_Color
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(Material_Editor_Plain_Color);

Material_Editor_Plain_Color::Material_Editor_Plain_Color( PlainColorMaterial* pMaterial )
	: Super( pMaterial )
	, m_material( pMaterial )
{
}

void Material_Editor_Plain_Color::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	new EdProperty_ColorRGBA( properties, "diffuseColor", m_material->diffuseColor );
	new EdProperty_ColorRGBA( properties, "specularColor", m_material->specularColor );
	new EdProperty_ColorRGBA( properties, "emissiveColor", m_material->emissiveColor );
}

/*
-----------------------------------------------------------------------------
	Material_Editor_Phong
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(Material_Editor_Phong);

Material_Editor_Phong::Material_Editor_Phong( PhongMaterial* pMaterial )
	: Super( pMaterial )
	, m_material( pMaterial )
{
}

void Material_Editor_Phong::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	new EdProperty_ColorRGBA( properties, "diffuseColor", m_material->diffuseColor );
	new EdProperty_ColorRGBA( properties, "specularColor", m_material->specularColor );
	new EdProperty_ColorRGBA( properties, "emissiveColor", m_material->emissiveColor );

	new EdProperty_AssetReference( m_material->baseMap, "baseMap", properties );
	new EdProperty_AssetReference( m_material->normalMap, "normalMap", properties );
	new EdProperty_AssetReference( m_material->specularMap, "specularMap", properties );
}

/*
-----------------------------------------------------------------------------
	Material_Editor_SSS
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(Material_Editor_SSS);

Material_Editor_SSS::Material_Editor_SSS( Material_SSS* pMaterial )
	: Super( pMaterial )
	, m_material( pMaterial )
{
}

void Material_Editor_SSS::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	Super::edCreateProperties(properties,bClear);

	new EdProperty_ColorRGBA( properties, "diffuseColor", m_material->diffuseColor );
	new EdProperty_ColorRGBA( properties, "specularColor", m_material->specularColor );
	new EdProperty_ColorRGBA( properties, "emissiveColor", m_material->emissiveColor );

	new EdProperty_AssetReference( m_material->baseMap, "baseMap", properties );
	new EdProperty_AssetReference( m_material->normalMap, "normalMap", properties );
	new EdProperty_AssetReference( m_material->specularMap, "specularMap", properties );
	new EdProperty_AssetReference( m_material->beckmannMap, "beckmannMap", properties );
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
