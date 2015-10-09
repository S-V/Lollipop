#ifndef SHADERGENUI_H
#define SHADERGENUI_H

#include <QtGui/QDialog>
#include "ui_shadergenui.h"

#include <../ShaderGen/ShaderGen.h>

class ShaderGenUI : public QDialog
{
	Q_OBJECT

public:
	ShaderGenUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ShaderGenUI();

public slots:
	void SelectInputFiles();
	void SelectOutputFolderHLSL();
	void SelectOutputFolderCPP();
	void GenerateCode();

private:
	void LoadSettings();
	void SaveSettings();

	void ReadOptionsFromGUI( ShaderGen::Options & options );

private:
	Ui::ShaderGenUIClass ui;

	SetupBaseUtil	m_setupBase;
	ShaderGen::Options	m_options;
	OSPathName	m_lastSourceFolder;
};

#endif // SHADERGENUI_H
