/********************************************************************************
** Form generated from reading UI file 'contentmanager.ui'
**
** Created: Tue 15. May 17:18:35 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTENTMANAGER_H
#define UI_CONTENTMANAGER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ContentManagerClass
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionNew;
    QAction *actionRebuild;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuProject;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ContentManagerClass)
    {
        if (ContentManagerClass->objectName().isEmpty())
            ContentManagerClass->setObjectName(QString::fromUtf8("ContentManagerClass"));
        ContentManagerClass->resize(656, 430);
        actionOpen = new QAction(ContentManagerClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(ContentManagerClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionNew = new QAction(ContentManagerClass);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionRebuild = new QAction(ContentManagerClass);
        actionRebuild->setObjectName(QString::fromUtf8("actionRebuild"));
        centralWidget = new QWidget(ContentManagerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ContentManagerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ContentManagerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 656, 18));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuProject = new QMenu(menuBar);
        menuProject->setObjectName(QString::fromUtf8("menuProject"));
        ContentManagerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ContentManagerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ContentManagerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ContentManagerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ContentManagerClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuProject->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionRebuild);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(ContentManagerClass);

        QMetaObject::connectSlotsByName(ContentManagerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ContentManagerClass)
    {
        ContentManagerClass->setWindowTitle(QApplication::translate("ContentManagerClass", "ContentManager", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("ContentManagerClass", "&Open", 0, QApplication::UnicodeUTF8));
        actionOpen->setShortcut(QApplication::translate("ContentManagerClass", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("ContentManagerClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("ContentManagerClass", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        actionNew->setText(QApplication::translate("ContentManagerClass", "&New", 0, QApplication::UnicodeUTF8));
        actionNew->setShortcut(QApplication::translate("ContentManagerClass", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actionRebuild->setText(QApplication::translate("ContentManagerClass", "Rebuild", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionRebuild->setToolTip(QApplication::translate("ContentManagerClass", "Rebuild all souce art", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionRebuild->setShortcut(QApplication::translate("ContentManagerClass", "Ctrl+Alt+F7", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("ContentManagerClass", "&File", 0, QApplication::UnicodeUTF8));
        menuProject->setTitle(QApplication::translate("ContentManagerClass", "Project", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ContentManagerClass: public Ui_ContentManagerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTENTMANAGER_H
