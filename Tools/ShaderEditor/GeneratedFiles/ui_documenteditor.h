/********************************************************************************
** Form generated from reading UI file 'documenteditor.ui'
**
** Created: Fri 15. Jul 23:06:58 2011
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOCUMENTEDITOR_H
#define UI_DOCUMENTEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_DocumentEditor
{
public:

    void setupUi(QTextEdit *DocumentEditor)
    {
        if (DocumentEditor->objectName().isEmpty())
            DocumentEditor->setObjectName(QString::fromUtf8("DocumentEditor"));
        DocumentEditor->resize(400, 300);

        retranslateUi(DocumentEditor);

        QMetaObject::connectSlotsByName(DocumentEditor);
    } // setupUi

    void retranslateUi(QTextEdit *DocumentEditor)
    {
        DocumentEditor->setWindowTitle(QApplication::translate("DocumentEditor", "DocumentEditor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DocumentEditor: public Ui_DocumentEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOCUMENTEDITOR_H
