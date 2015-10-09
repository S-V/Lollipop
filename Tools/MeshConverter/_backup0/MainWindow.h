#ifndef GRAPHICSDEBUGGER_H
#define GRAPHICSDEBUGGER_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

#include "MeshConverter.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

public slots:
	void SelectInputFiles();
	void SelectOutputFolder();
	void Exec_ConvertMeshes();

private:
	void LoadDefaults();
	void LoadSettings();
	void SaveSettings();
	void ReadSettingsFromGUI( MeshConverter::Settings & settings );
	void UpdateGuiFromSettings( const MeshConverter::Settings & settings );

private:
	Ui::MainWindowClass ui;

	SetupCoreUtil	setupCore;

	MeshConverter::Settings	m_settings;

	OSPathName		m_lastInputDir;

	MeshConverter	m_meshConverter;
};

#endif // GRAPHICSDEBUGGER_H
