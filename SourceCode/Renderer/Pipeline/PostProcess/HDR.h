#pragma once



/*
--------------------------------------------------------------
	HDR_System
--------------------------------------------------------------
*/
struct HDR_System
{
	RenderTarget	initialLuminance;	// HDR scene color -> gray-scale luminance
	RenderTarget	adaptedLuminance[2];// hold gray-scale values
	UINT			iCurrLuminanceRT;	// index of current RT with average luminance

public:
	HDR_System();
	~HDR_System();

	void Initialize( UINT viewportWidth, UINT viewportHeight );
	void Shutdown();

	void ApplyToneMapping( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT, const rxViewport& viewport );

public:

	// Measure the average log luminance in the scene.
	ID3D11ShaderResourceView* MeasureAverageLuminance( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT );

	// extract and blur bright parts of the scene
	ID3D11ShaderResourceView* GenerateBloom( D3DDeviceContext* pD3DContext, const RenderTarget& hdrSceneRT, const rxViewport& viewport );

private:
	void Callback_BeforeMainViewportResized( UINT newWidth, UINT newHeight );
	void Callback_AfterMainViewportResized( UINT newWidth, UINT newHeight );

private:
	void CreateRenderTargets( UINT viewportWidth, UINT viewportHeight );
	void ReleaseRenderTargets();
};

