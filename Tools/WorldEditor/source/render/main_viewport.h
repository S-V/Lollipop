#pragma once

#include <Base/Util/FPSTracker.h>

#include "render/viewports.h"

/*
-----------------------------------------------------------------------------
	EdMainViewport
-----------------------------------------------------------------------------
*/
class EdMainViewport : public EdSceneViewport
{
	mxDECLARE_CLASS( EdMainViewport, EdSceneViewport );

public:
	EdMainViewport();
	~EdMainViewport();

	virtual void InitViewport( EdViewportWidget* widget ) override;
	virtual void CloseViewport() override;

	virtual void OnProjectLoaded() override;
	virtual void OnProjectUnloaded() override;
	virtual void SerializeProjectData( ATextSerializer & archive ) override;

	//=-- EdSystemChild
	virtual void OnSelectionChanged( AEditable* theObject ) override;
	virtual void OnObjectDestroyed( AEditable* theObject ) override;

public:	// EdViewport interface
	virtual bool AcceptsDrop( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) override;
	virtual void OnDropEvent( const QMimeData* mimeData, const UINT mouseX, const UINT mouseY ) override;

public:	// AClientViewport interface

	virtual void TickViewport( FLOAT deltaSeconds ) override;
	virtual void Draw() override;

	virtual void OnResize( UINT newWidth, UINT newHeight, bool bFullScreen ) override;

	virtual void OnClose() override;

	//-- AInputClient interface

	virtual void OnKeyPressed( EKeyCode key ) override;
	virtual void OnKeyReleased( EKeyCode key ) override;

public:	// Editor
	virtual const char* edToChars( UINT column ) const override;
	virtual void edCreateProperties( EdPropertyList *properties, bool bClear = true ) override;

private:
	void OnEngineLoaded();

private:
	TIndexPtr< World_Editor >	m_worldEditor;

	FPSTracker<>	m_fpsCounter;
};

/*
-----------------------------------------------------------------------------
	EdMainViewportWidget
-----------------------------------------------------------------------------
*/
class EdMainViewportWidget : public EdViewportWidget
{
public:
	Q_OBJECT

public:
	typedef EdViewportWidget Super;

	EdMainViewportWidget( QWidget* parent = nil );
	~EdMainViewportWidget();

protected:
	virtual void contextMenuEvent( QContextMenuEvent* theEvent ) override;

private slots:
	void slot_Edit_Properties();
};

