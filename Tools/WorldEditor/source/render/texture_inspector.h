#pragma once

// who will debug the debugger?
#define DEBUG_THE_DEBUGGER	(0)

/*
-----------------------------------------------------------------------------
	EdTexturePreview
-----------------------------------------------------------------------------
*/
struct EdTexturePreview : public ATexturePreview
{
	StackString			m_descStr;
	ID3D11Texture2DPtr	m_pTexture;
	ID3D11ShaderResourceViewPtr m_pSRV;
	D3D11_TEXTURE2D_DESC	m_texDesc;

public:
	mxDECLARE_CLASS( EdTexturePreview, ATexturePreview );

	EdTexturePreview( ID3D11Texture2D* theTexture, PCSTR name );
	~EdTexturePreview();

	bool operator == (const EdTexturePreview& other) const
	{
		return m_pTexture == other.m_pTexture;
	}
	bool operator != (const EdTexturePreview& other) const
	{
		return m_pTexture != other.m_pTexture;
	}
	virtual void GetSize( UINT &OutWidth, UINT &OutHeight ) override
	{
		OutWidth = m_texDesc.Width;
		OutHeight = m_texDesc.Height;
	}
	virtual PCSTR GetName() const override
	{
		return m_descStr.ToChars();
	}
	virtual ID3D11ShaderResourceView* GetSRV() override
	{
		return m_pSRV;
	}
	const char* edToChars( UINT column ) const override
	{
		return m_descStr.ToChars();
	}
	virtual AEditable* edGetParent() override;
};

/*
-----------------------------------------------------------------------------
	EdTextureInspector
-----------------------------------------------------------------------------
*/
class EdTextureInspector : public ATextureInspector
{
public:
	mxDECLARE_CLASS(EdTextureInspector, ATextureInspector);

	EdTextureInspector();
	~EdTextureInspector();

	//=-- ATextureInspector

	virtual void AddDebugItem( ID3D11Texture2D* pTexture, PCSTR name ) override;
	virtual void RemoveDebugItem( ID3D11Texture2D* pTexture ) override;
	virtual ATexturePreview* GetTextureByName( PCSTR name ) override;

	//=-- AEditable

	virtual AEditable* edGetParent() override;
	virtual UINT edNumRows() const;
	virtual const char* edToChars( UINT column ) const override;

	void DbgDump();
};

