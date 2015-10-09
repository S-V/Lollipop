/*
=============================================================================
	File:	PlainColor.cpp
	Desc:	Plain color material.
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop
//#include "Renderer.h"

#include <Core/Editor/EditableProperties.h>
#include <Renderer/Pipeline/RenderQueue.h>
#include <Renderer/Scene/RenderEntity.h>

#include <Renderer/GPU/HLSL/BuildConfig.h>
#include <Renderer/GPU/ShaderPrograms.hxx>

#include "PlainColor.h"


/*
-----------------------------------------------------------------------------
	PlainColorMaterial
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(PlainColorMaterial);

mxBEGIN_REFLECTION(PlainColorMaterial)
	mxMEMBER_FIELD2( diffuseColor,	Diffuse_Color )
	mxMEMBER_FIELD2( specularColor,	Specular_Color )
	mxMEMBER_FIELD2( emissiveColor,	Emissive_Color )
mxEND_REFLECTION

static void ApplyMaterial_PlainColor( const rxMaterialRenderContext& context, rxMaterial* pMaterial )
{
	PlainColorMaterial* theMaterial = c_cast(PlainColorMaterial*) pMaterial;

	ID3D11DeviceContext* pD3DContext = context.pD3D;

	typedef GPU::p_material_plain_color Shader;

	Shader::Data* pData = Shader::cb_Data.Map( pD3DContext );
	{
		pData->materialDiffuseColor = theMaterial->diffuseColor.mSimdQuad;
		pData->materialSpecularColor = theMaterial->specularColor.mSimdQuad;
	}
	Shader::cb_Data.Unmap( pD3DContext );

	Shader::Set( pD3DContext );
}

PlainColorMaterial::PlainColorMaterial()
{
	this->rfBindProgram.SetAll( &ApplyMaterial_PlainColor );

	this->diffuseColor = FColor::WHITE;
	this->specularColor = FColor::WHITE;
	this->emissiveColor = FColor::BLACK;
}

PlainColorMaterial::~PlainColorMaterial()
{

}

void PlainColorMaterial::rfSubmitBatches( const rxMaterialViewContext& context )
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
