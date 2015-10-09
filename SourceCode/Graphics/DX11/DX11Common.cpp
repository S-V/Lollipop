/*
=============================================================================
	File:	DX11Common.cpp
	Desc:	common renderer stuff, functions that are usually not called every frame
=============================================================================
*/
#include "Graphics_PCH.h"
#pragma hdrstop
#include "Graphics_DX11.h"

#include "DX11Private.h"

#include <Base/Math/Hashing/CRC32.h>

mxNAMESPACE_BEGIN

//-------------------------------------------------------------------------------------------------------------//

#define RET_IF_EQUAL( X )	\
	if( mxStrEquAnsi( str, #X ) ) {	\
		return X;	\
	}

//-------------------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
UINT DXGIFormat_BitsPerPixel( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_A32B32G32R32F:
		return 128;

	case D3DFMT_A16B16G16R16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		return 64;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_G16R16:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_A2W10V10U10:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D24FS8:
	case D3DFMT_INDEX32:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 32;

	case D3DFMT_R8G8B8:
		return 24;

	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_L16:
	case D3DFMT_A8L8:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_V8U8:
	case D3DFMT_CxV8U8:
	case D3DFMT_L6V5U5:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
	case D3DFMT_D16:
	case D3DFMT_INDEX16:
	case D3DFMT_R16F:
	case D3DFMT_YUY2:
		return 16;

	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_A8P8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_A4L4:
		return 8;

	case D3DFMT_DXT1:
		return 4;
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		return  8;

	default:
		Unreachable; // unhandled format
		return 0;
	}
}

UINT DXGIFormat_GetElementSize( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_UNKNOWN : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS : 
	case DXGI_FORMAT_R32G32B32A32_FLOAT : 
	case DXGI_FORMAT_R32G32B32A32_UINT : 
	case DXGI_FORMAT_R32G32B32A32_SINT : 
		return 16;
	case DXGI_FORMAT_R32G32B32_TYPELESS : 
	case DXGI_FORMAT_R32G32B32_FLOAT : 
	case DXGI_FORMAT_R32G32B32_UINT : 
	case DXGI_FORMAT_R32G32B32_SINT : 
		return 12;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
	case DXGI_FORMAT_R16G16B16A16_FLOAT : 
	case DXGI_FORMAT_R16G16B16A16_UNORM : 
	case DXGI_FORMAT_R16G16B16A16_UINT : 
	case DXGI_FORMAT_R16G16B16A16_SNORM : 
	case DXGI_FORMAT_R16G16B16A16_SINT : 
	case DXGI_FORMAT_R32G32_TYPELESS : 
	case DXGI_FORMAT_R32G32_FLOAT : 
	case DXGI_FORMAT_R32G32_UINT : 
	case DXGI_FORMAT_R32G32_SINT : 
	case DXGI_FORMAT_R32G8X24_TYPELESS : 
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
		return 8;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
	case DXGI_FORMAT_R10G10B10A2_UNORM : 
	case DXGI_FORMAT_R10G10B10A2_UINT : 
	case DXGI_FORMAT_R11G11B10_FLOAT : 
	case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
	case DXGI_FORMAT_R8G8B8A8_UNORM : 
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
	case DXGI_FORMAT_R8G8B8A8_UINT : 
	case DXGI_FORMAT_R8G8B8A8_SNORM : 
	case DXGI_FORMAT_R8G8B8A8_SINT : 
	case DXGI_FORMAT_R16G16_TYPELESS : 
	case DXGI_FORMAT_R16G16_FLOAT : 
	case DXGI_FORMAT_R16G16_UNORM : 
	case DXGI_FORMAT_R16G16_UINT : 
	case DXGI_FORMAT_R16G16_SNORM : 
	case DXGI_FORMAT_R16G16_SINT : 
	case DXGI_FORMAT_R32_TYPELESS : 
	case DXGI_FORMAT_D32_FLOAT : 
	case DXGI_FORMAT_R32_FLOAT : 
	case DXGI_FORMAT_R32_UINT : 
	case DXGI_FORMAT_R32_SINT : 
	case DXGI_FORMAT_R24G8_TYPELESS : 
	case DXGI_FORMAT_D24_UNORM_S8_UINT : 
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
		return 4;
	case DXGI_FORMAT_R8G8_TYPELESS : 
	case DXGI_FORMAT_R8G8_UNORM : 
	case DXGI_FORMAT_R8G8_UINT : 
	case DXGI_FORMAT_R8G8_SNORM : 
	case DXGI_FORMAT_R8G8_SINT : 
	case DXGI_FORMAT_R16_TYPELESS : 
	case DXGI_FORMAT_R16_FLOAT : 
	case DXGI_FORMAT_D16_UNORM : 
	case DXGI_FORMAT_R16_UNORM : 
	case DXGI_FORMAT_R16_UINT : 
	case DXGI_FORMAT_R16_SNORM : 
	case DXGI_FORMAT_R16_SINT : 
		return 2;
	case DXGI_FORMAT_R8_TYPELESS : 
	case DXGI_FORMAT_R8_UNORM : 
	case DXGI_FORMAT_R8_UINT : 
	case DXGI_FORMAT_R8_SNORM : 
	case DXGI_FORMAT_R8_SINT : 
	case DXGI_FORMAT_A8_UNORM : 
		return 1;
	case DXGI_FORMAT_R1_UNORM : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
	case DXGI_FORMAT_R8G8_B8G8_UNORM : 
	case DXGI_FORMAT_G8R8_G8B8_UNORM : 
		return 4;
	case DXGI_FORMAT_BC1_TYPELESS : 
	case DXGI_FORMAT_BC1_UNORM : 
	case DXGI_FORMAT_BC1_UNORM_SRGB : 
	case DXGI_FORMAT_BC2_TYPELESS : 
	case DXGI_FORMAT_BC2_UNORM : 
	case DXGI_FORMAT_BC2_UNORM_SRGB : 
	case DXGI_FORMAT_BC3_TYPELESS : 
	case DXGI_FORMAT_BC3_UNORM : 
	case DXGI_FORMAT_BC3_UNORM_SRGB : 
	case DXGI_FORMAT_BC4_TYPELESS : 
	case DXGI_FORMAT_BC4_UNORM : 
	case DXGI_FORMAT_BC4_SNORM : 
	case DXGI_FORMAT_BC5_TYPELESS : 
	case DXGI_FORMAT_BC5_UNORM : 
	case DXGI_FORMAT_BC5_SNORM : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_B5G6R5_UNORM : 
	case DXGI_FORMAT_B5G5R5A1_UNORM : 
		return 2;
	case DXGI_FORMAT_B8G8R8A8_UNORM : 
	case DXGI_FORMAT_B8G8R8X8_UNORM : 
		return 4;
	case DXGI_FORMAT_FORCE_UINT : 
	default:
		;
	}
	Unreachable;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------//

mxWARNING("untested, may contain bugs");
UINT DXGIFormat_GetElementCount( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_UNKNOWN : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS : 
	case DXGI_FORMAT_R32G32B32A32_FLOAT : 
	case DXGI_FORMAT_R32G32B32A32_UINT : 
	case DXGI_FORMAT_R32G32B32A32_SINT : 
		return 4;
	case DXGI_FORMAT_R32G32B32_TYPELESS : 
	case DXGI_FORMAT_R32G32B32_FLOAT : 
	case DXGI_FORMAT_R32G32B32_UINT : 
	case DXGI_FORMAT_R32G32B32_SINT : 
		return 3;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
	case DXGI_FORMAT_R16G16B16A16_FLOAT : 
	case DXGI_FORMAT_R16G16B16A16_UNORM : 
	case DXGI_FORMAT_R16G16B16A16_UINT : 
	case DXGI_FORMAT_R16G16B16A16_SNORM : 
	case DXGI_FORMAT_R16G16B16A16_SINT :
		return 4;
	case DXGI_FORMAT_R32G32_TYPELESS : 
	case DXGI_FORMAT_R32G32_FLOAT : 
	case DXGI_FORMAT_R32G32_UINT : 
	case DXGI_FORMAT_R32G32_SINT : 
		return 2;
	case DXGI_FORMAT_R32G8X24_TYPELESS : 
		return 2;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
		return 3;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
	case DXGI_FORMAT_R10G10B10A2_UNORM : 
	case DXGI_FORMAT_R10G10B10A2_UINT : 
		return 4;
	case DXGI_FORMAT_R11G11B10_FLOAT : 
		return 3;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
	case DXGI_FORMAT_R8G8B8A8_UNORM : 
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
	case DXGI_FORMAT_R8G8B8A8_UINT : 
	case DXGI_FORMAT_R8G8B8A8_SNORM : 
	case DXGI_FORMAT_R8G8B8A8_SINT : 
		return 4;
	case DXGI_FORMAT_R16G16_TYPELESS : 
	case DXGI_FORMAT_R16G16_FLOAT : 
	case DXGI_FORMAT_R16G16_UNORM : 
	case DXGI_FORMAT_R16G16_UINT : 
	case DXGI_FORMAT_R16G16_SNORM : 
	case DXGI_FORMAT_R16G16_SINT : 
		return 2;
	case DXGI_FORMAT_R32_TYPELESS : 
	case DXGI_FORMAT_D32_FLOAT : 
	case DXGI_FORMAT_R32_FLOAT : 
	case DXGI_FORMAT_R32_UINT : 
	case DXGI_FORMAT_R32_SINT : 
	case DXGI_FORMAT_R24G8_TYPELESS : 
	case DXGI_FORMAT_D24_UNORM_S8_UINT : 
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
		return 1;
	case DXGI_FORMAT_R8G8_TYPELESS : 
	case DXGI_FORMAT_R8G8_UNORM : 
	case DXGI_FORMAT_R8G8_UINT : 
	case DXGI_FORMAT_R8G8_SNORM : 
	case DXGI_FORMAT_R8G8_SINT : 
		return 2;
	case DXGI_FORMAT_R16_TYPELESS : 
	case DXGI_FORMAT_R16_FLOAT : 
	case DXGI_FORMAT_D16_UNORM : 
	case DXGI_FORMAT_R16_UNORM : 
	case DXGI_FORMAT_R16_UINT : 
	case DXGI_FORMAT_R16_SNORM : 
	case DXGI_FORMAT_R16_SINT : 
	case DXGI_FORMAT_R8_TYPELESS : 
	case DXGI_FORMAT_R8_UNORM : 
	case DXGI_FORMAT_R8_UINT : 
	case DXGI_FORMAT_R8_SNORM : 
	case DXGI_FORMAT_R8_SINT : 
	case DXGI_FORMAT_A8_UNORM : 
	case DXGI_FORMAT_R1_UNORM : 
		return 1;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
		return 3;
	case DXGI_FORMAT_R8G8_B8G8_UNORM : 
	case DXGI_FORMAT_G8R8_G8B8_UNORM : 
		return 4;
	case DXGI_FORMAT_BC1_TYPELESS : 
	case DXGI_FORMAT_BC1_UNORM : 
	case DXGI_FORMAT_BC1_UNORM_SRGB : 
	case DXGI_FORMAT_BC2_TYPELESS : 
	case DXGI_FORMAT_BC2_UNORM : 
	case DXGI_FORMAT_BC2_UNORM_SRGB : 
	case DXGI_FORMAT_BC3_TYPELESS : 
	case DXGI_FORMAT_BC3_UNORM : 
	case DXGI_FORMAT_BC3_UNORM_SRGB : 
	case DXGI_FORMAT_BC4_TYPELESS : 
	case DXGI_FORMAT_BC4_UNORM : 
	case DXGI_FORMAT_BC4_SNORM : 
	case DXGI_FORMAT_BC5_TYPELESS : 
	case DXGI_FORMAT_BC5_UNORM : 
	case DXGI_FORMAT_BC5_SNORM : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_B5G6R5_UNORM :
		return 3;
	case DXGI_FORMAT_B5G5R5A1_UNORM : 
		return 4;
	case DXGI_FORMAT_B8G8R8A8_UNORM : 
	case DXGI_FORMAT_B8G8R8X8_UNORM : 
		return 4;
	case DXGI_FORMAT_FORCE_UINT : 
	default:
		;
	}
	Unreachable;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------//

LPCSTR D3D_GetSemanticName( EVertexElementUsage usage )
{
	switch( usage )
	{
	case EVertexElementUsage::VEU_Position :		return ("POSITION");
	case EVertexElementUsage::VEU_Normal :			return ("NORMAL");
	case EVertexElementUsage::VEU_TexCoords :		return ("TEXCOORD");
	case EVertexElementUsage::VEU_Color :			return ("COLOR");
	case EVertexElementUsage::VEU_Tangent :			return ("TANGENT");
	case EVertexElementUsage::VEU_Binormal :		return ("BINORMAL");
	case EVertexElementUsage::VEU_BlendWeights :	return ("BLENDWEIGHTS");
	case EVertexElementUsage::VEU_BlendIndices :	return ("BLENDINDICES");
	case EVertexElementUsage::VEU_Unknown :			return ("UNKNOWN");
	}
	Unreachable;
	return nil;
}

//-------------------------------------------------------------------------------------------------------------//

DXGI_FORMAT dxConvertVertexElementType( EVertexElementType type )
{
	switch( type )
	{
	case EVertexElementType::VET_Float1 :	return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	case EVertexElementType::VET_Float2 :	return DXGI_FORMAT_R32G32_FLOAT;
	case EVertexElementType::VET_Float3 :	return DXGI_FORMAT_R32G32B32_FLOAT;
	case EVertexElementType::VET_Float4 :	return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case EVertexElementType::VET_UByte4 :	return DXGI_FORMAT_R8G8B8A8_UINT;
	case EVertexElementType::VET_Unknown :	return DXGI_FORMAT_UNKNOWN;
	}
	Unreachable;
	return DXGI_FORMAT_UNKNOWN;
}

//-------------------------------------------------------------------------------------------------------------//

bool DXGIFormat_HasAlphaChannel( DXGI_FORMAT format )
{
	switch( format )
	{
	case DXGI_FORMAT_UNKNOWN	                 :			Unreachable;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS       :			return true;
	case DXGI_FORMAT_R32G32B32A32_FLOAT          :			return true;
	case DXGI_FORMAT_R32G32B32A32_UINT           :			return true;
	case DXGI_FORMAT_R32G32B32A32_SINT           :			return true;
	case DXGI_FORMAT_R32G32B32_TYPELESS          :			return false;
	case DXGI_FORMAT_R32G32B32_FLOAT             :			return false;
	case DXGI_FORMAT_R32G32B32_UINT              :			return false;
	case DXGI_FORMAT_R32G32B32_SINT              :			return false;
	
	case DXGI_FORMAT_R16G16B16A16_TYPELESS       :			return true;
	case DXGI_FORMAT_R16G16B16A16_FLOAT          :			return true;
	case DXGI_FORMAT_R16G16B16A16_UNORM          :			return true;
	case DXGI_FORMAT_R16G16B16A16_UINT           :			return true;
	case DXGI_FORMAT_R16G16B16A16_SNORM          :			return true;
	case DXGI_FORMAT_R16G16B16A16_SINT           :			return true;
	
	case DXGI_FORMAT_R32G32_TYPELESS             :			return false;
	case DXGI_FORMAT_R32G32_FLOAT                :			return false;
	case DXGI_FORMAT_R32G32_UINT                 :			return false;
	case DXGI_FORMAT_R32G32_SINT                 :			return false;
	case DXGI_FORMAT_R32G8X24_TYPELESS           :			return false;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT        :			return false;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    :			return false;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     :			return false;
	
	case DXGI_FORMAT_R10G10B10A2_TYPELESS        :			return true;
	case DXGI_FORMAT_R10G10B10A2_UNORM           :			return true;
	case DXGI_FORMAT_R10G10B10A2_UINT            :			return true;
	case DXGI_FORMAT_R11G11B10_FLOAT             :			return false;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS           :			return true;
	case DXGI_FORMAT_R8G8B8A8_UNORM              :			return true;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         :			return true;
	case DXGI_FORMAT_R8G8B8A8_UINT               :			return true;
	case DXGI_FORMAT_R8G8B8A8_SNORM              :			return true;
	case DXGI_FORMAT_R8G8B8A8_SINT               :			return true;
	
	case DXGI_FORMAT_R16G16_TYPELESS             :			return false;
	case DXGI_FORMAT_R16G16_FLOAT                :			return false;
	case DXGI_FORMAT_R16G16_UNORM                :			return false;
	case DXGI_FORMAT_R16G16_UINT                 :			return false;
	case DXGI_FORMAT_R16G16_SNORM                :			return false;
	case DXGI_FORMAT_R16G16_SINT                 :			return false;

	case DXGI_FORMAT_R32_TYPELESS                :			return false;
	case DXGI_FORMAT_D32_FLOAT                   :			return false;
	case DXGI_FORMAT_R32_FLOAT                   :			return false;
	case DXGI_FORMAT_R32_UINT                    :			return false;
	case DXGI_FORMAT_R32_SINT                    :			return false;
	case DXGI_FORMAT_R24G8_TYPELESS              :			return false;
	case DXGI_FORMAT_D24_UNORM_S8_UINT           :			return false;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS       :			return false;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT        :			return false;
	
	case DXGI_FORMAT_R8G8_TYPELESS               :			return false;
	case DXGI_FORMAT_R8G8_UNORM                  :			return false;
	case DXGI_FORMAT_R8G8_UINT                   :			return false;
	case DXGI_FORMAT_R8G8_SNORM                  :			return false;
	case DXGI_FORMAT_R8G8_SINT                   :			return false;
	
	case DXGI_FORMAT_R16_TYPELESS                :			return false;
	case DXGI_FORMAT_R16_FLOAT                   :			return false;
	case DXGI_FORMAT_D16_UNORM                   :			return false;
	case DXGI_FORMAT_R16_UNORM                   :			return false;
	case DXGI_FORMAT_R16_UINT                    :			return false;
	case DXGI_FORMAT_R16_SNORM                   :			return false;
	case DXGI_FORMAT_R16_SINT                    :			return false;
	
	case DXGI_FORMAT_R8_TYPELESS                 :			return false;
	case DXGI_FORMAT_R8_UNORM                    :			return false;
	case DXGI_FORMAT_R8_UINT                     :			return false;
	case DXGI_FORMAT_R8_SNORM                    :			return false;
	case DXGI_FORMAT_R8_SINT                     :			return false;
	case DXGI_FORMAT_A8_UNORM                    :			return false;

	case DXGI_FORMAT_R1_UNORM                    :			return false;

	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP          :			return false;
	case DXGI_FORMAT_R8G8_B8G8_UNORM             :			return false;
	case DXGI_FORMAT_G8R8_G8B8_UNORM             :			return false;

	case DXGI_FORMAT_BC1_TYPELESS                :			return false;
	case DXGI_FORMAT_BC1_UNORM                   :			return false;
	case DXGI_FORMAT_BC1_UNORM_SRGB              :			return false;
	case DXGI_FORMAT_BC2_TYPELESS                :			Unimplemented;
	case DXGI_FORMAT_BC2_UNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC2_UNORM_SRGB              :			Unimplemented;
	case DXGI_FORMAT_BC3_TYPELESS                :			Unimplemented;
	case DXGI_FORMAT_BC3_UNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC3_UNORM_SRGB              :			Unimplemented;
	case DXGI_FORMAT_BC4_TYPELESS                :			Unimplemented;
	case DXGI_FORMAT_BC4_UNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC4_SNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC5_TYPELESS                :			Unimplemented;
	case DXGI_FORMAT_BC5_UNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC5_SNORM                   :			Unimplemented;
	case DXGI_FORMAT_B5G6R5_UNORM                :			Unimplemented;
	case DXGI_FORMAT_B5G5R5A1_UNORM              :			return true;
	case DXGI_FORMAT_B8G8R8A8_UNORM              :			return true;
	case DXGI_FORMAT_B8G8R8X8_UNORM              :			Unimplemented;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  :			Unimplemented;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS           :			return true;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         :			return true;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS           :			Unimplemented;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         :			Unimplemented;
	case DXGI_FORMAT_BC6H_TYPELESS               :			Unimplemented;
	case DXGI_FORMAT_BC6H_UF16                   :			Unimplemented;
	case DXGI_FORMAT_BC6H_SF16                   :			Unimplemented;
	case DXGI_FORMAT_BC7_TYPELESS                :			Unimplemented;
	case DXGI_FORMAT_BC7_UNORM                   :			Unimplemented;
	case DXGI_FORMAT_BC7_UNORM_SRGB              :			Unimplemented;

//	case DXGI_FORMAT_FORCE_UINT                  :			Unreachable;
	}
	Unreachable;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------//

const char* DXGIFormat_ToString( DXGI_FORMAT format )
{
    switch( format )
    {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return "DXGI_FORMAT_R32G32B32A32_TYPELESS";
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return "DXGI_FORMAT_R32G32B32A32_FLOAT";
        case DXGI_FORMAT_R32G32B32A32_UINT: return "DXGI_FORMAT_R32G32B32A32_UINT";
        case DXGI_FORMAT_R32G32B32A32_SINT: return "DXGI_FORMAT_R32G32B32A32_SINT";
        case DXGI_FORMAT_R32G32B32_TYPELESS: return "DXGI_FORMAT_R32G32B32_TYPELESS";
        case DXGI_FORMAT_R32G32B32_FLOAT: return "DXGI_FORMAT_R32G32B32_FLOAT";
        case DXGI_FORMAT_R32G32B32_UINT: return "DXGI_FORMAT_R32G32B32_UINT";
        case DXGI_FORMAT_R32G32B32_SINT: return "DXGI_FORMAT_R32G32B32_SINT";
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return "DXGI_FORMAT_R16G16B16A16_TYPELESS";
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return "DXGI_FORMAT_R16G16B16A16_FLOAT";
        case DXGI_FORMAT_R16G16B16A16_UNORM: return "DXGI_FORMAT_R16G16B16A16_UNORM";
        case DXGI_FORMAT_R16G16B16A16_UINT: return "DXGI_FORMAT_R16G16B16A16_UINT";
        case DXGI_FORMAT_R16G16B16A16_SNORM: return "DXGI_FORMAT_R16G16B16A16_SNORM";
        case DXGI_FORMAT_R16G16B16A16_SINT: return "DXGI_FORMAT_R16G16B16A16_SINT";
        case DXGI_FORMAT_R32G32_TYPELESS: return "DXGI_FORMAT_R32G32_TYPELESS";
        case DXGI_FORMAT_R32G32_FLOAT: return "DXGI_FORMAT_R32G32_FLOAT";
        case DXGI_FORMAT_R32G32_UINT: return "DXGI_FORMAT_R32G32_UINT";
        case DXGI_FORMAT_R32G32_SINT: return "DXGI_FORMAT_R32G32_SINT";
        case DXGI_FORMAT_R32G8X24_TYPELESS: return "DXGI_FORMAT_R32G8X24_TYPELESS";
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return "DXGI_FORMAT_D32_FLOAT_S8X24_UINT";
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS";
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT";
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return "DXGI_FORMAT_R10G10B10A2_TYPELESS";
        case DXGI_FORMAT_R10G10B10A2_UNORM: return "DXGI_FORMAT_R10G10B10A2_UNORM";
        case DXGI_FORMAT_R10G10B10A2_UINT: return "DXGI_FORMAT_R10G10B10A2_UINT";
        case DXGI_FORMAT_R11G11B10_FLOAT: return "DXGI_FORMAT_R11G11B10_FLOAT";
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return "DXGI_FORMAT_R8G8B8A8_TYPELESS";
        case DXGI_FORMAT_R8G8B8A8_UNORM: return "DXGI_FORMAT_R8G8B8A8_UNORM";
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
        case DXGI_FORMAT_R8G8B8A8_UINT: return "DXGI_FORMAT_R8G8B8A8_UINT";
        case DXGI_FORMAT_R8G8B8A8_SNORM: return "DXGI_FORMAT_R8G8B8A8_SNORM";
        case DXGI_FORMAT_R8G8B8A8_SINT: return "DXGI_FORMAT_R8G8B8A8_SINT";
        case DXGI_FORMAT_R16G16_TYPELESS: return "DXGI_FORMAT_R16G16_TYPELESS";
        case DXGI_FORMAT_R16G16_FLOAT: return "DXGI_FORMAT_R16G16_FLOAT";
        case DXGI_FORMAT_R16G16_UNORM: return "DXGI_FORMAT_R16G16_UNORM";
        case DXGI_FORMAT_R16G16_UINT: return "DXGI_FORMAT_R16G16_UINT";
        case DXGI_FORMAT_R16G16_SNORM: return "DXGI_FORMAT_R16G16_SNORM";
        case DXGI_FORMAT_R16G16_SINT: return "DXGI_FORMAT_R16G16_SINT";
        case DXGI_FORMAT_R32_TYPELESS: return "DXGI_FORMAT_R32_TYPELESS";
        case DXGI_FORMAT_D32_FLOAT: return "DXGI_FORMAT_D32_FLOAT";
        case DXGI_FORMAT_R32_FLOAT: return "DXGI_FORMAT_R32_FLOAT";
        case DXGI_FORMAT_R32_UINT: return "DXGI_FORMAT_R32_UINT";
        case DXGI_FORMAT_R32_SINT: return "DXGI_FORMAT_R32_SINT";
        case DXGI_FORMAT_R24G8_TYPELESS: return "DXGI_FORMAT_R24G8_TYPELESS";
        case DXGI_FORMAT_D24_UNORM_S8_UINT: return "DXGI_FORMAT_D24_UNORM_S8_UINT";
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return "DXGI_FORMAT_R24_UNORM_X8_TYPELESS";
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return "DXGI_FORMAT_X24_TYPELESS_G8_UINT";
        case DXGI_FORMAT_R8G8_TYPELESS: return "DXGI_FORMAT_R8G8_TYPELESS";
        case DXGI_FORMAT_R8G8_UNORM: return "DXGI_FORMAT_R8G8_UNORM";
        case DXGI_FORMAT_R8G8_UINT: return "DXGI_FORMAT_R8G8_UINT";
        case DXGI_FORMAT_R8G8_SNORM: return "DXGI_FORMAT_R8G8_SNORM";
        case DXGI_FORMAT_R8G8_SINT: return "DXGI_FORMAT_R8G8_SINT";
        case DXGI_FORMAT_R16_TYPELESS: return "DXGI_FORMAT_R16_TYPELESS";
        case DXGI_FORMAT_R16_FLOAT: return "DXGI_FORMAT_R16_FLOAT";
        case DXGI_FORMAT_D16_UNORM: return "DXGI_FORMAT_D16_UNORM";
        case DXGI_FORMAT_R16_UNORM: return "DXGI_FORMAT_R16_UNORM";
        case DXGI_FORMAT_R16_UINT: return "DXGI_FORMAT_R16_UINT";
        case DXGI_FORMAT_R16_SNORM: return "DXGI_FORMAT_R16_SNORM";
        case DXGI_FORMAT_R16_SINT: return "DXGI_FORMAT_R16_SINT";
        case DXGI_FORMAT_R8_TYPELESS: return "DXGI_FORMAT_R8_TYPELESS";
        case DXGI_FORMAT_R8_UNORM: return "DXGI_FORMAT_R8_UNORM";
        case DXGI_FORMAT_R8_UINT: return "DXGI_FORMAT_R8_UINT";
        case DXGI_FORMAT_R8_SNORM: return "DXGI_FORMAT_R8_SNORM";
        case DXGI_FORMAT_R8_SINT: return "DXGI_FORMAT_R8_SINT";
        case DXGI_FORMAT_A8_UNORM: return "DXGI_FORMAT_A8_UNORM";
        case DXGI_FORMAT_R1_UNORM: return "DXGI_FORMAT_R1_UNORM";
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return "DXGI_FORMAT_R9G9B9E5_SHAREDEXP";
        case DXGI_FORMAT_R8G8_B8G8_UNORM: return "DXGI_FORMAT_R8G8_B8G8_UNORM";
        case DXGI_FORMAT_G8R8_G8B8_UNORM: return "DXGI_FORMAT_G8R8_G8B8_UNORM";
        case DXGI_FORMAT_BC1_TYPELESS: return "DXGI_FORMAT_BC1_TYPELESS";
        case DXGI_FORMAT_BC1_UNORM: return "DXGI_FORMAT_BC1_UNORM";
        case DXGI_FORMAT_BC1_UNORM_SRGB: return "DXGI_FORMAT_BC1_UNORM_SRGB";
        case DXGI_FORMAT_BC2_TYPELESS: return "DXGI_FORMAT_BC2_TYPELESS";
        case DXGI_FORMAT_BC2_UNORM: return "DXGI_FORMAT_BC2_UNORM";
        case DXGI_FORMAT_BC2_UNORM_SRGB: return "DXGI_FORMAT_BC2_UNORM_SRGB";
        case DXGI_FORMAT_BC3_TYPELESS: return "DXGI_FORMAT_BC3_TYPELESS";
        case DXGI_FORMAT_BC3_UNORM: return "DXGI_FORMAT_BC3_UNORM";
        case DXGI_FORMAT_BC3_UNORM_SRGB: return "DXGI_FORMAT_BC3_UNORM_SRGB";
        case DXGI_FORMAT_BC4_TYPELESS: return "DXGI_FORMAT_BC4_TYPELESS";
        case DXGI_FORMAT_BC4_UNORM: return "DXGI_FORMAT_BC4_UNORM";
        case DXGI_FORMAT_BC4_SNORM: return "DXGI_FORMAT_BC4_SNORM";
        case DXGI_FORMAT_BC5_TYPELESS: return "DXGI_FORMAT_BC5_TYPELESS";
        case DXGI_FORMAT_BC5_UNORM: return "DXGI_FORMAT_BC5_UNORM";
        case DXGI_FORMAT_BC5_SNORM: return "DXGI_FORMAT_BC5_SNORM";
        case DXGI_FORMAT_B5G6R5_UNORM: return "DXGI_FORMAT_B5G6R5_UNORM";
        case DXGI_FORMAT_B5G5R5A1_UNORM: return "DXGI_FORMAT_B5G5R5A1_UNORM";
        case DXGI_FORMAT_B8G8R8A8_UNORM: return "DXGI_FORMAT_B8G8R8A8_UNORM";
        default:                         return "Unknown format";
    }
}
//-------------------------------------------------------------------------------------------------------------//
DXGI_FORMAT String_ToDXGIFormat( const char* str )
{
	RET_IF_EQUAL( DXGI_FORMAT_UNKNOWN					);
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32A32_TYPELESS       );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32A32_FLOAT		  );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32A32_UINT           );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32A32_SINT           );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32_TYPELESS          );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32_FLOAT             );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32_UINT              );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32B32_SINT              );
	
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_TYPELESS       );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_FLOAT          );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_UNORM          );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_UINT           );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_SNORM          );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16B16A16_SINT           );
	
	RET_IF_EQUAL( DXGI_FORMAT_R32G32_TYPELESS             );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32_FLOAT                );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32_UINT                 );
	RET_IF_EQUAL( DXGI_FORMAT_R32G32_SINT                 );
	RET_IF_EQUAL( DXGI_FORMAT_R32G8X24_TYPELESS           );
	RET_IF_EQUAL( DXGI_FORMAT_D32_FLOAT_S8X24_UINT        );
	RET_IF_EQUAL( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    );
	RET_IF_EQUAL( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     );
	
	RET_IF_EQUAL( DXGI_FORMAT_R10G10B10A2_TYPELESS        );
	RET_IF_EQUAL( DXGI_FORMAT_R10G10B10A2_UNORM           );
	RET_IF_EQUAL( DXGI_FORMAT_R10G10B10A2_UINT            );
	RET_IF_EQUAL( DXGI_FORMAT_R11G11B10_FLOAT             );

	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_TYPELESS           );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_UNORM              );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_UINT               );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_SNORM              );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8B8A8_SINT               );
	
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_TYPELESS             );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_FLOAT                );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_UNORM                );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_UINT                 );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_SNORM                );
	RET_IF_EQUAL( DXGI_FORMAT_R16G16_SINT                 );

	RET_IF_EQUAL( DXGI_FORMAT_R32_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_D32_FLOAT                   );
	RET_IF_EQUAL( DXGI_FORMAT_R32_FLOAT                   );
	RET_IF_EQUAL( DXGI_FORMAT_R32_UINT                    );
	RET_IF_EQUAL( DXGI_FORMAT_R32_SINT                    );
	RET_IF_EQUAL( DXGI_FORMAT_R24G8_TYPELESS              );
	RET_IF_EQUAL( DXGI_FORMAT_D24_UNORM_S8_UINT           );
	RET_IF_EQUAL( DXGI_FORMAT_R24_UNORM_X8_TYPELESS       );
	RET_IF_EQUAL( DXGI_FORMAT_X24_TYPELESS_G8_UINT        );
	
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_TYPELESS               );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_UNORM                  );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_UINT                   );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_SNORM                  );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_SINT                   );
	
	RET_IF_EQUAL( DXGI_FORMAT_R16_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_R16_FLOAT                   );
	RET_IF_EQUAL( DXGI_FORMAT_D16_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_R16_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_R16_UINT                    );
	RET_IF_EQUAL( DXGI_FORMAT_R16_SNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_R16_SINT                    );
	
	RET_IF_EQUAL( DXGI_FORMAT_R8_TYPELESS                 );
	RET_IF_EQUAL( DXGI_FORMAT_R8_UNORM                    );
	RET_IF_EQUAL( DXGI_FORMAT_R8_UINT                     );
	RET_IF_EQUAL( DXGI_FORMAT_R8_SNORM                    );
	RET_IF_EQUAL( DXGI_FORMAT_R8_SINT                     );
	RET_IF_EQUAL( DXGI_FORMAT_A8_UNORM                    );

	RET_IF_EQUAL( DXGI_FORMAT_R1_UNORM                    );

	RET_IF_EQUAL( DXGI_FORMAT_R9G9B9E5_SHAREDEXP          );
	RET_IF_EQUAL( DXGI_FORMAT_R8G8_B8G8_UNORM             );
	RET_IF_EQUAL( DXGI_FORMAT_G8R8_G8B8_UNORM             );

	RET_IF_EQUAL( DXGI_FORMAT_BC1_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC1_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC1_UNORM_SRGB              );
	RET_IF_EQUAL( DXGI_FORMAT_BC2_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC2_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC2_UNORM_SRGB              );
	RET_IF_EQUAL( DXGI_FORMAT_BC3_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC3_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC3_UNORM_SRGB              );
	RET_IF_EQUAL( DXGI_FORMAT_BC4_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC4_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC4_SNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC5_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC5_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC5_SNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_B5G6R5_UNORM                );
	RET_IF_EQUAL( DXGI_FORMAT_B5G5R5A1_UNORM              );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8A8_UNORM              );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8X8_UNORM              );
	RET_IF_EQUAL( DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8A8_TYPELESS           );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8X8_TYPELESS           );
	RET_IF_EQUAL( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         );
	RET_IF_EQUAL( DXGI_FORMAT_BC6H_TYPELESS               );
	RET_IF_EQUAL( DXGI_FORMAT_BC6H_UF16                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC6H_SF16                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC7_TYPELESS                );
	RET_IF_EQUAL( DXGI_FORMAT_BC7_UNORM                   );
	RET_IF_EQUAL( DXGI_FORMAT_BC7_UNORM_SRGB              );

#if RX_DEBUG_RENDERER
	dxWarnf("Unknown format: '%s'.\n",str);
#endif

	return DXGI_FORMAT_UNKNOWN;
}

//--------------------------------------------------------------------------------------
// Helper functions to create SRGB formats from typeless formats and vice versa
//--------------------------------------------------------------------------------------

DXGI_FORMAT DXGIFormat_MAKE_SRGB( DXGI_FORMAT format )
{
	switch( format )
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	};

	return format;
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT DXGIFormat_MAKE_TYPELESS( DXGI_FORMAT format )
{
	switch( format )
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;

	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_TYPELESS;
	};

	return format;
}
//-------------------------------------------------------------------------------------------------------------//
mxWARNING("untested, may contain bugs");
const char* DXGIFormat_BaseTypeString( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_UNKNOWN : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS :
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_FLOAT : 
		return "float";
	case DXGI_FORMAT_R32G32B32A32_UINT : 
		return "uint";
	case DXGI_FORMAT_R32G32B32A32_SINT : 
		return "int";
	case DXGI_FORMAT_R32G32B32_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32_FLOAT : 
		return "float";
	case DXGI_FORMAT_R32G32B32_UINT : 
		return "uint";
	case DXGI_FORMAT_R32G32B32_SINT : 
		return "int";
	case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R16G16B16A16_FLOAT : 
		return "float";
	case DXGI_FORMAT_R16G16B16A16_UNORM : 
		return "uint";
	case DXGI_FORMAT_R16G16B16A16_UINT : 
		return "uint";
	case DXGI_FORMAT_R16G16B16A16_SNORM : 
		return "int";
	case DXGI_FORMAT_R16G16B16A16_SINT :
		return "int";
	case DXGI_FORMAT_R32G32_TYPELESS : 
		Unreachable;
		return 0;

		mxUNDONE;

	//case DXGI_FORMAT_R32G32_FLOAT : 
	//case DXGI_FORMAT_R32G32_UINT : 
	//case DXGI_FORMAT_R32G32_SINT : 
	//	return 2;
	//case DXGI_FORMAT_R32G8X24_TYPELESS : 
	//	return 2;
	//case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
	//case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
	//case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
	//	return 3;
	//case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
	//case DXGI_FORMAT_R10G10B10A2_UNORM : 
	//case DXGI_FORMAT_R10G10B10A2_UINT : 
	//	return 4;
	//case DXGI_FORMAT_R11G11B10_FLOAT : 
	//	return 3;
	//case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
	//case DXGI_FORMAT_R8G8B8A8_UNORM : 
	//case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
	//case DXGI_FORMAT_R8G8B8A8_UINT : 
	//case DXGI_FORMAT_R8G8B8A8_SNORM : 
	//case DXGI_FORMAT_R8G8B8A8_SINT : 
	//	return 4;
	//case DXGI_FORMAT_R16G16_TYPELESS : 
	//case DXGI_FORMAT_R16G16_FLOAT : 
	//case DXGI_FORMAT_R16G16_UNORM : 
	//case DXGI_FORMAT_R16G16_UINT : 
	//case DXGI_FORMAT_R16G16_SNORM : 
	//case DXGI_FORMAT_R16G16_SINT : 
	//	return 2;
	//case DXGI_FORMAT_R32_TYPELESS : 
	//case DXGI_FORMAT_D32_FLOAT : 
	//case DXGI_FORMAT_R32_FLOAT : 
	//case DXGI_FORMAT_R32_UINT : 
	//case DXGI_FORMAT_R32_SINT : 
	//case DXGI_FORMAT_R24G8_TYPELESS : 
	//case DXGI_FORMAT_D24_UNORM_S8_UINT : 
	//case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
	//case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
	//	return 1;
	//case DXGI_FORMAT_R8G8_TYPELESS : 
	//case DXGI_FORMAT_R8G8_UNORM : 
	//case DXGI_FORMAT_R8G8_UINT : 
	//case DXGI_FORMAT_R8G8_SNORM : 
	//case DXGI_FORMAT_R8G8_SINT : 
	//	return 2;
	//case DXGI_FORMAT_R16_TYPELESS : 
	//case DXGI_FORMAT_R16_FLOAT : 
	//case DXGI_FORMAT_D16_UNORM : 
	//case DXGI_FORMAT_R16_UNORM : 
	//case DXGI_FORMAT_R16_UINT : 
	//case DXGI_FORMAT_R16_SNORM : 
	//case DXGI_FORMAT_R16_SINT : 
	//case DXGI_FORMAT_R8_TYPELESS : 
	//case DXGI_FORMAT_R8_UNORM : 
	//case DXGI_FORMAT_R8_UINT : 
	//case DXGI_FORMAT_R8_SNORM : 
	//case DXGI_FORMAT_R8_SINT : 
	//case DXGI_FORMAT_A8_UNORM : 
	//case DXGI_FORMAT_R1_UNORM : 
	//	return 1;
	//case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
	//	return 3;
	//case DXGI_FORMAT_R8G8_B8G8_UNORM : 
	//case DXGI_FORMAT_G8R8_G8B8_UNORM : 
	//	return 4;
	//case DXGI_FORMAT_BC1_TYPELESS : 
	//case DXGI_FORMAT_BC1_UNORM : 
	//case DXGI_FORMAT_BC1_UNORM_SRGB : 
	//case DXGI_FORMAT_BC2_TYPELESS : 
	//case DXGI_FORMAT_BC2_UNORM : 
	//case DXGI_FORMAT_BC2_UNORM_SRGB : 
	//case DXGI_FORMAT_BC3_TYPELESS : 
	//case DXGI_FORMAT_BC3_UNORM : 
	//case DXGI_FORMAT_BC3_UNORM_SRGB : 
	//case DXGI_FORMAT_BC4_TYPELESS : 
	//case DXGI_FORMAT_BC4_UNORM : 
	//case DXGI_FORMAT_BC4_SNORM : 
	//case DXGI_FORMAT_BC5_TYPELESS : 
	//case DXGI_FORMAT_BC5_UNORM : 
	//case DXGI_FORMAT_BC5_SNORM : 
	//	Unreachable;
	//	return 0;
	//case DXGI_FORMAT_B5G6R5_UNORM :
	//	return 3;
	//case DXGI_FORMAT_B5G5R5A1_UNORM : 
	//	return 4;
	//case DXGI_FORMAT_B8G8R8A8_UNORM : 
	//case DXGI_FORMAT_B8G8R8X8_UNORM : 
	//	return 4;
	//case DXGI_FORMAT_FORCE_UINT : 
	default:
		;
	}
	Unreachable;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------//
mxWARNING("untested, may contain bugs");
const char* DXGIFormat_TypeString( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_UNKNOWN : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS :
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_FLOAT : 
		return "float4";
	case DXGI_FORMAT_R32G32B32A32_UINT : 
		return "uint4";
	case DXGI_FORMAT_R32G32B32A32_SINT : 
		return "int4";
	case DXGI_FORMAT_R32G32B32_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32_FLOAT : 
		return "float3";
	case DXGI_FORMAT_R32G32B32_UINT : 
		return "uint3";
	case DXGI_FORMAT_R32G32B32_SINT : 
		return "int3";
	case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R16G16B16A16_FLOAT : 
		return "float4";
	case DXGI_FORMAT_R16G16B16A16_UNORM : 
		return "uint4";
	case DXGI_FORMAT_R16G16B16A16_UINT : 
		return "uint4";
	case DXGI_FORMAT_R16G16B16A16_SNORM : 
		return "int4";
	case DXGI_FORMAT_R16G16B16A16_SINT :
		return "int4";
	case DXGI_FORMAT_R32G32_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32_FLOAT : 
		return "float2";
	case DXGI_FORMAT_R32G32_UINT : 
		return "uint2";
	case DXGI_FORMAT_R32G32_SINT : 
		return "int2";
	case DXGI_FORMAT_R32G8X24_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
	case DXGI_FORMAT_R10G10B10A2_UNORM : 
	case DXGI_FORMAT_R10G10B10A2_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R11G11B10_FLOAT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8B8A8_UNORM : 
		return "float4";
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
		return "float4";
	case DXGI_FORMAT_R8G8B8A8_UINT : 
		return "uint4";
	case DXGI_FORMAT_R8G8B8A8_SNORM : 
	case DXGI_FORMAT_R8G8B8A8_SINT : 
		return "int4";
	case DXGI_FORMAT_R16G16_TYPELESS : 
	case DXGI_FORMAT_R16G16_FLOAT : 
	case DXGI_FORMAT_R16G16_UNORM : 
	case DXGI_FORMAT_R16G16_UINT : 
	case DXGI_FORMAT_R16G16_SNORM : 
	case DXGI_FORMAT_R16G16_SINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R32_TYPELESS : 
	case DXGI_FORMAT_D32_FLOAT : 
	case DXGI_FORMAT_R32_FLOAT : 
	case DXGI_FORMAT_R32_UINT : 
	case DXGI_FORMAT_R32_SINT : 
	case DXGI_FORMAT_R24G8_TYPELESS : 
	case DXGI_FORMAT_D24_UNORM_S8_UINT : 
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8_TYPELESS : 
	case DXGI_FORMAT_R8G8_UNORM : 
	case DXGI_FORMAT_R8G8_UINT : 
	case DXGI_FORMAT_R8G8_SNORM : 
	case DXGI_FORMAT_R8G8_SINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R16_TYPELESS : 
	case DXGI_FORMAT_R16_FLOAT : 
	case DXGI_FORMAT_D16_UNORM : 
	case DXGI_FORMAT_R16_UNORM : 
	case DXGI_FORMAT_R16_UINT : 
	case DXGI_FORMAT_R16_SNORM : 
	case DXGI_FORMAT_R16_SINT : 
	case DXGI_FORMAT_R8_TYPELESS : 
	case DXGI_FORMAT_R8_UNORM : 
	case DXGI_FORMAT_R8_UINT : 
	case DXGI_FORMAT_R8_SNORM : 
	case DXGI_FORMAT_R8_SINT : 
	case DXGI_FORMAT_A8_UNORM : 
	case DXGI_FORMAT_R1_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8_B8G8_UNORM : 
	case DXGI_FORMAT_G8R8_G8B8_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_BC1_TYPELESS : 
	case DXGI_FORMAT_BC1_UNORM : 
	case DXGI_FORMAT_BC1_UNORM_SRGB : 
	case DXGI_FORMAT_BC2_TYPELESS : 
	case DXGI_FORMAT_BC2_UNORM : 
	case DXGI_FORMAT_BC2_UNORM_SRGB : 
	case DXGI_FORMAT_BC3_TYPELESS : 
	case DXGI_FORMAT_BC3_UNORM : 
	case DXGI_FORMAT_BC3_UNORM_SRGB : 
	case DXGI_FORMAT_BC4_TYPELESS : 
	case DXGI_FORMAT_BC4_UNORM : 
	case DXGI_FORMAT_BC4_SNORM : 
	case DXGI_FORMAT_BC5_TYPELESS : 
	case DXGI_FORMAT_BC5_UNORM : 
	case DXGI_FORMAT_BC5_SNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B5G6R5_UNORM :
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B5G5R5A1_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B8G8R8A8_UNORM : 
	case DXGI_FORMAT_B8G8R8X8_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_FORCE_UINT : 
	default:
		;
	}
	Unreachable;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------//
const char* DXGIFormat_ToEngineTypeString( DXGI_FORMAT format, EVertexElementUsage semantic )
{
	switch(format)
	{
	case DXGI_FORMAT_UNKNOWN : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS :
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32A32_FLOAT : 
		return "float4";
	case DXGI_FORMAT_R32G32B32A32_UINT : 
		//return "uint4";
	case DXGI_FORMAT_R32G32B32A32_SINT : 
		//return "int4";
	case DXGI_FORMAT_R32G32B32_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32B32_FLOAT : 
		return "float3";
	case DXGI_FORMAT_R32G32B32_UINT : 
		//return "uint3";
	case DXGI_FORMAT_R32G32B32_SINT : 
		//return "int3";
	case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R16G16B16A16_FLOAT : 
		//return "float4";
	case DXGI_FORMAT_R16G16B16A16_UNORM : 
		//return "uint4";
	case DXGI_FORMAT_R16G16B16A16_UINT : 
		//return "uint4";
	case DXGI_FORMAT_R16G16B16A16_SNORM : 
		//return "int4";
	case DXGI_FORMAT_R16G16B16A16_SINT :
		//return "int4";
	case DXGI_FORMAT_R32G32_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_R32G32_FLOAT : 
		return "float2";
	case DXGI_FORMAT_R32G32_UINT : 
		//return "uint2";
	case DXGI_FORMAT_R32G32_SINT : 
		//return "int2";
	case DXGI_FORMAT_R32G8X24_TYPELESS : 
		Unreachable;
		return 0;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
	case DXGI_FORMAT_R10G10B10A2_UNORM : 
	case DXGI_FORMAT_R10G10B10A2_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R11G11B10_FLOAT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
	case DXGI_FORMAT_R8G8B8A8_UNORM :
		if( VEU_Color == semantic ) {
			return "R8G8B8A8";
		}
		Unimplemented;

	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
		Unimplemented;

	case DXGI_FORMAT_R8G8B8A8_UINT : 
		if( VEU_Normal == semantic ) {
			return "rxNormal4";
		}
		if( VEU_Tangent == semantic ) {
			return "rxNormal4";
		}
		Unimplemented;
		return 0;

	case DXGI_FORMAT_R8G8B8A8_SNORM :
		Unimplemented;
		return 0;

	case DXGI_FORMAT_R8G8B8A8_SINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R16G16_TYPELESS :
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R16G16_FLOAT : 
		return "float2";
	case DXGI_FORMAT_R16G16_UNORM : 
	case DXGI_FORMAT_R16G16_UINT : 
		return "uint2";
	case DXGI_FORMAT_R16G16_SNORM : 
	case DXGI_FORMAT_R16G16_SINT : 
		return "int2";
	case DXGI_FORMAT_R32_TYPELESS : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_D32_FLOAT : 
	case DXGI_FORMAT_R32_FLOAT :
		return "float";
	case DXGI_FORMAT_R32_UINT : 
	case DXGI_FORMAT_R32_SINT : 
	case DXGI_FORMAT_R24G8_TYPELESS : 
	case DXGI_FORMAT_D24_UNORM_S8_UINT : 
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8_TYPELESS : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8_UNORM : 
	case DXGI_FORMAT_R8G8_UINT : 
		return "uint2";
	case DXGI_FORMAT_R8G8_SNORM : 
	case DXGI_FORMAT_R8G8_SINT : 
		return "int2";
	case DXGI_FORMAT_R16_TYPELESS : 
	case DXGI_FORMAT_R16_FLOAT : 
	case DXGI_FORMAT_D16_UNORM : 
	case DXGI_FORMAT_R16_UNORM : 
	case DXGI_FORMAT_R16_UINT : 
	case DXGI_FORMAT_R16_SNORM : 
	case DXGI_FORMAT_R16_SINT : 
	case DXGI_FORMAT_R8_TYPELESS : 
	case DXGI_FORMAT_R8_UNORM : 
	case DXGI_FORMAT_R8_UINT : 
	case DXGI_FORMAT_R8_SNORM : 
	case DXGI_FORMAT_R8_SINT : 
	case DXGI_FORMAT_A8_UNORM : 
	case DXGI_FORMAT_R1_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_R8G8_B8G8_UNORM : 
	case DXGI_FORMAT_G8R8_G8B8_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_BC1_TYPELESS : 
	case DXGI_FORMAT_BC1_UNORM : 
	case DXGI_FORMAT_BC1_UNORM_SRGB : 
	case DXGI_FORMAT_BC2_TYPELESS : 
	case DXGI_FORMAT_BC2_UNORM : 
	case DXGI_FORMAT_BC2_UNORM_SRGB : 
	case DXGI_FORMAT_BC3_TYPELESS : 
	case DXGI_FORMAT_BC3_UNORM : 
	case DXGI_FORMAT_BC3_UNORM_SRGB : 
	case DXGI_FORMAT_BC4_TYPELESS : 
	case DXGI_FORMAT_BC4_UNORM : 
	case DXGI_FORMAT_BC4_SNORM : 
	case DXGI_FORMAT_BC5_TYPELESS : 
	case DXGI_FORMAT_BC5_UNORM : 
	case DXGI_FORMAT_BC5_SNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B5G6R5_UNORM :
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B5G5R5A1_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_B8G8R8A8_UNORM : 
	case DXGI_FORMAT_B8G8R8X8_UNORM : 
		Unimplemented;
		return 0;
	case DXGI_FORMAT_FORCE_UINT : 
	default:
		;
	}
	Unreachable;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------//
bool D3D_SupportsPrimitiveTopology( EPrimitiveType::Code topology )
{
	switch( topology )
	{
		case EPrimitiveType::PT_PointList :		return true;
		case EPrimitiveType::PT_LineList :		return true;
		case EPrimitiveType::PT_LineStrip :		return true;
		case EPrimitiveType::PT_TriangleList :	return true;
		case EPrimitiveType::PT_TriangleStrip :	return true;
		case EPrimitiveType::PT_TriangleFan :	return false;
		case EPrimitiveType::PT_Quads:			return false;
	}
	Unreachable;
	return false;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_PRIMITIVE_TOPOLOGY D3D_GetPrimitiveTopology( EPrimitiveType::Code topology )
{
#if 0

	switch( topology )
	{
		case EPrimitiveType::PT_Unknown :		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case EPrimitiveType::PT_PointList :		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case EPrimitiveType::PT_LineList :		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case EPrimitiveType::PT_LineStrip :		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case EPrimitiveType::PT_TriangleList :	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case EPrimitiveType::PT_TriangleStrip :	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case EPrimitiveType::PT_TriangleFan :	Unimplemented;
		case EPrimitiveType::PT_Quads:			Unimplemented;
	}
	Unreachable;
	return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

#else

	// for performance
	mxSTATIC_ASSERT(
			(int)EPrimitiveType::PT_Unknown == (int)D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED
		&& (int)EPrimitiveType::PT_PointList == (int)D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
		&& (int)EPrimitiveType::PT_LineList == (int)D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		&& (int)EPrimitiveType::PT_LineStrip == (int)D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
		&& (int)EPrimitiveType::PT_TriangleList == (int)D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		&& (int)EPrimitiveType::PT_TriangleStrip == (int)D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);
	AssertX( topology != EPrimitiveType::PT_TriangleFan, "Triangle fans are not supported in Direct3D 11" );
	AssertX( topology != EPrimitiveType::PT_Quads, "Quads are not supported in Direct3D 11" );

	return c_cast(D3D11_PRIMITIVE_TOPOLOGY) topology;

#endif
}
//-------------------------------------------------------------------------------------------------------------//
EPrimitiveType::Code D3D_GetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	// for performance
	mxSTATIC_ASSERT(
			(int)EPrimitiveType::PT_Unknown == (int)D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED
		&& (int)EPrimitiveType::PT_PointList == (int)D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
		&& (int)EPrimitiveType::PT_LineList == (int)D3D11_PRIMITIVE_TOPOLOGY_LINELIST
		&& (int)EPrimitiveType::PT_LineStrip == (int)D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
		&& (int)EPrimitiveType::PT_TriangleList == (int)D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		&& (int)EPrimitiveType::PT_TriangleStrip == (int)D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);
	return c_cast(EPrimitiveType::Code) topology;
}
//-------------------------------------------------------------------------------------------------------------//
const char* D3D_FeatureLevelToStr( D3D_FEATURE_LEVEL featureLevel )
{
	switch( featureLevel )
	{
	case D3D_FEATURE_LEVEL_11_0 :	return "D3D_FEATURE_LEVEL_11_0";
	case D3D_FEATURE_LEVEL_10_1 :	return "D3D_FEATURE_LEVEL_10_1";
	case D3D_FEATURE_LEVEL_10_0 :	return "D3D_FEATURE_LEVEL_10_0";
	case D3D_FEATURE_LEVEL_9_3 :	return "D3D_FEATURE_LEVEL_9_3";
	case D3D_FEATURE_LEVEL_9_2 :	return "D3D_FEATURE_LEVEL_9_2";
	case D3D_FEATURE_LEVEL_9_1 :	return "D3D_FEATURE_LEVEL_9_1";
	default:	Unreachable;		return "Unknown";
	}
}
//-------------------------------------------------------------------------------------------------------------//
const char* DXGI_ScanlineOrder_ToStr( DXGI_MODE_SCANLINE_ORDER scanlineOrder )
{
	switch( scanlineOrder )
	{
	case DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED :			return "DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED";
	case DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE :			return "DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE";
	case DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST :	return "DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST";
	case DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST :	return "DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST";
	default:	Unreachable;
	}
	return "Unknown";
}
//-------------------------------------------------------------------------------------------------------------//
const char* DXGI_ScalingMode_ToStr( DXGI_MODE_SCALING scaling )
{
	switch( scaling )
	{
	case DXGI_MODE_SCALING_UNSPECIFIED :return "DXGI_MODE_SCALING_UNSPECIFIED";
	case DXGI_MODE_SCALING_CENTERED :	return "DXGI_MODE_SCALING_CENTERED";
	case DXGI_MODE_SCALING_STRETCHED :	return "DXGI_MODE_SCALING_STRETCHED";
	default:	Unreachable;
	}
	return "Unknown";
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_INPUT_CLASSIFICATION D3D_GetInputSlotClass( const char* str )
{
	if( 0 == mxStrCmpAnsi( str, "D3D11_INPUT_PER_VERTEX_DATA" ) ) {
		return D3D11_INPUT_PER_VERTEX_DATA;
	}
	if( 0 == mxStrCmpAnsi( str, "D3D11_INPUT_PER_INSTANCE_DATA" ) ) {
		return D3D11_INPUT_PER_INSTANCE_DATA;
	}
	mxDEBUG_BREAK;
	return D3D11_INPUT_PER_VERTEX_DATA;
}

//-------------------------------------------------------------------------------------------------------------//

D3D11_FILTER D3D_TextureFilteringModeFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_FILTER_MIN_MAG_MIP_POINT );
	RET_IF_EQUAL( D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR );
	RET_IF_EQUAL( D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT );
	RET_IF_EQUAL( D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR );

	RET_IF_EQUAL( D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT );
	RET_IF_EQUAL( D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR );
	RET_IF_EQUAL( D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT );
	RET_IF_EQUAL( D3D11_FILTER_MIN_MAG_MIP_LINEAR );
	RET_IF_EQUAL( D3D11_FILTER_ANISOTROPIC              );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT               );
	
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR             );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT                );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR                );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT                 );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR                );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT                 );

	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR                );
	RET_IF_EQUAL( D3D11_FILTER_COMPARISON_ANISOTROPIC                   );
	
#if RX_DEBUG_RENDERER
	dxWarnf("Unknown texture filtering mode: '%s'.\n",str);
#endif

	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_TEXTURE_ADDRESS_MODE D3D_TextureAddressingModeFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_TEXTURE_ADDRESS_WRAP                );
	RET_IF_EQUAL( D3D11_TEXTURE_ADDRESS_MIRROR                );
	RET_IF_EQUAL( D3D11_TEXTURE_ADDRESS_CLAMP                 );
	RET_IF_EQUAL( D3D11_TEXTURE_ADDRESS_BORDER                );
	RET_IF_EQUAL( D3D11_TEXTURE_ADDRESS_MIRROR_ONCE                 );

#if RX_DEBUG_RENDERER
	dxWarnf("Unknown texture addressing mode: '%s'.\n",str);
#endif

	return D3D11_TEXTURE_ADDRESS_WRAP;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_COMPARISON_FUNC D3D_ComparisonFunctionFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_COMPARISON_NEVER                );
	RET_IF_EQUAL( D3D11_COMPARISON_LESS                );
	RET_IF_EQUAL( D3D11_COMPARISON_EQUAL                 );
	RET_IF_EQUAL( D3D11_COMPARISON_LESS_EQUAL                );
	RET_IF_EQUAL( D3D11_COMPARISON_GREATER                 );
	RET_IF_EQUAL( D3D11_COMPARISON_NOT_EQUAL                 );
	RET_IF_EQUAL( D3D11_COMPARISON_GREATER_EQUAL                 );
	RET_IF_EQUAL( D3D11_COMPARISON_ALWAYS                 );

#if RX_DEBUG_RENDERER
	dxWarnf("Unknown texture addressing mode: '%s'.\n",str);
#endif

	return D3D11_COMPARISON_NEVER;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_DEPTH_WRITE_MASK D3D_DepthWriteMaskFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_DEPTH_WRITE_MASK_ZERO                );
	RET_IF_EQUAL( D3D11_DEPTH_WRITE_MASK_ALL                );

#if RX_DEBUG_RENDERER
	dxWarnf("Unknown depth write mask: '%s'.\n",str);
#endif

	return D3D11_DEPTH_WRITE_MASK_ZERO;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_BLEND D3D_BlendOptionFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_BLEND_ZERO                );
	RET_IF_EQUAL( D3D11_BLEND_ONE                );
	RET_IF_EQUAL( D3D11_BLEND_SRC_COLOR                );
	RET_IF_EQUAL( D3D11_BLEND_INV_SRC_COLOR                );
	RET_IF_EQUAL( D3D11_BLEND_SRC_ALPHA                );
	RET_IF_EQUAL( D3D11_BLEND_INV_SRC_ALPHA                );
	RET_IF_EQUAL( D3D11_BLEND_DEST_ALPHA                );
	RET_IF_EQUAL( D3D11_BLEND_INV_DEST_ALPHA                );
	RET_IF_EQUAL( D3D11_BLEND_DEST_COLOR                );
	RET_IF_EQUAL( D3D11_BLEND_INV_DEST_COLOR                );

	RET_IF_EQUAL( D3D11_BLEND_SRC_ALPHA_SAT                );
	RET_IF_EQUAL( D3D11_BLEND_BLEND_FACTOR                );
	RET_IF_EQUAL( D3D11_BLEND_INV_BLEND_FACTOR                );
	RET_IF_EQUAL( D3D11_BLEND_SRC1_COLOR                );
	RET_IF_EQUAL( D3D11_BLEND_INV_SRC1_COLOR                );
	RET_IF_EQUAL( D3D11_BLEND_SRC1_ALPHA                );
	RET_IF_EQUAL( D3D11_BLEND_INV_SRC1_ALPHA                );


#if RX_DEBUG_RENDERER
	dxWarnf("Unknown blend option: '%s'.\n",str);
#endif

	return D3D11_BLEND_ZERO;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_BLEND_OP D3D_BlendOperationFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_BLEND_OP_ADD                );
	RET_IF_EQUAL( D3D11_BLEND_OP_SUBTRACT                );
	RET_IF_EQUAL( D3D11_BLEND_OP_REV_SUBTRACT                );
	RET_IF_EQUAL( D3D11_BLEND_OP_MIN                );
	RET_IF_EQUAL( D3D11_BLEND_OP_MAX                );
	
#if RX_DEBUG_RENDERER
	dxWarnf("Unknown blend operation: '%s'.\n",str);
#endif

	return D3D11_BLEND_OP_ADD;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_FILL_MODE D3D_FillModeFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_FILL_WIREFRAME                );
	RET_IF_EQUAL( D3D11_FILL_SOLID                );
	
#if RX_DEBUG_RENDERER
	dxWarnf("Unknown fill mode: '%s'.\n",str);
#endif

	return D3D11_FILL_SOLID;
}
//-------------------------------------------------------------------------------------------------------------//
D3D11_CULL_MODE D3D_CullModeFromString( const char* str )
{
	RET_IF_EQUAL( D3D11_CULL_NONE                );
	RET_IF_EQUAL( D3D11_CULL_FRONT               );
	RET_IF_EQUAL( D3D11_CULL_BACK                );

#if RX_DEBUG_RENDERER
	dxWarnf("Unknown cull mode: '%s'.\n",str);
#endif

	return D3D11_CULL_BACK;
}
//-------------------------------------------------------------------------------------------------------------//
D3DX11_IMAGE_FILE_FORMAT D3D_GetImageFormat( EImageFileFormat format )
{
	switch( format )
	{
	case IFF_BMP :	return D3DX11_IFF_BMP;
	case IFF_JPG :	return D3DX11_IFF_JPG;
	case IFF_PNG :	return D3DX11_IFF_PNG;
	case IFF_DDS :	return D3DX11_IFF_DDS;
	case IFF_TIFF :	return D3DX11_IFF_TIFF;
	case IFF_GIF :	return D3DX11_IFF_GIF;
	case IFF_WMP :	return D3DX11_IFF_WMP;

	case IFF_Unknown :
	default:
		Unreachable;
	}
	return D3DX11_IFF_BMP;
}


const char* D3D11_USAGE_To_Chars( const D3D11_USAGE usage )
{
	switch( usage )
	{
	case D3D11_USAGE_DEFAULT :	return "Default";
	case D3D11_USAGE_IMMUTABLE :return "Immutable";
	case D3D11_USAGE_DYNAMIC :	return "Dynamic";
	case D3D11_USAGE_STAGING :	return "Staging";
	default:
		Unreachable;
	}
	return "???";
}

const char* D3D11_SRV_DIMENSION_To_Chars( const D3D11_SRV_DIMENSION e )
{
	switch( e )
	{
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_UNKNOWN);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_BUFFER);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE1D);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE1DARRAY);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE2D);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE2DARRAY);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE2DMS);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE2DMSARRAY);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURE3D);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURECUBE);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_TEXTURECUBEARRAY);
	//mxSTATE_CASE_STRING(D3D_SRV_DIMENSION_BUFFEREX);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_UNKNOWN);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_BUFFER);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE1D);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE1DARRAY);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE2D);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE2DARRAY);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE2DMS);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURE3D);
	//mxSTATE_CASE_STRING(D3D10_SRV_DIMENSION_TEXTURECUBE);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_UNKNOWN);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_BUFFER);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE1D);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE2D);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE2DMS);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURE3D);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURECUBE);
	//mxSTATE_CASE_STRING(D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_UNKNOWN);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_BUFFER);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE1D);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE1DARRAY);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE2D);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE2DARRAY);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE2DMS);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURE3D);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURECUBE);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_TEXTURECUBEARRAY);
	mxSTATE_CASE_STRING(D3D11_SRV_DIMENSION_BUFFEREX);

	default:
		Unreachable;
	}
	return "???";
}

//-------------------------------------------------------------------------------------------------------------//
//
// Returns a string corresponding to the error code.
//
const char* D3D_GetErrorCodeString( HRESULT hErrorCode )
{
#define MYTEXT(x)	(x)
	switch ( hErrorCode )
	{
		case D3DERR_INVALIDCALL :
			return MYTEXT("The method call is invalid");

		case D3DERR_WASSTILLDRAWING :
			return MYTEXT("The previous blit operation that is transferring information to or from this surface is incomplete");

		case E_FAIL :
			return MYTEXT("Attempted to create a device with the debug layer enabled and the layer is not installed");

		case E_INVALIDARG :
			return MYTEXT("An invalid parameter was passed to the returning function");

		case E_OUTOFMEMORY :
			return MYTEXT("Direct3D could not allocate sufficient memory to complete the call");

		case S_FALSE :
			return MYTEXT("Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context");

		case S_OK :
			return MYTEXT("No error occurred"); 


			// Direct3D 11 specific error codes

		case D3D11_ERROR_FILE_NOT_FOUND :
			return MYTEXT("The file was not found");

		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS :
			return MYTEXT("There are too many unique instances of a particular type of state object");

		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS :
			return MYTEXT("There are too many unique instances of a particular type of view object");

		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD :
			return MYTEXT("Deferred context map without initial discard");


		default:
			/* fallthrough */;
	}

	return DXGetErrorDescriptionA( hErrorCode );

#undef MYTEXT

}
//-------------------------------------------------------------------------------------------------------------//
void dxDbgOut( const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	GetGlobalLogger().Log( ELogLevel::LL_Info, buffer, length );
}
//--------------------------------------------------------------//
void dxMsg( const char* message )
{
	GetGlobalLogger().Log( ELogLevel::LL_Info, message, mxStrLenAnsi(message) );
}
//--------------------------------------------------------------//
void dxMsgf( const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	GetGlobalLogger().Log( ELogLevel::LL_Info, buffer, length );
}
//--------------------------------------------------------------//
void dxWarnf( const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	GetGlobalLogger().Log( ELogLevel::LL_Warning, buffer, length );
}
//--------------------------------------------------------------//
void dxWarnf( HRESULT errorCode, const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	mxSPrintfAnsi( buffer + length, ARRAY_SIZE(buffer), "\nReason:\n %s\n", D3D_GetErrorCodeString(errorCode) );

	GetGlobalLogger().Log( ELogLevel::LL_Warning, buffer, length );
}
//--------------------------------------------------------------//
void dxErrf( const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	GetGlobalLogger().Log( ELogLevel::LL_Error, buffer, length );
}
//--------------------------------------------------------------//
void dxErrf( HRESULT errorCode, const char* message, ... )
{
	char	buffer[ MAX_STRING_CHARS ];
	UINT	length;
	MX_GET_VARARGS_ANSI_X( buffer, message, length );

	mxSPrintfAnsi( buffer + length, ARRAY_SIZE(buffer), "\nReason:\n %s\n", D3D_GetErrorCodeString(errorCode) );

	GetGlobalLogger().Log( ELogLevel::LL_Error, buffer, length );
}
//--------------------------------------------------------------//
void dxError( HRESULT errorCode )
{
	dxErrf( "Error:\n%s\n",
		D3D_GetErrorCodeString( errorCode ) );
}


/*================================
		dxVertexFormat
================================*/

dxVertexFormat::dxVertexFormat()
{
	ZERO_OUT(*this);
}

dxVertexFormat::dxVertexFormat(ENoInit)
{
}

dxVertexFormat::dxVertexFormat(EInitZero)
{
	ZERO_OUT(*this);
}

void dxVertexFormat::Clear()
{
	ZERO_OUT(*this);
}

bool dxVertexFormat::Equals( const dxVertexFormat& other ) const
{
	if( this->elements.Num() != other.elements.Num() ) {
		return false;
	}

	for( UINT iVertexElement = 0; iVertexElement < this->elements.Num(); iVertexElement++ )
	{
		const D3D11_INPUT_ELEMENT_DESC & a = this->elements[ iVertexElement ];
		const D3D11_INPUT_ELEMENT_DESC & b = other.elements[ iVertexElement ];

		//mxOPTIMIZE("reorder these to improve perf");
		if(    a.SemanticName			!=	b.SemanticName
			|| a.SemanticIndex			!=	b.SemanticIndex
			|| a.Format					!=	b.Format
			|| a.InputSlot				!=	b.InputSlot
			|| a.AlignedByteOffset		!=	b.AlignedByteOffset
			|| a.InputSlotClass			!=	b.InputSlotClass
			|| a.InstanceDataStepRate	!=	b.InstanceDataStepRate )
		{
			return false;
		}
	}

	return true;
}

UINT dxVertexFormat::CalcSize() const
{
	UINT vertexSize = 0;
	for( UINT iVertexElement = 0; iVertexElement < this->elements.Num(); iVertexElement++ )
	{
		vertexSize += DXGIFormat_GetElementSize( this->elements[ iVertexElement ].Format );
	}
	return vertexSize;
}

UINT dxVertexFormat::GetHash()const
{
	return CRC32_BlockChecksum( &this->elements, this->elements.Num() * sizeof(this->elements[0]) );
}

UINT mxGetHashCode( const dxVertexFormat& vertexFormat ) {
	return vertexFormat.GetHash();
}


StateBlock::StateBlock()
{
	stencilRef = nil;
	ZERO_OUT(blendFactorRGBA);
	sampleMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

GrShaderData::GrShaderData()
{
	uniqueId = INDEX_NONE;

	VS_EntryPoint = nil;
	HS_EntryPoint = nil;
	DS_EntryPoint = nil;
	GS_EntryPoint = nil;
	PS_EntryPoint = nil;

	name = nil;
	file = nil;

	numInstances = 0;
	instances = nil;

	getDefines = nil;

	instancesData = nil;

}

bool GrShaderData::isOk() const
{
	return 1
		&& uniqueId != INDEX_NONE
		&& name != nil
		&& file != nil
		&& numInstances > 0
		;
}

/*================================
	MessageListener
================================*/

MessageListener	MessageListener::defaultInstance;

void MessageListener::Put( const char* message )
{
	dxMsg( message );
}

GrShaderLibraryContext::GrShaderLibraryContext()
{
	backBufferWidth = 0;
	backBufferHeight = 0;
}


mxSWIPED("http://maid2.googlecode.com/svn/trunk/source/graphics/core/win32/direct3d11/debug.h");
#if 0
std::string DebugStringD3D11( BOOL b );
std::string DebugStringD3D11( D3D11_BLEND type );
std::string DebugStringD3D11( D3D11_BLEND_OP type );
std::string DebugStringD3D11( D3D11_USAGE usage );
std::string DebugStringD3D11( D3D11_BIND_FLAG flag );
std::string DebugStringD3D11( D3D11_CPU_ACCESS_FLAG flag );
std::string DebugStringD3D11( D3D11_RESOURCE_MISC_FLAG flag );
std::string DebugStringD3D11( D3D11_MAP type );
std::string DebugStringD3D11( D3D11_INPUT_CLASSIFICATION type );


std::string DebugStringD3D11( const D3D11_TEXTURE2D_DESC& desc );
std::string DebugStringD3D11( const D3D11_SUBRESOURCE_DATA* desc );

std::string DebugStringD3D11( const D3D11_BLEND_DESC& desc );
std::string DebugStringD3D11( const D3D11_BUFFER_DESC& desc );
std::string DebugStringD3D11( const D3D11_INPUT_ELEMENT_DESC& desc );






static std::string _sprintf( const char* format, ...)
{
	va_list args;
	char buf[1024];

	va_start(args,format);
	vsprintf_s(buf, format, args);

	return std::string(buf);
}





std::string DebugStringD3D11( BOOL b )
{
	std::string ret;

	if( b==TRUE ) { ret = "TRUE"; }
	else          { ret = "FALSE"; }

	ret += _sprintf( "(%0x)", b );
	return ret;
}


std::string DebugStringD3D11( D3D11_BLEND type )
{
	std::string ret;

	switch( type )
	{
	case D3D11_BLEND_ZERO: { ret = "ZERO"; }break;
	case D3D11_BLEND_ONE:  { ret = "ONE"; }break;
	case D3D11_BLEND_SRC_COLOR:       { ret = "SRC_COLOR";      }break;
	case D3D11_BLEND_INV_SRC_COLOR:   { ret = "INV_SRC_COLOR";  }break;
	case D3D11_BLEND_SRC_ALPHA:       { ret = "SRC_ALPHA";      }break;
	case D3D11_BLEND_INV_SRC_ALPHA:   { ret = "INV_SRC_ALPHA";  }break;
	case D3D11_BLEND_DEST_ALPHA:      { ret = "DEST_ALPHA";     }break;
	case D3D11_BLEND_INV_DEST_ALPHA:  { ret = "INV_DEST_ALPHA"; }break;
	case D3D11_BLEND_DEST_COLOR:      { ret = "DEST_COLOR";     }break;
	case D3D11_BLEND_INV_DEST_COLOR:  { ret = "INV_DEST_COLOR"; }break;
	case D3D11_BLEND_SRC_ALPHA_SAT:   { ret = "SRC_ALPHA_SAT";  }break;
	case D3D11_BLEND_BLEND_FACTOR:    { ret = "BLEND_FACTOR";   }break;
	case D3D11_BLEND_INV_BLEND_FACTOR:{ ret = "INV_BLEND_FACTOR"; }break;
	case D3D11_BLEND_SRC1_COLOR:      { ret = "SRC1_COLOR";     }break;
	case D3D11_BLEND_INV_SRC1_COLOR:  { ret = "INV_SRC1_COLOR"; }break;
	case D3D11_BLEND_SRC1_ALPHA:      { ret = "SRC1_ALPHA";     }break;
	case D3D11_BLEND_INV_SRC1_ALPHA:  { ret = "INV_SRC1_ALPHA"; }break;
	}

	ret += _sprintf( "(%0x)", type );
	return ret;
}

std::string DebugStringD3D11( D3D11_BLEND_OP type )
{
	std::string ret;

	switch( type )
	{
	case D3D11_BLEND_OP_ADD:          { ret = "ADD"; }break;
	case D3D11_BLEND_OP_SUBTRACT:     { ret = "SUBTRACT"; }break;
	case D3D11_BLEND_OP_REV_SUBTRACT: { ret = "REV_SUBTRACT"; }break;
	case D3D11_BLEND_OP_MIN:          { ret = "MIN"; }break;
	case D3D11_BLEND_OP_MAX:          { ret = "MAX"; }break;
	}

	ret += _sprintf( "(%0x)", type );
	return ret;
}


std::string DebugStringD3D11( D3D11_USAGE usage )
{
	std::string ret;

	switch( usage )
	{
	case D3D11_USAGE_DEFAULT:   { ret = "DEFAULT"; }break;
	case D3D11_USAGE_IMMUTABLE: { ret = "IMMUTABLE"; }break;
	case D3D11_USAGE_DYNAMIC:   { ret = "DYNAMIC"; }break;
	case D3D11_USAGE_STAGING:   { ret = "STAGING"; }break;
	}
	ret += _sprintf( "(%0x)", usage );

	return ret;
}

std::string DebugStringD3D11( D3D11_BIND_FLAG flag )
{
	std::string ret;

	if( IsFlag(flag,D3D11_BIND_VERTEX_BUFFER   ) ) { ret+="+VERTEX_BUFFER"; }
	if( IsFlag(flag,D3D11_BIND_INDEX_BUFFER    ) ) { ret+="+INDEX_BUFFER"; }
	if( IsFlag(flag,D3D11_BIND_CONSTANT_BUFFER ) ) { ret+="+CONSTANT_BUFFER"; }
	if( IsFlag(flag,D3D11_BIND_SHADER_RESOURCE ) ) { ret+="+SHADER_RESOURCE"; }
	if( IsFlag(flag,D3D11_BIND_STREAM_OUTPUT   ) ) { ret+="+STREAM_OUTPUT"; }
	if( IsFlag(flag,D3D11_BIND_RENDER_TARGET   ) ) { ret+="+RENDER_TARGET"; }
	if( IsFlag(flag,D3D11_BIND_DEPTH_STENCIL   ) ) { ret+="+DEPTH_STENCIL"; }

	ret += _sprintf( "(%0x)", flag );

	return ret;
}

std::string DebugStringD3D11( D3D11_CPU_ACCESS_FLAG flag )
{
	std::string ret;

	if( IsFlag(flag,D3D11_CPU_ACCESS_WRITE ) ) { ret+="+WRITE"; }
	if( IsFlag(flag,D3D11_CPU_ACCESS_READ  ) ) { ret+="+READ";  }

	ret += _sprintf( "(%0x)", flag );

	return ret;
}

std::string DebugStringD3D11( D3D11_RESOURCE_MISC_FLAG flag )
{
	std::string ret;

	if( IsFlag(flag,D3D11_RESOURCE_MISC_GENERATE_MIPS     ) ) { ret+="+GENERATE_MIPS"; }
	if( IsFlag(flag,D3D11_RESOURCE_MISC_SHARED            ) ) { ret+="+SHARED";  }
	if( IsFlag(flag,D3D11_RESOURCE_MISC_TEXTURECUBE       ) ) { ret+="+TEXTURECUBE";  }
	//  if( IsFlag(flag,D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX ) ) { ret+="+SHARED_KEYEDMUTEX";  }
	//  if( IsFlag(flag,D3D11_RESOURCE_MISC_GDI_COMPATIBLE    ) ) { ret+="+GDI_COMPATIBLE";  }

	ret += _sprintf( "(%0x)", flag );

	return ret;
}

std::string DebugStringD3D11( D3D11_MAP type )
{
	std::string ret;

	switch( type )
	{
	case D3D11_MAP_READ:          { ret = "READ"; }break;
	case D3D11_MAP_WRITE:         { ret = "WRITE"; }break;
	case D3D11_MAP_READ_WRITE:    { ret = "READ_WRITE"; }break;
	case D3D11_MAP_WRITE_DISCARD: { ret = "WRITE_DISCARD"; }break;
	case D3D11_MAP_WRITE_NO_OVERWRITE:{ ret = "WRITE_NO_OVERWRITE"; }break;
	}
	ret += _sprintf( "(%0x)", type );

	return ret;
}

std::string DebugStringD3D11( D3D11_INPUT_CLASSIFICATION type )
{
	std::string ret;

	switch( type )
	{
	case D3D11_INPUT_PER_VERTEX_DATA :          { ret = "PER_VERTEX"; }break;
	case D3D11_INPUT_PER_INSTANCE_DATA:         { ret = "PER_INSTANCE"; }break;
	}
	ret += _sprintf( "(%0x)", type );

	return ret;
}



std::string DebugStringD3D11( const D3D11_TEXTURE2D_DESC& desc )
{
	std::string ret;

	ret += _sprintf( " Width:%0d", desc.Width );
	ret += _sprintf( " Height:%0d", desc.Height );
	ret += _sprintf( " MipLevels:%0d", desc.MipLevels );
	ret += _sprintf( " ArraySize:%0d", desc.ArraySize );
	ret += _sprintf( " Format:%s", DebugStringDXGI(desc.Format).c_str() );
	ret += _sprintf( " SampleCount:%0d", desc.SampleDesc.Count );
	ret += _sprintf( " SampleQuality:%0d", desc.SampleDesc.Quality );
	ret += _sprintf( " Usage:%s", DebugStringD3D11(desc.Usage).c_str() );
	ret += _sprintf( " BindFlags:%s", DebugStringD3D11((D3D11_BIND_FLAG)desc.BindFlags).c_str() );
	ret += _sprintf( " CPUAccessFlags:%s", DebugStringD3D11((D3D11_CPU_ACCESS_FLAG)desc.CPUAccessFlags).c_str() );
	ret += _sprintf( " MiscFlags:%s", DebugStringD3D11((D3D11_RESOURCE_MISC_FLAG)desc.MiscFlags).c_str() );

	return ret;
}

std::string DebugStringD3D11( const D3D11_SUBRESOURCE_DATA* desc )
{
	if( desc==NULL ) { return "NULL"; }
	std::string ret;

	ret += _sprintf( " pSysMem:%0x", desc->pSysMem );
	ret += _sprintf( " SysMemPitch:%0d", desc->SysMemPitch );
	ret += _sprintf( " SysMemSlicePitch:%0d", desc->SysMemSlicePitch );

	return ret;
}




std::string DebugStringD3D11( const D3D11_BLEND_DESC& desc )
{
	std::string ret;

	ret += _sprintf( " AlphaToCoverageEnable:%s", DebugStringD3D11(desc.AlphaToCoverageEnable).c_str() );

	for( int i=0; i<NUMELEMENTS(desc.RenderTarget); ++i )
	{
		const D3D11_RENDER_TARGET_BLEND_DESC& rt = desc.RenderTarget[i];

		ret += _sprintf( " rt[%0d].BlendEnable:%s",   i, DebugStringD3D11(rt.BlendEnable).c_str() );
		ret += _sprintf( " rt[%0d].SrcBlend:%s",      i, DebugStringD3D11(rt.SrcBlend).c_str() );
		ret += _sprintf( " rt[%0d].DestBlend:%s",     i, DebugStringD3D11(rt.DestBlend).c_str() );
		ret += _sprintf( " rt[%0d].BlendOp:%s",       i, DebugStringD3D11(rt.BlendOp).c_str() );
		ret += _sprintf( " rt[%0d].SrcBlendAlpha:%s", i, DebugStringD3D11(rt.SrcBlendAlpha).c_str() );
		ret += _sprintf( " rt[%0d].DestBlendAlpha:%s",i, DebugStringD3D11(rt.DestBlendAlpha).c_str() );
		ret += _sprintf( " rt[%0d].BlendOpAlpha:%s",  i, DebugStringD3D11(rt.BlendOpAlpha).c_str() );
		ret += _sprintf( " rt[%0d].RenderTargetWriteMask:%x", i, rt.RenderTargetWriteMask );
	}


	return ret;
}


std::string DebugStringD3D11( const D3D11_BUFFER_DESC& desc )
{
	std::string ret;

	ret += _sprintf( " ByteWidth:%0d", desc.ByteWidth );
	ret += _sprintf( " Usage:%s", DebugStringD3D11(desc.Usage).c_str() );
	ret += _sprintf( " BindFlags:%s", DebugStringD3D11((D3D11_BIND_FLAG)desc.BindFlags).c_str() );
	ret += _sprintf( " CPUAccessFlags:%s", DebugStringD3D11((D3D11_CPU_ACCESS_FLAG)desc.CPUAccessFlags).c_str() );
	ret += _sprintf( " MiscFlags:%s", DebugStringD3D11((D3D11_RESOURCE_MISC_FLAG)desc.MiscFlags).c_str() );

	return ret;
}


std::string DebugStringD3D11( const D3D11_INPUT_ELEMENT_DESC& desc )
{
	std::string ret;
	ret += _sprintf( " SemanticName:%s", desc.SemanticName );
	ret += _sprintf( " SemanticIndex:%0d", desc.SemanticIndex );
	ret += _sprintf( " Format:%s", DebugStringDXGI(desc.Format).c_str() );
	ret += _sprintf( " InputSlot:%0d", desc.InputSlot );
	ret += _sprintf( " AlignedByteOffset:%0d", desc.AlignedByteOffset );
	ret += _sprintf( " InputSlotClass:%s", DebugStringD3D11(desc.InputSlotClass).c_str() );

	ret += _sprintf( " InstanceDataStepRate:%0d", desc.InstanceDataStepRate );
	return ret;
}

#endif

mxNAMESPACE_END

//-------------------------------------------------------------------------------------------------------------//
//				End Of File.									//
//-------------------------------------------------------------------------------------------------------------//
