/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Dec 5 11:04:30 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_Select_Input_Files;
    QLabel *label_2;
    QListWidget *listWidget_InputFiles;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_Select_Output_Dir;
    QLineEdit *lineEdit_Output_Folder;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Execute;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(669, 547);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        pushButton_Select_Input_Files = new QPushButton(groupBox);
        pushButton_Select_Input_Files->setObjectName(QString::fromUtf8("pushButton_Select_Input_Files"));

        gridLayout_3->addWidget(pushButton_Select_Input_Files, 0, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 1, 0, 1, 1);

        listWidget_InputFiles = new QListWidget(groupBox);
        listWidget_InputFiles->setObjectName(QString::fromUtf8("listWidget_InputFiles"));

        gridLayout_3->addWidget(listWidget_InputFiles, 2, 0, 1, 2);


        gridLayout->addWidget(groupBox, 0, 0, 1, 3);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        pushButton_Select_Output_Dir = new QPushButton(groupBox_2);
        pushButton_Select_Output_Dir->setObjectName(QString::fromUtf8("pushButton_Select_Output_Dir"));

        gridLayout_2->addWidget(pushButton_Select_Output_Dir, 0, 0, 1, 1);

        lineEdit_Output_Folder = new QLineEdit(groupBox_2);
        lineEdit_Output_Folder->setObjectName(QString::fromUtf8("lineEdit_Output_Folder"));

        gridLayout_2->addWidget(lineEdit_Output_Folder, 0, 1, 1, 1);


        gridLayout->addWidget(groupBox_2, 1, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(279, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        pushButton_Execute = new QPushButton(centralWidget);
        pushButton_Execute->setObjectName(QString::fromUtf8("pushButton_Execute"));

        gridLayout->addWidget(pushButton_Execute, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(279, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 669, 18));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Mesh Converter", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindowClass", "Inputs", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Input_Files->setText(QApplication::translate("MainWindowClass", "Select Files", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindowClass", "Input files:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindowClass", "Outputs", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Output_Dir->setText(QApplication::translate("MainWindowClass", "Select Folder", 0, QApplication::UnicodeUTF8));
        pushButton_Execute->setText(QApplication::translate("MainWindowClass", "Execute", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
