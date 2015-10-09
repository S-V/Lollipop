#pragma once

#include <Renderer/Scene/Model.h>
#include <EditorSupport/AssetPipeline/MeshConverter.h>

#include "editor_common.h"

namespace GraphicsUtil
{

void F_Create_Render_Model_From_Mesh( rxModel & rModel, const SAssetInfo& assetInfo, const StaticTriangleMeshData* pStaticMeshData );

}//namespace GraphicsUtil
