#include "stdafx.h"
#include <../QtSupport/common.h>
#include "mainwindow.h"

const char* SETTINGS_FILE_NAME = "MeshConverter.settings";
const char* LAYOUT_FILE_NAME = "MeshConverter.layout";

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	connect(
		ui.pushButton_Select_Input_Files,
		SIGNAL(clicked()),
		this,
		SLOT(SelectInputFiles())
	);

	connect(
		ui.pushButton_Select_Output_Dir,
		SIGNAL(clicked()),
		this,
		SLOT(SelectOutputFolder())
	);

	connect(
		ui.pushButton_Execute,
		SIGNAL(clicked()),
		this,
		SLOT(Exec_ConvertMeshes())
	);

	bool bOk = m_meshConverter.Setup();
	Assert(bOk);

	this->LoadSettings();
}

MainWindow::~MainWindow()
{
	m_meshConverter.Close();

	this->SaveSettings();
}

void MainWindow::SelectInputFiles()
{
	QFileDialog dialog(this);

	if(SelectFiles(dialog,"Mesh files (*.3ds *.x *.mesh)",
		m_lastInputDir,m_settings.inputFiles))
	{
		ui.listWidget_InputFiles->clear();
		ui.listWidget_InputFiles->addItems(dialog.selectedFiles());
	}
}

void MainWindow::SelectOutputFolder()
{
	QFileDialog dialog(this);
	if( SelectFolder(dialog, m_settings.outputDir) )
	{
		ui.lineEdit_Output_Folder->setText(m_settings.outputDir.ToChars());
	}
}

void MainWindow::Exec_ConvertMeshes()
{
	this->ReadSettingsFromGUI(m_settings);

	if(m_settings.inputFiles.IsEmpty())
	{
		QMessageBox::warning(this,"Error",
			"Select one or more input mesh files");
		return;
	}

	m_meshConverter.Convert( m_settings );
}

void MainWindow::LoadDefaults()
{
	to_str( m_lastInputDir, QApplication::applicationDirPath() );
}

void MainWindow::LoadSettings()
{
	FileReader	file(SETTINGS_FILE_NAME,FileRead_NoErrors);
	if( file.IsOpen() )
	{
		DataLoader	loader(file);
		loader & m_settings & m_lastInputDir;
	}
	else
	{
		this->LoadDefaults();
	}

	QFile	layoutFile(LAYOUT_FILE_NAME);
	layoutFile.open(QIODevice::ReadOnly);
	if( layoutFile.isOpen() )
	{
		QByteArray	geometryData(layoutFile.readAll());
		this->restoreGeometry( geometryData );
	}

	this->UpdateGuiFromSettings(m_settings);
}

void MainWindow::SaveSettings()
{
	this->ReadSettingsFromGUI( m_settings );

	FileWriter	file(SETTINGS_FILE_NAME,FileWrite_NoErrors);
	if( file.IsOpen() )
	{
		DataSaver	saver(file);
		saver & m_settings & m_lastInputDir;
	}

	QFile	layoutFile(LAYOUT_FILE_NAME);
	layoutFile.open(QIODevice::WriteOnly);
	if( layoutFile.isOpen() )
	{
		QByteArray geometryData(this->saveGeometry());
		layoutFile.write( geometryData );
	}
}

void MainWindow::ReadSettingsFromGUI( MeshConverter::Settings & settings )
{
	settings.inputFiles.SetNum(ui.listWidget_InputFiles->count());
	for( UINT iSrcFile = 0; iSrcFile < ui.listWidget_InputFiles->count(); iSrcFile++ )
	{
		QListWidgetItem* listItem = ui.listWidget_InputFiles->item(iSrcFile);
		settings.inputFiles[iSrcFile].SetString(listItem->text().toAscii().data());
	}

	settings.outputDir.SetString(ui.lineEdit_Output_Folder->text().toAscii().data());
	FS_FixSlashes(settings.outputDir);
}

void MainWindow::UpdateGuiFromSettings( const MeshConverter::Settings & settings )
{
	ui.listWidget_InputFiles->clear();
	for( UINT iSrcFile = 0; iSrcFile < m_settings.inputFiles.Num(); iSrcFile++ )
	{
		ui.listWidget_InputFiles->addItem(m_settings.inputFiles[iSrcFile].ToChars());
	}

	ui.lineEdit_Output_Folder->setText(m_settings.outputDir.ToChars());
}
