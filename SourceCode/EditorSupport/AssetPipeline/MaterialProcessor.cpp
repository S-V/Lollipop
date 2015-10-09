#include <EditorSupport_PCH.h>
#pragma hdrstop

#include <Renderer/Core/Material.h>
#include <Renderer/Materials/PlainColor.h>
#include <Renderer/Materials/Phong.h>

#include <EditorSupport/Reflection/ReflectionUtil.h>
#include <EditorSupport/AssetPipeline.h>
#include "MaterialProcessor.h"

namespace Editor
{

MaterialProcessor::MaterialProcessor()
{
	F_RegisterAssetProcessor(".material",this);

	PlainColorMaterial::StaticClass();
	PhongMaterial::StaticClass();
}

MaterialProcessor::~MaterialProcessor()
{

}

void MaterialProcessor::ProcessFile(
	const ProcessFileInput& input,
	ProcessFileOutput &output
	)
{
	output.assetType = Asset_Graphics_Material;
	output.fileExtension = ".gfx_mat";

	const char* filePath = input.filePath.ToChars();

	//JsonBuildConfig	buildConfig;
	//JsonBuildConfig::F_Get_Asset_Build_Config( filePath, buildConfig );

	//bool	is_normal_map = false;
	//buildConfig.GetBool("is_normal_map", is_normal_map);

	FileReader		srcFile( filePath );
	VRET_IF_NOT(srcFile.IsOpen());

	MemoryBlobWriter	dstStream( output.compiledData );
	ReflectionUtil::ConvertTextToBinary( srcFile, dstStream );
}

}//namespace Editor
