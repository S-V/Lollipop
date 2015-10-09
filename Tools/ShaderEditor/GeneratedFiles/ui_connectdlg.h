/********************************************************************************
** Form generated from reading UI file 'connectdlg.ui'
**
** Created: Mon Dec 5 11:04:28 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTDLG_H
#define UI_CONNECTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ConnectDlg
{
public:
    QPushButton *pushButton_OK;
    QPushButton *pushButton_Cancel;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEdit_Host;
    QSpinBox *spinBox_Port;

    void setupUi(QDialog *ConnectDlg)
    {
        if (ConnectDlg->objectName().isEmpty())
            ConnectDlg->setObjectName(QString::fromUtf8("ConnectDlg"));
        ConnectDlg->resize(306, 154);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ConnectDlg->sizePolicy().hasHeightForWidth());
        ConnectDlg->setSizePolicy(sizePolicy);
        pushButton_OK = new QPushButton(ConnectDlg);
        pushButton_OK->setObjectName(QString::fromUtf8("pushButton_OK"));
        pushButton_OK->setGeometry(QRect(60, 110, 81, 31));
        pushButton_Cancel = new QPushButton(ConnectDlg);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(160, 110, 81, 31));
        label = new QLabel(ConnectDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 51, 16));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label_2 = new QLabel(ConnectDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 51, 16));
        label_2->setFont(font);
        lineEdit_Host = new QLineEdit(ConnectDlg);
        lineEdit_Host->setObjectName(QString::fromUtf8("lineEdit_Host"));
        lineEdit_Host->setGeometry(QRect(80, 30, 191, 20));
        spinBox_Port = new QSpinBox(ConnectDlg);
        spinBox_Port->setObjectName(QString::fromUtf8("spinBox_Port"));
        spinBox_Port->setGeometry(QRect(80, 60, 91, 22));
        spinBox_Port->setMaximum(65535);

        retranslateUi(ConnectDlg);

        QMetaObject::connectSlotsByName(ConnectDlg);
    } // setupUi

    void retranslateUi(QDialog *ConnectDlg)
    {
        ConnectDlg->setWindowTitle(QApplication::translate("ConnectDlg", "ConnectDlg", 0, QApplication::UnicodeUTF8));
        pushButton_OK->setText(QApplication::translate("ConnectDlg", "OK", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("ConnectDlg", "Cancel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ConnectDlg", "Host:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ConnectDlg", "Port:", 0, QApplication::UnicodeUTF8));
        lineEdit_Host->setText(QApplication::translate("ConnectDlg", "localhost", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConnectDlg: public Ui_ConnectDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTDLG_H
