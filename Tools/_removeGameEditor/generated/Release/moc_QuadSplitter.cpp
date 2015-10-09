/****************************************************************************
** Meta object code from reading C++ file 'QuadSplitter.h'
**
** Created: Tue Apr 10 18:55:02 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\QuadSplitter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QuadSplitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdQuadSplitter[] = {

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
      16,   15,   15,   15, 0x08,
      24,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdQuadSplitter[] = {
    "EdQuadSplitter\0\0sync1()\0sync2()\0"
};

const QMetaObject EdQuadSplitter::staticMetaObject = {
    { &QSplitter::staticMetaObject, qt_meta_stringdata_EdQuadSplitter,
      qt_meta_data_EdQuadSplitter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdQuadSplitter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdQuadSplitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdQuadSplitter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdQuadSplitter))
        return static_cast<void*>(const_cast< EdQuadSplitter*>(this));
    return QSplitter::qt_metacast(_clname);
}

int EdQuadSplitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSplitter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sync1(); break;
        case 1: sync2(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
