/****************************************************************************
** Meta object code from reading C++ file 'consolepanel.h'
**
** Created: Mon Dec 5 11:04:25 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\consolepanel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'consolepanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConsolePanel[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      24,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ConsolePanel[] = {
    "ConsolePanel\0\0onEnter()\0onCommand(QString)\0"
};

const QMetaObject ConsolePanel::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_ConsolePanel,
      qt_meta_data_ConsolePanel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConsolePanel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConsolePanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConsolePanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConsolePanel))
        return static_cast<void*>(const_cast< ConsolePanel*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int ConsolePanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onEnter(); break;
        case 1: onCommand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
