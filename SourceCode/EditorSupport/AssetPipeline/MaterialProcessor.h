// texture asset importer
// 
#pragma once

#include <EditorSupport/AssetPipeline/AssetProcessor.h>

namespace Editor
{

	// converts from source texture (e.g. "*.psd", "*.bmp")
	// into platform-specific format (e.g. "*.dds")
	//
	struct MaterialProcessor : AssetProcessor
	{
		MaterialProcessor();
		~MaterialProcessor();

		virtual EAssetType GetAssetType() const override
		{
			return EAssetType::Asset_Texture2D;
		}

		virtual void ProcessFile(
			const ProcessFileInput& input,
			ProcessFileOutput &output
			) override;
	};

}//namespace Editor
