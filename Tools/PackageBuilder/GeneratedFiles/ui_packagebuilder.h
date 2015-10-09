/********************************************************************************
** Form generated from reading UI file 'packagebuilder.ui'
**
** Created: Mon Feb 20 19:06:31 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PACKAGEBUILDER_H
#define UI_PACKAGEBUILDER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PackageBuilderClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_InputFolder;
    QLabel *label_2;
    QLineEdit *lineEdit_OutputFolder;
    QPushButton *pushButton_BuildArchive;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PackageBuilderClass)
    {
        if (PackageBuilderClass->objectName().isEmpty())
            PackageBuilderClass->setObjectName(QString::fromUtf8("PackageBuilderClass"));
        PackageBuilderClass->resize(768, 203);
        centralWidget = new QWidget(PackageBuilderClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_InputFolder = new QLineEdit(centralWidget);
        lineEdit_InputFolder->setObjectName(QString::fromUtf8("lineEdit_InputFolder"));

        gridLayout->addWidget(lineEdit_InputFolder, 0, 1, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_OutputFolder = new QLineEdit(centralWidget);
        lineEdit_OutputFolder->setObjectName(QString::fromUtf8("lineEdit_OutputFolder"));

        gridLayout->addWidget(lineEdit_OutputFolder, 1, 1, 1, 1);

        pushButton_BuildArchive = new QPushButton(centralWidget);
        pushButton_BuildArchive->setObjectName(QString::fromUtf8("pushButton_BuildArchive"));

        gridLayout->addWidget(pushButton_BuildArchive, 2, 1, 1, 1);

        PackageBuilderClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PackageBuilderClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 768, 18));
        PackageBuilderClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PackageBuilderClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        PackageBuilderClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PackageBuilderClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        PackageBuilderClass->setStatusBar(statusBar);

        retranslateUi(PackageBuilderClass);

        QMetaObject::connectSlotsByName(PackageBuilderClass);
    } // setupUi

    void retranslateUi(QMainWindow *PackageBuilderClass)
    {
        PackageBuilderClass->setWindowTitle(QApplication::translate("PackageBuilderClass", "PackageBuilder", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PackageBuilderClass", "Input folder:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PackageBuilderClass", "Output folder:", 0, QApplication::UnicodeUTF8));
        pushButton_BuildArchive->setText(QApplication::translate("PackageBuilderClass", "Build Archive", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PackageBuilderClass: public Ui_PackageBuilderClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PACKAGEBUILDER_H
