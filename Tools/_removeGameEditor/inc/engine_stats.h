/*
=============================================================================
	File:	console.h
	Desc:	Log, engine output.
=============================================================================
*/
#pragma once

#include "editor_system.h"

/*
-----------------------------------------------------------------------------
	EdStatsView
-----------------------------------------------------------------------------
*/
class EdStatsView
	: public QDockWidget
	, public EdModule
	, public EdWidgetChild
	, SingleInstance< EdStatsView >
{
	Q_OBJECT

public:
	EdStatsView();
	~EdStatsView();

	//--EdModule
	virtual void SetDefaultValues() override;

	// serialize app-specific settings
	virtual void SerializeWidgetLayout( QDataStream & stream ) override;

private:
	void OnUpdate( const mxDeltaTime& );

private:
	EdAction		m_showEngineStatsAct;

	QListWidget		m_listWidget;

	QListWidgetItem	m_label_numBatches;
	QListWidgetItem	m_label_numEntities;
	QListWidgetItem	m_label_frameCounts;
	QListWidgetItem	m_label_viewCount;
	QListWidgetItem	m_label_numMaterialChanges;

	//QListWidgetItem	m_label_framesPerSecond;
	QListWidgetItem	m_label_millisecondsPerFrame;

	QListWidgetItem	m_label_totalNumGfxMaterials;
};
