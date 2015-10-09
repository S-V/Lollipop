#include <EditorSupport_PCH.h>
#pragma hdrstop

#include "EditorMeshCommon.h"

#if 0
/*================================
		SubMesh
================================*/

EdSubMesh::EdSubMesh()
{
	baseVertex = 0;
	startIndex = 0;
	indexCount = 0;
	vertexCount = 0;
	bounds.SetInfinity();
}
//---------------------------------------------------------------------------
EdSubMesh::~EdSubMesh()
{
}
//---------------------------------------------------------------------------
bool EdSubMesh::isOk() const
{
	return vertexCount > 0
		;
}

#if MX_EDITOR
void EdSubMesh::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	new EdProperty_UInt32( properties, "baseVertex", baseVertex, PF_ReadOnly );
	new EdProperty_UInt32( properties, "indexCount", indexCount, PF_ReadOnly );
	new EdProperty_UInt32( properties, "startIndex", startIndex, PF_ReadOnly );
	new EdProperty_UInt32( properties, "vertexCount", vertexCount, PF_ReadOnly );

	new EdProperty_Vector3D( properties, "localCenter", bounds.Center, PF_ReadOnly );
	new EdProperty_Float( properties, "localRadius", bounds.Radius, PF_ReadOnly );

	UNDONE;
	//new EdProperty_AssetReference( properties, "material", material, EAssetType::Asset_Graphics_Material );
}
#endif // MX_EDITOR



/*
-----------------------------------------------------------------------------
	rxModelBatch
-----------------------------------------------------------------------------
*/
rxModelBatch::rxModelBatch()
{
	baseVertex = 0;
	startIndex = 0;
	indexCount = 0;

	//bounds.Clear();
}

void rxModelBatch::Setup( const EdSubMesh& source )
{
	UNDONE;
//	material = TGetResourceIdByGuid<rxMaterial>( source.material );

	baseVertex = source.baseVertex;
	startIndex = source.startIndex;
	indexCount = source.indexCount;

	//bounds = source.bounds;
}

mxArchive& operator && ( mxArchive & serializer, rxModelBatch & o )
{
	UNDONE;
	return serializer
		//&& o.material
		//&& o.baseVertex
		//&& o.startIndex
		//&& o.indexCount
		//&& o.bounds
		;
}

#if MX_EDITOR

void rxModelBatch::edCreateProperties( EdPropertyList *properties, bool bClear )
{
	UNDONE;
	//new EdProperty_AssetReference( properties, "material", &material, EAssetType::Asset_Graphics_Material );
	//new EdProperty_UInt32( properties, "indexCount", indexCount, PF_ReadOnly );
	//new EdProperty_UInt32( properties, "startIndex", startIndex, PF_ReadOnly );
	//new EdProperty_UInt32( properties, "baseVertex", baseVertex, PF_ReadOnly );

	//new EdProperty_Vector3D( properties, "localCenter", bounds.mOrigin, PF_ReadOnly );
	//new EdProperty_Float( properties, "localRadius", bounds.mRadius, PF_ReadOnly );
}

#endif // MX_EDITOR


#if MX_EDITOR

mxDEFINE_ABSTRACT_CLASS( HModelBatch );

#endif // MX_EDITOR

#endif

NO_EMPTY_FILE

