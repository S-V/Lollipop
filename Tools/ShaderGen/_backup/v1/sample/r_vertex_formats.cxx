#include "Renderer_PCH.h"
#pragma hdrstop
#include "Renderer.h"

#include <Graphics/DX11/src/DX11Private.h>
#include <Graphics/DX11/src/DX11Helpers.h>


#include "r_vertex_formats.hxx"

namespace GPU
{
	InputLayout vtx_static::IL;
	
	static void CreateInputLayouts()
	{
		{
			dxVertexFormat	vtxFormat;
			{
				D3D11_INPUT_ELEMENT_DESC & elemDesc = vtxFormat.elements.Add();
				elemDesc.SemanticName = "Position";
				elemDesc.SemanticIndex = 0;
				elemDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				elemDesc.InputSlot = 0;
				elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elemDesc.InstanceDataStepRate = 0;
			}
			{
				D3D11_INPUT_ELEMENT_DESC & elemDesc = vtxFormat.elements.Add();
				elemDesc.SemanticName = "Tangent";
				elemDesc.SemanticIndex = 0;
				elemDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
				elemDesc.InputSlot = 0;
				elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elemDesc.InstanceDataStepRate = 0;
			}
			{
				D3D11_INPUT_ELEMENT_DESC & elemDesc = vtxFormat.elements.Add();
				elemDesc.SemanticName = "Normal";
				elemDesc.SemanticIndex = 0;
				elemDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
				elemDesc.InputSlot = 0;
				elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elemDesc.InstanceDataStepRate = 0;
			}
			{
				D3D11_INPUT_ELEMENT_DESC & elemDesc = vtxFormat.elements.Add();
				elemDesc.SemanticName = "TexCoord";
				elemDesc.SemanticIndex = 0;
				elemDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				elemDesc.InputSlot = 0;
				elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elemDesc.InstanceDataStepRate = 0;
			}
			graphics.resources->Create_InputLayout( vtxFormat.elements.ToPtr(), vtxFormat.elements.Num(), vtx_static::IL );
		}
	}
	
	//===========================================================================
	//	Creation / Destruction function
	//===========================================================================
	
	void InitializeModule_r_vertex_formats()
	{
		CreateInputLayouts();
	}
	
	void ShutdownModule_r_vertex_formats()
	{
		graphics.resources->Destroy_InputLayout( vtx_static::IL );
	}
}
