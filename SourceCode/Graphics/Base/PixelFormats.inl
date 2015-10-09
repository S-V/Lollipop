/*
=============================================================================
	File:	TextureFormats.inl
	Desc:	Descriptions of various texture formats.
=============================================================================
*/

//------------------------------------------------
// DECLARE_PIXEL_FORMAT( e, name, desc, fourCC )
//------------------------------------------------
// 1) Enum 
// 2) Name
// 3) ShortDescription
// 4) FourCC
//------------------------------------------------

/*
==============================
	Uncompressed formats
==============================
*/
DECLARE_PIXEL_FORMAT(
	PF_A8R8G8B8,
	"A8R8G8B8",
	"32-bit ARGB pixel format with alpha, using 8 bits per channel",
	'ARGB'
)

/*
==============================
	Compressed formats
==============================
*/

/*
	DXT1 should be used for images without alpha (or 1-bit alpha)
*/
DECLARE_PIXEL_FORMAT(
	PF_DXT1,
	"DXT1",
	"DDS (DirectDraw Surface) DXT1 compression texture format",
	'DXT1'
)
DECLARE_PIXEL_FORMAT(
	PF_DXT3,
	"DXT3",
	"DDS (DirectDraw Surface) DXT3 compression texture format",
	'DXT3'
)
/*
	DXT1 should be used for images with alpha (8-bit alpha)
*/
DECLARE_PIXEL_FORMAT(
	PF_DXT5,
	"DXT5",
	"DDS (DirectDraw Surface) DXT5 compression texture format",
	'DXT5'
)

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
