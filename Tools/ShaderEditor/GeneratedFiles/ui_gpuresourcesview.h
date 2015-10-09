/********************************************************************************
** Form generated from reading UI file 'gpuresourcesview.ui'
**
** Created: Mon Dec 5 11:04:27 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GPURESOURCESVIEW_H
#define UI_GPURESOURCESVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GpuResourcesView
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;

    void setupUi(QDockWidget *GpuResourcesView)
    {
        if (GpuResourcesView->objectName().isEmpty())
            GpuResourcesView->setObjectName(QString::fromUtf8("GpuResourcesView"));
        GpuResourcesView->resize(484, 630);
        widget = new QWidget();
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout = new QGridLayout(widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        GpuResourcesView->setWidget(widget);

        retranslateUi(GpuResourcesView);

        QMetaObject::connectSlotsByName(GpuResourcesView);
    } // setupUi

    void retranslateUi(QDockWidget *GpuResourcesView)
    {
        GpuResourcesView->setWindowTitle(QApplication::translate("GpuResourcesView", "GPU Resources", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GpuResourcesView: public Ui_GpuResourcesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPURESOURCESVIEW_H
