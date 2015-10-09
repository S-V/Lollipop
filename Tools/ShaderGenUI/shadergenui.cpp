#include "stdafx.h"
#include "shadergenui.h"

#include <Core/Serialization.h>
#pragma comment( lib, "Core.lib" )

#include <../ShaderGen/ShaderGen.h>
#pragma comment( lib, "ShaderGen.lib" )

#include <../QtSupport/qt_common.h>

#define SETTINGS_FILE_NAME	"ShaderGen.settings"
#define LAYOUT_FILE_NAME	"ShaderGen.layout"


ShaderGenUI::ShaderGenUI(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);

	connect(
		ui.pushButton_Select_Input_Files,
		SIGNAL(clicked()),
		this,
		SLOT(SelectInputFiles())
	);

	connect(
		ui.pushButton_Select_Output_Dir_HLSL,
		SIGNAL(clicked()),
		this,
		SLOT(SelectOutputFolderHLSL())
	);
	connect(
		ui.pushButton_Select_Output_Dir_Cpp,
		SIGNAL(clicked()),
		this,
		SLOT(SelectOutputFolderCPP())
	);

	connect(
		ui.pushButton_Generate,
		SIGNAL(clicked()),
		this,
		SLOT(GenerateCode())
	);

	LoadSettings();
}

ShaderGenUI::~ShaderGenUI()
{
	SaveSettings();
}

void ShaderGenUI::SelectInputFiles()
{
	QFileDialog dialog(this);
#if 0
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Source files (*.fx *.shader *.program)"));
	dialog.setDirectory(m_lastSourceFolder.ToChars());

	if (dialog.exec())
	{
		m_options.srcFiles.Empty();
		foreach(QString fileName, dialog.selectedFiles())
		{
			OSPathName & newInputFile = m_options.srcFiles.Add();
			newInputFile.SetString(fileName.toAscii().data());
		}

		m_lastSourceFolder.SetString(dialog.directory().path().toAscii().data());

		ui.listWidget_InputFiles->clear();
		ui.listWidget_InputFiles->addItems(dialog.selectedFiles());
	}
#endif
	if( SelectFiles( dialog, "Source files (*.fx *.shader *.program)",
		m_lastSourceFolder, m_options.srcFiles ))
	{
		ui.listWidget_InputFiles->clear();
		ui.listWidget_InputFiles->addItems(dialog.selectedFiles());
	}
}
void ShaderGenUI::SelectOutputFolderHLSL()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setDirectory(m_options.outputFolderHLSL.ToChars());

	if (dialog.exec())
	{
		m_options.outputFolderHLSL.SetString(dialog.directory().path().toAscii().data());

		ui.lineEdit_Output_Folder_HLSL->setText(m_options.outputFolderHLSL.ToChars());
	}
}
void ShaderGenUI::SelectOutputFolderCPP()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setDirectory(m_options.outputFolderCPP.ToChars());

	if (dialog.exec())
	{
		m_options.outputFolderCPP.SetString(dialog.directory().path().toAscii().data());
 
		ui.lineEdit_Output_Folder_Cpp->setText(m_options.outputFolderCPP.ToChars());
	}
}
void ShaderGenUI::GenerateCode()
{
	ReadOptionsFromGUI( m_options );

	if( ui.listWidget_InputFiles->count() == 0 )
	{
		QMessageBox::warning(this,"Error",
			"Select one or more input source files");
		return;
	}
	if( ui.lineEdit_cppNamespace->text().isEmpty() )
	{
		QMessageBox::warning(this,"Error",
			"Specify a valid C++ namespace");
		return;
	}
	if( ui.lineEdit_headerFileName->text().isEmpty() )
	{
		QMessageBox::warning(this,"Error",
			"Specify a valid C++ header file name");
		return;
	}
	if( ui.lineEdit_sourceFileName->text().isEmpty() )
	{
		QMessageBox::warning(this,"Error",
			"Specify a valid C++ source file name");
		return;
	}
	if( ui.lineEdit_Output_Folder_Cpp->text().isEmpty() )
	{
		QMessageBox::warning(this,"Error",
			"Enter the path where generated C++ files will be stored");
		return;
	}
	if( ui.lineEdit_Output_Folder_HLSL->text().isEmpty() )
	{
		QMessageBox::warning(this,"Error",
			"Enter the path where generated shader files will be stored");
		return;
	}

	ShaderGen::Generator* pShaderGen = ShaderGen::Generator::Create(m_options);
	pShaderGen->GenerateShaderLib();
	ShaderGen::Generator::Destroy(pShaderGen);
}
void ShaderGenUI::LoadSettings()
{
	FileReader	file(SETTINGS_FILE_NAME,FileRead_NoErrors);
	if( file.IsOpen() )
	{
		DataReader	loader(file);
		loader & m_options & m_lastSourceFolder;
	}

	QFile	layoutFile(LAYOUT_FILE_NAME);
	layoutFile.open(QIODevice::ReadOnly);
	if( layoutFile.isOpen() )
	{
		QByteArray	geometryData(layoutFile.readAll());
		this->restoreGeometry( geometryData );
	}

	ui.checkBox_EmitCppVertexFormats->setChecked(m_options.bEmitVertexFormatsDecl);
	ui.checkBox_EmitCppMainHeader->setChecked(m_options.bEmitMainHeader);

	ui.checkBox_EmitPreamble->setChecked(m_options.bEmitPreamble);
	ui.checkBox_EmitComments->setChecked(m_options.bEmitComments);
	ui.checkBox_EmitHLSLComments->setChecked(m_options.bEmitHLSLComments);

	ui.lineEdit_cppNamespace->setText(m_options.cppNamespace.ToChars());

	ui.listWidget_InputFiles->clear();
	for( UINT iSrcFile = 0; iSrcFile < m_options.srcFiles.Num(); iSrcFile++ )
	{
		ui.listWidget_InputFiles->addItem(m_options.srcFiles[iSrcFile].ToChars());
	}

	ui.plainTextEdit_headerFilePrefix->setPlainText(m_options.headerFilePrefix.ToChars());
	ui.plainTextEdit_sourceFilePrefix->setPlainText(m_options.sourceFilePrefix.ToChars());

	ui.lineEdit_headerFileName->setText(m_options.headerFileName.ToChars());
	ui.lineEdit_sourceFileName->setText(m_options.sourceFileName.ToChars());

	ui.lineEdit_Output_Folder_HLSL->setText(m_options.outputFolderHLSL.ToChars());
	ui.lineEdit_Output_Folder_Cpp->setText(m_options.outputFolderCPP.ToChars());
}

void ShaderGenUI::SaveSettings()
{
	ReadOptionsFromGUI( m_options );

	FileWriter	file(SETTINGS_FILE_NAME,FileWrite_NoErrors);
	if( file.IsOpen() )
	{
		DataWriter	saver(file);
		saver & m_options & m_lastSourceFolder;
	}

	QFile	layoutFile(LAYOUT_FILE_NAME);
	layoutFile.open(QIODevice::WriteOnly);
	if( layoutFile.isOpen() )
	{
		QByteArray geometryData(this->saveGeometry());
		layoutFile.write( geometryData );
	}
}
void ShaderGenUI::ReadOptionsFromGUI( ShaderGen::Options & options )
{
	options.bDebugMode = true;

	options.bEmitVertexFormatsDecl = ui.checkBox_EmitCppVertexFormats->isChecked();
	options.bEmitMainHeader = ui.checkBox_EmitCppMainHeader->isChecked();

	options.bEmitPreamble = ui.checkBox_EmitPreamble->isChecked();
	options.bEmitComments = ui.checkBox_EmitComments->isChecked();
	options.bEmitHLSLComments = ui.checkBox_EmitHLSLComments->isChecked();

	m_options.cppNamespace.SetString(ui.lineEdit_cppNamespace->text().toAscii().data());

	m_options.srcFiles.SetNum(ui.listWidget_InputFiles->count());
	for( UINT iSrcFile = 0; iSrcFile < ui.listWidget_InputFiles->count(); iSrcFile++ )
	{
		QListWidgetItem* listItem = ui.listWidget_InputFiles->item(iSrcFile);
		m_options.srcFiles[iSrcFile].SetString(listItem->text().toAscii().data());
	}

	options.headerFilePrefix.SetString(ui.plainTextEdit_headerFilePrefix->toPlainText().toAscii().data());
	options.sourceFilePrefix.SetString(ui.plainTextEdit_sourceFilePrefix->toPlainText().toAscii().data());


	options.headerFileName.SetString(ui.lineEdit_headerFileName->text().toAscii().data());
	options.sourceFileName.SetString(ui.lineEdit_sourceFileName->text().toAscii().data());


	options.outputFolderHLSL.SetString(ui.lineEdit_Output_Folder_HLSL->text().toAscii().data());
	options.outputFolderCPP.SetString(ui.lineEdit_Output_Folder_Cpp->text().toAscii().data());
}
