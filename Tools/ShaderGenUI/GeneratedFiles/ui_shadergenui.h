/********************************************************************************
** Form generated from reading UI file 'shadergenui.ui'
**
** Created: Tue 15. May 17:17:22 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHADERGENUI_H
#define UI_SHADERGENUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_ShaderGenUIClass
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Generate;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLineEdit *lineEdit_headerFileName;
    QLabel *label_5;
    QLineEdit *lineEdit_sourceFileName;
    QPushButton *pushButton_Select_Output_Dir_HLSL;
    QLineEdit *lineEdit_Output_Folder_HLSL;
    QPushButton *pushButton_Select_Output_Dir_Cpp;
    QLineEdit *lineEdit_Output_Folder_Cpp;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_Select_Input_Files;
    QCheckBox *checkBox_EmitPreamble;
    QCheckBox *checkBox_EmitComments;
    QCheckBox *checkBox_EmitHLSLComments;
    QCheckBox *checkBox_EmitCppVertexFormats;
    QCheckBox *checkBox_EmitCppMainHeader;
    QLabel *label_6;
    QLineEdit *lineEdit_cppNamespace;
    QLabel *label_2;
    QListWidget *listWidget_InputFiles;
    QLabel *label;
    QLabel *label_4;
    QPlainTextEdit *plainTextEdit_headerFilePrefix;
    QPlainTextEdit *plainTextEdit_sourceFilePrefix;

    void setupUi(QDialog *ShaderGenUIClass)
    {
        if (ShaderGenUIClass->objectName().isEmpty())
            ShaderGenUIClass->setObjectName(QString::fromUtf8("ShaderGenUIClass"));
        ShaderGenUIClass->resize(935, 642);
        gridLayout_2 = new QGridLayout(ShaderGenUIClass);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalSpacer = new QSpacerItem(327, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 2, 0, 1, 1);

        pushButton_Generate = new QPushButton(ShaderGenUIClass);
        pushButton_Generate->setObjectName(QString::fromUtf8("pushButton_Generate"));

        gridLayout_2->addWidget(pushButton_Generate, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(326, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        groupBox_2 = new QGroupBox(ShaderGenUIClass);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        lineEdit_headerFileName = new QLineEdit(groupBox_2);
        lineEdit_headerFileName->setObjectName(QString::fromUtf8("lineEdit_headerFileName"));

        gridLayout->addWidget(lineEdit_headerFileName, 0, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        lineEdit_sourceFileName = new QLineEdit(groupBox_2);
        lineEdit_sourceFileName->setObjectName(QString::fromUtf8("lineEdit_sourceFileName"));

        gridLayout->addWidget(lineEdit_sourceFileName, 1, 1, 1, 1);

        pushButton_Select_Output_Dir_HLSL = new QPushButton(groupBox_2);
        pushButton_Select_Output_Dir_HLSL->setObjectName(QString::fromUtf8("pushButton_Select_Output_Dir_HLSL"));

        gridLayout->addWidget(pushButton_Select_Output_Dir_HLSL, 2, 0, 1, 1);

        lineEdit_Output_Folder_HLSL = new QLineEdit(groupBox_2);
        lineEdit_Output_Folder_HLSL->setObjectName(QString::fromUtf8("lineEdit_Output_Folder_HLSL"));

        gridLayout->addWidget(lineEdit_Output_Folder_HLSL, 2, 1, 1, 2);

        pushButton_Select_Output_Dir_Cpp = new QPushButton(groupBox_2);
        pushButton_Select_Output_Dir_Cpp->setObjectName(QString::fromUtf8("pushButton_Select_Output_Dir_Cpp"));

        gridLayout->addWidget(pushButton_Select_Output_Dir_Cpp, 3, 0, 1, 1);

        lineEdit_Output_Folder_Cpp = new QLineEdit(groupBox_2);
        lineEdit_Output_Folder_Cpp->setObjectName(QString::fromUtf8("lineEdit_Output_Folder_Cpp"));

        gridLayout->addWidget(lineEdit_Output_Folder_Cpp, 3, 1, 1, 2);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 3);

        groupBox = new QGroupBox(ShaderGenUIClass);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        pushButton_Select_Input_Files = new QPushButton(groupBox);
        pushButton_Select_Input_Files->setObjectName(QString::fromUtf8("pushButton_Select_Input_Files"));

        gridLayout_3->addWidget(pushButton_Select_Input_Files, 0, 0, 1, 1);

        checkBox_EmitPreamble = new QCheckBox(groupBox);
        checkBox_EmitPreamble->setObjectName(QString::fromUtf8("checkBox_EmitPreamble"));
        checkBox_EmitPreamble->setChecked(true);

        gridLayout_3->addWidget(checkBox_EmitPreamble, 0, 1, 1, 1);

        checkBox_EmitComments = new QCheckBox(groupBox);
        checkBox_EmitComments->setObjectName(QString::fromUtf8("checkBox_EmitComments"));
        checkBox_EmitComments->setChecked(true);

        gridLayout_3->addWidget(checkBox_EmitComments, 0, 2, 1, 2);

        checkBox_EmitHLSLComments = new QCheckBox(groupBox);
        checkBox_EmitHLSLComments->setObjectName(QString::fromUtf8("checkBox_EmitHLSLComments"));
        checkBox_EmitHLSLComments->setChecked(true);

        gridLayout_3->addWidget(checkBox_EmitHLSLComments, 0, 4, 1, 1);

        checkBox_EmitCppVertexFormats = new QCheckBox(groupBox);
        checkBox_EmitCppVertexFormats->setObjectName(QString::fromUtf8("checkBox_EmitCppVertexFormats"));
        checkBox_EmitCppVertexFormats->setChecked(true);

        gridLayout_3->addWidget(checkBox_EmitCppVertexFormats, 0, 5, 1, 1);

        checkBox_EmitCppMainHeader = new QCheckBox(groupBox);
        checkBox_EmitCppMainHeader->setObjectName(QString::fromUtf8("checkBox_EmitCppMainHeader"));
        checkBox_EmitCppMainHeader->setChecked(true);

        gridLayout_3->addWidget(checkBox_EmitCppMainHeader, 0, 6, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 1, 1, 1, 2);

        lineEdit_cppNamespace = new QLineEdit(groupBox);
        lineEdit_cppNamespace->setObjectName(QString::fromUtf8("lineEdit_cppNamespace"));

        gridLayout_3->addWidget(lineEdit_cppNamespace, 1, 3, 1, 2);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 2, 0, 1, 1);

        listWidget_InputFiles = new QListWidget(groupBox);
        listWidget_InputFiles->setObjectName(QString::fromUtf8("listWidget_InputFiles"));

        gridLayout_3->addWidget(listWidget_InputFiles, 3, 0, 1, 7);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_3->addWidget(label, 4, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_3->addWidget(label_4, 4, 4, 1, 1);

        plainTextEdit_headerFilePrefix = new QPlainTextEdit(groupBox);
        plainTextEdit_headerFilePrefix->setObjectName(QString::fromUtf8("plainTextEdit_headerFilePrefix"));

        gridLayout_3->addWidget(plainTextEdit_headerFilePrefix, 5, 0, 1, 4);

        plainTextEdit_sourceFilePrefix = new QPlainTextEdit(groupBox);
        plainTextEdit_sourceFilePrefix->setObjectName(QString::fromUtf8("plainTextEdit_sourceFilePrefix"));

        gridLayout_3->addWidget(plainTextEdit_sourceFilePrefix, 5, 4, 1, 3);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 3);


        retranslateUi(ShaderGenUIClass);

        QMetaObject::connectSlotsByName(ShaderGenUIClass);
    } // setupUi

    void retranslateUi(QDialog *ShaderGenUIClass)
    {
        ShaderGenUIClass->setWindowTitle(QApplication::translate("ShaderGenUIClass", "ShaderGenUI", 0, QApplication::UnicodeUTF8));
        pushButton_Generate->setText(QApplication::translate("ShaderGenUIClass", "Generate", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ShaderGenUIClass", "Outputs", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ShaderGenUIClass", "Header file name:", 0, QApplication::UnicodeUTF8));
        lineEdit_headerFileName->setText(QApplication::translate("ShaderGenUIClass", "Main.hxx", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ShaderGenUIClass", "Source file name:", 0, QApplication::UnicodeUTF8));
        lineEdit_sourceFileName->setText(QApplication::translate("ShaderGenUIClass", "Main.cxx", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Output_Dir_HLSL->setText(QApplication::translate("ShaderGenUIClass", "Select Folder to generated shader files", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Output_Dir_Cpp->setText(QApplication::translate("ShaderGenUIClass", "Select Folder to generated C++ files", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ShaderGenUIClass", "Inputs", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Input_Files->setText(QApplication::translate("ShaderGenUIClass", "Select Files", 0, QApplication::UnicodeUTF8));
        checkBox_EmitPreamble->setText(QApplication::translate("ShaderGenUIClass", "Preamble", 0, QApplication::UnicodeUTF8));
        checkBox_EmitComments->setText(QApplication::translate("ShaderGenUIClass", "Comments", 0, QApplication::UnicodeUTF8));
        checkBox_EmitHLSLComments->setText(QApplication::translate("ShaderGenUIClass", " HLSL line number info", 0, QApplication::UnicodeUTF8));
        checkBox_EmitCppVertexFormats->setText(QApplication::translate("ShaderGenUIClass", " Emit vertex formats (C++)", 0, QApplication::UnicodeUTF8));
        checkBox_EmitCppMainHeader->setText(QApplication::translate("ShaderGenUIClass", " Emit main header (C++)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ShaderGenUIClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"right\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">C++ namespace:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lineEdit_cppNamespace->setText(QApplication::translate("ShaderGenUIClass", "GPU", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ShaderGenUIClass", "Input files:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ShaderGenUIClass", "Insert code in headers:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ShaderGenUIClass", "Insert code in source files:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ShaderGenUIClass: public Ui_ShaderGenUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHADERGENUI_H
