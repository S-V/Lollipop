#pragma once

#include "editor_common.h"

#include "editor_system.h"



/*
-----------------------------------------------------------------------------
	EdWidget_ObjectInspector
-----------------------------------------------------------------------------
*/
class EdWidget_ObjectInspector
	: public QTreeView
{
	Q_OBJECT

public:
	typedef QTreeView Super;

	EdWidget_ObjectInspector( QWidget* parent = nil );
	~EdWidget_ObjectInspector();

	void SetData( void* pObject, const mxType& typeInfo );
};

/*
-----------------------------------------------------------------------------
	EdObjectInspector
-----------------------------------------------------------------------------
*/
class EdObjectInspector
	: public QDockWidget
	, public EdModule
	, public virtual EdWidgetChild
	, SingleInstance< EdObjectInspector >
{
	Q_OBJECT

public:
	typedef QDockWidget Super;

	EdObjectInspector();
	~EdObjectInspector();

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

	EdAction	m_actViewObjectInspector;
};
