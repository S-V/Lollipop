/****************************************************************************
** Meta object code from reading C++ file 'connectdlg.h'
**
** Created: Mon Dec 5 11:04:26 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\connectdlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectdlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConnectDlg[] = {

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
      12,   11,   11,   11, 0x0a,
      21,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ConnectDlg[] = {
    "ConnectDlg\0\0accept()\0decline()\0"
};

const QMetaObject ConnectDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConnectDlg,
      qt_meta_data_ConnectDlg, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConnectDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConnectDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConnectDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectDlg))
        return static_cast<void*>(const_cast< ConnectDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConnectDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: decline(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
