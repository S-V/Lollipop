/********************************************************************************
** Form generated from reading UI file 'addnewitemdialog.ui'
**
** Created: Tue 17. Apr 18:01:19 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDNEWITEMDIALOG_H
#define UI_ADDNEWITEMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_AddNewItemDialog
{
public:
    QGridLayout *gridLayout;
    QTreeView *treeView;
    QHBoxLayout *_2;
    QSpacerItem *spacerItem;
    QPushButton *addButton;
    QPushButton *cancelButton_2;
    QListView *listView;

    void setupUi(QDialog *AddNewItemDialog)
    {
        if (AddNewItemDialog->objectName().isEmpty())
            AddNewItemDialog->setObjectName(QString::fromUtf8("AddNewItemDialog"));
        AddNewItemDialog->resize(788, 464);
        AddNewItemDialog->setSizeGripEnabled(true);
        gridLayout = new QGridLayout(AddNewItemDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        treeView = new QTreeView(AddNewItemDialog);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        gridLayout->addWidget(treeView, 0, 0, 1, 1);

        _2 = new QHBoxLayout();
        _2->setSpacing(6);
        _2->setContentsMargins(0, 0, 0, 0);
        _2->setObjectName(QString::fromUtf8("_2"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        _2->addItem(spacerItem);

        addButton = new QPushButton(AddNewItemDialog);
        addButton->setObjectName(QString::fromUtf8("addButton"));

        _2->addWidget(addButton);

        cancelButton_2 = new QPushButton(AddNewItemDialog);
        cancelButton_2->setObjectName(QString::fromUtf8("cancelButton_2"));

        _2->addWidget(cancelButton_2);


        gridLayout->addLayout(_2, 1, 1, 1, 1);

        listView = new QListView(AddNewItemDialog);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setAlternatingRowColors(true);

        gridLayout->addWidget(listView, 0, 1, 1, 1);


        retranslateUi(AddNewItemDialog);

        QMetaObject::connectSlotsByName(AddNewItemDialog);
    } // setupUi

    void retranslateUi(QDialog *AddNewItemDialog)
    {
        AddNewItemDialog->setWindowTitle(QApplication::translate("AddNewItemDialog", "AddNewItemDialog", 0, QApplication::UnicodeUTF8));
        addButton->setText(QApplication::translate("AddNewItemDialog", "Add", 0, QApplication::UnicodeUTF8));
        cancelButton_2->setText(QApplication::translate("AddNewItemDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddNewItemDialog: public Ui_AddNewItemDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDNEWITEMDIALOG_H
