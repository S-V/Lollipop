#pragma once

class EdPakFileBuilder
{
public:
	bool Build_Optimized_Pak_File(
		const char* destFilePath,
		const StringListType& referencedAssets
		);

	bool Build_Hashed_Pak_File(
		const char* destFilePath
		);
};
