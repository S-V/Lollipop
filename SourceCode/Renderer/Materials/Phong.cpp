/*
=============================================================================
	File:	Material.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include <Core/Editor/EditableProperties.h>
#include <Renderer/Pipeline/RenderQueue.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/GPU/ShaderPrograms.hxx>
#include "Phong.h"

/*
-----------------------------------------------------------------------------
	PhongMaterial
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(PhongMaterial);

mxBEGIN_REFLECTION(PhongMaterial)
	mxMEMBER_FIELD2( diffuseColor,	Diffuse_Color )
	mxMEMBER_FIELD2( specularColor,	Specular_Color )
	mxMEMBER_FIELD2( emissiveColor,	Emissive_Color )

	mxMEMBER_FIELD2( baseMap,		Diffuse_Map )
	mxMEMBER_FIELD2( normalMap,		Normal_Map,		Field_NoDefaultInit )
	mxMEMBER_FIELD2( specularMap,	Specular_Map,	Field_NoDefaultInit )
mxEND_REFLECTION

static void Apply_PhongMaterial( const rxMaterialRenderContext& context, rxMaterial* pMaterial )
{
	PhongMaterial* pPhongMaterial = c_cast(PhongMaterial*) pMaterial;

	ID3D11DeviceContext* pD3DContext = context.pD3D;

	typedef GPU::p_material_phong ShaderType;

	ShaderType::Data* pData = ShaderType::cb_Data.Map( pD3DContext );
	{
		pData->materialDiffuseColor = pPhongMaterial->diffuseColor.mSimdQuad;
		pData->materialSpecularColor = pPhongMaterial->specularColor.mSimdQuad;
		pData->materialEmissiveColor = pPhongMaterial->emissiveColor.mSimdQuad;
	}
	ShaderType::cb_Data.Unmap( pD3DContext );

	rxShaderInstanceId	shaderInstanceId = ShaderType::DefaultInstanceId;

	if( pPhongMaterial->baseMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bHasBaseMap;
		ShaderType::baseMap = pPhongMaterial->baseMap.ToPtr()->pSRV;
	}
	if( pPhongMaterial->normalMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bHasNormalMap;
		ShaderType::normalMap = pPhongMaterial->normalMap.ToPtr()->pSRV;
	}
	if( pPhongMaterial->specularMap.IsValid() )
	{
		shaderInstanceId |= ShaderType::bHasSpecularMap;
		ShaderType::specularMap = pPhongMaterial->specularMap.ToPtr()->pSRV;
	}

	ShaderType::Set( pD3DContext, shaderInstanceId );
}

PhongMaterial::PhongMaterial()
{
	this->rfBindProgram.SetAll( &Apply_PhongMaterial );

	diffuseColor = FColor::WHITE;
	specularColor = FColor::WHITE;
	emissiveColor = FColor::BLACK;
}

PhongMaterial::~PhongMaterial()
{

}

void PhongMaterial::rfSubmitBatches( const rxMaterialViewContext& context )
{
	DEBUG_CODE( context.DbgCheckValid() );

	rxSurface & b = context.q->AddBatch(
		this,
		RS_Deferred_FillGBuffer,
		DO_Opaque,
		mxFtoI(context.fDistance),mxTODO("dist not used")
		context.entity,
		context.subset
	);
	(void)b;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
