/********************************************************************************
** Form generated from reading UI file 'create_asset_db_dialog.ui'
**
** Created: Tue 15. May 17:18:36 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATE_ASSET_DB_DIALOG_H
#define UI_CREATE_ASSET_DB_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Create_Asset_Db_Dialog
{
public:
    QPushButton *pushButton_OK;
    QPushButton *pushButton_Cancel;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QPushButton *pushButton_SelectPathToSrcAssets;
    QLineEdit *lineEdit_pathToSrcAssets;
    QPushButton *pushButton_SelectPathToIntAssets;
    QLineEdit *lineEdit_pathToIntAssets;
    QPushButton *pushButton_SelectPathToBinAssets;
    QLineEdit *lineEdit_pathToBinAssets;

    void setupUi(QDialog *Create_Asset_Db_Dialog)
    {
        if (Create_Asset_Db_Dialog->objectName().isEmpty())
            Create_Asset_Db_Dialog->setObjectName(QString::fromUtf8("Create_Asset_Db_Dialog"));
        Create_Asset_Db_Dialog->setWindowModality(Qt::WindowModal);
        Create_Asset_Db_Dialog->resize(740, 199);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Create_Asset_Db_Dialog->sizePolicy().hasHeightForWidth());
        Create_Asset_Db_Dialog->setSizePolicy(sizePolicy);
        Create_Asset_Db_Dialog->setMinimumSize(QSize(740, 199));
        Create_Asset_Db_Dialog->setMaximumSize(QSize(740, 199));
        Create_Asset_Db_Dialog->setModal(true);
        pushButton_OK = new QPushButton(Create_Asset_Db_Dialog);
        pushButton_OK->setObjectName(QString::fromUtf8("pushButton_OK"));
        pushButton_OK->setGeometry(QRect(380, 160, 151, 23));
        pushButton_Cancel = new QPushButton(Create_Asset_Db_Dialog);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(560, 160, 161, 23));
        groupBox = new QGroupBox(Create_Asset_Db_Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 20, 701, 122));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_SelectPathToSrcAssets = new QPushButton(groupBox);
        pushButton_SelectPathToSrcAssets->setObjectName(QString::fromUtf8("pushButton_SelectPathToSrcAssets"));

        gridLayout->addWidget(pushButton_SelectPathToSrcAssets, 0, 0, 1, 1);

        lineEdit_pathToSrcAssets = new QLineEdit(groupBox);
        lineEdit_pathToSrcAssets->setObjectName(QString::fromUtf8("lineEdit_pathToSrcAssets"));

        gridLayout->addWidget(lineEdit_pathToSrcAssets, 0, 1, 1, 1);

        pushButton_SelectPathToIntAssets = new QPushButton(groupBox);
        pushButton_SelectPathToIntAssets->setObjectName(QString::fromUtf8("pushButton_SelectPathToIntAssets"));

        gridLayout->addWidget(pushButton_SelectPathToIntAssets, 1, 0, 1, 1);

        lineEdit_pathToIntAssets = new QLineEdit(groupBox);
        lineEdit_pathToIntAssets->setObjectName(QString::fromUtf8("lineEdit_pathToIntAssets"));

        gridLayout->addWidget(lineEdit_pathToIntAssets, 1, 1, 1, 1);

        pushButton_SelectPathToBinAssets = new QPushButton(groupBox);
        pushButton_SelectPathToBinAssets->setObjectName(QString::fromUtf8("pushButton_SelectPathToBinAssets"));

        gridLayout->addWidget(pushButton_SelectPathToBinAssets, 2, 0, 1, 1);

        lineEdit_pathToBinAssets = new QLineEdit(groupBox);
        lineEdit_pathToBinAssets->setObjectName(QString::fromUtf8("lineEdit_pathToBinAssets"));

        gridLayout->addWidget(lineEdit_pathToBinAssets, 2, 1, 1, 1);


        retranslateUi(Create_Asset_Db_Dialog);

        QMetaObject::connectSlotsByName(Create_Asset_Db_Dialog);
    } // setupUi

    void retranslateUi(QDialog *Create_Asset_Db_Dialog)
    {
        Create_Asset_Db_Dialog->setWindowTitle(QApplication::translate("Create_Asset_Db_Dialog", "Create_Asset_Db_Dialog", 0, QApplication::UnicodeUTF8));
        pushButton_OK->setText(QApplication::translate("Create_Asset_Db_Dialog", "OK", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("Create_Asset_Db_Dialog", "Cancel", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Create_Asset_Db_Dialog", "Asset Database Parameters", 0, QApplication::UnicodeUTF8));
        pushButton_SelectPathToSrcAssets->setText(QApplication::translate("Create_Asset_Db_Dialog", "Select Path to Source Assets", 0, QApplication::UnicodeUTF8));
        pushButton_SelectPathToIntAssets->setText(QApplication::translate("Create_Asset_Db_Dialog", "Select Path to Intermediate Assets", 0, QApplication::UnicodeUTF8));
        pushButton_SelectPathToBinAssets->setText(QApplication::translate("Create_Asset_Db_Dialog", "Select Path to Compiled Assets", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Create_Asset_Db_Dialog: public Ui_Create_Asset_Db_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATE_ASSET_DB_DIALOG_H
