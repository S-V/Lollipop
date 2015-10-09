


void SetupRenderTargets()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZERO_OUT( texDesc );
	texDesc.Width				= desc.width;
	texDesc.Height				= desc.height;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZERO_OUT( rtvDesc );
	rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice	= 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZERO_OUT( srvDesc );
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels			= 1;
	srvDesc.Texture2D.MostDetailedMip	= 0;

	texDesc.Format = desc.format;
	rtvDesc.Format = desc.format;
	srvDesc.Format = desc.format;

	rxRenderTarget & newRT = mRenderTargetsByName.Set( name, rxRenderTarget() );
	newRT.Setup( &texDesc, &rtvDesc, &srvDesc );
	newRT.defaultClearColor = desc.defaultClearColor;

}
