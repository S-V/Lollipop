/********************************************************************************
** Form generated from reading UI file 'shadereditor.ui'
**
** Created: Mon Dec 5 11:04:27 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHADEREDITOR_H
#define UI_SHADEREDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShaderEditorClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QMdiArea *mdiArea;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ShaderEditorClass)
    {
        if (ShaderEditorClass->objectName().isEmpty())
            ShaderEditorClass->setObjectName(QString::fromUtf8("ShaderEditorClass"));
        ShaderEditorClass->resize(671, 502);
        centralWidget = new QWidget(ShaderEditorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));

        gridLayout->addWidget(mdiArea, 0, 0, 1, 1);

        ShaderEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ShaderEditorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 671, 18));
        ShaderEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ShaderEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ShaderEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ShaderEditorClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ShaderEditorClass->setStatusBar(statusBar);

        retranslateUi(ShaderEditorClass);

        QMetaObject::connectSlotsByName(ShaderEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *ShaderEditorClass)
    {
        ShaderEditorClass->setWindowTitle(QApplication::translate("ShaderEditorClass", "ShaderEditor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ShaderEditorClass: public Ui_ShaderEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHADEREDITOR_H
