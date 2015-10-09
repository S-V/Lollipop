#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <EditorSupport/AssetPipeline.h>


/*
-----------------------------------------------------------------------------
	AssetInfo
-----------------------------------------------------------------------------
*/
SAssetInfo::SAssetInfo()
{
	srcFileName.ZeroOut();
	binFileName.ZeroOut();
	assetType = Asset_Unknown;
	assetGuid.v = mxNULL_OBJECT_GUID;
	//classType = mxNULL_TYPE_GUID;
}

bool SAssetInfo::DbgChk_IsValid() const
{
	CHK_VRET_FALSE_IF_NOT( !srcFileName.IsEmpty() );
	CHK_VRET_FALSE_IF_NOT( !binFileName.IsEmpty() );

	CHK_VRET_FALSE_IF_NOT( assetGuid.IsValid() );

	CHK_VRET_FALSE_IF_NOT( assetType != EAssetType::Asset_Unknown );

	return true;
}

mxDEFINE_CLASS_NO_DEFAULT_CTOR(EdAssetInfo);

/*
-----------------------------------------------------------------------------
	AssetData
-----------------------------------------------------------------------------
*/
mxDEFINE_CLASS(AssetData);

void AssetData::Serialize( mxArchive & archive )
{
	Super::Serialize( archive );
}
