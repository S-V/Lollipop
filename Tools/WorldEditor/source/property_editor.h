/*
=============================================================================
	File:	property_editor.h
	Desc:	
=============================================================================
*/
#pragma once

#include "property_grid.h"

#include "editor_system.h"

/*
-----------------------------------------------------------------------------
	PropertyEditorPlugin
-----------------------------------------------------------------------------
*/
class PropertyEditorPlugin
	: public ProperyEditorDockWidget
	, public EdModule
	, SingleInstance< PropertyEditorPlugin >
{
	Q_OBJECT

public:
	PropertyEditorPlugin();
	~PropertyEditorPlugin();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;

	virtual void Shutdown() override;

	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

protected:
	virtual void OnProjectUnloaded() override;

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

private:
	EdAction	m_actViewProperties;
};
