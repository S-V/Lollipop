/*
=============================================================================
	File:	property_editor_module2.h
	Desc:	
=============================================================================
*/
#pragma once

#include <QtSupport/property_editor.h>

#include "editor_common.h"
#include "editor_system.h"

/*
-----------------------------------------------------------------------------
	PropertyEditorPlugin2
-----------------------------------------------------------------------------
*/
class PropertyEditorPlugin2
	: public QtSupport::Property_Editor_Dock_Widget
	, public EdModule
	, public virtual EdWidgetChild
	, SingleInstance< PropertyEditorPlugin2 >
{
	Q_OBJECT

public:
	PropertyEditorPlugin2();
	~PropertyEditorPlugin2();

	virtual void PreInit() override;
	virtual void SetDefaultValues() override;

	virtual void Shutdown() override;

	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

protected:

	//=-- EdSystemChild
	virtual void OnSelectionChanged( AEditable* theObject ) override;
	virtual void OnObjectDestroyed( AEditable* theObject ) override;

	//=-- EdProjectChild
	virtual void OnProjectUnloaded() override;

private:
	void CreateWidgets();
	void CreateActions();
	void CreateMenus();
	void ConnectSigSlots();

private:
//	QtSupport::Property_Editor_Dock_Widget		m_propsEditorDockWidget;

	EdAction	m_actViewProperties;
};
