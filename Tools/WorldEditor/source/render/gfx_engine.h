#pragma once

#include "editor_system.h"

class HitTesting;

/*
-----------------------------------------------------------------------------
	EdRenderer
-----------------------------------------------------------------------------
*/
class EdRenderer
	: public EdModule
	, public EdWidgetChild
	, public EdGraphics
	, public TGlobal<EdRenderer>//, SingleInstance<EdRenderer>
{
public:
	EdRenderer();
	~EdRenderer();

	//=-- EdModule
	virtual void PreInit() override;
	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;

	//=-- EdAppChild
	virtual void SerializeAddData( ATextSerializer & serializer ) override;

	//=-- EdWidgetChild
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

	//=-- EdGraphics

	virtual void InitHitTesting( UINT viewportWidth, UINT viewportHeight ) override;
	virtual void CloseHitTesting() override;

	virtual ATextureInspector* GetTextureInspector() override;


	HitTesting& GetHitTesting();

protected:
	virtual void OnProjectUnloaded() override;

private:
	void UpdateViewports( const mxDeltaTime& deltaTime );
	void RenderViewports();
};
