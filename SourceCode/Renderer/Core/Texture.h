/*
=============================================================================
	File:	Texture.h
	Desc:	
=============================================================================
*/
#pragma once

//#include <Core/Memory.h>
//#include <Core/Resources.h>

/*
=======================================================================

	Texture management

=======================================================================
*/

/*
-----------------------------------------------------------------------------
	rxTexture

	represents a device texture that is used at run-time for actual rendering
-----------------------------------------------------------------------------
*/
struct rxTexture
	: SResourceObject
{
	static inline EAssetType StaticGetResourceType()
	{ return EAssetType::Asset_Texture2D; }

	typedef TResPtr< rxTexture >	Ref;

public:
	// NOTE: don't cache pointers to them - they may change when streaming/loading
	ID3D11ResourcePtr			pTexture;
	ID3D11ShaderResourceViewPtr	pSRV;

public:
	rxTexture();
	~rxTexture();
};

/*
-----------------------------------------------------------------------------
	TextureManager
-----------------------------------------------------------------------------
*/
class rxTextureManager
	: public AResourceManager
	, public TGlobal<rxTextureManager>
{
public:

	rxTexture& GetFallbackTexture();

	//=-- AResourceManager
	virtual SResourceObject* LoadResource( SResourceLoadArgs & loadArgs ) override;
	virtual SResourceObject* GetDefaultResource() override;

public_internal:
	rxTextureManager();
	~rxTextureManager();

private:
	TList< rxTexture* >	m_allTextures;

public:
	rxTexture		m_defaultTexture;	// fallback texture
};

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
