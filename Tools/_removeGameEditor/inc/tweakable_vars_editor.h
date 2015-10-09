#pragma once

#include "property_grid.h"

#include "editor_system.h"

class TweakableVarsEditor
	: public ProperyEditorDockWidget
	, public EdModule
	, SingleInstance< TweakableVarsEditor >
{
	Q_OBJECT

public:
	TweakableVarsEditor();
	~TweakableVarsEditor();

	virtual void SetDefaultValues() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

protected:
	virtual void OnProjectUnloaded() override;

private:
	void OnObjectModified( AEditable* theObject );

public:
	EdAction	m_viewTweakableVarsBar;
};
