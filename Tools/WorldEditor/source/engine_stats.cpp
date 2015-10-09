#include "stdafx.h"

#include <Renderer/Core/Material.h>

#include "main_window_frame.h"
#include "editor_app.h"

#include "engine_stats.h"

/*
-----------------------------------------------------------------------------
	EdStatsView
-----------------------------------------------------------------------------
*/
EdStatsView::EdStatsView()
	: QDockWidget(EditorApp::GetMainFrame())
{
	this->setWindowTitle("Engine Stats");
	this->setObjectName("Engine Stats");
	this->setAttribute( Qt::WidgetAttribute::WA_DeleteOnClose, false );



	m_showEngineStatsAct.setText(tr("Engine Stats"));


	EdMenus& menus = EdApp::Get().menus;
	menus.viewMenu->addAction( &m_showEngineStatsAct );
	EditorApp::ShowOnActionTriggered( &m_showEngineStatsAct, this );

	{
		m_listWidget.addItem(&m_label_numBatches);
		m_listWidget.addItem(&m_label_numEntities);
		m_listWidget.addItem(&m_label_frameCounts);
		m_listWidget.addItem(&m_label_viewCount);
		m_listWidget.addItem(&m_label_numMaterialChanges);

		m_listWidget.addItem(&m_label_millisecondsPerFrame);

		m_listWidget.addItem(&m_label_totalNumGfxMaterials);
	}
	this->setWidget(&m_listWidget);
	//{
	//	QVBoxLayout* pLayout = new QVBoxLayout(this);

	//	pLayout->addWidget(&m_labelNumGfxMaterials);

	//	pLayout->addWidget(&m_label_numBatches);
	//	pLayout->addWidget(&m_label_numEntities);
	//	pLayout->addWidget(&m_label_frameCounts);
	//	pLayout->addWidget(&m_label_viewCount);
	//	pLayout->addWidget(&m_label_numMaterialChanges);

	//	this->setLayout(pLayout);
	//}

	mxCONNECT_THIS( EdSystem::Get().Event_UpdateState, EdStatsView, OnUpdate );
}

EdStatsView::~EdStatsView()
{
}

void EdStatsView::SetDefaultValues()
{
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	mainWindow->addDockWidget( Qt::BottomDockWidgetArea, this );

	this->hide();
}

void EdStatsView::SerializeWidgetLayout( QDataStream & stream )
{
	//DBG_TRACE_CALL;
	QMainWindow* mainWindow = EditorApp::GetMainFrame();

	SerializeDockWidgetState( stream, mainWindow, Qt::BottomDockWidgetArea, this );
}

void EdStatsView::OnUpdate( const mxDeltaTime& )
{
	m_label_numBatches.setText(QString("numBatches %1").arg(gfxStats.numBatches));
	m_label_numEntities.setText(QString("numEntities %1").arg(gfxStats.numEntities));
	m_label_frameCounts.setText(QString("frameCount %1").arg(gfxStats.frameCount));
	m_label_viewCount.setText(QString("viewCount %1").arg(gfxStats.viewCount));
	m_label_numMaterialChanges.setText(QString("numMaterialChanges %1").arg(gfxStats.numMaterialChanges));


	DBG_DO_INTERVAL( m_label_millisecondsPerFrame.setText(QString("msec %1").arg(gfxBEStats.lastFrameRenderTime)), 1000 );


	m_label_totalNumGfxMaterials.setText(QString("Graphics Materials %1").arg(rxMaterial::TotalCount()));
}
