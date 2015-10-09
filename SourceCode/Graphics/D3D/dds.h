//--------------------------------------------------------------------------------------
// dds.h
//
// This header defines constants and structures that are useful when parsing 
// DDS files.  DDS files were originally designed to use several structures
// and constants that are native to DirectDraw and are defined in ddraw.h,
// such as DDSURFACEDESC2 and DDSCAPS2.  This file defines similar 
// (compatible) constants and structures so that one can use DDS files 
// without needing to include ddraw.h.
//--------------------------------------------------------------------------------------

#ifndef _DDS_H_
#define _DDS_H_

#include <dxgiformat.h>
#include <d3d10.h>

#pragma pack(push,1)

#define DDS_MAGIC 0x20534444 // "DDS "

enum { DDS_MAGIC_SIZE_IN_BYTES = 4 };	// sizeof( DWORD )

struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

const DDS_PIXELFORMAT DDSPF_DXT1 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT2 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT3 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT4 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT5 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_A8R8G8B8 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };

const DDS_PIXELFORMAT DDSPF_A1R5G5B5 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 };

const DDS_PIXELFORMAT DDSPF_A4R4G4B4 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 };

const DDS_PIXELFORMAT DDSPF_R8G8B8 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 };

const DDS_PIXELFORMAT DDSPF_R5G6B5 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 };

// This indicates the DDS_HEADER_DXT10 extension is present (the format is in dxgiFormat)
const DDS_PIXELFORMAT DDSPF_DX10 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','1','0'), 0, 0, 0, 0, 0 };

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                               DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                               DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

typedef struct
{
    DWORD dwSize;
    DWORD dwHeaderFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD dwSurfaceFlags;
    DWORD dwCubemapFlags;
    DWORD dwReserved2[3];
} DDS_HEADER;

// DDS header extension to handle resource arrays.
typedef struct
{
    DXGI_FORMAT dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    UINT miscFlag;
    UINT arraySize;
    UINT reserved;
} DDS_HEADER_DXT10;

#pragma pack(pop)



//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------

#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )


//--------------------------------------------------------------------------------------
inline
DXGI_FORMAT ConvertToDXGI_FORMAT( D3DFORMAT d3dformat )
{
	switch( d3dformat )
	{
	case D3DFMT_A32B32G32R32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case D3DFMT_A16B16G16R16:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_A16B16G16R16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case D3DFMT_G32R32F:
		return DXGI_FORMAT_R32G32_FLOAT;

	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case D3DFMT_G16R16:
	case D3DFMT_V16U16:
		return DXGI_FORMAT_R16G16_UNORM;

	case D3DFMT_G16R16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case D3DFMT_R32F:
		return DXGI_FORMAT_R32_FLOAT;

	case D3DFMT_R16F:
		return DXGI_FORMAT_R16_FLOAT;

	case D3DFMT_A8:
		return DXGI_FORMAT_A8_UNORM;
	case D3DFMT_P8:
	case D3DFMT_L8:
		return DXGI_FORMAT_R8_UNORM;

	case D3DFMT_DXT1:
		return DXGI_FORMAT_BC1_UNORM;
	case D3DFMT_DXT2:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case D3DFMT_DXT3:
		return DXGI_FORMAT_BC2_UNORM;
	case D3DFMT_DXT4:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case D3DFMT_DXT5:
		return DXGI_FORMAT_BC3_UNORM;

	default:
		assert( FALSE ); // unhandled format
		return DXGI_FORMAT_UNKNOWN;
	}
}

extern UINT DXGIFormat_BitsPerPixel( D3DFORMAT fmt );

//--------------------------------------------------------------------------------------
inline
D3DFORMAT ConvertToD3DFORMAT( DXGI_FORMAT dxgiformat )
{
	switch( dxgiformat )
	{

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return D3DFMT_A32B32G32R32F;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return D3DFMT_A16B16G16R16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return D3DFMT_A16B16G16R16F;
	case DXGI_FORMAT_R32G32_FLOAT:
		return D3DFMT_G32R32F;

	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return D3DFMT_A8R8G8B8;

	case DXGI_FORMAT_R16G16_UNORM:
		return D3DFMT_G16R16;

	case DXGI_FORMAT_R16G16_FLOAT:
		return D3DFMT_G16R16F;
	case DXGI_FORMAT_R32_FLOAT:
		return D3DFMT_R32F;

	case DXGI_FORMAT_R16_FLOAT:
		return D3DFMT_R16F;

	case DXGI_FORMAT_A8_UNORM:
		return D3DFMT_A8;
	case DXGI_FORMAT_R8_UNORM:
		return D3DFMT_L8;

	case DXGI_FORMAT_BC1_UNORM:
		return D3DFMT_DXT1;
	case DXGI_FORMAT_BC2_UNORM:
		return D3DFMT_DXT3;
	case DXGI_FORMAT_BC3_UNORM:
		return D3DFMT_DXT5;

	default:
		assert( FALSE ); // unhandled format
		return D3DFMT_UNKNOWN;
	}
}


//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
inline
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	// From the DXSDK docs:
	//
	//     When computing DXTn compressed sizes for non-square textures, the 
	//     following formula should be used at each mipmap level:
	//
	//         max(1, width ÷ 4) x max(1, height ÷ 4) x 8(DXT1) or 16(DXT2-5)
	//
	//     The pitch for DXTn formats is different from what was returned in 
	//     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks. 
	//     For example, if you have a width of 16, then you will have a pitch 
	//     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

	if( fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5 )
	{
		// Note: we support width and/or height being 0 in order to compute
		// offsets in functions like CBufferLockEntry::CopyBLEToPerfectSizedBuffer().
		int numBlocksWide = 0;
		if( width > 0 )
			numBlocksWide = largest( 1, width / 4 );
		int numBlocksHigh = 0;
		if( height > 0 )
			numBlocksHigh = largest( 1, height / 4 );
		//int numBlocks = numBlocksWide * numBlocksHigh;
		int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
		rowBytes = numBlocksWide * numBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = DXGIFormat_BitsPerPixel( fmt );
		rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if( pNumBytes != NULL )
		*pNumBytes = numBytes;
	if( pRowBytes != NULL )
		*pRowBytes = rowBytes;
	if( pNumRows != NULL )
		*pNumRows = numRows;
}

//--------------------------------------------------------------------------------------
inline
D3DFORMAT GetD3D9Format( DDS_PIXELFORMAT ddpf )
{
	// See DDSTextureLoader for a more complete example of this...

	if( ddpf.dwFlags & DDS_RGB )	//rgb codes
		// Only do the more common formats
	{
		if( 32 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
				return D3DFMT_A8R8G8B8;
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_X8R8G8B8;
			if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0xff000000) )
				return D3DFMT_A8B8G8R8;
			if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0x00000000) )
				return D3DFMT_X8B8G8R8;
			if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
				return D3DFMT_R32F;
		}

		if( 24 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_R8G8B8;
		}

		if( 16 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x0000F800,0x000007E0,0x0000001F,0x00000000) )
				return D3DFMT_R5G6B5;
		}
	}
	else if( ddpf.dwFlags & DDS_LUMINANCE )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			return D3DFMT_L8;
		}
	}
	else if( ddpf.dwFlags & DDS_ALPHA )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			return D3DFMT_A8;
		}
	}
	else if( ddpf.dwFlags & DDS_FOURCC )  //fourcc codes (dxtn)
	{
		if( MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.dwFourCC )
			return D3DFMT_DXT1;
		if( MAKEFOURCC( 'D', 'X', 'T', '2' ) == ddpf.dwFourCC )
			return D3DFMT_DXT2;
		if( MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.dwFourCC )
			return D3DFMT_DXT3;
		if( MAKEFOURCC( 'D', 'X', 'T', '4' ) == ddpf.dwFourCC )
			return D3DFMT_DXT4;
		if( MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.dwFourCC )
			return D3DFMT_DXT5;
	}

	return D3DFMT_UNKNOWN;
}



#endif // _DDS_H
