/*
=============================================================================
	File:	Material.cpp
	Desc:	
=============================================================================
*/
#include "Renderer_PCH.h"
#pragma hdrstop

#include <Core/Editor/EditableProperties.h>
#include <Core/Serialization.h>
#include <Renderer/Scene/RenderEntity.h>
#include <Renderer/GPU/Main.hxx>

#include <Renderer/Common.h>
#include <Renderer/Core/Material.h>
#include <Renderer/Materials/PlainColor.h>
#include <Renderer/Materials/Phong.h>

#define MX_MATERIAL_FILE_EXTENSION	"material"

bool rxMaterialViewContext::DbgCheckValid() const
{
	CHK_VRET_FALSE_IF_NIL( s );
	CHK_VRET_FALSE_IF_NIL( q );
	CHK_VRET_FALSE_IF_NIL( entity );
	CHK_VRET_FALSE_IF_NOT( subset != INDEX_NONE );
	//CHK_VRET_FALSE_IF_NOT( nMaterial != INDEX_NONE );
	//CHK_VRET_FALSE_IF_NIL( mesh );
	//CHK_VRET_FALSE_IF_NIL( batch );
	return true;
}

namespace
{
	static UINT	g_totalNumMaterials = 0;

	struct SortByTypeGuid
	{
		// for comparison-based sorting algorithms
		static FORCEINLINE bool Compare( const rxMaterial* a, const rxMaterial* b )
		{
			return a->rttiGetTypeGuid() < b->rttiGetTypeGuid();
		}
	};

	static void F_ApplyNullMaterial( const rxMaterialRenderContext& context, rxMaterial* pMaterial )
	{
		mxUNUSED(context);
		mxUNUSED(pMaterial);
	}

}//namespace

/*
-----------------------------------------------------------------------------
	rxMaterial
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS( rxMaterial );

rxMaterial::rxMaterial()
{
	++g_totalNumMaterials;
	rfBindProgram.SetAll( F_ApplyNullMaterial );
}

rxMaterial::~rxMaterial()
{
	--g_totalNumMaterials;
}

UINT rxMaterial::TotalCount()
{
	return ++g_totalNumMaterials;
}

/*
-----------------------------------------------------------------------------
	rxMaterialSystem
-----------------------------------------------------------------------------
*/
namespace
{
	struct MaterialSystemData
	{
		PlainColorMaterial	defaultMaterial;
	};

	static TPtr< MaterialSystemData >	gData;

}//namespace

rxMaterialSystem::rxMaterialSystem()
{
	gData.ConstructInPlace();

	gCore.resources->SetManager( Asset_Graphics_Material, this );
}

rxMaterialSystem::~rxMaterialSystem()
{
	gData.Destruct();
}

SResourceObject* rxMaterialSystem::LoadResource( SResourceLoadArgs & loadArgs )
{
	InPlaceMemoryReader	stream( loadArgs.Map(), loadArgs.GetSize() );

	const TypeGUID classGuid = ReadTypeGuid( stream );
	const mxClass* pClassInfo = TypeRegistry::Get().FindClassInfoByGuid( classGuid );
	CHK_VRET_NIL_IF_NIL(pClassInfo);


	rxMaterial* pNewMaterial = UpCast< rxMaterial >( ObjectUtil::Create_Object_Instance( *pClassInfo ) );
	CHK_VRET_NIL_IF_NIL(pNewMaterial);


	BinaryObjectReader	deserializer( stream );

	deserializer.Deserialize( pNewMaterial, *pClassInfo );

	//DBGOUT("Loaded graphics material type %s\n",pNewMaterial->rttiGetClass().GetTypeName());

	return pNewMaterial;
}

SResourceObject* rxMaterialSystem::GetDefaultResource()
{
	return &gData->defaultMaterial;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
